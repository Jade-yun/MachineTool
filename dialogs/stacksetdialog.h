#ifndef STACKSETDIALOG_H
#define STACKSETDIALOG_H

#include <QDialog>
#include "basewindow.h"
namespace Ui {
class StackSetDialog;
}

class StackSetDialog : public BaseWindow
{
    Q_OBJECT
protected:
    void showEvent(QShowEvent *event) override;//重写显示事件
public:
    explicit StackSetDialog(QWidget *parent = nullptr);
    ~StackSetDialog();

private slots:
    void on_btnOK_clicked();

    void on_btnCancel_clicked();

private:
    void changeEnable();


private:
    Ui::StackSetDialog *ui;
    QString editXName;
    QString editYName;
    QString editZName;
    QString chboxName;
};

#endif // STACKSETDIALOG_H
