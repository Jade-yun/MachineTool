#ifndef SOFTKEYWIDGET_H
#define SOFTKEYWIDGET_H

#include <QDialog>

namespace Ui {
class SoftKeyWidget;
}

class SoftKeyWidget : public QDialog
{
    Q_OBJECT

public:
    explicit SoftKeyWidget(QWidget *parent = nullptr);
    ~SoftKeyWidget();
private:
    bool moveEnable;
    QPoint mousePoint;
signals:
    void SoftKeyWidgetButtonClickSignal(uint16_t code, int32_t value);//软键盘按键按下后，触发相应的物理按键处理事件

private:
    Ui::SoftKeyWidget *ui;

protected:
    bool eventFilter(QObject *watched, QEvent *event);
};

#endif // SOFTKEYWIDGET_H
