#ifndef CLEARDIALOG_H
#define CLEARDIALOG_H

#include <QDialog>
#include <QCheckBox>
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
    void UpdateShow(ClearDialog::ClearMode mode);
private:
    Ui::ClearDialog *ui;
    QVector<QCheckBox*> StackClearGroup;//堆叠组清零选择
    QVector<QCheckBox*> VarAutoClear;//变量自动启动时清零
    QVector<QCheckBox*> VarPowerOffMemory;//变量断电记忆
protected:
    void changeEvent(QEvent* e) override;
};

#endif // CLEARDIALOG_H
