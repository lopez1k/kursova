#include "adminwindow.h"
#include "QDebug"
#include "ui_adminwindow.h"

AdminWindow::AdminWindow(UserManager* usermanager, CarManager* carmanager, RentalManager* rentalmanager, QWidget* loginWindow, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AdminWindow)
    , usermanager(usermanager)
    , carmanager(carmanager)
    , rentalmanager(rentalmanager)
    , loginWindow(loginWindow)
{
    ui->setupUi(this);
    if (usermanager && usermanager->getCurrentUser()) {
        User* currentUser = usermanager->getCurrentUser();
        ui->welcomeLabel->setText("<b>Вітаємо</b>, " + currentUser->getName() + "!");
    }
    loadUsersTable();
    carmanager->loadCarsTable(ui->carsTable, false);
    rentalmanager->loadRentalsTable(ui->rentalsTable);
}

AdminWindow::~AdminWindow()
{
    delete ui;
}



void AdminWindow::on_btnCars_clicked()
{
    carmanager->loadCarsTable(ui->carsTable, false);
    ui->stackedWidget->setCurrentIndex(0);
}

void AdminWindow::on_btnUsers_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void AdminWindow::on_btnRentals_clicked()
{
    rentalmanager->loadRentalsTable(ui->rentalsTable);
    ui->stackedWidget->setCurrentIndex(2);
}

void AdminWindow::on_btnLogout_clicked()
{
    if (usermanager) {
        usermanager->logout();
    }
    if (this->loginWindow) {
        this->loginWindow->show();
    }
    this->close();
}



void AdminWindow::loadUsersTable() const {
    if (!usermanager) {
        return;
    }

    const QList<User>& users = usermanager->getAllUsers();
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

