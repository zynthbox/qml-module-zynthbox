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

QQC2.AbstractButton {
    id: root
    padding: 1

    property color backgroundColor : Kirigami.Theme.backgroundColor
    property color highlightColor: Kirigami.Theme.highlightColor
    property bool highlighted: false

    background: Item {
        Rectangle {
            anchors.fill: parent
            visible: !svgBg.visible
            border.width: root.highlighted ? 1 : 0
            border.color: root.highlightColor

            color: root.backgroundColor
        }

        PlasmaCore.FrameSvgItem {
            id: svgBg
            visible: fromCurrentTheme
            anchors.fill: parent

            readonly property real leftPadding: fixedMargins.left
            readonly property real rightPadding: fixedMargins.right
            readonly property real topPadding: fixedMargins.top
            readonly property real bottomPadding: fixedMargins.bottom

            imagePath: "widgets/column-delegate-background"
            prefix: root.highlighted ? ["focus", ""] : ""
            colorGroup: PlasmaCore.Theme.ButtonColorGroup
        }
    }

    // background: Rectangle {
    //     property bool highlighted: ((root.highlightOnFocus && root.activeFocus) || root.highlighted)
    //     opacity: highlighted ? 0.2 : 1
    //     color: "#1f2022"
    //     radius: ZUI.Theme.radius
    // }
}