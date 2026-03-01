#include "database.h"
#include <QCoreApplication>
#include <QDebug>
#include <QSqlError>
#include <QSqlTableModel>

bool Database::init()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QCoreApplication::applicationDirPath() + "/toolkit.db");
    if (!db.open())
    {
        qDebug() << "链接数据库失败： " << db.lastError().text();
        return false;
    }
    QSqlQuery query;
    QString items_sql = "CREATE TABLE IF NOT EXISTS items ("
                        "id INTEGER PRIMARY KEY AUTOINCREMENT," // 0
                        "name TEXT,"                            // 1
                        "type INTEGER,"                         // 2
                        "tag TEXT,"                             // 3
                        "icon TEXT,"                            // 4
                        "note TEXT,"                            // 5
                        "url TEXT,"                             // 6
                        "path TEXT,"                            // 7
                        "row INTEGER)";                         // 8

    QString settings_sql = "CREATE TABLE IF NOT EXISTS settings ("
                           "id INTEGER PRIMARY KEY AUTOINCREMENT," // 0
                           "name TEXT,"                            // 1
                           "value TEXT)";                          // 2

    if (!query.exec(items_sql))
    {
        qDebug() << "创建items表失败： " << query.lastError().text();
        return false;
    }
    if (!query.exec(settings_sql))
    {
        qDebug() << "创建settings表失败： " << query.lastError().text();
        return false;
    }
    qDebug() << "数据库初始化成功，文件为" << db.databaseName();

    return setModels();
}

QVariant Database::getSetting(QString name, const QVariant &defaultValue)
{
    QSqlQuery query(db);
    query.prepare("SELECT value FROM settings WHERE name = :name");
    query.bindValue(":name", name);
    if (!query.exec())
    {
        qDebug() << "查询settings表失败： " << query.lastError().text();
        return defaultValue;
    }
    if (query.next())
    {
        return query.value(0);
    }
    setSetting(name, defaultValue);
    return defaultValue;
}

bool Database::setSetting(QString name, QVariant value)
{
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM settings WHERE name = :name");
    query.bindValue(":name", name);
    if (!query.exec())
    {
        qDebug() << "查询settings表失败： " << query.lastError().text();
        return false;
    }
    query.next();
    bool isExist = query.value(0).toInt() > 0;
    if (isExist)
    {
        query.prepare("UPDATE settings SET value = :value WHERE name = :name");
        query.bindValue(":name", name);
        query.bindValue(":value", value);
    }
    else
    {
        query.prepare("INSERT INTO settings (name, value) VALUES (:name, :value)");
        query.bindValue(":name", name);
        query.bindValue(":value", value);
    }
    if (!query.exec())
    {
        qDebug() << "设置settings失败： " << query.lastError().text();
        return false;
    }
    return true;
}

// QList<QVariantMap> Database::getAllItems()

// {
//     QList<QVariantMap> list;
//     QSqlQuery query(db);
//     query.prepare("SELECT id,name,type,tag,icon FROM items ORDER BY row ASC");
//     if (query.exec())
//     {
//         while (query.next())
//         {
//             QVariantMap item;
//             item["id"] = query.value("id");
//             item["name"] = query.value("name");
//             item["type"] = query.value("type");
//             item["tag"] = query.value("tag");
//             item["icon"] = query.value("icon");
//             list.append(item);
//         }

//         return list;
//     }
//     qDebug() << "查询items表失败： " << query.lastError().text();
//     return list;
// }

bool Database::setModels()
{
    this->itemsModel = new ToolkitModel(this, db);
    itemsModel->setTable("items");
    itemsModel->setEditStrategy(ToolkitModel::OnManualSubmit);
    int colindex = itemsModel->fieldIndex("row");
    itemsModel->setSort(colindex, Qt::AscendingOrder);
    if (!itemsModel->select())
    {
        qDebug() << "设置items表模型失败： " << itemsModel->lastError().text();
        return false;
    }
    qDebug() << "设置items表模型成功";

    return true;
}