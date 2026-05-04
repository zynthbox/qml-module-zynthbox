#include "recorder.h"
#include <QDebug>
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <KConfigGroup>
#include <QJsonObject>
#include <KDesktopFile>
#include <QDateTime>
#include <QRegularExpression>

Recorder::Recorder(QObject *parent)
    : QObject(parent)
    , m_process(nullptr)
    , m_pwTopTimer(nullptr)
    , m_pid(-1)
    , m_outputDirectory(DEFAULT_OUTPUT_DIRECTORY)
    , m_format(WAV)
    , m_fileName()
    , m_recording(false)
    , m_verbose(false)
    , m_remote()
    , m_mediaType("Audio")
    , m_mediaCategory("Playback")
    , m_mediaRole("Music")
    , m_target()
    , m_latency("100ms")
    , m_properties()
    , m_rate(48000)
    , m_channels(2)
    , m_channelMap()
    , m_listLayouts(false)
    , m_listChannelNames(false)
    , m_sampleFormat("s16")
    , m_listFormats(false)
    , m_container("wav")
    , m_listContainers(false)
    , m_volume(1.0)
    , m_quality(4)
    , m_raw(false)
    , m_forceMidi("ump")
    , m_sampleCount(0)
    , m_pipeWireClients()
    , m_pipeWireClientsLoaded(false)
{
    // startPipeWireMonitoring(5000);
}

Recorder::~Recorder()
{
    // stopPipeWireMonitoring();
}

void Recorder::startPipeWireMonitoring(int intervalMs)
{
    if (m_pwTopTimer)
    {
        qDebug() << "Recorder::startPipeWireMonitoring(): Monitoring already running";
        return;
    }

    m_pwTopTimer = new QTimer(this);
    connect(m_pwTopTimer, &QTimer::timeout, this, &Recorder::onPipeWireMonitorTimer);
    m_pwTopTimer->start(intervalMs);
    qDebug() << "Recorder::startPipeWireMonitoring(): PipeWire monitoring started with interval" << intervalMs << "ms";
    
    // Do an initial check
    onPipeWireMonitorTimer();
}

void Recorder::stopPipeWireMonitoring()
{
    if (!m_pwTopTimer)
    {
        qDebug() << "Recorder::stopPipeWireMonitoring(): Monitoring not running";
        return;
    }

    m_pwTopTimer->stop();
    m_pwTopTimer->deleteLater();
    m_pwTopTimer = nullptr;
    m_knownNodeIds.clear();
    m_knownClientIds.clear();
    qDebug() << "Recorder::stopPipeWireMonitoring(): PipeWire monitoring stopped";
}

void Recorder::onPipeWireMonitorTimer()
{
    updatePipeWireObjectsFromPwTop();
}

void Recorder::updatePipeWireObjectsFromPwTop()
{
    QProcess pwTopProcess;
    pwTopProcess.start("pw-top", QStringList() << "-b" << "-n" << "1");
    
    if (!pwTopProcess.waitForFinished(5000))
    {
        qWarning() << "Recorder::updatePipeWireObjectsFromPwTop(): pw-top did not finish";
        return;
    }

    const QString output = QString::fromUtf8(pwTopProcess.readAllStandardOutput());
    
    if (output.isEmpty())
    {
        qWarning() << "Recorder::updatePipeWireObjectsFromPwTop(): pw-top returned no output";
        return;
    }

    // Parse pw-top output to extract node IDs and client names
    // pw-top output format includes lines like:
    // ID  name
    // 39  pipewire
    // 42  ALSA plug-in
    // etc.
    
    QStringList lines = output.split('\n', Qt::SkipEmptyParts);
    
    for (const QString &line : lines)
    {
        // Skip header lines and empty lines
        if (line.contains("ID") || line.isEmpty() || !line[0].isDigit())
            continue;

        QStringList parts = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        if (parts.size() < 2)
            continue;

        bool ok = false;
        int objectId = parts[0].toInt(&ok);
        if (!ok)
            continue;

        QString objectName = parts.size() > 1 ? parts.mid(1).join(' ') : QString();
        
        // Check if this is a new node (nodes usually have higher IDs)
        if (!m_knownNodeIds.contains(objectId))
        {
            m_knownNodeIds.insert(objectId);
            qDebug() << "Recorder::updatePipeWireObjectsFromPwTop(): New node detected - ID:" << objectId << "Name:" << objectName;
            emit newPipeWireNodeDetected(objectId, objectName);
            emit isValidChanged();
        }
    }
}

