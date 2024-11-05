#include "mythread.h"
#include <QThread>

SerialSendCmdThread::SerialSendCmdThread(QObject* parent)
    : QObject(parent)
{
    m_sendLocker=new QMutex;

    connect(this,&SerialSendCmdThread::APP_Uart_SendDataSignal,g_Usart,&Usart::APP_Uart_SendData);

}

void SerialSendCmdThread::serialSendCmd()
{
    QByteArray data;
    while (1)
    {
        m_sendLocker->lock();
        if(!m_sendCmdQueue.isEmpty())
        {
            data = m_sendCmdQueue.dequeue();      //获取队列开头元素
            if(!data.isEmpty())
            {
//                g_Usart->APP_Uart_SendData(data);
                emit APP_Uart_SendDataSignal(data);
            }
        }
        m_sendLocker->unlock();
        QThread::msleep(5);
    }
}


/******************************************************************
                                发送线程
******************************************************************/
SerialRecvCmdThread::SerialRecvCmdThread(QObject* parent)
    : QObject(parent)
{
//    m_recvLocker=new QMutex;

    connect(this,&SerialRecvCmdThread::ExtendReadParDealSignal,g_Usart,&Usart::ExtendReadParDeal);
}

void SerialRecvCmdThread::serialRecvCmd()
{
    int dataLen = 0;                           //数据长度（命令+所有数据）
    uint8_t* sumCheckData = new uint8_t[1];                           //校验和数据
    uint8_t* recDataBuf = new uint8_t[1024];        //Data数据
    uint8_t* headBuf = new uint8_t[2];                //头命令
    uint8_t* lenBuf = new uint8_t[2];                 //数据长度
    uint8_t* cmdBuf = new uint8_t[2];                 //命令指令
    while (1)
    {
        m_recvLocker.lock();
        if(!CircularBuffer_IsEmpty())
        {
            CircularBuffer_ReadData(headBuf,2);
//            QByteArray byteArray((const char*)headBuf,2);
            if(headBuf[0] == EXTEND_HEAD_1 && headBuf[1]==EXTEND_HEAD_2)
            {
                //帧头正确，解析长度
                CircularBuffer_ReadData(lenBuf,2);
//                QByteArray byteArray1((const char*)lenBuf,2);
                dataLen=(lenBuf[1]<<8)+lenBuf[0];
                CircularBuffer_ReadData(cmdBuf,2);
//                QByteArray byteArray2((const char*)cmdBuf,2);
                CircularBuffer_ReadData(recDataBuf,dataLen-2);
//                QByteArray byteArray3((const char*)recDataBuf,dataLen-2);
                CircularBuffer_ReadData(sumCheckData,1);
                if(SUM_CHK(lenBuf,cmdBuf,recDataBuf,dataLen-2,sumCheckData[0]))
                {
                    QByteArray byteTemp;
                    byteTemp.resize(dataLen-2);
                    for(int i=0;i<dataLen-2;i++)
                    {
                        byteTemp[i] = recDataBuf[i];
                    }
                    emit ExtendReadParDealSignal((char)cmdBuf[0],(char)cmdBuf[1],byteTemp,dataLen-2);
//                    g_Usart->ExtendReadParDeal(cmdBuf[0],cmdBuf[1],recDataBuf,dataLen-2);
                }
            }
        }
        m_recvLocker.unlock();
        QThread::msleep(5);
    }
}

/*和校验**/
uint16_t SerialRecvCmdThread::SUM_CHK(uint8_t *dataLen, uint8_t *dataCmd, uint8_t *data, uint16_t len, uint8_t sum)
{
    uint16_t i=0;
    uint8_t dataSum=0;
    for(i=0; i<2; i++)
    {
        dataSum += dataLen[i];
    }
    for(i=0; i<2; i++)
    {
        dataSum += dataCmd[i];
    }
    for(i=0; i<len; i++)
    {
        dataSum += data[i];
    }

    if(dataSum == sum)
    {
        return 1;
    }

    return 0;
}

