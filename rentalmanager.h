#ifndef RENTALMANAGER_H
#define RENTALMANAGER_H

#include "QList"
#include "rental.h"
#include "carmanager.h"
#include "usermanager.h"

class RentalManager
{
private:
    QList<Rental> rentals;
    UserManager& usermanager;
    CarManager& carmanager;
    QString filename;
    int nextId = 1;
    QString generateId();
public:
    RentalManager(UserManager& _usermanager, CarManager& _carmanager, QString _filename = "rentals.json");
    ~RentalManager();

    void saveToJson() const;
    void loadFromJson();

    void rentCar(const QString&  _userId, const QString& _carId, const QDateTime& _endDate);
    bool completeRental(const QString& _id);
    bool cancelRental(const QString& _id);

    Rental* findById(const QString& _id);
    const QList<Rental>& getAll() const;
    QList<Rental> getByUser(const QString& userId) const;
    QList<Rental> getByCar(const QString& carId) const;
    QList<Rental> getActive() const;
    void loadRentalsTable(QTableWidget* ui, const QString& _userId = "") const;

    bool isCarRented(const QString& _carId) const;
};

#endif // RENTALMANAGER_H
