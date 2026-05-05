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

import QtGraphicalEffects 1.0
import QtQuick 2.10
import QtQuick.Controls 2.4 as QQC2
import QtQuick.Layouts 1.4
import io.zynthbox.mpris 1.0 as MPRIS
import io.zynthbox.ui 1.0 as ZUI
import org.kde.kirigami 2.6 as Kirigami
import org.kde.plasma.core 2.0 as PlasmaCore
import QtQuick.Particles 2.15

Item {
    id: control

    readonly property alias count   : _listView.count
    readonly property alias players : _mpris.players
    readonly property QtObject currentPlayer: _listView.currentItem ? _listView.currentItem.player : null
    signal recordClicked(int pid)
    property bool isRecording: false
    MPRIS.Mpris {
        id: _mpris
    }

    function currentPlayer() {
        if (_listView.currentIndex < 0)
            return null;

        return _listView.currentItem.player;
    }

    ListView {
        id: _listView
        clip: true
        anchors.fill: parent
        orientation: ListView.Horizontal
        interactive: true
        // Snaps the view so only one item at a time is flicked through
        snapMode: ListView.SnapOneItem
        // Optional: Strictly aligns the current item in the view
        highlightRangeMode: ListView.StrictlyEnforceRange
        model: _mpris.players

        delegate: QQC2.Pane {
            id: _card

            property MPRIS.MprisPlayer player: model.player
            readonly property bool isCurrent: ListView.isCurrentItem
            readonly property bool isPlaying: state === "Playing"
            property string state: player ? player.status : ""
            readonly property var currentMetadata: player ? player.metadata : undefined
            readonly property string albumArt: currentMetadata ? currentMetadata["mpris:artUrl"] || "" : ""
            readonly property string track: {
                if (!currentMetadata)
                    return "";

                var xesamTitle = currentMetadata["xesam:title"];
                if (xesamTitle)
                    return xesamTitle;

                // if no track title is given, print out the file name
                var xesamUrl = currentMetadata["xesam:url"] ? currentMetadata["xesam:url"].toString() : "";
                if (!xesamUrl)
                    return "";

                var lastSlashPos = xesamUrl.lastIndexOf('/');
                if (lastSlashPos < 0)
                    return "";

                var lastUrlPart = xesamUrl.substring(lastSlashPos + 1);
                return decodeURIComponent(lastUrlPart);
            }
            property string artist: {
                if (!currentMetadata)
                    return "";

                var xesamArtist = currentMetadata["xesam:artist"];
                if (!xesamArtist)
                    return "";

                if (typeof xesamArtist == "string")
                    return xesamArtist;
                else
                    return xesamArtist.join(", ");
            }
            readonly property bool canControl: (_card.player && _card.player.capabilities & MPRIS.MprisPlayer.CanControl) || false
            readonly property bool canGoPrevious: (_card.player && _card.player.capabilities & MPRIS.MprisPlayer.CanGoPrevious) || false
            readonly property bool canGoNext: (_card.player && _card.player.capabilities & MPRIS.MprisPlayer.CanGoNext) || false
            readonly property bool canPlay: _card.player ? (_card.player.capabilities & MPRIS.MprisPlayer.CanPlay) : false
            readonly property bool canPause: _card.player ? (_card.player.capabilities & MPRIS.MprisPlayer.CanPause) : false
            readonly property bool canStop: _card.player ? (_card.player.capabilities & MPRIS.MprisPlayer.CanStop) : false
            property double position: (_card.player && _card.player.position) || 0
            readonly property real rate: (_card.player && _card.player.rate) || 1
            readonly property int length: currentMetadata ? currentMetadata["mpris:length"] || 0 : 0
            readonly property bool canSeek: (_card.player && _card.player.capabilities & Cask.MprisPlayer.CanSeek) || false
            readonly property bool noPlayer: _card.player
            readonly property string identity: !_card.noPlayer ? player.identity || _card.sourceName : ""
            property string sourceName: player ? player.serviceName : ""
            property string playerName: player ? player.identity : ""
            property string playerIcon: player ? player.iconName : ""
            property bool disablePositionUpdate: false

            function togglePlaying() {
                if (_card.isPlaying) {
                    if (_card.canPause)
                        _card.action_pause();

                } else {
                    if (_card.canPlay)
                        _card.action_play();

                }
            }

            function action_open() {
                _card.player.raise();
            }

            function action_quit() {
                _card.player.quit();
            }

            function action_play() {
                _card.player.play();
            }

            function action_pause() {
                _card.player.pause();
            }

            function action_playPause() {
                _card.player.playPause();
            }

            function action_previous() {
                _card.player.previous();
            }

            function action_next() {
                _card.player.next();
            }

            function action_stop() {
                _card.player.stop();
            }

            height: ListView.view.height
            width: ListView.view.width
            onPositionChanged: {
                // we don't want to interrupt the user dragging the slider
                if (!seekSlider.pressed) {
                    // we also don't want passive position updates
                    disablePositionUpdate = true;
                    seekSlider.value = position;
                    disablePositionUpdate = false;
                }
            }
            onLengthChanged: {
                disablePositionUpdate = true;
                // When reducing maximumValue, value is clamped to it, however
                // when increasing it again it gets its old value back.
                // To keep us from seeking to the end of the track when moving
                // to a new track, we'll reset the value to zero and ask for the position again
                seekSlider.value = 0;
                seekSlider.to = _card.length;
                _card.player.updatePosition();
                disablePositionUpdate = false;
            }
            Component.onCompleted: {
                _card.player.updatePosition();
            }

            Timer {
                id: seekTimer

                interval: 1000 / _card.rate
                repeat: true
                running: _card.isPlaying && interval > 0 && seekSlider.to >= 1e+06
                onTriggered: {
                    // some players don't continuously update the seek slider position via mpris
                    // add one second; value in microseconds
                    if (!seekSlider.pressed) {
                        disablePositionUpdate = true;
                        if (seekSlider.value == seekSlider.to)
                            _card.player.updatePosition();
                        else
                            seekSlider.value += 1e+06;
                        disablePositionUpdate = false;
                    }
                }
            }

            contentItem: RowLayout {
                spacing: ZUI.Theme.sectionSpacing
                Item {
                    Layout.minimumWidth: 100
                    Layout.maximumWidth: 500
                    Layout.preferredWidth: 250
                    // Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.margins: Kirigami.Units.gridUnit

                    ColumnLayout {
                        anchors.fill: parent
                        spacing: ZUI.Theme.sectionSpacing

                        Item {
                            Layout.fillWidth: true
                            Layout.fillHeight: true

                            Item {
                                anchors.fill: parent
                                visible:  control.isRecording && _card.isPlaying && _card.isCurrent
                                opacity: visible ? 1 : 0
                                Behavior on opacity {
                                    NumberAnimation {
                                        duration: 250
                                        easing.type: Easing.InOutQuad
                                    }
                                }

                                ParticleSystem {
                                    id: sys

                                    running: parent.visible
                                }

                                Emitter {
                                    id: emitter

                                    anchors.centerIn: parent
                                    system: sys
                                    emitRate: 120 // Particles per second
                                    lifeSpan: 2000 // Lasts 2 seconds
                                    lifeSpanVariation: 500
                                    size: 200
                                    sizeVariation: 40

                                    velocity: AngleDirection {
                                        angleVariation: 360
                                        magnitude: 15
                                    }

                                    // velocity: TargetDirection {
                                    //     targetItem: _targetRecord
                                    //     magnitude: control.isRecording ? 200 : 100 // Speed of particles
                                    // }

                                }

                                ImageParticle {
                                    system: sys
                                    source: "qrc:///particleresources/glowdot.png" // Uses default Qt resource
                                    color: Qt.lighter("#e500a4", 2)
                                    redVariation: 0 // Becomes more "white" as red and green increase
                                    greenVariation: 0.5
                                    blueVariation: 0.2 // Keeps some blue dominance
                                }
                            }

                            // Attractor {
                            //     system: sys
                            //     pointX: _targetRecord.x + _targetRecord.width/2
                            //     pointY: _targetRecord.y + _targetRecord.height/2
                            //     strength: 1.0  // Attraction force
                            //     affectedParameter: Attractor.Position // Directly moves the particle coordinate
                            //     proportionalToDistance: Attractor.Constant // Uniform pull strength
                            // }

                            Rectangle {
                                id: _recordArt
                                anchors.centerIn: parent
                                height: 200
                                width: 200 
                                color: "white"
                                radius: _card.isPlaying ? width / 2 : 6
                                Behavior on radius {
                                            NumberAnimation {
                                                duration: 150
                                                easing.type: Easing.OutInQuad
                                            }
                                        }



                                layer.effect: DropShadow {
                                    color: "black"
                                    radius: 8
                                    samples: 16
                                    horizontalOffset: 0
                                    verticalOffset: 4
                                }

                                RotationAnimation {
                                    target: _img
                                    property: "rotation"
                                    from: 0
                                    to: 360
                                    duration: 3000
                                    loops: Animation.Infinite
                                    running: _card.isPlaying
                                    easing.type: Easing.Linear
                                    onStopped: _img.rotation = 0
                                }

                                Image {
                                    id: _img
                                    width: parent.width -10
                                    sourceSize.width: width
                                    fillMode: Image.PreserveAspectFit
                                    anchors.centerIn: parent
                                    source: _card.albumArt
                                    layer.enabled: true
                                    layer.effect: OpacityMask {
                                        maskSource: Rectangle {
                                            width: _img.paintedWidth
                                            height: _img.paintedHeight
                                            radius: _card.isPlaying ? width / 2 : 6
                                            color: "white"
                                            Behavior on radius {
                                                NumberAnimation {
                                                    duration: 150
                                                    easing.type: Easing.OutInQuad
                                                }
                                            }
                                        }
                                    }
                                }
                            }

                            // Rectangle {
                            //     id: _targetRecord

                            //     width: 64
                            //     height: 64
                            //     anchors.horizontalCenter: _recordArt.right
                            //     anchors.verticalCenter: _recordArt.bottom
                                
                            //     radius: 32
                            //     color: "white"
                            //     layer.effect: DropShadow {
                            //         color: "black"
                            //         radius: 8           
                            //         samples: 16
                            //         horizontalOffset: 0
                            //         verticalOffset: 4
                            //     }

                            //     RotationAnimation {
                            //         target: _targetRecord
                            //         property: "rotation"
                            //         from: 0
                            //         to: 360
                            //         duration: 3000
                            //         loops: Animation.Infinite
                            //         running: _card.isPlaying && control.isRecording
                            //         easing.type: Easing.Linear
                            //         onStopped: _targetRecord.rotation = 0
                            //     }
                            // }

                            Kirigami.Icon {
                                width: 64
                                height: 64
                                anchors.horizontalCenter: _recordArt.right
                                anchors.verticalCenter: _recordArt.bottom
                                source: _card.playerIcon
                            }
                        }

                        
                    }

                    //  Text {
                    //         anchors.bottom: parent.bottom
                    //         width: parent.width
                    //         text: _card.player.pid + " / " + _card.player.serviceName
                    //         color: "red"
                    //     }

                }

                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    Column {
                        width: parent.width
                        anchors.verticalCenter: parent.verticalCenter
                        spacing: ZUI.Theme.sectionSpacing

                        QQC2.Label {
                            text: _card.track
                            font.pointSize: 20
                            font.bold: true
                        }

                        QQC2.Label {
                            text: _card.artist
                            font.pointSize: 16
                        }

                        QQC2.Label {
                            text: _card.playerName
                            font.pointSize: 12
                        }

                        QQC2.Slider {
                            id: seekSlider

                            orientation: Qt.Horizontal
                            enabled: _card.canSeek
                            implicitHeight: visible ? 22 : 0
                            from: 0
                            value: 0
                            to: _card.length
                            onMoved: {
                                // delay setting the position to avoid race conditions

                                if (!_card.disablePositionUpdate)
                                    queuedPositionUpdate.restart();

                            }

                            Timer {
                                id: queuedPositionUpdate

                                interval: 100
                                onTriggered: {
                                    if (_card.position == seekSlider.value)
                                        return ;

                                    _card.player.setPosition(seekSlider.value);
                                }
                            }

                        }
                        
                        ZUI.SectionGroup {
                            Layout.alignment: Qt.AlignHCenter
                            implicitWidth: (Kirigami.Units.gridUnit * 4) * 4
                            implicitHeight: (Kirigami.Units.gridUnit * 2) * 2
                            // implicitWidth: _buttonsRow.implicitWidth + leftPadding + rightPadding
                            // implicitHeight: _buttonsRow.implicitHeight + topPadding + bottomPadding


                            GridLayout {
                                id: _buttonsRow
                                rowSpacing: ZUI.Theme.spacing
                                columnSpacing: ZUI.Theme.spacing
                                anchors.fill: parent
                                rows: 2
                                columns: 4
                                
                                ZUI.SectionButton{
                                    Layout.row: 0
                                    Layout.column: 0
                                    icon.name: "media-skip-backward"
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                    enabled: _card.canGoPrevious
                                    onClicked: {
                                        seekSlider.value = 0; // Let the media start from beginning. Bug 362473
                                        _card.action_previous();
                                    }
                                }

                                ZUI.SectionButton {
                                    Layout.row: 0
                                    Layout.column: 1
                                    enabled: _card.canStop
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                    icon.name: "media-playback-stop"
                                    onClicked: _card.action_stop()
                                }

                                ZUI.SectionButton {
                                    Layout.row: 0
                                    Layout.column: 2
                                    //                        text: qsTr("Play and pause")
                                    enabled: _card.canPlay
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                    icon.name: _card.isPlaying ? "media-playback-pause" : "media-playback-start"
                                    onClicked: _card.togglePlaying()
                                }

                                ZUI.SectionButton {
                                    Layout.row: 0
                                    Layout.column: 3
                                    icon.name: "media-skip-forward"
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                    enabled: _card.canGoNext
                                    onClicked: {
                                        seekSlider.value = 0; // Let the media start from beginning. Bug 362473
                                        _card.action_next();
                                    }
                                }

                                ZUI.SectionButton {
                                    id: _recordButton
                                    Layout.row: 1
                                    Layout.column: 0
                                    Layout.columnSpan: 4
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                    text: control.isRecording ? "Stop Recording" : "Record"
                                    
                                    onClicked: control.recordClicked(_card.player.pid)
                                }

                            }

                    }

                    }

                }

            }

        }

    }

    Text {
        color: "orange"
        text: "Foudn players:" + _mpris.players.count
    }

}
