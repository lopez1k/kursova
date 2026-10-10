#include "ui_userwindow.h"
#include "userwindow.h"
#include "QMessageBox"
#include "rentdialog.h"

UserWindow::UserWindow(UserManager* usermanager, CarManager* carmanager, RentalManager* rentalmanager, QWidget* loginWindow, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::UserWindow)
    , usermanager(usermanager)
    , carmanager(carmanager)
    , rentalmanager(rentalmanager)
    , loginWindow(loginWindow)
{
    ui->setupUi(this);
    if (usermanager && usermanager->getCurrentUser()) {
        User* currentUser = usermanager->getCurrentUser();
        ui->lblProfileEmail->setText("Email: <b>" + currentUser->getEmail() + "</b>");
        ui->lblProfileName->setText("Ім'я: <b>" + currentUser->getName() + "</b>");
        rentalmanager->loadRentalsTable(ui->myRentalsTable, currentUser->getId());
    }
    carmanager->loadCarsList(ui->carsListWidget);

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
    if (usermanager) {
        usermanager->logout();
    }

    if (loginWindow)
    {
        loginWindow->show();
    }

    this->close();
}

void UserWindow::on_carsListWidget_itemDoubleClicked(QListWidgetItem *item) {
    if (!item) return;
    User* user = usermanager->getCurrentUser();
    if (!user) {
        return;
    }
    qDebug() << item->data(Qt::UserRole).toString();
    QString carId = item->data(Qt::UserRole).toString();

    Car* selectedCar = carmanager->getCarById(carId);

    if (!selectedCar) {
        QMessageBox::warning(this, "Помилка", "Автомобіль з таким ID не знайдено в базі!");
        return;
    }

    RentDialog dialog(*selectedCar, this);

    if (dialog.exec() == QDialog::Accepted) {
        QDate startDate = dialog.getStartDate();
        QDate endDate = dialog.getEndDate();
        double totalPrice = dialog.getTotalPrice();

        QDateTime end(dialog.getEndDate(), QTime(23, 59));

        rentalmanager->rentCar(user->getId(), selectedCar->getId(), end);
        bool success = true; // або ваша логіка збереження оренди в БД/файл

        if (success) {
            QMessageBox::information(this, "Успіх",
                                     QString("Автомобіль успішно заброньовано на %1 днів!\nСума: %2 грн")
                                         .arg(startDate.daysTo(endDate))
                                         .arg(totalPrice, 0, 'f', 2));

            carmanager->loadCarsList(ui->carsListWidget, true);
            rentalmanager->loadRentalsTable(ui->myRentalsTable, user->getId());
        } else {
            QMessageBox::warning(this, "Помилка", "Не вдалося оформити бронювання.");
        }
    }
}