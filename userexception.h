#ifndef USEREXCEPTION_H
#define USEREXCEPTION_H

#include <stdexcept>
#include <QString>

class UserException : public std::runtime_error {
public:
    explicit UserException(const QString& msg);
};

class UserNotFoundException : public UserException {
public:
    explicit UserNotFoundException(const QString& email);
};


class InvalidPasswordException : public UserException {
public:
    InvalidPasswordException();
};

class PasswordTooShortException : public UserException {
public:
    explicit PasswordTooShortException(int minLength = 8);
};

class InvalidNameException : public UserException {
public:
    InvalidNameException();
};

class InvalidEmailException : public UserException {
public:
    InvalidEmailException();
};

class EmailAlreadyExistsException : public UserException {
public:
    explicit EmailAlreadyExistsException(const QString& email);
};

#endif // USEREXCEPTION_H