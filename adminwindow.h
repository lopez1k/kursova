#ifndef ADMINWINDOW_H
#define ADMINWINDOW_H

#include <QWidget>
#include "usermanager.h"

namespace Ui {
class AdminWindow;
}

class AdminWindow : public QWidget
{
    Q_OBJECT

public:
    explicit AdminWindow(UserManager* manager, QWidget* loginWindow = nullptr, QWidget *parent = nullptr);
    ~AdminWindow();


private slots:
    void on_btnCars_clicked();
    void on_btnUsers_clicked();
    void on_btnRentals_clicked();
    void on_btnLogout_clicked();


private:
    Ui::AdminWindow *ui;
    UserManager* manager;
    void loadUsersTable() const;
    QWidget* loginWindow;
};


#endif // ADMINWINDOW_H
