#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include "basewindow.h"
namespace Ui {
class LoginDialog;
}

class LoginDialog : public BaseWindow
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

private:
    Ui::LoginDialog *ui;
};

#endif // LOGINDIALOG_H