bool Recorder::compareVersions(const QString &version, const QString &target) const
{
    // Parse versions into components (e.g., "1.2.3" -> [1, 2, 3])
    QStringList versionParts = version.split('.');
    QStringList targetParts = target.split('.');
    
    // Compare each component
    for (int i = 0; i < qMax(versionParts.size(), targetParts.size()); ++i)
    {
        int versionNum = i < versionParts.size() ? versionParts[i].toInt() : 0;
        int targetNum = i < targetParts.size() ? targetParts[i].toInt() : 0;
        
        if (versionNum > targetNum)
            return true;
        else if (versionNum < targetNum)
            return false;
    }
    
    return false; // versions are equal
}

bool Recorder::ensurePipeWireClientsLoaded() const
{
    if (m_pipeWireClientsLoaded)
        return true;

    QProcess pwDump;
    QStringList arguments;
    
    // Check pw-dump version and add "-R" flag if version > 1.6.0
    const QString pwDumpVersion = getPwDumpVersion();
    if (!pwDumpVersion.isEmpty() && compareVersions(pwDumpVersion, "1.6.0"))
    {
        qDebug() << "Recorder::ensurePipeWireClientsLoaded(): pw-dump version" << pwDumpVersion << "is greater than 1.6.0, adding -R flag";
        arguments << "-R";
    }
    
    pwDump.start("pw-dump", arguments);
    if (!pwDump.waitForFinished(10000))
    {
        qWarning() << "Recorder::ensurePipeWireClientsLoaded(): pw-dump did not finish";
        m_pipeWireClientsLoaded = true;
        return false;
    }

    const QByteArray output = pwDump.readAllStandardOutput();
    if (output.isEmpty())
    {
        qWarning() << "Recorder::ensurePipeWireClientsLoaded(): pw-dump returned no output";
        m_pipeWireClientsLoaded = true;
        return false;
    }

    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(output, &parseError);
    if (parseError.error != QJsonParseError::NoError)
    {
        qWarning() << "Recorder::ensurePipeWireClientsLoaded(): Failed to parse pw-dump JSON:" << parseError.errorString();
        m_pipeWireClientsLoaded = true;
        return false;
    }

    if (!document.isArray())
    {
        qWarning() << "Recorder::ensurePipeWireClientsLoaded(): pw-dump output is not a JSON array";
        m_pipeWireClientsLoaded = true;
        return false;
    }

    const QJsonArray rootArray = document.array();
    for (const QJsonValue &value : rootArray)
    {
        if (!value.isObject())
            continue;

        const QJsonObject obj = value.toObject();
        if (obj.value("type").toString() == QLatin1String("PipeWire:Interface:Node"))
            m_pipeWireClients.append(obj);
    }

    m_pipeWireClientsLoaded = true;
    return true;
}

void Recorder::resetPipeWireClientsJson()
{
    m_pipeWireClients = QJsonArray();
    m_pipeWireClientsLoaded = false;
    emit isValidChanged();
}


int Recorder::pid() const
{
    return m_pid;
}

void Recorder::setPid(int pid)
{
    if (m_pid == pid)
        return;

    m_pid = pid;
    emit pidChanged();
    emit isValidChanged();
}

QString Recorder::outputDirectory() const
{
    return m_outputDirectory;
}

