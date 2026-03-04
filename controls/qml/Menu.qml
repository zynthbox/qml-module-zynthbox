/* -*- coding: utf-8 -*-
******************************************************************************
ZYNTHIAN PROJECT: Zynthian Qt GUI

Zynthian Dialog Component

Copyright (C) 2023 Anupam Basak <anupam.basak27@gmail.com>

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
import org.kde.kirigami 2.4 as Kirigami

QQC2.Menu {
    id: root

    property var cuiaCallback: function(cuia, originId, track, slot, value) {
        let result = false;
        switch (cuia) {
            case "SWITCH_ARROW_DOWN_RELEASED":
            case "KNOB3_DOWN":
                root.currentIndex = Math.max(0, Math.min(root.currentIndex - 1, root.count - 1));
                result = true;
                break;
            case "SWITCH_ARROW_UP_RELEASED":
            case "KNOB3_UP":
                root.currentIndex = Math.max(0, Math.min(root.currentIndex + 1, root.count - 1));
                result = true;
                break;
            case "SWITCH_KNOB3_RELEASED":
            case "SWITCH_SELECT_RELEASED":
                let theItem = itemAt(root.currentIndex);
                if (theItem) {
                    if (theItem.action) {
                        theItem.action.onTriggered();
                    } else {
                        theItem.onClicked();
                    }
                } else {
                    let theAction = actionAt(root.currentIndex);
                    if (theAction) {
                        theAction.onTriggered();
                    }
                }
                result = true;
                break;
        }
        return result;
    }
    /** Handle opened changed to push/pop dialog to zynqtgui dialog stack
      * This will allow main program to pass CUIA events to the dialog stack
      *
      * Since this is a signal handler it is okay if one of the derived components
      * overrides the same signal. In that case both the handlers will be called
      */
    onOpenedChanged: {
        if (root.opened) {
            zynqtgui.pushDialog(root)
        } else {
            zynqtgui.popDialog(root)
        }
    }
}
