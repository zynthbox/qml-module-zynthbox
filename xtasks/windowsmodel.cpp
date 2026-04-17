#include "windowsmodel.h"
#include <KWindowSystem/KWindowSystem>
#include <KWindowSystem/KWindowInfo>
#include <KWindowSystem/KX11Extras>

WindowsModel::WindowsModel(QObject *parent) : QAbstractListModel(parent)
{
    connect(KX11Extras::self(), &KX11Extras::windowAdded, this, &WindowsModel::refresh);
    connect(KX11Extras::self(), &KX11Extras::windowRemoved, this, &WindowsModel::refresh);
    // connect(KWindowSystem::self(), &KWindowSystem::windowActivated, this, &WindowsModel::refresh);
    // connect(KWindowSystem::self(), &KWindowSystem::windowDemandsAttentionChanged, this, &WindowsModel::refresh);    
    // connect(KWindowSystem::self(), &KWindowSystem::windowMinimized, this, &WindowsModel::refresh);
    // connect(KWindowSystem::self(), &KWindowSystem::windowUnminimized, this, &WindowsModel::refresh);
    // connect(KWindowSystem::self(), &KWindowSystem::windowMaximized, this, &WindowsModel::refresh);
    // connect(KWindowSystem::self(), &KWindowSystem::windowUnmaximized, this, &WindowsModel::refresh);
    // connect(KWindowSystem::self(), &KWindowSystem::windowHidden, this, &WindowsModel::refresh);
    // connect(KWindowSystem::self(), &KWindowSystem::windowShown, this, &WindowsModel::refresh);          
}  

int WindowsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return KX11Extras::windows().size();
}

QVariant WindowsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    WId winId = KX11Extras::windows().at(index.row());
    KWindowInfo info(winId, NET::WMName | NET::WMIcon | NET::WMState);
    switch (role) {
    case WindowIdRole:
        return QString::number(winId);
    case WindowTitleRole:
        return info.name();
    case WindowIconRole:
        return info.iconName();
    case WindowIsActiveRole:
        return info.hasState(NET::Focused);
    case WindowIsMinimizedRole:
        return info.hasState(NET::Hidden);
    case WindowIsMaximizedRole:
        return info.hasState(NET::Max);
    case WindowIsHiddenRole:
        return info.hasState(NET::Hidden);
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> WindowsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[WindowIdRole] = "windowId";
    roles[WindowTitleRole] = "windowTitle";
    roles[WindowIconRole] = "windowIcon";
    roles[WindowIsActiveRole] = "windowIsActive";
    roles[WindowIsMinimizedRole] = "windowIsMinimized";
    roles[WindowIsMaximizedRole] = "windowIsMaximized";
    roles[WindowIsHiddenRole] = "windowIsHidden";
    return roles;
}

void WindowsModel::refresh(WId id)
{
    beginResetModel();
    endResetModel();
}

