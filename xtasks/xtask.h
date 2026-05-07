#pragma once

#include <QObject>
#include <QRect>
#include <QString>


class KWindowSystem;
class WindowsModel;
class XTask : public QObject
{
    Q_OBJECT
    Q_PROPERTY(WindowsModel *windowsModel READ windowsModel CONSTANT)
    Q_PROPERTY(QString activeWindow READ activeWindow NOTIFY activeWindowChanged)
    Q_PROPERTY(int activeWindowPid READ activeWindowPid NOTIFY activeWindowPidChanged)
    Q_PROPERTY(QString activeWindowId READ activeWindowId NOTIFY activeWindowChanged)

public:
    explicit XTask(QObject *parent = nullptr);
    ~XTask() override = default;

public slots:
    void activate(const QString &windowId);
    void close(const QString &windowId);
    void closeActiveWindow();
    void toggle(const QString &windowId);
    void minimize(const QString &windowId);
    void maximize(const QString &windowId);
    void unmaximize(const QString &windowId);
    void show(const QString &windowId);
    void hide(const QString &windowId);
    void minimizeActiveWindow();
    void setWindowGeometry(const QString &windowId, const QRect &geometry);

    WindowsModel *windowsModel();
    QString activeWindow();
    QString activeWindowId();
    int activeWindowPid() const;

    static void startPipeWirePulseService();
    static void stopPipeWirePulseService();

signals:
    void activeWindowChanged();
    void activeWindowPidChanged();

private:
    WindowsModel *m_windowsModel = nullptr;
};
