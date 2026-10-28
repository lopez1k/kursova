#include "rentdialog.h"

RentDialog::RentDialog(const Car& car, QWidget *parent)
    : QDialog(parent), m_pricePerDay(car.getRentPrice())
{
    setWindowTitle("Оформлення бронювання");
    setMinimumWidth(350);
    setObjectName("RentDialog");
    setStyleSheet(R"(
QWidget { font-family: "Segoe UI"; font-size: 10pt; color: #1F2933; }
QDialog#RentDialog { background: #FFFFFF; }
QLabel { color: #5B6573; background: transparent; }
QLabel#lblCarInfo { color: #1F2933; font-size: 11pt; padding-bottom: 6px; }
QLabel#lblTotalPrice { color: #2E6B2E; font-size: 11pt; font-weight: 600; padding-top: 6px; }

QDateEdit {
    background: #FFFFFF; border: 1px solid #C9D0D9; border-radius: 4px;
    padding: 5px 26px 5px 8px; min-height: 22px; selection-background-color: #4A6FA5;
}
QDateEdit:hover { border-color: #A9B8CC; }
QDateEdit:focus { border-color: #4A6FA5; }
QDateEdit::drop-down {
    subcontrol-origin: padding; subcontrol-position: center right;
    width: 24px; border: none; background: transparent;
}
QDateEdit::down-arrow {
    image: none; width: 0px; height: 0px;
    border-left: 4px solid transparent; border-right: 4px solid transparent;
    border-top: 5px solid #5B6573;
}

QCalendarWidget QWidget { background: #FFFFFF; color: #1F2933; }
QCalendarWidget QWidget#qt_calendar_navigationbar { background: #F4F6F8; border-bottom: 1px solid #DDE2E8; }
QCalendarWidget QToolButton { background: transparent; border: none; padding: 4px 8px; font-weight: 600; }
QCalendarWidget QToolButton:hover { background: #EAEEF3; border-radius: 4px; }
QCalendarWidget QAbstractItemView {
    outline: 0; selection-background-color: #4A6FA5; selection-color: #FFFFFF;
}

QPushButton {
    background: #F4F6F8; border: 1px solid #DDE2E8; border-radius: 4px;
    padding: 8px 18px; min-height: 20px;
}
QPushButton:hover   { background: #EAEEF3; border-color: #C9D0D9; }
QPushButton:pressed { background: #DFE5EC; }
QPushButton#btnConfirm { background: #4A6FA5; border-color: #4A6FA5; color: #FFFFFF; font-weight: 600; }
QPushButton#btnConfirm:hover   { background: #3F5F8F; border-color: #3F5F8F; }
QPushButton#btnConfirm:pressed { background: #34507A; }
)");

    auto *mainLayout = new QVBoxLayout(this);

    // Інформація про авто
    auto *lblCarInfo = new QLabel(QString("<b>Автомобіль:</b> %1 %2<br><b>Ціна:</b> %3 грн/день")
                                      .arg(car.getBrand())
                                      .arg(car.getModel())
                                      .arg(car.getRentPrice(), 0, 'f', 2));
    lblCarInfo->setObjectName("lblCarInfo");
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
    m_lblTotalPrice->setObjectName("lblTotalPrice");
    mainLayout->addWidget(m_lblTotalPrice);

    // Кнопки Підтвердити / Скасувати
    auto *btnLayout = new QHBoxLayout();
    auto *btnConfirm = new QPushButton("Підтвердити", this);
    auto *btnCancel = new QPushButton("Скасувати", this);
    btnConfirm->setObjectName("btnConfirm");
    btnConfirm->setDefault(true);

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