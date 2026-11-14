#include "registerdialog.h"
#include "ui_registerdialog.h"
#include <QMessageBox>

RegisterDialog::RegisterDialog(UserManager* usermanager, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegisterDialog)
    , usermanager(usermanager)
{
    ui->setupUi(this);
}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}




void RegisterDialog::on_btnRegister_clicked()
{
    QString name = ui->txtName->text();
    QString email = ui->txtEmail->text();
    QString password = ui->txtPassword->text();
    QString confirmPassword = ui->txtConfirmPassword->text();

    if (password != confirmPassword) {
        QMessageBox::warning(this, "Реєстрація", "Паролі не збігаються.");
        return;
    }

    try {
        usermanager->addUser(name, email, password, Role::Customer);
    }
    catch (const AppException& ex) {
        QMessageBox::warning(this, "Некоректні дані", ex.what());
        return;
    }

    QMessageBox::information(this, "Реєстрація", "Ви успішно зареєструвались. Тепер увійдіть в обліковий запис.");

    accept();
}


void RegisterDialog::on_btnCancel_clicked()
{
    reject();
}

