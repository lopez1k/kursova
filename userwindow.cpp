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

    // Вартість за задану кількість днів
    const int days = ui->spinDays->value();
    const double total = days * p;
    const int totalDecimals = (std::floor(total) == total) ? 0 : 2;
    QLabel* costN = new QLabel(QString("Вартість за %1 дн.: %2 ₴")
                                   .arg(days)
                                   .arg(ua.toString(total, 'f', totalDecimals)));
    costN->setObjectName("carCostN");

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
    layout->addWidget(costN);
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

    const QSize cardSize(220, 156);
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
    QString userId = user->getId();
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

        QDateTime start = (startDate == QDate::currentDate())
                              ? QDateTime::currentDateTime()
                              : QDateTime(startDate, QTime(0, 0));
        QDateTime end(endDate, QTime(23, 59));
        try {
            rentalmanager->rentCar(userId, carId, start, end);

            QMessageBox::information(this, "Успіх",
                                     QString("Автомобіль успішно заброньовано на %1 днів!\nСума: %2 грн")
                                         .arg(startDate.daysTo(endDate))
                                         .arg(totalPrice, 0, 'f', 2));

            loadCarsList(ui->carsListWidget, carmanager->getAllCars(), true);
            rentalmanager->loadRentalsTable(ui->myRentalsTable, userId);
        }
        catch (const AppException& ex) {
            QMessageBox::warning(this, "Не вдалося оформити бронювання", ex.what());
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

QList<Car> UserWindow::selectMinMaxMileage(const QList<Car>& cars) const
{
    if (cars.isEmpty()) {
        return {};
    }

    double minMileage = cars[0].getMileage();
    double maxMileage = cars[0].getMileage();

    for (const Car& car : cars) {
        if (car.getMileage() < minMileage) {
            minMileage = car.getMileage();
        }
        if (car.getMileage() > maxMileage) {
            maxMileage = car.getMileage();
        }
    }

    QList<Car> result;
    for (const Car& car : cars) {
        if (car.getMileage() == minMileage || car.getMileage() == maxMileage) {
            result.append(car);
        }
    }
    return result;
}

QList<Car> UserWindow::groupByClassYear(QList<Car> cars) const
{
    std::sort(cars.begin(), cars.end(), [](const Car& a, const Car& b) {
        if (a.getCategory() != b.getCategory()) {
            return static_cast<int>(a.getCategory()) < static_cast<int>(b.getCategory());
        }
        return a.getManufacturerYear() > b.getManufacturerYear();
    });
    return cars;
}

void UserWindow::renderCatalog()
{
    // щоразу беремо авто з applyFilters і лишаємо лише доступні
    QList<Car> available;
    for (const Car& car : applyFilters()) {
        if (car.getStatus() == Status::Available) {
            available.append(car);
        }
    }

    // далі — залежно від обраного режиму
    QList<Car> result;
    switch (mode) {
    case CatalogMode::MinMaxMileage:
        result = selectMinMaxMileage(available);
        break;
    case CatalogMode::GroupClassYear:
        result = groupByClassYear(available);
        break;
    case CatalogMode::SortPrice:
    default:
        result = carmanager->mergeSort(available, currentAscending);
        break;
    }

    loadCarsList(ui->carsListWidget, result, false);
}


void UserWindow::on_comboCategory_currentIndexChanged(int index)
{
    renderCatalog();
}


void UserWindow::on_spinMinSeats_valueChanged(int arg1)
{
    renderCatalog();
}


void UserWindow::on_spinMaxPrice_valueChanged(double arg1)
{
    renderCatalog();
}


void UserWindow::on_spinDays_valueChanged(int arg1)
{
    renderCatalog();
}


void UserWindow::on_btnSortMerge_clicked()
{
    mode = CatalogMode::SortPrice;
    currentAscending = isAscending;
    if (isAscending) {
        ui->btnSortMerge->setText("Сортувати ціною (за спаданням)");
    }
    else {
        ui->btnSortMerge->setText("Сортувати ціною (за зростанням)");
    }
    isAscending = !isAscending;
    renderCatalog();
}


void UserWindow::on_btnMinMaxMileage_clicked()
{
    mode = CatalogMode::MinMaxMileage;
    renderCatalog();
}


void UserWindow::on_btnGroupAndSort_clicked()
{
    mode = CatalogMode::GroupClassYear;
    renderCatalog();
}
void UserWindow::on_myRentalsTable_itemSelectionChanged()
{
    ui->btnReturnCar->setEnabled(false);
    ui->btnCancelRental->setEnabled(false);

    int row = ui->myRentalsTable->currentRow();
    if (row < 0) {
        return;
    }

    QTableWidgetItem* idItem = ui->myRentalsTable->item(row, 0);
    if (!idItem) {
        return;
    }
    QString rentId = idItem->text();
    Rental* rent = rentalmanager->findById(rentId);
    if (!rent || rent->getStatus() != RentalStatus::Active) {
        return;
    }

    const bool started = rent->getStartDate() <= QDateTime::currentDateTime();

    // оренда вже почалась → її можна повернути; ще не почалась → можна скасувати
    ui->btnReturnCar->setEnabled(started);
    ui->btnCancelRental->setEnabled(!started);
}


void UserWindow::on_btnReturnCar_clicked()
{
    int row = ui->myRentalsTable->currentRow();
    if (row < 0) {
        return;
    }
    User* user = usermanager->getCurrentUser();
    if (!user) {
        return;
    }

    QTableWidgetItem* idItem = ui->myRentalsTable->item(row, 0);
    if (!idItem) {
        return;
    }
    QString rentId = idItem->text();
    try {
        rentalmanager->completeRental(rentId);

        rentalmanager->loadRentalsTable(ui->myRentalsTable, user->getId());
        loadCarsList(ui->carsListWidget, carmanager->getAllCars(), true);
        ui->btnReturnCar->setEnabled(false);
        ui->btnCancelRental->setEnabled(false);

        QMessageBox::information(this, "Готово", "Автомобіль повернено.");
    }
    catch (const AppException& ex) {
        QMessageBox::warning(this, "Не вдалося повернути автомобіль", ex.what());
    }
}


void UserWindow::on_btnCancelRental_clicked()
{
    int row = ui->myRentalsTable->currentRow();
    if (row < 0) {
        return;
    }
    User* user = usermanager->getCurrentUser();
    if (!user) {
        return;
    }

    QTableWidgetItem* idItem = ui->myRentalsTable->item(row, 0);
    if (!idItem) {
        return;
    }
    QString rentId = idItem->text();
    try {
        rentalmanager->cancelRental(rentId);

        rentalmanager->loadRentalsTable(ui->myRentalsTable, user->getId());
        loadCarsList(ui->carsListWidget, carmanager->getAllCars(), true);
        ui->btnReturnCar->setEnabled(false);
        ui->btnCancelRental->setEnabled(false);

        QMessageBox::information(this, "Готово", "Бронювання скасовано.");
    }
    catch (const AppException& ex) {
        QMessageBox::warning(this, "Не вдалося скасувати бронювання", ex.what());
    }
}

