#ifndef ADDCARWINDOW_H
#define ADDCARWINDOW_H

#include <QWidget>
#include "usermanager.h"
#include "carmanager.h"
#include "rentalmanager.h"


namespace Ui {
class AddCarWindow;
}

class AddCarWindow : public QWidget
{
    Q_OBJECT

public:
    explicit AddCarWindow(UserManager* usermanager, CarManager* carmanager, RentalManager* rentalmanager, QWidget* adminWindow = nullptr, Car* car = {}, QWidget *parent = nullptr);
    ~AddCarWindow();

private slots:
    void on_btnCancel_clicked();

    void on_btnAdd_clicked();

private:
    Ui::AddCarWindow *ui;
    UserManager* usermanager;
    CarManager* carmanager;
    RentalManager* rentalmanager;
    QWidget* adminWindow;
    Car* car;
};

#endif // ADDCARWINDOW_H
