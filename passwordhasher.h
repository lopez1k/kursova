#ifndef PASSWORDHASHER_H
#define PASSWORDHASHER_H
#include <QString>

class PasswordHasher {
public:
    static QString hashPassword(const QString& password, const QString& salt = "CarRental_Salt_2026");
    static bool verifyPassword(const QString& rawPassword, const QString& storedHash, const QString& salt = "CarRental_Salt_2026");
};

#endif // PASSWORDHASHER_H
