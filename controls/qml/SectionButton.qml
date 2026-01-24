/* -*- coding: utf-8 -*-
******************************************************************************
ZYNTHIAN PROJECT: Zynthian Qt GUI

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
import QtQuick.Controls 2.2 as QQC2
import QtQuick.Layouts 1.4
import io.zynthbox.ui 1.0 as ZUI
import org.kde.kirigami 2.6 as Kirigami
import org.kde.plasma.core 2.0 as PlasmaCore
import "private"  as Private

 QQC2.Button {

    id: root

    topPadding: _loader.ready ? _loader.item.topPadding : 4
    bottomPadding: _loader.ready ? _loader.item.bottomPadding : 4
    leftPadding:  _loader.ready ? _loader.item.leftPadding : 4
    rightPadding:  _loader.ready ? _loader.item.rightPadding : 4

    background: Loader
    {
        id: _loader
        property bool ready : status == Loader.Ready
        sourceComponent : svgBg.fromCurrentTheme ? _newButton : _oldButton
    }

    PlasmaCore.FrameSvgItem {
            id: svgBg
            visible: false          
            imagePath: "widgets/section_button"          
        }


    Component {
        id: _newButton

        PlasmaCore.FrameSvgItem {

            readonly property real leftPadding: margins.left
            readonly property real rightPadding: margins.right
            readonly property real topPadding: margins.top
            readonly property real bottomPadding: margins.bottom

            imagePath: "widgets/section_button"
            prefix: root.highlighted || root.checked ? ["focus", ""] : "normal"
            colorGroup: PlasmaCore.Theme.ButtonColorGroup
        }
    }

    Component {
        id: _oldButton

         Private.ButtonBackground {
            id: svgBg
            control: root

            readonly property real leftPadding: leftMargin
            readonly property real rightPadding: rightMargin
            readonly property real topPadding: topMargin
            readonly property real bottomPadding: bottomMargin
        }
    }


    // background: Rectangle {
    //     opacity: root.highlighted ? 0 : 1
    //     color: root.highlighted ? "#181918" : "#1f2022"
    //     radius: 2
    // }
}