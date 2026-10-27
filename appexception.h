#ifndef APPEXCEPTION_H
#define APPEXCEPTION_H

#include <stdexcept>
#include <QString>

class AppException : public std::runtime_error {
public:
    explicit AppException(const QString& msg);
};

#endif // APPEXCEPTION_H
