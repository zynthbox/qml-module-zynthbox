# SPDX-FileCopyrightText: 2020 Camilo Higuita <milo.h@aol.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later

project(ZynthboxQmlModule_Rec VERSION 1.0.0 DESCRIPTION "A recorder manager for Zynthbox")
set(ZB_REC_URI io.zynthbox.rec)

find_package(Qt${QT_MAJOR_VERSION} ${REQUIRED_QT_VERSION} REQUIRED COMPONENTS
    Core
    DBus
    Qml
    Quick
    QuickControls2
    Svg
    Widgets)

find_package(Qt5 REQUIRED COMPONENTS X11Extras)

find_program(PW_DUMP_EXECUTABLE pw-dump REQUIRED)
message(STATUS "Found pw-dump: ${PW_DUMP_EXECUTABLE}")

find_program(PW_RECORD_EXECUTABLE pw-record REQUIRED)
message(STATUS "Found pw-record: ${PW_RECORD_EXECUTABLE}")

find_package(KF${KF_MAJOR_VERSION} ${REQUIRED_KF_VERSION} REQUIRED COMPONENTS
    IconThemes
    Config
    WindowSystem)

ecm_add_qml_module(${PROJECT_NAME}
    VERSION 1.0
    URI ${ZB_REC_URI}
    CLASS_NAME RecorderPlugin
    DEPENDENCIES
    "QtQuick.Controls"
    NO_PLUGIN_OPTIONAL)

target_sources(${PROJECT_NAME}
    PRIVATE
    plugin.cpp
    recorder.cpp
    recorder.h)

add_executable(recorder-test
    main.cpp)

target_link_libraries(recorder-test
    PRIVATE
    ${PROJECT_NAME}
    Qt${QT_MAJOR_VERSION}::Core
    Qt${QT_MAJOR_VERSION}::DBus
    Qt5::X11Extras
    KF5::ConfigCore
    KF5::IconThemes
    KF5::WindowSystem)

target_include_directories(recorder-test
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR})

# ecm_target_qml_sources(ZynthboxQmlModule_MPRIS SOURCES
#     controls/PlayersView.qml)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    Qt${QT_MAJOR_VERSION}::Core
    Qt${QT_MAJOR_VERSION}::DBus
    Qt5::X11Extras
    KF5::ConfigCore
    KF5::IconThemes
    KF5::WindowSystem
    PRIVATE
    Qt${QT_MAJOR_VERSION}::Widgets
    Qt${QT_MAJOR_VERSION}::QuickControls2)


ecm_finalize_qml_module(${PROJECT_NAME}
    DESTINATION ${KDE_INSTALL_QMLDIR})

ecm_generate_qmltypes(${ZB_REC_URI} 1.0 DESTINATION ${KDE_INSTALL_QMLDIR}/io/zynthbox/rec)

