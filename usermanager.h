#ifndef USERMANAGER_H
#define USERMANAGER_H
#include "user.h"
#include "QList"
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QMessageBox>

class UserManager {
private:
    QList<User> users;
    QString currentUserId;
    QString filename;
    int nextId = 1;
    QString generateId();
public:
    UserManager(QString _filename = "users.json");
    ~UserManager();

    void saveToJson() const;
    void loadFromJson();
    void addUser(const User& _user);
    void addUser(const QString& _name, const QString& _email, const QString& _password, Role _role);
    bool login(const QString& _email, const QString& _password);
    void logout();

    User* getCurrentUser();
    User* findUserByEmail(const QString& email);
    User* findUserById(const QString& id);

    const QList<User>& getAllUsers() const;

    void loadUsersFromFile(const QString& filePath);
    void saveUsersToFile(const QString& filePath) const;
};

#endif // USERMANAGER_H
