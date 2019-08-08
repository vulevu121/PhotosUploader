#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QSettings>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();

    Ui::SettingsDialog *ui;
private:
    QSettings *settings = new QSettings("Pixyl", "PixylLibrary");
public slots:
    void saveSettings();
signals:
    void settingsSaved();
};

#endif // SETTINGSDIALOG_H
