/*
 *   Copyright 2018 Camilo Higuita <milo.h@aol.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "zynthbox.h"
#include <QCoreApplication>
#include <QDebug>
#include <QQmlContext>

#include "theme.h"


ZynthboxKit::ZynthboxKit(QObject *parent) : QQmlExtensionPlugin(parent)
{
}

QUrl ZynthboxKit::componentUrl(const QString &fileName) const
{
    return QUrl(resolveFileUrl(fileName));
}

void ZynthboxKit::initializeEngine(QQmlEngine* engine, const char* uri)
{
    Q_UNUSED(uri);
}

void ZynthboxKit::registerTypes(const char *uri)
{
    qDebug() << "REGISTER ZYNTHBOX KIT TYPES <<<<<<<<<<<<<<<<<<<<<<";

    Q_ASSERT(QLatin1String(uri) == QLatin1String("io.zynthbox.ui2"));

    qmlRegisterType(componentUrl(QStringLiteral("ActionPickerPopup.qml")), uri, 1, 0, "ActionPickerPopup");
    qmlRegisterType(componentUrl(QStringLiteral("ActionBar.qml")), uri, 1, 0, "ActionBar");
    qmlRegisterType(componentUrl(QStringLiteral("ActionRow.qml")), uri, 1, 0, "ActionRow");
    qmlRegisterType(componentUrl(QStringLiteral("AbstractController.qml")), uri, 1, 0, "AbstractController");
    qmlRegisterType(componentUrl(QStringLiteral("AbstractADSRView.qml")), uri, 1, 0, "AbstractADSRView");
    qmlRegisterType(componentUrl(QStringLiteral("ADSRControlView.qml")), uri, 1, 0, "ADSRControlView");
    qmlRegisterType(componentUrl(QStringLiteral("ADSRClipView.qml")), uri, 1, 0, "ADSRClipView");
    qmlRegisterType(componentUrl(QStringLiteral("SwitchController.qml")), uri, 1, 0, "SwitchController");
    qmlRegisterType(componentUrl(QStringLiteral("SliderController.qml")), uri, 1, 0, "SliderController");
    qmlRegisterType(componentUrl(QStringLiteral("MultiSwitchController.qml")), uri, 1, 0, "MultiSwitchController");
    qmlRegisterType(componentUrl(QStringLiteral("DialController.qml")), uri, 1, 0, "DialController");
    qmlRegisterType(componentUrl(QStringLiteral("ControllerLoader.qml")), uri, 1, 0, "ControllerLoader");
    qmlRegisterType(componentUrl(QStringLiteral("ControllerGroup.qml")), uri, 1, 0, "ControllerGroup");
    qmlRegisterType(componentUrl(QStringLiteral("BasicDelegate.qml")), uri, 1, 0, "BasicDelegate");
    qmlRegisterType(componentUrl(QStringLiteral("BreadcrumbButton.qml")), uri, 1, 0, "BreadcrumbButton");
    qmlRegisterType(componentUrl(QStringLiteral("Button.qml")), uri, 1, 0, "Button");
    qmlRegisterType(componentUrl(QStringLiteral("ComboBox.qml")), uri, 1, 0, "ComboBox");
    qmlRegisterType(componentUrl(QStringLiteral("Card.qml")), uri, 1, 0, "Card");
    qmlRegisterType(componentUrl(QStringLiteral("Dialog.qml")), uri, 1, 0, "Dialog");
    qmlRegisterType(componentUrl(QStringLiteral("Drawer.qml")), uri, 1, 0, "Drawer");
    qmlRegisterType(componentUrl(QStringLiteral("DialogQuestion.qml")), uri, 1, 0, "DialogQuestion");
    qmlRegisterType(componentUrl(QStringLiteral("InfinitySlider.qml")), uri, 1, 0, "InfinitySlider");
    qmlRegisterType(componentUrl(QStringLiteral("LibraryPagePicker.qml")), uri, 1, 0, "LibraryPagePicker");
    qmlRegisterType(componentUrl(QStringLiteral("Menu.qml")), uri, 1, 0, "Menu");
    qmlRegisterType(componentUrl(QStringLiteral("MultiSelectorPage.qml")), uri, 1, 0, "MultiSelectorPage");
    qmlRegisterType(componentUrl(QStringLiteral("KnobIndicator.qml")), uri, 1, 0, "KnobIndicator");
    qmlRegisterType(componentUrl(QStringLiteral("NewStuffPage.qml")), uri, 1, 0, "NewStuffPage");
    qmlRegisterType(componentUrl(QStringLiteral("NotePickerPopup.qml")), uri, 1, 0, "NotePickerPopup");
    qmlRegisterType(componentUrl(QStringLiteral("NotePad.qml")), uri, 1, 0, "NotePad");
    qmlRegisterType(componentUrl(QStringLiteral("Popup.qml")), uri, 1, 0, "Popup");
    qmlRegisterType(componentUrl(QStringLiteral("PlayGridButton.qml")), uri, 1, 0, "PlayGridButton");
    qmlRegisterType(componentUrl(QStringLiteral("DialogQuestionButtonFocusHighlight.qml")), uri, 1, 0, "DialogQuestionButtonFocusHighlight");

    qmlRegisterType(componentUrl(QStringLiteral("Rating.qml")), uri, 1, 0, "Rating");
    qmlRegisterType(componentUrl(QStringLiteral("ResetableSlider.qml")), uri, 1, 0, "ResetableSlider");
    qmlRegisterType(componentUrl(QStringLiteral("Stack.qml")), uri, 1, 0, "Stack");
    qmlRegisterType(componentUrl(QStringLiteral("ScrollableScreenPage.qml")), uri, 1, 0, "ScrollableScreenPage");
    qmlRegisterType(componentUrl(QStringLiteral("ScreenPage.qml")), uri, 1, 0, "ScreenPage");
    qmlRegisterType(componentUrl(QStringLiteral("SelectorPage.qml")), uri, 1, 0, "SelectorPage");
    qmlRegisterType(componentUrl(QStringLiteral("SelectorDelegate.qml")), uri, 1, 0, "SelectorDelegate");
    qmlRegisterType(componentUrl(QStringLiteral("SelectorView.qml")), uri, 1, 0, "SelectorView");
    qmlRegisterType(componentUrl(QStringLiteral("SelectorWrapper.qml")), uri, 1, 0, "SelectorWrapper");
    qmlRegisterType(componentUrl(QStringLiteral("SketchpadDial.qml")), uri, 1, 0, "SketchpadDial");
    qmlRegisterType(componentUrl(QStringLiteral("SketchpadMultiSwitch.qml")), uri, 1, 0, "SketchpadMultiSwitch");
    qmlRegisterType(componentUrl(QStringLiteral("TabbedControlViewAction.qml")), uri, 1, 0, "TabbedControlViewAction");
    qmlRegisterType(componentUrl(QStringLiteral("TabbedControlView.qml")), uri, 1, 0, "TabbedControlView");
    qmlRegisterType(componentUrl(QStringLiteral("TableHeader.qml")), uri, 1, 0, "TableHeader");
    qmlRegisterType(componentUrl(QStringLiteral("TableHeaderLabel.qml")), uri, 1, 0, "TableHeaderLabel");

    qmlRegisterType(componentUrl(QStringLiteral("private/ActionButton.qml")), uri, 1, 0, "ActionButton");
    qmlRegisterType(componentUrl(QStringLiteral("private/SelectorViewBackground.qml")), uri, 1, 0, "SelectorViewBackground");
    qmlRegisterType(componentUrl(QStringLiteral("private/DelegateBackground.qml")), uri, 1, 0, "DelegateBackground");
    qmlRegisterType(componentUrl(QStringLiteral("private/CardBackground.qml")), uri, 1, 0, "CardBackground");

    qmlRegisterUncreatableType<Theme>(uri, 1, 0, "Theme", QStringLiteral("Cannot create objects of type Theme, use it as an attached property"));

    // qmlProtectModule(uri, 3);
}

// #include "mauikit.moc"
// #include "moc_mauikit.cpp"
