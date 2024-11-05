#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QObject>
#include <cmd.h>
#include <QMutex>

class SerialSendCmdThread:public QObject
{
    Q_OBJECT
public:
    SerialSendCmdThread(QObject *parent = nullptr);


public slots:
    void serialSendCmd();               //发送线程

private:
    QMutex *m_sendLocker;
signals:
    void APP_Uart_SendDataSignal(const QByteArray &data);

};




class SerialRecvCmdThread:public QObject
{
    Q_OBJECT
public:
    SerialRecvCmdThread(QObject *parent = nullptr);
private:
    QMutex m_recvLocker;
public slots:
    void serialRecvCmd();
public:
    uint16_t SUM_CHK(uint8_t *dataLen, uint8_t *dataCmd, uint8_t *data, uint16_t len, uint8_t sum);         //校验和
signals:
    void ExtendReadParDealSignal(char mainCmd, char sunCmd, const QByteArray &recDataBuf,int dataLen);         //解析数据信号


};


#endif // MYTHREAD_H
