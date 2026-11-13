#ifndef ADDCARWINDOW_H
#define ADDCARWINDOW_H

#include <QWidget>
#include "rentalmanager.h"


namespace Ui {
class AddCarWindow;
}

class AddCarWindow : public QWidget
{
    Q_OBJECT

public:
    explicit AddCarWindow(UserManager* usermanager, CarManager* carmanager, RentalManager* rentalmanager, QWidget* adminWindow = nullptr, const QString& carId = "", QWidget *parent = nullptr);
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
    QString carId;
};

#endif // ADDCARWINDOW_H
