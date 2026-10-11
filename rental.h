#ifndef RENTAL_H
#define RENTAL_H

#include <QString>
#include <QDateTime>
#include <QJsonObject>

enum class RentalStatus {
    Active,
    Completed,
    Cancelled
};

class Rental
{
private:
    QString id;
    QString userId;
    QString carId;
    QDateTime startDate;
    QDateTime endDate;
    QDateTime returnedDate;
    double pricePerDay = 0.0;
    double totalPrice = 0.0;
    RentalStatus status = RentalStatus::Active;
    static constexpr double OVERDUE_MULTIPLIER = 1.5;


public:
    Rental();
    Rental(const QString& _id, const QString& _userId, const QString& _carId, const QDateTime& _startDate, const QDateTime& _endDate, const double _pricePerDay);

    QString getId() const;
    QString getUserId() const;
    QString getCarId() const;
    QDateTime getStartDate() const;
    QDateTime getEndDate() const;
    QDateTime getReturnedDate() const;
    double getPricePerDay() const;
    double getTotalPrice() const;
    RentalStatus getStatus() const;
    QString getStatusString() const;

    void complete();
    void cancel();
    static RentalStatus stringToStatus(const QString& str);

    friend QJsonObject& operator << (QJsonObject& json, const Rental& rental);
    friend const QJsonObject& operator >> (const QJsonObject& json, Rental& rental);

};

#endif // RENTAL_H