void Recorder::setOutputDirectory(const QString &dir)
{
    if (m_outputDirectory == dir)
        return;

    m_outputDirectory = dir;
    emit outputDirectoryChanged();
}

Recorder::Format Recorder::format() const
{
    return m_format;
}

void Recorder::setFormat(Format fmt)
{
    if (m_format == fmt)
        return;

    m_format = fmt;
    emit formatChanged();
}

QString Recorder::fileName() const
{
    return m_fileName;
}

void Recorder::setFileName(const QString &fileName)
{
    if (m_fileName == fileName)
        return;

    m_fileName = fileName;
    emit fileNameChanged();
}

QString Recorder::getRecordedFilePath() const
{
    if (m_fileName.isEmpty())
        return QString();

    if (m_outputDirectory.isEmpty())
        return m_fileName;

    return QDir(m_outputDirectory).filePath(m_fileName);
}

bool Recorder::verbose() const
{
    return m_verbose;
}

void Recorder::setVerbose(bool verbose)
{
    if (m_verbose == verbose)
        return;

    m_verbose = verbose;
    emit verboseChanged();
}

QString Recorder::remote() const
{
    return m_remote;
}

void Recorder::setRemote(const QString &remote)
{
    if (m_remote == remote)
        return;

    m_remote = remote;
    emit remoteChanged();
}

QString Recorder::mediaType() const
{
    return m_mediaType;
}

void Recorder::setMediaType(const QString &mediaType)
{
    if (m_mediaType == mediaType)
        return;

    m_mediaType = mediaType;
    emit mediaTypeChanged();
}

QString Recorder::mediaCategory() const
{
    return m_mediaCategory;
}

void Recorder::setMediaCategory(const QString &mediaCategory)
{
    if (m_mediaCategory == mediaCategory)
        return;

    m_mediaCategory = mediaCategory;
    emit mediaCategoryChanged();
}

QString Recorder::mediaRole() const
{
    return m_mediaRole;
}

void Recorder::setMediaRole(const QString &mediaRole)
{
    if (m_mediaRole == mediaRole)
        return;

    m_mediaRole = mediaRole;
    emit mediaRoleChanged();
}

QString Recorder::target() const
{
    return m_target;
}

void Recorder::setTarget(const QString &target)
{
    if (m_target == target)
        return;

    m_target = target;
    emit targetChanged();
}

QString Recorder::latency() const
{
    return m_latency;
}

void Recorder::setLatency(const QString &latency)
{
    if (m_latency == latency)
        return;

    m_latency = latency;
    emit latencyChanged();
}

QString Recorder::properties() const
{
    return m_properties;
}

void Recorder::setProperties(const QString &properties)
{
    if (m_properties == properties)
        return;

    m_properties = properties;
    emit propertiesChanged();
}

int Recorder::rate() const
{
    return m_rate;
}

void Recorder::setRate(int rate)
{
    if (m_rate == rate)
        return;

    m_rate = rate;
    emit rateChanged();
}

int Recorder::channels() const
{
    return m_channels;
}

void Recorder::setChannels(int channels)
{
    if (m_channels == channels)
        return;

    m_channels = channels;
    emit channelsChanged();
}

QString Recorder::channelMap() const
{
    return m_channelMap;
}

void Recorder::setChannelMap(const QString &channelMap)
{
    if (m_channelMap == channelMap)
        return;

    m_channelMap = channelMap;
    emit channelMapChanged();
}

bool Recorder::listLayouts() const
{
    return m_listLayouts;
}

void Recorder::setListLayouts(bool listLayouts)
{
    if (m_listLayouts == listLayouts)
        return;

    m_listLayouts = listLayouts;
    emit listLayoutsChanged();
}

bool Recorder::listChannelNames() const
{
    return m_listChannelNames;
}

void Recorder::setListChannelNames(bool listChannelNames)
{
    if (m_listChannelNames == listChannelNames)
        return;

    m_listChannelNames = listChannelNames;
    emit listChannelNamesChanged();
}

