import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.6 as Kirigami
import org.kde.plasma.core 2.0 as PlasmaCore

import QtGraphicalEffects 1.15
import io.zynthbox.ui 1.0 as ZUI
import io.zynthbox.components 1.0 as Zynthbox


QQC2.Control {
    id: control
    property bool highlighted
    property bool checked

    property bool dragEnabled
    property bool doubleClickEnabled
    property bool clickAndHoldEnabled

    property bool isEmpty

    padding: ZUI.Theme.padding

    property double barValue : 0.0
    property double meterValue: 0.0

    property alias elide : _label.elide
    property alias color : _label.color
    property alias text : _label.text

    signal pressAndHold()
    signal clicked()
    signal doubleClicked()
    signal moved(double value)

    contentItem: Item {
        id: _content
         Loader { 
            anchors {
                fill: parent
                margins: ZUI.Theme.padding
            }
            active: !ZUI.Theme.altVolume
            visible: control.dragEnabled && active
            sourceComponent: Item {            
                Rectangle {
                    width: parent.width * control.barValue
                    anchors {
                        left: parent.left
                        top: parent.top
                        bottom: parent.bottom
                    }
                    radius: ZUI.Theme.radius
                    opacity: 0.8
                    color: Kirigami.Theme.highlightColor
                }
            }
        }

        ColumnLayout {
            anchors.fill: parent
            spacing: ZUI.Theme.spacing
            QQC2.Label {
                id: _label
                Layout.fillWidth: true
                Layout.fillHeight: true
                padding: 2
                background: Rectangle {
                    visible: ZUI.Theme.altVolume
                    opacity: control.isEmpty || control.checked  ? 0 : 1

                    color: control.isEmpty  ? "#181918" : "#1f2022"
                    radius: ZUI.Theme.radius
                }

                horizontalAlignment: Text.AlignLeft                
            }
            
            Loader { 
                Layout.fillWidth: true
                active: ZUI.Theme.altVolume
                visible: control.dragEnabled && active
                sourceComponent: Item {
                    implicitHeight: 6                            
                    Rectangle {
                        width: parent.width * control.barValue
                        height: parent.height
                        radius: ZUI.Theme.radius
                        opacity: 0.8
                        color: Kirigami.Theme.textColor
                    }
                }
            }

            Item {
                Layout.fillWidth: true
                implicitHeight: 1
                Rectangle {
                    height: parent.height
                    color: control.meterValue > 1 ? "red" : "white"
                    opacity: width > 1 ? 0.8 : 0
                    width: parent.width * control.meterValue
                }
            }
        }

        // Implement a double tap gesture
        // On released event, start the double tap timer if it is not already running
        // On pressed event, if the timer is already running then it means the 2nd tap was done within given time and hence a double tap event should be emitted
        // On pressed event, if the timer is not running then it means it is the first click. Dont do anything as released handler will start the double tap timer
        Timer {
            id: doublePressedTimer
            interval: zynqtgui.ui_settings.doubleClickThreshold
            repeat: false
            onTriggered: {
                if (!delegateMouseArea.dragHappened && !delegateMouseArea.holdHappened) {
                    control.clicked() 
                }
            }
        }

        Timer {
            id: dragHappenedResetTimer
            interval: 300
            repeat: false
            onTriggered: {
                delegateMouseArea.dragHappened = false
            }
        }

        MouseArea {
            id: delegateMouseArea
            property real initialMouseX
            property bool dragHappened: false
            property bool holdHappened: false

            anchors.fill: parent
            onPressed: {
                if (control.dragEnabled) {
                    delegateMouseArea.initialMouseX = mouse.x
                }
                delegateMouseArea.holdHappened = false    
            }
            onReleased: {
                if (control.dragEnabled) {
                    dragHappenedResetTimer.restart();
                }
                if (control.doubleClickEnabled) {
                    if (doublePressedTimer.running) {
                        doublePressedTimer.stop();
                        control.doubleClicked()
                    } else {
                        doublePressedTimer.restart();
                    }
                } else {
                    if(!delegateMouseArea.holdHappened)
                    {
                        control.clicked()
                    }
                    
                }
            }
            onMouseXChanged: {
                if (control.dragEnabled) {
                    var newVal = ZUI.CommonUtils.clamp(mouse.x / _content.width, 0, 1); 
                    if(mouse.x - delegateMouseArea.initialMouseX != 0) {
                        delegateMouseArea.dragHappened = true;
                        control.moved(newVal)   
                    }                                      
                }
            }
            onPressAndHold: {
                if (control.clickAndHoldEnabled && control.highlighted && !delegateMouseArea.dragHappened) {
                    control.pressAndHold()
                    delegateMouseArea.holdHappened = true;
                }
            }
        }
    }

    background: Item {
        id: backgroundItem
        
        // Show highlighted color on slot border when slot is a sample-loop and is enabled
        Rectangle {
            visible: !svgBg.visible
            anchors.fill: parent
            Kirigami.Theme.inherit: false
            Kirigami.Theme.colorSet: Kirigami.Theme.Button
            color: Kirigami.Theme.backgroundColor
            // If slot is a enabled clip or slot is selectedSlot, show highlighted border
            border.color: control.checked ? Kirigami.Theme.highlightColor : "#ff999999"
            border.width: 2
            radius: ZUI.Theme.radius
        }

        PlasmaCore.FrameSvgItem {
            id: svgBg
            anchors.fill: parent
            visible: fromCurrentTheme

            readonly property real leftPadding: margins.left
            readonly property real rightPadding: margins.right
            readonly property real topPadding: margins.top
            readonly property real bottomPadding: margins.bottom

            imagePath: "widgets/slots-delegate-background"
            prefix: control.checked ? ["focus", ""] : (control.isEmpty ? "inactive" : "")
            colorGroup: PlasmaCore.Theme.ButtonColorGroup
        }
    }
}