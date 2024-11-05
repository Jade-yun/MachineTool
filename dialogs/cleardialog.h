#ifndef CLEARDIALOG_H
#define CLEARDIALOG_H

#include <QDialog>
#include "basewindow.h"
namespace Ui {
class ClearDialog;
}

class ClearDialog : public BaseWindow
{
    Q_OBJECT

public:
    explicit ClearDialog(QWidget *parent = nullptr);
    ~ClearDialog();

private:
    Ui::ClearDialog *ui;
};

#endif // CLEARDIALOG_H
