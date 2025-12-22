#pragma once

#include <QObject>
#include <QQmlEngine>
#include <KSharedConfig>
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

public:
    explicit Theme(QObject *parent = nullptr);


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

