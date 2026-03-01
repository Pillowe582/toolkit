#include "changelog.h"
#include "ui_changelog.h"
#include <QDebug>
#include "updater.h"
#include <QFile>
#include <QTextStream>

Changelog::Changelog(QWidget *parent) : QDialog(parent),
                                        ui(new Ui::Changelog)
{
    ui->setupUi(this);
    qDebug() << "更新日志已打开";
    QFile file(":/assets/UPDATE.md");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "更新日志打开失败";
        return;
    }
    QTextStream in(&file);
    QString md = in.readAll();
    file.close();
    ui->textBrowser->setMarkdown(md);
    connect(ui->chkupdatebtn, &QPushButton::clicked, this, []()
            { Updater::checkForUpdates(VERSION, true); });
}

Changelog::~Changelog()
{
    qDebug() << "更新日志将关闭";
    delete ui;
}

// void Changelog::showUpdater()
// {
//     Updater *updater = new Updater();
//     updater->exec();
// }