#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "QMessageBox"
#include "adminwindow.h"
#include "userwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , usermanager(new UserManager())
    , carmanager(new CarManager())
    , rentalmanager(new RentalManager(*usermanager, *carmanager))
{
    ui->setupUi(this);
}


MainWindow::~MainWindow()
{
    delete rentalmanager;
    delete usermanager;
    delete carmanager;
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QString _email = ui->emailInput->text();
    QString _password = ui->passwordInput->text();

    try {
        if (usermanager->login(_email, _password)) {
            if (usermanager->getCurrentUser()->isAdmin()) {
                AdminWindow* adminui = new AdminWindow(this->usermanager, this->carmanager, this->rentalmanager, this);
                adminui->setAttribute(Qt::WA_DeleteOnClose);
                adminui->show();
                this->hide();
            }
            else {
                UserWindow*  userui  = new UserWindow(this->usermanager, this->carmanager, this->rentalmanager, this);
                userui->setAttribute(Qt::WA_DeleteOnClose);
                userui->show();
                this->hide();
            }
        }

    }
    catch (const UserNotFoundException& ex) {
        QMessageBox::warning(this, "Помилка входу", "Користувача з такою поштою не знайдено.");
    }
    catch (const InvalidPasswordException& ex) {
        QMessageBox::warning(this, "Помилка входу", "Введено невірний пароль.");
    }
    catch (const UserException& ex) {
        QMessageBox::critical(this, "Помилка", ex.what());
    }
}

