#ifndef UPDATER_H
#define UPDATER_H
#include <QDialog>
namespace Ui
{
    class Updater;
}
class Updater : public QDialog
{
    Q_OBJECT
public:
    explicit Updater(QWidget *parent = nullptr);
    ~Updater();
    static void checkForUpdates(const QString &currentVersion = VERSION, bool alert = false);

private:
    Ui::Updater *ui;
    void fetchRemote();
};

#endif