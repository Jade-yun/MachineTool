#ifndef HANDWHEELDIALOG_H
#define HANDWHEELDIALOG_H

#include <QDialog>
#include "basewindow.h"
#include <QCheckBox>
namespace Ui {
class HandWheelDialog;
}

class HandWheelDialog : public BaseWindow
{
    Q_OBJECT

public:
    explicit HandWheelDialog(QWidget *parent = nullptr);
    ~HandWheelDialog();

private:
    Ui::HandWheelDialog *ui;

private:
    void initControls();
    void initCheckBoxGrop();

private:
    QVector<QCheckBox*> handWheelScale;         //手轮刻度
    QVector<QCheckBox*> handWheelAxis;          //手轮轴
};

#endif // HANDWHEELDIALOG_H
