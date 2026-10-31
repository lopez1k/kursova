#ifndef ADDUSERWINDOW_H
#define ADDUSERWINDOW_H

#include <QWidget>
#include "usermanager.h"

namespace Ui {
class AddUserWindow;
}

class AddUserWindow : public QWidget
{
    Q_OBJECT

public:
    explicit AddUserWindow(UserManager* usermanager, QWidget* adminWindow = nullptr, QWidget *parent = nullptr);
    ~AddUserWindow();

private slots:
    void on_btnCancel_clicked();
    void on_btnAdd_clicked();

private:
    Ui::AddUserWindow *ui;
    UserManager* usermanager;
    QWidget* adminWindow;
};

#endif // ADDUSERWINDOW_H
