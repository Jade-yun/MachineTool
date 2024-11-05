#ifndef CLEARVARDIALOG_H
#define CLEARVARDIALOG_H

#include <QDialog>
#include "basewindow.h"
namespace Ui {
class ClearVarDialog;
}

class ClearVarDialog : public BaseWindow
{
    Q_OBJECT

public:
    explicit ClearVarDialog(QWidget *parent = nullptr);
    ~ClearVarDialog();

private:
    Ui::ClearVarDialog *ui;
};

#endif // CLEARVARDIALOG_H
