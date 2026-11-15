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

class RentalNotFoundException : public RentalException {
public:
    explicit RentalNotFoundException(const QString& rentalId);
};

class RentalNotActiveException : public RentalException {
public:
    explicit RentalNotActiveException(const QString& rentalId);
};

class InvalidRentalDataException : public RentalException {
public:
    explicit InvalidRentalDataException(const QString& msg);
};

#endif // RENTALEXCEPTION_H
