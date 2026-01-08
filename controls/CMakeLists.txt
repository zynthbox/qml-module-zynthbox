# SPDX-FileCopyrightText: 2020 Camilo Higuita <milo.h@aol.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/libs)

set(PLASMA_RELATIVE_DATA_INSTALL_DIR "plasma")
set(PLASMA_DATA_INSTALL_DIR "${KDE_INSTALL_DATADIR}/${PLASMA_RELATIVE_DATA_INSTALL_DIR}")

find_package(Qt5 REQUIRED COMPONENTS Core)

ecm_add_qml_module(ZynthboxQmlModule
    VERSION 1.0
    URI "io.zynthbox.ui2"
    CLASS_NAME ZynthboxKit
    DEPENDENCIES
    "QtQuick.Controls"
    "Qt5Compat.GraphicalEffects"
    NO_PLUGIN_OPTIONAL)

# target_sources(MauiKit${MAUI_MAJOR_VERSION}

#     PRIVATE
#     libs/appview.h
#     libs/tabview.h)
target_sources(ZynthboxQmlModule
    PRIVATE
    zynthbox.cpp
    zynthbox.h
    theme.h theme.cpp)

if(QUICK_COMPILER)
    qtquick_compiler_add_resources(zb_QML_QRC zynthbox.qrc)
else()
    qt5_add_resources(zb_QML_QRC zynthbox.qrc)
endif()

ecm_target_qml_sources(ZynthboxQmlModule SOURCES

    qml/Button.qml
    qml/Popup.qml)

# ecm_target_qml_sources(ZynthboxKit PATH private SOURCES

#     qml/private/AboutDialog.qml)

if(QUICK_COMPILER)
    target_sources(ZynthboxQmlModule PRIVATE ${zb_QML_QRC})

    add_definitions(-DQUICK_COMPILER)
    target_compile_definitions(ZynthboxQmlModule PUBLIC QUICK_COMPILER)
endif()

target_link_libraries(ZynthboxQmlModule
    PUBLIC
    Qt${QT_MAJOR_VERSION}::Core
    KF${KF_MAJOR_VERSION}::CoreAddons
    KF${KF_MAJOR_VERSION}::ConfigCore
    PRIVATE
     KF5::Plasma
    Qt${QT_MAJOR_VERSION}::Qml
    Qt${QT_MAJOR_VERSION}::Svg
    Qt${QT_MAJOR_VERSION}::Widgets
    Qt${QT_MAJOR_VERSION}::QuickControls2)


ecm_finalize_qml_module(ZynthboxQmlModule
    DESTINATION ${KDE_INSTALL_QMLDIR})

ecm_generate_qmltypes(${ZB_URI} 1.3 DESTINATION ${KDE_INSTALL_QMLDIR}/io/zynthbox/ui2)

##INSTALL MAUI STYLE
    # install(DIRECTORY style.5/ DESTINATION ${KDE_INSTALL_QMLDIR}/QtQuick/Controls.2/maui-style)
