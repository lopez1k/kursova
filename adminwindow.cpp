#include "adminwindow.h"
#include "ui_adminwindow.h"
#include "addcarwindow.h"
#include "adduserwindow.h"
#include <QFileDialog>
#include <QStandardPaths>
#include <QDir>
#include <QMessageBox>
#include <QLabel>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLocale>
#include <cmath>

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
        ui->lblProfileName->setText("Ім'я: <b>" + currentUser->getName() + "</b>");
        ui->lblProfileEmail->setText("Email: <b>" + currentUser->getEmail() + "</b>");
        ui->lblProfileRole->setText("Роль: <b>" + currentUser->getRoleString() + "</b>");
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
    ui->btnEditCar->setEnabled(false);
    ui->btnDeleteCar->setEnabled(false);

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
        this->loginWindow->setWindowState(this->windowState());
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


void AdminWindow::on_btnAddCar_clicked()
{
    AddCarWindow* addcarui = new AddCarWindow(this->usermanager, this->carmanager, this->rentalmanager, this);
    addcarui->setAttribute(Qt::WA_DeleteOnClose);
    addcarui->show();
    connect(addcarui, &QObject::destroyed, this, &AdminWindow::applyFilters);
    return;
}


void AdminWindow::on_btnAddUser_clicked()
{
    AddUserWindow* adduserui = new AddUserWindow(this->usermanager, this);
    adduserui->setAttribute(Qt::WA_DeleteOnClose);
    adduserui->show();
    connect(adduserui, &QObject::destroyed, this, &AdminWindow::loadUsersTable);
}


void AdminWindow::on_carsListWidget_itemClicked(QListWidgetItem *item)
{
    if (!item) {
        ui->btnEditCar->setEnabled(false);
        ui->btnDeleteCar->setEnabled(false);
        return;
    }

    QString carId = item->data(Qt::UserRole).toString();
    Car* car = carmanager->getCarById(carId);

    
    const bool editable = car
                          && car->getStatus() != Status::Taken
                          && !rentalmanager->isCarRented(carId);

    ui->btnEditCar->setEnabled(editable);
    ui->btnDeleteCar->setEnabled(editable);
}


void AdminWindow::on_btnEditCar_clicked()
{
    QListWidgetItem* item = ui->carsListWidget->currentItem();
    if (!item) {
        return;
    }
    QString carId = item->data(Qt::UserRole).toString();
    Car* car = carmanager->getCarById(carId);
    if (!car || car->getStatus() == Status::Taken || rentalmanager->isCarRented(carId)) {
        QMessageBox::information(this, "Недоступно", "Орендоване авто не можна редагувати.");
        return;
    }
    AddCarWindow* addcarui = new AddCarWindow(this->usermanager, this->carmanager, this->rentalmanager, this, carId);
    addcarui->setAttribute(Qt::WA_DeleteOnClose);
    addcarui->show();
    ui->btnEditCar->setEnabled(false);
    ui->btnDeleteCar->setEnabled(false);
    connect(addcarui, &QObject::destroyed, this, &AdminWindow::applyFilters);

}


void AdminWindow::on_btnDeleteCar_clicked()
{
    QListWidgetItem* item = ui->carsListWidget->currentItem();
    if (!item) {
        return;
    }
    QString carId = item->data(Qt::UserRole).toString();
    Car* car = carmanager->getCarById(carId);
    if (!car) {
        return;
    }
    if (car->getStatus() == Status::Taken || rentalmanager->isCarRented(carId)) {
        QMessageBox::information(this, "Недоступно", "Орендоване авто не можна видалити.");
        return;
    }
    QString carName = car->getBrand() + " " + car->getModel();

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Видалення " + carName, "Ви справді хочете видалити це авто?", QMessageBox::Yes | QMessageBox::No);
    car = carmanager->getCarById(carId);
    if (reply == QMessageBox::Yes && car) {
        car->setStatus(Status::Archived);
        carmanager->saveToJson();
        loadCarsList(ui->carsListWidget, carmanager->getAllCars(), false);
    }
    ui->btnEditCar->setEnabled(false);
    ui->btnDeleteCar->setEnabled(false);
}



void AdminWindow::on_actionExportUsers_triggered() {
    QString docs = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString path = QFileDialog::getSaveFileName(this, "Експорт користувачів", QDir(docs).filePath("users.csv"), "CSV (*.csv)");
    if (path.isEmpty()) {
        return;
    }
    try {
        usermanager->saveToFile(path);
        QMessageBox::information(this, "Експорт користувачів", "Користувачів успішно експортовано!");
    } catch (const AppException& ex) {
        QMessageBox::warning(this, "Помилка експорту", ex.what());
    }
}

