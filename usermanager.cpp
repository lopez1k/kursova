#include "usermanager.h"

UserManager::UserManager(QString _filename) : currentUser(nullptr), filename(_filename) {
    loadFromJson();
}


UserManager::~UserManager() {
    saveToJson();
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

    for(const QJsonValue& value: jsonArray) {
        if (value.isObject()) {
            User user;
            value.toObject() >> user;
            users.append(user);
        }
    }
}


void UserManager::addUser(const User& _user) {
    users.append(_user);
}

bool UserManager::login(const QString& _email, const QString& _password) {
    User* user = this->findUserByEmail(_email);

    if (!PasswordHasher::verifyPassword(_password, user->getPassword())) {
        throw InvalidPasswordException();
    }

    this->currentUser = user;
    return true;
}


void UserManager::logout() {
    this->currentUser = nullptr;
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

    throw UserNotFoundException(id);
}


User* UserManager::getCurrentUser() const {
    return this->currentUser;
}


const QList<User>& UserManager::getAllUsers() const {
    return users;
}