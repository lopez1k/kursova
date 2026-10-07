#include "adminwindow.h"
#include "ui_adminwindow.h"

AdminWindow::AdminWindow(UserManager* manager, QWidget* loginWindow, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AdminWindow)
    , manager(manager)
    , loginWindow(loginWindow)
{
    ui->setupUi(this);
    if (manager && manager->getCurrentUser()) {
        User* currentUser = manager->getCurrentUser();
        ui->welcomeLabel->setText("<b>Вітаємо</b>, " + currentUser->getName() + "!");
    }
    loadUsersTable();
}

AdminWindow::~AdminWindow()
{
    delete ui;
}



void AdminWindow::on_btnCars_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void AdminWindow::on_btnUsers_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void AdminWindow::on_btnRentals_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void AdminWindow::on_btnLogout_clicked()
{
    if (manager) {
        manager->logout();
    }
    if (this->loginWindow) {
        this->loginWindow->show();
    }
    this->close();
}



void AdminWindow::loadUsersTable() const {
    if (!manager) {
        return;
    }

    const QList<User>& users = manager->getAllUsers();
    ui->usersTable->setColumnCount(4);
    ui->usersTable->setRowCount(users.size());
    for (int i = 0; i < users.size(); ++i) {
        const User& user = users[i];
        ui->usersTable->setItem(i, 0, new QTableWidgetItem(user.getId()));
        ui->usersTable->setItem(i, 1, new QTableWidgetItem(user.getName()));
        ui->usersTable->setItem(i, 2, new QTableWidgetItem(user.getEmail()));
        ui->usersTable->setItem(i, 3, new QTableWidgetItem(user.getRoleString()));
    }

}
