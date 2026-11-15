#include "usermanager.h"
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QRegularExpression>

UserManager::UserManager(QString _filename) : filename(_filename) {
    loadFromJson();
}


UserManager::~UserManager() {
    saveToJson();
}

QString UserManager::generateId() {
    return QString("U-%1").arg(nextId++);
}


void UserManager::saveToJson() const {
    QJsonArray jsonArray;

    for(const User& user: this->users) {
        QJsonObject userObject;
        userObject << user;
        jsonArray.append(userObject);
    }

    QJsonDocument jsonDocument(jsonArray);

    QFile file(this->filename);
    if (!file.open(QIODevice::WriteOnly)) {
        return;
    }

    file.write(jsonDocument.toJson(QJsonDocument::Indented));
    file.close();
}

void UserManager::loadFromJson() {
    QFile file(this->filename);
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument jsonDocument = QJsonDocument::fromJson(data);
    if (!jsonDocument.isArray()) {
        return;
    }

    this->users.clear();

    QJsonArray jsonArray = jsonDocument.array();

    int maxNum = 0;

    for(const QJsonValue& value: jsonArray) {
        if (value.isObject()) {
            try {
                User user;
                value.toObject() >> user;
                users.append(user);

                QString numStr = user.getId();
                numStr.remove(QRegularExpression("[^0-9]"));

                int currentNum = numStr.toInt();
                if (currentNum > maxNum) {
                    maxNum = currentNum;
                }
            } catch (const AppException& ex) {
                qWarning() << "При зчитуванні з файлу сталась помилка: " << ex.what();
            }
        }
    }
    this->nextId = maxNum + 1;
}


void UserManager::addUser(const User& _user) {
    users.append(_user);
    saveToJson();
}

void UserManager::addUser(const QString& _name, const QString& _email, const QString& _password, Role _role) {
    if (_name.trimmed().isEmpty()) {
        throw InvalidNameException();
    }

    static const QRegularExpression emailRegex("^[^@\\s]+@[^@\\s]+\\.[^@\\s]+$");
    if (!emailRegex.match(_email.trimmed()).hasMatch()) {
        throw InvalidEmailException();
    }

    if (_password.length() < 8) {
        throw PasswordTooShortException();
    }

    const QString cleanEmail = _email.trimmed().toLower();
    for (const User& u : users) {
        if (u.getEmail().trimmed().toLower() == cleanEmail) {
            throw EmailAlreadyExistsException(_email);
        }
    }

    users.append(User(generateId(), _name.trimmed(), cleanEmail, _password, _role));
    saveToJson();
}

bool UserManager::login(const QString& _email, const QString& _password) {
    if(_password.length() < 8) {
        throw PasswordTooShortException(8);
    }
    User* user = this->findUserByEmail(_email);

    if (!PasswordHasher::verifyPassword(_password, user->getPassword())) {
        throw InvalidPasswordException();
    }

    this->currentUserId = user->getId();
    return true;
}


void UserManager::logout() {
    this->currentUserId.clear();
}

User* UserManager::findUserByEmail(const QString& email) {
    QString cleanEmail = email.trimmed().toLower();

    for (User& u : users) {
        if (u.getEmail().trimmed().toLower() == cleanEmail) {
            return &u;
        }
    }

    throw UserNotFoundException(email);
}

User* UserManager::findUserById(const QString& id) {
    for (User& u : users) {
        if (u.getId() == id) {
            return &u;
        }
    }

    throw UserNotFoundByIdException(id);
}


User* UserManager::getCurrentUser() {
    for (User& u : users) {
        if (u.getId() == this->currentUserId) {
            return &u;
        }
    }
    return nullptr;
}


const QList<User>& UserManager::getAllUsers() const {
    return users;
}

void UserManager::saveToFile(const QString& filePath) const {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        throw UserException("Не вдалося відкрити файл для запису: " + filePath);
    }
    QTextStream out(&file);
    out << "ID;Ім'я;Email;Роль\n";
    for (const User& u : this->getAllUsers()) {
        out << u;
    }
    file.close();
}

void UserManager::changePassword(const QString& _oldPassword, const QString& _newPassword) {
    User* user = this->getCurrentUser();
    if (!user) {
        throw UserException("Немає авторизованого користувача.");
    }
    if (!PasswordHasher::verifyPassword(_oldPassword, user->getPassword())) {
        throw InvalidPasswordException();
    }
    if (_oldPassword == _newPassword) {
        throw UserException("Новий пароль має відрізнятися від поточного.");
    }
    user->setPassword(_newPassword);
    this->saveToJson();
}

void UserManager::removeUser(const QString& _id) {
    if (_id == this->currentUserId) {
        throw UserException("Не можна видалити власний обліковий запис.");
    }
    for (int i = 0; i < users.size(); ++i) {
        if (users[i].getId() == _id) {
            users.removeAt(i);
            this->saveToJson();
            return;
        }
    }
    throw UserNotFoundByIdException(_id);
}
