#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QFileIconProvider>
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
    ToolkitModel *itemsModel;
    QSqlTableModel *settingsModel;

private:
    QSqlDatabase db;
    bool setModels();
};

#include <QIdentityProxyModel>
#include <QIcon>
class IconProxyModel : public QIdentityProxyModel
{
private:
    QFileIconProvider iconProvider;

public:
    using QIdentityProxyModel::QIdentityProxyModel;

    QVariant data(const QModelIndex &proxyIndex, int role) const override
    {
        // 我们只在第 1 列（标题列）绘制图标
        if (role == Qt::DecorationRole && proxyIndex.column() == 1)
        {

            // 尝试从第 7 列获取文件路径并提取系统图标
            QString filePath = sourceModel()->index(proxyIndex.row(), 7).data().toString();
            if (!filePath.isEmpty())
            {
                return iconProvider.icon(QFileInfo(filePath));
            }

            // 3. 兜底方案：返回程序默认图标
            return QIcon(":/assets/MainIcon.ico");
        }

        // 其他角色（如文字内容）正常返回
        return QIdentityProxyModel::data(proxyIndex, role);
    }
};
#endif