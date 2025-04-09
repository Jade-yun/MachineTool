#include "ioread.h"
#include <QThread>
IoRead::IoRead(QObject *parent) : QObject(parent)
{

}
IoRead::~IoRead()
{
    stop();
}

void IoRead::start()
{
    if (!m_isRunning)
    {
        m_isRunning = true;
        Three_way_switch_key_IoInit();
        m_thread = new QThread(this);
        connect(m_thread, &QThread::started, this, &IoRead::Three_way_switch_key_Check);
        moveToThread(m_thread);
        m_thread->start();
    }
}

void IoRead::stop()
{
    if (m_isRunning)
    {
        m_isRunning = false;
        m_thread->quit();
        m_thread->wait();
        delete m_thread;
    }
}
//三档开关按键初始化  106-手动档IO 107-停止档  109-自动档
void IoRead::Three_way_switch_key_IoInit()
{
    initGPIO(106);
    initGPIO(107);
    initGPIO(109);
    valueFile106.setFileName("/sys/class/gpio/gpio106/value");
    valueFile107.setFileName("/sys/class/gpio/gpio107/value");
    valueFile108.setFileName("/sys/class/gpio/gpio109/value");

    if (!valueFile106.open(QIODevice::ReadOnly) ||
        !valueFile107.open(QIODevice::ReadOnly) ||
        !valueFile108.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open GPIO files!";
    }
}

//三档开关按键检测
void IoRead::Three_way_switch_key_Check()
{
    while (m_isRunning) {
        valueFile106.seek(0); // 重置文件指针到开头
        valueFile107.seek(0);
        valueFile108.seek(0);

        int val106 = valueFile106.readAll().trimmed() == "0" ? 1 : 0;
        int val107 = valueFile107.readAll().trimmed() == "0" ? 1 : 0;
        int val109 = valueFile108.readAll().trimmed() == "0" ? 1 : 0;
        if(OldIoVal106 != val106 && val106 == 1)
        {
            emit IoTrimodeSwitch(106,val106);
            qDebug() << QString("手动档IO状态%1").arg(val106);
        }
        if(OldIoVal107 != val107 && val107 == 1)
        {
            emit IoTrimodeSwitch(107,val107);
            qDebug() << QString("停止档IO状态%1").arg(val107);
        }
        if(OldIoVal109 != val109 && val109 == 1)
        {
            emit IoTrimodeSwitch(109,val109);
            qDebug() << QString("自动档IO状态%1").arg(val109);
        }
        OldIoVal106 = val106;
        OldIoVal107 = val107;
        OldIoVal109 = val109;
        QThread::msleep(50);
    }
}

void IoRead::initGPIO(int gpioNumber) {
    // 导出 GPIO
    QFile exportFile("/sys/class/gpio/export");
    if (exportFile.open(QIODevice::WriteOnly)) {
        exportFile.write(QString::number(gpioNumber).toUtf8());
        exportFile.close();
    }

    // 设置为输入模式
    QFile directionFile(QString("/sys/class/gpio/gpio%1/direction").arg(gpioNumber));
    if (directionFile.open(QIODevice::WriteOnly)) {
        directionFile.write("in");
        directionFile.close();
    }
}
