#include "settings.h"
#include "ui_settings.h"
#include <QDebug>

Settings::Settings(QWidget *parent) : QDialog(parent),
                                      ui(new Ui::Settings)
{
    ui->setupUi(this);
    qDebug() << "设置窗口已打开";
}

Settings::~Settings()
{
    qDebug() << "设置窗口将关闭";
    delete ui;
}