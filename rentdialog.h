#ifndef RENTDIALOG_H
#define RENTDIALOG_H

#include <QDialog>
#include <QDateEdit>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDate>
#include "car.h"

class RentDialog : public QDialog {
    Q_OBJECT

public:
    explicit RentDialog(const Car& car, QWidget *parent = nullptr);

    QDate getStartDate() const { return m_startDateEdit->date(); }
    QDate getEndDate() const { return m_endDateEdit->date(); }
    double getTotalPrice() const { return m_totalPrice; }

private slots:
    void updateTotalPrice();

private:
    double m_pricePerDay;
    double m_totalPrice;

    QDateEdit *m_startDateEdit;
    QDateEdit *m_endDateEdit;
    QLabel *m_lblTotalPrice;
};

#endif // RENTDIALOG_H