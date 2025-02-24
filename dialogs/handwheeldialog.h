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
public slots:
   void HandWheelButtonClickHandel();//手轮按键按下后触发处理函数
   void RefreshHandWheel_handle();//刷新手轮界面参数处理函数
private:
    Ui::HandWheelDialog *ui;

private:
    void initWidgets();

private:
    QVector<QCheckBox*> handWheelScale;         //手轮刻度
    QVector<QCheckBox*> handWheelAxis;          //手轮轴
signals:
    void HandWheelModeChange_Signal(bool state);//手轮模式状态改变信号
protected:
    void showEvent(QShowEvent *event);
};

#endif // HANDWHEELDIALOG_H
