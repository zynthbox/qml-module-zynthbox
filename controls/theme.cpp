#include "theme.h"
#include <KSharedConfig>
#include <KConfigGroup>
#include <KDirWatch>
// #include <Plasma/plasma/theme.h>
#include <QStringBuilder>
#include <Plasma/Theme>

using QSP = QStandardPaths;
#define PLASMA_RELATIVE_DATA_INSTALL_DIR "plasma"

KSharedConfig::Ptr configForTheme(const QString &theme)
{
    const QString baseName = QLatin1String(PLASMA_RELATIVE_DATA_INSTALL_DIR "/desktoptheme/") % theme;
    QString configPath = QSP::locate(QSP::GenericDataLocation, baseName + QLatin1String("/plasmarc"));
    if (!configPath.isEmpty()) {
        return KSharedConfig::openConfig(configPath, KConfig::SimpleConfig);
    }
    QString metadataPath = QSP::locate(QSP::GenericDataLocation, baseName + QLatin1String("/metadata.desktop"));
    return KSharedConfig::openConfig(metadataPath, KConfig::SimpleConfig);
}

Theme *Theme::qmlAttachedProperties(QObject *object)
{
    return new Theme(object);
}

void Theme::sync()
{
    KSharedConfigPtr metadata = configForTheme(m_themeName);
    processZynthboxSettings(metadata);
}

Theme::Theme(QObject *parent)
    : QObject{parent}
    , p_theme(new Plasma::Theme(this))
    ,m_sectionSpacing(2)
    ,m_cellSpacing(1)
    ,m_useBreadcrumb(true)
    ,m_altVolume(false)
{
    m_slotSpacing = {4,20,4,4,4};
    connect(p_theme, &Plasma::Theme::themeChanged, this, [this](){
        setThemeName(p_theme->themeName());
    });

    m_themeName = p_theme->themeName();
    sync();
}

int Theme::spacing() const
{
    return m_spacing;
}

int Theme::padding() const
{
    return m_padding;
}

int Theme::radius() const
{
    return m_radius;
}

QString Theme::themeName() const
{
    return m_themeName;
}

void Theme::setThemeName(const QString &newThemeName)
{
    if (m_themeName == newThemeName)
        return;
    m_themeName = newThemeName;
    sync();
    Q_EMIT themeChanged();
}

void Theme::processZynthboxSettings(const KSharedConfigPtr &metadata)
{
    KConfigGroup cg;
    if (metadata->hasGroup("Zynthbox")) {
        cg = KConfigGroup(metadata, "Zynthbox");

        m_spacing = cg.readEntry("spacing", 1);
        m_padding = cg.readEntry("padding", 1);
        m_radius = cg.readEntry("radius", 4);
        m_sectionSpacing = cg.readEntry("sectionSpacing", 2);
        m_cellSpacing = cg.readEntry("cellSpacing", 1);
        m_sectionPadding = cg.readEntry("sectionPadding", 2);
        m_useBreadcrumb = cg.readEntry("useBreadcrumb", true);
        m_altVolume = cg.readEntry("altVolume", false);
        m_ghostButton = cg.readEntry("ghostButton", false);
        m_monoColorHandles = cg.readEntry("monoColorHandles", false);
        m_slotSpacing = cg.readEntry("slotSpacing", {4,20,4,4,4});
        m_altTabs = cg.readEntry("altTabs", false);
        m_altPanels = cg.readEntry("altPanels", false);

    } else {
        m_spacing = 1;
        m_padding = 1;
        m_sectionSpacing = 2;
        m_cellSpacing = 1;
        m_sectionPadding = 2;
        m_radius = 4;
        m_useBreadcrumb = true;
        m_altVolume = false;
        m_ghostButton = false;
        m_monoColorHandles = false;
        m_slotSpacing = {4,20,4,4,4};
        m_altTabs= false;
        m_altPanels=false;
    }
}

bool Theme::backgroundContrastEnabled() const
{
    return p_theme->backgroundContrastEnabled();
}

qreal Theme::backgroundContrast() const
{
    return p_theme->backgroundContrast();
}

qreal Theme::backgroundIntensity() const
{
    return p_theme->backgroundIntensity();
}

qreal Theme::backgroundSaturation() const
{
    return p_theme->backgroundSaturation();
}

QString Theme::wallpaperPath() const
{
    return p_theme->wallpaperPath();
}
