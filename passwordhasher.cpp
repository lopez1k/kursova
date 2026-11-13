#include "passwordhasher.h"
#include <QCryptographicHash>

QString PasswordHasher::hashPassword(const QString& password, const QString& salt) {
        QByteArray data = (password + salt).toUtf8();
        QByteArray hash = QCryptographicHash::hash(data, QCryptographicHash::Sha256);
        return QString(hash.toHex());
}

bool PasswordHasher::verifyPassword(const QString& rawPassword, const QString& storedHash, const QString& salt) {
    return hashPassword(rawPassword, salt) == storedHash;
}