QString Recorder::sampleFormat() const
{
    return m_sampleFormat;
}

void Recorder::setSampleFormat(const QString &sampleFormat)
{
    if (m_sampleFormat == sampleFormat)
        return;

    m_sampleFormat = sampleFormat;
    emit sampleFormatChanged();
}

bool Recorder::listFormats() const
{
    return m_listFormats;
}

void Recorder::setListFormats(bool listFormats)
{
    if (m_listFormats == listFormats)
        return;

    m_listFormats = listFormats;
    emit listFormatsChanged();
}

QString Recorder::container() const
{
    return m_container;
}

void Recorder::setContainer(const QString &container)
{
    if (m_container == container)
        return;

    m_container = container;
    emit containerChanged();
}

bool Recorder::listContainers() const
{
    return m_listContainers;
}

void Recorder::setListContainers(bool listContainers)
{
    if (m_listContainers == listContainers)
        return;

    m_listContainers = listContainers;
    emit listContainersChanged();
}

double Recorder::volume() const
{
    return m_volume;
}

void Recorder::setVolume(double volume)
{
    if (m_volume == volume)
        return;

    m_volume = volume;
    emit volumeChanged();
}

int Recorder::quality() const
{
    return m_quality;
}

void Recorder::setQuality(int quality)
{
    if (m_quality == quality)
        return;

    m_quality = quality;
    emit qualityChanged();
}

bool Recorder::raw() const
{
    return m_raw;
}

void Recorder::setRaw(bool raw)
{
    if (m_raw == raw)
        return;

    m_raw = raw;
    emit rawChanged();
}

QString Recorder::forceMidi() const
{
    return m_forceMidi;
}

void Recorder::setForceMidi(const QString &forceMidi)
{
    if (m_forceMidi == forceMidi)
        return;

    m_forceMidi = forceMidi;
    emit forceMidiChanged();
}

int Recorder::sampleCount() const
{
    return m_sampleCount;
}

void Recorder::setSampleCount(int sampleCount)
{
    if (m_sampleCount == sampleCount)
        return;

    m_sampleCount = sampleCount;
    emit sampleCountChanged();
}

bool Recorder::isValid() const
{
    return m_pid != -1 && isPipeWireClientPid(m_pid);
}

bool Recorder::recording() const
{
    return m_recording;
}

void Recorder::start()
{
    if (m_recording)
        return;

    if (m_fileName.isEmpty())
    {
        // Generate a random filename with timestamp
        QString randomName = "recording_" + QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
        setFileName(randomName);
    }

   if (!startProcess())
    {
        return;
    }
    m_recording = true;
    emit recordingChanged();
}

void Recorder::stop()
{
    if (!m_recording)
        return;

    if (m_process && m_process->state() != QProcess::NotRunning)
    {
        m_process->terminate();
        if (!m_process->waitForFinished(3000))
        {
            qWarning() << "Recorder::stop(): Process did not finish gracefully, killing it";
            m_process->kill();
            m_process->waitForFinished();
        }
    }

    m_recording = false;
    emit recordingChanged();
}

void Recorder::restart()
{
    if (m_process && m_process->state() != QProcess::NotRunning)
    {
        stop();
    }
    start();
}

