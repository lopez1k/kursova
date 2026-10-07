#ifndef USERMANAGER_H
#define USERMANAGER_H
#include "user.h"
#include "QList"

class UserManager {
private:
    QList<User> users;
    User* currentUser;
public:
    UserManager();

    void addUser(const User& _user);
    bool login(const QString& _email, const QString& _password);
    void logout();

    User* getCurrentUser() const;
    User* findUserByEmail(const QString& email);

    const QList<User>& getAllUsers() const;

    void loadUsersFromFile(const QString& filePath);
    void saveUsersToFile(const QString& filePath) const;
};

#endif // USERMANAGER_H
