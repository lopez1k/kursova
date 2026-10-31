#include "userexception.h"

UserException::UserException(const QString& msg) : AppException(msg) {}

UserNotFoundException::UserNotFoundException(const QString& email) : UserException("Користувача з поштою " + email + " не знайдено.") {}

UserNotFoundByIdException::UserNotFoundByIdException(const QString& id) : UserException("Користувача з ID " + id + " не знайдено.") {}

InvalidPasswordException::InvalidPasswordException() : UserException("Введено неправильний пароль.") {}

PasswordTooShortException::PasswordTooShortException(int minLength) : UserException(QString("Пароль занадто короткий. Мінімальна довжина - %1 символів.").arg(minLength)) {}

InvalidNameException::InvalidNameException() : UserException("Введіть коректне ім'я") {}

InvalidEmailException::InvalidEmailException() : UserException("Введіть коректний формат електронної пошти") {}

EmailAlreadyExistsException::EmailAlreadyExistsException(const QString& email) : UserException("Користувач з поштою " + email + " вже існує.") {}