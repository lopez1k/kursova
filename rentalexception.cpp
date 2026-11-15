#include "rentalexception.h"

RentalException::RentalException(const QString& msg) : AppException(msg) {}

CarNotFoundException::CarNotFoundException(const QString& carId) : RentalException("Автомобіль з ID " + carId + " не знайдено.") {}

CarNotAvailableException::CarNotAvailableException(const QString& carName) : RentalException("Автомобіль " + carName + " зараз недоступний для оренди.") {}

InvalidRentalPeriodException::InvalidRentalPeriodException() : RentalException("Некоректний період оренди: початок не може бути в минулому, а кінець має бути пізніше за початок.") {}

RentalNotFoundException::RentalNotFoundException(const QString& rentalId) : RentalException("Оренду з ID " + rentalId + " не знайдено.") {}

RentalNotActiveException::RentalNotActiveException(const QString& rentalId) : RentalException("Оренда " + rentalId + " не є активною, дію неможливо виконати.") {}

InvalidRentalDataException::InvalidRentalDataException(const QString& msg) : RentalException(msg) {}
