# SPDX-FileCopyrightText: 2020 Camilo Higuita <milo.h@aol.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later
cmake_minimum_required(VERSION 3.16)

option(BUILD_WITH_QT5 "Use Qt 5" OFF)
option(BUILD_WITH_QT6 "Use Qt 6" OFF)

set(QT_MAJOR_VERSION 5)
set(KF_MAJOR_VERSION 5)

set(REQUIRED_QT_VERSION 5.15.2)
set(REQUIRED_KF_VERSION 5.95.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(ECM ${REQUIRED_KF_VERSION} NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

#DEFAULT COMPONENTS DEFINITIONS
include(GenerateExportHeader)
include(CMakePackageConfigHelpers)

include(ECMSetupVersion)
include(ECMGenerateHeaders)
include(ECMFindQmlModule)
include(ECMQmlModule)
include(ECMGenerateQmlTypes)
include(ECMSourceVersionControl)

include(KDEClangFormat)
include(KDEInstallDirs)
include(KDECMakeSettings)

project(ZynthboxQmlModule_WebMIDI VERSION 1.0.0 DESCRIPTION "A web MIDI module for Zynthbox")
set(ZB_WEBMIDI_URI io.zynthbox.webmidi)

find_package(Qt${QT_MAJOR_VERSION} ${REQUIRED_QT_VERSION} REQUIRED COMPONENTS
    Core
    Qml
    Quick
    QuickControls2
    WebEngine
    WebEngineCore
    WebEngineWidgets
    WebChannel)

find_package(PkgConfig REQUIRED)
pkg_check_modules(ALSA REQUIRED alsa)

ecm_add_qml_module(${PROJECT_NAME}
    VERSION 1.0
    URI ${ZB_WEBMIDI_URI}
    CLASS_NAME WebMIDIPlugin
    DEPENDENCIES
    "QtQuick.Controls"
    NO_PLUGIN_OPTIONAL)

target_sources(${PROJECT_NAME}
    PRIVATE
    plugin.cpp
    MidiTransport.cpp
    MidiTransport.h
    app.qrc)

# add_executable(webmidi-test
#     main.cpp)

# target_link_libraries(webmidi-test
#     PRIVATE
#     ${PROJECT_NAME}
#     Qt${QT_MAJOR_VERSION}::Core
#     Qt${QT_MAJOR_VERSION}::WebEngine
#     Qt${QT_MAJOR_VERSION}::WebEngineCore)

# target_include_directories(webmidi-test
#     PRIVATE
#     ${CMAKE_CURRENT_SOURCE_DIR})

ecm_target_qml_sources(${PROJECT_NAME} SOURCES
    WebView.qml)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    Qt${QT_MAJOR_VERSION}::Core
    PRIVATE
    Qt${QT_MAJOR_VERSION}::QuickControls2
    Qt5::WebEngine
    Qt5::WebEngineCore
    Qt5::WebEngineWidgets
    Qt5::WebChannel
    ${ALSA_LIBRARIES})


ecm_finalize_qml_module(${PROJECT_NAME}
    DESTINATION ${KDE_INSTALL_QMLDIR})

ecm_generate_qmltypes(${ZB_WEBMIDI_URI} 1.0 DESTINATION ${KDE_INSTALL_QMLDIR}/io/zynthbox/webmidi)

