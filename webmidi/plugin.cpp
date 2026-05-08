// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QtQml/QtQml>

#include "MidiTransport.h"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtWebEngine/QtWebEngine>
#include <QtWebEngineCore/QWebEngineUrlScheme>
#include <QtWebEngineWidgets/QWebEngineProfile>
#include <QtWebEngineWidgets/QWebEngineScript>
#include <QtWebEngineWidgets/QWebEngineScriptCollection>
#include <QFile>
#include <QUrl>
#include <QString>


static void registerUrlSchemes()
{
    for (const QByteArray &name : {"https", "http", "qrc"}) {
        QWebEngineUrlScheme scheme = QWebEngineUrlScheme::schemeByName(name);
        scheme.setFlags(scheme.flags()
                        | QWebEngineUrlScheme::ContentSecurityPolicyIgnored
                        | QWebEngineUrlScheme::CorsEnabled
                        | QWebEngineUrlScheme::SecureScheme);
        QWebEngineUrlScheme::registerScheme(scheme);
    }
}

// Inject scripts in order: qwebchannel → midi_prestub → jzz → jzz_webchannel
static void injectScripts(QWebEngineProfile *profile)
{
    auto addUrl = [&](const QString &name, const QString &qrcPath) {
        QFile f(qrcPath);
        if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning() << "[Viewer] Could not load script:" << qrcPath;
            return;
        }
        QWebEngineScript s;
        s.setName(name);
        s.setSourceCode(QString::fromUtf8(f.readAll()));
        s.setInjectionPoint(QWebEngineScript::DocumentCreation);
        s.setWorldId(QWebEngineScript::MainWorld);
        profile->scripts()->insert(s);
    };

    addUrl("qwebchannel", ":/audiocontrol-viewer/qwebchannel.js");

    // Clear browser's native requestMIDIAccess before JZZ loads so JZZ
    // skips its webmidi engine and waits for our custom implementation.
    QWebEngineScript prestub;
    prestub.setName("midi_prestub");
    prestub.setSourceCode(
        "(function(){"
        "  try { Object.defineProperty(navigator,'requestMIDIAccess',"
        "    {configurable:true,writable:true,value:undefined}); }"
        "  catch(e) { try { navigator.requestMIDIAccess=undefined; } catch(_){} }"
        "  console.log('[Viewer] browser requestMIDIAccess cleared');"
        "})();"
    );
    prestub.setInjectionPoint(QWebEngineScript::DocumentCreation);
    prestub.setWorldId(QWebEngineScript::MainWorld);
    profile->scripts()->insert(prestub);

    addUrl("jzz",            ":/audiocontrol-viewer/jzz.js");
    addUrl("jzz_webchannel", ":/audiocontrol-viewer/jzz_webchannel.js");
}

class WebMIDIPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void initializeEngine(QQmlEngine *engine, const char *uri) override
    {
        Q_UNUSED(engine)
        Q_UNUSED(uri)
        // Must be set before QGuiApplication
        qputenv("QTWEBENGINE_CHROMIUM_FLAGS",
            "--no-sandbox"
            " --disable-web-security"
            " --allow-running-insecure-content"
            " --ignore-certificate-errors"
            " --ignore-ssl-errors"
            " --ignore-urlfetcher-cert-requests");

        registerUrlSchemes();
        // QtWebEngine::initialize();
        injectScripts(QWebEngineProfile::defaultProfile());
    }

    void registerTypes(const char *uri)
    {
        Q_ASSERT(QLatin1String(uri) == QLatin1String("io.zynthbox.webmidi"));

        qmlRegisterSingletonType<MidiTransport>(uri, 1, 0, "MidiTransport", [](QQmlEngine *, QJSEngine *) -> QObject * {
            return new MidiTransport();
        });
    }
};

#include "plugin.moc"