bool Recorder::startProcess()
{
    if (m_process && m_process->state() != QProcess::NotRunning)
    {
        qDebug() << "Recorder::startProcess(): process already running, skipping start";
        return true;
    }

    if (!m_process)
    {
        m_process = new QProcess(this);
        connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                this, &Recorder::onProcessFinished);
    }

    QString adjustedFileName = m_fileName;
    // Check if filename has a valid suffix
    if (adjustedFileName.endsWith(QLatin1String(".wav"), Qt::CaseInsensitive))
    {
        if (m_format != WAV)
        {
            m_format = WAV;
            emit formatChanged();
        }
        // Keep as is
    }
    else if (adjustedFileName.endsWith(QLatin1String(".flac"), Qt::CaseInsensitive))
    {
        if (m_format != FLAC)
        {
            m_format = FLAC;
            emit formatChanged();
        }
        // Keep as is
    }
    else if (adjustedFileName.contains(QLatin1Char('.')))
    {
        // Has a suffix but not .wav or .flac
        qWarning() << "Recorder::startProcess(): Invalid file suffix. Only .wav and .flac are supported.";
        emit recordingError("Invalid file suffix. Only .wav and .flac are supported.");
        return false;
    }
    else
    {
        // No suffix, append based on format
        if (m_format == WAV)
        {
            adjustedFileName += QLatin1String(".wav");
        }
        else
        {
            adjustedFileName += QLatin1String(".flac");
        }
    }

    QString outputPath = m_outputDirectory.isEmpty() ? adjustedFileName : m_outputDirectory + "/" + adjustedFileName;
    QStringList arguments;

    // if (m_verbose)
    //     arguments << "--verbose";

    // if (!m_remote.isEmpty())
    //     arguments << "--remote" << m_remote;

    // if (!m_mediaType.isEmpty())
    //     arguments << "--media-type" << m_mediaType;

    // if (!m_mediaCategory.isEmpty())
    //     arguments << "--media-category" << m_mediaCategory;

    // if (!m_mediaRole.isEmpty())
    //     arguments << "--media-role" << m_mediaRole;

    if (!m_target.isEmpty())
    {
        arguments << "--target" << m_target;
    }
    else
    {
        int targetSerial = -1;
        if (m_pid > 0)
        {
            targetSerial = getPipeWireClientSerialForPid(m_pid);
        }

        if (targetSerial > 0)
        {
            arguments << "--target=" + QString::number(targetSerial);
        }
        else
        {
            const int defaultSinkId = getDefaultAudioOutputSinkId();
            if (defaultSinkId > 0)
            {
                arguments << "--target=" + QString::number(defaultSinkId);
            }
            else if (m_pid > 0)
            {
                qWarning() << "Recorder::startProcess(): Could not resolve pipewire client serial for PID" << m_pid
                           << "and no default sink ID available; using raw PID fallback";
                arguments << "--target=" + QString::number(m_pid);
            }
        }
    }

    // if (!m_latency.isEmpty())
    //     arguments << "--latency" << m_latency;

    // if (!m_properties.isEmpty())
    //     arguments << "--properties" << m_properties;

    // if (m_rate > 0)
    //     arguments << "--rate" << QString::number(m_rate);

    // if (m_channels > 0)
    //     arguments << "--channels" << QString::number(m_channels);

    // if (!m_channelMap.isEmpty())
    //     arguments << "--channel-map" << m_channelMap;

    // if (m_listLayouts)
    //     arguments << "--list-layouts";

    // if (m_listChannelNames)
    //     arguments << "--list-channel-names";

    // if (!m_sampleFormat.isEmpty())
    //     arguments << "--format" << m_sampleFormat;

    // if (m_listFormats)
    //     arguments << "--list-formats";

    // const QString containerOption = !m_container.isEmpty()
    //     ? m_container
    //     : (m_format == WAV ? QStringLiteral("wav") : QStringLiteral("flac"));
    // arguments << "--container" << containerOption;

    // if (m_listContainers)
    //     arguments << "--list-containers";

    // if (m_volume >= 0.0)
    //     arguments << "--volume" << QString::number(m_volume, 'f', 3);

    // if (m_quality >= 0)
    //     arguments << "--quality" << QString::number(m_quality);

    // if (m_raw)
    //     arguments << "--raw";

    // if (!m_forceMidi.isEmpty())
    //     arguments << "--force-midi" << m_forceMidi;

    // if (m_sampleCount > 0)
    //     arguments << "--sample-count" << QString::number(m_sampleCount);

    arguments << outputPath;

    m_process->start("pw-record", arguments);
    
    if (!m_process->waitForStarted())
    {
        qWarning() << "Recorder::startProcess(): Failed to start pw-record process";
        emit recordingError("Failed to start recording process");
    }
    else
    {
        m_pid = m_process->processId();
        emit pidChanged();
        qDebug() << "Recorder::startProcess(): pw-record started with PID" << m_pid;
    }
    return true;
}

