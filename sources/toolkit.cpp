#include "mainwindow.h"
#include "database.h"
#include "ui_toolkit.h"
#include <QDebug>
#include <QDesktopServices>
#include <QUrl>
#include <QIcon>
#include <QSqlError>
#include <QMessageBox>

// MARK: -Basic Functions
// 必须写成 MainWindow:: 否则编译器认为这是个全局函数，而不是类的成员
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qDebug() << "主界面已打开";

    // 初始化数据库
    db = new Database();
    if (!db->init())
    {
        QMessageBox::critical(this, "数据库初始化失败", "bug是凉爽的夏夜，可供人无忧地安眠。");
    }

    setupTray();
    // 窗口设置
    setWindowTitle(QString("Pillowe's Toolkit v%1").arg(VERSION));
    setWindowIcon(QIcon(":/assets/MainIcon.ico"));

    // 绑定各种信号和槽
    connect(ui->changelog, &QAction::triggered, this, &MainWindow::showChangelog);
    connect(ui->ngguu, &QAction::triggered, this, &MainWindow::showSurprise);
    connect(ui->appendbtn, &QPushButton::clicked, this, &MainWindow::addItem);

    // 加载列表数据
    loadList();
}

MainWindow::~MainWindow()
{
    qDebug() << "主界面将关闭";
    delete db; // 清理数据库对象
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    event->ignore();
    minimizeToTray();
}

// MARK: -Changelog
void MainWindow::showChangelog()

{
    qDebug() << "正在打开更新日志";
    if (changelog == nullptr)
    {
        changelog = new Changelog(this);
    }
    if (changelog->exec() == QDialog::Rejected)
    {
        qDebug() << "更新日志已关闭";
    }
    changelog->raise();
    changelog->activateWindow();
}

// MARK: -Tray
void MainWindow::minimizeToTray()
{
    qDebug() << "正在最小化到托盘";
    if (!hasMinimizeNoticed)
    {
        tray->showMessage("已最小化到托盘", "本次运行不再提醒");
        hasMinimizeNoticed = true;
    }
    hide();
}

void MainWindow::onTrayClicked(QSystemTrayIcon::ActivationReason reason)
{

    switch (reason)
    {
    case QSystemTrayIcon::Trigger:
        qDebug() << "托盘图标被点击";
        if (isVisible())
        {
            minimizeToTray();
        }
        else
        {
            show();
        }
        break;
    default:
        break;
    }
}

void MainWindow::setupTray()
{
    tray = new QSystemTrayIcon(this);
    tray->setIcon(QIcon(":/assets/MainIcon.ico"));
    tray->setToolTip("Pillowe's Toolkit");
    tray->setVisible(true);
    QMenu *menu = new QMenu(this);
    // 菜单项
    QAction *restore = new QAction("恢复主窗口", this);
    connect(restore, &QAction::triggered, this, &MainWindow::show);
    QAction *quit = new QAction("退出", this);
    connect(quit, &QAction::triggered, this, &QApplication::quit);
    QAction *surprise = new QAction("不要点击", this);
    connect(surprise, &QAction::triggered, this, &MainWindow::showSurprise);
    QAction *minimize = new QAction("最小化", this);
    connect(minimize, &QAction::triggered, this, &MainWindow::minimizeToTray);
    QAction *settings = new QAction("设置...", this);
    // connect(settings, &QAction::triggered, this, &MainWindow::showSettings);
    QAction *changelog = new QAction("更新日志...", this);
    connect(changelog, &QAction::triggered, this, &MainWindow::showChangelog);
    menu->addAction(restore);
    menu->addAction(minimize);
    menu->addSeparator();
    menu->addAction(settings);
    menu->addAction(changelog);
    menu->addSeparator();
    menu->addAction(surprise);
    menu->addAction(quit);
    tray->setContextMenu(menu);
    connect(tray, &QSystemTrayIcon::activated, this, &MainWindow::onTrayClicked);
}

// MARK: -Surprise
void MainWindow::showSurprise()

{
    qDebug() << "正在打开彩蛋";
    QDesktopServices::openUrl(QUrl("https://vdse.bdstatic.com//192d9a98d782d9c74c96f09db9378d93.mp4"));
}

// MARK: -List
void MainWindow::loadList()
{
    qDebug() << "开始加载列表";
    ui->itemlist->setModel(db->model);
    ui->itemlist->setModelColumn(1);
    qDebug() << "权限检查：" << db->model->flags(db->model->index(0, 1));
    qDebug() << "列表已加载完毕";
}

void MainWindow::addItem()
{
    qDebug() << "开始添加项目";
    if (!db->model->insertRow(0))
    {
        qDebug() << "添加项目失败：" << db->model->lastError().text();
        return;
    }
    db->model->setData(db->model->index(0, 1), "新增项");
    db->model->setData(db->model->index(0, 2), 0);
    db->model->setData(db->model->index(0, 4), ":/assets/MainIcon.ico");

    saveSort();
    qDebug() << "项目已添加";
}

void MainWindow::saveSort()
{
    db->model->submitAll();
    for (int i = 0; i < db->model->rowCount(); i++)
        db->model->setData(db->model->index(i, 8), i);
    qDebug() << "正在保存至数据库";
    db->model->submitAll();
}