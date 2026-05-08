import QtQuick 2.15
import QtQuick.Controls 2.15
import QtWebEngine 1.10
import QtWebChannel 1.0
import io.zynthbox.webmidi 1.0 

Item {
    id: root  

    property alias url: _view.url
    property alias loading: _view.loading
      // ── WebChannel owns the transport ─────────────────────────────────────────
    WebChannel {
        id: _channel
        Component.onCompleted: _channel.registerObject("midiTransport", MidiTransport)
    }

    // ── WebEngineView ─────────────────────────────────────────────────────────
    WebEngineView {
        id: _view
        anchors.fill: parent
        url: "https://audiocontrol.org/roland/s330/editor"

        webChannel: _channel

        settings {
            javascriptEnabled:               true
            localContentCanAccessRemoteUrls: true
            allowRunningInsecureContent:     true
            errorPageEnabled:                false
        }

        onCertificateError: function(error) {
            error.ignoreCertificateError()
        }

        onFeaturePermissionRequested: function(origin, feature) {
            Qt.callLater(function() { grantFeaturePermission(origin, feature, true); });
        }

        onLoadingChanged: {
            if (loadRequest.status === WebEngineView.LoadSucceededStatus) {
                var m = _view.url.toString().match(/^(https?:\/\/[^/]+)/);
                var origin = m ? m[1] : _view.url.toString();
                Qt.callLater(function() {
                    grantFeaturePermission(origin, WebEngineView.Midi,      true);
                    grantFeaturePermission(origin, WebEngineView.MidiSysex, true);
                });
            }
        }
    }

    // ── Status overlay ────────────────────────────────────────────────────────
    BusyIndicator {
        anchors.centerIn: parent
        running: _view.loading
        visible: running
    }

    Label {
        anchors { bottom: parent.bottom; horizontalCenter: parent.horizontalCenter; margins: 6 }
        text: _view.loading ? "Loading…" : _view.url.toString()
        font.pixelSize: 11
        opacity: 0.6
    }
}
