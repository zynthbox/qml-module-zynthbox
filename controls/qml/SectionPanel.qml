import QtQuick 2.10
import QtQuick.Controls 2.2 as QQC2
import QtQuick.Layouts 1.4
import QtGraphicalEffects 1.15
import io.zynthbox.ui 1.0 as ZUI
import org.kde.kirigami 2.6 as Kirigami
import org.kde.plasma.core 2.0 as PlasmaCore

QQC2.Pane {
    id: root
    // topPadding: svgBg.visible ? svgBg.topPadding : ZUI.Theme.padding
    // bottomPadding: svgBg.visible ? svgBg.bottomPadding : ZUI.Theme.padding
    // leftPadding: svgBg.visible ? svgBg.leftPadding : ZUI.Theme.padding
    // rightPadding: svgBg.visible ? svgBg.rightPadding : ZUI.Theme.padding

    // background: Item {
    //     PlasmaCore.FrameSvgItem {
    //         // enabledBorders: PlasmaCore.FrameSvgItem.BottomBorder

    //         id: svgBg
    //         anchors.fill: parent

    //         readonly property real leftPadding: fixedMargins.left
    //         readonly property real rightPadding: fixedMargins.right
    //         readonly property real topPadding: fixedMargins.top
    //         readonly property real bottomPadding: fixedMargins.bottom

    //         visible: fromCurrentTheme
    //         imagePath: "widgets/section_panel"
    //         colorGroup: PlasmaCore.Theme.WindowColorGroup
    //     }
    // }

    padding: ZUI.Theme.sectionPadding
    topPadding: padding
    bottomPadding: padding
    rightPadding: padding
    leftPadding: padding
    
    background: Rectangle 
    {
        // color: "#2e3336"
        color: Kirigami.Theme.backgroundColor
        opacity: 0.4

        Image
        {
            anchors.fill: parent
            source: "qrc:/img/broken-noise.png"
            fillMode: Image.Tile
            // opacity: 0.7
        }
    }
}