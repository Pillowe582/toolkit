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
    QString darkHtmlStyle =
        "h1 { font-size: 24px; font-weight: 600; color: #f0f6fc; border-bottom: 1px solid #30363d; }"
        "h2 { font-size: 20px; font-weight: 600; color: #f0f6fc; border-bottom: 1px solid #30363d; }"
        "h3 { font-size: 16px; font-weight: 600; color: #f0f6fc; }"
        "a { color: #58a6ff; text-decoration: none; }"
        "code { background-color: #343941; border-radius: 3px; font-family: 'Consolas'; }"
        "blockquote { color: #8b949e; border-left: 4px solid #30363d; padding-left: 10px; }";
    ui->textBrowser->clear();
    ui->textBrowser->setHtml("");
    ui->textBrowser->document()->setDefaultStyleSheet(darkHtmlStyle);
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