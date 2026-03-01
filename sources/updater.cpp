#include "updater.h"
#include "ui_updater.h"
#include <QNetworkAccessManager>
#include <QJsonArray>
#include <QJsonObject>
#include <QNetworkReply>
#include <QMessageBox>
Updater::Updater(QWidget *parent) : QDialog(parent),
                                    ui(new Ui::Updater)
{
    ui->setupUi(this);
    qDebug() << "更新界面已打开";
}

Updater::~Updater()
{
    delete ui;
    qDebug() << "更新界面将关闭";
}

void Updater::fetchRemote()
{
}

void Updater::checkForUpdates(const QString &currentVersion = VERSION)
{
    // 1. 在静态函数内部创建一个临时 Manager
    // 注意：由于是静态函数，manager 需要手动管理生命周期
    auto *manager = new QNetworkAccessManager();

    QUrl url("https://data.jsdelivr.com/v1/packages/gh/Pillowe582/toolkit");
    QNetworkRequest request(url);

    // 2. 发起请求
    QNetworkReply *reply = manager->get(request);

    // 3. 核心逻辑：监听完成信号
    QObject::connect(reply, &QNetworkReply::finished, [=]()
                     {
        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            QJsonArray versions = doc.object()["versions"].toArray();
            if (!versions.isEmpty()) {
                QString latestTag = versions.first()["version"].toString();
                // 4. 对比版本
                qDebug() << "当前版本：" << currentVersion;
                qDebug() << "最新版本：" << latestTag;
                if (latestTag > currentVersion) {
                    // 这里可以发出一个全局信号，或者弹出一个简单的提示框
                    QMessageBox::information(nullptr, "发现新版本 v" + latestTag, "可前往GitHub下载新版本              ");
                }
            }
        }

        // 5. 任务完成，自清理：先删 reply，再删 manager
        reply->deleteLater();
        manager->deleteLater(); });
}