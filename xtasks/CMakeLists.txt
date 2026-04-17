# SPDX-FileCopyrightText: 2020 Camilo Higuita <milo.h@aol.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later

project(ZynthboxQmlModule_XTask VERSION 1.0.0 DESCRIPTION "A task manager for Zynthbox")
set(ZB_XTASK_URI io.zynthbox.xtask)

find_package(Qt${QT_MAJOR_VERSION} ${REQUIRED_QT_VERSION} REQUIRED COMPONENTS
    DBus)

find_package(KF${KF_MAJOR_VERSION} ${REQUIRED_KF_VERSION} REQUIRED COMPONENTS
    WindowSystem)

ecm_add_qml_module(${PROJECT_NAME}
    VERSION 1.0
    URI ${ZB_XTASK_URI}
    CLASS_NAME XTaskPlugin
    DEPENDENCIES
    "QtQuick.Controls"
    NO_PLUGIN_OPTIONAL)

target_sources(${PROJECT_NAME}
    PRIVATE
    plugin.cpp
    xtask.cpp
    xtask.h
    windowsmodel.cpp
    windowsmodel.h)

# ecm_target_qml_sources(ZynthboxQmlModule_MPRIS SOURCES
#     controls/PlayersView.qml)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    Qt${QT_MAJOR_VERSION}::Core
    Qt${QT_MAJOR_VERSION}::DBus
    KF5::ConfigCore
    KF5::WindowSystem
    PRIVATE
    Qt${QT_MAJOR_VERSION}::Widgets
    Qt${QT_MAJOR_VERSION}::QuickControls2)


ecm_finalize_qml_module(${PROJECT_NAME}
    DESTINATION ${KDE_INSTALL_QMLDIR})

ecm_generate_qmltypes(${ZB_XTASK_URI} 1.0 DESTINATION ${KDE_INSTALL_QMLDIR}/io/zynthbox/xtask)

