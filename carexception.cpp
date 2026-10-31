#include "carexception.h"

CarException::CarException(const QString& msg) : AppException(msg) {}

InvalidCarDataException::InvalidCarDataException(const QString& msg) : CarException(msg) {}
