#include "appexception.h"

AppException::AppException(const QString& msg) : std::runtime_error(msg.toUtf8().constData()) {}
