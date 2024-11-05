#include "backlighter.h"
#include <fcntl.h>
#include <unistd.h>
#include <QDebug>

BackLighter::BackLighter(QObject* parent)
    : QObject(parent),
      fd(-1), brightness(0), isScreenOn(false)
{
    offTimer = new QTimer;
    offTimer->setInterval(60*1000);

    fd = open("/sys/class/backlight/backlight/brightness", O_WRONLY);
    if (fd == -1) {
        qDebug() << "Failed to open the brightness file.";
    }


    int readFd = open("/sys/class/backlight/backlight/brightness", O_RDONLY);

    std::string data(4, '\0');
    ssize_t bytesRead = read(readFd, &data[0], data.size());
    if (bytesRead > 0) {
        brightness = std::stoi(data); // 转换
        qDebug() << "current brightness:" << brightness;
    }

    close(readFd);

    brightness = brightness <= 0 ? 150 : brightness;


    connect(offTimer, &QTimer::timeout, this, &BackLighter::turnOffScreen);
}

BackLighter::~BackLighter()
{
    delete offTimer;
    if (fd > 0)
    {
        close(fd);
    }
}

BackLighter* BackLighter::instance()
{
    static BackLighter instance;
    return &instance;
}

void BackLighter::setBrightness(int level)
{
    brightness = level * 2 + 50;
    QByteArray brightnessValue = QByteArray::number(brightness);
    write(fd, brightnessValue.data(), brightnessValue.size());
}

int BackLighter::getBrightness() const
{
    int level = (brightness - 50) * 100 / 200;
    return level;
}

void BackLighter::setScreenOffTime(int durationSeconds) {
    // 限制屏幕关闭时间的范围为30到65535秒
    if (durationSeconds < 30) {
        durationSeconds = 30;
    }
    else if (durationSeconds > 65535) {
        durationSeconds = 65535;
    }

    offTimer->setInterval(durationSeconds * 1000); // 转换为毫秒
    offTimer->start();
}

int BackLighter::getScreenOffTime() const
{
    int seconds = offTimer->interval() / 1000;
    return seconds;
}

void BackLighter::turnOffScreen()
{
    // save the brightness before close screen so that to use it in next lighton
    isScreenOn = false;
    write(fd, "0", 1);
//    qDebug() << "turnOffScreen";
    offTimer->stop();
}

bool BackLighter::lightOn()
{
    offTimer->start();

    if (isScreenOn) {
        return false; // 如果屏幕已经亮起，直接返回 false
    }

    QByteArray brightnessValue = QByteArray::number(brightness);
    write(fd, brightnessValue.data(), brightnessValue.size());
    isScreenOn = true;

    return true; // 点亮屏幕后返回 true
}