void Recorder::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitStatus == QProcess::NormalExit && exitCode == 0)
    {
        qDebug() << "Recorder::onProcessFinished(): Recording finished successfully";
    }
    else
    {
        QString errorMessage = QString("Recording process exited with status: %1, code: %2")
            .arg(exitStatus == QProcess::NormalExit ? "Normal" : "Crashed")
            .arg(exitCode);
        qWarning() << "Recorder::onProcessFinished():" << errorMessage;
        emit recordingError(errorMessage);
    }

    m_pid = -1;
    emit pidChanged();
}

QString Recorder::getExecFromDesktopFile(const QString &desktopFilename)
{
    QString actualFilename = desktopFilename;
    if (!actualFilename.endsWith(QLatin1String(".desktop"), Qt::CaseInsensitive))
    {
        actualFilename += QLatin1String(".desktop");
    }

    KDesktopFile desktopFile(actualFilename);
    
    if (!desktopFile.desktopGroup().hasKey("Exec"))
    {
        qWarning() << "Recorder::getExecFromDesktopFile(): No Exec key found in" << actualFilename;
        return QString();
    }
    
    QString exec = desktopFile.desktopGroup().readEntry("Exec", QString());
    qDebug() << "Recorder::getExecFromDesktopFile(): Found Exec:" << exec;
    
    return exec;
}

int Recorder::getPidFromExecutable(const QString &executableName)
{
    QProcess pidofProcess;
    pidofProcess.start("pidof", QStringList() << executableName);
    
    if (!pidofProcess.waitForFinished())
    {
        qWarning() << "Recorder::getPidFromExecutable(): Failed to execute pidof command for" << executableName;
        return -1;
    }
    
    QString output = QString::fromUtf8(pidofProcess.readAllStandardOutput()).trimmed();
    
    if (output.isEmpty())
    {
        qDebug() << "Recorder::getPidFromExecutable(): No process found for" << executableName;
        return -1;
    }
    
    // pidof returns multiple PIDs separated by spaces, return the first one
    QStringList pids = output.split(' ');
    if (pids.isEmpty())
        return -1;
    
    bool ok = false;
    int pid = pids.first().toInt(&ok);
    
    if (!ok)
    {
        qWarning() << "Recorder::getPidFromExecutable(): Failed to parse PID from pidof output:" << output;
        return -1;
    }
    
    qDebug() << "Recorder::getPidFromExecutable(): Found PID" << pid << "for executable" << executableName;
    return pid;
}

QString Recorder::getPwRecordVersion() const
{
    QProcess pwRecordProcess;
    pwRecordProcess.start("pw-record", QStringList() << "--version");
    
    if (!pwRecordProcess.waitForFinished())
    {
        qWarning() << "Recorder::getPwRecordVersion(): Failed to execute pw-record --version";
        return QString();
    }
    
    QString output = QString::fromUtf8(pwRecordProcess.readAllStandardOutput()).trimmed();
    QString errorOutput = QString::fromUtf8(pwRecordProcess.readAllStandardError()).trimmed();
    
    QString fullOutput;
    if (!output.isEmpty())
    {
        fullOutput = output;
    }
    else if (!errorOutput.isEmpty())
    {
        fullOutput = errorOutput;
    }
    else
    {
        qWarning() << "Recorder::getPwRecordVersion(): No output from pw-record --version";
        return QString();
    }
    
    // Use regex to extract version number (e.g., "1.2.3")
    QRegularExpression versionRegex("\\d+(?:\\.\\d+)+");
    QRegularExpressionMatch match = versionRegex.match(fullOutput);
    
    if (match.hasMatch())
    {
        QString version = match.captured(0);
        qDebug() << "Recorder::getPwRecordVersion(): Extracted version:" << version << "from output:" << fullOutput;
        return version;
    }
    else
    {
        qDebug() << "Recorder::getPwRecordVersion(): No version pattern found, returning full output:" << fullOutput;
        return fullOutput;
    }
}

