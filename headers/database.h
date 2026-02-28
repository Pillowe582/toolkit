#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>

class ToolkitModel : public QSqlTableModel
{
    Q_OBJECT
public:
    using QSqlTableModel::QSqlTableModel;

    // 必须有 const，必须返回 Qt::ItemFlags
    // Qt::ItemFlags flags(const QModelIndex &index) const override
    // {
    //     Qt::ItemFlags f = QSqlTableModel::flags(index);
    //     if (index.isValid())
    //     {
    //         // 核心权限：允许拖动
    //         f |= Qt::ItemIsDragEnabled;
    //     }
    //     else
    //     {
    //         // 允许在空白处放下（移动到列表末尾）
    //         f |= Qt::ItemIsDropEnabled;
    //     }
    //     return f;
    // }
    // Qt::DropActions supportedDropActions() const override
    // {
    //     // 明确告诉 View，本模型支持“移动”动作
    //     return Qt::MoveAction;
    // }
    // QStringList mimeTypes() const override
    // {
    //     return {"application/x-qabstractitemmodeldatalist"};
    // }
};

class Database : public QObject
{
    Q_OBJECT
public:
    bool init();
    QVariant getSetting(QString name, const QVariant &defaultValue);
    bool setSetting(QString name, QVariant value);
    QList<QVariantMap> getAllItems();
    ToolkitModel *model;

private:
    QSqlDatabase db;
    bool setListModel();
};
#endif