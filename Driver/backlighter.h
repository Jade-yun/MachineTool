#ifndef BACKLIGHTER_H
#define BACKLIGHTER_H

#include <QObject>
#include <QTimer>

class BackLighter : public QObject {
    Q_OBJECT

public:
    static BackLighter* instance();

    void setBrightness(int level); // 设置屏幕亮度等级 0~100%
    int getBrightness() const;
    void setScreenOffTime(int durationSeconds); // 设置屏幕息屏时间（秒）
    int getScreenOffTime() const;
    bool lightOn();

private slots:
    void turnOffScreen();

private:
    BackLighter(QObject* parent = nullptr);
    ~BackLighter();
    BackLighter(const BackLighter&) = delete;
    BackLighter& operator=(const BackLighter&) = delete;

    int fd;
    int brightness;
    bool isScreenOn;
    QTimer *offTimer = nullptr;
    QString devName;
};

#endif // BACKLIGHTER_H
