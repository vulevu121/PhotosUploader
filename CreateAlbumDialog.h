#ifndef CREATEALBUMDIALOG_H
#define CREATEALBUMDIALOG_H

#include <QDialog>
#include <QDebug>


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
    void emitCreateAlbumSignal();
signals:
    void createAlbumSignal(QString const &name, QString const &desc);
private:
    Ui::CreateAlbumDialog *ui;
};

#endif // CREATEALBUMDIALOG_H
