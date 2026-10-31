#include "user.h"
#include <QRegularExpression>


User::User(const QString& _id, const QString& _name, const QString& _email, const QString& _password, Role _role) : id(_id), name(_name), email(_email), password(PasswordHasher::hashPassword(_password)), role(_role) {}

Role User::stringToRole(const QString& str) {
    if (str == "Адміністратор") {
        return Role::Administrator;
    }
    if (str == "Користувач") {
        return Role::Customer;
    }
    return Role::Customer;
}



Role User::getRole() const {
    return this->role;
}

QString User::getRoleString() const {
    switch(this->getRole()){
        case Role::Administrator:
            return "Адміністратор";
        case Role::Customer:
        default:
            return "Користувач";
    }
}

bool User::isAdmin() const {
    return this->getRole() == Role::Administrator;
}

QString User::getId() const {
    return this->id;
}

QString User::getName() const {
    return this->name;
}

QString User::getEmail() const {
    return this->email;
}

QString User::getPassword() const {
    return this->password ;
}

void User::setName(const QString& _name) {
    if (!_name.trimmed().isEmpty()) {
        this->name = _name;
        return;
    }
    throw InvalidNameException();
}

void User::setEmail(const QString& _email) {
    static const QRegularExpression emailRegex("^[^@\\s]+@[^@\\s]+\\.[^@\\s]+$");
    if (!emailRegex.match(_email.trimmed()).hasMatch()) {
        throw InvalidEmailException();
    }
    this->email = _email.trimmed().toLower();
}

void User::setPassword(const QString& _password) {
    if (_password.length() < 8) {
        throw PasswordTooShortException();
    }

    this->password = PasswordHasher::hashPassword(_password);
}


QJsonObject& operator << (QJsonObject& json, const User& user) {
    json["id"] = user.id;
    json["name"] = user.name;
    json["email"] = user.email;
    json["password"] = user.password;
    json["role"] = user.getRoleString();
    return json;
}

const QJsonObject& operator >> (const QJsonObject& json, User& user) {
    user.id = json["id"].toString();
    user.name = json["name"].toString();
    user.email = json["email"].toString();
    user.password = json["password"].toString();
    user.role = User::stringToRole(json["role"].toString());

    return json;
}
