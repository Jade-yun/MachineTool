#include "alarmbutton.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

// === 报警按钮实现 ===
AlarmButton::AlarmButton(QWidget *parent) : QPushButton(parent),
    alarmActive(false), currentIconVisible(true) {

    alarmPixmap = QPixmap(":/images/alarm_red.png").scaled(70, 70, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // 创建 QLabel 作为图标显示
    iconLabel = new QLabel(this);
    iconLabel->setFixedSize(70, 70); // 设置图标大小
    iconLabel->setScaledContents(true);
    iconLabel->setPixmap(alarmPixmap); // 默认黄色图标

    // 使 QLabel 居中
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(iconLabel, 0, Qt::AlignCenter);
    layout->setContentsMargins(5, 0, 5, 5);
    setLayout(layout);

    // 闪烁定时器
    blinkTimer = new QTimer(this);
    blinkTimer->setInterval(500);
    connect(blinkTimer, &QTimer::timeout, this, &AlarmButton::toggleAlarmIcon);

}

// 启动报警（红色图标开始闪烁）
void AlarmButton::startAlarm() {
    if (!blinkTimer->isActive()) {
        alarmActive = true;
        iconLabel->setPixmap(alarmPixmap);
        blinkTimer->start();

        this->setAttribute(Qt::WA_TransparentForMouseEvents, false);
    }
}

// 停止报警
void AlarmButton::stopAlarm() {
    if (blinkTimer->isActive()) {
        blinkTimer->stop();
        alarmActive = false;
        iconLabel->setPixmap(alarmPixmap);

        this->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    }
}

void AlarmButton::toggleAlarmIcon() {
    if (alarmActive) {
        if (currentIconVisible) {
            iconLabel->setPixmap(QPixmap());
        } else {
            iconLabel->setPixmap(alarmPixmap); // 显示红色图标
        }
        currentIconVisible = !currentIconVisible;
    }
}
