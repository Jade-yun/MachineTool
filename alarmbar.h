#ifndef ALARMBAR_H
#define ALARMBAR_H

#include <QWidget>

namespace Ui {
class AlarmBar;
}

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QPropertyAnimation>

class AlarmBar : public QDialog
{
    Q_OBJECT

public:
    explicit AlarmBar(QWidget* parent = nullptr);
    ~AlarmBar();

    void showAlarm(int alarmNum, const QString& alarmContent); // 显示报警信息
    void hideAlarm();                                       // 隐藏报警条

protected:
    void mousePressEvent(QMouseEvent* event) override;      // 鼠标按下事件
    void mouseMoveEvent(QMouseEvent* event) override;       // 鼠标移动事件
    void mouseReleaseEvent(QMouseEvent* event) override;    // 鼠标释放事件

private:
    QPropertyAnimation* animation;                         // 动画对象
    int barHeight;                                         // 报警条高度

    QPoint dragStartPos;                                   // 鼠标拖动起始位置
    bool isDragging;                                       // 是否正在拖动

private:
    Ui::AlarmBar *ui;
};

#endif // ALARMBAR_H
