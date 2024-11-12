#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT
public:
    enum LoginMode {
        Operator = 0,
        Admin,
        Advance
    };

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

    int getLoginMode() const;
    uint getInputPasswd() const;

private slots:
    void onBtnOk();

private:
    Ui::LoginDialog *ui;

private:
    LoginMode mode;
    unsigned int passwd;

    // QWidget interface
protected:
    void showEvent(QShowEvent *event);
    void paintEvent(QPaintEvent *event);
};

#endif // LOGINDIALOG_H
