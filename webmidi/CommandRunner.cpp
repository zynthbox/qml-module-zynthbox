// SPDX-FileCopyrightText: 2026 Camilo Higuita <milo.h@aol.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "CommandRunner.h"
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <csignal>

CommandRunner::CommandRunner(QObject *parent)
    : QObject(parent)
{
}

void CommandRunner::setCommand(const QString &cmd)
{
    if (m_command == cmd) return;
    m_command = cmd;
    Q_EMIT commandChanged();
}

void CommandRunner::setArguments(const QStringList &args)
{
    if (m_arguments == args) return;
    m_arguments = args;
    Q_EMIT argumentsChanged();
}

void CommandRunner::setWorkingDirectory(const QString &dir)
{
    if (m_workingDirectory == dir) return;
    m_workingDirectory = dir;
    Q_EMIT workingDirectoryChanged();
}

void CommandRunner::setEnvironment(const QStringList &env)
{
    if (m_environment == env) return;
    m_environment = env;
    Q_EMIT environmentChanged();
}

void CommandRunner::setSuccessPattern(const QString &pattern)
{
    if (m_successPattern == pattern) return;
    m_successPattern = pattern;
    Q_EMIT successPatternChanged();
}

void CommandRunner::run()
{
    if (m_running || m_command.isEmpty()) return;

    m_output.clear();
    m_success = false;
    Q_EMIT outputChanged();
    Q_EMIT successChanged();

    m_process = new QProcess(this);
    m_process->setProcessChannelMode(QProcess::MergedChannels);
    if (!m_workingDirectory.isEmpty())
        m_process->setWorkingDirectory(m_workingDirectory);
    if (!m_environment.isEmpty()) {
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        for (const QString &entry : m_environment) {
            const int eq = entry.indexOf('=');
            if (eq > 0)
                env.insert(entry.left(eq), entry.mid(eq + 1));
        }
        m_process->setProcessEnvironment(env);
    }

    connect(m_process, &QProcess::readyRead, this, &CommandRunner::onReadyRead);
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &CommandRunner::onProcessFinished);

    m_running = true;
    Q_EMIT runningChanged();

    m_process->start(m_command, m_arguments);

    if (!m_process->waitForStarted(3000)) {
        qWarning() << "[CommandRunner] Failed to start:" << m_command << m_arguments;
        m_process->deleteLater();
        m_process = nullptr;
        m_running = false;
        Q_EMIT runningChanged();
        Q_EMIT stopped(-1);
        return;
    }
}

void CommandRunner::stop()
{
    if (!m_process) return;
    m_process->terminate();
    if (!m_process->waitForFinished(2000))
        m_process->kill();
}

void CommandRunner::onReadyRead()
{
    const QString chunk = QString::fromUtf8(m_process->readAll());
    if (chunk.isEmpty()) return;

    m_output += chunk;
    Q_EMIT outputChanged();

    // Check pattern on each new chunk; only fire once per run.
    if (!m_success && !m_successPattern.isEmpty()) {
        qDebug() << "[CommandRunner]" << m_command << "- checking pattern:" << m_successPattern;
        qDebug() << "[CommandRunner] output chunk:" << chunk.trimmed();
        const QRegularExpressionMatch match = QRegularExpression(m_successPattern).match(m_output);
        qDebug() << "[CommandRunner] pattern match result:" << match.hasMatch();
        if (match.hasMatch()) {
            m_success = true;
            Q_EMIT successChanged();
            Q_EMIT started();
        }
    }
}

void CommandRunner::onProcessFinished(int exitCode, QProcess::ExitStatus)
{
    // Drain any remaining output
    const QString remaining = QString::fromUtf8(m_process->readAll());
    if (!remaining.isEmpty()) {
        m_output += remaining;
        Q_EMIT outputChanged();
    }

    m_process->deleteLater();
    m_process = nullptr;

    // For short-lived commands the pattern may not have been checked yet via
    // readyRead — check once more against the full accumulated output.
    if (!m_success) {
        bool ok = false;
        if (!m_successPattern.isEmpty()) {
            qDebug() << "[CommandRunner]" << m_command << "- final pattern check:" << m_successPattern;
            qDebug() << "[CommandRunner] full output:" << m_output.trimmed();
            const QRegularExpressionMatch match = QRegularExpression(m_successPattern).match(m_output);
            qDebug() << "[CommandRunner] pattern match result:" << match.hasMatch();
            ok = match.hasMatch();
        } else {
            // No pattern: rely purely on exit code
            ok = (exitCode == 0);
            qDebug() << "[CommandRunner]" << m_command << "- no pattern, exit code:" << exitCode << "success:" << ok;
        }
        if (ok) {
            m_success = true;
            Q_EMIT successChanged();
            Q_EMIT started();
        }
    }

    m_running = false;
    Q_EMIT runningChanged();

    Q_EMIT stopped(exitCode);
}

// Reads /proc/net/tcp and /proc/net/tcp6 to find PIDs listening on `port`,
// then sends SIGTERM (falling back to SIGKILL) to each.
bool CommandRunner::killPort(int port)
{
    // The local address column in /proc/net/tcp* encodes port as hex after ':'.
    const QString hexPort = QString::number(port, 16).toUpper().rightJustified(4, '0');

    // Collect inodes listening on the port from tcp and tcp6
    QSet<quint64> inodes;
    for (const QString &procFile : {QStringLiteral("/proc/net/tcp"), QStringLiteral("/proc/net/tcp6")}) {
        QFile f(procFile);
        if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) continue;
        QTextStream in(&f);
        in.readLine(); // skip header
        while (!in.atEnd()) {
            const QString line = in.readLine().trimmed();
            const QStringList parts = line.split(' ', Qt::SkipEmptyParts);
            // parts[1] = local_address, parts[3] = state (0A = LISTEN), parts[9] = inode
            if (parts.size() < 10) continue;
            if (!parts[1].endsWith(':' + hexPort)) continue;
            if (parts[3] != QLatin1String("0A")) continue; // 0A = TCP_LISTEN
            inodes.insert(parts[9].toULongLong());
        }
    }

    if (inodes.isEmpty()) {
        qDebug() << "[killPort] no listening process found on port" << port;
        return false;
    }

    // Walk /proc/<pid>/fd to match socket inodes to PIDs
    bool killed = false;
    for (const QString &pidStr : QDir(QStringLiteral("/proc")).entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        bool ok;
        const pid_t pid = pidStr.toInt(&ok);
        if (!ok) continue;

        const QDir fdDir(QStringLiteral("/proc/") + pidStr + QStringLiteral("/fd"));
        for (const QString &fdEntry : fdDir.entryList(QDir::Files | QDir::System)) {
            const QString link = fdDir.filePath(fdEntry);
            const QString target = QFile::symLinkTarget(link);
            // target looks like "socket:[1234567]"
            if (!target.startsWith(QLatin1String("socket:["))) continue;
            const quint64 inode = target.mid(8, target.length() - 9).toULongLong();
            if (!inodes.contains(inode)) continue;

            qDebug() << "[killPort] sending SIGTERM to pid" << pid << "on port" << port;
            if (::kill(pid, SIGTERM) != 0) {
                qDebug() << "[killPort] SIGTERM failed, trying SIGKILL";
                ::kill(pid, SIGKILL);
            }
            killed = true;
            break;
        }
    }

    qDebug() << "[killPort] killed:" << killed;
    return killed;
}
