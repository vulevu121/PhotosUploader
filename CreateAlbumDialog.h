#ifndef CREATEALBUMDIALOG_H
#define CREATEALBUMDIALOG_H

#include <QDialog>

namespace Ui {
class CreateAlbumDialog;
}

class CreateAlbumDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateAlbumDialog(QWidget *parent = nullptr);
    ~CreateAlbumDialog();

private:
    Ui::CreateAlbumDialog *ui;
};

#endif // CREATEALBUMDIALOG_H
