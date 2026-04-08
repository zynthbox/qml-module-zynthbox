# SPDX-FileCopyrightText: 2020 Camilo Higuita <milo.h@aol.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later

set(ZB_MRPIS_URI io.zynthbox.mpris)

find_package(Qt${QT_MAJOR_VERSION} ${REQUIRED_QT_VERSION} REQUIRED COMPONENTS
    DBus)

ecm_add_qml_module(ZynthboxQmlModule_MPRIS
    VERSION 1.0
    URI ${ZB_MRPIS_URI}
    CLASS_NAME ZynthboxKitMRPIS
    DEPENDENCIES
    "QtQuick.Controls"
    NO_PLUGIN_OPTIONAL)

set(mpris2_SRCS
mpris2engine.cpp
mpris2engine.h
mpris2player.cpp
mpris2player.h
playersmodel.cpp)

set_source_files_properties(org.freedesktop.DBus.Properties.xml PROPERTIES NO_NAMESPACE true)
qt_add_dbus_interface(mpris2_SRCS org.freedesktop.DBus.Properties.xml properties_interface)

set_source_files_properties(org.mpris.MediaPlayer2.Player.xml PROPERTIES NO_NAMESPACE true)
qt_add_dbus_interface(mpris2_SRCS org.mpris.MediaPlayer2.Player.xml player_interface)

set_source_files_properties(org.mpris.MediaPlayer2.xml PROPERTIES NO_NAMESPACE true)
qt_add_dbus_interface(mpris2_SRCS org.mpris.MediaPlayer2.xml mediaplayer2_interface)

target_sources(ZynthboxQmlModule_MPRIS
    PRIVATE
    plugin.cpp
    ${mpris2_SRCS})

ecm_target_qml_sources(ZynthboxQmlModule_MPRIS SOURCES
    controls/PlayersView.qml)

target_link_libraries(ZynthboxQmlModule_MPRIS
    PUBLIC
    Qt${QT_MAJOR_VERSION}::Core
    Qt${QT_MAJOR_VERSION}::DBus
    KF5::ConfigCore
    PRIVATE
    Qt${QT_MAJOR_VERSION}::Widgets
    Qt${QT_MAJOR_VERSION}::QuickControls2)


ecm_finalize_qml_module(ZynthboxQmlModule_MPRIS
    DESTINATION ${KDE_INSTALL_QMLDIR})

ecm_generate_qmltypes(${ZB_MRPIS_URI} 1.0 DESTINATION ${KDE_INSTALL_QMLDIR}/io/zynthbox/mpris)

