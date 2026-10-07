#ifndef USERWINDOW_H
#define USERWINDOW_H

#include <QWidget>
#include "usermanager.h"

namespace Ui {
class UserWindow;
}

class UserWindow : public QWidget
{
    Q_OBJECT

public:
    explicit UserWindow(UserManager* manager, QWidget* loginWindow = nullptr, QWidget *parent = nullptr);
    ~UserWindow();

private slots:
    void on_btnCatalog_clicked();
    void on_btnMyRentals_clicked();
    void on_btnProfile_clicked();
    void on_btnLogout_clicked();

private:
    Ui::UserWindow* ui;
    UserManager* manager;
    QWidget* loginWindow;
};

#endif // USERWINDOW_H
