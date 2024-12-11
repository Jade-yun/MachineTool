#ifndef HANDWHEELDIALOG_H
#define HANDWHEELDIALOG_H

#include <QDialog>
#include <QCheckBox>
namespace Ui {
class HandWheelDialog;
}

class HandWheelDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HandWheelDialog(QWidget *parent = nullptr);
    ~HandWheelDialog();

private:
    Ui::HandWheelDialog *ui;

private:
    void initWidgets();

private:
    QVector<QCheckBox*> handWheelScale;         //手轮刻度
    QVector<QCheckBox*> handWheelAxis;          //手轮轴

protected:
    void showEvent(QShowEvent *event);
};

#endif // HANDWHEELDIALOG_H
