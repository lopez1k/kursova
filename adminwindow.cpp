#include "adminwindow.h"
#include "QDebug"
#include "ui_adminwindow.h"
#include "addcarwindow.h"

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
    rentalmanager->loadRentalsTable(ui->rentalsTable);
    loadCarsList(ui->carsListWidget, carmanager->getAllCars(), false);
}

AdminWindow::~AdminWindow()
{
    delete ui;
}
QWidget* AdminWindow::createCarCardWidget(const Car& car) const {
    QLocale ua(QLocale::Ukrainian);

    QFrame* card = new QFrame();
    card->setObjectName("carCard");
    card->setAttribute(Qt::WA_Hover);
    card->setProperty("selected", false);

    QVBoxLayout* layout = new QVBoxLayout(card);
    layout->setContentsMargins(14, 12, 14, 12);
    layout->setSpacing(4);

    QLabel* title = new QLabel(QString("%1 %2").arg(car.getBrand(), car.getModel()));
    title->setObjectName("carTitle");

    QLabel* meta = new QLabel(QString("%1 · %2 · %3 місць")
                                  .arg(car.getCategoryString())
                                  .arg(car.getManufacturerYear())
                                  .arg(car.getSeats()));
    meta->setObjectName("carMeta");

    QLabel* mileage = new QLabel(QString("Пробіг: %1 км")
                                     .arg(ua.toString(qlonglong(car.getMileage()))));
    mileage->setObjectName("carMeta");

    QFrame* line = new QFrame();
    line->setObjectName("carLine");
    line->setFixedHeight(1);

    const double p = car.getRentPrice();
    const int decimals = (std::floor(p) == p) ? 0 : 2;
    QLabel* price = new QLabel(QString("%1 ₴ / доба").arg(ua.toString(p, 'f', decimals)));
    price->setObjectName("carPrice");

    QLabel* status = new QLabel(car.getStatusString());
    switch (car.getStatus()) {
    case Status::Available: status->setObjectName("carStatusFree"); break;  // зелений
    case Status::Taken:     status->setObjectName("carStatusBusy"); break;  // помаранчевий
    default:                status->setObjectName("carStatusOff");  break;  // червоний
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


void AdminWindow::loadCarsList(QListWidget* list, const QList<Car>& result, bool isAvailable) const {
    list->clear();
    list->setViewMode(QListView::IconMode);
    list->setResizeMode(QListView::Adjust);
    list->setMovement(QListView::Static);
    list->setSpacing(4);

    const QSize cardSize(220, 132);
    const QSize itemSize(cardSize.width() + 12, cardSize.height() + 12);

    for (const Car& car : result) {
        if (car.getStatus() == Status::Archived)
            continue;
        if (isAvailable &&
            (car.getStatus() == Status::NotAvailable || car.getStatus() == Status::Taken))
            continue;

        QListWidgetItem* item = new QListWidgetItem(list);
        item->setSizeHint(itemSize);
        item->setData(Qt::UserRole, car.getId());

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


void AdminWindow::on_btnCars_clicked()
{
    loadCarsList(ui->carsListWidget, carmanager->getAllCars(), false);
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

void AdminWindow::applyFilters()
{
    QList<Car> cars = carmanager->getAllCars();
    int categoryIndex = ui->categoryFilter->currentIndex();
    int statusIndex = ui->statusFilter->currentIndex();

    QList<Car> result;
    for (const Car& car : cars) {
        if (categoryIndex != 0) {
            if (car.getCategoryString() != ui->categoryFilter->currentText()) {
                continue;
            }
        }
        if (statusIndex != 0) {
            if (car.getStatusString() != ui->statusFilter->currentText()) {
                continue;
            }
        }
        result.append(car);
    }
    loadCarsList(ui->carsListWidget, result, false);
}



void AdminWindow::on_categoryFilter_currentIndexChanged(int index)
{
    applyFilters();
}


void AdminWindow::on_statusFilter_currentIndexChanged(int index)
{
    applyFilters();
}


void AdminWindow::on_carsListWidget_itemDoubleClicked(QListWidgetItem *item)
{
    return;
}


void AdminWindow::on_btnAddCar_clicked()
{
    AddCarWindow* addcarui = new AddCarWindow(this->usermanager, this->carmanager, this->rentalmanager, this);
    addcarui->setAttribute(Qt::WA_DeleteOnClose);
    addcarui->show();
    connect(addcarui, &QObject::destroyed, this, &AdminWindow::applyFilters);
    return;
}


void AdminWindow::on_carsListWidget_itemClicked(QListWidgetItem *item)
{
    ui->btnEditCar->setEnabled(true);
    ui->btnDeleteCar->setEnabled(true);

}


void AdminWindow::on_btnEditCar_clicked()
{
    QString carId = ui->carsListWidget->currentItem()->data(Qt::UserRole).toString();
    Car* car = carmanager->getCarById(carId);
    AddCarWindow* addcarui = new AddCarWindow(this->usermanager, this->carmanager, this->rentalmanager, this, car);
    addcarui->setAttribute(Qt::WA_DeleteOnClose);
    addcarui->show();
    ui->btnEditCar->setEnabled(false);
    ui->btnDeleteCar->setEnabled(false);
    connect(addcarui, &QObject::destroyed, this, &AdminWindow::applyFilters);

}


void AdminWindow::on_btnDeleteCar_clicked()
{
    QString carId = ui->carsListWidget->currentItem()->data(Qt::UserRole).toString();
    Car* car = carmanager->getCarById(carId);
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Видалення " + car->getBrand() + " " + car->getModel(), "Ви справді хочете видалити це авто?", QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        car->setStatus(Status::Archived);
        loadCarsList(ui->carsListWidget, carmanager->getAllCars());
    }
    ui->btnEditCar->setEnabled(false);
    ui->btnDeleteCar->setEnabled(false);
}

