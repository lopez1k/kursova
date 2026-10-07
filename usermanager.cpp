#include "usermanager.h"

UserManager::UserManager() : currentUser(nullptr) {
    // Для тестування можна додати дефолтного адміна
    this->users.append(User("1", "Admin", "admin@gmail.com", "admin1", Role::Administrator));
    this->users.append(User("2", "User", "user@gmail.com", "user1", Role::Customer));
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

    for (int i = 0; i < users.size(); ++i) {
        if (users[i].getEmail().trimmed().toLower() == cleanEmail) {
            return &users[i];
        }
    }

    throw UserNotFoundException(email);
}

User* UserManager::getCurrentUser() const {
    return this->currentUser;
}


const QList<User>& UserManager::getAllUsers() const {
    return users;
}