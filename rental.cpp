#include "rental.h"
#include "rentalexception.h"
#include <QRegularExpression>
#include <algorithm>

Rental::Rental() = default;

Rental::Rental(const QString& _id, const QString& _userId, const QString& _carId, const QDateTime& _startDate, const QDateTime& _endDate, const double _pricePerDay) {
    setId(_id);
    setUserId(_userId);
    setCarId(_carId);
    setStartDate(_startDate);
    setEndDate(_endDate);
    setPricePerDay(_pricePerDay);

    int days = std::max(1, static_cast<int>(startDate.daysTo(endDate)));
    setTotalPrice(days * pricePerDay);
}

RentalStatus Rental::stringToStatus(const QString& str) {
    if (str == "Скасовано") {
        return RentalStatus::Cancelled;
    }
    if (str == "Повернено") {
        return RentalStatus::Completed;
    }
    return RentalStatus::Active;
}


QString Rental::getId() const {
    return this->id;
}

QString Rental::getUserId() const {
    return this->userId;
}

QString Rental::getCarId() const {
    return this->carId;
}

QDateTime Rental::getStartDate() const {
    return this->startDate;
}

QDateTime Rental::getEndDate() const {
    return this->endDate;
}

QDateTime Rental::getReturnedDate() const {
    return this->returnedDate;
}

double Rental::getPricePerDay() const {
    return this->pricePerDay;
}

double Rental::getTotalPrice() const {
    return this->totalPrice;
}

RentalStatus Rental::getStatus() const {
    return this->status;
}

QString Rental::getStatusString() const {
    switch(this->getStatus()) {
        case RentalStatus::Cancelled: return "Скасовано";
        case RentalStatus::Completed: return "Повернено";
        case RentalStatus::Active:
        default: return "Активно";
    }
}

void Rental::setId(const QString& _id) {
    static const QRegularExpression pattern("^R-\\d+$");
    if (_id.trimmed().isEmpty()) {
        throw InvalidRentalDataException("ID оренди не може бути порожнім.");
    }
    if (!pattern.match(_id).hasMatch()) {
        throw InvalidRentalDataException("Некоректний ID оренди: " + _id + " (очікується формат R-<число>).");
    }
    this->id = _id;
}

void Rental::setUserId(const QString& _userId) {
    static const QRegularExpression pattern("^U-\\d+$");
    if (_userId.trimmed().isEmpty()) {
        throw InvalidRentalDataException("ID користувача не може бути порожнім.");
    }
    if (!pattern.match(_userId).hasMatch()) {
        throw InvalidRentalDataException("Некоректний ID користувача: " + _userId + " (очікується формат U-<число>).");
    }
    this->userId = _userId;
}

void Rental::setCarId(const QString& _carId) {
    static const QRegularExpression pattern("^C-\\d+$");
    if (_carId.trimmed().isEmpty()) {
        throw InvalidRentalDataException("ID авто не може бути порожнім.");
    }
    if (!pattern.match(_carId).hasMatch()) {
        throw InvalidRentalDataException("Некоректний ID авто: " + _carId + " (очікується формат C-<число>).");
    }
    this->carId = _carId;
}

void Rental::setStartDate(const QDateTime& _startDate) {
    if (!_startDate.isValid()) {
        throw InvalidRentalDataException("Некоректна дата початку оренди.");
    }
    if (endDate.isValid() && _startDate >= endDate) {
        throw InvalidRentalDataException("Дата початку оренди має бути раніше за дату завершення.");
    }
    this->startDate = _startDate;
}

void Rental::setEndDate(const QDateTime& _endDate) {
    if (!_endDate.isValid()) {
        throw InvalidRentalDataException("Некоректна дата завершення оренди.");
    }
    if (startDate.isValid() && _endDate <= startDate) {
        throw InvalidRentalDataException("Дата завершення оренди має бути пізніше за дату початку.");
    }
    this->endDate = _endDate;
}

void Rental::setReturnedDate(const QDateTime& _returnedDate) {
    // порожня (невалідна) дата означає, що авто ще не повернули
    if (!_returnedDate.isValid()) {
        this->returnedDate = QDateTime();
        return;
    }
    if (startDate.isValid() && _returnedDate < startDate) {
        throw InvalidRentalDataException("Дата повернення не може бути раніше за дату початку оренди.");
    }
    this->returnedDate = _returnedDate;
}

void Rental::setPricePerDay(const double _pricePerDay) {
    if (_pricePerDay < 0) {
        throw InvalidRentalDataException("Ціна за добу не може бути від'ємною.");
    }
    this->pricePerDay = _pricePerDay;
}

void Rental::setTotalPrice(const double _totalPrice) {
    if (_totalPrice < 0) {
        throw InvalidRentalDataException("Загальна вартість оренди не може бути від'ємною.");
    }
    this->totalPrice = _totalPrice;
}

void Rental::setStatus(const RentalStatus _status) {
    this->status = _status;
}

void Rental::complete() {
    if (status != RentalStatus::Active) {
        return;
    }

    returnedDate = QDateTime::currentDateTime();
    status = RentalStatus::Completed;

    if (returnedDate > endDate) {
        int plannedDays = std::max(1, static_cast<int>(startDate.daysTo(endDate)));
        int overdueDays = std::max(1, static_cast<int>(endDate.daysTo(returnedDate)));
        totalPrice = plannedDays * pricePerDay
                   + overdueDays * OVERDUE_MULTIPLIER * pricePerDay;
    } else {
        int actualDays = std::max(1, static_cast<int>(startDate.daysTo(returnedDate)));
        totalPrice = actualDays * pricePerDay;
    }
}


void Rental::cancel() {
    if (status != RentalStatus::Active)
        return;

    status = RentalStatus::Cancelled;
    totalPrice = 0.0;
}



QJsonObject& operator << (QJsonObject& json, const Rental& rental) {
    json["id"] = rental.id;
    json["userId"] = rental.userId;
    json["carId"] = rental.carId;
    json["startDate"] = rental.startDate.toString(Qt::ISODate);
    json["endDate"] = rental.endDate.toString(Qt::ISODate);
    json["returnedDate"] = rental.returnedDate.toString(Qt::ISODate);
    json["pricePerDay"] = rental.pricePerDay;
    json["totalPrice"] = rental.totalPrice;
    json["status"] = rental.getStatusString();

    return json;
}

QTextStream& operator << (QTextStream& out, const Rental& rental) {
    out << rental.id << ";" << rental.carId << ";" << rental.userId << ";" << rental.startDate.toString(Qt::ISODate) << ";" << rental.endDate.toString(Qt::ISODate) << ";" << rental.returnedDate.toString(Qt::ISODate) << ";" << rental.pricePerDay << ";" << rental.totalPrice << ";" << rental.getStatusString() << "\n";
    return out;
}

const QJsonObject& operator >> (const QJsonObject& json, Rental& rental) {
    rental.setId(json["id"].toString());
    rental.setCarId(json["carId"].toString());
    rental.setUserId(json["userId"].toString());
    rental.setStartDate(QDateTime::fromString(json["startDate"].toString(), Qt::ISODate));
    rental.setEndDate(QDateTime::fromString(json["endDate"].toString(), Qt::ISODate));
    rental.setReturnedDate(QDateTime::fromString(json["returnedDate"].toString(), Qt::ISODate));
    rental.setPricePerDay(json["pricePerDay"].toDouble());
    rental.setTotalPrice(json["totalPrice"].toDouble());
    rental.setStatus(Rental::stringToStatus(json["status"].toString()));

    return json;
}
