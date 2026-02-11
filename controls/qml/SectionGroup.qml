import QtQuick 2.10
import QtQuick.Controls 2.2 as QQC2
import QtQuick.Layouts 1.4
import QtGraphicalEffects 1.15
import io.zynthbox.ui 1.0 as ZUI
import org.kde.kirigami 2.6 as Kirigami
import org.kde.plasma.core 2.0 as PlasmaCore

QQC2.Pane {
    id: root

    property bool mask : false
    property string asset : "widgets/statusinfo_background"
    
    property Component fallbackBackground : null
    property int fallbackPadding: 0

    default property alias content : _content.data

// If there is not a section panel background, then do not add any padding
    topPadding: svgBg.visible ? svgBg.topPadding : fallbackPadding
    bottomPadding: svgBg.visible ? svgBg.bottomPadding : fallbackPadding
    leftPadding: svgBg.visible ? svgBg.leftPadding : fallbackPadding
    rightPadding: svgBg.visible ? svgBg.rightPadding : fallbackPadding
   
    contentItem: Item {
        id: _content
        layer.enabled: root.mask
        layer.effect: OpacityMask
        {
            maskSource: Rectangle
            {
                width: _content.width
                height: _content.height
                radius: ZUI.Theme.radius
            }
        }
    }

    background: Item
    {
        Loader {
            anchors.fill: parent
            active: !svgBg.visible
            sourceComponent: root.fallbackBackground
        }
    
        PlasmaCore.FrameSvgItem {
            id: svgBg
            visible: fromCurrentTheme
            anchors.fill: parent

            readonly property real leftPadding: margins.left
            readonly property real rightPadding: margins.right
            readonly property real topPadding: margins.top
            readonly property real bottomPadding: margins.bottom

            imagePath: "widgets/sectiongroup_background"
            colorGroup: PlasmaCore.Theme.ViewColorGroup
        }
    }
}