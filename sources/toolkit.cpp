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
    // 加载列表数据
    loadList();
    selectRow(0);

    // 窗口设置
    setupTray();
    setWindowTitle(QString("Pillowe's Toolkit v%1").arg(VERSION));
    setWindowIcon(QIcon(":/assets/MainIcon.ico"));

    // 绑定各种信号和槽
    connect(ui->changelog, &QAction::triggered, this, &MainWindow::showChangelog);
    connect(ui->ngguu, &QAction::triggered, this, &MainWindow::showSurprise);
    connect(ui->appendbtn, &QPushButton::clicked, this, [this]()
            { addItem(ui->itemlist->currentIndex().row() + 1); });
    connect(ui->removebtn, &QPushButton::clicked, this, [this]()
            { removeItem(ui->itemlist->currentIndex().row()); });
    connect(ui->itemlist->selectionModel(), &QItemSelectionModel::currentRowChanged, this, [this](const QModelIndex &current, const QModelIndex &previous)
            { onCurrentRowChanged(current, previous); });
    connect(ui->upbtn, &QPushButton::clicked, this, [this]()
            { swapItems(ui->itemlist->currentIndex().row(), -1); });
    connect(ui->downbtn, &QPushButton::clicked, this, [this]()
            { swapItems(ui->itemlist->currentIndex().row(), 1); });
}

MainWindow::~MainWindow()
{
    qDebug() << "主界面将关闭";
    saveSort();
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

void MainWindow::addItem(int targetRow)
{
    qDebug() << "开始添加项目";
    if (!db->model->insertRow(targetRow))
    {
        qDebug() << "添加项目失败：" << db->model->lastError().text();
        return;
    }
    db->model->setData(db->model->index(targetRow, 1), "新增项");
    db->model->setData(db->model->index(targetRow, 2), 0);
    db->model->setData(db->model->index(targetRow, 4), ":/assets/MainIcon.ico");
    saveSort();
    selectRow(targetRow);
    qDebug() << "项目已添加于 " << targetRow << " 行";
}

void MainWindow::removeItem(int targetRow)
{
    if (QMessageBox::warning(this, "删除项目？", "这样将会永久失去这一项！（真的很久！）") != QMessageBox::Yes)
        return;
    qDebug() << "开始删除项目";
    if (!db->model->removeRow(targetRow))
    {
        qDebug() << "删除项目失败：" << db->model->lastError().text();
        return;
    }
    saveSort();
    qDebug() << "第 " << targetRow << " 行已删除";
}

void MainWindow::saveSort()
{
    for (int i = 0; i < db->model->rowCount(); i++)
        db->model->setData(db->model->index(i, 8), i);
    qDebug() << "正在保存至数据库";
    if (db->model->submitAll())
    {
        qDebug() << "保存成功";
        return;
    }
    QMessageBox::critical(this, "数据库保存失败", "bug是凉爽的夏夜，可供人无忧地安眠。");
    qDebug() << "保存失败：" << db->model->lastError().text();
    return;
}

void MainWindow::swapItems(int currentRow, int direction)
{
    int targetRow = currentRow + direction;
    if (targetRow < 0 || targetRow >= db->model->rowCount())
    {
        return;
    }
    db->model->setData(db->model->index(currentRow, 8), targetRow);
    db->model->setData(db->model->index(targetRow, 8), currentRow);
    db->model->submitAll();
    selectRow(targetRow);
}
void MainWindow::onCurrentRowChanged(const QModelIndex &current, const QModelIndex &previous)
{
    qDebug() << "当前行已切换至 " << current.row();
    ui->titleinput->setPlainText(db->model->data(db->model->index(current.row(), 1)).toString());
    ui->noteinput->setPlainText(db->model->data(db->model->index(current.row(), 5)).toString());
    ui->sitelbl->setText(db->model->data(db->model->index(current.row(), 6)).toString());
    ui->pathlbl->setPlainText(db->model->data(db->model->index(current.row(), 7)).toString());
}

void MainWindow::selectRow(int row)
{
    QModelIndex nextSelection = db->model->index(row, 1);
    ui->itemlist->setCurrentIndex(nextSelection);
    ui->itemlist->selectionModel()->select(nextSelection, QItemSelectionModel::ClearAndSelect);
}