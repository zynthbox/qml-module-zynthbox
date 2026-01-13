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
    qml/ActionPickerPopup.qml
    qml/ActionRow.qml
    qml/AbstractADSRView.qml
    qml/ADSRClipView.qml
    qml/ADSRControlView.qml
    
    qml/ActionBar.qml
    qml/Card.qml

    qml/BasicDelegate.qml
    qml/Button.qml
    qml/Dialog.qml
    qml/Drawer.qml
    qml/DialogQuestion.qml
    qml/DialogQuestionButtonFocusHighlight.qml
    qml/ComboBox.qml
    qml/InfinitySlider.qml
    qml/LibraryPagePicker.qml
    qml/KnobIndicator.qml
    qml/MultiSelectorPage.qml
    qml/NotePickerPopup.qml
    qml/NewStuffPage.qml
    qml/Popup.qml
    qml/PlayGridButton.qml
    qml/Stack.qml
    qml/ScreenPage.qml
    qml/SelectorView.qml
    qml/SelectorPage.qml
    qml/SelectorDelegate.qml

    #Controllers
    qml/AbstractController.qml
    qml/SwitchController.qml
    qml/DialController.qml
    qml/SliderController.qml
    qml/MultiSwitchController.qml
    qml/ControllerLoader.qml
    qml/ControllerGroup.qml
    )

ecm_target_qml_sources(ZynthboxQmlModule PATH private SOURCES

    qml/private/ActionButton.qml
    qml/private/CardBackground.qml
    qml/private/DelegateBackground.qml
    qml/private/SelectorViewBackground.qml
    )

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
