#ifndef RECORDER_H
#define RECORDER_H

#include <QObject>
#include <QProcess>
#include <QString>
#include <QVariant>
#include <QtGlobal>
#include <KDesktopFile>

class Q_DECL_EXPORT Recorder : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int pid READ pid WRITE setPid NOTIFY pidChanged)
    Q_PROPERTY(QString outputDirectory READ outputDirectory WRITE setOutputDirectory NOTIFY outputDirectoryChanged)
    Q_PROPERTY(Format format READ format WRITE setFormat NOTIFY formatChanged)
    Q_PROPERTY(QString fileName READ fileName WRITE setFileName NOTIFY fileNameChanged)
    Q_PROPERTY(bool recording READ recording NOTIFY recordingChanged)
    Q_PROPERTY(bool verbose READ verbose WRITE setVerbose NOTIFY verboseChanged)
    Q_PROPERTY(QString remote READ remote WRITE setRemote NOTIFY remoteChanged)
    Q_PROPERTY(QString mediaType READ mediaType WRITE setMediaType NOTIFY mediaTypeChanged)
    Q_PROPERTY(QString mediaCategory READ mediaCategory WRITE setMediaCategory NOTIFY mediaCategoryChanged)
    Q_PROPERTY(QString mediaRole READ mediaRole WRITE setMediaRole NOTIFY mediaRoleChanged)
    Q_PROPERTY(QString target READ target WRITE setTarget NOTIFY targetChanged)
    Q_PROPERTY(QString latency READ latency WRITE setLatency NOTIFY latencyChanged)
    Q_PROPERTY(QString properties READ properties WRITE setProperties NOTIFY propertiesChanged)
    Q_PROPERTY(int rate READ rate WRITE setRate NOTIFY rateChanged)
    Q_PROPERTY(int channels READ channels WRITE setChannels NOTIFY channelsChanged)
    Q_PROPERTY(QString channelMap READ channelMap WRITE setChannelMap NOTIFY channelMapChanged)
    Q_PROPERTY(bool listLayouts READ listLayouts WRITE setListLayouts NOTIFY listLayoutsChanged)
    Q_PROPERTY(bool listChannelNames READ listChannelNames WRITE setListChannelNames NOTIFY listChannelNamesChanged)
    Q_PROPERTY(QString sampleFormat READ sampleFormat WRITE setSampleFormat NOTIFY sampleFormatChanged)
    Q_PROPERTY(bool listFormats READ listFormats WRITE setListFormats NOTIFY listFormatsChanged)
    Q_PROPERTY(QString container READ container WRITE setContainer NOTIFY containerChanged)
    Q_PROPERTY(bool listContainers READ listContainers WRITE setListContainers NOTIFY listContainersChanged)
    Q_PROPERTY(double volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(int quality READ quality WRITE setQuality NOTIFY qualityChanged)
    Q_PROPERTY(bool raw READ raw WRITE setRaw NOTIFY rawChanged)
    Q_PROPERTY(QString forceMidi READ forceMidi WRITE setForceMidi NOTIFY forceMidiChanged)
    Q_PROPERTY(int sampleCount READ sampleCount WRITE setSampleCount NOTIFY sampleCountChanged)

public:
    enum Format {
        WAV,
        FLAC
    };
    Q_ENUM(Format)

    explicit Recorder(QObject *parent = nullptr);
    ~Recorder();

    int pid() const;
    void setPid(int pid);
    QString outputDirectory() const;
    void setOutputDirectory(const QString &dir);
    Format format() const;
    void setFormat(Format fmt);
    QString fileName() const;
    void setFileName(const QString &name);
    bool recording() const;
    bool verbose() const;
    void setVerbose(bool verbose);
    QString remote() const;
    void setRemote(const QString &remote);
    QString mediaType() const;
    void setMediaType(const QString &mediaType);
    QString mediaCategory() const;
    void setMediaCategory(const QString &mediaCategory);
    QString mediaRole() const;
    void setMediaRole(const QString &mediaRole);
    QString target() const;
    void setTarget(const QString &target);
    QString latency() const;
    void setLatency(const QString &latency);
    QString properties() const;
    void setProperties(const QString &properties);
    int rate() const;
    void setRate(int rate);
    int channels() const;
    void setChannels(int channels);
    QString channelMap() const;
    void setChannelMap(const QString &channelMap);
    bool listLayouts() const;
    void setListLayouts(bool listLayouts);
    bool listChannelNames() const;
    void setListChannelNames(bool listChannelNames);
    QString sampleFormat() const;
    void setSampleFormat(const QString &sampleFormat);
    bool listFormats() const;
    void setListFormats(bool listFormats);
    QString container() const;
    void setContainer(const QString &container);
    bool listContainers() const;
    void setListContainers(bool listContainers);
    double volume() const;
    void setVolume(double volume);
    int quality() const;
    void setQuality(int quality);
    bool raw() const;
    void setRaw(bool raw);
    QString forceMidi() const;
    void setForceMidi(const QString &forceMidi);
    int sampleCount() const;
    void setSampleCount(int sampleCount);

public slots:
    void start();
    void stop();
    void restart();
    QString getPipeWireClientsJson() const;
    QVariantList getPipeWireClientsJson(bool asVariantList) const;
    int getPipeWireClientSerialForPid(int pid) const;
    int getPipeWireNodeIdForPid(int pid) const;
    bool isPipeWireClientPid(int pid) const;
    int getDefaultAudioOutputSinkId() const;
    QString getExecFromDesktopFile(const QString &desktopFilename);
    int getPidFromExecutable(const QString &executableName);

signals:
    void pidChanged();
    void outputDirectoryChanged();
    void formatChanged();
    void fileNameChanged();
    void recordingChanged();
    void verboseChanged();
    void remoteChanged();
    void mediaTypeChanged();
    void mediaCategoryChanged();
    void mediaRoleChanged();
    void targetChanged();
    void latencyChanged();
    void propertiesChanged();
    void rateChanged();
    void channelsChanged();
    void channelMapChanged();
    void listLayoutsChanged();
    void listChannelNamesChanged();
    void sampleFormatChanged();
    void listFormatsChanged();
    void containerChanged();
    void listContainersChanged();
    void volumeChanged();
    void qualityChanged();
    void rawChanged();
    void forceMidiChanged();
    void sampleCountChanged();
    void recordingError(const QString &error);

private slots:
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    bool startProcess();
    QProcess *m_process;
    int m_pid;
    QString m_outputDirectory;
    Format m_format;
    QString m_fileName;
    bool m_recording;
    bool m_verbose;
    QString m_remote;
    QString m_mediaType;
    QString m_mediaCategory;
    QString m_mediaRole;
    QString m_target;
    QString m_latency;
    QString m_properties;
    int m_rate;
    int m_channels;
    QString m_channelMap;
    bool m_listLayouts;
    bool m_listChannelNames;
    QString m_sampleFormat;
    bool m_listFormats;
    QString m_container;
    bool m_listContainers;
    double m_volume;
    int m_quality;
    bool m_raw;
    QString m_forceMidi;
    int m_sampleCount;
};

#endif // RECORDER_H