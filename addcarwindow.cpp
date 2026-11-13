#include "addcarwindow.h"
#include "ui_addcarwindow.h"
#include <QMessageBox>

AddCarWindow::AddCarWindow(UserManager* usermanager, CarManager* carmanager, RentalManager* rentalmanager, QWidget* adminWindow, const QString& carId, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AddCarWindow)
    , usermanager(usermanager)
    , carmanager(carmanager)
    , rentalmanager(rentalmanager)
    , adminWindow(adminWindow)
    , carId(carId)
{
    ui->setupUi(this);

    Car* car = carmanager->getCarById(carId);
    if(car) {
        ui->txtBrand->setText(car->getBrand());
        ui->txtModel->setText(car->getModel());
        ui->numYear->setValue(car->getManufacturerYear());
        ui->cmbCategory->setCurrentText(car->getCategoryString());
        ui->cmbStatus->setCurrentText(car->getStatusString());
        ui->numSeats->setValue(car->getSeats());
        ui->numMileage->setValue(car->getMileage());
        ui->numPrice->setValue(car->getRentPrice());
    }
}

AddCarWindow::~AddCarWindow()
{
    delete ui;
}

void AddCarWindow::on_btnCancel_clicked()
{
    this->close();
}


void AddCarWindow::on_btnAdd_clicked()
{
    QString brand = ui->txtBrand->text();
    QString model = ui->txtModel->text();
    int manufacturerYear = ui->numYear->value();
    Category category = Car::stringToCategory(ui->cmbCategory->currentText());
    Status status = Car::stringToStatus(ui->cmbStatus->currentText());
    int seats = ui->numSeats->value();
    double mileage = ui->numMileage->value();
    double rentPrice = ui->numPrice->value();

    try {
        if(carId.isEmpty()) {
            carmanager->addCar(brand, model, category, manufacturerYear, seats, rentPrice, mileage, status);
        }
        else {
            Car* car = carmanager->getCarById(carId);
            if (!car) {
                QMessageBox::warning(this, "Помилка", "Автомобіль не знайдено. Можливо, його вже видалено.");
                this->close();
                return;
            }
            car->setBrand(brand);
            car->setModel(model);
            car->setCategory(category);
            car->setManufacturerYear(manufacturerYear);
            car->setSeats(seats);
            car->setRentPrice(rentPrice);
            car->setMileage(mileage);
            car->setStatus(status);
            carmanager->saveToJson();
        }
    }
    catch (const AppException& ex) {
        QMessageBox::warning(this, "Некоректні дані", ex.what());
        return;
    }

    this->close();

}

