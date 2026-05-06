#include "xtask.h"
#include "windowsmodel.h"
#include <KWindowSystem/KX11Extras>
#include <KWindowSystem/KWindowInfo>
#include <KWindowSystem/KWindowSystem>
#include <NETWM>
#include <QX11Info>
#include <QProcess>

XTask::XTask(QObject *parent) : QObject(parent)
{
    connect(KX11Extras::self(), &KX11Extras::activeWindowChanged, [this]() {
        emit activeWindowChanged();
        emit activeWindowPidChanged();
    });

}

void XTask::activate(const QString &windowId)
{
    WId winId = windowId.toULongLong();
    KX11Extras::activateWindow(winId);
}

void XTask::close(const QString &windowId)
{
    WId winId = windowId.toULongLong();
    KWindowInfo wInfo(winId, NET::WMState);
    if (!wInfo.valid()) {
        return;
    }

    NETRootInfo ri(QX11Info::connection(), NET::CloseWindow);
    ri.closeWindowRequest(winId);
}

void XTask::closeActiveWindow()
{
    WId activeWinId = KX11Extras::activeWindow();
    close(QString::number(activeWinId));
}

void XTask::toggle(const QString &windowId)
{
    // WId winId = windowId.toULongLong();
    // KWindowInfo info(winId, NET::WMState);
    // if (info.hasState(NET::Hidden))
    //     KX11Extras::showWindow(winId);
    // else
    //     KX11Extras::hideWindow(winId);
}

void XTask::minimize(const QString &windowId)
{
    WId winId = windowId.toULongLong();
    KX11Extras::minimizeWindow(winId);
}

void XTask::maximize(const QString &windowId)
{
    WId winId = windowId.toULongLong();
    // KX11Extras::maximizeWindow(winId);
}

void XTask::unmaximize(const QString &windowId)
{
    WId winId = windowId.toULongLong();
    // KX11Extras::unmaximizeWindow(winId);
}

void XTask::show(const QString &windowId)
{
    WId winId = windowId.toULongLong();
    KX11Extras::unminimizeWindow(winId);
}

void XTask::hide(const QString &windowId)
{
    WId winId = windowId.toULongLong();
    // KX11Extras::hideWindow(winId);
}

void XTask::minimizeActiveWindow()
{
    WId activeWinId = KX11Extras::activeWindow();
    if (activeWinId) {
        KX11Extras::minimizeWindow(activeWinId);
    }
}

WindowsModel *XTask::windowsModel()
{
    if (!m_windowsModel) {
        m_windowsModel = new WindowsModel(const_cast<XTask *>(this));
    }
    return m_windowsModel;
}

QString XTask::activeWindow()
{
    WId activeWinId = KX11Extras::activeWindow();
    if (activeWinId) {
        KWindowInfo info(activeWinId, NET::WMName | NET::WMIcon | NET::WMState | NET::WMIconName, NET::WM2WindowClass);
        return info.windowClassName();
    }
    return QString();
}

int XTask::activeWindowPid() const
{
    WId activeWinId = KX11Extras::activeWindow();
    if (!activeWinId)
        return -1;

    KWindowInfo info(activeWinId, NET::WMState);
    if (!info.valid())
        return -1;

    return info.pid();
}

void XTask::startPipeWirePulseService()
{
    QProcess::startDetached(QStringLiteral("systemctl"), {QStringLiteral("--user"), QStringLiteral("start"), QStringLiteral("pipewire-pulse.service")});
}

void XTask::stopPipeWirePulseService()
{
    QProcess::startDetached(QStringLiteral("systemctl"), {QStringLiteral("--user"), QStringLiteral("stop"), QStringLiteral("pipewire-pulse.service")});
}