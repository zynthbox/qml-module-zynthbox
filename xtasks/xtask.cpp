#include "xtask.h"
#include "windowsmodel.h"
#include <KWindowSystem/KX11Extras>
#include <KWindowSystem/KWindowInfo>
#include <KWindowSystem/KWindowSystem>  

XTask::XTask(QObject *parent) : QObject(parent)
{

}

void XTask::activate(const QString &windowId)
{
    WId winId = windowId.toULongLong();
    KX11Extras::activateWindow(winId);
}

void XTask::close(const QString &windowId)
{
    WId winId = windowId.toULongLong();
    // KX11Extras::closeWindow(winId);
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
    // KX11Extras::showWindow(winId);
}

void XTask::hide(const QString &windowId)
{
    WId winId = windowId.toULongLong();
    // KX11Extras::hideWindow(winId);
}

WindowsModel *XTask::windowsModel()
{
    if (!m_windowsModel) {
        m_windowsModel = new WindowsModel(const_cast<XTask *>(this));
    }
    return m_windowsModel;
}
