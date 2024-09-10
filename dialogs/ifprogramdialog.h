#ifndef IFPROGRAMDIALOG_H
#define IFPROGRAMDIALOG_H

#include <QDialog>
#include <QButtonGroup>
#include "customkeyboard.h"

namespace Ui {
class IfProgramDialog;
}

class IfProgramDialog : public QDialog
{
    Q_OBJECT

public:
    explicit IfProgramDialog(QWidget *parent = nullptr);
    ~IfProgramDialog();

private slots:
    void on_chboxConstantVar_clicked();

    void on_chboxVariableVar_clicked();

    void on_chboxRealProd_clicked();

    void on_chboxConstantRealProd_clicked();

    void on_chboxVariableRealProd_clicked();

    void on_chboxConstantStack_clicked();

    void on_chboxStack_clicked();

    void on_chboxConstantTimer_clicked();

    void on_chboxTimerTimer_clicked();

    void on_chboxConstantPos_clicked();

    void on_chboxVariablePos_clicked();

    void on_chboxAxis_clicked();

public:
    QString getMainBoardSig() const;
    QString getVarOperateExpression() const;
    QString getPosExpression() const;
    QString getTimerExpression() const;

    QString getConditionExpression() const;

private:
    Ui::IfProgramDialog *ui;

private:
    QButtonGroup* chboxsSigMb;
};


#endif // IFPROGRAMDIALOG_H
