/* -*- coding: utf-8 -*-
******************************************************************************
ZYNTHIAN PROJECT: Zynthian Qt GUI

Zynthian slots bar page

Copyright (C) 2021 Anupam Basak <anupam.basak27@gmail.com>
Copyright (C) 2021 Marco MArtin <mart@kde.org>
Copyright (C) 2026 Camilo Higuita <milo.h@aol.com>

******************************************************************************

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of
the License, or any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

For a full copy of the GNU General Public License see the LICENSE.txt file.

******************************************************************************
*/

import QtQuick 2.10
import QtQuick.Layouts 1.4
import QtQuick.Controls 2.4 as QQC2
import org.kde.kirigami 2.6 as Kirigami
import org.kde.plasma.core 2.0 as PlasmaCore

import io.zynthbox.ui 1.0 as ZUI

QQC2.Pane {
    id: control

    padding: 0
    background: null

    property bool altTabs : ZUI.Theme.altTabs
    // property alias spacing: _layout.columnSpacing

    default property alias middleTab : _middleTab.data
    property alias leftTab : _leftTab.data
    property alias rightTab : _rightTab.data

    property int sideTabsWidth: Kirigami.Units.gridUnit * 6

    contentItem: GridLayout {
        id: _layout

        columnSpacing: ZUI.Theme.sectionSpacing
        rowSpacing: 0

        rows: 1
        columns: 3

        Item {            
            Layout.fillWidth: false
            Layout.fillHeight: true
            Layout.minimumWidth: control.sideTabsWidth
            Layout.maximumWidth: control.sideTabsWidth
            Layout.preferredWidth: control.sideTabsWidth
            Layout.column: control.altTabs ? 2: 0
            StackLayout {
                id: _leftTab
                anchors.fill: parent
            }
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.column: 1
            StackLayout {
                id: _middleTab
                anchors.fill: parent
            }
        }

        Item {
            Layout.fillWidth: false
            Layout.fillHeight: true
            Layout.minimumWidth: control.sideTabsWidth
            Layout.maximumWidth: control.sideTabsWidth
            Layout.preferredWidth: control.sideTabsWidth
            Layout.column: control.altTabs ? 0: 2
            StackLayout {
                id: _rightTab
                anchors.fill: parent
            }
        }
    }
}