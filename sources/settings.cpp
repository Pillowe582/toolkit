#include "settings.h"
#include "ui_settings.h"
#include <QDebug>
#include "database.h"

Settings::Settings(QWidget *parent, Database *db) : QDialog(parent),
                                                    ui(new Ui::Settings)
{

    if (!db)
    {
        qDebug() << "数据库未初始化";
        return;
    }
    ui->setupUi(this);

    // 同步数据库
    ui->executeOnStarter->setChecked(db->getSetting("executeOnStart", false).toBool());
    ui->focusOutMinimizer->setChecked(db->getSetting("focusOutMinimize", true).toBool());
    ui->closeMinimizer->setChecked(db->getSetting("closeMinimize", true).toBool());

    // 连接信号和槽
    connect(ui->dialogbtn, &QDialogButtonBox::accepted, this, [this, db]()
            { saveSettings(db);
            emit settingsSaved(); });

    qDebug() << "设置窗口已打开";
}

Settings::~Settings()
{
    qDebug() << "设置窗口将关闭";
    delete ui;
}

bool Settings::saveSettings(Database *db)
{
    if (!db)
        return false;
    bool success;
    success &= db->setSetting("executeOnStart", ui->executeOnStarter->isChecked());
    success &= db->setSetting("focusOutMinimize", ui->focusOutMinimizer->isChecked());
    success &= db->setSetting("closeMinimize", ui->closeMinimizer->isChecked());
    if (success)
    {
        qDebug() << "设置已保存";
    }
    return success;
}