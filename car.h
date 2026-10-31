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

public:
    Car(const QString& _id = "C-1", const QString& _brand = "Reno", const QString& _model = "Megan", const Category& _category = Category::Econom, const int& _manufacturerYear = 2004, const int& _seats = 4, const double& _rentPrice = 751.0, const double& mileage = 196.42, const Status& _status = Status::NotAvailable);
    
    // копіювання лишаємо як було (неявне) — потрібне для QList<Car>
    Car(const Car& other) = default;
    Car& operator=(const Car& other) = default;

    Car(Car&& other) noexcept;
    Car& operator=(Car&& other) noexcept;

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
    void setId(const QString& _id);
    void setBrand(const QString& _brand);
    void setModel(const QString& _model);
    void setCategory(const Category _category);
    void setManufacturerYear(const int _manufacturerYear);
    void setSeats(const int _seats);
    void setRentPrice(const double _rentPrice);
    void setMileage(const double _mileage);
    void setStatus(const Status _status);
    static Category stringToCategory(const QString& str);
    static Status stringToStatus(const QString& str);


    friend QJsonObject& operator << (QJsonObject& json, const Car& car);
    friend const QJsonObject& operator >> (const QJsonObject& json, Car& car);

};

#endif // CAR_H
