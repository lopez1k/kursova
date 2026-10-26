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
    QLocale ua(QLocale::Ukrainian);

    QFrame* card = new QFrame();
    card->setObjectName("carCard");
    card->setAttribute(Qt::WA_Hover);

    QVBoxLayout* layout = new QVBoxLayout(card);
    layout->setContentsMargins(14, 12, 14, 12);
    layout->setSpacing(4);

    // Назва
    QLabel* title = new QLabel(QString("%1 %2").arg(car.getBrand(), car.getModel()));
    title->setObjectName("carTitle");

    // Клас · рік · місця
    QLabel* meta = new QLabel(QString("%1 · %2 · %3 місць")
                                  .arg(car.getCategoryString())
                                  .arg(car.getManufacturerYear())
                                  .arg(car.getSeats()));
    meta->setObjectName("carMeta");

    // Пробіг
    QLabel* mileage = new QLabel(QString("Пробіг: %1 км")
                                     .arg(ua.toString(qlonglong(car.getMileage()))));
    mileage->setObjectName("carMeta");

    // Тонка лінія-роздільник
    QFrame* line = new QFrame();
    line->setObjectName("carLine");
    line->setFixedHeight(1);

    // Ціна (без копійок, якщо їх немає)
    const double p = car.getRentPrice();
    const int decimals = (std::floor(p) == p) ? 0 : 2;
    QLabel* price = new QLabel(QString("%1 ₴ / доба").arg(ua.toString(p, 'f', decimals)));
    price->setObjectName("carPrice");

    // Статус
    QLabel* status = new QLabel();
    if (car.getStatus() == Status::Taken) {
        status->setText("Орендовано");
        status->setObjectName("carStatusBusy");
    } else if (car.getStatus() == Status::NotAvailable) {
        status->setText("Недоступний");
        status->setObjectName("carStatusBusy");
    } else {
        status->setText("Доступний");
        status->setObjectName("carStatusFree");
    }

    QHBoxLayout* bottom = new QHBoxLayout();
    bottom->setContentsMargins(0, 4, 0, 0);
    bottom->addWidget(price);
    bottom->addStretch();
    bottom->addWidget(status);

    layout->addWidget(title);
    layout->addWidget(meta);
    layout->addWidget(mileage);
    layout->addStretch();
    layout->addWidget(line);
    layout->addLayout(bottom);

    return card;
}


void UserWindow::loadCarsList(QListWidget* list, const QList<Car>& result, bool isAvailable) const {
    list->clear();
    list->setViewMode(QListView::IconMode);
    list->setResizeMode(QListView::Adjust);
    list->setMovement(QListView::Static);
    list->setSpacing(4);

    const QSize cardSize(220, 132);
    const QSize itemSize(cardSize.width() + 12, cardSize.height() + 12);   // + відступи контейнера

    for (const Car& car : result) {
        if (isAvailable &&
            (car.getStatus() == Status::NotAvailable || car.getStatus() == Status::Taken)) {
            continue;
        }

        QListWidgetItem* item = new QListWidgetItem(list);
        item->setSizeHint(itemSize);
        item->setData(Qt::UserRole, car.getId());

        // Прозорий контейнер навколо картки
        QWidget* container = new QWidget();
        QVBoxLayout* containerLayout = new QVBoxLayout(container);
        containerLayout->setContentsMargins(6, 6, 6, 6);
        containerLayout->setAlignment(Qt::AlignCenter);

        QWidget* cardWidget = createCarCardWidget(car);
        cardWidget->setFixedSize(cardSize);
        containerLayout->addWidget(cardWidget);

        list->setItemWidget(item, container);
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

QList<Car> UserWindow::applyFilters()
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
    return result;
}

void UserWindow::refreshCatalog()
{
    ui->btnSortMerge->setText("Сортувати ціною (за зростанням)");
    loadCarsList(ui->carsListWidget, carmanager->mergeSort(applyFilters(), this->currentAscending), true);
}



void UserWindow::on_comboCategory_currentIndexChanged(int index)
{
    refreshCatalog();
}


void UserWindow::on_spinMinSeats_valueChanged(int arg1)
{
    refreshCatalog();

}


void UserWindow::on_spinMaxPrice_valueChanged(double arg1)
{
    refreshCatalog();
}


void UserWindow::on_btnSortMerge_clicked()
{
    QList<Car> cars = applyFilters();
    QList<Car> result;

    for(const Car& car : cars) {
        if (car.getStatus() != Status::Available) {
            continue;
        }
        result.append(car);
    }
    this->currentAscending = this->isAscending;
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
    QList<Car> cars = applyFilters();
    if (cars.isEmpty()) {
        ui->carsListWidget->clear();
        return;
    }

    QList<Car> available;
    QList<Car> result;

    for(const Car& car : cars) {
        if (car.getStatus() != Status::Available) {
            continue;
        }
        available.append(car);
    }

    double minMileage = available[0].getMileage();
    double maxMileage = available[0].getMileage();

    for(const Car& car : available) {
        if (car.getMileage() < minMileage) {
            minMileage = car.getMileage();
        }
        if (car.getMileage() > maxMileage) {
            maxMileage = car.getMileage();
        }
    }

    for(const Car& car : available) {
        if (car.getMileage() == minMileage || car.getMileage() == maxMileage) {
            result.append(car);
        }
    }
    loadCarsList(ui->carsListWidget, result);
}

void UserWindow::on_btnGroupAndSort_clicked() {
    QList<Car> cars = applyFilters();

    std::sort(cars.begin(), cars.end(), [](const Car& a, const Car& b) {
        if (a.getCategory() != b.getCategory()) {
            return static_cast<int>(a.getCategory()) < static_cast<int>(b.getCategory());
        }
        return a.getManufacturerYear() > b.getManufacturerYear();
    });

    loadCarsList(ui->carsListWidget, cars);
}