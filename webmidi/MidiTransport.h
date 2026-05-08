#pragma once
#include <QList>
#include <QMap>
#include <QObject>
#include <QThread>
#include <QVariantList>
#include <alsa/asoundlib.h>

/**
 * MidiTransport
 *
 * ALSA-backed MIDI engine exposed via QWebChannel.
 * The JS side (jzz_webchannel.js) registers this as a JZZ.js engine.
 */
class MidiTransport : public QObject
{
    Q_OBJECT
public:
    explicit MidiTransport(QObject *parent = nullptr);
    ~MidiTransport() override;

    Q_INVOKABLE QVariantList inputPorts()  const;
    Q_INVOKABLE QVariantList outputPorts() const;

    Q_INVOKABLE bool openInput (int portIndex, const QString &id);
    Q_INVOKABLE bool openOutput(int portIndex, const QString &id);
    Q_INVOKABLE void closeInput (const QString &id);
    Q_INVOKABLE void closeOutput(const QString &id);
    Q_INVOKABLE void send(const QString &portId, const QVariantList &bytes);

signals:
    void received(const QString &portId, const QVariantList &bytes);

private:
    // Enumerate ALSA sequencer ports
    struct PortInfo { int client; int port; QString name; };
    QList<PortInfo> enumeratePorts(unsigned int caps) const;

    // ALSA sequencer handle shared across all ports
    snd_seq_t *_seq = nullptr;

    struct InputPort {
        int alsaPort;   // our own seq port subscribed to the source
        int srcClient;
        int srcPort;
    };
    struct OutputPort {
        int alsaPort;   // our own seq port connected to the destination
        int dstClient;
        int dstPort;
    };

    QMap<QString, InputPort>  _inputs;
    QMap<QString, OutputPort> _outputs;

    // Background thread polling ALSA for incoming events
    QThread  *_pollThread = nullptr;
    bool      _running    = false;

    void _startPolling();
    void _stopPolling();
    void _pollLoop();
};
