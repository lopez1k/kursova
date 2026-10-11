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
    QWidget* card = new QWidget();
    card->setObjectName("carCard");
    card->setFixedSize(200, 180);

    card->setStyleSheet(
        "#carCard {"
        "    background-color: #383838;"
        "    border: 1px solid #555555;"
        "    border-radius: 8px;"
        "}"
        "#carCard:hover {"
        "    background-color: #424242;"
        "    border: 1px solid #4CAF50;"
        "}"
        );

    QVBoxLayout* layout = new QVBoxLayout(card);
    layout->setContentsMargins(12, 10, 12, 10);
    layout->setSpacing(5);


    QLabel* title = new QLabel(QString("%1 %2 <font color='#8a8a8a'>(%3)</font>")
                                   .arg(car.getBrand(), car.getModel()).arg(car.getManufacturerYear()));
    title->setStyleSheet("font-size: 13px; font-weight: bold; color: #ffffff; border: none; background: transparent;");


    QFrame* line1 = new QFrame();
    line1->setFrameShape(QFrame::HLine);
    line1->setStyleSheet("color: #4a4a4a; background-color: #4a4a4a; max-height: 1px; border: none;");

    QString statusColor = "#cccccc";
    if (car.getStatusString() == "Доступно") statusColor = "#4CAF50";
    else if (car.getStatusString() == "Зайнято") statusColor = "#FF9800";
    else statusColor = "#E57373";

    QLabel* info = new QLabel(QString(
                                  "Клас: <b>%1</b><br>"
                                  "Місць: <b>%2</b><br>"
                                  "Пробіг: <b>%3 км</b><br>"
                                  "Статус: <font color='%4'><b>%5</b></font>")
                                  .arg(car.getCategoryString())
                                  .arg(car.getSeats())
                                  .arg(car.getMileage())
                                  .arg(statusColor)
                                  .arg(car.getStatusString()));
    info->setStyleSheet("font-size: 11px; color: #cccccc; border: none; background: transparent; line-height: 125%;");


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



void AdminWindow::loadCarsList(QListWidget* ui, const QList<Car>& result, bool isAvailable) const {
    ui->clear();
    ui->setViewMode(QListView::IconMode);
    ui->setResizeMode(QListView::Adjust);
    ui->setMovement(QListView::Static);


    ui->setSpacing(6);


    const QSize cardSize(200, 160);

    const QSize itemSize(216, 176);

    for (const Car& car : result) {
        if (car.getStatus() == Status::Archived) {
            continue;
        }
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

