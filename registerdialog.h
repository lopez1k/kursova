#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H


#include <QDialog>
#include "usermanager.h"

namespace Ui {
class RegisterDialog;
}

class RegisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDialog(UserManager* usermanager, QWidget *parent = nullptr);
    ~RegisterDialog();

private slots:

    void on_btnRegister_clicked();
    void on_btnCancel_clicked();

private:
    Ui::RegisterDialog *ui;
    UserManager* usermanager;
};

#endif // REGISTERDIALOG_H
