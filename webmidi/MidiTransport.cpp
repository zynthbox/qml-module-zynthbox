#include "MidiTransport.h"
#include <QDebug>
#include <QVariantMap>

MidiTransport::MidiTransport(QObject *parent) : QObject(parent)
{
    if (snd_seq_open(&_seq, "default", SND_SEQ_OPEN_DUPLEX, 0) < 0) {
        qWarning() << "[MidiTransport] Failed to open ALSA sequencer";
        _seq = nullptr;
        return;
    }
    snd_seq_set_client_name(_seq, "AudioControlViewer");
}

MidiTransport::~MidiTransport()
{
    _stopPolling();
    if (_seq) snd_seq_close(_seq);
}

// ── Port enumeration ──────────────────────────────────────────────────────────

QList<MidiTransport::PortInfo> MidiTransport::enumeratePorts(unsigned int caps) const
{
    QList<PortInfo> list;
    if (!_seq) return list;

    snd_seq_client_info_t *cinfo;
    snd_seq_port_info_t   *pinfo;
    snd_seq_client_info_alloca(&cinfo);
    snd_seq_port_info_alloca(&pinfo);

    snd_seq_client_info_set_client(cinfo, -1);
    while (snd_seq_query_next_client(_seq, cinfo) >= 0) {
        int client = snd_seq_client_info_get_client(cinfo);
        if (client == SND_SEQ_CLIENT_SYSTEM) continue;

        snd_seq_port_info_set_client(pinfo, client);
        snd_seq_port_info_set_port(pinfo, -1);
        while (snd_seq_query_next_port(_seq, pinfo) >= 0) {
            unsigned int portCaps = snd_seq_port_info_get_capability(pinfo);
            if ((portCaps & caps) == caps) {
                PortInfo p;
                p.client = client;
                p.port   = snd_seq_port_info_get_port(pinfo);
                p.name   = QString("%1:%2")
                    .arg(snd_seq_client_info_get_name(cinfo))
                    .arg(snd_seq_port_info_get_name(pinfo));
                list.append(p);
            }
        }
    }
    return list;
}

QVariantList MidiTransport::inputPorts() const
{
    QVariantList result;
    const auto ports = enumeratePorts(SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ);
    for (int i = 0; i < ports.size(); ++i) {
        QVariantMap m;
        m[QStringLiteral("index")] = i;
        m[QStringLiteral("id")]    = QStringLiteral("in_%1").arg(i);
        m[QStringLiteral("name")]  = ports[i].name;
        m[QStringLiteral("_client")] = ports[i].client;
        m[QStringLiteral("_port")]   = ports[i].port;
        result.append(m);
    }
    return result;
}

QVariantList MidiTransport::outputPorts() const
{
    QVariantList result;
    const auto ports = enumeratePorts(SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE);
    for (int i = 0; i < ports.size(); ++i) {
        QVariantMap m;
        m[QStringLiteral("index")] = i;
        m[QStringLiteral("id")]    = QStringLiteral("out_%1").arg(i);
        m[QStringLiteral("name")]  = ports[i].name;
        m[QStringLiteral("_client")] = ports[i].client;
        m[QStringLiteral("_port")]   = ports[i].port;
        result.append(m);
    }
    return result;
}

// ── Open / close ──────────────────────────────────────────────────────────────

