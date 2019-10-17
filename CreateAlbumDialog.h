#ifndef CREATEALBUMDIALOG_H
#define CREATEALBUMDIALOG_H

#include <QDialog>
#include <QDebug>
#include <QString>

namespace Ui {
class CreateAlbumDialog;
}

class CreateAlbumDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateAlbumDialog(QWidget *parent = nullptr);
    ~CreateAlbumDialog();

public slots:
    void emitAlbumSignal();
    void toggleExistingAlbumOption(bool on);
    void toggleNewAlbumOption(bool on);
    void setExistingAlbum(QString const &id,QString const &name);

signals:
    void createAlbumSignal(QString const &name, QString const &desc);
    void existingAlbumSignal(QString const &id);

private:
    Ui::CreateAlbumDialog *ui;
    QString lastAlbumName;
    QString lastAlbumId;
};

#endif // CREATEALBUMDIALOG_H
