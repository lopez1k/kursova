#ifndef ADMINWINDOW_H
#define ADMINWINDOW_H

#include <QWidget>
#include "usermanager.h"
#include "carmanager.h"
#include "rentalmanager.h"

namespace Ui {
class AdminWindow;
}

class AdminWindow : public QWidget
{
    Q_OBJECT

public:
    explicit AdminWindow(UserManager* usermanager, CarManager* carmanager, RentalManager* rentalmanager, QWidget* loginWindow = nullptr, QWidget *parent = nullptr);
    ~AdminWindow();


private slots:
    void on_btnCars_clicked();
    void on_btnUsers_clicked();
    void on_btnRentals_clicked();
    void on_btnLogout_clicked();


    void on_categoryFilter_currentIndexChanged(int index);

    void on_statusFilter_currentIndexChanged(int index);

    void on_carsListWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_btnAddCar_clicked();

    void on_carsListWidget_itemClicked(QListWidgetItem *item);

    void on_btnEditCar_clicked();

    void on_btnDeleteCar_clicked();

private:
    Ui::AdminWindow *ui;
    UserManager* usermanager;
    CarManager* carmanager;
    RentalManager* rentalmanager;
    void loadUsersTable() const;
    QWidget* loginWindow;
    QWidget* createCarCardWidget(const Car& car) const;
    void applyFilters();
    void loadCarsList(QListWidget* ui, const QList<Car>& result, bool isAvailable = true) const;
};


#endif // ADMINWINDOW_H
