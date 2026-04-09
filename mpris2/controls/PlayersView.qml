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
import QtGraphicalEffects 1.0

import io.zynthbox.ui 1.0 as ZUI
import io.zynthbox.mpris 1.0 as MPRIS

Item {
    id: control
    MPRIS.Mpris {
        id: _mpris
    }


    ListView {
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
            height: ListView.view.height
            width: ListView.view.width    

            property MPRIS.MprisPlayer player : model.player
            readonly property bool isPlaying : state === "Playing"
            property string state : player ? player.status : ""

            readonly property var currentMetadata : player ? player.metadata : undefined

            readonly property string albumArt: currentMetadata ? currentMetadata["mpris:artUrl"] || "" : ""

            readonly property string track: {
                if (!currentMetadata) {
                    return ""
                }
                var xesamTitle = currentMetadata["xesam:title"]
                if (xesamTitle) {
                    return xesamTitle
                }
                // if no track title is given, print out the file name
                var xesamUrl = currentMetadata["xesam:url"] ? currentMetadata["xesam:url"].toString() : ""
                if (!xesamUrl) {
                    return ""
                }
                var lastSlashPos = xesamUrl.lastIndexOf('/')
                if (lastSlashPos < 0) {
                    return ""
                }
                var lastUrlPart = xesamUrl.substring(lastSlashPos + 1)
                return decodeURIComponent(lastUrlPart)
            }

             property string artist: {
                if (!currentMetadata) {
                    return ""
                }
                var xesamArtist = currentMetadata["xesam:artist"]
                if (!xesamArtist) {
                    return "";
                }

                if (typeof xesamArtist == "string") {
                    return xesamArtist
                } else {
                    return xesamArtist.join(", ")
                }
            }

            readonly property bool canControl: (_card.player && _card.player.capabilities & MPRIS.MprisPlayer.CanControl) || false

            readonly property bool canGoPrevious: (_card.player && _card.player.capabilities & MPRIS.MprisPlayer.CanGoPrevious) || false

            readonly property bool canGoNext:(_card.player && _card.player.capabilities & MPRIS.MprisPlayer.CanGoNext) || false

            readonly property bool canPlay: _card.player ? (_card.player.capabilities & MPRIS.MprisPlayer.CanPlay) : false

            readonly property bool canPause: _card.player ? (_card.player.capabilities & MPRIS.MprisPlayer.CanPause) : false

            
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

            onPositionChanged:
            {
                // we don't want to interrupt the user dragging the slider
                if (!seekSlider.pressed)
                {
                    // we also don't want passive position updates
                    disablePositionUpdate = true
                    seekSlider.value = position
                    disablePositionUpdate = false
                }
            }

            onLengthChanged:
            {
                disablePositionUpdate = true
                // When reducing maximumValue, value is clamped to it, however
                // when increasing it again it gets its old value back.
                // To keep us from seeking to the end of the track when moving
                // to a new track, we'll reset the value to zero and ask for the position again
                seekSlider.value = 0
                seekSlider.to = _card.length
                _card.player.updatePosition()
                disablePositionUpdate = false
            }

            Component.onCompleted:
            {
                _card.player.updatePosition();
            }

            Timer
            {
                id: seekTimer
                interval: 1000 / _card.rate
                repeat: true
                running: _card.isPlaying && interval > 0 && seekSlider.to >= 1000000
                onTriggered:
                {
                    // some players don't continuously update the seek slider position via mpris
                    // add one second; value in microseconds
                    if (!seekSlider.pressed)
                    {
                        disablePositionUpdate = true
                        if (seekSlider.value == seekSlider.to)
                        {
                            _card.player.updatePosition();
                        } else
                        {
                            seekSlider.value += 1000000
                        }
                        disablePositionUpdate = false
                    }
                }
            }


            contentItem: RowLayout {
                Item {
                    Layout.minimumWidth: 100
                    Layout.maximumWidth: 500
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.alignment: Qt.AlignCenter

                    Image {
                        width: parent.width
                        sourceSize.width: width
                        fillMode: Image.PreserveAspectFit
                        anchors.centerIn: parent
                        source: _card.albumArt
                    }
                }

                Column {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    QQC2.Label {
                        text: _card.track
                    }

                    QQC2.Label {
                        text: _card.artist
                    }

                    QQC2.Label {
                        text: _card.playerName

                    }

                    QQC2.Label {
                        text: _card.position
                    }

                    QQC2.Slider {
                        id: seekSlider
                        orientation: Qt.Horizontal
                        enabled: _card.canSeek
                        Layout.fillWidth: true
                        implicitHeight: visible ? 22 : 0
                        from: 0
                        value: 0
                        to: _card.length

                        onMoved:
                        {
                            if (!_card.disablePositionUpdate)
                            {
                                // delay setting the position to avoid race conditions
                                queuedPositionUpdate.restart()
                            }
                        }

                        Timer
                        {
                            id: queuedPositionUpdate
                            interval: 100
                            onTriggered:
                            {
                                if (_card.position == seekSlider.value) {
                                    return;
                                }

                                _card.player.setPosition(seekSlider.value)
                            }
                        }
                    }
                }
            }
        }
        
    }

    Text {
        color: "orange"
        text: "Foudn players:" +  _mpris.players.count
    } 
}