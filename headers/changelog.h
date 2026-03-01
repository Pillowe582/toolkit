#ifndef CHANGELOG_H
#define CHANGELOG_H

#include <QDialog>

// 这里的 Ui::Changelog 是由编译工具根据 assets/changelog.ui 自动生成的
namespace Ui
{
    class Changelog;
}

class Changelog : public QDialog
{
    Q_OBJECT

public:
    explicit Changelog(QWidget *parent = nullptr);
    ~Changelog();

private:
    Ui::Changelog *ui;
    void showUpdater();
};

#endif // CHANGELOG_H