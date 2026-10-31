#ifndef CAREXCEPTION_H
#define CAREXCEPTION_H

#include "appexception.h"

class CarException : public AppException {
public:
    explicit CarException(const QString& msg);
};

class InvalidCarDataException : public CarException {
public:
    explicit InvalidCarDataException(const QString& msg);
};

#endif // CAREXCEPTION_H
