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
    loadCarsList(ui->carsListWidget, carmanager->getAllCars(), true);
    QList<Car> cars = carmanager->getAllCars();
    double maxRentPrice = 0.0;
    for(const Car& car : cars) {
        if (car.getStatus() != Status::Available) {
            continue;
        }
        if (car.getRentPrice() > maxRentPrice) {
            maxRentPrice = car.getRentPrice();
        }
    }
    ui->spinMaxPrice->setMaximum(maxRentPrice);
    ui->spinMaxPrice->setValue(maxRentPrice);
}

UserWindow::~UserWindow()
{
    delete ui;
}

QWidget* UserWindow::createCarCardWidget(const Car& car) const {

    QWidget* card = new QWidget();
    card->setObjectName("carCard");
    card->setFixedSize(200, 160);

    card->setStyleSheet(
        "#carCard {"
        "   background-color: #383838;"
        "   border: 1px solid #555555;"
        "   border-radius: 8px;"
        "}"
        "#carCard:hover {"
        "   background-color: #424242;"
        "   border: 1px solid #4CAF50;"
        "}"
        );

    QVBoxLayout* layout = new QVBoxLayout(card);
    layout->setContentsMargins(12, 10, 12, 10);
    layout->setSpacing(4);


    QLabel* title = new QLabel(QString("%1 %2 <font color='#8a8a8a'>(%3)</font>")
                                   .arg(car.getBrand(), car.getModel()).arg(car.getManufacturerYear()));
    title->setStyleSheet("font-size: 13px; font-weight: bold; color: #ffffff; border: none; background: transparent;");


    QFrame* line1 = new QFrame();
    line1->setFrameShape(QFrame::HLine);
    line1->setStyleSheet("color: #4a4a4a; background-color: #4a4a4a; max-height: 1px; border: none;");


    QLabel* info = new QLabel(QString(
                                  "Клас: <b>%1</b><br>"
                                  "Місць: <b>%2</b><br>"
                                  "Пробіг: <b>%3 км</b>")
                                  .arg(car.getCategoryString())
                                  .arg(car.getSeats())
                                  .arg(car.getMileage()));
    info->setStyleSheet("font-size: 11px; color: #cccccc; border: none; background: transparent; line-height: 130%;");


    QFrame* line2 = new QFrame();
    line2->setFrameShape(QFrame::HLine);
    line2->setStyleSheet("color: #4a4a4a; background-color: #4a4a4a; max-height: 1px; border: none;");

    QLabel* price = new QLabel(QString("<font color='#4caf50'><b>%1 грн</b></font> <font color='#8a8a8a' size='2'>/ день</font>")
                                   .arg(QString::number(car.getRentPrice(), 'f', 2)));
    price->setStyleSheet("font-size: 13px; border: none; background: transparent;");

    layout->addWidget(title);
    layout->addWidget(line1);
    layout->addWidget(info);
    layout->addWidget(line2);
    layout->addWidget(price);

    return card;
}



void UserWindow::loadCarsList(QListWidget* ui, const QList<Car>& result, bool isAvailable) const {
    ui->clear();
    ui->setViewMode(QListView::IconMode);
    ui->setResizeMode(QListView::Adjust);
    ui->setMovement(QListView::Static);

    ui->setSpacing(6);


    const QSize cardSize(200, 160);

    const QSize itemSize(216, 176);

    for (const Car& car : result) {
        if (isAvailable) {
            if (car.getStatus() == Status::NotAvailable || car.getStatus() == Status::Taken) {
                continue;
            }
        }

        QListWidgetItem* item = new QListWidgetItem(ui);
        item->setSizeHint(itemSize);
        item->setData(Qt::UserRole, car.getId());


        QWidget* container = new QWidget();


        QVBoxLayout* containerLayout = new QVBoxLayout(container);


        containerLayout->setContentsMargins(8, 8, 8, 8);
        containerLayout->setAlignment(Qt::AlignCenter);

        QWidget* cardWidget = createCarCardWidget(car);
        cardWidget->setFixedSize(cardSize);

        containerLayout->addWidget(cardWidget);

        ui->addItem(item);
        ui->setItemWidget(item, container);
    }
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
        bool success = true;

        if (success) {
            QMessageBox::information(this, "Успіх",
                                     QString("Автомобіль успішно заброньовано на %1 днів!\nСума: %2 грн")
                                         .arg(startDate.daysTo(endDate))
                                         .arg(totalPrice, 0, 'f', 2));

            loadCarsList(ui->carsListWidget, carmanager->getAllCars(), true);
            rentalmanager->loadRentalsTable(ui->myRentalsTable, user->getId());
        } else {
            QMessageBox::warning(this, "Помилка", "Не вдалося оформити бронювання.");
        }
    }
}

