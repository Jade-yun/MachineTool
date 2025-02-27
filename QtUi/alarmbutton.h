#ifndef ALARMBUTTON_H
#define ALARMBUTTON_H

#include <QPushButton>
#include <QTimer>
#include <QLabel>
#include <QDialog>

class AlarmButton : public QPushButton {
    Q_OBJECT

public:
    explicit AlarmButton(QWidget *parent = nullptr);

    void startAlarm();  // 启动报警（红色图标闪烁）
    void stopAlarm();   // 停止报警

private slots:
    void toggleAlarmIcon();

private:
    QLabel *iconLabel;   // 用于显示图标的 QLabel
    QTimer *blinkTimer;  // 控制闪烁的定时器
    bool alarmActive;    // 报警状态
    bool currentIconVisible; // 当前图标是否可见

    QPixmap alarmPixmap;
    QPixmap blankPixmap;
};

#endif // ALARMBUTTON_H
