#include "windowsmodel.h"
#include <KWindowSystem/KWindowSystem>
#include <KWindowSystem/KWindowInfo>
#include <KWindowSystem/KX11Extras>
#include <QIcon>
#include <KIconLoader>
WindowsModel::WindowsModel(QObject *parent) : QAbstractListModel(parent)
{
    connect(KX11Extras::self(), &KX11Extras::windowAdded, this, &WindowsModel::refresh);
    connect(KX11Extras::self(), &KX11Extras::windowRemoved, this, &WindowsModel::refresh);
    connect(KX11Extras::self(), &KX11Extras::windowChanged, this, &WindowsModel::onWindowChanged);
    // connect(KWindowSystem::self(), &KWindowSystem::windowActivated, this, &WindowsModel::refresh);
    // connect(KWindowSystem::self(), &KWindowSystem::windowDemandsAttentionChanged, this, &WindowsModel::refresh);    
    // connect(KWindowSystem::self(), &KWindowSystem::windowMinimized, this, &WindowsModel::refresh);
    // connect(KWindowSystem::self(), &KWindowSystem::windowUnminimized, this, &WindowsModel::refresh);
    // connect(KWindowSystem::self(), &KWindowSystem::windowMaximized, this, &WindowsModel::refresh);
    // connect(KWindowSystem::self(), &KWindowSystem::windowUnmaximized, this, &WindowsModel::refresh);
    // connect(KWindowSystem::self(), &KWindowSystem::windowHidden, this, &WindowsModel::refresh);
    // connect(KWindowSystem::self(), &KWindowSystem::windowShown, this, &WindowsModel::refresh);          
}  

void WindowsModel::onWindowChanged(WId id, NET::Properties properties, NET::Properties2 properties2)
{
    //  if (properties.testFlag(NET::WMVisibleName) || properties.testFlag(NET::WMName))
    //     this->refresh(id);


    // XXX: we are setting window icon geometry -> don't need to handle NET::WMIconGeometry
    // Icon of the button can be based on windowClass
    // if (properties.testFlag(NET::WMIcon) || properties2.testFlag(NET::WM2WindowClass))
    //     emit windowPropertyChanged(windowId, int(LXQtTaskBarWindowProperty::Icon));
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
    KWindowInfo info(winId, NET::WMName | NET::WMIcon | NET::WMState | NET::WMIconName, NET::WM2WindowClass);

    switch (role) {
    case WindowIdRole:
        return QString::number(winId);
    case WindowTitleRole:
        return info.name();
    case WindowIconRole:
        return QString(info.windowClassName());
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
    emit countChanged();
    endResetModel();
}

static QIcon iconFor(WId id)
{
    QIcon icon;

    icon.addPixmap(KWindowSystem::icon(id, KIconLoader::SizeSmall, KIconLoader::SizeSmall, false));
    icon.addPixmap(KWindowSystem::icon(id, KIconLoader::SizeSmallMedium, KIconLoader::SizeSmallMedium, false));
    icon.addPixmap(KWindowSystem::icon(id, KIconLoader::SizeMedium, KIconLoader::SizeMedium, false));
    icon.addPixmap(KWindowSystem::icon(id, KIconLoader::SizeLarge, KIconLoader::SizeLarge, false));

    return icon;
}

