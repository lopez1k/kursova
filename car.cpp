#include "car.h"


Car::Car(const QString _id, const QString _brand, const QString _model, const Category _category, const int _manufacturerYear, const int _seats, const double _rentPrice, const double _mileage, const Status _status) : id(_id), brand(_brand), model(_model), category(_category), manufacturerYear(_manufacturerYear), seats(_seats), rentPrice(_rentPrice), mileage(_mileage), status(_status) {}


Category Car::stringToCategory(const QString& str) {
    if (str == "Бізнес") {
        return Category::Business;
    }
    if (str == "Комфорт") {
        return Category::Comfort;
    }
    return Category::Econom;
}


Status Car::stringToStatus(const QString& str) {
    if (str == "Зайнято") {
        return Status::Taken;
    }
    if (str == "Не доступно") {
        return Status::NotAvailable;
    }
    if (str == "Архівовано")
    {
        return Status::Archived;
    }
    return Status::Available;
}

QString Car::getId() const {
    return this->id;
}

QString Car::getBrand() const {
    return this->brand;
}


QString Car::getModel() const {
    return this->model;
}


Category Car::getCategory() const {
    return this->category;
}


QString Car::getCategoryString() const {
    switch(this->getCategory()) {
        case Category::Business: return "Бізнес";
        case Category::Comfort: return "Комфорт";
        case Category::Econom:
        default: return "Економ";
    }
}


int Car::getManufacturerYear() const {
    return this->manufacturerYear;
}

int Car::getSeats() const {
    return this->seats;
}


double Car::getRentPrice() const {
    return this->rentPrice;
}


double Car::getMileage() const {
    return this->mileage;
}

Status Car::getStatus() const {
    return this->status;
}

QString Car::getStatusString() const {
    switch(this->getStatus()) {
        case Status::Available: return "Доступно";
        case Status::Taken: return "Зайнято";
        case Status::Archived: return "Архівовно";
        case Status::NotAvailable:
        default: return "Недоступно";
    }
}

void Car::setId(const QString _id) {
    this->id = _id;
}

void Car::setBrand(const QString _brand) {
    this->brand = _brand;
}

void Car::setModel(const QString _model) {
    this->model = _model;
}

void Car::setCategory(const Category _category) {
    this->category = _category;
}

void Car::setManufacturerYear(const int _manufacturerYear) {
    this->manufacturerYear = _manufacturerYear;
}

void Car::setSeats(const int _seats) {
    this->seats = _seats;
}

void Car::setRentPrice(const double _rentPrice) {
    this->rentPrice = _rentPrice;
}

void Car::setMileage(const double _mileage) {
    this->mileage = _mileage;
}

void Car::setStatus(const Status _status) {
    this->status = _status;
}


QJsonObject& operator << (QJsonObject& json, const Car& car) {
    json["id"] = car.id;
    json["brand"] = car.brand;
    json["model"] = car.model;
    json["category"] = car.getCategoryString();
    json["manufacturerYear"] = car.manufacturerYear;
    json["seats"] = car.seats;
    json["rentPrice"] = car.rentPrice;
    json["mileage"] = car.mileage;
    json["status"] = car.getStatusString();

    return json;
}

const QJsonObject& operator >> (const QJsonObject& json, Car& car) {
    car.id = json["id"].toString();
    car.brand = json["brand"].toString();
    car.model = json["model"].toString();
    car.category = Car::stringToCategory(json["category"].toString());
    car.manufacturerYear = json["manufacturerYear"].toInt();
    car.seats = json["seats"].toInt();
    car.rentPrice = json["rentPrice"].toDouble();
    car.mileage = json["mileage"].toDouble();
    car.status = Car::stringToStatus(json["status"].toString());

    return json;
}