void AdminWindow::on_actionExportCars_triggered() {
    QString docs = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString path = QFileDialog::getSaveFileName(this, "Експорт автомобілів", QDir(docs).filePath("cars.csv"), "CSV (*.csv)");
    if (path.isEmpty()) {
        return;
    }
    try{ 
        carmanager->saveToFile(path);
        QMessageBox::information(this, "Експорт автомобілів", "Автомобілі успішно експортовано!");
    } catch (const AppException& ex) {
        QMessageBox::warning(this, "Помилка експорту", ex.what());   
    }
}

void AdminWindow::on_actionExportRentals_triggered() {
    QString docs = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString path = QFileDialog::getSaveFileName(this, "Експорт історії оренди", QDir(docs).filePath("history.csv"), "CSV (*.csv)");
    if (path.isEmpty()) {
        return;
    }
    try {
        rentalmanager->saveToFile(path);
        QMessageBox::information(this, "Експорт історії оренди", "Історію оренди успішно експортовано!");
    } catch (const AppException& ex) {
        QMessageBox::warning(this, "Помилка експорту", ex.what());   
    }
}

void AdminWindow::on_btnProfile_clicked()
{
    updateProfileStats();
    ui->stackedWidget->setCurrentIndex(3);
}

void AdminWindow::on_btnChangePassword_clicked() {
    const QString oldPassword = ui->editOldPassword->text();
    const QString newPassword = ui->editNewPassword->text();
    const QString confirmPassword = ui->editConfirmPassword->text();

    if (oldPassword.isEmpty() || newPassword.isEmpty() || confirmPassword.isEmpty()) {
        QMessageBox::warning(this, "Зміна пароля", "Заповніть усі поля.");
        return;
    }
    if (newPassword != confirmPassword) {
        QMessageBox::warning(this, "Зміна пароля", "Нові паролі не збігаються.");
        return;
    }

    try {
        usermanager->changePassword(oldPassword, newPassword);
    } catch (const AppException& ex) {
        QMessageBox::warning(this, "Зміна пароля", ex.what());
        return;
    }

    ui->editOldPassword->clear();
    ui->editNewPassword->clear();
    ui->editConfirmPassword->clear();
    QMessageBox::information(this, "Зміна пароля", "Пароль успішно змінено.");
}

void AdminWindow::updateProfileStats() {
    int carsInFleet = 0;
    for (const Car& car : carmanager->getAllCars()) {
        if (car.getStatus() != Status::Archived) ++carsInFleet;
    }

    double revenue = 0.0;
    for (const Rental& r : rentalmanager->getAll()) {
        revenue += r.getTotalPrice();   
    }

    QLocale ua(QLocale::Ukrainian);
    ui->statValue1->setText(QString::number(carsInFleet));
    ui->statValue2->setText(QString::number(usermanager->getAllUsers().size()));
    ui->statValue3->setText(QString::number(rentalmanager->getActive().size()));
    ui->statValue4->setText(ua.toString(revenue, 'f', 0));
}

void AdminWindow::on_usersTable_itemSelectionChanged()
{
    const int row = ui->usersTable->currentRow();
    QTableWidgetItem* idItem = row >= 0 ? ui->usersTable->item(row, 0) : nullptr;
    User* current = usermanager->getCurrentUser();

  
    const bool canDelete = idItem
                           && ui->usersTable->selectionModel()->hasSelection()
                           && (!current || idItem->text() != current->getId());
    ui->btnDeleteUser->setEnabled(canDelete);
}

void AdminWindow::on_btnDeleteUser_clicked()
{
    const int row = ui->usersTable->currentRow();
    QTableWidgetItem* idItem = row >= 0 ? ui->usersTable->item(row, 0) : nullptr;
    if (!idItem) {
        return;
    }
    const QString userId = idItem->text();
    const QString userName = ui->usersTable->item(row, 1) ? ui->usersTable->item(row, 1)->text() : userId;

    if (rentalmanager->hasActiveRentals(userId)) {
        QMessageBox::information(this, "Недоступно",
                                 "У користувача є активні оренди. Спочатку завершіть або скасуйте їх.");
        return;
    }

    auto reply = QMessageBox::question(this, "Видалення " + userName,
                                       "Ви справді хочете видалити цього користувача?",
                                       QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes) {
        return;
    }

    try {
        usermanager->removeUser(userId);
    } catch (const AppException& ex) {
        QMessageBox::warning(this, "Помилка видалення", ex.what());
        return;
    }

    ui->usersTable->clearSelection();
    loadUsersTable();
}
