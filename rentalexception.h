#ifndef RENTALEXCEPTION_H
#define RENTALEXCEPTION_H

#include "appexception.h"

class RentalException : public AppException {
public:
    explicit RentalException(const QString& msg);
};

class CarNotFoundException : public RentalException {
public:
    explicit CarNotFoundException(const QString& carId);
};

class CarNotAvailableException : public RentalException {
public:
    explicit CarNotAvailableException(const QString& carName);
};

class InvalidRentalPeriodException : public RentalException {
public:
    InvalidRentalPeriodException();
};

#endif // RENTALEXCEPTION_H
