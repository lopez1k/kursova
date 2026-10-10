#ifndef USERWINDOW_H
#define USERWINDOW_H

#include <QWidget>
#include "usermanager.h"
#include "carmanager.h"
#include "rentalmanager.h"

namespace Ui {
class UserWindow;
}

class UserWindow : public QWidget
{
    Q_OBJECT

public:
    explicit UserWindow(UserManager* usermanager, CarManager* carmanager, RentalManager* rentalmanager, QWidget* loginWindow = nullptr, QWidget *parent = nullptr);
    ~UserWindow();

private slots:
    void on_btnCatalog_clicked();
    void on_btnMyRentals_clicked();
    void on_btnProfile_clicked();
    void on_btnLogout_clicked();

    void on_carsListWidget_itemDoubleClicked(QListWidgetItem *item);

private:
    Ui::UserWindow* ui;
    UserManager* usermanager;
    CarManager* carmanager;
    RentalManager* rentalmanager;
    QWidget* loginWindow;
};

#endif // USERWINDOW_H
