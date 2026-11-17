#include "rentdialog.h"
#include "ui_rentdialog.h"

RentDialog::RentDialog(const Car& car, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RentDialog)
    , m_pricePerDay(car.getRentPrice())
{
    ui->setupUi(this);

    // Інформація про авто
    ui->lblCarInfo->setText(QString("<b>Автомобіль:</b> %1 %2<br><b>Ціна:</b> %3 грн/день")
                                .arg(car.getBrand())
                                .arg(car.getModel())
                                .arg(car.getRentPrice(), 0, 'f', 2));

    // Дати залежать від поточного дня, тому задаються в коді, а не в .ui
    ui->startDateEdit->setMinimumDate(QDate::currentDate());
    ui->startDateEdit->setDate(QDate::currentDate());
    ui->endDateEdit->setMinimumDate(QDate::currentDate().addDays(1));
    ui->endDateEdit->setDate(QDate::currentDate().addDays(1));

    connect(ui->startDateEdit, &QDateEdit::dateChanged, this, &RentDialog::updateTotalPrice);
    connect(ui->endDateEdit, &QDateEdit::dateChanged, this, &RentDialog::updateTotalPrice);

    updateTotalPrice(); // Первинний розрахунок
}

RentDialog::~RentDialog()
{
    delete ui;
}

QDate RentDialog::getStartDate() const {
    return ui->startDateEdit->date();
}

QDate RentDialog::getEndDate() const {
    return ui->endDateEdit->date();
}

void RentDialog::updateTotalPrice() {
    QDate start = ui->startDateEdit->date();
    QDate end = ui->endDateEdit->date();

    // Перевірка коректності дат
    if (end <= start) {
        ui->endDateEdit->setDate(start.addDays(1));
        end = ui->endDateEdit->date();
    }

    qint64 days = start.daysTo(end);
    m_totalPrice = days * m_pricePerDay;

    ui->lblTotalPrice->setText(QString("Днів: %1 | Разом до сплати: %2 грн")
                                   .arg(days)
                                   .arg(m_totalPrice, 0, 'f', 2));
}

void RentDialog::on_btnConfirm_clicked()
{
    accept();
}

void RentDialog::on_btnCancel_clicked()
{
    reject();
}