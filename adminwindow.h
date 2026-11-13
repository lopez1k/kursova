#ifndef ADMINWINDOW_H
#define ADMINWINDOW_H

#include <QWidget>
#include <QListWidget>
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
    void on_btnProfile_clicked();
    void on_btnChangePassword_clicked();


    void on_categoryFilter_currentIndexChanged(int index);

    void on_statusFilter_currentIndexChanged(int index);


    void on_btnAddCar_clicked();

    void on_carsListWidget_itemClicked(QListWidgetItem *item);

    void on_btnEditCar_clicked();

    void on_btnDeleteCar_clicked();

    void on_btnAddUser_clicked();
    void on_btnDeleteUser_clicked();
    void on_usersTable_itemSelectionChanged();

    void on_actionExportUsers_triggered();
    void on_actionExportCars_triggered();
    void on_actionExportRentals_triggered();

private:
    void updateProfileStats();
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
