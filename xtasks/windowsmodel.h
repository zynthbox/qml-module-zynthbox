#pragma once
#include <QQmlParserStatus>
#include <QAbstractListModel>
#include <KWindowSystem/KWindowSystem>

class WindowsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        WindowIdRole = Qt::UserRole + 1,
        WindowTitleRole,
        WindowIconRole,
        WindowIsActiveRole,
        WindowIsMinimizedRole,
        WindowIsMaximizedRole,
        WindowIsHiddenRole,
    };
    explicit WindowsModel(QObject *parent = nullptr);
    ~WindowsModel() override = default;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
public slots:
    void refresh(WId id = 0);
};