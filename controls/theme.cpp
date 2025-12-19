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
{

    auto theme = new Plasma::Theme(this);
    connect(theme, &Plasma::Theme::themeChanged, this, [this, theme](){
        setThemeName(theme->themeName());
    });

    m_themeName = theme->themeName();
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
    } else {
        m_spacing = 1;
        m_padding = 1;
        m_radius = 4;
    }
}
