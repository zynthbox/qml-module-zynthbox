#include <QCoreApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QJsonDocument>
#include <QTimer>
#include <QDebug>
#include <csignal>

#include "recorder.h"

static Recorder *g_recorder = nullptr;
static QCoreApplication *g_app = nullptr;

void signalHandler(int sig)
{
    Q_UNUSED(sig)
    if (g_app)
    {
        g_app->quit();
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationName("recorder-test");
    app.setApplicationVersion("1.0");

    QCommandLineParser parser;
    parser.setApplicationDescription("Recorder test utility for pw-record and pw-dump integration.");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption listClientsOption(
        QStringList() << QStringLiteral("l") << QStringLiteral("list-clients"),
        QStringLiteral("Print all PipeWire client entries."));
    QCommandLineOption recordPidOption(
        QStringList() << QStringLiteral("p") << QStringLiteral("record-pid"),
        QStringLiteral("Start recording for the given PipeWire client PID."),
        QStringLiteral("pid"));
    QCommandLineOption serialForPidOption(
        QStringList() << QStringLiteral("s") << QStringLiteral("serial-for-pid"),
        QStringLiteral("Print the PipeWire object serial for the given application PID."),
        QStringLiteral("pid"));
    QCommandLineOption nodeIdForPidOption(
        QStringList() << QStringLiteral("n") << QStringLiteral("node-id-for-pid"),
        QStringLiteral("Print the PipeWire node ID for the given application PID."),
        QStringLiteral("pid"));
    QCommandLineOption pidForDesktopOption(
        QStringList() << QStringLiteral("D") << QStringLiteral("pid-for-desktop"),
        QStringLiteral("Print the process PID for the given .desktop file."),
        QStringLiteral("desktop-file"));
    QCommandLineOption outputFileOption(
        QStringList() << QStringLiteral("o") << QStringLiteral("output"),
        QStringLiteral("Output file name."),
        QStringLiteral("file"));
    QCommandLineOption outputDirOption(
        QStringList() << QStringLiteral("d") << QStringLiteral("output-dir"),
        QStringLiteral("Output directory."),
        QStringLiteral("directory"));
    QCommandLineOption durationOption(
        QStringList() << QStringLiteral("t") << QStringLiteral("duration"),
        QStringLiteral("Recording duration in seconds."),
        QStringLiteral("seconds"));
    QCommandLineOption verboseOption(
        QStringList() << QStringLiteral("v") << QStringLiteral("verbose"),
        QStringLiteral("Enable verbose output for the recorder."));
    QCommandLineOption formatOption(
        QStringList() << QStringLiteral("f") << QStringLiteral("format"),
        QStringLiteral("Output format: wav or flac."),
        QStringLiteral("format"),
        QStringLiteral("flac"));
    QCommandLineOption targetOption(
        QStringList() << QStringLiteral("T") << QStringLiteral("target"),
        QStringLiteral("Optional pw-record target string to override PID target."),
        QStringLiteral("target"));

    parser.addOptions({
        listClientsOption,
        recordPidOption,
        serialForPidOption,
        nodeIdForPidOption,
        pidForDesktopOption,
        outputFileOption,
        outputDirOption,
        durationOption,
        verboseOption,
        formatOption,
        targetOption
    });

    parser.process(app);

    Recorder recorder;

    QObject::connect(&recorder, &Recorder::recordingError, [&](const QString &error)
    {
        qCritical() << "Recording error:" << error;
        app.quit();
    });

    g_recorder = &recorder;
    g_app = &app;

    // Stop recording when the application is about to quit
    QObject::connect(&app, &QCoreApplication::aboutToQuit, [&recorder]() {
        if (recorder.recording())
        {
            recorder.stop();
        }
    });

    // Install signal handlers for graceful shutdown
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    if (parser.isSet(verboseOption))
    {
        recorder.setVerbose(true);
    }

    if (parser.isSet(outputDirOption))
    {
        recorder.setOutputDirectory(parser.value(outputDirOption));
    }

    const QString formatValue = parser.value(formatOption).toLower();
    if (formatValue == QLatin1String("wav"))
    {
        recorder.setFormat(Recorder::WAV);
    }
    else
    {
        recorder.setFormat(Recorder::FLAC);
    }

    if (parser.isSet(listClientsOption))
    {
        const QString clientsJson = recorder.getPipeWireClientsJson();
        if (clientsJson.isEmpty())
        {
            qWarning() << "No PipeWire clients available or pw-dump failed.";
            return 1;
        }

        const QJsonDocument document = QJsonDocument::fromJson(clientsJson.toUtf8());
        if (document.isArray())
        {
            qDebug().noquote() << QJsonDocument(document).toJson(QJsonDocument::Indented);
        }
        else
        {
            qDebug().noquote() << clientsJson;
        }
        return 0;
    }

    if (parser.isSet(serialForPidOption))
    {
        bool ok = false;
        const int pid = parser.value(serialForPidOption).toInt(&ok);
        if (!ok || pid <= 0)
        {
            qCritical() << "Invalid PID provided for --serial-for-pid.";
            return 1;
        }

        const int serial = recorder.getPipeWireClientSerialForPid(pid);
        if (serial <= 0)
        {
            qCritical() << "Could not resolve PipeWire object serial for PID" << pid;
            return 1;
        }

        qDebug() << "serial.object.id:" << serial;
        return 0;
    }

    if (parser.isSet(nodeIdForPidOption))
    {
        bool ok = false;
        const int pid = parser.value(nodeIdForPidOption).toInt(&ok);
        if (!ok || pid <= 0)
        {
            qCritical() << "Invalid PID provided for --node-id-for-pid.";
            return 1;
        }

        const int nodeId = recorder.getPipeWireNodeIdForPid(pid);
        if (nodeId <= 0)
        {
            qCritical() << "Could not resolve PipeWire node ID for PID" << pid;
            return 1;
        }

        qDebug() << "node.id:" << nodeId;
        return 0;
    }

    if (parser.isSet(pidForDesktopOption))
    {
        const QString desktopFile = parser.value(pidForDesktopOption);
        const QString exec = recorder.getExecFromDesktopFile(desktopFile).trimmed();
        if (exec.isEmpty())
        {
            qCritical() << "Could not read Exec entry from" << desktopFile;
            return 1;
        }

        const QString executable = exec.split(QLatin1Char(' '), Qt::SkipEmptyParts).first();
        if (executable.isEmpty())
        {
            qCritical() << "Could not parse executable from Exec entry:" << exec;
            return 1;
        }

        const int pid = recorder.getPidFromExecutable(executable);
        if (pid <= 0)
        {
            qCritical() << "Could not find PID for executable" << executable;
            return 1;
        }

        qDebug() << "pid:" << pid;
        return 0;
    }

    if (parser.isSet(recordPidOption))
    {
        bool ok = false;
        const int pid = parser.value(recordPidOption).toInt(&ok);
        if (!ok || pid <= 0)
        {
            qCritical() << "Invalid PID provided for --record-pid.";
            return 1;
        }

        recorder.setPid(pid);

        if (parser.isSet(outputFileOption))
        {
            recorder.setFileName(parser.value(outputFileOption));
        }

        if (parser.isSet(outputDirOption))
        {
            recorder.setOutputDirectory(parser.value(outputDirOption));
        }

        if (parser.isSet(targetOption))
        {
            recorder.setTarget(parser.value(targetOption));
        }
        
        recorder.start();

        if (!recorder.recording())
        {
            qCritical() << "Failed to start recording.";
            return 1;
        }

        qDebug() << "Recording started for PID" << pid
                 << "output=" << recorder.outputDirectory() + (recorder.outputDirectory().isEmpty() ? QString() : QStringLiteral("/")) + recorder.fileName();

        if (parser.isSet(durationOption))
        {
            bool durationOk = false;
            const int seconds = parser.value(durationOption).toInt(&durationOk);
            if (durationOk && seconds > 0)
            {
                qDebug() << "Will stop recording after" << seconds << "seconds.";
                QTimer::singleShot(seconds * 1000, [&]() {
                    qDebug() << "Stopping recording after duration.";
                    recorder.stop();
                    app.quit();
                });
            }
        }

        return app.exec();
    }

    parser.showHelp(1);
    return 0;
}
