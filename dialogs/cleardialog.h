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
    enum ClearMode {
        AutoState = 0,
        VarState,
        StackState,
        Step
    };

public:
    explicit ClearDialog(QWidget *parent = nullptr);
    explicit ClearDialog(ClearMode mode, QWidget *parent = nullptr);
    ~ClearDialog();

    void setMode(ClearMode mode);

private:
    Ui::ClearDialog *ui;
};

#endif // CLEARDIALOG_H
