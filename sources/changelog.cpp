#include "changelog.h"
#include "ui_changelog.h"
#include <QDebug>
Changelog::Changelog(QWidget *parent) : QDialog(parent),
                                        ui(new Ui::Changelog)
{
    ui->setupUi(this);
    qDebug() << "更新日志已打开";
}

Changelog::~Changelog()
{
    qDebug() << "更新日志将关闭";
    delete ui;
}
