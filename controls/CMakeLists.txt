# SPDX-FileCopyrightText: 2020 Camilo Higuita <milo.h@aol.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later

set(PLASMA_RELATIVE_DATA_INSTALL_DIR "plasma")
set(PLASMA_DATA_INSTALL_DIR "${KDE_INSTALL_DATADIR}/${PLASMA_RELATIVE_DATA_INSTALL_DIR}")

ecm_add_qml_module(ZynthboxQmlModule
    VERSION 1.0
    URI "io.zynthbox.ui"
    CLASS_NAME ZynthboxKit
    DEPENDENCIES
    "QtQuick.Controls"
    NO_PLUGIN_OPTIONAL)

target_sources(ZynthboxQmlModule
    PRIVATE
    assets.qrc
    zynthbox.h zynthbox.cpp    
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
    qml/CellControl.qml

    qml/BasicDelegate.qml
    qml/BreadcrumbButton.qml
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
    qml/Menu.qml
    qml/NotePickerPopup.qml
    qml/NotePad.qml
    qml/NewStuffPage.qml
    qml/Popup.qml
    qml/PlayGridButton.qml
    qml/Rating.qml
    qml/ResetableSlider.qml
    qml/Stack.qml
    qml/ScrollableScreenPage.qml
    qml/ScreenPage.qml
    qml/SectionGroup.qml
    qml/SectionButton.qml
    qml/SectionPanel.qml
    qml/SelectorView.qml
    qml/SelectorPage.qml
    qml/SelectorDelegate.qml
    qml/SelectorWrapper.qml
    qml/SketchpadDial.qml
    qml/SketchpadMultiSwitch.qml
    qml/TabbedControlViewAction.qml
    qml/TabbedControlView.qml
    qml/TableHeader.qml
    qml/TableHeaderLabel.qml

    #Controllers
    qml/AbstractController.qml
    qml/SwitchController.qml
    qml/DialController.qml
    qml/SliderController.qml
    qml/MultiSwitchController.qml
    qml/ControllerLoader.qml
    qml/ControllerGroup.qml
    qml/CommonUtils.js)

ecm_target_qml_sources(ZynthboxQmlModule PATH private SOURCES

    qml/private/ActionButton.qml
    qml/private/CardBackground.qml
    qml/private/DelegateBackground.qml
    qml/private/SelectorViewBackground.qml
    
    qml/private/ButtonBackground.qml
    qml/private/FlatButtonBackground.qml
    qml/private/RaisedButtonBackground.qml
    qml/private/ButtonFocus.qml
    qml/private/ButtonShadow.qml
    qml/private/ButtonHover.qml)

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

ecm_generate_qmltypes(${ZB_URI} 1.0 DESTINATION ${KDE_INSTALL_QMLDIR}/io/zynthbox/ui)