bool MidiTransport::openInput(int portIndex, const QString &id)
{
    if (_inputs.contains(id) || !_seq) return _inputs.contains(id);

    const auto ports = enumeratePorts(SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ);
    if (portIndex < 0 || portIndex >= ports.size()) return false;
    const auto &src = ports[portIndex];

    int myPort = snd_seq_create_simple_port(_seq,
        id.toUtf8().constData(),
        SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE,
        SND_SEQ_PORT_TYPE_APPLICATION);
    if (myPort < 0) { qWarning() << "[MidiTransport] create input port failed"; return false; }

    snd_seq_addr_t sender{ (unsigned char)src.client, (unsigned char)src.port };
    snd_seq_addr_t dest  { (unsigned char)snd_seq_client_id(_seq), (unsigned char)myPort };
    snd_seq_port_subscribe_t *sub;
    snd_seq_port_subscribe_alloca(&sub);
    snd_seq_port_subscribe_set_sender(sub, &sender);
    snd_seq_port_subscribe_set_dest(sub, &dest);

    if (snd_seq_subscribe_port(_seq, sub) < 0) {
        snd_seq_delete_port(_seq, myPort);
        qWarning() << "[MidiTransport] subscribe input failed for" << id;
        return false;
    }

    _inputs.insert(id, { myPort, src.client, src.port });
    qDebug() << "[MidiTransport] input opened:" << id;

    if (_inputs.size() == 1) _startPolling();
    return true;
}

bool MidiTransport::openOutput(int portIndex, const QString &id)
{
    if (_outputs.contains(id) || !_seq) return _outputs.contains(id);

    const auto ports = enumeratePorts(SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE);
    if (portIndex < 0 || portIndex >= ports.size()) return false;
    const auto &dst = ports[portIndex];

    int myPort = snd_seq_create_simple_port(_seq,
        id.toUtf8().constData(),
        SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ,
        SND_SEQ_PORT_TYPE_APPLICATION);
    if (myPort < 0) { qWarning() << "[MidiTransport] create output port failed"; return false; }

    snd_seq_addr_t sender{ (unsigned char)snd_seq_client_id(_seq), (unsigned char)myPort };
    snd_seq_addr_t dest  { (unsigned char)dst.client, (unsigned char)dst.port };
    snd_seq_port_subscribe_t *sub;
    snd_seq_port_subscribe_alloca(&sub);
    snd_seq_port_subscribe_set_sender(sub, &sender);
    snd_seq_port_subscribe_set_dest(sub, &dest);

    if (snd_seq_subscribe_port(_seq, sub) < 0) {
        snd_seq_delete_port(_seq, myPort);
        qWarning() << "[MidiTransport] subscribe output failed for" << id;
        return false;
    }

    _outputs.insert(id, { myPort, dst.client, dst.port });
    qDebug() << "[MidiTransport] output opened:" << id;
    return true;
}

void MidiTransport::closeInput(const QString &id)
{
    if (!_seq || !_inputs.contains(id)) return;
    auto p = _inputs.take(id);
    snd_seq_delete_port(_seq, p.alsaPort);
    if (_inputs.isEmpty()) _stopPolling();
}

void MidiTransport::closeOutput(const QString &id)
{
    if (!_seq || !_outputs.contains(id)) return;
    auto p = _outputs.take(id);
    snd_seq_delete_port(_seq, p.alsaPort);
}

// ── Send ──────────────────────────────────────────────────────────────────────

void MidiTransport::send(const QString &portId, const QVariantList &bytes)
{
    if (!_seq || !_outputs.contains(portId)) return;
    const auto &out = _outputs[portId];

    snd_seq_event_t ev;
    snd_seq_ev_clear(&ev);
    snd_seq_ev_set_source(&ev, out.alsaPort);
    snd_seq_ev_set_dest(&ev, out.dstClient, out.dstPort);
    snd_seq_ev_set_direct(&ev);

    QVector<unsigned char> data;
    for (const QVariant &b : bytes) data.append(static_cast<unsigned char>(b.toInt()));

    if (data.isEmpty()) return;
    unsigned char status = data[0];

    if (status == 0xF0) {
        // SysEx
        snd_seq_ev_set_sysex(&ev, data.size(), data.data());
    } else {
        unsigned char d1 = data.size() > 1 ? data[1] : 0;
        unsigned char d2 = data.size() > 2 ? data[2] : 0;
        switch (status & 0xF0) {
        case 0x80: snd_seq_ev_set_noteoff(&ev,  status & 0xF, d1, d2); break;
        case 0x90: snd_seq_ev_set_noteon(&ev,   status & 0xF, d1, d2); break;
        case 0xA0: snd_seq_ev_set_keypress(&ev, status & 0xF, d1, d2); break;
        case 0xB0: snd_seq_ev_set_controller(&ev, status & 0xF, d1, d2); break;
        case 0xC0: snd_seq_ev_set_pgmchange(&ev,  status & 0xF, d1);    break;
        case 0xD0: snd_seq_ev_set_chanpress(&ev,  status & 0xF, d1);    break;
        case 0xE0: snd_seq_ev_set_pitchbend(&ev,  status & 0xF, (d1 | (d2 << 7)) - 8192); break;
        default:
            snd_seq_ev_set_sysex(&ev, data.size(), data.data());
            break;
        }
    }

    snd_seq_event_output_direct(_seq, &ev);
}

