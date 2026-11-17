#ifndef RENTDIALOG_H
#define RENTDIALOG_H

#include <QDialog>
#include <QDate>
#include "car.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class RentDialog;
}
QT_END_NAMESPACE

class RentDialog : public QDialog {
    Q_OBJECT

public:
    explicit RentDialog(const Car& car, QWidget *parent = nullptr);
    ~RentDialog();

    QDate getStartDate() const;
    QDate getEndDate() const;
    double getTotalPrice() const { return m_totalPrice; }

private slots:
    void updateTotalPrice();
    void on_btnConfirm_clicked();
    void on_btnCancel_clicked();

private:
    Ui::RentDialog *ui;
    double m_pricePerDay;
    double m_totalPrice = 0.0;
};

#endif // RENTDIALOG_H