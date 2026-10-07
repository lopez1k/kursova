#ifndef USER_H
#define USER_H
#include "QString"
#include "QTextStream"
#include <iostream>
#include "userexception.h"
#include "passwordhasher.h"

enum class Role {
    Customer,
    Administrator
};

class User {
private:
    QString id;
    QString name;
    QString email;
    QString password;
    Role role;

public:
    User() = default;
    User(const QString& _id, const QString& _name, const QString& _email, const QString& _password, Role _role = Role::Customer);
    Role getRole() const;
    QString getRoleString() const;
    bool isAdmin() const;
    QString getId() const;
    QString getName() const;
    QString getEmail() const;
    QString getPassword() const;
    void setName(const QString& _name);
    void setEmail(const QString& _email);
    void setPassword(const QString& _password);

    friend QTextStream& operator<<(QTextStream& out, const User& user);
    friend QTextStream& operator>>(QTextStream& in, User& user);

};

#endif // USER_H