// ── Poll loop (background thread) ─────────────────────────────────────────────

void MidiTransport::_startPolling()
{
    _running = true;
    _pollThread = QThread::create([this]{ _pollLoop(); });
    _pollThread->setObjectName(QStringLiteral("MidiPollThread"));
    _pollThread->start();
}

void MidiTransport::_stopPolling()
{
    _running = false;
    if (_pollThread) {
        _pollThread->quit();
        _pollThread->wait(500);
        delete _pollThread;
        _pollThread = nullptr;
    }
}

void MidiTransport::_pollLoop()
{
    int nfds = snd_seq_poll_descriptors_count(_seq, POLLIN);
    QVector<pollfd> pfds(nfds);
    snd_seq_poll_descriptors(_seq, pfds.data(), nfds, POLLIN);

    while (_running) {
        if (poll(pfds.data(), nfds, 100) <= 0) continue;

        snd_seq_event_t *ev = nullptr;
        while (snd_seq_event_input(_seq, &ev) > 0 && ev) {
            // Find which of our input ports received this event
            QString portId;
            for (auto it = _inputs.cbegin(); it != _inputs.cend(); ++it) {
                if (it.value().alsaPort == ev->dest.port) { portId = it.key(); break; }
            }
            if (portId.isEmpty()) { snd_seq_free_event(ev); continue; }

            QVariantList bytes;
            switch (ev->type) {
            case SND_SEQ_EVENT_NOTEON:
                bytes << (0x90 | ev->data.note.channel)
                      << ev->data.note.note << ev->data.note.velocity; break;
            case SND_SEQ_EVENT_NOTEOFF:
                bytes << (0x80 | ev->data.note.channel)
                      << ev->data.note.note << ev->data.note.velocity; break;
            case SND_SEQ_EVENT_KEYPRESS:
                bytes << (0xA0 | ev->data.note.channel)
                      << ev->data.note.note << ev->data.note.velocity; break;
            case SND_SEQ_EVENT_CONTROLLER:
                bytes << (0xB0 | ev->data.control.channel)
                      << ev->data.control.param << ev->data.control.value; break;
            case SND_SEQ_EVENT_PGMCHANGE:
                bytes << (0xC0 | ev->data.control.channel)
                      << ev->data.control.value; break;
            case SND_SEQ_EVENT_CHANPRESS:
                bytes << (0xD0 | ev->data.control.channel)
                      << ev->data.control.value; break;
            case SND_SEQ_EVENT_PITCHBEND: {
                int v = ev->data.control.value + 8192;
                bytes << (0xE0 | ev->data.control.channel)
                      << (v & 0x7F) << ((v >> 7) & 0x7F); break;
            }
            case SND_SEQ_EVENT_SYSEX:
                for (unsigned int i = 0; i < ev->data.ext.len; ++i)
                    bytes << static_cast<int>(static_cast<unsigned char *>(ev->data.ext.ptr)[i]);
                break;
            default: break;
            }

            if (!bytes.isEmpty())
                QMetaObject::invokeMethod(this, "received", Qt::QueuedConnection,
                                          Q_ARG(QString, portId),
                                          Q_ARG(QVariantList, bytes));
            snd_seq_free_event(ev);
        }
    }
}
