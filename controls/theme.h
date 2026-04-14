#pragma once

#include <QObject>
#include <QQmlEngine>
#include <KSharedConfig>
#include <QVariantList> 
#include <QColor>

namespace Plasma{
class Theme;
}
class Theme : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(Theme)
    QML_ATTACHED(Theme)
    QML_UNCREATABLE("Attached Property")
    Q_PROPERTY(QString themeName READ themeName WRITE setThemeName NOTIFY themeChanged)
    Q_PROPERTY(int spacing READ spacing NOTIFY themeChanged)
    Q_PROPERTY(int padding READ padding NOTIFY themeChanged)
    Q_PROPERTY(int radius READ radius NOTIFY themeChanged)
    Q_PROPERTY(bool contrastEnabled READ backgroundContrastEnabled NOTIFY themeChanged)
    Q_PROPERTY(qreal backgroundContrast READ backgroundContrast NOTIFY themeChanged)
    Q_PROPERTY(qreal backgroundIntensity READ backgroundIntensity NOTIFY themeChanged)
    Q_PROPERTY(qreal backgroundSaturation READ backgroundSaturation NOTIFY themeChanged)
    Q_PROPERTY(QString wallpaperPath READ wallpaperPath NOTIFY themeChanged)
    Q_PROPERTY(int sectionSpacing MEMBER m_sectionSpacing NOTIFY themeChanged)
    Q_PROPERTY(int cellSpacing MEMBER m_cellSpacing NOTIFY themeChanged)
    Q_PROPERTY(int sectionPadding MEMBER m_sectionPadding NOTIFY themeChanged)
    Q_PROPERTY(bool useBreadcrumb MEMBER m_useBreadcrumb NOTIFY themeChanged)
    Q_PROPERTY(bool altVolume MEMBER m_altVolume NOTIFY themeChanged)
    Q_PROPERTY(bool ghostButton MEMBER m_ghostButton NOTIFY themeChanged)
    Q_PROPERTY(bool monoColorHandles MEMBER m_monoColorHandles NOTIFY themeChanged)
    Q_PROPERTY(QVariantList slotSpacing MEMBER m_slotSpacing NOTIFY themeChanged)
    Q_PROPERTY(bool altTabs MEMBER m_altTabs NOTIFY themeChanged)
    Q_PROPERTY(bool altPanels MEMBER m_altPanels NOTIFY themeChanged)
    Q_PROPERTY(QColor buttonActiveColor MEMBER m_buttonActiveColor NOTIFY themeChanged)
    Q_PROPERTY(QColor buttonInactiveColor MEMBER m_buttonInactiveColor NOTIFY themeChanged)
    Q_PROPERTY(QColor buttonOffColor MEMBER m_buttonOffColor NOTIFY themeChanged)
    Q_PROPERTY(QColor stepEmptyColor MEMBER m_stepEmptyColor NOTIFY themeChanged)
    Q_PROPERTY(QColor stepWithNotesDimmedColor MEMBER m_stepWithNotesDimmedColor NOTIFY themeChanged)
    Q_PROPERTY(QColor stepWithNotesColor MEMBER m_stepWithNotesColor NOTIFY themeChanged)
    Q_PROPERTY(QColor stepHighlightedColor MEMBER m_stepHighlightedColor NOTIFY themeChanged)
    Q_PROPERTY(QColor stepMutedColor MEMBER m_stepMutedColor NOTIFY themeChanged)
    Q_PROPERTY(QColor stepCurrentColor MEMBER m_stepCurrentColor NOTIFY themeChanged)
    Q_PROPERTY(QColor stepRecordingColor MEMBER m_stepRecordingColor NOTIFY themeChanged)
    Q_PROPERTY(QColor sequencerModeColor MEMBER m_sequencerModeColor NOTIFY themeChanged)
    Q_PROPERTY(QColor trackClipModeColor MEMBER m_trackClipModeColor NOTIFY themeChanged)
    Q_PROPERTY(QColor musicalKeysModeColor MEMBER m_musicalKeysModeColor NOTIFY themeChanged)
    Q_PROPERTY(QColor velocityKeysModeColor MEMBER m_velocityKeysModeColor NOTIFY themeChanged)
    Q_PROPERTY(QColor slotModeColor MEMBER m_slotModeColor NOTIFY themeChanged)

public:
    explicit Theme(QObject *parent = nullptr);
    
    int m_sectionSpacing = 2;
    int m_cellSpacing = 1;
    int m_sectionPadding = 2;
    bool m_useBreadcrumb = true;
    bool m_altVolume = false;
    bool m_ghostButton = false;
    QVariantList m_slotSpacing;
    bool m_monoColorHandles = false;
    bool m_altTabs=false;
    bool m_altPanels=false;

    // Default values for buttons and stepsequencer colors
    // Track and note colors are not meant to be configurable by the theme 
    // and are defined in zynthian_gui_theme_chooser.py
    QColor m_buttonActiveColor = QColor(0, 255, 0, 255);
    QColor m_buttonInactiveColor = QColor(0, 0, 255, 255);
    QColor m_buttonOffColor = QColor(0, 0, 0, 255);
    QColor m_stepEmptyColor = QColor(26, 26, 26, 255);
    QColor m_stepWithNotesDimmedColor = QColor(0, 0, 179, 255);
    QColor m_stepWithNotesColor = QColor(128, 128, 255, 255);
    QColor m_stepHighlightedColor = QColor(128, 255, 255, 255);
    QColor m_stepMutedColor = QColor(128, 0, 0, 255);
    QColor m_stepCurrentColor = QColor(255, 255, 0, 255);
    QColor m_stepRecordingColor = QColor(255, 0, 0, 255);
    QColor m_sequencerModeColor = QColor(255, 0, 0, 255);
    QColor m_trackClipModeColor = QColor(255, 255, 0, 255);
    QColor m_musicalKeysModeColor = QColor(0, 0, 255, 255);
    QColor m_velocityKeysModeColor = QColor(0, 0, 255, 255);
    QColor m_slotModeColor = QColor(0, 255, 0, 255);

    int spacing() const;
    int padding() const;
    int radius() const;

    QString themeName() const;
    void setThemeName(const QString &newThemeName);
    static Theme *qmlAttachedProperties(QObject *object);

    bool backgroundContrastEnabled() const;

    qreal backgroundContrast() const;

    qreal backgroundIntensity() const;

    qreal backgroundSaturation() const;

    QString wallpaperPath() const;

private Q_SLOTS:
    void sync();

Q_SIGNALS:
    void themeChanged();

private:
    Plasma::Theme *p_theme;
    QString m_themeName;

    void processZynthboxSettings(const KSharedConfigPtr &metadata);

    int m_spacing;
    int m_padding;
    int m_radius;
    
};

