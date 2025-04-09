#ifndef IOREAD_H
#define IOREAD_H

#include <QObject>
#include <QFile>
#include <QtDebug>
class IoRead : public QObject
{
    Q_OBJECT
public:
    explicit IoRead(QObject *parent = nullptr);
    ~IoRead();
    void start();
    void stop();
private:
    QFile valueFile106, valueFile107, valueFile108; // 声明为成员变量
    uint8_t OldIoVal106,OldIoVal107,OldIoVal109;
    QThread* m_thread;
    bool m_isRunning;
private:
    void Three_way_switch_key_IoInit();
    void Three_way_switch_key_Check();
    void initGPIO(int gpioNumber);
signals:
    void IoTrimodeSwitch(int IoIndex, int value);
};

#endif // IOREAD_H