void UserWindow::applyFilters()
{
    QList<Car> cars = carmanager->getAllCars();
    int categoryIndex = ui->comboCategory->currentIndex();
    int seats = ui->spinMinSeats->value();
    double maxPrice = ui->spinMaxPrice->value();

    QList<Car> result;
    for (const Car& car : cars) {
        if (categoryIndex != 0) {
            if (car.getCategoryString() != ui->comboCategory->currentText()) {
                continue;
            }
        }
        if (car.getSeats() >= seats && car.getRentPrice() <= maxPrice) {
            result.append(car);
        }
    }
    loadCarsList(ui->carsListWidget, result, true);
}


void UserWindow::on_comboCategory_currentIndexChanged(int index)
{
    applyFilters();

}


void UserWindow::on_spinMinSeats_valueChanged(int arg1)
{
    applyFilters();
}


void UserWindow::on_spinMaxPrice_valueChanged(double arg1)
{
    applyFilters();
}


void UserWindow::on_btnSortMerge_clicked()
{
    QList<Car> cars = carmanager->getAllCars();
    QList<Car> result;

    for(const Car& car : cars) {
        if (car.getStatus() != Status::Available) {
            continue;
        }
        result.append(car);
    }

    result = carmanager->mergeSort(result, this->isAscending);
    loadCarsList(ui->carsListWidget, result);
    if (this->isAscending == true) {
        ui->btnSortMerge->setText("Сортувати ціною (за спаданням)");
    }
    else {
        ui->btnSortMerge->setText("Сортувати ціною (за зростанням)");
    }
    this->isAscending = !(this->isAscending);
}



void UserWindow::on_btnMinMaxMileage_clicked()
{
    QList<Car> cars = carmanager->getAllCars();
    if (cars.isEmpty()) {
        ui->carsListWidget->clear();
        return;
    }
    QList<Car> result;


    double minMileage = cars[0].getMileage();
    double maxMileage = cars[0].getMileage();

    for(const Car& car : cars) {
        if (car.getStatus() != Status::Available) {
            continue;
        }

        if (car.getMileage() < minMileage) {
            minMileage = car.getMileage();
        }
        if (car.getMileage() > maxMileage) {
            maxMileage = car.getMileage();
        }
    }

    for(const Car& car : cars) {
        if (car.getStatus() != Status::Available) {
            continue;
        }
        if (car.getMileage() == minMileage || car.getMileage() == maxMileage) {
            result.append(car);
        }
    }
    loadCarsList(ui->carsListWidget, result);


}

void UserWindow::on_btnGroupAndSort_clicked() {
    QList<Car> cars = carmanager->getAllCars();

    std::sort(cars.begin(), cars.end(), [](const Car& a, const Car& b) {
        if (a.getCategory() != b.getCategory()) {
            return static_cast<int>(a.getCategory()) < static_cast<int>(b.getCategory());
        }
        return a.getManufacturerYear() > b.getManufacturerYear();
    });

    loadCarsList(ui->carsListWidget, cars);
}