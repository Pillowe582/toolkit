#include "mainwindow.h"
#include "database.h"
#include <QApplication>
#include <QSharedMemory>
#include <QMessageBox>
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/assets/MainIcon.ico"));

    QSharedMemory sharedmemory;
    sharedmemory.setKey("Pillowe's Toolkit");
    if (sharedmemory.attach())
    {
        QMessageBox::information(nullptr, "替身？！", "请勿重复打开本软件！           ");
        return 1;
    }
    if (!sharedmemory.create(1))
    {
        QMessageBox::critical(nullptr, "共享内存创建失败！", "bug是凉爽的夏夜，可供人无忧地安眠。");
        return 2;
    }
    MainWindow window;
    window.setWindowOpacity(0);
    window.show();
    window.setWindowOpacity(1);
    qDebug() << window.timer.elapsed() << "窗口显示完毕";
    int result = app.exec();
    system("pause");
    return result;
}