QString Recorder::getPwDumpVersion() const
{
    QProcess pwDumpProcess;
    pwDumpProcess.start("pw-dump", QStringList() << "--version");
    
    if (!pwDumpProcess.waitForFinished())
    {
        qWarning() << "Recorder::getPwDumpVersion(): Failed to execute pw-dump --version";
        return QString();
    }
    
    QString output = QString::fromUtf8(pwDumpProcess.readAllStandardOutput()).trimmed();
    QString errorOutput = QString::fromUtf8(pwDumpProcess.readAllStandardError()).trimmed();
    
    QString fullOutput;
    if (!output.isEmpty())
    {
        fullOutput = output;
    }
    else if (!errorOutput.isEmpty())
    {
        fullOutput = errorOutput;
    }
    else
    {
        qWarning() << "Recorder::getPwDumpVersion(): No output from pw-dump --version";
        return QString();
    }
    
    // Use regex to extract version number (e.g., "1.2.3")
    QRegularExpression versionRegex("\\d+(?:\\.\\d+)+");
    QRegularExpressionMatch match = versionRegex.match(fullOutput);
    
    if (match.hasMatch())
    {
        QString version = match.captured(0);
        qDebug() << "Recorder::getPwDumpVersion(): Extracted version:" << version << "from output:" << fullOutput;
        return version;
    }
    else
    {
        qDebug() << "Recorder::getPwDumpVersion(): No version pattern found, returning full output:" << fullOutput;
        return fullOutput;
    }
}

QString Recorder::getPipeWireClientsJson() const
{
    if (!ensurePipeWireClientsLoaded())
        return QString();

    return QString::fromUtf8(QJsonDocument(m_pipeWireClients).toJson(QJsonDocument::Compact));
}

QVariantList Recorder::getPipeWireClientsJson(bool asVariantList) const
{
    Q_UNUSED(asVariantList)
    QVariantList result;
    if (!ensurePipeWireClientsLoaded())
        return result;

    for (const QJsonValue &value : m_pipeWireClients)
    {
        if (!value.isObject())
            continue;

        const QJsonObject obj = value.toObject();
        QVariantMap pair;
        pair[QStringLiteral("id")] = obj.value(QStringLiteral("id")).toVariant();
        pair[QStringLiteral("client")] = obj.toVariantMap();
        result.append(pair);
    }

    return result;
}

int Recorder::getPipeWireClientSerialForPid(int pid) const
{
    if (!ensurePipeWireClientsLoaded())
        return -1;

    for (const QJsonValue &value : m_pipeWireClients)
    {
        if (!value.isObject())
            continue;

        const QJsonObject obj = value.toObject();
        const QJsonObject info = obj.value(QStringLiteral("info")).toObject();
        const QJsonObject props = info.value(QStringLiteral("props")).toObject();
        const int applicationPid = props.value(QStringLiteral("application.process.id")).toInt(-1);
        const int secPid = props.value(QStringLiteral("pipewire.sec.pid")).toInt(-1);
        if (applicationPid == pid || secPid == pid)
        {
            const int serial = props.value(QStringLiteral("object.serial")).toInt(-1);
            if (serial > 0)
                return serial;
        }
    }

    return -1;
}

