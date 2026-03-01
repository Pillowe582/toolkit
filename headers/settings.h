#ifndef SETTINGS_H
#define SETTINGS_H
#include <QDialog>
#include "database.h"
namespace Ui
{
    class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = nullptr, Database *db = nullptr);
    ~Settings();

private:
    Ui::Settings *ui;
    bool saveSettings(Database *db);

signals:
    void settingsSaved();
};

#endif