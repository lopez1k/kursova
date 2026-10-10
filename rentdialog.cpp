#include "rentdialog.h"

RentDialog::RentDialog(const Car& car, QWidget *parent)
    : QDialog(parent), m_pricePerDay(car.getRentPrice())
{
    setWindowTitle("Оформлення бронювання");
    setMinimumWidth(350);

    auto *mainLayout = new QVBoxLayout(this);

    // Інформація про авто
    auto *lblCarInfo = new QLabel(QString("<b>Автомобіль:</b> %1 %2<br><b>Ціна:</b> %3 грн/день")
                                      .arg(car.getBrand())
                                      .arg(car.getModel())
                                      .arg(car.getRentPrice(), 0, 'f', 2));
    mainLayout->addWidget(lblCarInfo);

    // Вибір дати початку
    mainLayout->addWidget(new QLabel("Дата початку оренди:"));
    m_startDateEdit = new QDateEdit(QDate::currentDate(), this);
    m_startDateEdit->setCalendarPopup(true); // Показувати календар
    m_startDateEdit->setMinimumDate(QDate::currentDate());
    mainLayout->addWidget(m_startDateEdit);

    // Вибір дати завершення
    mainLayout->addWidget(new QLabel("Дата кінця оренди:"));
    m_endDateEdit = new QDateEdit(QDate::currentDate().addDays(1), this);
    m_endDateEdit->setCalendarPopup(true);
    m_endDateEdit->setMinimumDate(QDate::currentDate().addDays(1));
    mainLayout->addWidget(m_endDateEdit);

    // Підсумкова вартість
    m_lblTotalPrice = new QLabel(this);
    m_lblTotalPrice->setStyleSheet("font-size: 14px; font-weight: bold; color: #27ae60;");
    mainLayout->addWidget(m_lblTotalPrice);

    // Кнопки Підтвердити / Скасувати
    auto *btnLayout = new QHBoxLayout();
    auto *btnConfirm = new QPushButton("Підтвердити", this);
    auto *btnCancel = new QPushButton("Скасувати", this);

    btnLayout->addWidget(btnCancel);
    btnLayout->addWidget(btnConfirm);
    mainLayout->addLayout(btnLayout);

    // Сигнали
    connect(m_startDateEdit, &QDateEdit::dateChanged, this, &RentDialog::updateTotalPrice);
    connect(m_endDateEdit, &QDateEdit::dateChanged, this, &RentDialog::updateTotalPrice);
    connect(btnConfirm, &QPushButton::clicked, this, &QDialog::accept);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);

    updateTotalPrice(); // Первинний розрахунок
}

void RentDialog::updateTotalPrice() {
    QDate start = m_startDateEdit->date();
    QDate end = m_endDateEdit->date();

    // Перевірка коректності дат
    if (end <= start) {
        m_endDateEdit->setDate(start.addDays(1));
        end = m_endDateEdit->date();
    }

    qint64 days = start.daysTo(end);
    m_totalPrice = days * m_pricePerDay;

    m_lblTotalPrice->setText(QString("Днів: %1 | Разом до сплати: %2 грн")
                                 .arg(days)
                                 .arg(m_totalPrice, 0, 'f', 2));
}