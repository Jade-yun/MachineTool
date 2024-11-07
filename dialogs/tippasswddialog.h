#ifndef TIPPASSWDDIALOG_H
#define TIPPASSWDDIALOG_H

#include <QDialog>
#include "basewindow.h"

namespace Ui {
class TipPasswdDialog;
}

class TipPasswdDialog : public BaseWindow
{
    Q_OBJECT

public:
    explicit TipPasswdDialog(const QString message, QWidget *parent = nullptr);
    ~TipPasswdDialog();

    uint getPasswd() const;

private:
    Ui::TipPasswdDialog *ui;
};

#endif // TIPPASSWDDIALOG_H
