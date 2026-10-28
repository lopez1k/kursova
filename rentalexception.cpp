#include "rentalexception.h"

RentalException::RentalException(const QString& msg) : AppException(msg) {}

CarNotFoundException::CarNotFoundException(const QString& carId) : RentalException("Автомобіль з ID " + carId + " не знайдено.") {}

CarNotAvailableException::CarNotAvailableException(const QString& carName) : RentalException("Автомобіль " + carName + " зараз недоступний для оренди.") {}

InvalidRentalPeriodException::InvalidRentalPeriodException() : RentalException("Некоректний період оренди: початок не може бути в минулому, а кінець має бути пізніше за початок.") {}
