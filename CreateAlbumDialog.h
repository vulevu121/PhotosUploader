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
//    bool useExistingAlbum;

public slots:
    void emitCreateAlbumSignal();
    void toggleExistingAlbumOption(bool on);
    void toggleNewAlbumOption(bool on);
    void setExistingAlbumId(QString const &id);

signals:
    void createAlbumSignal(QString const &name, QString const &desc, QString const &albumId, bool useExistingAlbum);

private:
    Ui::CreateAlbumDialog *ui;
};

#endif // CREATEALBUMDIALOG_H
