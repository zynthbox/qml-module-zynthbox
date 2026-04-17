// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QtQml/QtQml>
#include "xtask.h"
#include "windowsmodel.h"

class XTaskPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")
public:
    void registerTypes(const char *uri)
    {
        Q_ASSERT(QLatin1String(uri) == QLatin1String("io.zynthbox.xtask"));

        // // @uri Liri.Mpris
        qmlRegisterSingletonType<XTask>(uri, 1, 0, "XTask", [](QQmlEngine *, QJSEngine *) -> QObject * {
            return new XTask();
        });
        qmlRegisterUncreatableType<WindowsModel>(uri, 1, 0, "WindowsModel",
                                         QStringLiteral("Cannot create WindowsModel object"));

        // qmlRegisterType<Mpris2Engine>(uri, 1, 0, "Mpris");
        // qmlRegisterUncreatableType<Mpris2Player>(uri, 1, 0, "MprisPlayer",
        //                                          QStringLiteral("Cannot create MprisPlayer object"));
        // qmlRegisterUncreatableType<PlayersModel>(uri, 1, 0, "PlayersModel",
        //                                          QStringLiteral("Cannot create PlayersModel object"));
    }
};

#include "plugin.moc"
