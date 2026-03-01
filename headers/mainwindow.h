#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include "changelog.h"
#include <QSystemTrayIcon>
#include "database.h"
QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    Changelog *changelog = nullptr;
    QSystemTrayIcon *tray;
    Database *db;
    void setupTray();
    bool hasMinimizeNoticed = false;
    void loadList();
    void addItem(int);
    void removeItem(int);
    void saveSort();
    void swapItems(int, int);
    void selectRow(int);
private slots:
    void showChangelog();
    void minimizeToTray();
    void onTrayClicked(QSystemTrayIcon::ActivationReason reason);
    void pasteClipboard();

protected:
    void closeEvent(QCloseEvent *event) override;
    void onCurrentRowChanged(const QModelIndex &current, const QModelIndex &previous);
};

#endif