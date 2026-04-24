#pragma once
#include <QQmlParserStatus>
#include <QAbstractListModel>
#include <KWindowSystem/KWindowSystem>

class WindowsModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

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
    void onWindowChanged(WId id, NET::Properties properties, NET::Properties2 properties2);
signals:
    void countChanged();
};