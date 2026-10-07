#include "ui_userwindow.h"
#include "userwindow.h"

UserWindow::UserWindow(UserManager* manager, QWidget* loginWindow, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::UserWindow)
    , manager(manager)
    , loginWindow(loginWindow)
{
    ui->setupUi(this);
    if (manager && manager->getCurrentUser()) {
        User* currentUser = manager->getCurrentUser();
        ui->lblProfileEmail->setText("Email: <b>" + currentUser->getEmail() + "</b>");
        ui->lblProfileName->setText("Ім'я: <b>" + currentUser->getName() + "</b>");
    }
}

UserWindow::~UserWindow()
{
    delete ui;
}


void UserWindow::on_btnCatalog_clicked() {
    ui->stackedWidget->setCurrentIndex(0);
}

void UserWindow::on_btnMyRentals_clicked() {
    ui->stackedWidget->setCurrentIndex(1);
}

void UserWindow::on_btnProfile_clicked() {
    ui->stackedWidget->setCurrentIndex(2);
}

void UserWindow::on_btnLogout_clicked() {
    if (manager) {
        manager->logout();
    }

    if (loginWindow)
    {
        loginWindow->show();
    }

    this->close();
}