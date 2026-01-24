/* -*- coding: utf-8 -*-
******************************************************************************
ZYNTHIAN PROJECT: Zynthian Qt GUI

Zynthian Loopgrid Page

Copyright (C) 2021 Anupam Basak <anupam.basak27@gmail.com>

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
import QtQuick.Controls 2.2 as QQC2
import QtQml.Models 2.10
import org.kde.kirigami 2.4 as Kirigami
import org.kde.plasma.core 2.0 as PlasmaCore
import io.zynthbox.ui 1.0 as ZUI

ZUI.CellControl {
    id: root
    Kirigami.Theme.inherit: false
    Kirigami.Theme.colorSet: Kirigami.Theme.Button

    property alias subText: contents.text2
    property alias subSubText: contents.text3

    property alias textSize: contents.text1Size
    property alias subTextSize: contents.text2Size
    property alias subSubTextSize: contents.text3Size

    highlighted: root.highlightOnFocus && root.activeFocus
    property bool highlightOnFocus: true
    property bool active: true

    contentItem: Item {
        ZUI.TableHeaderLabel {
            id: contents
            anchors.fill: parent
            text: root.text
            opacity: root.active ? 1 : 0.3
        }
    }

    onPressed: root.forceActiveFocus()

}