int Recorder::getPipeWireNodeIdForPid(int pid) const
{
    if (!ensurePipeWireClientsLoaded())
        return -1;

    int clientId = -1;

    // Find client with matching PID
    for (const QJsonValue &value : m_pipeWireClients)
    {
        if (!value.isObject())
            continue;

        const QJsonObject obj = value.toObject();
        const QJsonObject info = obj.value("info").toObject();
        const QJsonObject props = info.value("props").toObject();
        const int appPid = props.value("application.process.id").toInt(-1);
        const int secPid = props.value("pipewire.sec.pid").toInt(-1);
        if (appPid == pid || secPid == pid)
        {
            clientId = obj.value("id").toInt(-1);
            break;
        }
    }

    if (clientId == -1)
    {
        qWarning() << "Recorder::getPipeWireNodeIdForPid(): No client found for PID" << pid;
        return -1;
    }

    return clientId;
}

bool Recorder::isPipeWireClientPid(int pid) const
{
    QProcess pwDump;
    QStringList arguments;
    
    // Check pw-dump version and add "-R" flag if version > 1.6.0
    const QString pwDumpVersion = getPwDumpVersion();
    if (!pwDumpVersion.isEmpty() && compareVersions(pwDumpVersion, "1.6.0"))
    {
        qDebug() << "Recorder::isPipeWireClientPid(): pw-dump version" << pwDumpVersion << "is greater than 1.6.0, adding -R flag for PID" << pid;
        arguments << "-R";
    }
    
    pwDump.start("pw-dump", arguments);
    if (!pwDump.waitForFinished(10000))
    {
        qWarning() << "Recorder::isPipeWireClientPid(): pw-dump did not finish for PID" << pid;
        return false;
    }

    const QByteArray output = pwDump.readAllStandardOutput();
    if (output.isEmpty())
    {
        qWarning() << "Recorder::isPipeWireClientPid(): pw-dump returned no output for PID" << pid;
        return false;
    }

    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(output, &parseError);
    if (parseError.error != QJsonParseError::NoError)
    {
        qWarning() << "Recorder::isPipeWireClientPid(): Failed to parse pw-dump JSON for PID" << pid << ":" << parseError.errorString();
        return false;
    }

    if (!document.isArray())
    {
        qWarning() << "Recorder::isPipeWireClientPid(): pw-dump output is not a JSON array for PID" << pid;
        return false;
    }

    const QJsonArray rootArray = document.array();
    for (const QJsonValue &value : rootArray)
    {
        if (!value.isObject())
            continue;

        const QJsonObject obj = value.toObject();
        if (obj.value("type").toString() != QLatin1String("PipeWire:Interface:Client"))
            continue;

        const QJsonObject info = obj.value("info").toObject();
        const QJsonObject props = info.value("props").toObject();

        if (props.value("application.process.id").toInt(-1) == pid)
            return true;

        if (props.value("pipewire.sec.pid").toInt(-1) == pid)
            return true;
    }

    return false;
}

int Recorder::getDefaultAudioOutputSinkId() const
{
    const QString command = QStringLiteral("wpctl status | grep -A 5 \"Sinks:\" | grep \"*\" | awk '{print $3}' | sed 's/\\.//' ");
    QProcess process;
    process.start("sh", QStringList() << "-c" << command);
    if (!process.waitForFinished(10000))
    {
        qWarning() << "Recorder::getDefaultAudioOutputSinkId(): wpctl command did not finish";
        return -1;
    }

    const QString output = QString::fromUtf8(process.readAllStandardOutput()).trimmed();
    if (output.isEmpty())
    {
        qWarning() << "Recorder::getDefaultAudioOutputSinkId(): no default sink returned from wpctl";
        return -1;
    }

    const QStringList outputLines = output.split(QLatin1Char('\n'), Qt::SkipEmptyParts);
    const QString sinkLine = outputLines.isEmpty() ? QString() : outputLines.first().trimmed();
    if (sinkLine.isEmpty())
    {
        qWarning() << "Recorder::getDefaultAudioOutputSinkId(): no valid sink line found in output";
        return -1;
    }

    bool ok = false;
    const int sinkId = sinkLine.toInt(&ok);
    if (!ok)
    {
        qWarning() << "Recorder::getDefaultAudioOutputSinkId(): failed to parse sink id from output" << sinkLine;
        return -1;
    }

    return sinkId;
}