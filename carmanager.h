#ifndef CARMANAGER_H
#define CARMANAGER_H
#include "car.h"
#include <QList>
#include <QTableWidget>
#include <QListWidget>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QMessageBox>

class CarManager
{
private:
    QList<Car> cars;
    QString filename;
public:
    CarManager(QString _filename = "cars.json");
    ~CarManager();

    void saveToJson() const;
    void loadFromJson();
    void addCar(const Car& _car);
    const QList<Car>& getAllCars() const;
    void loadCarsTable(QTableWidget* ui, bool isAvailable = true) const;
    void loadCarsList(QListWidget* ui, bool isAvailable = true) const;
    Car* getCarById(const QString& _uid);

};

#endif // CARMANAGER_H
