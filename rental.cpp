#include "rental.h"
#include <algorithm>

Rental::Rental() = default;

Rental::Rental(const QString& _id, const QString& _userId, const QString& _carId, const QDateTime& _startDate, const QDateTime& _endDate, const double _pricePerDay) : id(_id), userId(_userId), carId(_carId), startDate(_startDate), endDate(_endDate), pricePerDay(_pricePerDay) {
    int days = std::max(1, static_cast<int>(_startDate.daysTo(_endDate)));
    this->totalPrice = days * _pricePerDay;
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

void Rental::complete() {
    if (this->getStatus() != RentalStatus::Active) {
        return;
    }

    this->returnedDate = QDateTime::currentDateTime();
    this->status = RentalStatus::Completed;

    if (returnedDate > this->getEndDate()) {
        int overdueDays = std::max(1, static_cast<int>(endDate.daysTo(returnedDate)));
        this->totalPrice += overdueDays * OVERDUE_MULTIPLIER * this->getPricePerDay();
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

const QJsonObject& operator >> (const QJsonObject& json, Rental& rental) {
    rental.id = json["id"].toString();
    rental.carId = json["carId"].toString();
    rental.userId = json["userId"].toString();
    rental.startDate = QDateTime::fromString(json["startDate"].toString(), Qt::ISODate);
    rental.endDate = QDateTime::fromString(json["endDate"].toString(), Qt::ISODate);
    rental.returnedDate = QDateTime::fromString(json["returnedDate"].toString(), Qt::ISODate);
    rental.pricePerDay = json["pricePerDay"].toDouble();
    rental.totalPrice = json["totalPrice"].toDouble();
    rental.status = Rental::stringToStatus(json["status"].toString());

    return json;
}
