#pragma once

#include <QObject>
#include <QString>


class KWindowSystem;
class WindowsModel;
class XTask : public QObject
{
    Q_OBJECT
    Q_PROPERTY(WindowsModel *windowsModel READ windowsModel CONSTANT)

public:
    explicit XTask(QObject *parent = nullptr);
    ~XTask() override = default;

public slots:
    void activate(const QString &windowId);
    void close(const QString &windowId);
    void toggle(const QString &windowId);
    void minimize(const QString &windowId);
    void maximize(const QString &windowId);
    void unmaximize(const QString &windowId);
    void show(const QString &windowId);
    void hide(const QString &windowId);

    WindowsModel *windowsModel();

    private:
    WindowsModel *m_windowsModel;   
};
