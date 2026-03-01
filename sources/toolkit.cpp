#include "mainwindow.h"
#include "database.h"
#include "ui_toolkit.h"
#include <QDebug>
#include <QDesktopServices>
#include <QUrl>
#include <QIcon>
#include <QSqlError>
#include <QMessageBox>
#include <QFileDialog>
#include <QClipboard>
#include <QTimer>
#include <QSettings>

// MARK: -Basic Functions
// 必须写成 MainWindow:: 否则编译器认为这是个全局函数，而不是类的成员
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    timer.start();
    ui->setupUi(this);
    qDebug() << timer.elapsed() << "主界面已打开";

    // 初始化数据库
    db = new Database();
    if (!db->init())
    {
        QMessageBox::critical(this, "数据库初始化失败", "bug是凉爽的夏夜，可供人无忧地安眠。");
    }
    qDebug() << timer.elapsed() << "数据库初始化完毕";
    // 加载列表数据
    loadList();
    selectRow(0);
    qDebug() << timer.elapsed() << "列表数据加载完毕";

    // 窗口设置
    setupTray();
    setWindowTitle(QString("Pillowe's Toolkit v%1").arg(VERSION));
    setWindowIcon(QIcon(":/assets/MainIcon.ico"));
    qDebug() << timer.elapsed() << "窗口设置完毕";
    // 绑定各种信号和槽
    connect(ui->changelog, &QAction::triggered, this, &MainWindow::showChangelog);
    connect(ui->settings, &QAction::triggered, this, &MainWindow::showSettings);
    connect(ui->filebtn, &QPushButton::clicked, this, [this]()
            { openFileDialog(0); });
    connect(ui->folderbtn, &QPushButton::clicked, this, [this]()
            { openFileDialog(1); });
    connect(ui->ngguu, &QAction::triggered, this, []()
            { QDesktopServices::openUrl(QUrl("https://vdse.bdstatic.com//192d9a98d782d9c74c96f09db9378d93.mp4")); });
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
    connect(ui->titleinput, &QPlainTextEdit::textChanged, this, [this]()
            { db->itemsModel->setData(db->itemsModel->index(ui->itemlist->currentIndex().row(), 1), ui->titleinput->toPlainText()); });
    connect(ui->noteinput, &QPlainTextEdit::textChanged, this, [this]()
            { db->itemsModel->setData(db->itemsModel->index(ui->itemlist->currentIndex().row(), 5), ui->noteinput->toPlainText()); });
    connect(ui->pastebtn, &QPushButton::clicked, this, &MainWindow::pasteClipboard);
    connect(ui->executebtn, &QPushButton::clicked, this, [this]()
            { QDesktopServices::openUrl(QUrl::fromLocalFile(db->itemsModel->data(db->itemsModel->index(ui->itemlist->currentIndex().row(), 7)).toString())); });
    qDebug()
        << timer.elapsed() << "信号与槽绑定完毕";

    readSettings();
}

MainWindow::~MainWindow()
{
    qDebug() << timer.elapsed() << "主界面将关闭";
    saveSort();
    delete db; // 清理数据库对象
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (closeMinimize)
    {
        event->ignore();
        minimizeToTray();
    }
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::ActivationChange)
    {
        QWidget *activeWindow = QApplication::activeWindow();

        if (activeWindow == nullptr)
        {
            qDebug() << timer.elapsed() << "失去焦点";
            if (focusOutMinimize)
            {
                minimizeToTray();
            }
        }
    }
    QMainWindow::changeEvent(event);
}
// MARK: -Changelog
void MainWindow::showChangelog()

{
    qDebug() << timer.elapsed() << "正在打开更新日志";
    if (changelog == nullptr)
    {
        changelog = new Changelog(this);
    }
    if (changelog->exec() == QDialog::Rejected)
    {
        qDebug() << timer.elapsed() << "更新日志已关闭";
    }
    changelog->raise();
    changelog->activateWindow();
}

