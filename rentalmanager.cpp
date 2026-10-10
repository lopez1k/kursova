#include "rentalmanager.h"

RentalManager::RentalManager(UserManager& _usermanager, CarManager& _carmanager, QString _filename): usermanager(_usermanager), carmanager(_carmanager), filename(_filename) {
    loadFromJson();
}

QString RentalManager::generateId() {
    return QString("R-%1").arg(nextId++);
}



RentalManager::~RentalManager() {
    saveToJson();
}

void RentalManager::saveToJson() const {
    QJsonArray jsonArray;

    for(const Rental& rental: this->rentals) {
        QJsonObject rentalObject;
        rentalObject << rental;
        jsonArray.append(rentalObject);
    }

    QJsonDocument jsonDocument(jsonArray);

    QFile file(this->filename);
    if (!file.open(QIODevice::WriteOnly)) {
        return;
    }

    file.write(jsonDocument.toJson(QJsonDocument::Indented));
    file.close();
}

void RentalManager::loadFromJson() {
    QFile file(this->filename);
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument jsonDocument = QJsonDocument::fromJson(data);
    if (!jsonDocument.isArray()) {
        return;
    }

    this->rentals.clear();

    QJsonArray jsonArray = jsonDocument.array();

    for(const QJsonValue& value: jsonArray) {
        if (value.isObject()) {
            Rental rental;
            value.toObject() >> rental;
            rentals.append(rental);
        }
    }
}


void RentalManager::rentCar(const QString& _userId, const QString& _carId, const QDateTime& _endDate) {
    // ASSUMPTION: findById повертає nullptr, якщо не знайдено
    if (usermanager.findUserById(_userId) == nullptr)
        return ;

    Car* car = carmanager.getCarById(_carId);
    if (car == nullptr)
        return;

    if (car->getStatus() == Status::Archived)
        return;

    if (isCarRented(_carId) || car->getStatus() != Status::Available)
        return;

    QDateTime start = QDateTime::currentDateTime();
    if (_endDate <= start)
        return;

    rentals.append(Rental(generateId(), _userId, _carId, start, _endDate, car->getRentPrice()));
    car->setStatus(Status::Taken);

    return;
}

bool RentalManager::completeRental(const QString& rentalId) {
    Rental* r = findById(rentalId);
    if (r == nullptr || r->getStatus() != RentalStatus::Active)
        return false;

    r->complete();

    Car* car = carmanager.getCarById(r->getCarId());
    if (car != nullptr && car->getStatus() == Status::Taken)
        car->setStatus(Status::Available);

    return true;
}

bool RentalManager::cancelRental(const QString& rentalId) {
    Rental* r = findById(rentalId);
    if (r == nullptr || r->getStatus() != RentalStatus::Active)
        return false;

    r->cancel();

    Car* car = carmanager.getCarById(r->getCarId());
    if (car != nullptr && car->getStatus() == Status::Taken)
        car->setStatus(Status::Available);

    return true;
}

Rental* RentalManager::findById(const QString& rentalId) {
    for (Rental& r : rentals) {
        if (r.getId() == rentalId)
            return &r;
    }
    return nullptr;
}

const QVector<Rental>& RentalManager::getAll() const {
    return rentals;
}

QVector<Rental> RentalManager::getByUser(const QString& userId) const {
    QVector<Rental> result;
    for (const Rental& r : rentals) {
        if (r.getUserId() == userId)
            result.append(r);
    }
    return result;
}

QVector<Rental> RentalManager::getByCar(const QString& carId) const {
    QVector<Rental> result;
    for (const Rental& r : rentals) {
        if (r.getCarId() == carId)
            result.append(r);
    }
    return result;
}

QVector<Rental> RentalManager::getActive() const {
    QVector<Rental> result;
    for (const Rental& r : rentals) {
        if (r.getStatus() == RentalStatus::Active)
            result.append(r);
    }
    return result;
}

bool RentalManager::isCarRented(const QString& carId) const {
    for (const Rental& r : rentals) {
        if (r.getCarId() == carId && r.getStatus() == RentalStatus::Active)
            return true;
    }
    return false;
}

void RentalManager::loadRentalsTable(QTableWidget* table, const QString& _userId) const {
    if (!table) return;

    const bool onlyUser = !_userId.isEmpty();

    table->setRowCount(0);
    table->setColumnCount(6);
    table->setHorizontalHeaderLabels({"ID", "Авто", "Початок", "Кінець (план)", "Сума", "Статус"});

    for (const Rental& r : rentals) {
        if (onlyUser && r.getUserId() != _userId)
            continue;

        Car* car = carmanager.getCarById(r.getCarId());
        QString carName = car ? car->getBrand() + " " + car->getModel() : "—";

        int row = table->rowCount();
        table->insertRow(row);
        table->setItem(row, 0, new QTableWidgetItem(r.getId()));
        table->setItem(row, 1, new QTableWidgetItem(carName));
        table->setItem(row, 2, new QTableWidgetItem(r.getStartDate().toString("dd.MM.yyyy HH:mm")));
        table->setItem(row, 3, new QTableWidgetItem(r.getEndDate().toString("dd.MM.yyyy HH:mm")));
        table->setItem(row, 4, new QTableWidgetItem(QString::number(r.getTotalPrice(), 'f', 2)));
        table->setItem(row, 5, new QTableWidgetItem(r.getStatusString()));
    }
}