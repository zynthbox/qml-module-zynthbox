/*
 *   Copyright 2018 Camilo Higuita <milo.h@aol.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "zynthbox.h"
#include <QCoreApplication>
#include <QDebug>
#include <QQmlContext>

#include "theme.h"


ZynthboxKit::ZynthboxKit(QObject *parent) : QQmlExtensionPlugin(parent)
{   
}

QUrl ZynthboxKit::componentUrl(const QString &fileName) const
{
    return QUrl(resolveFileUrl(fileName));
}

void ZynthboxKit::initializeEngine(QQmlEngine* engine, const char* uri)
{
    Q_UNUSED(uri);
}

void ZynthboxKit::registerTypes(const char *uri)
{    
    qDebug() << "REGISTER ZYNTHBOX KIT TYPES <<<<<<<<<<<<<<<<<<<<<<";

    Q_ASSERT(QLatin1String(uri) == QLatin1String("io.zynthbox.ui2"));
    
    qmlRegisterType(componentUrl(QStringLiteral("Button.qml")), uri, 1, 0, "Button");
    qmlRegisterType(componentUrl(QStringLiteral("Popup.qml")), uri, 1, 0, "Popup");
    qmlRegisterUncreatableType<Theme>(uri, 1, 0, "Theme", QStringLiteral("Cannot create objects of type Theme, use it as an attached property"));

    // qmlProtectModule(uri, 3);
}

// #include "mauikit.moc"
// #include "moc_mauikit.cpp"
