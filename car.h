#ifndef CAR_H
#define CAR_H
#include "QString"
#include <QJsonObject>


enum class Category {
    Econom,
    Comfort,
    Business
};

enum class Status {
    Available,
    Taken,
    NotAvailable,
    Archived
};

class Car
{
private:
    QString id;
    QString brand;
    QString model;
    Category category;
    int manufacturerYear;
    int seats;
    double rentPrice;
    double mileage;
    Status status;
    static Category stringToCategory(const QString& str);
    static Status stringToStatus(const QString& str);


public:
    Car(const QString _id = "sdsd", const QString _brand = "Meno", const QString _model = "Regan", const Category _category = Category::Econom, const int _manufacturerYear = 2004, const int _seats = 4, const double _rentPrice = 751.0, const double mileage = 196.42, const Status _status = Status::NotAvailable);
    QString getId() const;
    QString getBrand() const;
    QString getModel() const;
    Category getCategory() const;
    QString getCategoryString() const;

    int getManufacturerYear() const;
    int getSeats() const;
    double getRentPrice() const;
    double getMileage() const;
    Status getStatus() const;
    QString getStatusString() const;
    void setId(const QString _id = "sdsd");
    void setBrand(const QString _brand = "Meno");
    void setModel(const QString _model = "Regan");
    void setCategory(const Category _category = Category::Econom);
    void setManufacturerYear(const int _manufacturerYear = 2004);
    void setSeats(const int _seats = 4);
    void setRentPrice(const double _rentPrice = 751.0);
    void setMileage(const double _mileage = 196.42);
    void setStatus(const Status _status = Status::NotAvailable);

    friend QJsonObject& operator << (QJsonObject& json, const Car& car);
    friend const QJsonObject& operator >> (const QJsonObject& json, Car& car);

};

#endif // CAR_H
