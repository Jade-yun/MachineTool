// logindialog.h
#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

enum class LoginMode : unsigned char {
    Operator = 0,
    Admin,
    Advance
};

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

    static LoginMode getLoginMode();

signals:
    void loginModeChanged(LoginMode mode);
private slots:
    void onBtnOk();
    bool verifyPassword(uint32_t inputPasswd, uint32_t correctPasswd);

private:
    Ui::LoginDialog *ui;
    static LoginMode mode;

protected:
    void showEvent(QShowEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void changeEvent(QEvent* e) override;
};

#endif // LOGINDIALOG_H
