// SPDX-FileCopyrightText: 2026 Camilo Higuita <milo.h@aol.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QObject>
#include <QProcess>
#include <QRegularExpression>
#include <QString>
#include <QStringList>

/**
 * CommandRunner
 *
 * Wraps QProcess for long-running commands (e.g. "pnpm dev").
 * The process is kept alive until stop() is called or it exits on its own.
 *
 * Success is determined by watching stdout+stderr in real-time:
 * as soon as successPattern matches any accumulated output the
 * `success` property becomes true and `started` is emitted —
 * without waiting for the process to exit.
 *
 * `stopped` is emitted when the process actually exits (normally or via stop()).
 *
 * QML usage:
 *   CommandRunner {
 *       id: runner
 *       command: "pnpm"
 *       arguments: ["dev"]
 *       successPattern: "Local:.*http://localhost"   // regex matched on output
 *       onStarted: console.log("server is up!")
 *       onStopped: function(exitCode) { console.log("exited:", exitCode) }
 *   }
 *   runner.run()
 *   runner.stop()
 */
class CommandRunner : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString     command          READ command          WRITE setCommand          NOTIFY commandChanged)
    Q_PROPERTY(QStringList arguments        READ arguments        WRITE setArguments        NOTIFY argumentsChanged)
    Q_PROPERTY(QString     workingDirectory READ workingDirectory WRITE setWorkingDirectory NOTIFY workingDirectoryChanged)
    Q_PROPERTY(QString     successPattern   READ successPattern   WRITE setSuccessPattern   NOTIFY successPatternChanged)
    Q_PROPERTY(bool        running        READ running        NOTIFY runningChanged)
    Q_PROPERTY(bool        success        READ success        NOTIFY successChanged)
    Q_PROPERTY(QString     output         READ output         NOTIFY outputChanged)

public:
    explicit CommandRunner(QObject *parent = nullptr);

    QString     command()          const { return m_command; }
    QStringList arguments()        const { return m_arguments; }
    QString     workingDirectory() const { return m_workingDirectory; }
    QString     successPattern()   const { return m_successPattern; }
    bool        running()        const { return m_running; }
    bool        success()        const { return m_success; }
    QString     output()         const { return m_output; }

    void setCommand(const QString &cmd);
    void setArguments(const QStringList &args);
    void setWorkingDirectory(const QString &dir);
    void setSuccessPattern(const QString &pattern);

    Q_INVOKABLE void run();
    Q_INVOKABLE void stop();

    // Finds and kills all processes listening on the given TCP port.
    // Returns true if at least one process was killed.
    Q_INVOKABLE static bool killPort(int port);

signals:
    void commandChanged();
    void argumentsChanged();
    void workingDirectoryChanged();
    void successPatternChanged();
    void runningChanged();
    void successChanged();
    void outputChanged();

    // Emitted when successPattern first matches while the process is running.
    void started();
    // Emitted when the process exits for any reason.
    void stopped(int exitCode);

private:
    void onReadyRead();
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

    QString     m_command;
    QStringList m_arguments;
    QString     m_workingDirectory;
    QString     m_successPattern;
    bool        m_running = false;
    bool        m_success = false;
    QString     m_output;

    QProcess   *m_process = nullptr;
};
