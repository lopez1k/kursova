#include "carmanager.h"
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QRegularExpression>
#include <algorithm>



CarManager::CarManager(QString _filename) : filename(_filename) {
    loadFromJson();
}

CarManager::~CarManager() {
    saveToJson();
}

QString CarManager::generateId() {
    return QString("C-%1").arg(nextId++);
}



void CarManager::saveToJson() const {
    QJsonArray jsonArray;

    for(const Car& car: this->cars) {
        QJsonObject carObject;
        carObject << car;
        jsonArray.append(carObject);
    }

    QJsonDocument jsonDocument(jsonArray);

    QFile file(this->filename);
    if (!file.open(QIODevice::WriteOnly)) {
        return;
    }

    file.write(jsonDocument.toJson(QJsonDocument::Indented));
    file.close();
}

void CarManager::loadFromJson() {
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

    this->cars.clear();

    QJsonArray jsonArray = jsonDocument.array();

    int maxNum = 0;

    for(const QJsonValue& value: jsonArray) {
        if (value.isObject()) {
            try {
                Car car;
                value.toObject() >> car;
                cars.append(car);
                QString numStr = car.getId();
                numStr.remove(QRegularExpression("[^0-9]"));

                int currentNum = numStr.toInt();
                if (currentNum > maxNum) {
                    maxNum = currentNum;
                }
            } catch (const AppException& ex) {
                qWarning() << "При зчитуванні з файлу сталась помилка: " << ex.what();
            }
        }
    }
    this->nextId = maxNum + 1;
}




void CarManager::addCar(const QString& _brand, const QString& _model, const Category& _category, const int& _manufacturerYear, const int& _seats, const double& _rentPrice, const double& _mileage, const Status& _status) {
    Car car("", _brand, _model, _category, _manufacturerYear, _seats, _rentPrice, _mileage, _status);
    car.setId(generateId());
    cars.append(car);
    saveToJson();
}

const QList<Car>& CarManager::getAllCars() const {
    return cars;
}



Car* CarManager::getCarById(const QString& _uid) {
    for (Car& c : cars) {
        if (c.getId().trimmed() == _uid) {
            return &c;
        }
    }
    return nullptr;
}


QList<Car> CarManager::mergeSort(const QList<Car>& list, bool isAscending) {
    if (list.size()<= 1) {
        return list;
    }

    int mid = list.size() / 2;

    QList<Car> left;
    QList<Car> right;

    for(int i = 0; i < mid; ++i) {
        left.append(list[i]);
    }

    for(int i = mid; i < list.size(); ++i) {
        right.append(list[i]);
    }

    return merge(mergeSort(left, isAscending), mergeSort(right, isAscending), isAscending);
}

QList<Car> CarManager::merge(const QList<Car>& left, const QList<Car>& right, bool isAscending) {
    QList<Car> result;
    int i = 0;
    int j = 0;

    while(i < left.size() && j < right.size()) {
        if(isAscending) {
            if(left[i].getRentPrice() <= right[j].getRentPrice()) {
                result.append(left[i]);
                ++i;
            }
            else{
                result.append(right[j]);
                ++j;
            }
        }
        else {
            if(left[i].getRentPrice() >= right[j].getRentPrice()) {
                result.append(left[i]);
                ++i;
            }
            else{
                result.append(right[j]);
                ++j;
            }
        }
    }

    while(i < left.size()) {
        result.append(left[i]);
        ++i;
    }

    while(j < right.size()) {
        result.append(right[j]);
        ++j;
    }

    return result;
}

void CarManager::saveToFile(const QString& filePath) const {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        throw CarException("Не вдалось відкрити файл для запису: " + filePath);
    }
    QTextStream out(&file);

    out << "ID;Марка;Модель;Категорія;Рік випуску;Місць;Ціна оренди;Пробіг;Статус\n";
    
    for(const Car& car : this->getAllCars()){
        out << car;
    }

    file.close();
}

QList<Car> CarManager::selectMinMaxMileage(const QList<Car>& list) const {
    if (list.isEmpty()) {
        return {};
    }

    double minMileage = list[0].getMileage();
    double maxMileage = list[0].getMileage();

    for (const Car& car : list) {
        if (car.getMileage() < minMileage) {
            minMileage = car.getMileage();
        }
        if (car.getMileage() > maxMileage) {
            maxMileage = car.getMileage();
        }
    }

    QList<Car> result;
    for (const Car& car : list) {
        if (car.getMileage() == minMileage || car.getMileage() == maxMileage) {
            result.append(car);
        }
    }
    return result;
}

QList<Car> CarManager::groupByClassYear(QList<Car> list) const {
    std::sort(list.begin(), list.end(), [](const Car& a, const Car& b) {
        if (a.getCategory() != b.getCategory()) {
            return static_cast<int>(a.getCategory()) < static_cast<int>(b.getCategory());
        }
        return a.getManufacturerYear() > b.getManufacturerYear();
    });
    return list;
}
