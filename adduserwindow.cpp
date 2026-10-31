#include "adduserwindow.h"
#include "ui_adduserwindow.h"
#include "appexception.h"
#include <QMessageBox>

AddUserWindow::AddUserWindow(UserManager* usermanager, QWidget* adminWindow, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AddUserWindow)
    , usermanager(usermanager)
    , adminWindow(adminWindow)
{
    ui->setupUi(this);
}

AddUserWindow::~AddUserWindow()
{
    delete ui;
}

void AddUserWindow::on_btnCancel_clicked()
{
    this->close();
}

void AddUserWindow::on_btnAdd_clicked()
{
    QString name = ui->txtName->text();
    QString email = ui->txtEmail->text();
    QString password = ui->txtPassword->text();
    Role role = User::stringToRole(ui->cmbRole->currentText());

    try {
        usermanager->addUser(name, email, password, role);
    }
    catch (const AppException& ex) {
        QMessageBox::warning(this, "Некоректні дані", ex.what());
        return;
    }

    this->close();
}
