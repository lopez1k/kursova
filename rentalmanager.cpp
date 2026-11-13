#include "rentalmanager.h"
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QRegularExpression>
#include <QTableWidget>

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
    int maxNum = 0;

    for(const QJsonValue& value: jsonArray) {
        if (value.isObject()) {
            Rental rental;
            value.toObject() >> rental;
            rentals.append(rental);
            QString numStr = rental.getId();
            numStr.remove(QRegularExpression("[^0-9]"));

            int currentNum = numStr.toInt();
            if (currentNum > maxNum) {
                maxNum = currentNum;
            }
        }
    }
    this->nextId = maxNum + 1;
}


void RentalManager::rentCar(const QString& _userId, const QString& _carId, const QDateTime& _startDate, const QDateTime& _endDate) {
    usermanager.findUserById(_userId);

    Car* car = carmanager.getCarById(_carId);
    if (car == nullptr)
        throw CarNotFoundException(_carId);

    if (isCarRented(_carId) || car->getStatus() != Status::Available)
        throw CarNotAvailableException(car->getBrand() + " " + car->getModel());

    if (_startDate.date() < QDate::currentDate() || _endDate <= _startDate)
        throw InvalidRentalPeriodException();

    rentals.append(Rental(generateId(), _userId, _carId, _startDate, _endDate, car->getRentPrice()));
    car->setStatus(Status::Taken);

    saveToJson();
    carmanager.saveToJson();
}

void RentalManager::completeRental(const QString& rentalId) {
    Rental* r = findById(rentalId);
    if (r == nullptr)
        throw RentalNotFoundException(rentalId);
    if (r->getStatus() != RentalStatus::Active)
        throw RentalNotActiveException(rentalId);

    r->complete();

    Car* car = carmanager.getCarById(r->getCarId());
    if (car != nullptr && car->getStatus() == Status::Taken)
        car->setStatus(Status::Available);

    saveToJson();
    carmanager.saveToJson();
}

void RentalManager::cancelRental(const QString& rentalId) {
    Rental* r = findById(rentalId);
    if (r == nullptr)
        throw RentalNotFoundException(rentalId);
    if (r->getStatus() != RentalStatus::Active)
        throw RentalNotActiveException(rentalId);

    r->cancel();

    Car* car = carmanager.getCarById(r->getCarId());
    if (car != nullptr && car->getStatus() == Status::Taken)
        car->setStatus(Status::Available);

    saveToJson();
    carmanager.saveToJson();
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
    table->setColumnCount(7);
    table->setHorizontalHeaderLabels({"ID", "Авто", "Початок", "Кінець (план)", "Повернено", "Сума", "Статус"});

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
        const QDateTime returned = r.getReturnedDate();
        table->setItem(row, 4, new QTableWidgetItem(returned.isValid() ? returned.toString("dd.MM.yyyy HH:mm") : "—"));
        table->setItem(row, 5, new QTableWidgetItem(QString::number(r.getTotalPrice(), 'f', 2)));
        table->setItem(row, 6, new QTableWidgetItem(r.getStatusString()));
    }
    table->resizeColumnsToContents();
}

void RentalManager::saveToFile(const QString& filePath) const{
    QFile file(filePath);
    if(!file.open(QIODevice::WriteOnly)) {
        throw RentalException("Не вдалось відкрити файл для запису: " + filePath);
    }
    QTextStream out(&file);
    out << "ID;Car ID;User ID;Дата початку оренди;Дата кінця оренди;Фактична дата повернення;Ціна оренди за день;Кінцева ціна оренди;Статус\n";

    for(const Rental& rental : this->getAll()) {
        out << rental;
    }
    
    file.close();
}

bool RentalManager::hasActiveRentals(const QString& _userId) const {
    for (const Rental& r : rentals) {
        if (r.getUserId() == _userId && r.getStatus() == RentalStatus::Active)
            return true;
    }
    return false;
}
