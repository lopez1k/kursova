#include "carmanager.h"




CarManager::CarManager(QString _filename) : filename(_filename) {
    loadFromJson();
}

CarManager::~CarManager() {
    saveToJson();
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

    for(const QJsonValue& value: jsonArray) {
        if (value.isObject()) {
            Car car;
            value.toObject() >> car;
            cars.append(car);
        }
    }
}

void CarManager::addCar(const Car& _car) {
    cars.append(_car);
}

const QList<Car>& CarManager::getAllCars() const {
    return cars;
}

void CarManager::loadCarsTable(QTableWidget* ui, bool isAvailable) const {
    const QList<Car>& cars = this->getAllCars();
    ui->setColumnCount(9);
    ui->setRowCount(0);
    int currentRow = 0;
    for (int i = 0; i < cars.size(); ++i) {
        const Car& car = cars[i];
        if (isAvailable == true) {
            if (car.getStatus() == Status::NotAvailable || car.getStatus() == Status::Taken) {
                continue;
            }
        }
        ui->setRowCount(currentRow + 1);
        ui->setItem(currentRow, 0, new QTableWidgetItem(car.getId()));
        ui->setItem(currentRow, 1, new QTableWidgetItem(car.getBrand()));
        ui->setItem(currentRow, 2, new QTableWidgetItem(car.getModel()));
        ui->setItem(currentRow, 3, new QTableWidgetItem(QString::number(car.getManufacturerYear())));
        ui->setItem(currentRow, 4, new QTableWidgetItem(QString::number(car.getMileage(), 'f', 3)));
        ui->setItem(currentRow, 5, new QTableWidgetItem(car.getCategoryString()));
        ui->setItem(currentRow, 6, new QTableWidgetItem(QString::number(car.getSeats())));
        ui->setItem(currentRow, 7, new QTableWidgetItem(QString::number(car.getRentPrice(), 'f', 3)));
        ui->setItem(currentRow, 8, new QTableWidgetItem(car.getStatusString()));
        ++currentRow;

    }
}


#include <QLabel>

void CarManager::loadCarsList(QListWidget* ui, bool isAvailable) const {
    const QList<Car>& cars = this->getAllCars();

    ui->clear();
    QSize itemSize(260, 160);

    for (const Car& car : cars) {
        if (isAvailable) {
            if (car.getStatus() == Status::NotAvailable || car.getStatus() == Status::Taken) {
                continue;
            }
        }

        QString cardHtml = QString(
            "<div style='background-color: #2b2b2b; padding: 10px; border-radius: 6px;'>"
            "<div style='font-size: 13px; font-weight: bold; color: #ffffff; margin-bottom: 4px;'>"
            "%1 %2 <span style='color: #8a8a8a; font-weight: normal;'>(%3)</span>"
            "</div>"
            "<hr style='border: none; border-top: 1px solid #3a3a3a; margin: 6px 0;'>"
            "<table border='0' cellspacing='2' cellpadding='0' style='color: #cccccc; font-size: 12px;'>"
            "<tr><td style='color: #8a8a8a; padding-right: 10px;'>Клас:</td><td><b>%4</b></td></tr>"
            "<tr><td style='color: #8a8a8a; padding-right: 10px;'>Місць:</td><td><b>%5</b></td></tr>"
            "<tr><td style='color: #8a8a8a; padding-right: 10px;'>Пробіг:</td><td><b>%6 км</b></td></tr>"
            "</table>"
            "<hr style='border: none; border-top: 1px solid #3a3a3a; margin: 6px 0;'>"
            "<div style='font-size: 13px; color: #4caf50; font-weight: bold;'>"
            "%7 грн <span style='font-size: 11px; color: #8a8a8a; font-weight: normal;'>/ день</span>"
            "</div>"
            "</div>"
        ).arg(car.getBrand())
        .arg(car.getModel())
        .arg(car.getManufacturerYear())
        .arg(car.getCategoryString())
        .arg(car.getSeats())
        .arg(QString::number(car.getMileage(), 'f', 0))
        .arg(QString::number(car.getRentPrice(), 'f', 2));

        auto *item = new QListWidgetItem(ui);
        item->setSizeHint(itemSize);

        item->setData(Qt::UserRole, car.getId());

        QLabel *cardLabel = new QLabel();
        cardLabel->setTextFormat(Qt::RichText);
        cardLabel->setText(cardHtml);
        cardLabel->setWordWrap(true);
        cardLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);

        ui->addItem(item);
        ui->setItemWidget(item, cardLabel);
    }
}


Car* CarManager::getCarById(const QString& _uid) {
    for (Car& c : cars) {
        if (c.getId().trimmed() == _uid) {
            return &c;
        }
    }
}