// MARK: Settings
void MainWindow::showSettings()
{
    qDebug() << timer.elapsed() << "正在打开设置";
    if (settings == nullptr)
    {
        settings = new Settings(this, db);
        connect(settings, &Settings::settingsSaved, this, [this]()
                { readSettings(); });
    }
    settings->raise();
    settings->activateWindow();
    if (settings->exec())
        qDebug() << timer.elapsed() << "设置已关闭";
}

void MainWindow::setExecuteOnStart(bool autoStart)
{

    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    if (autoStart)
    {
        QString path = QString("\"%1\"").arg(QDir::toNativeSeparators(QCoreApplication::applicationFilePath()));
        settings.setValue("Pillowe's Toolkit", path);
    }
    else
    {
        settings.remove("Pillowe's Toolkit");
    }
    qDebug() << timer.elapsed() << "已设置开机启动状态";
}
// MARK: -Tray
void MainWindow::minimizeToTray()
{
    qDebug() << timer.elapsed() << "正在最小化到托盘";
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
        qDebug() << timer.elapsed() << "托盘图标被点击";
        if (isVisible())
        {
            minimizeToTray();
        }
        else
        {
            setWindowOpacity(0);
            show();
            QTimer::singleShot(0, this, [this]()
                               { setWindowOpacity(1); }); // 防止窗口激活时的短暂白屏
            raise();
            activateWindow();
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
    connect(surprise, &QAction::triggered, this, []()
            { QDesktopServices::openUrl(QUrl("https://vdse.bdstatic.com//192d9a98d782d9c74c96f09db9378d93.mp4")); });
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

    qDebug() << timer.elapsed() << "托盘设置完毕";
}

// MARK: -List
void MainWindow::loadList()
{

    qDebug() << timer.elapsed() << "开始加载列表";
    static IconProxyModel *proxy = new IconProxyModel(this);
    proxy->setSourceModel(db->itemsModel);
    ui->itemlist->setModel(proxy);
    ui->itemlist->setModelColumn(1);
    ui->itemlist->setIconSize(QSize(24, 24));
    ui->itemlist->setSpacing(2);
    // qDebug() << timer.elapsed() << "权限检查：" << db->itemsModel->flags(db->itemsModel->index(0, 1));
    qDebug() << timer.elapsed() << "列表已加载完毕";
}

void MainWindow::addItem(int targetRow)
{
    if (!db->itemsModel->insertRow(targetRow))
    {
        qDebug() << timer.elapsed() << "添加项目失败：" << db->itemsModel->lastError().text();
        return;
    }
    db->itemsModel->setData(db->itemsModel->index(targetRow, 1), "新增项");
    db->itemsModel->setData(db->itemsModel->index(targetRow, 2), 0);
    db->itemsModel->setData(db->itemsModel->index(targetRow, 4), ":/assets/MainIcon.ico");
    saveSort();
    selectRow(targetRow);
    qDebug() << timer.elapsed() << "项目已添加于 " << targetRow << " 行";
}

void MainWindow::removeItem(int targetRow)
{
    QString title = db->itemsModel->data(db->itemsModel->index(targetRow, 1)).toString();
    if (QMessageBox::warning(this, QString("删除项目 %1 ？").arg(title), "这样将会永久失去这一项！（真的很久！）", QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
        return;
    if (!db->itemsModel->removeRow(targetRow))
    {
        qDebug() << timer.elapsed() << "删除项目失败：" << db->itemsModel->lastError().text();
        return;
    }
    saveSort();
    qDebug() << timer.elapsed() << "第 " << targetRow << " 行已删除";
}

void MainWindow::saveSort()
{
    for (int i = 0; i < db->itemsModel->rowCount(); i++)
        db->itemsModel->setData(db->itemsModel->index(i, 8), i);
    qDebug() << timer.elapsed() << "正在保存至数据库";
    if (db->itemsModel->submitAll())
    {
        return;
    }
    QMessageBox::critical(this, "数据库保存失败", "bug是凉爽的夏夜，可供人无忧地安眠。");
    qDebug() << timer.elapsed() << "保存失败：" << db->itemsModel->lastError().text();
    return;
}

void MainWindow::swapItems(int currentRow, int direction)
{
    int targetRow = currentRow + direction;
    if (targetRow < 0 || targetRow >= db->itemsModel->rowCount())
    {
        return;
    }
    db->itemsModel->setData(db->itemsModel->index(currentRow, 8), targetRow);
    db->itemsModel->setData(db->itemsModel->index(targetRow, 8), currentRow);
    db->itemsModel->submitAll();
    selectRow(targetRow);
}
void MainWindow::onCurrentRowChanged(const QModelIndex &current, const QModelIndex &previous)
{

    ui->titleinput->setPlainText(db->itemsModel->data(db->itemsModel->index(current.row(), 1)).toString());
    ui->noteinput->setPlainText(db->itemsModel->data(db->itemsModel->index(current.row(), 5)).toString());
    ui->sitelbl->setHtml(QString("<a href=\"%1\">%1</a>").arg(db->itemsModel->data(db->itemsModel->index(current.row(), 6)).toString()));
    QString path = db->itemsModel->data(db->itemsModel->index(current.row(), 7)).toString();
    ui->pathlbl->setPlainText(path);
    QIcon icon = iconProvider.icon(QFileInfo(path));
    ui->executebtn->setIcon(icon);
    qDebug() << timer.elapsed() << "当前行已切换至 " << current.row();
}

void MainWindow::selectRow(int row)
{
    QModelIndex nextSelection = db->itemsModel->index(row, 1);
    ui->itemlist->setCurrentIndex(nextSelection);
    ui->itemlist->selectionModel()->select(nextSelection, QItemSelectionModel::ClearAndSelect);
}

// MARK: -Input User Data
void MainWindow::pasteClipboard()
{
    QClipboard *clipboard = QApplication::clipboard();
    QString text = clipboard->text().toHtmlEscaped();
    qDebug() << timer.elapsed() << "剪贴板内容：" << text;
    if (text.isEmpty())
    {
        QMessageBox::information(this, "滚木", "剪贴板无有效内容               ");
        return;
    }
    if (QMessageBox::question(this, "粘贴？！", "要粘贴并覆盖吗                 ", QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
    {

        db->itemsModel->setData(db->itemsModel->index(ui->itemlist->currentIndex().row(), 6), text);
        db->itemsModel->submitAll();
        ui->sitelbl->setHtml(QString("<a href=\"%1\">%1</a>").arg(text));
        return;
    }
}

void MainWindow::openFileDialog(int type)
{
    QString path;
    switch (type)
    {
    case 0:
        path = QFileDialog::getOpenFileName(
            this,
            "选择文件",
            QDir::homePath(),
            "所有文件 (*)",
            nullptr,
            QFileDialog::DontUseNativeDialog);
        break;
    case 1:
        path = QFileDialog::getExistingDirectory(
            this,
            "选择文件夹",
            QDir::homePath(),
            QFileDialog::DontUseNativeDialog);
        break;
    default:
        break;
    }

    if (path.isEmpty())
    {
        return;
    }
    int currentRow = ui->itemlist->currentIndex().row();

    db->itemsModel->setData(db->itemsModel->index(currentRow, 7), path);
    db->itemsModel->submitAll();
    selectRow(currentRow);
    ui->pathlbl->setPlainText(path);
}

void MainWindow::readSettings()
{
    // 读取设置
    executeOnStart = db->getSetting("executeOnStart", false).toBool();
    setExecuteOnStart(executeOnStart);
    focusOutMinimize = db->getSetting("focusOutMinimize", true).toBool();
    closeMinimize = db->getSetting("closeMinimize", true).toBool();
}