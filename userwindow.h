#ifndef USERWINDOW_H
#define USERWINDOW_H

#include <QWidget>
#include <QListWidget>
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
    void on_btnChangePassword_clicked();

    void on_carsListWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_comboCategory_currentIndexChanged(int index);

    void on_spinMinSeats_valueChanged(int arg1);

    void on_spinMaxPrice_valueChanged(double arg1);

    void on_btnSortMerge_clicked();

    void on_btnMinMaxMileage_clicked();

    void on_btnGroupAndSort_clicked();

    void on_spinDays_valueChanged(int arg1);

    void renderCatalog();

    void on_myRentalsTable_itemSelectionChanged();

    void on_btnCancelRental_clicked();


    void on_btnReturnCar_clicked();

private:
    void updateProfileStats();
    enum class CatalogMode {
        SortPrice,
        MinMaxMileage,
        GroupClassYear
    };

    Ui::UserWindow* ui;
    UserManager* usermanager;
    CarManager* carmanager;
    RentalManager* rentalmanager;
    QWidget* loginWindow;
    bool isAscending = true;
    bool currentAscending = true;
    CatalogMode mode = CatalogMode::SortPrice;
    QList<Car> applyFilters();
    QList<Car> selectMinMaxMileage(const QList<Car>& cars) const;
    QList<Car> groupByClassYear(QList<Car> cars) const;
    QWidget* createCarCardWidget(const Car& car) const;
    void loadCarsList(QListWidget* ui, const QList<Car>& result, bool isAvailable = true) const;
};

#endif // USERWINDOW_H
