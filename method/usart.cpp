#include "usart.h"
#include <QThread>
#include <QDebug>
int dataLen = 0;                           //数据长度（命令+所有数据）
uint8_t* sumCheckData = new uint8_t[1];                           //校验和数据
uint8_t* recDataBuf = new uint8_t[1024];        //Data数据
uint8_t* headBuf = new uint8_t[2];                //头命令
uint8_t* lenBuf = new uint8_t[2];                 //数据长度
uint8_t* cmdBuf = new uint8_t[2];                 //命令指令
AxisCurPos m_AxisCurPos;//轴当前实时位置
Usart::Usart(QObject *parent):QObject(parent)
{
    m_serialPort=new QSerialPort(this);
    UsartTimer = new QTimer(this);
    connect(m_serialPort, &QSerialPort::readyRead, this, &Usart::onReadyRead);
    connect(m_serialPort, &QSerialPort::errorOccurred,this,&Usart::onHandleError);
    connect(UsartTimer, &QTimer::timeout,this,&Usart::DataSyc);
}

Usart::~Usart()
{
    m_serialPort->close();
    UsartTimer->deleteLater();
}

void Usart::closeSerialPort()
{
    if (m_serialPort->isOpen())
        m_serialPort->close();
}

uint16_t Usart::openSerialPort()
{
    m_serialPort->setPortName("/dev/ttySAC2");
    if(!m_serialPort->open(QIODevice::ReadWrite))
    {
      return  SERIAL_NOT_OPEN;
    }
    m_serialPort->setBaudRate(QSerialPort::Baud115200);//设置波特率为115200
    m_serialPort->setDataBits(QSerialPort::Data8);//设置数据位8
    m_serialPort->setParity(QSerialPort::NoParity); //校验位设置为0
    m_serialPort->setStopBits(QSerialPort::OneStop);//停止位设置为1
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);//设置为无流控制

    return 0;
}

void Usart::APP_Uart_SendData(const QByteArray &data)
{
    m_serialPort->write(data);
//    m_serialPort->waitForBytesWritten(1000);//确保
}


void Usart::onReadyRead()
{
    QByteArray byteArray = m_serialPort->readAll();
    // 获取QByteArray的数据指针
    uint16_t size=byteArray.size();
    uint8_t* uint8Array = new uint8_t[size];
    for(int i=0;i<size;i++)
    {
        uint8Array[i] = byteArray[i];
    }
    CircularBuffer_WriteData(uint8Array,size);
    delete [] uint8Array;
}

void Usart::onHandleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        closeSerialPort();
    }
}

/*和校验**/
uint16_t Usart::SUM_CHK(uint8_t *data, uint16_t len, uint8_t sum)
{
    uint16_t i=0;
    uint8_t dataSum=0;
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

//发送数据，等串口发送写好后添加发送过程
/**********************************************************
**  函数名：  ExtendSendDataDeal()
**	输入参数：data 待发送的数据指针；len 待发送的数据长度
**	输出参数：
**	函数功能：发送一帧待发送的数据，该数据帧是不需要满足自定义协议的
**	返回：		0成功，-1失败
***********************************************************/
uint16_t Usart::ExtendSendDataDeal(uint8_t *data, uint16_t len)
{
    uint16_t totalLen=len+5;
    uint8_t sendData[EXTEND_BUF_LEN] = {0};
    uint16_t i = 0;
    uint8_t sum=0;
    if(data == NULL)
    {
        return UART_ERROR;
    }

    sendData[0] = EXTEND_HEAD_1;
    sendData[1] = EXTEND_HEAD_2;
    sendData[2] = (uint8_t)len;
    sum += sendData[2];
    sendData[3] = (uint8_t)(len >> 8);
    sum += sendData[3];

    for(i=0; i<len; i++)
    {
        sendData[4+i] = data[i];
        sum += data[i];
    }
    sendData[len + 4] = sum;

    QByteArray byteArray((const char*)sendData,totalLen);
//    APP_Uart_SendData(byteArray);
    m_sendCmdQueue.enqueue(byteArray);

    return 0;
}

//发送参数、程序、状态读取应答
void Usart::ExtendSendParProReadAnswer(uint8_t mainCmd, uint8_t sunCmd, uint8_t *recDataBuf, uint8_t dataLen)
{
    uint16_t i = 0;
    uint8_t data[EXTEND_BUF_LEN] = {0};
    uint8_t len = 2 + dataLen;

    data[0] = mainCmd;
    data[1] = sunCmd;

    for(i=0; i<dataLen; i++)
    {
        data[i + 2] = recDataBuf[i];
    }

    ExtendSendDataDeal(data, len);
}

//手动操作下发
void Usart::ExtendSendManualOperationDeal(uint8_t mainCmd, uint8_t sunCmd, uint16_t parNum, uint16_t parNum2)
{
    uint8_t sendDataBuf[EXTEND_BUF_LEN] = {0};
    uint16_t len=0;
    if(mainCmd == CMD_MAIN_MANUAL)
    {
        switch(sunCmd)
        {
        case CMD_SUN_MANUAL_OUT://输出控制
            sendDataBuf[len] = (uint8_t)parNum;
            sendDataBuf[len+1] = (uint8_t)parNum2;
            len += 2;
            break;
        case CMD_SUN_MANUAL_AXIS://轴连动
            sendDataBuf[len] = (uint8_t)parNum;
            sendDataBuf[len+1] = (uint8_t)parNum2;
            sendDataBuf[len+2] = (uint8_t)m_manualAxis.speed;
            sendDataBuf[len+3] = (uint8_t)(m_manualAxis.speed>>8);
            len += 4;
            sendDataBuf[len] = 0;
            sendDataBuf[len+1] = 0;
            sendDataBuf[len+2] = 0;
            sendDataBuf[len+3] = 0;
            len += 4;
            sendDataBuf[len] = m_manualAxis.ZDrop;
            len += 1;
            break;
        case CMD_SUN_MANUAL_KEY://按键操作

            break;
        case CMD_SUN_MANUAL_HAND_WHEEL://手轮
            sendDataBuf[len] = (uint8_t)m_manualAxis.handwheelAxis;
            sendDataBuf[len+1] = (uint8_t)parNum;
            sendDataBuf[len+2] = m_manualAxis.multiply;
            sendDataBuf[len+3] = (uint8_t)m_manualAxis.speed;
            sendDataBuf[len+4] = (uint8_t)(m_manualAxis.speed>>8);
            sendDataBuf[len+5] = m_manualAxis.ZDrop;
            len += 6;
            break;
        case CMD_SUN_MANUAL_INCREMENT://增量
            sendDataBuf[len] = (uint8_t)parNum;
            sendDataBuf[len+1] = (uint8_t)parNum2;
            sendDataBuf[len+2] = (uint8_t)m_manualAxis.speed;
            sendDataBuf[len+3] = (uint8_t)(m_manualAxis.speed>>8);
            len += 4;
            if(parNum2 == 1)
            {//负向
                sendDataBuf[len] = (uint8_t)m_manualAxis.sub_pos;
                sendDataBuf[len+1] = (uint8_t)(m_manualAxis.sub_pos>>8);
                sendDataBuf[len+2] = (uint8_t)(m_manualAxis.sub_pos>>16);
                sendDataBuf[len+3] = (uint8_t)(m_manualAxis.sub_pos>>24);
            }
            else
            {//正向
                sendDataBuf[len] = (uint8_t)m_manualAxis.pos_pos;
                sendDataBuf[len+1] = (uint8_t)(m_manualAxis.pos_pos>>8);
                sendDataBuf[len+2] = (uint8_t)(m_manualAxis.pos_pos>>16);
                sendDataBuf[len+3] = (uint8_t)(m_manualAxis.pos_pos>>24);
            }
            len += 4;
            sendDataBuf[len] = m_manualAxis.ZDrop;
            len += 1;
            break;
        case CMD_SUN_MANUAL_STACK://移至堆叠点

            break;
        }
    }
    ExtendSendParProReadAnswer(mainCmd, sunCmd, sendDataBuf, len);
}

//下发——参数
void Usart::ExtendSendParDeal(uint8_t mainCmd, uint8_t sunCmd, uint16_t parNum, uint16_t parNum2)
{
    uint16_t i=0;
    uint16_t num=0;
    uint8_t axis=0;
    uint16_t index=0;
    uint8_t sendDataBuf[EXTEND_BUF_LEN] = {0};
    uint16_t len=0;
    if(mainCmd == CMD_MAIN_SIGNAL)
    {
        switch(sunCmd)
        {
        case CMD_SUN_SIGNAL_OUT_TYPE://输出类型
            for(i=0; i<OUT_PORT_TYPE_NUM; i++)
            {
                sendDataBuf[i*2] = m_OutPortType[i][0];
                sendDataBuf[i*2+1] = m_OutPortType[i][1];
                len += 2;
            }
            break;
        case CMD_SUN_SIGNAL_INTERLOCK://互锁设置
            for(i=0; i<OUT_INTERLOCK_NUM; i++)
            {
                sendDataBuf[i*4] = m_OutportInterlock[i][0];
                sendDataBuf[i*4+1] = m_OutportInterlock[i][1];
                sendDataBuf[i*4+2] = m_OutportInterlock[i][2];
                sendDataBuf[i*4+3] = m_OutportInterlock[i][3];
                len += 4;
            }
            break;
        case CMD_SUN_SIGNAL_IN_FUNC_DEF://输入端口自定义
            for(i=0; i<INPUT_TOTAL_NUM; i++)
            {
                sendDataBuf[i] = m_InportFuncDefine[i];
                len += 1;
            }
            break;
        case CMD_SUN_SIGNAL_OUT_FUNC_DEF://输出端口自定义
            for(i=0; i<OUTPUT_TOTAL_NUM; i++)
            {
                sendDataBuf[i] = m_OutportFuncDefine[i];
                len += 1;
            }
            break;
        case CMD_SUN_SIGNAL_OUT_RELEVENCY://预留关联
            for(i=0; i<OUT_PORT_RELEVANCY_NUM; i++)
            {
                sendDataBuf[i*3] = m_OutportRelevancy[i][0];
                sendDataBuf[i*3+1] = m_OutportRelevancy[i][1];
                sendDataBuf[i*3+2] = m_OutportRelevancy[i][2];
                len += 3;
            }
            break;
        case CMD_SUN_SIGNAL_RELATE_OUT://预留出类型
            for(i=0; i<OUT_PORT_RELEOUT_NUM; i++)
            {
                sendDataBuf[i*2] = m_OutportReleteOut[i][0];
                sendDataBuf[i*2+1] = m_OutportReleteOut[i][1];
                len += 2;
            }
            break;
        case CMD_SUN_SIGNAL_KEY://按键/信号
            for(i=0; i<OPR_KEY_NUM; i++)
            {
                sendDataBuf[i*3] = m_KeyFunc[i].keyType;
                sendDataBuf[i*3+1] = m_KeyFunc[i].funcNum;
                sendDataBuf[i*3+2] = m_KeyFunc[i].oprMode;
                len += 3;
            }

            for(i=0; i<OPR_LED_NUM; i++)
            {
                sendDataBuf[OPR_KEY_NUM*3 + i*2] = m_LedFunc[i].ledType;
                sendDataBuf[OPR_KEY_NUM*3 + i*2+1] = m_LedFunc[i].funcNum;
                len += 2;
            }
            break;
        case CMD_SUN_SIGNAL_SENIOR://高级
            len = sizeof(D_SeniorFuncStruct);
            memcpy(sendDataBuf, &m_SeniorFunc, len);			//使用拷贝函数需要注意结构体内的元素顺序以及4字节对其
            break;
        case CMD_SUN_SIGNAL_SENIOR_PORT://高级功能端口
            len = sizeof(D_SeniorFuncPortStruct);
            memcpy(sendDataBuf, &m_SeniorFuncPort, len);	//使用拷贝函数需要注意结构体内的元素顺序以及4字节对其
            break;
        default:
            break;
        }
    }
    else if(mainCmd == CMD_MAIN_SAVE)
    {//安全设置
        switch(sunCmd)
        {
        case CMD_SUN_SAVE_MACHINE://机床安全
            sendDataBuf[0] = m_MachineSave.armFallAutoDoor1Swt;
            sendDataBuf[1] = m_MachineSave.armFallAutoDoor1SwtSta;
            sendDataBuf[2] = m_MachineSave.armFallAutoDoor2Swt;
            sendDataBuf[3] = m_MachineSave.armFallAutoDoor2SwtSta;
            sendDataBuf[4] = m_MachineSave.firstProductStartRun;
            sendDataBuf[5] = m_MachineSave.firstProductFinish;
            len = 6;
            break;
        case CMD_SUN_SAVE_STACK://料仓安全
            sendDataBuf[0] = m_StackSave.stackSaveIn1Roatore;
            sendDataBuf[1] = m_StackSave.stackSaveIn2Roatore;
            len = 2;
            break;
        case CMD_SUN_SAVE_CALW://卡爪安全
            sendDataBuf[0] = parNum;
            num = parNum - 1;
            sendDataBuf[1] = m_ClawSave[num].clawKeepoutSta;
            len = 2;
            sendDataBuf[len] = m_ClawSave[num].axisSelect_X;
            len += 1;
            sendDataBuf[len] = m_ClawSave[num].axisSelect_Z;
            len += 1;
            sendDataBuf[len] = (uint8_t)m_ClawSave[num].clawKeepoutMinX;
            sendDataBuf[len+1] = (uint8_t)(m_ClawSave[num].clawKeepoutMinX>>8);
            sendDataBuf[len+2] = (uint8_t)(m_ClawSave[num].clawKeepoutMinX>>16);
            sendDataBuf[len+3] = (uint8_t)(m_ClawSave[num].clawKeepoutMinX>>24);
            len += 4;
            sendDataBuf[len] = (uint8_t)m_ClawSave[num].clawKeepoutMaxX;
            sendDataBuf[len+1] = (uint8_t)(m_ClawSave[num].clawKeepoutMaxX>>8);
            sendDataBuf[len+2] = (uint8_t)(m_ClawSave[num].clawKeepoutMaxX>>16);
            sendDataBuf[len+3] = (uint8_t)(m_ClawSave[num].clawKeepoutMaxX>>24);
            len += 4;
            sendDataBuf[len] = (uint8_t)m_ClawSave[num].clawKeepoutHighZ;
            sendDataBuf[len+1] = (uint8_t)(m_ClawSave[num].clawKeepoutHighZ>>8);
            sendDataBuf[len+2] = (uint8_t)(m_ClawSave[num].clawKeepoutHighZ>>16);
            sendDataBuf[len+3] = (uint8_t)(m_ClawSave[num].clawKeepoutHighZ>>24);
            len += 4;
            sendDataBuf[len] = (uint8_t)m_ClawSave[num].cKeepoutMinX;
            sendDataBuf[len+1] = (uint8_t)(m_ClawSave[num].cKeepoutMinX>>8);
            sendDataBuf[len+2] = (uint8_t)(m_ClawSave[num].cKeepoutMinX>>16);
            sendDataBuf[len+3] = (uint8_t)(m_ClawSave[num].cKeepoutMinX>>24);
            len += 4;
            sendDataBuf[len] = (uint8_t)m_ClawSave[num].cKeepoutMaxX;
            sendDataBuf[len+1] = (uint8_t)(m_ClawSave[num].cKeepoutMaxX>>8);
            sendDataBuf[len+2] = (uint8_t)(m_ClawSave[num].cKeepoutMaxX>>16);
            sendDataBuf[len+3] = (uint8_t)(m_ClawSave[num].cKeepoutMaxX>>24);
            len += 4;
            sendDataBuf[len] = (uint8_t)m_ClawSave[num].cKeepoutHighZ;
            sendDataBuf[len+1] = (uint8_t)(m_ClawSave[num].cKeepoutHighZ>>8);
            sendDataBuf[len+2] = (uint8_t)(m_ClawSave[num].cKeepoutHighZ>>16);
            sendDataBuf[len+3] = (uint8_t)(m_ClawSave[num].cKeepoutHighZ>>24);
            len += 4;
            sendDataBuf[len] = (uint8_t)m_ClawSave[num].cKeepoutPosMinC;
            sendDataBuf[len+1] = (uint8_t)(m_ClawSave[num].cKeepoutPosMinC>>8);
            sendDataBuf[len+2] = (uint8_t)(m_ClawSave[num].cKeepoutPosMinC>>16);
            sendDataBuf[len+3] = (uint8_t)(m_ClawSave[num].cKeepoutPosMinC>>24);
            len += 4;
            sendDataBuf[len] = (uint8_t)m_ClawSave[num].cKeepoutPosMaxC;
            sendDataBuf[len+1] = (uint8_t)(m_ClawSave[num].cKeepoutPosMaxC>>8);
            sendDataBuf[len+2] = (uint8_t)(m_ClawSave[num].cKeepoutPosMaxC>>16);
            sendDataBuf[len+3] = (uint8_t)(m_ClawSave[num].cKeepoutPosMaxC>>24);
            len += 4;
            sendDataBuf[len] = (uint8_t)m_ClawSave[num].cKeepoutNegMinC;
            sendDataBuf[len+1] = (uint8_t)(m_ClawSave[num].cKeepoutNegMinC>>8);
            sendDataBuf[len+2] = (uint8_t)(m_ClawSave[num].cKeepoutNegMinC>>16);
            sendDataBuf[len+3] = (uint8_t)(m_ClawSave[num].cKeepoutNegMinC>>24);
            len += 4;
            sendDataBuf[len] = (uint8_t)m_ClawSave[num].cKeepoutNegMaxC;
            sendDataBuf[len+1] = (uint8_t)(m_ClawSave[num].cKeepoutNegMaxC>>8);
            sendDataBuf[len+2] = (uint8_t)(m_ClawSave[num].cKeepoutNegMaxC>>16);
            sendDataBuf[len+3] = (uint8_t)(m_ClawSave[num].cKeepoutNegMaxC>>24);
            len += 4;
            break;
        case CMD_SUN_SAVE_ONLINE://联机安全
            sendDataBuf[0] = parNum;
            num = parNum - 1;
            len = 1;
            sendDataBuf[len] = m_OnlineSave[num].mainSunMode;
            len += 1;
            sendDataBuf[len] = (uint8_t)m_OnlineSave[num].pluseTime;
            sendDataBuf[len+1] = (uint8_t)(m_OnlineSave[num].pluseTime>>8);
            len += 2;
            sendDataBuf[len] = m_OnlineSave[num].onlineSelect;
            len += 1;
            sendDataBuf[len] = m_OnlineSave[num].axisNum;
            len += 1;
            sendDataBuf[len] = m_OnlineSave[num].areaInNum;
            len += 1;
            sendDataBuf[len] = m_OnlineSave[num].areaOutNum;
            len += 1;
            sendDataBuf[len] = m_OnlineSave[num].requestInNum;
            len += 1;
            sendDataBuf[len] = m_OnlineSave[num].requestOutNum;
            len += 1;
            sendDataBuf[len] = (uint8_t)m_OnlineSave[num].a1A2MainPos;
            sendDataBuf[len+1] = (uint8_t)(m_OnlineSave[num].a1A2MainPos>>8);
            sendDataBuf[len+2] = (uint8_t)(m_OnlineSave[num].a1A2MainPos>>16);
            sendDataBuf[len+3] = (uint8_t)(m_OnlineSave[num].a1A2MainPos>>24);
            len += 4;
            sendDataBuf[len] = (uint8_t)m_OnlineSave[num].a1A2SunPos;
            sendDataBuf[len+1] = (uint8_t)(m_OnlineSave[num].a1A2SunPos>>8);
            sendDataBuf[len+2] = (uint8_t)(m_OnlineSave[num].a1A2SunPos>>16);
            sendDataBuf[len+3] = (uint8_t)(m_OnlineSave[num].a1A2SunPos>>24);
            len += 4;
            break;
        default:
            break;
        }
    }
    else if(mainCmd == CMD_MAIN_PRODUCT)
    {//产品设置
        switch(sunCmd)
        {
        case CMD_SUN_PRODUCT_PAR://产品参数
            len=0;
            sendDataBuf[len] = (uint8_t)m_ProductSet.waitProcessFinTime;
            sendDataBuf[len+1] = (uint8_t)(m_ProductSet.waitProcessFinTime>>8);
            len+=2;
            sendDataBuf[len] = (uint8_t)m_ProductSet.pluseOutTime;
            sendDataBuf[len+1] = (uint8_t)(m_ProductSet.pluseOutTime>>8);
            len+=2;
            sendDataBuf[len] = (uint8_t)m_ProductSet.airVlaveLimitTime;
            sendDataBuf[len+1] = (uint8_t)(m_ProductSet.airVlaveLimitTime>>8);
            len+=2;
            sendDataBuf[len] = (uint8_t)m_ProductSet.autoCycTime;
            sendDataBuf[len+1] = (uint8_t)(m_ProductSet.autoCycTime>>8);
            len+=2;
            sendDataBuf[len] = (uint8_t)m_ProductSet.alarmBuzzerTime;
            sendDataBuf[len+1] = (uint8_t)(m_ProductSet.alarmBuzzerTime>>8);
            len+=2;
            sendDataBuf[len] = (uint8_t)m_ProductSet.chainReachDelay;
            sendDataBuf[len+1] = (uint8_t)(m_ProductSet.chainReachDelay>>8);
            len+=2;
            sendDataBuf[len] = (uint8_t)m_ProductSet.clampCheckFilterTime;
            sendDataBuf[len+1] = (uint8_t)(m_ProductSet.clampCheckFilterTime>>8);
            len+=2;
            break;
        case CMD_SUN_PRODUCT_SENIOR://高级
            len=0;
            sendDataBuf[len] = (uint8_t)m_ProductSenior.lubricOpenTime;
            sendDataBuf[len+1] = (uint8_t)(m_ProductSenior.lubricOpenTime>>8);
            len+=2;
            sendDataBuf[len] = (uint8_t)m_ProductSenior.lubricCloseTime;
            sendDataBuf[len+1] = (uint8_t)(m_ProductSenior.lubricCloseTime>>8);
            len+=2;
            sendDataBuf[len] = (uint8_t)m_ProductSenior.lubricDelay;
            sendDataBuf[len+1] = (uint8_t)(m_ProductSenior.lubricDelay>>8);
            len+=2;
            sendDataBuf[len] = (uint8_t)m_ProductSenior.lubricGapCyc;
            sendDataBuf[len+1] = (uint8_t)(m_ProductSenior.lubricGapCyc>>8);
            len+=2;
            sendDataBuf[len] = m_ProductSenior.productMemoryFlag;
            len+=1;
            break;
        case CMD_SUN_PRODUCT_INTERNET://物联网
            sendDataBuf[0] = m_Internet.netCommType;
            sendDataBuf[1] = m_Internet.parityMode;
            sendDataBuf[2] = m_Internet.baudMode;
            len=3;
            break;
        default:
            break;
        }
    }
    else if(mainCmd == CMD_MAIN_SERVO)
    {//伺服速度
        switch(sunCmd)
        {
        case CMD_SUN_SERVO_ACC_DEC://加减速时间
            sendDataBuf[0] = parNum;
            num = parNum - 1;
            len = 1;
            if(num < AXIS_TOTAL_NUM)
            {//轴
                sendDataBuf[len] = (uint8_t)m_AxisPar[num].accTime;
                sendDataBuf[len+1] = (uint8_t)(m_AxisPar[num].accTime>>8);
                len += 2;
                sendDataBuf[len] = (uint8_t)m_AxisPar[num].decTime;
                sendDataBuf[len+1] = (uint8_t)(m_AxisPar[num].decTime>>8);
                len += 2;
                sendDataBuf[len] = (uint8_t)m_AxisPar[num].accAcc;
                sendDataBuf[len+1] = (uint8_t)(m_AxisPar[num].accAcc>>8);
                len += 2;
                sendDataBuf[len] = (uint8_t)m_AxisPar[num].decDec;
                sendDataBuf[len+1] = (uint8_t)(m_AxisPar[num].decDec>>8);
                len += 2;
            }
            else
            {//手轮
                sendDataBuf[len] = (uint8_t)m_HandwheelPar.accTime;
                sendDataBuf[len+1] = (uint8_t)(m_HandwheelPar.accTime>>8);
                sendDataBuf[len+2] = (uint8_t)(m_HandwheelPar.accTime>>16);
                sendDataBuf[len+3] = (uint8_t)(m_HandwheelPar.accTime>>24);
                len += 4;
                sendDataBuf[len] = (uint8_t)m_HandwheelPar.decTime;
                sendDataBuf[len+1] = (uint8_t)(m_HandwheelPar.decTime>>8);
                sendDataBuf[len+2] = (uint8_t)(m_HandwheelPar.decTime>>16);
                sendDataBuf[len+3] = (uint8_t)(m_HandwheelPar.decTime>>24);
                len += 4;
                sendDataBuf[len] = (uint8_t)m_HandwheelPar.accAcc;
                sendDataBuf[len+1] = (uint8_t)(m_HandwheelPar.accAcc>>8);
                len += 2;
                sendDataBuf[len] = (uint8_t)m_HandwheelPar.decDec;
                sendDataBuf[len+1] = (uint8_t)(m_HandwheelPar.decDec>>8);
                len += 2;
            }
            break;
        case CMD_SUN_SERVO_MAX_SPEED://最大速度
            for(i=0; i<AXIS_TOTAL_NUM; i++)
            {
                sendDataBuf[len] = (uint8_t)m_AxisPar[i].maxSpeed;
                sendDataBuf[len+1] = (uint8_t)(m_AxisPar[i].maxSpeed>>8);
                len += 2;
            }
            break;
        case CMD_SUN_SERVO_TOLERANCE://容差
            sendDataBuf[len] = (uint8_t)m_DegreeParS.tolerance;
            sendDataBuf[len+1] = (uint8_t)(m_DegreeParS.tolerance>>8);
            sendDataBuf[len+2] = (uint8_t)(m_DegreeParS.tolerance>>16);
            sendDataBuf[len+3] = (uint8_t)(m_DegreeParS.tolerance>>24);
            len += 4;
            sendDataBuf[len] = (uint8_t)m_DegreeParS.originDegree;
            sendDataBuf[len+1] = (uint8_t)(m_DegreeParS.originDegree>>8);
            sendDataBuf[len+2] = (uint8_t)(m_DegreeParS.originDegree>>16);
            sendDataBuf[len+3] = (uint8_t)(m_DegreeParS.originDegree>>24);
            len += 4;
            sendDataBuf[len] = (uint8_t)m_DegreeParS.actionFinDegree;
            sendDataBuf[len+1] = (uint8_t)(m_DegreeParS.actionFinDegree>>8);
            len += 2;
            break;
        default:
            break;
        }
    }
    else if(mainCmd == CMD_MAIN_SP)
    {//伺服安全点
        switch(sunCmd)
        {
        case CMD_SUN_SP_AREA://安全区
            sendDataBuf[0] = parNum;
            num = parNum - 1;
            sendDataBuf[1] = m_SaveArea[num].saveAreaSwt;
            len = 2;
            for(i=0; i<STACK_AXIS_NUM; i++)
            {
                sendDataBuf[len+i] = m_SaveArea[num].axisSelect[i];
            }
            len += STACK_AXIS_NUM;
            sendDataBuf[len] = m_SaveArea[num].processFinishNum;
            len += 1;
            sendDataBuf[len] = (uint8_t)m_SaveArea[num].machineA1Pos[STACK_X_AXIS];
            sendDataBuf[len+1] = (uint8_t)(m_SaveArea[num].machineA1Pos[STACK_X_AXIS]>>8);
            sendDataBuf[len+2] = (uint8_t)(m_SaveArea[num].machineA1Pos[STACK_X_AXIS]>>16);
            sendDataBuf[len+3] = (uint8_t)(m_SaveArea[num].machineA1Pos[STACK_X_AXIS]>>24);
            len += 4;
            sendDataBuf[len] = (uint8_t)m_SaveArea[num].machineA1Pos[STACK_Y_AXIS];
            sendDataBuf[len+1] = (uint8_t)(m_SaveArea[num].machineA1Pos[STACK_Y_AXIS]>>8);
            sendDataBuf[len+2] = (uint8_t)(m_SaveArea[num].machineA1Pos[STACK_Y_AXIS]>>16);
            sendDataBuf[len+3] = (uint8_t)(m_SaveArea[num].machineA1Pos[STACK_Y_AXIS]>>24);
            len += 4;
            sendDataBuf[len] = (uint8_t)m_SaveArea[num].machineA2Pos[STACK_X_AXIS];
            sendDataBuf[len+1] = (uint8_t)(m_SaveArea[num].machineA2Pos[STACK_X_AXIS]>>8);
            sendDataBuf[len+2] = (uint8_t)(m_SaveArea[num].machineA2Pos[STACK_X_AXIS]>>16);
            sendDataBuf[len+3] = (uint8_t)(m_SaveArea[num].machineA2Pos[STACK_X_AXIS]>>24);
            len += 4;
            sendDataBuf[len] = (uint8_t)m_SaveArea[num].machineA2Pos[STACK_Y_AXIS];
            sendDataBuf[len+1] = (uint8_t)(m_SaveArea[num].machineA2Pos[STACK_Y_AXIS]>>8);
            sendDataBuf[len+2] = (uint8_t)(m_SaveArea[num].machineA2Pos[STACK_Y_AXIS]>>16);
            sendDataBuf[len+3] = (uint8_t)(m_SaveArea[num].machineA2Pos[STACK_Y_AXIS]>>24);
            len += 4;
            sendDataBuf[len] = (uint8_t)m_SaveArea[num].starckB1Pos[STACK_X_AXIS];
            sendDataBuf[len+1] = (uint8_t)(m_SaveArea[num].starckB1Pos[STACK_X_AXIS]>>8);
            sendDataBuf[len+2] = (uint8_t)(m_SaveArea[num].starckB1Pos[STACK_X_AXIS]>>16);
            sendDataBuf[len+3] = (uint8_t)(m_SaveArea[num].starckB1Pos[STACK_X_AXIS]>>24);
            len += 4;
            sendDataBuf[len] = (uint8_t)m_SaveArea[num].starckB1Pos[STACK_Y_AXIS];
            sendDataBuf[len+1] = (uint8_t)(m_SaveArea[num].starckB1Pos[STACK_Y_AXIS]>>8);
            sendDataBuf[len+2] = (uint8_t)(m_SaveArea[num].starckB1Pos[STACK_Y_AXIS]>>16);
            sendDataBuf[len+3] = (uint8_t)(m_SaveArea[num].starckB1Pos[STACK_Y_AXIS]>>24);
            len += 4;
            sendDataBuf[len] = (uint8_t)m_SaveArea[num].starckB2Pos[STACK_X_AXIS];
            sendDataBuf[len+1] = (uint8_t)(m_SaveArea[num].starckB2Pos[STACK_X_AXIS]>>8);
            sendDataBuf[len+2] = (uint8_t)(m_SaveArea[num].starckB2Pos[STACK_X_AXIS]>>16);
            sendDataBuf[len+3] = (uint8_t)(m_SaveArea[num].starckB2Pos[STACK_X_AXIS]>>24);
            len += 4;
            sendDataBuf[len] = (uint8_t)m_SaveArea[num].starckB2Pos[STACK_Y_AXIS];
            sendDataBuf[len+1] = (uint8_t)(m_SaveArea[num].starckB2Pos[STACK_Y_AXIS]>>8);
            sendDataBuf[len+2] = (uint8_t)(m_SaveArea[num].starckB2Pos[STACK_Y_AXIS]>>16);
            sendDataBuf[len+3] = (uint8_t)(m_SaveArea[num].starckB2Pos[STACK_Y_AXIS]>>24);
            len += 4;
            sendDataBuf[len] = (uint8_t)m_SaveArea[num].machineA1Pos[STACK_Z_AXIS];
            sendDataBuf[len+1] = (uint8_t)(m_SaveArea[num].machineA1Pos[STACK_Z_AXIS]>>8);
            sendDataBuf[len+2] = (uint8_t)(m_SaveArea[num].machineA1Pos[STACK_Z_AXIS]>>16);
            sendDataBuf[len+3] = (uint8_t)(m_SaveArea[num].machineA1Pos[STACK_Z_AXIS]>>24);
            len += 4;
            sendDataBuf[len] = (uint8_t)m_SaveArea[num].machineA2Pos[STACK_Z_AXIS];
            sendDataBuf[len+1] = (uint8_t)(m_SaveArea[num].machineA2Pos[STACK_Z_AXIS]>>8);
            sendDataBuf[len+2] = (uint8_t)(m_SaveArea[num].machineA2Pos[STACK_Z_AXIS]>>16);
            sendDataBuf[len+3] = (uint8_t)(m_SaveArea[num].machineA2Pos[STACK_Z_AXIS]>>24);
            len += 4;
            sendDataBuf[len] = (uint8_t)m_SaveArea[num].machine_Z_WaitMaxPos;
            sendDataBuf[len+1] = (uint8_t)(m_SaveArea[num].machine_Z_WaitMaxPos>>8);
            sendDataBuf[len+2] = (uint8_t)(m_SaveArea[num].machine_Z_WaitMaxPos>>16);
            sendDataBuf[len+3] = (uint8_t)(m_SaveArea[num].machine_Z_WaitMaxPos>>24);
            len += 4;
            sendDataBuf[len] = (uint8_t)m_SaveArea[num].machine_Z_FallMaxPos;
            sendDataBuf[len+1] = (uint8_t)(m_SaveArea[num].machine_Z_FallMaxPos>>8);
            sendDataBuf[len+2] = (uint8_t)(m_SaveArea[num].machine_Z_FallMaxPos>>16);
            sendDataBuf[len+3] = (uint8_t)(m_SaveArea[num].machine_Z_FallMaxPos>>24);
            len += 4;
            sendDataBuf[len] = (uint8_t)m_SaveArea[num].machine_Z_InsideHigh;
            sendDataBuf[len+1] = (uint8_t)(m_SaveArea[num].machine_Z_InsideHigh>>8);
            sendDataBuf[len+2] = (uint8_t)(m_SaveArea[num].machine_Z_InsideHigh>>16);
            sendDataBuf[len+3] = (uint8_t)(m_SaveArea[num].machine_Z_InsideHigh>>24);
            len += 4;
            sendDataBuf[len] = (uint8_t)m_SaveArea[num].stack_Z_StartMaxPos;
            sendDataBuf[len+1] = (uint8_t)(m_SaveArea[num].stack_Z_StartMaxPos>>8);
            sendDataBuf[len+2] = (uint8_t)(m_SaveArea[num].stack_Z_StartMaxPos>>16);
            sendDataBuf[len+3] = (uint8_t)(m_SaveArea[num].stack_Z_StartMaxPos>>24);
            len += 4;
            sendDataBuf[len] = (uint8_t)m_SaveArea[num].stack_Z_FallMaxPos;
            sendDataBuf[len+1] = (uint8_t)(m_SaveArea[num].stack_Z_FallMaxPos>>8);
            sendDataBuf[len+2] = (uint8_t)(m_SaveArea[num].stack_Z_FallMaxPos>>16);
            sendDataBuf[len+3] = (uint8_t)(m_SaveArea[num].stack_Z_FallMaxPos>>24);
            len += 4;
            break;
        case CMD_SUN_SP_AXIS_LIMIT://轴运动位置限定
            sendDataBuf[0] = parNum;
            num = parNum - 1;
            len = 1;
            sendDataBuf[len] = (uint8_t)m_AxisPar[num].axisMinPos;
            sendDataBuf[len+1] = (uint8_t)(m_AxisPar[num].axisMinPos>>8);
            sendDataBuf[len+2] = (uint8_t)(m_AxisPar[num].axisMinPos>>16);
            sendDataBuf[len+3] = (uint8_t)(m_AxisPar[num].axisMinPos>>24);
            len += 4;
            sendDataBuf[len] = (uint8_t)m_AxisPar[num].axisMaxPos;
            sendDataBuf[len+1] = (uint8_t)(m_AxisPar[num].axisMaxPos>>8);
            sendDataBuf[len+2] = (uint8_t)(m_AxisPar[num].axisMaxPos>>16);
            sendDataBuf[len+3] = (uint8_t)(m_AxisPar[num].axisMaxPos>>24);
            len += 4;
            sendDataBuf[len] = m_AxisPar[num].minPosSignal;
            sendDataBuf[len+1] = m_AxisPar[num].maxPosSignal;
            len += 2;
            break;
        case CMD_SUN_SP_RAMPAGE_LIMIT://横行位置限定
            sendDataBuf[len] = (uint8_t)m_DegreeParS.saveRampageHige_Z1;
            sendDataBuf[len+1] = (uint8_t)(m_DegreeParS.saveRampageHige_Z1>>8);
            sendDataBuf[len+2] = (uint8_t)(m_DegreeParS.saveRampageHige_Z1>>16);
            sendDataBuf[len+3] = (uint8_t)(m_DegreeParS.saveRampageHige_Z1>>24);
            len += 4;
            sendDataBuf[len] = (uint8_t)m_DegreeParS.saveRampageHige_Z2;
            sendDataBuf[len+1] = (uint8_t)(m_DegreeParS.saveRampageHige_Z2>>8);
            sendDataBuf[len+2] = (uint8_t)(m_DegreeParS.saveRampageHige_Z2>>16);
            sendDataBuf[len+3] = (uint8_t)(m_DegreeParS.saveRampageHige_Z2>>24);
            len += 4;
            sendDataBuf[len] = (uint8_t)m_DegreeParS.saveRotatePos_Y1;
            sendDataBuf[len+1] = (uint8_t)(m_DegreeParS.saveRotatePos_Y1>>8);
            sendDataBuf[len+2] = (uint8_t)(m_DegreeParS.saveRotatePos_Y1>>16);
            sendDataBuf[len+3] = (uint8_t)(m_DegreeParS.saveRotatePos_Y1>>24);
            len += 4;
            sendDataBuf[len] = (uint8_t)m_DegreeParS.saveRotatePos_Y2;
            sendDataBuf[len+1] = (uint8_t)(m_DegreeParS.saveRotatePos_Y2>>8);
            sendDataBuf[len+2] = (uint8_t)(m_DegreeParS.saveRotatePos_Y2>>16);
            sendDataBuf[len+3] = (uint8_t)(m_DegreeParS.saveRotatePos_Y2>>24);
            len += 4;
            break;
        default:
            break;
        }
    }
    else if(mainCmd == CMD_MAIN_MAC)
    {//机器参数
        switch(sunCmd)
        {
        case CMD_SUN_MAC_AXIS://轴参数
            sendDataBuf[0] = parNum;
            num = parNum - 1;
            len = 1;
            sendDataBuf[len] = (uint8_t)m_AxisPar[num].circlePluseNum;
            sendDataBuf[len+1] = (uint8_t)(m_AxisPar[num].circlePluseNum>>8);
            sendDataBuf[len+2] = (uint8_t)(m_AxisPar[num].circlePluseNum>>16);
            sendDataBuf[len+3] = (uint8_t)(m_AxisPar[num].circlePluseNum>>24);
            len += 4;
            sendDataBuf[len] = (uint8_t)m_AxisPar[num].circleDis;
            sendDataBuf[len+1] = (uint8_t)(m_AxisPar[num].circleDis>>8);
            sendDataBuf[len+2] = (uint8_t)(m_AxisPar[num].circleDis>>16);
            sendDataBuf[len+3] = (uint8_t)(m_AxisPar[num].circleDis>>24);
            len += 4;
            sendDataBuf[len] = (uint8_t)m_AxisPar[num].findOriginSpeed;
            sendDataBuf[len+1] = (uint8_t)(m_AxisPar[num].findOriginSpeed>>8);
            len += 2;
            sendDataBuf[len] = (uint8_t)m_AxisPar[num].leaveOriginSpeed;
            sendDataBuf[len+1] = (uint8_t)(m_AxisPar[num].leaveOriginSpeed>>8);
            len += 2;
            sendDataBuf[len] = (uint8_t)m_AxisPar[num].originOffset;
            sendDataBuf[len+1] = (uint8_t)(m_AxisPar[num].originOffset>>8);
            sendDataBuf[len+2] = (uint8_t)(m_AxisPar[num].originOffset>>16);
            sendDataBuf[len+3] = (uint8_t)(m_AxisPar[num].originOffset>>24);
            len += 4;
            sendDataBuf[len] = (uint8_t)m_AxisPar[num].originDir;
            len += 1;
            sendDataBuf[len] = (uint8_t)m_AxisPar[num].coordDir;
            len += 1;
            sendDataBuf[len] = (uint8_t)m_AxisPar[num].originSignal;
            len += 1;
            break;
        case CMD_SUN_MAC_LIMIT_SWT://限位开关
            for(i=0; i<AXIS_TOTAL_NUM; i++)
            {
                sendDataBuf[i*2] = m_AxisPar[i].limitPosSwt;
                sendDataBuf[i*2+1] = m_AxisPar[i].limitNegSwt;
            }
            len = AXIS_TOTAL_NUM * 2;
            break;
        case CMD_SUN_MAC_STRUCT://结构
            for(i=0; i<AXIS_TOTAL_NUM; i++)
            {
                sendDataBuf[i] = m_AxisPar[i].axisType;
            }
            len = AXIS_TOTAL_NUM;
            for(i=0; i<AXIS_TOTAL_NUM; i++)
            {
                sendDataBuf[i+len] = m_AxisPar[i].axisMoveMade;
            }
            len += AXIS_TOTAL_NUM;
            sendDataBuf[len] = m_ServoPar.encoderType;
            len += 1;
            sendDataBuf[len] = m_ServoPar.encoderBitNum;
            len += 1;
            sendDataBuf[len] = m_ServoPar.servoBrand;
            len += 1;
            sendDataBuf[len] = m_ServoPar.servoModel;
            len += 1;
            sendDataBuf[len] = m_ServoPar.extendIOBoard;
            len += 1;
            sendDataBuf[len] = m_ServoPar.rotateAxisUnit;
            len += 1;
            break;
        case CMD_SUN_MAC_SERVO://伺服参数
            sendDataBuf[0] = parNum;
            num = parNum - 1;
            len = 1;
            sendDataBuf[len] = m_ServoCommPar[num].axis;
            len += 1;
            sendDataBuf[len] = (uint8_t)m_ServoCommPar[num].index;
            sendDataBuf[len+1] = (uint8_t)(m_ServoCommPar[num].index>>8);
            len += 2;
            sendDataBuf[len] = m_ServoCommPar[num].sunIndex;
            len += 1;
            sendDataBuf[len] = m_ServoCommPar[num].parLen;
            len += 1;
            sendDataBuf[len] = (uint8_t)m_ServoCommPar[num].parValue;
            sendDataBuf[len+1] = (uint8_t)(m_ServoCommPar[num].parValue>>8);
            sendDataBuf[len+2] = (uint8_t)(m_ServoCommPar[num].parValue>>16);
            sendDataBuf[len+3] = (uint8_t)(m_ServoCommPar[num].parValue>>24);
            len += 1;
            sendDataBuf[len] = m_ServoCommPar[num].saveFlag;
            len += 1;
            sendDataBuf[len] = m_ServoCommPar[num].powerOnSet;
            len += 1;
            break;
        case CMD_SUN_MAC_ORIGIN://原点
            for(i=0; i<AXIS_TOTAL_NUM; i++)
            {
                sendDataBuf[i] = m_AxisPar[i].backOriginOrder;
            }
            len = AXIS_TOTAL_NUM;
            for(i=0; i<AXIS_TOTAL_NUM; i++)
            {
                sendDataBuf[i+len] = m_AxisPar[i].originType;
            }
            len += AXIS_TOTAL_NUM;
            sendDataBuf[len] = m_ServoPar.resetSunProFlag;
            len += 1;
            sendDataBuf[len] = m_ServoPar.torqueLimitFlag;
            len += 1;
            break;
        default:
            break;
        }
    }
    else if(mainCmd == CMD_MAIN_STACK)
    {//堆叠参数
        switch(sunCmd)
        {
        case CMD_SUN_STACK_PAR://堆叠基础参数
            sendDataBuf[0] = parNum;
            num = parNum - 1;
            len = 1;
            sendDataBuf[len] = m_StackInfo[num].stackOrder;
            len += 1;
            sendDataBuf[len] = m_StackInfo[num].countMode;
            len += 1;
            sendDataBuf[len] = m_StackInfo[num].stackFinStopFlag;
            len += 1;
            for(i=0; i<STACK_AXIS_NUM; i++)
            {
                sendDataBuf[len+i] = m_StackInfo[num].axisSelect[i];
            }
            len += STACK_AXIS_NUM;
            for(i=0; i<STACK_AXIS_NUM; i++)
            {
                sendDataBuf[len+i] = m_StackInfo[num].axisSpeed[i];
            }
            len += STACK_AXIS_NUM;
            for(i=0; i<STACK_AXIS_NUM; i++)
            {
                sendDataBuf[len+i] = m_StackInfo[num].stackPointNum[i];
            }
            len += STACK_AXIS_NUM;
            for(i=0; i<STACK_AXIS_NUM; i++)
            {
                sendDataBuf[len+i] = m_StackInfo[num].stackDir[i];
            }
            len += STACK_AXIS_NUM;
            for(i=0; i<STACK_AXIS_NUM; i++)
            {
                sendDataBuf[len+i] = m_StackInfo[num].dischangeSpeed[i];
            }
            len += STACK_AXIS_NUM;
            break;
        case CMD_SUN_STACK_POINT://堆叠点位参数
            sendDataBuf[0] = parNum;
            num = parNum - 1;
            len = 1;
            sendDataBuf[1] = parNum2;
            axis = parNum2 - 1;
            len += 1;
            sendDataBuf[len] = (uint8_t)m_StackInfo[num].stackStartPos[axis];
            sendDataBuf[len+1] = (uint8_t)(m_StackInfo[num].stackStartPos[axis]>>8);
            sendDataBuf[len+2] = (uint8_t)(m_StackInfo[num].stackStartPos[axis]>>16);
            sendDataBuf[len+3] = (uint8_t)(m_StackInfo[num].stackStartPos[axis]>>24);
            len += 4;
            sendDataBuf[len] = (uint8_t)m_StackInfo[num].stack_X_EndPos[axis];
            sendDataBuf[len+1] = (uint8_t)(m_StackInfo[num].stack_X_EndPos[axis]>>8);
            sendDataBuf[len+2] = (uint8_t)(m_StackInfo[num].stack_X_EndPos[axis]>>16);
            sendDataBuf[len+3] = (uint8_t)(m_StackInfo[num].stack_X_EndPos[axis]>>24);
            len += 4;
            sendDataBuf[len] = (uint8_t)m_StackInfo[num].stack_Y_EndPos[axis];
            sendDataBuf[len+1] = (uint8_t)(m_StackInfo[num].stack_Y_EndPos[axis]>>8);
            sendDataBuf[len+2] = (uint8_t)(m_StackInfo[num].stack_Y_EndPos[axis]>>16);
            sendDataBuf[len+3] = (uint8_t)(m_StackInfo[num].stack_Y_EndPos[axis]>>24);
            len += 4;
            sendDataBuf[len] = (uint8_t)m_StackInfo[num].stackDiagonalPos[axis];
            sendDataBuf[len+1] = (uint8_t)(m_StackInfo[num].stackDiagonalPos[axis]>>8);
            sendDataBuf[len+2] = (uint8_t)(m_StackInfo[num].stackDiagonalPos[axis]>>16);
            sendDataBuf[len+3] = (uint8_t)(m_StackInfo[num].stackDiagonalPos[axis]>>24);
            len += 4;
            sendDataBuf[len] = (uint8_t)m_StackInfo[num].dischangeStartPos[axis];
            sendDataBuf[len+1] = (uint8_t)(m_StackInfo[num].dischangeStartPos[axis]>>8);
            sendDataBuf[len+2] = (uint8_t)(m_StackInfo[num].dischangeStartPos[axis]>>16);
            sendDataBuf[len+3] = (uint8_t)(m_StackInfo[num].dischangeStartPos[axis]>>24);
            len += 4;
            sendDataBuf[len] = (uint8_t)m_StackInfo[num].intevalDis[axis];
            sendDataBuf[len+1] = (uint8_t)(m_StackInfo[num].intevalDis[axis]>>8);
            sendDataBuf[len+2] = (uint8_t)(m_StackInfo[num].intevalDis[axis]>>16);
            sendDataBuf[len+3] = (uint8_t)(m_StackInfo[num].intevalDis[axis]>>24);
            len += 4;
            sendDataBuf[len] = (uint8_t)m_StackInfo[num].offsetDis[axis];
            sendDataBuf[len+1] = (uint8_t)(m_StackInfo[num].offsetDis[axis]>>8);
            sendDataBuf[len+2] = (uint8_t)(m_StackInfo[num].offsetDis[axis]>>16);
            sendDataBuf[len+3] = (uint8_t)(m_StackInfo[num].offsetDis[axis]>>24);
            len += 4;
            break;
        case CMD_SUN_STACK_SET://堆叠设置
            sendDataBuf[0] = m_StackFunc.stackType;
            len = 1;
            sendDataBuf[len] = m_StackFunc.groupStack;
            len += 1;
            sendDataBuf[len] = m_StackFunc.rowStack;
            len += 1;
            sendDataBuf[len] = m_StackFunc.storeyStack;
            len += 1;
            sendDataBuf[len] = m_StackFunc.divideStack;
            len += 1;
            sendDataBuf[len] = m_StackFunc.stackMemory;
            len += 1;
            sendDataBuf[len] = m_StackFunc.siloType;
            len += 1;
            sendDataBuf[len] = m_StackFunc.rotateSiloFinHint;
            len += 1;
            sendDataBuf[len] = m_StackFunc.rotateSiloPlaceNum;
            len += 1;
            for(i=0; i<STACK_TOTAL_NUM; i++)
            {
                sendDataBuf[len+i] = m_StackInfo[i].groupLeaveBlank;
            }
            len += STACK_TOTAL_NUM;
            for(i=0; i<STACK_TOTAL_NUM; i++)
            {
                sendDataBuf[len+i] = m_StackInfo[i].stackMoveMode;
            }
            len += STACK_TOTAL_NUM;
            break;
        default:
            break;
        }
    }
    else if(mainCmd == CMD_MAIN_READ)
    {
        if(sunCmd == CMD_SUN_SYSDATA_FINISH)
        {
            sendDataBuf[0]=0;
        }

    }
    else if(mainCmd == CMD_MAIN_STA)
    {
        if(sunCmd == CMD_SUN_STA_PAR)
        {
            len = 0;
            sendDataBuf[len] = (uint8_t)m_RunPar.planProductNum;
            sendDataBuf[len+1] = (uint8_t)(m_RunPar.planProductNum>>8);
            sendDataBuf[len+2] = (uint8_t)(m_RunPar.planProductNum>>16);
            sendDataBuf[len+3] = (uint8_t)(m_RunPar.planProductNum>>24);
            len+=4;
            sendDataBuf[len] = m_RunPar.globalSpeed;
            len+=1;
            sendDataBuf[len] = m_RunPar.breakPointFlag;
            len+=1;
            sendDataBuf[len] = m_RunPar.breakPointProNum;
            len+=1;
            sendDataBuf[len] = (uint8_t)m_RunPar.breakPointList;
            sendDataBuf[len+1] = (uint8_t)(m_RunPar.breakPointList>>8);
            len+=1;
        }
    }

    ExtendSendParProReadAnswer(mainCmd, sunCmd, sendDataBuf, len);
}

//解析——参数
void Usart::ExtendReadParDeal(char mainCmd, char sunCmd, const QByteArray &recDataBuf1,int dataLen)
{
    uint8_t *recDataBuf=new uint8_t[dataLen];
    for(int i=0;i<dataLen;i++)
    {
        recDataBuf[i]=recDataBuf1[i];
    }
    if(mainCmd ==CMD_MAIN_READ)
    {
        if(sunCmd == CMD_SUN_READ_PAR_WR)
        {
            switch (recDataBuf[0]) {
            case CMD_MAIN_SIGNAL:
                switch (recDataBuf[1]) {
                case CMD_SUN_SIGNAL_OUT_TYPE:
                    if(MySync_Data.SysDataFlag == 1)
                    {
                        MySync_Data.out_type_State = true;
                    }
                    break;
                case CMD_SUN_SIGNAL_INTERLOCK://互锁设置
                    if(MySync_Data.SysDataFlag == 1)
                    {
                        MySync_Data.interlock_State = true;

                    }
                    break;
                case CMD_SUN_SIGNAL_IN_FUNC_DEF://输入端口自定义
                    if(MySync_Data.SysDataFlag == 1)
                    {
                        MySync_Data.in_func_def_State = true;
                    }

                    break;
                case CMD_SUN_SIGNAL_OUT_FUNC_DEF:
                    if(MySync_Data.SysDataFlag == 1)
                    {
                        MySync_Data.out_func_def_State = true;
                    }

                    break;
                case CMD_SUN_SIGNAL_OUT_RELEVENCY:
                    if(MySync_Data.SysDataFlag == 1)
                    {
                        MySync_Data.out_relevency_State = true;
                    }
                    break;
                case CMD_SUN_SIGNAL_RELATE_OUT:
                    if(MySync_Data.SysDataFlag == 1)
                    {
                        MySync_Data.out_relate_out = true;
                    }
                    break;
                case CMD_SUN_SIGNAL_KEY:
                    if(MySync_Data.SysDataFlag == 1)
                    {
                        MySync_Data.out_key = true;
                    }
                    break;
                case CMD_SUN_SIGNAL_SENIOR:
                    if(MySync_Data.SysDataFlag == 1)
                    {
                        MySync_Data.senior = true;
                    }
                    break;
                case CMD_SUN_SIGNAL_SENIOR_PORT:
                    if(MySync_Data.SysDataFlag == 1)
                    {
                        MySync_Data.senior_port = true;
                    }
                    break;
                default:
                    break;
                }
                break;
            case CMD_MAIN_SAVE:
                switch (recDataBuf[1]) {
                case CMD_SUN_SAVE_MACHINE:
                    if(MySync_Data.SysDataFlag == 1)
                    {
                        MySync_Data.save_machine = true;
                    }
                    break;
                case CMD_SUN_SAVE_STACK:
                    if(MySync_Data.SysDataFlag == 1)
                    {
                        MySync_Data.save_stack = true;
                    }
                    break;
                case CMD_SUN_SAVE_CALW:
                    if(MySync_Data.SysDataFlag == 1)
                    {
                        MySync_Data.save_calw++;
                    }
                    break;
                case CMD_SUN_SAVE_ONLINE:
                    if(MySync_Data.SysDataFlag == 1)
                    {
                        MySync_Data.save_online++;
                    }
                    break;
                default:
                    break;
                }
                break;
            case CMD_MAIN_PRODUCT:
                switch (recDataBuf[1]) {
                case CMD_SUN_PRODUCT_PAR:
                    if(MySync_Data.SysDataFlag == 1)
                    {
                        MySync_Data.product_par = true;
                    }
                    break;
                case CMD_SUN_PRODUCT_SENIOR:
                    if(MySync_Data.SysDataFlag == 1)
                    {
                        MySync_Data.product_senior = true;
                    }
                    break;
                case CMD_SUN_PRODUCT_INTERNET:
                    if(MySync_Data.SysDataFlag == 1)
                    {
                        MySync_Data.product_internet = true;
                    }
                    break;
                default:
                    break;
                }
                break;
            case CMD_MAIN_SERVO:
                switch (recDataBuf[1]) {
                case CMD_SUN_SERVO_ACC_DEC:
                    if(MySync_Data.SysDataFlag == 1)
                    {
                        MySync_Data.servo_acc_dec++;
                    }
                    break;
                case CMD_SUN_SERVO_MAX_SPEED:
                    if(MySync_Data.SysDataFlag == 1)
                    {
                        MySync_Data.servo_max_speed = true;
                    }
                    break;
                case CMD_SUN_SERVO_TOLERANCE:
                    if(MySync_Data.SysDataFlag == 1)
                    {
                        MySync_Data.servo_tolerance = true;
                    }
                    break;
                default:
                    break;
                }
                break;
            case CMD_MAIN_SP:
                switch (recDataBuf[1]) {
                case CMD_SUN_SP_AREA:
                    if(MySync_Data.SysDataFlag == 1)
                    {
                        MySync_Data.sp_area++;
                    }
                    break;
                case CMD_SUN_SP_AXIS_LIMIT:
                    if(MySync_Data.SysDataFlag == 1)
                    {
                        MySync_Data.sp_axis_limit++;
                    }
                    break;
                case CMD_SUN_SP_RAMPAGE_LIMIT:
                    if(MySync_Data.SysDataFlag == 1)
                    {
                        MySync_Data.sp_rampage_limit = true;
                    }
                    break;
                default:
                    break;
                }
                break;
            case CMD_MAIN_MAC:
                switch (recDataBuf[1]) {
                case CMD_SUN_MAC_AXIS:
                    if(MySync_Data.SysDataFlag == 1)
                    {
                        MySync_Data.mac_aixs++;

                    }
                    break;
                case CMD_SUN_MAC_LIMIT_SWT:
                    if(MySync_Data.SysDataFlag == 1)
                    {
                        MySync_Data.mac_limit_swt = true;
                    }
                    break;
                case CMD_SUN_MAC_STRUCT:
                    if(MySync_Data.SysDataFlag == 1)
                    {
                        MySync_Data.mac_struct = true;
                    }
                    break;
                case CMD_SUN_MAC_SERVO:
                    if(MySync_Data.SysDataFlag == 1)
                    {
                        MySync_Data.mac_servo++;
                    }
                    break;
                case CMD_SUN_MAC_ORIGIN:
                    if(MySync_Data.SysDataFlag == 1)
                    {
                        MySync_Data.mac_origin = true;

                    }
                    break;
                default:
                    break;
                }
                break;
            case CMD_MAIN_STACK:
                switch (recDataBuf[1]) {
                case CMD_SUN_STACK_PAR:
                    if(MySync_Data.SysDataFlag == 1)
                    {
                        MySync_Data.stack_par++;
                    }
                    break;
                case CMD_SUN_STACK_POINT:
                    if(MySync_Data.SysDataFlag == 1)
                    {
                        MySync_Data.stack_point++;
                    }
                    break;
                case CMD_SUN_STACK_SET:
                    if(MySync_Data.SysDataFlag == 1)
                    {
                        MySync_Data.stack_set = true;
                    }
                    break;

                default:
                    break;
                }
                break;
            default:
                break;
            }
        }
        else if(sunCmd == CMD_SUN_SYSDATA_FINISH)
        {
            if(MySync_Data.SysDataFlag == 1)
            {
                MySync_Data.sysdatafinish = true;
            }
        }
    }
    ExtendReadStaDeal(mainCmd,sunCmd,recDataBuf);
}

//解析——状态处理函数
void Usart::ExtendReadStaDeal(uint8_t mainCmd, uint8_t sunCmd, uint8_t *recDataBuf)
{
    uint16_t i = 0;
    uint8_t ret = 0;
    uint8_t errCode = 0;
    uint16_t index = 0;
    int32_t AxisCurPos = 0;
    if(mainCmd == CMD_MAIN_STA)
    {//运行状态及实时位置
        switch(sunCmd)
        {
        case CMD_SUN_STA_IN_OUT://输入输出状态读取
            for(i=0;i<8;i++)
            {
                if(i==7)
                {
                    for(int j=0;j<4;j++)
                    {
                        m_InPortSta[i*8+j]=recDataBuf[i]>>(7-j) & 1;
                    }
                }
                else
                {
                    for(int j=0;j<8;j++)
                    {
                        m_InPortSta[i*8+j]=recDataBuf[i]>>(7-j) & 1;
                    }
                }

            }
            for(i=8;i<=13;i++)
            {
                for(int j=0;j<8;j++)
                {
                    m_OutPortSta[(i-8)*8+j]=recDataBuf[i]>>(7-j) & 1;
                }
            }
            break;
        case CMD_SUN_STA_AXIS_POS://轴当前位置读取
            index=0;
            for(i=0;i<AXIS_TOTAL_NUM;i++)
            {
                index=i*4;
                AxisCurPos = (int32_t)(((uint32_t)recDataBuf[index])  |((uint32_t)recDataBuf[index+1]<<8)  |((uint32_t)recDataBuf[index+2]<<16)  |((uint32_t)recDataBuf[index+3]<<24));
                switch (i) {
                case X1_AXIS:
                    m_AxisCurPos.Pos_x = AxisCurPos;
                    break;
                case Y1_AXIS:
                    m_AxisCurPos.Pos_y = AxisCurPos;
                    break;
                case Z1_AXIS:
                    m_AxisCurPos.Pos_z = AxisCurPos;
                    break;
                case C_AXIS:
                    m_AxisCurPos.Pos_c = AxisCurPos;
                    break;
                case Y2_AXIS:
                    m_AxisCurPos.Pos_y2 = AxisCurPos;
                    break;
                case Z2_AXIS:
                    m_AxisCurPos.Pos_z2 = AxisCurPos;
                    break;
                default:
                    break;
                }

            }
            emit posflashsignal(m_AxisCurPos);
            index=24;
            m_AxisCurSpeed=(uint16_t)recDataBuf[index] + ((uint16_t)recDataBuf[index+1]<<8);
            index=26;
            m_AxisCurTorque=(uint16_t)recDataBuf[index] + ((uint16_t)recDataBuf[index+1]<<8);

            break;
        case CMD_SUN_STA_MAC://控制器状态读取
            index = 0;
            m_RobotRunSta = (uint8_t)recDataBuf[index];//机器状态
            index = 1;
            m_AlarmNum = (uint16_t)recDataBuf[index] + ((uint16_t)recDataBuf[index+1]<<8);//报警编号
            index = 3;
            m_RobotResetState = (uint8_t)recDataBuf[index];//复位状态
            index = 4;
            m_ProRunInfo.proNum[0] = ((uint16_t)recDataBuf[index])  | ((uint16_t)recDataBuf[index+1]<<8);
            index = 6;
            for(i=1; i<PRO_NUM; i++)
            {
                m_ProRunInfo.proNum[i] = (uint8_t)recDataBuf[index + i - 1];
            }
            emit robotstaRefreshsignal();
            break;
        case CMD_SUN_STA_INFO://运行信息读取
            index = 0;
            m_RunInfo.runTime = (uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
            index = 4;
            m_RunInfo.preShootCyc = (uint16_t)recDataBuf[index] + ((uint16_t)recDataBuf[index+1]<<8);
            index = 6;
            m_RunInfo.takeShapeCyc = (uint16_t)recDataBuf[index] + ((uint16_t)recDataBuf[index+1]<<8);
            index = 8;
            m_RunInfo.fetchTime = (uint16_t)recDataBuf[index] + ((uint16_t)recDataBuf[index+1]<<8);
            index = 10;
            m_RunInfo.actualProductNum = (uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
            index += 4;
            break;
//        case CMD_SUN_STA_PAR://运行参数读写
//            index = 0;
//            m_RunPar.planProductNum = (uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
//            index += 4;
//            m_RunPar.globalSpeed = recDataBuf[index];
//            index += 1;
//            m_RunPar.breakPointFlag = recDataBuf[index];
//            index += 1;
//            m_RunPar.breakPointProNum = recDataBuf[index];
//            index += 1;
//            m_RunPar.breakPointList = (uint16_t)recDataBuf[index] + ((uint16_t)recDataBuf[index+1]<<8);
//            index += 2;
//            break;
        case CMD_SUN_STA_STACK://堆叠实时参数读写
            for(i=0; i<STACK_TOTAL_NUM; i++)
            {
                m_StackCurPileCnt[i] = (uint16_t)recDataBuf[i*2] + ((uint16_t)recDataBuf[i*2+1]<<8);
            }
            index = STACK_TOTAL_NUM * 2;
            break;
        case CMD_SUN_STA_VAR://变量值读写
            for(i=0; i<VAR_TOTAL_NUM; i++)
            {
                m_VariableCurValue[i] = (uint32_t)recDataBuf[i*4] + ((uint32_t)recDataBuf[i*4+1]<<8) + ((uint32_t)recDataBuf[i*4+2]<<16) + ((uint32_t)recDataBuf[i*4+3]<<24);
            }
            index = VAR_TOTAL_NUM * 4;
            break;
        case CMD_SUN_STA_TIME://定时器读写
            for(i=0; i<TIME_TOTAL_NUM; i++)
            {
                m_TimeCurValue[i] = (uint32_t)recDataBuf[i*4] + ((uint32_t)recDataBuf[i*4+1]<<8) + ((uint32_t)recDataBuf[i*4+2]<<16) + ((uint32_t)recDataBuf[i*4+3]<<24);
            }
            index = TIME_TOTAL_NUM * 4;
            break;
        default:
            break;
        }
    }

    //刷新实时参数读取（发射相关信号）

}

//下发——程序编辑处理
void Usart::ExtendSendProDeal(uint8_t mainCmd, uint8_t sunCmd, uint16_t parNum, uint16_t parNum2, uint16_t parNum3)
{
    uint8_t sendDataBuf[EXTEND_BUF_LEN] = {0};
    uint16_t len=0;
    int index=0;
    if(mainCmd == CMD_MAIN_PRO)
    {
        switch(sunCmd)
        {
        case CMD_SUN_PRO_INFO://程序信息读写
            sendDataBuf[index++]=(uint8_t)m_ProInfo.proNum[0];
            sendDataBuf[index++] = (uint8_t)(m_ProInfo.proNum[0]>>8);
            for(int i=1; i<PRO_NUM; i++)
            {
                sendDataBuf[index++]=(uint8_t)m_ProInfo.proNum[i];
            }
            len = index;
            break;
        case CMD_SUN_PRO_REF://参考点读写
            sendDataBuf[index++]=(uint8_t)(parNum+1);
            sendDataBuf[index++]=m_RefPoint[parNum].refFlag;
            for(int i=0; i<AXIS_TOTAL_NUM; i++)
            {
                sendDataBuf[index++] = (uint8_t)m_RefPoint[parNum].pos[i];
                sendDataBuf[index++] = (uint8_t)(m_RefPoint[parNum].pos[i]>>8);
                sendDataBuf[index++] = (uint8_t)(m_RefPoint[parNum].pos[i]>>16);
                sendDataBuf[index++] = (uint8_t)(m_RefPoint[parNum].pos[i]>>24);
            }
            len = index;
            break;
        case CMD_SUN_PRO_ORDER://命令行读写
            GetProData(parNum, parNum2, sendDataBuf, len,PRO_FLAG_CMDLINE_03);
            break;
        case CMD_SUN_PRO_INSERT://命令行插入、修改
            GetProData1(parNum, parNum2, sendDataBuf, len,PRO_FLAG_CMDLINE_04,parNum3);
            break;
        case CMD_SUN_PRO_DELET://命令行删除、分解、组合、上移、下移、屏蔽、取消屏蔽
            sendDataBuf[index++] = (uint8_t)parNum;
            sendDataBuf[index++]=(uint8_t)m_OperateProOrder[m_CurrentSelectProOrderList].list;
            sendDataBuf[index++] = (uint8_t)(m_OperateProOrder[m_CurrentSelectProOrderList].list>>8);
            sendDataBuf[index++] = (uint8_t)parNum2;
            sendDataBuf[index++] = (uint8_t)parNum3;
            sendDataBuf[index++] = (uint8_t)(parNum3>>8);
            len = index;
            break;
        case CMD_SUN_PRO_SPEED://程序轴速度教导
            sendDataBuf[index++] = (uint8_t)parNum;//程序号
            sendDataBuf[index++] = (uint8_t)parNum2;//轴编号
            sendDataBuf[index++] = (uint8_t)parNum3;//速度
            sendDataBuf[index++] = (uint8_t)(parNum3>>8);
            len = index;
            break;
        case CMD_SUN_PRO_OUT://程序信号输出
            sendDataBuf[index++] = (uint8_t)parNum;
            sendDataBuf[index++] = (uint8_t)parNum2;
            len = index;
            break;
        case CMD_SUN_PRO_DEBUG://程序试行控制
            sendDataBuf[index++] = (uint8_t)parNum;
            sendDataBuf[index++] = (uint8_t)parNum2;
            sendDataBuf[index++] = (uint8_t)(parNum2>>8);
            sendDataBuf[index++] = (uint8_t)parNum3;
            sendDataBuf[index++] = m_RunPar.globalSpeed;//全局速度，在自动运行界面设置
            len = index;
            break;
        case CMD_SUN_PRO_START://程序启停控制
            sendDataBuf[index++] = (uint8_t)parNum;
            sendDataBuf[index++] = (uint8_t)parNum2;
            sendDataBuf[index++] = (uint8_t)(parNum2>>8);
            sendDataBuf[index++] = (uint8_t)parNum3;
            len = index;
            break;
        case CMD_SUN_PRO_MODE://设备模式切换
            sendDataBuf[index++] = (uint8_t)parNum;
            len = index;
            break;
        case CMD_SUN_PRO_SAVE://保存、切换操作的程序
            sendDataBuf[index++] = (uint8_t)parNum;
            sendDataBuf[index++] = (uint8_t)parNum2;
            len = index;
            break;
        case CMD_SUN_STA_VAR_TYPE:
            len=0;
            sendDataBuf[len]= (uint8_t)parNum;
            for(int i=0;i<VAR_TOTAL_NUM;i++)
            {
                sendDataBuf[i+1] = m_VariableType[i];
                len+=1;
            }
        default:
            break;

        }
    }
    ExtendSendParProReadAnswer(mainCmd, sunCmd, sendDataBuf, len);

}


//写入程序命令内容----外部调用
/**********************************************************
**  函数名：  GetProData()
**	输入参数：
**          parNum:参数1——程序号
**          parNum2:参数2——第几条程序命令
**          sendDataBuf:整个数据帧
**          len:数据长度
**          cmdLineType:程序命令类型
**          operateType:操作类型:
**                  程序命令行插入、停止修改、自动修改: 0-插入，1-停止修改，2-自动修改
**	输出参数：
**	函数功能：程序下发组合函数
**	返回：	无
***********************************************************/
void Usart::GetProData1(uint16_t parNum, uint16_t parNum2, uint8_t* sendDataBuf, uint16_t &len, uint16_t cmdLineType, uint16_t operateType)
{
    sendDataBuf[EXTEND_BUF_LEN] = {0};
    int index = 0;
    switch (cmdLineType)
    {
    case PRO_FLAG_CMDLINE_03:
        break;
    case PRO_FLAG_CMDLINE_04:
        sendDataBuf[index++] = (uint8_t)operateType;
        break;
    default:
        break;
    }
    sendDataBuf[index++] = (uint8_t)parNum;

    sendDataBuf[index++]=(uint8_t)m_OperateProOrder[parNum2].list;
    sendDataBuf[index++] = (uint8_t)(m_OperateProOrder[parNum2].list>>8);
    sendDataBuf[index++]=(uint8_t)m_OperateProOrder[parNum2].runOrderNum;
    sendDataBuf[index++] = (uint8_t)(m_OperateProOrder[parNum2].runOrderNum>>8);
    sendDataBuf[index++]=(uint8_t)m_OperateProOrder[parNum2].cmd;
    sendDataBuf[index++]=(uint8_t)m_OperateProOrder[parNum2].noteFlag;
    sendDataBuf[index++]=(uint8_t)m_OperateProOrder[parNum2].delay;
    sendDataBuf[index++] = (uint8_t)(m_OperateProOrder[parNum2].delay>>8);

    switch (m_OperateProOrder[parNum2].cmd)
    {
    case C_AXIS_MOVE:
        sendDataBuf[index++] = (uint8_t)((P_AxisMoveStruct*)m_OperateProOrder[parNum2].pData)->pos;
        sendDataBuf[index++] = (uint8_t)(((P_AxisMoveStruct*)m_OperateProOrder[parNum2].pData)->pos>>8);
        sendDataBuf[index++] = (uint8_t)(((P_AxisMoveStruct*)m_OperateProOrder[parNum2].pData)->pos>>16);
        sendDataBuf[index++] = (uint8_t)(((P_AxisMoveStruct*)m_OperateProOrder[parNum2].pData)->pos>>24);
        sendDataBuf[index++] = (uint8_t)((P_AxisMoveStruct*)m_OperateProOrder[parNum2].pData)->advEndDis;
        sendDataBuf[index++] = (uint8_t)(((P_AxisMoveStruct*)m_OperateProOrder[parNum2].pData)->advEndDis>>8);
        sendDataBuf[index++] = (uint8_t)(((P_AxisMoveStruct*)m_OperateProOrder[parNum2].pData)->advEndDis>>16);
        sendDataBuf[index++] = (uint8_t)(((P_AxisMoveStruct*)m_OperateProOrder[parNum2].pData)->advEndDis>>24);
        sendDataBuf[index++] = (uint8_t)((P_AxisMoveStruct*)m_OperateProOrder[parNum2].pData)->advCSpeedDis;
        sendDataBuf[index++] = (uint8_t)(((P_AxisMoveStruct*)m_OperateProOrder[parNum2].pData)->advCSpeedDis>>8);
        sendDataBuf[index++] = (uint8_t)(((P_AxisMoveStruct*)m_OperateProOrder[parNum2].pData)->advCSpeedDis>>16);
        sendDataBuf[index++] = (uint8_t)(((P_AxisMoveStruct*)m_OperateProOrder[parNum2].pData)->advCSpeedDis>>24);
        sendDataBuf[index++] = ((P_AxisMoveStruct*)m_OperateProOrder[parNum2].pData)->axis;
        sendDataBuf[index++] = ((P_AxisMoveStruct*)m_OperateProOrder[parNum2].pData)->speed;
        sendDataBuf[index++] = ((P_AxisMoveStruct*)m_OperateProOrder[parNum2].pData)->advEndFlag;
        sendDataBuf[index++] = ((P_AxisMoveStruct*)m_OperateProOrder[parNum2].pData)->advCSpeedFlag;
        sendDataBuf[index++] = ((P_AxisMoveStruct*)m_OperateProOrder[parNum2].pData)->advCSpeedSpeed;
        sendDataBuf[index++] = ((P_AxisMoveStruct*)m_OperateProOrder[parNum2].pData)->referPointNum;
        for(int m=0;m<2;m++)
        {
            sendDataBuf[index++] = ((P_AxisMoveStruct*)m_OperateProOrder[parNum2].pData)->ret[m];
        }
        len = index;
        break;
    case C_CLAW_ACTION:
        sendDataBuf[index++] = ((P_ClawActionStruct*)m_OperateProOrder[parNum2].pData)->outportNum;
        sendDataBuf[index++] = ((P_ClawActionStruct*)m_OperateProOrder[parNum2].pData)->type;
        for(int m=0;m<2;m++)
        {
            sendDataBuf[index++] = ((P_ClawActionStruct*)m_OperateProOrder[parNum2].pData)->ret[m];
        }
        len = index;
        break;
    case C_CLAW_CHECK:
        sendDataBuf[index++] = ((P_ClawCheckStruct*)m_OperateProOrder[parNum2].pData)->outportNum;
        sendDataBuf[index++] = ((P_ClawCheckStruct*)m_OperateProOrder[parNum2].pData)->type;
        for(int m=0;m<2;m++)
        {
            sendDataBuf[index++] = ((P_ClawCheckStruct*)m_OperateProOrder[parNum2].pData)->ret[m];
        }
        len = index;
        break;
    case C_RESERVE_CHECK:
        sendDataBuf[index++] = ((P_ReserveCheckStruct*)m_OperateProOrder[parNum2].pData)->inportNum;
        sendDataBuf[index++] = ((P_ReserveCheckStruct*)m_OperateProOrder[parNum2].pData)->type;
        for(int m=0;m<2;m++)
        {
            sendDataBuf[index++] = ((P_ReserveCheckStruct*)m_OperateProOrder[parNum2].pData)->ret[m];
        }
        len = index;
        break;
    case C_MACHINE_OUT:
        sendDataBuf[index++] = ((P_MachineOutStruct*)m_OperateProOrder[parNum2].pData)->outportNum;
        sendDataBuf[index++] = ((P_MachineOutStruct*)m_OperateProOrder[parNum2].pData)->type;
        for(int m=0;m<2;m++)
        {
            sendDataBuf[index++] = ((P_MachineOutStruct*)m_OperateProOrder[parNum2].pData)->ret[m];
        }
        len = index;
        break;
    case C_STACK_RESET_ZERO:
        sendDataBuf[index++]=(uint8_t)((P_StackResetZeroStruct*)m_OperateProOrder[parNum2].pData)->number;
        sendDataBuf[index++] = (uint8_t)(((P_StackResetZeroStruct*)m_OperateProOrder[parNum2].pData)->number>>8);
        sendDataBuf[index++] = ((P_StackResetZeroStruct*)m_OperateProOrder[parNum2].pData)->stackNum;
        for(int m=0;m<1;m++)
        {
            sendDataBuf[index++] = ((P_StackResetZeroStruct*)m_OperateProOrder[parNum2].pData)->ret[m];
        }
        len = index;
        break;
    case C_STACK_MOVE:
        for(int m=0;m<STACK_AXIS_NUM;m++)
        {
            sendDataBuf[index++] = (uint8_t)((P_StackMoveStruct*)m_OperateProOrder[parNum2].pData)->advEndDis[m];
            sendDataBuf[index++] = (uint8_t)(((P_StackMoveStruct*)m_OperateProOrder[parNum2].pData)->advEndDis[m]>>8);
            sendDataBuf[index++] = (uint8_t)(((P_StackMoveStruct*)m_OperateProOrder[parNum2].pData)->advEndDis[m]>>16);
            sendDataBuf[index++] = (uint8_t)(((P_StackMoveStruct*)m_OperateProOrder[parNum2].pData)->advEndDis[m]>>24);
        }
        for(int m=0;m<STACK_AXIS_NUM;m++)
        {
            sendDataBuf[index++] = (uint8_t)((P_StackMoveStruct*)m_OperateProOrder[parNum2].pData)->advCSpeedDis[m];
            sendDataBuf[index++] = (uint8_t)(((P_StackMoveStruct*)m_OperateProOrder[parNum2].pData)->advCSpeedDis[m]>>8);
            sendDataBuf[index++] = (uint8_t)(((P_StackMoveStruct*)m_OperateProOrder[parNum2].pData)->advCSpeedDis[m]>>16);
            sendDataBuf[index++] = (uint8_t)(((P_StackMoveStruct*)m_OperateProOrder[parNum2].pData)->advCSpeedDis[m]>>24);
        }
        sendDataBuf[index++] = ((P_StackMoveStruct*)m_OperateProOrder[parNum2].pData)->stackNum;
        for(int m=0;m<3;m++)
        {
            sendDataBuf[index++] = ((P_StackMoveStruct*)m_OperateProOrder[parNum2].pData)->ret1[m];
        }
        for(int m=0;m<STACK_AXIS_NUM;m++)
        {
            sendDataBuf[index++] = ((P_StackMoveStruct*)m_OperateProOrder[parNum2].pData)->advEndFlag[m];
        }
        for(int m=0;m<1;m++)
        {
            sendDataBuf[index++] = ((P_StackMoveStruct*)m_OperateProOrder[parNum2].pData)->ret2[m];
        }
        for(int m=0;m<STACK_AXIS_NUM;m++)
        {
            sendDataBuf[index++] = ((P_StackMoveStruct*)m_OperateProOrder[parNum2].pData)->advCSpeedFlag[m];
        }
        for(int m=0;m<1;m++)
        {
            sendDataBuf[index++] = ((P_StackMoveStruct*)m_OperateProOrder[parNum2].pData)->ret3[m];
        }
        for(int m=0;m<STACK_AXIS_NUM;m++)
        {
            sendDataBuf[index++] = ((P_StackMoveStruct*)m_OperateProOrder[parNum2].pData)->advCSpeedSpeed[m];
        }
        for(int m=0;m<1;m++)
        {
            sendDataBuf[index++] = ((P_StackMoveStruct*)m_OperateProOrder[parNum2].pData)->ret4[m];
        }
        len = index;
        break;
    case C_STACK_FOLLOW:
        for(int m=0;m<STACK_AXIS_NUM;m++)
        {
            sendDataBuf[index++] = (uint8_t)((P_StackFollowStruct*)m_OperateProOrder[parNum2].pData)->advEndDis[m];
            sendDataBuf[index++] = (uint8_t)(((P_StackFollowStruct*)m_OperateProOrder[parNum2].pData)->advEndDis[m]>>8);
            sendDataBuf[index++] = (uint8_t)(((P_StackFollowStruct*)m_OperateProOrder[parNum2].pData)->advEndDis[m]>>16);
            sendDataBuf[index++] = (uint8_t)(((P_StackFollowStruct*)m_OperateProOrder[parNum2].pData)->advEndDis[m]>>24);
        }
        for(int m=0;m<STACK_AXIS_NUM;m++)
        {
            sendDataBuf[index++] = (uint8_t)((P_StackFollowStruct*)m_OperateProOrder[parNum2].pData)->advCSpeedDis[m];
            sendDataBuf[index++] = (uint8_t)(((P_StackFollowStruct*)m_OperateProOrder[parNum2].pData)->advCSpeedDis[m]>>8);
            sendDataBuf[index++] = (uint8_t)(((P_StackFollowStruct*)m_OperateProOrder[parNum2].pData)->advCSpeedDis[m]>>16);
            sendDataBuf[index++] = (uint8_t)(((P_StackFollowStruct*)m_OperateProOrder[parNum2].pData)->advCSpeedDis[m]>>24);
        }
        sendDataBuf[index++] = ((P_StackFollowStruct*)m_OperateProOrder[parNum2].pData)->stackNum;
        for(int m=0;m<3;m++)
        {
            sendDataBuf[index++] = ((P_StackFollowStruct*)m_OperateProOrder[parNum2].pData)->ret1[m];
        }
        for(int m=0;m<STACK_AXIS_NUM;m++)
        {
            sendDataBuf[index++] = ((P_StackFollowStruct*)m_OperateProOrder[parNum2].pData)->advEndFlag[m];
        }
        for(int m=0;m<1;m++)
        {
            sendDataBuf[index++] = ((P_StackFollowStruct*)m_OperateProOrder[parNum2].pData)->ret2[m];
        }
        for(int m=0;m<STACK_AXIS_NUM;m++)
        {
            sendDataBuf[index++] = ((P_StackFollowStruct*)m_OperateProOrder[parNum2].pData)->advCSpeedFlag[m];
        }
        for(int m=0;m<1;m++)
        {
            sendDataBuf[index++] = ((P_StackFollowStruct*)m_OperateProOrder[parNum2].pData)->ret3[m];
        }
        for(int m=0;m<STACK_AXIS_NUM;m++)
        {
            sendDataBuf[index++] = ((P_StackFollowStruct*)m_OperateProOrder[parNum2].pData)->advCSpeedSpeed[m];
        }
        for(int m=0;m<1;m++)
        {
            sendDataBuf[index++] = ((P_StackFollowStruct*)m_OperateProOrder[parNum2].pData)->ret4[m];
        }
        len = index;
        break;
    case C_RESERVE_OUT:
        sendDataBuf[index++]=(uint8_t)((P_ReserveOutStruct*)m_OperateProOrder[parNum2].pData)->interval;
        sendDataBuf[index++] = (uint8_t)(((P_ReserveOutStruct*)m_OperateProOrder[parNum2].pData)->interval>>8);
        sendDataBuf[index++] = ((P_ReserveOutStruct*)m_OperateProOrder[parNum2].pData)->outportNum;
        sendDataBuf[index++] = ((P_ReserveOutStruct*)m_OperateProOrder[parNum2].pData)->function;
        sendDataBuf[index++] = ((P_ReserveOutStruct*)m_OperateProOrder[parNum2].pData)->type;
        for(int m=0;m<3;m++)
        {
            sendDataBuf[index++] = ((P_ReserveOutStruct*)m_OperateProOrder[parNum2].pData)->ret[m];
        }
        len = index;
        break;
    case C_WAIT_IN_MACHINE:
        sendDataBuf[index++]=(uint8_t)((P_WaitInMachineStruct*)m_OperateProOrder[parNum2].pData)->backListNum;
        sendDataBuf[index++] = (uint8_t)(((P_WaitInMachineStruct*)m_OperateProOrder[parNum2].pData)->backListNum>>8);
        sendDataBuf[index++] = ((P_WaitInMachineStruct*)m_OperateProOrder[parNum2].pData)->inportNum;
        sendDataBuf[index++] = ((P_WaitInMachineStruct*)m_OperateProOrder[parNum2].pData)->inportSta;
        sendDataBuf[index++] = ((P_WaitInMachineStruct*)m_OperateProOrder[parNum2].pData)->type;
        sendDataBuf[index++] = ((P_WaitInMachineStruct*)m_OperateProOrder[parNum2].pData)->label;
        for(int m=0;m<2;m++)
        {
            sendDataBuf[index++] = ((P_WaitInMachineStruct*)m_OperateProOrder[parNum2].pData)->ret[m];
        }
        len = index;
        break;
    case C_WAIT_IN_CLAW:
        sendDataBuf[index++]=(uint8_t)((P_WaitInClawStruct*)m_OperateProOrder[parNum2].pData)->backListNum;
        sendDataBuf[index++] = (uint8_t)(((P_WaitInClawStruct*)m_OperateProOrder[parNum2].pData)->backListNum>>8);
        sendDataBuf[index++] = ((P_WaitInClawStruct*)m_OperateProOrder[parNum2].pData)->inportNum;
        sendDataBuf[index++] = ((P_WaitInClawStruct*)m_OperateProOrder[parNum2].pData)->inportSta;
        sendDataBuf[index++] = ((P_WaitInClawStruct*)m_OperateProOrder[parNum2].pData)->type;
        sendDataBuf[index++] = ((P_WaitInClawStruct*)m_OperateProOrder[parNum2].pData)->label;
        for(int m=0;m<3;m++)
        {
            sendDataBuf[index++] = ((P_WaitInClawStruct*)m_OperateProOrder[parNum2].pData)->ret[m];
        }
        len = index;
        break;
    case C_WAIT_IN_RESERVE:
        sendDataBuf[index++]=(uint8_t)((P_WaitInReserveStruct*)m_OperateProOrder[parNum2].pData)->backListNum;
        sendDataBuf[index++] = (uint8_t)(((P_WaitInReserveStruct*)m_OperateProOrder[parNum2].pData)->backListNum>>8);
        sendDataBuf[index++] = ((P_WaitInReserveStruct*)m_OperateProOrder[parNum2].pData)->inportNum;
        sendDataBuf[index++] = ((P_WaitInReserveStruct*)m_OperateProOrder[parNum2].pData)->inportSta;
        sendDataBuf[index++] = ((P_WaitInReserveStruct*)m_OperateProOrder[parNum2].pData)->type;
        sendDataBuf[index++] = ((P_WaitInReserveStruct*)m_OperateProOrder[parNum2].pData)->label;
        for(int m=0;m<3;m++)
        {
            sendDataBuf[index++] = ((P_WaitInClawStruct*)m_OperateProOrder[parNum2].pData)->ret[m];
        }
        len = index;
        break;
    case C_OTHER_DELAY:
        for(int m=0;m<4;m++)
        {
            sendDataBuf[index++] = ((P_OtherDelayStruct*)m_OperateProOrder[parNum2].pData)->res[m];
        }
        len = index;
        break;
    case C_OTHER_ALARM_CUST:
        sendDataBuf[index++]=(uint8_t)((P_OtherAlarmCustStruct*)m_OperateProOrder[parNum2].pData)->alarmNum;
        sendDataBuf[index++] = (uint8_t)(((P_OtherAlarmCustStruct*)m_OperateProOrder[parNum2].pData)->alarmNum>>8);
        sendDataBuf[index++] = ((P_OtherAlarmCustStruct*)m_OperateProOrder[parNum2].pData)->type;
        for(int m=0;m<1;m++)
        {
            sendDataBuf[index++] = ((P_OtherAlarmCustStruct*)m_OperateProOrder[parNum2].pData)->ret[m];
        }
        len = index;
        break;
    case C_OTHER_CYC_STOP:
        sendDataBuf[index++]=(uint8_t)((P_OtherCycStopStruct*)m_OperateProOrder[parNum2].pData)->cycleNum;
        sendDataBuf[index++] = (uint8_t)(((P_OtherCycStopStruct*)m_OperateProOrder[parNum2].pData)->cycleNum>>8);
        for(int m=0;m<2;m++)
        {
            sendDataBuf[index++] = ((P_OtherCycStopStruct*)m_OperateProOrder[parNum2].pData)->ret[m];
        }
        len = index;
        break;
    case C_OTHER_ALARM_LAMP:
        sendDataBuf[index++]=(uint8_t)((P_OtherAlarmLampStruct*)m_OperateProOrder[parNum2].pData)->outportNum;
        sendDataBuf[index++] = (uint8_t)(((P_OtherAlarmLampStruct*)m_OperateProOrder[parNum2].pData)->type);
        for(int m=0;m<2;m++)
        {
            sendDataBuf[index++] = ((P_OtherAlarmLampStruct*)m_OperateProOrder[parNum2].pData)->ret[m];
        }
        len = index;
        break;
    case C_OTHER_ALARM_SOUND:
        sendDataBuf[index++]=(uint8_t)((P_OtherAlarmSoundStruct*)m_OperateProOrder[parNum2].pData)->outportNum;
        sendDataBuf[index++] = (uint8_t)(((P_OtherAlarmSoundStruct*)m_OperateProOrder[parNum2].pData)->type);
        for(int m=0;m<2;m++)
        {
            sendDataBuf[index++] = ((P_OtherAlarmSoundStruct*)m_OperateProOrder[parNum2].pData)->ret[m];
        }
        len = index;
        break;
    case C_LABEL:
        sendDataBuf[index++]=(uint8_t)((P_LabelStruct*)m_OperateProOrder[parNum2].pData)->labelNum;
        sendDataBuf[index++] = (uint8_t)(((P_LabelStruct*)m_OperateProOrder[parNum2].pData)->labelNum>>8);
        sendDataBuf[index++]=(uint8_t)((P_LabelStruct*)m_OperateProOrder[parNum2].pData)->type;
        for(int m=0;m<2;m++)
        {
            sendDataBuf[index++] = ((P_LabelStruct*)m_OperateProOrder[parNum2].pData)->ret[m];
        }
        len = index;
        break;
    case C_LOGIC_IF:
        for(int m=0;m<REQUIRE_TOTAL_NUM;m++)
        {
            sendDataBuf[index++] = (uint8_t)((P_LogicIfStruct*)m_OperateProOrder[parNum2].pData)->sufferCmpValue[m];
            sendDataBuf[index++] = (uint8_t)(((P_LogicIfStruct*)m_OperateProOrder[parNum2].pData)->sufferCmpValue[m]>>8);
            sendDataBuf[index++] = (uint8_t)(((P_LogicIfStruct*)m_OperateProOrder[parNum2].pData)->sufferCmpValue[m]>>16);
            sendDataBuf[index++] = (uint8_t)(((P_LogicIfStruct*)m_OperateProOrder[parNum2].pData)->sufferCmpValue[m]>>24);
        }
        for(int m=0;m<REQUIRE_TOTAL_NUM;m++)
        {
            sendDataBuf[index++] = ((P_LogicIfStruct*)m_OperateProOrder[parNum2].pData)->reqSelectFlag[m];
        }
        for(int m=0;m<REQUIRE_TOTAL_NUM;m++)
        {
            sendDataBuf[index++] = ((P_LogicIfStruct*)m_OperateProOrder[parNum2].pData)->cmpType[m];
        }
        for(int m=0;m<REQUIRE_TOTAL_NUM;m++)
        {
            sendDataBuf[index++] = ((P_LogicIfStruct*)m_OperateProOrder[parNum2].pData)->inportNum[m];
        }
        for(int m=0;m<REQUIRE_TOTAL_NUM;m++)
        {
            sendDataBuf[index++] = ((P_LogicIfStruct*)m_OperateProOrder[parNum2].pData)->inportType[m];
        }
        for(int m=0;m<REQUIRE_TOTAL_NUM;m++)
        {
            sendDataBuf[index++] = ((P_LogicIfStruct*)m_OperateProOrder[parNum2].pData)->cmpMode[m];
        }
        for(int m=0;m<REQUIRE_TOTAL_NUM;m++)
        {
            sendDataBuf[index++] = ((P_LogicIfStruct*)m_OperateProOrder[parNum2].pData)->sufferCmpType[m];
        }
        len = index;
        break;
    case C_LOGIC_ELSE:
        for(int m=0;m<4;m++)
        {
            sendDataBuf[index++] = ((P_LogicElseStruct*)m_OperateProOrder[parNum2].pData)->res[m];
        }
        len = index;
        break;
    case C_LOGIC_END:
        for(int m=0;m<4;m++)
        {
            sendDataBuf[index++] = ((P_LogicEndStruct*)m_OperateProOrder[parNum2].pData)->res[m];
        }
        len = index;
        break;
    case C_LOGIC_WHILE_START:
        sendDataBuf[index++] = (uint8_t)((P_LogicWhileStartStruct*)m_OperateProOrder[parNum2].pData)->cycNum;
        sendDataBuf[index++] = (uint8_t)(((P_LogicWhileStartStruct*)m_OperateProOrder[parNum2].pData)->cycNum>>8);
        sendDataBuf[index++] = (uint8_t)(((P_LogicWhileStartStruct*)m_OperateProOrder[parNum2].pData)->cycNum>>16);
        sendDataBuf[index++] = (uint8_t)(((P_LogicWhileStartStruct*)m_OperateProOrder[parNum2].pData)->cycNum>>24);
        len = index;
        break;
    case C_LOGIC_WHILE_END:
        for(int m=0;m<4;m++)
        {
            sendDataBuf[index++] = ((P_LogicWhileEndStruct*)m_OperateProOrder[parNum2].pData)->res[m];
        }
        len = index;
        break;
    case C_LOGIC_VAR:
        sendDataBuf[index++] = (uint8_t)((P_LogicVarStruct*)m_OperateProOrder[parNum2].pData)->sufferOperValue;
        sendDataBuf[index++] = (uint8_t)(((P_LogicVarStruct*)m_OperateProOrder[parNum2].pData)->sufferOperValue>>8);
        sendDataBuf[index++] = (uint8_t)(((P_LogicVarStruct*)m_OperateProOrder[parNum2].pData)->sufferOperValue>>16);
        sendDataBuf[index++] = (uint8_t)(((P_LogicVarStruct*)m_OperateProOrder[parNum2].pData)->sufferOperValue>>24);
        sendDataBuf[index++] = ((P_LogicVarStruct*)m_OperateProOrder[parNum2].pData)->varNum;
        sendDataBuf[index++] = ((P_LogicVarStruct*)m_OperateProOrder[parNum2].pData)->operMode;
        sendDataBuf[index++] = ((P_LogicVarStruct*)m_OperateProOrder[parNum2].pData)->sufferOperType;
        for(int m=0;m<1;m++)
        {
            sendDataBuf[index++] = ((P_LogicVarStruct*)m_OperateProOrder[parNum2].pData)->ret[m];
        }
        len = index;
        break;
    case C_LOGIC_AXIS:
        sendDataBuf[index++] = (uint8_t)((P_LogicAxisStruct*)m_OperateProOrder[parNum2].pData)->sufferOperValue;
        sendDataBuf[index++] = (uint8_t)(((P_LogicAxisStruct*)m_OperateProOrder[parNum2].pData)->sufferOperValue>>8);
        sendDataBuf[index++] = (uint8_t)(((P_LogicAxisStruct*)m_OperateProOrder[parNum2].pData)->sufferOperValue>>16);
        sendDataBuf[index++] = (uint8_t)(((P_LogicAxisStruct*)m_OperateProOrder[parNum2].pData)->sufferOperValue>>24);
        sendDataBuf[index++] = ((P_LogicAxisStruct*)m_OperateProOrder[parNum2].pData)->axisNum;
        sendDataBuf[index++] = ((P_LogicAxisStruct*)m_OperateProOrder[parNum2].pData)->operMode;
        sendDataBuf[index++] = ((P_LogicAxisStruct*)m_OperateProOrder[parNum2].pData)->sufferOperType;
        for(int m=0;m<1;m++)
        {
            sendDataBuf[index++] = ((P_LogicAxisStruct*)m_OperateProOrder[parNum2].pData)->ret[m];
        }
        len = index;
        break;
    case C_LOGIC_STACK:
        sendDataBuf[index++] = (uint8_t)((P_LogicStackStruct*)m_OperateProOrder[parNum2].pData)->sufferOperValue;
        sendDataBuf[index++] = (uint8_t)(((P_LogicStackStruct*)m_OperateProOrder[parNum2].pData)->sufferOperValue>>8);
        sendDataBuf[index++] = (uint8_t)(((P_LogicStackStruct*)m_OperateProOrder[parNum2].pData)->sufferOperValue>>16);
        sendDataBuf[index++] = (uint8_t)(((P_LogicStackStruct*)m_OperateProOrder[parNum2].pData)->sufferOperValue>>24);
        sendDataBuf[index++] = ((P_LogicStackStruct*)m_OperateProOrder[parNum2].pData)->stackNum;
        sendDataBuf[index++] = ((P_LogicStackStruct*)m_OperateProOrder[parNum2].pData)->operMode;
        sendDataBuf[index++] = ((P_LogicStackStruct*)m_OperateProOrder[parNum2].pData)->sufferOperType;
        for(int m=0;m<1;m++)
        {
            sendDataBuf[index++] = ((P_LogicStackStruct*)m_OperateProOrder[parNum2].pData)->ret[m];
        }
        len = index;
        break;
    case C_LOGIC_PRODUCT:
        sendDataBuf[index++] = (uint8_t)((P_LogicCurProductNumStruct*)m_OperateProOrder[parNum2].pData)->sufferOperValue;
        sendDataBuf[index++] = (uint8_t)(((P_LogicCurProductNumStruct*)m_OperateProOrder[parNum2].pData)->sufferOperValue>>8);
        sendDataBuf[index++] = (uint8_t)(((P_LogicCurProductNumStruct*)m_OperateProOrder[parNum2].pData)->sufferOperValue>>16);
        sendDataBuf[index++] = (uint8_t)(((P_LogicCurProductNumStruct*)m_OperateProOrder[parNum2].pData)->sufferOperValue>>24);
        sendDataBuf[index++] = ((P_LogicCurProductNumStruct*)m_OperateProOrder[parNum2].pData)->productNum;
        sendDataBuf[index++] = ((P_LogicCurProductNumStruct*)m_OperateProOrder[parNum2].pData)->operMode;
        sendDataBuf[index++] = ((P_LogicCurProductNumStruct*)m_OperateProOrder[parNum2].pData)->sufferOperType;
        for(int m=0;m<1;m++)
        {
            sendDataBuf[index++] = ((P_LogicCurProductNumStruct*)m_OperateProOrder[parNum2].pData)->ret[m];
        }
        len = index;
        break;
    case C_LOGIC_TIME:
        sendDataBuf[index++] = ((P_LogicTimeStruct*)m_OperateProOrder[parNum2].pData)->timeNum;
        sendDataBuf[index++] = ((P_LogicTimeStruct*)m_OperateProOrder[parNum2].pData)->operMode;
        for(int m=0;m<2;m++)
        {
            sendDataBuf[index++] = ((P_LogicTimeStruct*)m_OperateProOrder[parNum2].pData)->ret[m];
        }
        len = index;
        break;
    case C_SEARCH_STOP:
        for(int m=0;m<4;m++)
        {
            sendDataBuf[index++] = ((P_SearchStopStruct*)m_OperateProOrder[parNum2].pData)->res[m];
        }
        len = index;
        break;
    case C_SEARCH_AXIS_MOVE:
        sendDataBuf[index++] = (uint8_t)((P_SearchAxisMoveStruct*)m_OperateProOrder[parNum2].pData)->maxPos;
        sendDataBuf[index++] = (uint8_t)(((P_SearchAxisMoveStruct*)m_OperateProOrder[parNum2].pData)->maxPos>>8);
        sendDataBuf[index++] = (uint8_t)(((P_SearchAxisMoveStruct*)m_OperateProOrder[parNum2].pData)->maxPos>>16);
        sendDataBuf[index++] = (uint8_t)(((P_SearchAxisMoveStruct*)m_OperateProOrder[parNum2].pData)->maxPos>>24);
        sendDataBuf[index++] = (uint8_t)((P_SearchAxisMoveStruct*)m_OperateProOrder[parNum2].pData)->advCDis;
        sendDataBuf[index++] = (uint8_t)(((P_SearchAxisMoveStruct*)m_OperateProOrder[parNum2].pData)->advCDis>>8);
        sendDataBuf[index++] = (uint8_t)(((P_SearchAxisMoveStruct*)m_OperateProOrder[parNum2].pData)->advCDis>>16);
        sendDataBuf[index++] = (uint8_t)(((P_SearchAxisMoveStruct*)m_OperateProOrder[parNum2].pData)->advCDis>>24);
        sendDataBuf[index++] = (uint8_t)((P_SearchAxisMoveStruct*)m_OperateProOrder[parNum2].pData)->offsetDis;
        sendDataBuf[index++] = (uint8_t)(((P_SearchAxisMoveStruct*)m_OperateProOrder[parNum2].pData)->offsetDis>>8);
        sendDataBuf[index++] = (uint8_t)(((P_SearchAxisMoveStruct*)m_OperateProOrder[parNum2].pData)->offsetDis>>16);
        sendDataBuf[index++] = (uint8_t)(((P_SearchAxisMoveStruct*)m_OperateProOrder[parNum2].pData)->offsetDis>>24);
        sendDataBuf[index++] = ((P_SearchAxisMoveStruct*)m_OperateProOrder[parNum2].pData)->axis;
        sendDataBuf[index++] = ((P_SearchAxisMoveStruct*)m_OperateProOrder[parNum2].pData)->posStoreFlag;
        sendDataBuf[index++] = ((P_SearchAxisMoveStruct*)m_OperateProOrder[parNum2].pData)->reachPosAlarmFlag;
        sendDataBuf[index++] = ((P_SearchAxisMoveStruct*)m_OperateProOrder[parNum2].pData)->runSpeed;
        sendDataBuf[index++] = ((P_SearchAxisMoveStruct*)m_OperateProOrder[parNum2].pData)->advCSpeed;
        sendDataBuf[index++] = ((P_SearchAxisMoveStruct*)m_OperateProOrder[parNum2].pData)->searchNum;
        for(int m=0;m<2;m++)
        {
            sendDataBuf[index++] = ((P_SearchAxisMoveStruct*)m_OperateProOrder[parNum2].pData)->ret[m];
        }
        for(int m=0;m<SEARCH_INPORT_TOTAL_NUM;m++)
        {
            sendDataBuf[index++] = ((P_SearchAxisMoveStruct*)m_OperateProOrder[parNum2].pData)->inportNum[m];
        }
        for(int m=0;m<SEARCH_INPORT_TOTAL_NUM;m++)
        {
            sendDataBuf[index++] = ((P_SearchAxisMoveStruct*)m_OperateProOrder[parNum2].pData)->inporttype[m];
        }
        len = index;
        break;
    case C_OFFSET_AXIS:
        sendDataBuf[index++] = (uint8_t)((P_OffsetAxisStruct*)m_OperateProOrder[parNum2].pData)->offsetPos;
        sendDataBuf[index++] = (uint8_t)(((P_OffsetAxisStruct*)m_OperateProOrder[parNum2].pData)->offsetPos>>8);
        sendDataBuf[index++] = (uint8_t)(((P_OffsetAxisStruct*)m_OperateProOrder[parNum2].pData)->offsetPos>>16);
        sendDataBuf[index++] = (uint8_t)(((P_OffsetAxisStruct*)m_OperateProOrder[parNum2].pData)->offsetPos>>24);
        sendDataBuf[index++] = (uint8_t)((P_OffsetAxisStruct*)m_OperateProOrder[parNum2].pData)->advEndDis;
        sendDataBuf[index++] = (uint8_t)(((P_OffsetAxisStruct*)m_OperateProOrder[parNum2].pData)->advEndDis>>8);
        sendDataBuf[index++] = (uint8_t)(((P_OffsetAxisStruct*)m_OperateProOrder[parNum2].pData)->advEndDis>>16);
        sendDataBuf[index++] = (uint8_t)(((P_OffsetAxisStruct*)m_OperateProOrder[parNum2].pData)->advEndDis>>24);
        sendDataBuf[index++] = (uint8_t)((P_OffsetAxisStruct*)m_OperateProOrder[parNum2].pData)->advCSpeedDis;
        sendDataBuf[index++] = (uint8_t)(((P_OffsetAxisStruct*)m_OperateProOrder[parNum2].pData)->advCSpeedDis>>8);
        sendDataBuf[index++] = (uint8_t)(((P_OffsetAxisStruct*)m_OperateProOrder[parNum2].pData)->advCSpeedDis>>16);
        sendDataBuf[index++] = (uint8_t)(((P_OffsetAxisStruct*)m_OperateProOrder[parNum2].pData)->advCSpeedDis>>24);
        sendDataBuf[index++] = ((P_OffsetAxisStruct*)m_OperateProOrder[parNum2].pData)->axis;
        sendDataBuf[index++] = ((P_OffsetAxisStruct*)m_OperateProOrder[parNum2].pData)->speed;
        sendDataBuf[index++] = ((P_OffsetAxisStruct*)m_OperateProOrder[parNum2].pData)->advEndFlag;
        sendDataBuf[index++] = ((P_OffsetAxisStruct*)m_OperateProOrder[parNum2].pData)->advCSpeedFlag;
        sendDataBuf[index++] = ((P_OffsetAxisStruct*)m_OperateProOrder[parNum2].pData)->advCSpeedSpeed;
        for(int m=0;m<3;m++)
        {
            sendDataBuf[index++] = ((P_OffsetAxisStruct*)m_OperateProOrder[parNum2].pData)->ret[m];
        }
        len = index;
        break;
    case C_TORQUE_GARD:
        sendDataBuf[index++] = ((P_TorqueGardStruct*)m_OperateProOrder[parNum2].pData)->axis;
        sendDataBuf[index++] = ((P_TorqueGardStruct*)m_OperateProOrder[parNum2].pData)->torqueValue;
        for(int m=0;m<2;m++)
        {
            sendDataBuf[index++] = ((P_TorqueGardStruct*)m_OperateProOrder[parNum2].pData)->ret[m];
        }
        len = index;
        break;
    case C_AXIS_STOP:
        sendDataBuf[index++] = ((P_AxisStopStruct*)m_OperateProOrder[parNum2].pData)->axis;
        for(int m=0;m<3;m++)
        {
            sendDataBuf[index++] = ((P_AxisStopStruct*)m_OperateProOrder[parNum2].pData)->ret[m];
        }
        len = index;
        break;
    case C_SUN_PRO:
        sendDataBuf[index++] = ((P_SunProStruct*)m_OperateProOrder[parNum2].pData)->sunProNum;
        sendDataBuf[index++] = ((P_SunProStruct*)m_OperateProOrder[parNum2].pData)->oprMode;
        for(int m=0;m<2;m++)
        {
            sendDataBuf[index++] = ((P_SunProStruct*)m_OperateProOrder[parNum2].pData)->ret[m];
        }
        len = index;
        break;
    case C_PRO_END:
        for(int m=0;m<4;m++)
        {
            sendDataBuf[index++] = ((P_ProEndStruct*)m_OperateProOrder[parNum2].pData)->res[m];
        }
        len = index;
        break;

    default:
        break;
    }

}



void Usart::GetProData(uint16_t parNum, uint16_t parNum2, uint8_t* sendDataBuf, uint16_t &len, uint16_t cmdLineType, uint16_t operateType)
{
    sendDataBuf[EXTEND_BUF_LEN] = {0};
    int index = 0;
    switch (cmdLineType)
    {
    case PRO_FLAG_CMDLINE_03:
        break;
    case PRO_FLAG_CMDLINE_04:
        sendDataBuf[index++] = (uint8_t)operateType;
        break;
    default:
        break;
    }
    sendDataBuf[index++] = (uint8_t)parNum;
    sendDataBuf[index++]=(uint8_t)m_ProOrder[parNum][parNum2].list;
    sendDataBuf[index++] = (uint8_t)(m_ProOrder[parNum][parNum2].list>>8);
    sendDataBuf[index++]=(uint8_t)m_ProOrder[parNum][parNum2].runOrderNum;
    sendDataBuf[index++] = (uint8_t)(m_ProOrder[parNum][parNum2].runOrderNum>>8);
    sendDataBuf[index++]=(uint8_t)m_ProOrder[parNum][parNum2].cmd;
    sendDataBuf[index++]=(uint8_t)m_ProOrder[parNum][parNum2].noteFlag;
    sendDataBuf[index++]=(uint8_t)m_ProOrder[parNum][parNum2].delay;
    sendDataBuf[index++] = (uint8_t)(m_ProOrder[parNum][parNum2].delay>>8);

    switch (m_ProOrder[parNum][parNum2].cmd)
    {
    case C_AXIS_MOVE:
        sendDataBuf[index++] = (uint8_t)((P_AxisMoveStruct*)m_ProOrder[parNum][parNum2].pData)->pos;
        sendDataBuf[index++] = (uint8_t)(((P_AxisMoveStruct*)m_ProOrder[parNum][parNum2].pData)->pos>>8);
        sendDataBuf[index++] = (uint8_t)(((P_AxisMoveStruct*)m_ProOrder[parNum][parNum2].pData)->pos>>16);
        sendDataBuf[index++] = (uint8_t)(((P_AxisMoveStruct*)m_ProOrder[parNum][parNum2].pData)->pos>>24);
        sendDataBuf[index++] = (uint8_t)((P_AxisMoveStruct*)m_ProOrder[parNum][parNum2].pData)->advEndDis;
        sendDataBuf[index++] = (uint8_t)(((P_AxisMoveStruct*)m_ProOrder[parNum][parNum2].pData)->advEndDis>>8);
        sendDataBuf[index++] = (uint8_t)(((P_AxisMoveStruct*)m_ProOrder[parNum][parNum2].pData)->advEndDis>>16);
        sendDataBuf[index++] = (uint8_t)(((P_AxisMoveStruct*)m_ProOrder[parNum][parNum2].pData)->advEndDis>>24);
        sendDataBuf[index++] = (uint8_t)((P_AxisMoveStruct*)m_ProOrder[parNum][parNum2].pData)->advCSpeedDis;
        sendDataBuf[index++] = (uint8_t)(((P_AxisMoveStruct*)m_ProOrder[parNum][parNum2].pData)->advCSpeedDis>>8);
        sendDataBuf[index++] = (uint8_t)(((P_AxisMoveStruct*)m_ProOrder[parNum][parNum2].pData)->advCSpeedDis>>16);
        sendDataBuf[index++] = (uint8_t)(((P_AxisMoveStruct*)m_ProOrder[parNum][parNum2].pData)->advCSpeedDis>>24);
        sendDataBuf[index++] = ((P_AxisMoveStruct*)m_ProOrder[parNum][parNum2].pData)->axis;
        sendDataBuf[index++] = ((P_AxisMoveStruct*)m_ProOrder[parNum][parNum2].pData)->speed;
        sendDataBuf[index++] = ((P_AxisMoveStruct*)m_ProOrder[parNum][parNum2].pData)->advEndFlag;
        sendDataBuf[index++] = ((P_AxisMoveStruct*)m_ProOrder[parNum][parNum2].pData)->advCSpeedFlag;
        sendDataBuf[index++] = ((P_AxisMoveStruct*)m_ProOrder[parNum][parNum2].pData)->advCSpeedFlag;
        sendDataBuf[index++] = ((P_AxisMoveStruct*)m_ProOrder[parNum][parNum2].pData)->referPointNum;
        for(int m=0;m<2;m++)
        {
            sendDataBuf[index++] = ((P_AxisMoveStruct*)m_ProOrder[parNum][parNum2].pData)->ret[m];
        }
        len = index;
        break;
    case C_CLAW_ACTION:
        sendDataBuf[index++] = ((P_ClawActionStruct*)m_ProOrder[parNum][parNum2].pData)->outportNum;
        sendDataBuf[index++] = ((P_ClawActionStruct*)m_ProOrder[parNum][parNum2].pData)->type;
        for(int m=0;m<2;m++)
        {
            sendDataBuf[index++] = ((P_ClawActionStruct*)m_ProOrder[parNum][parNum2].pData)->ret[m];
        }
        len = index;
        break;
    case C_CLAW_CHECK:
        sendDataBuf[index++] = ((P_ClawCheckStruct*)m_ProOrder[parNum][parNum2].pData)->outportNum;
        sendDataBuf[index++] = ((P_ClawCheckStruct*)m_ProOrder[parNum][parNum2].pData)->type;
        for(int m=0;m<2;m++)
        {
            sendDataBuf[index++] = ((P_ClawCheckStruct*)m_ProOrder[parNum][parNum2].pData)->ret[m];
        }
        len = index;
        break;
    case C_RESERVE_CHECK:
        sendDataBuf[index++] = ((P_ReserveCheckStruct*)m_ProOrder[parNum][parNum2].pData)->inportNum;
        sendDataBuf[index++] = ((P_ReserveCheckStruct*)m_ProOrder[parNum][parNum2].pData)->type;
        for(int m=0;m<2;m++)
        {
            sendDataBuf[index++] = ((P_ReserveCheckStruct*)m_ProOrder[parNum][parNum2].pData)->ret[m];
        }
        len = index;
        break;
    case C_MACHINE_OUT:
        sendDataBuf[index++] = ((P_MachineOutStruct*)m_ProOrder[parNum][parNum2].pData)->outportNum;
        sendDataBuf[index++] = ((P_MachineOutStruct*)m_ProOrder[parNum][parNum2].pData)->type;
        for(int m=0;m<2;m++)
        {
            sendDataBuf[index++] = ((P_MachineOutStruct*)m_ProOrder[parNum][parNum2].pData)->ret[m];
        }
        len = index;
        break;
    case C_STACK_RESET_ZERO:
        sendDataBuf[index++]=(uint8_t)((P_StackResetZeroStruct*)m_ProOrder[parNum][parNum2].pData)->number;
        sendDataBuf[index++] = (uint8_t)(((P_StackResetZeroStruct*)m_ProOrder[parNum][parNum2].pData)->number>>8);
        sendDataBuf[index++] = ((P_StackResetZeroStruct*)m_ProOrder[parNum][parNum2].pData)->stackNum;
        for(int m=0;m<1;m++)
        {
            sendDataBuf[index++] = ((P_StackResetZeroStruct*)m_ProOrder[parNum][parNum2].pData)->ret[m];
        }
        len = index;
        break;
    case C_STACK_MOVE:
        for(int m=0;m<STACK_AXIS_NUM;m++)
        {
            sendDataBuf[index++] = (uint8_t)((P_StackMoveStruct*)m_ProOrder[parNum][parNum2].pData)->advEndDis[m];
            sendDataBuf[index++] = (uint8_t)(((P_StackMoveStruct*)m_ProOrder[parNum][parNum2].pData)->advEndDis[m]>>8);
            sendDataBuf[index++] = (uint8_t)(((P_StackMoveStruct*)m_ProOrder[parNum][parNum2].pData)->advEndDis[m]>>16);
            sendDataBuf[index++] = (uint8_t)(((P_StackMoveStruct*)m_ProOrder[parNum][parNum2].pData)->advEndDis[m]>>24);
        }
        for(int m=0;m<STACK_AXIS_NUM;m++)
        {
            sendDataBuf[index++] = (uint8_t)((P_StackMoveStruct*)m_ProOrder[parNum][parNum2].pData)->advCSpeedDis[m];
            sendDataBuf[index++] = (uint8_t)(((P_StackMoveStruct*)m_ProOrder[parNum][parNum2].pData)->advCSpeedDis[m]>>8);
            sendDataBuf[index++] = (uint8_t)(((P_StackMoveStruct*)m_ProOrder[parNum][parNum2].pData)->advCSpeedDis[m]>>16);
            sendDataBuf[index++] = (uint8_t)(((P_StackMoveStruct*)m_ProOrder[parNum][parNum2].pData)->advCSpeedDis[m]>>24);
        }
        sendDataBuf[index++] = ((P_StackMoveStruct*)m_ProOrder[parNum][parNum2].pData)->stackNum;
        for(int m=0;m<3;m++)
        {
            sendDataBuf[index++] = ((P_StackMoveStruct*)m_ProOrder[parNum][parNum2].pData)->ret1[m];
        }
        for(int m=0;m<STACK_AXIS_NUM;m++)
        {
            sendDataBuf[index++] = ((P_StackMoveStruct*)m_ProOrder[parNum][parNum2].pData)->advEndFlag[m];
        }
        for(int m=0;m<1;m++)
        {
            sendDataBuf[index++] = ((P_StackMoveStruct*)m_ProOrder[parNum][parNum2].pData)->ret2[m];
        }
        for(int m=0;m<STACK_AXIS_NUM;m++)
        {
            sendDataBuf[index++] = ((P_StackMoveStruct*)m_ProOrder[parNum][parNum2].pData)->advCSpeedFlag[m];
        }
        for(int m=0;m<1;m++)
        {
            sendDataBuf[index++] = ((P_StackMoveStruct*)m_ProOrder[parNum][parNum2].pData)->ret3[m];
        }
        for(int m=0;m<STACK_AXIS_NUM;m++)
        {
            sendDataBuf[index++] = ((P_StackMoveStruct*)m_ProOrder[parNum][parNum2].pData)->advCSpeedSpeed[m];
        }
        for(int m=0;m<1;m++)
        {
            sendDataBuf[index++] = ((P_StackMoveStruct*)m_ProOrder[parNum][parNum2].pData)->ret4[m];
        }
        len = index;
        break;
    case C_STACK_FOLLOW:
        for(int m=0;m<STACK_AXIS_NUM;m++)
        {
            sendDataBuf[index++] = (uint8_t)((P_StackFollowStruct*)m_ProOrder[parNum][parNum2].pData)->advEndDis[m];
            sendDataBuf[index++] = (uint8_t)(((P_StackFollowStruct*)m_ProOrder[parNum][parNum2].pData)->advEndDis[m]>>8);
            sendDataBuf[index++] = (uint8_t)(((P_StackFollowStruct*)m_ProOrder[parNum][parNum2].pData)->advEndDis[m]>>16);
            sendDataBuf[index++] = (uint8_t)(((P_StackFollowStruct*)m_ProOrder[parNum][parNum2].pData)->advEndDis[m]>>24);
        }
        for(int m=0;m<STACK_AXIS_NUM;m++)
        {
            sendDataBuf[index++] = (uint8_t)((P_StackFollowStruct*)m_ProOrder[parNum][parNum2].pData)->advCSpeedDis[m];
            sendDataBuf[index++] = (uint8_t)(((P_StackFollowStruct*)m_ProOrder[parNum][parNum2].pData)->advCSpeedDis[m]>>8);
            sendDataBuf[index++] = (uint8_t)(((P_StackFollowStruct*)m_ProOrder[parNum][parNum2].pData)->advCSpeedDis[m]>>16);
            sendDataBuf[index++] = (uint8_t)(((P_StackFollowStruct*)m_ProOrder[parNum][parNum2].pData)->advCSpeedDis[m]>>24);
        }
        sendDataBuf[index++] = ((P_StackFollowStruct*)m_ProOrder[parNum][parNum2].pData)->stackNum;
        for(int m=0;m<3;m++)
        {
            sendDataBuf[index++] = ((P_StackFollowStruct*)m_ProOrder[parNum][parNum2].pData)->ret1[m];
        }
        for(int m=0;m<STACK_AXIS_NUM;m++)
        {
            sendDataBuf[index++] = ((P_StackFollowStruct*)m_ProOrder[parNum][parNum2].pData)->advEndFlag[m];
        }
        for(int m=0;m<1;m++)
        {
            sendDataBuf[index++] = ((P_StackFollowStruct*)m_ProOrder[parNum][parNum2].pData)->ret2[m];
        }
        for(int m=0;m<STACK_AXIS_NUM;m++)
        {
            sendDataBuf[index++] = ((P_StackFollowStruct*)m_ProOrder[parNum][parNum2].pData)->advCSpeedFlag[m];
        }
        for(int m=0;m<1;m++)
        {
            sendDataBuf[index++] = ((P_StackFollowStruct*)m_ProOrder[parNum][parNum2].pData)->ret3[m];
        }
        for(int m=0;m<STACK_AXIS_NUM;m++)
        {
            sendDataBuf[index++] = ((P_StackFollowStruct*)m_ProOrder[parNum][parNum2].pData)->advCSpeedSpeed[m];
        }
        for(int m=0;m<1;m++)
        {
            sendDataBuf[index++] = ((P_StackFollowStruct*)m_ProOrder[parNum][parNum2].pData)->ret4[m];
        }
        len = index;
        break;
    case C_RESERVE_OUT:
        sendDataBuf[index++]=(uint8_t)((P_ReserveOutStruct*)m_ProOrder[parNum][parNum2].pData)->interval;
        sendDataBuf[index++] = (uint8_t)(((P_ReserveOutStruct*)m_ProOrder[parNum][parNum2].pData)->interval>>8);
        sendDataBuf[index++] = ((P_ReserveOutStruct*)m_ProOrder[parNum][parNum2].pData)->outportNum;
        sendDataBuf[index++] = ((P_ReserveOutStruct*)m_ProOrder[parNum][parNum2].pData)->function;
        sendDataBuf[index++] = ((P_ReserveOutStruct*)m_ProOrder[parNum][parNum2].pData)->type;
        for(int m=0;m<3;m++)
        {
            sendDataBuf[index++] = ((P_ReserveOutStruct*)m_ProOrder[parNum][parNum2].pData)->ret[m];
        }
        len = index;
        break;
    case C_WAIT_IN_MACHINE:
        sendDataBuf[index++]=(uint8_t)((P_WaitInMachineStruct*)m_ProOrder[parNum][parNum2].pData)->backListNum;
        sendDataBuf[index++] = (uint8_t)(((P_WaitInMachineStruct*)m_ProOrder[parNum][parNum2].pData)->backListNum>>8);
        sendDataBuf[index++] = ((P_WaitInMachineStruct*)m_ProOrder[parNum][parNum2].pData)->inportNum;
        sendDataBuf[index++] = ((P_WaitInMachineStruct*)m_ProOrder[parNum][parNum2].pData)->inportSta;
        sendDataBuf[index++] = ((P_WaitInMachineStruct*)m_ProOrder[parNum][parNum2].pData)->type;
        sendDataBuf[index++] = ((P_WaitInMachineStruct*)m_ProOrder[parNum][parNum2].pData)->label;
        for(int m=0;m<2;m++)
        {
            sendDataBuf[index++] = ((P_WaitInMachineStruct*)m_ProOrder[parNum][parNum2].pData)->ret[m];
        }
        len = index;
        break;
    case C_WAIT_IN_CLAW:
        sendDataBuf[index++]=(uint8_t)((P_WaitInClawStruct*)m_ProOrder[parNum][parNum2].pData)->backListNum;
        sendDataBuf[index++] = (uint8_t)(((P_WaitInClawStruct*)m_ProOrder[parNum][parNum2].pData)->backListNum>>8);
        sendDataBuf[index++] = ((P_WaitInClawStruct*)m_ProOrder[parNum][parNum2].pData)->inportNum;
        sendDataBuf[index++] = ((P_WaitInClawStruct*)m_ProOrder[parNum][parNum2].pData)->inportSta;
        sendDataBuf[index++] = ((P_WaitInClawStruct*)m_ProOrder[parNum][parNum2].pData)->type;
        sendDataBuf[index++] = ((P_WaitInClawStruct*)m_ProOrder[parNum][parNum2].pData)->label;
        for(int m=0;m<3;m++)
        {
            sendDataBuf[index++] = ((P_WaitInClawStruct*)m_ProOrder[parNum][parNum2].pData)->ret[m];
        }
        len = index;
        break;
    case C_WAIT_IN_RESERVE:
        sendDataBuf[index++]=(uint8_t)((P_WaitInReserveStruct*)m_ProOrder[parNum][parNum2].pData)->backListNum;
        sendDataBuf[index++] = (uint8_t)(((P_WaitInReserveStruct*)m_ProOrder[parNum][parNum2].pData)->backListNum>>8);
        sendDataBuf[index++] = ((P_WaitInReserveStruct*)m_ProOrder[parNum][parNum2].pData)->inportNum;
        sendDataBuf[index++] = ((P_WaitInReserveStruct*)m_ProOrder[parNum][parNum2].pData)->inportSta;
        sendDataBuf[index++] = ((P_WaitInReserveStruct*)m_ProOrder[parNum][parNum2].pData)->type;
        sendDataBuf[index++] = ((P_WaitInReserveStruct*)m_ProOrder[parNum][parNum2].pData)->label;
        for(int m=0;m<3;m++)
        {
            sendDataBuf[index++] = ((P_WaitInClawStruct*)m_ProOrder[parNum][parNum2].pData)->ret[m];
        }
        len = index;
        break;
    case C_OTHER_DELAY:
        for(int m=0;m<4;m++)
        {
            sendDataBuf[index++] = ((P_OtherDelayStruct*)m_ProOrder[parNum][parNum2].pData)->res[m];
        }
        len = index;
        break;
    case C_OTHER_ALARM_CUST:
        sendDataBuf[index++]=(uint8_t)((P_OtherAlarmCustStruct*)m_ProOrder[parNum][parNum2].pData)->alarmNum;
        sendDataBuf[index++] = (uint8_t)(((P_OtherAlarmCustStruct*)m_ProOrder[parNum][parNum2].pData)->alarmNum>>8);
        sendDataBuf[index++] = ((P_OtherAlarmCustStruct*)m_ProOrder[parNum][parNum2].pData)->type;
        for(int m=0;m<1;m++)
        {
            sendDataBuf[index++] = ((P_OtherAlarmCustStruct*)m_ProOrder[parNum][parNum2].pData)->ret[m];
        }
        len = index;
        break;
    case C_OTHER_CYC_STOP:
        sendDataBuf[index++]=(uint8_t)((P_OtherCycStopStruct*)m_ProOrder[parNum][parNum2].pData)->cycleNum;
        sendDataBuf[index++] = (uint8_t)(((P_OtherCycStopStruct*)m_ProOrder[parNum][parNum2].pData)->cycleNum>>8);
        for(int m=0;m<2;m++)
        {
            sendDataBuf[index++] = ((P_OtherCycStopStruct*)m_ProOrder[parNum][parNum2].pData)->ret[m];
        }
        len = index;
        break;
    case C_LABEL:
        sendDataBuf[index++]=(uint8_t)((P_LabelStruct*)m_ProOrder[parNum][parNum2].pData)->labelNum;
        sendDataBuf[index++] = (uint8_t)(((P_LabelStruct*)m_ProOrder[parNum][parNum2].pData)->labelNum>>8);
        sendDataBuf[index++]=(uint8_t)((P_LabelStruct*)m_ProOrder[parNum][parNum2].pData)->type;
        for(int m=0;m<2;m++)
        {
            sendDataBuf[index++] = ((P_LabelStruct*)m_ProOrder[parNum][parNum2].pData)->ret[m];
        }
        len = index;
        break;
    case C_LOGIC_IF:
        for(int m=0;m<REQUIRE_TOTAL_NUM;m++)
        {
            sendDataBuf[index++] = (uint8_t)((P_LogicIfStruct*)m_ProOrder[parNum][parNum2].pData)->sufferCmpValue[m];
            sendDataBuf[index++] = (uint8_t)(((P_LogicIfStruct*)m_ProOrder[parNum][parNum2].pData)->sufferCmpValue[m]>>8);
            sendDataBuf[index++] = (uint8_t)(((P_LogicIfStruct*)m_ProOrder[parNum][parNum2].pData)->sufferCmpValue[m]>>16);
            sendDataBuf[index++] = (uint8_t)(((P_LogicIfStruct*)m_ProOrder[parNum][parNum2].pData)->sufferCmpValue[m]>>24);
        }
        for(int m=0;m<REQUIRE_TOTAL_NUM;m++)
        {
            sendDataBuf[index++] = ((P_LogicIfStruct*)m_ProOrder[parNum][parNum2].pData)->reqSelectFlag[m];
        }
        for(int m=0;m<REQUIRE_TOTAL_NUM;m++)
        {
            sendDataBuf[index++] = ((P_LogicIfStruct*)m_ProOrder[parNum][parNum2].pData)->cmpType[m];
        }
        for(int m=0;m<REQUIRE_TOTAL_NUM;m++)
        {
            sendDataBuf[index++] = ((P_LogicIfStruct*)m_ProOrder[parNum][parNum2].pData)->inportNum[m];
        }
        for(int m=0;m<REQUIRE_TOTAL_NUM;m++)
        {
            sendDataBuf[index++] = ((P_LogicIfStruct*)m_ProOrder[parNum][parNum2].pData)->inportType[m];
        }
        for(int m=0;m<REQUIRE_TOTAL_NUM;m++)
        {
            sendDataBuf[index++] = ((P_LogicIfStruct*)m_ProOrder[parNum][parNum2].pData)->cmpMode[m];
        }
        for(int m=0;m<REQUIRE_TOTAL_NUM;m++)
        {
            sendDataBuf[index++] = ((P_LogicIfStruct*)m_ProOrder[parNum][parNum2].pData)->sufferCmpType[m];
        }
        len = index;
        break;
    case C_LOGIC_ELSE:
        for(int m=0;m<4;m++)
        {
            sendDataBuf[index++] = ((P_LogicElseStruct*)m_ProOrder[parNum][parNum2].pData)->res[m];
        }
        len = index;
        break;
    case C_LOGIC_END:
        for(int m=0;m<4;m++)
        {
            sendDataBuf[index++] = ((P_LogicEndStruct*)m_ProOrder[parNum][parNum2].pData)->res[m];
        }
        len = index;
        break;
    case C_LOGIC_WHILE_START:
        sendDataBuf[index++] = (uint8_t)((P_LogicWhileStartStruct*)m_ProOrder[parNum][parNum2].pData)->cycNum;
        sendDataBuf[index++] = (uint8_t)(((P_LogicWhileStartStruct*)m_ProOrder[parNum][parNum2].pData)->cycNum>>8);
        sendDataBuf[index++] = (uint8_t)(((P_LogicWhileStartStruct*)m_ProOrder[parNum][parNum2].pData)->cycNum>>16);
        sendDataBuf[index++] = (uint8_t)(((P_LogicWhileStartStruct*)m_ProOrder[parNum][parNum2].pData)->cycNum>>24);
        len = index;
        break;
    case C_LOGIC_WHILE_END:
        for(int m=0;m<4;m++)
        {
            sendDataBuf[index++] = ((P_LogicWhileEndStruct*)m_ProOrder[parNum][parNum2].pData)->res[m];
        }
        len = index;
        break;
    case C_LOGIC_VAR:
        sendDataBuf[index++] = (uint8_t)((P_LogicVarStruct*)m_ProOrder[parNum][parNum2].pData)->sufferOperValue;
        sendDataBuf[index++] = (uint8_t)(((P_LogicVarStruct*)m_ProOrder[parNum][parNum2].pData)->sufferOperValue>>8);
        sendDataBuf[index++] = (uint8_t)(((P_LogicVarStruct*)m_ProOrder[parNum][parNum2].pData)->sufferOperValue>>16);
        sendDataBuf[index++] = (uint8_t)(((P_LogicVarStruct*)m_ProOrder[parNum][parNum2].pData)->sufferOperValue>>24);
        sendDataBuf[index++] = ((P_LogicVarStruct*)m_ProOrder[parNum][parNum2].pData)->varNum;
        sendDataBuf[index++] = ((P_LogicVarStruct*)m_ProOrder[parNum][parNum2].pData)->operMode;
        sendDataBuf[index++] = ((P_LogicVarStruct*)m_ProOrder[parNum][parNum2].pData)->sufferOperType;
        for(int m=0;m<1;m++)
        {
            sendDataBuf[index++] = ((P_LogicVarStruct*)m_ProOrder[parNum][parNum2].pData)->ret[m];
        }
        len = index;
        break;
    case C_LOGIC_AXIS:
        sendDataBuf[index++] = (uint8_t)((P_LogicAxisStruct*)m_ProOrder[parNum][parNum2].pData)->sufferOperValue;
        sendDataBuf[index++] = (uint8_t)(((P_LogicAxisStruct*)m_ProOrder[parNum][parNum2].pData)->sufferOperValue>>8);
        sendDataBuf[index++] = (uint8_t)(((P_LogicAxisStruct*)m_ProOrder[parNum][parNum2].pData)->sufferOperValue>>16);
        sendDataBuf[index++] = (uint8_t)(((P_LogicAxisStruct*)m_ProOrder[parNum][parNum2].pData)->sufferOperValue>>24);
        sendDataBuf[index++] = ((P_LogicAxisStruct*)m_ProOrder[parNum][parNum2].pData)->axisNum;
        sendDataBuf[index++] = ((P_LogicAxisStruct*)m_ProOrder[parNum][parNum2].pData)->operMode;
        sendDataBuf[index++] = ((P_LogicAxisStruct*)m_ProOrder[parNum][parNum2].pData)->sufferOperType;
        for(int m=0;m<1;m++)
        {
            sendDataBuf[index++] = ((P_LogicAxisStruct*)m_ProOrder[parNum][parNum2].pData)->ret[m];
        }
        len = index;
        break;
    case C_LOGIC_STACK:
        sendDataBuf[index++] = (uint8_t)((P_LogicStackStruct*)m_ProOrder[parNum][parNum2].pData)->sufferOperValue;
        sendDataBuf[index++] = (uint8_t)(((P_LogicStackStruct*)m_ProOrder[parNum][parNum2].pData)->sufferOperValue>>8);
        sendDataBuf[index++] = (uint8_t)(((P_LogicStackStruct*)m_ProOrder[parNum][parNum2].pData)->sufferOperValue>>16);
        sendDataBuf[index++] = (uint8_t)(((P_LogicStackStruct*)m_ProOrder[parNum][parNum2].pData)->sufferOperValue>>24);
        sendDataBuf[index++] = ((P_LogicStackStruct*)m_ProOrder[parNum][parNum2].pData)->stackNum;
        sendDataBuf[index++] = ((P_LogicStackStruct*)m_ProOrder[parNum][parNum2].pData)->operMode;
        sendDataBuf[index++] = ((P_LogicStackStruct*)m_ProOrder[parNum][parNum2].pData)->sufferOperType;
        for(int m=0;m<1;m++)
        {
            sendDataBuf[index++] = ((P_LogicStackStruct*)m_ProOrder[parNum][parNum2].pData)->ret[m];
        }
        len = index;
        break;
    case C_LOGIC_PRODUCT:
        sendDataBuf[index++] = (uint8_t)((P_LogicCurProductNumStruct*)m_ProOrder[parNum][parNum2].pData)->sufferOperValue;
        sendDataBuf[index++] = (uint8_t)(((P_LogicCurProductNumStruct*)m_ProOrder[parNum][parNum2].pData)->sufferOperValue>>8);
        sendDataBuf[index++] = (uint8_t)(((P_LogicCurProductNumStruct*)m_ProOrder[parNum][parNum2].pData)->sufferOperValue>>16);
        sendDataBuf[index++] = (uint8_t)(((P_LogicCurProductNumStruct*)m_ProOrder[parNum][parNum2].pData)->sufferOperValue>>24);
        sendDataBuf[index++] = ((P_LogicCurProductNumStruct*)m_ProOrder[parNum][parNum2].pData)->productNum;
        sendDataBuf[index++] = ((P_LogicCurProductNumStruct*)m_ProOrder[parNum][parNum2].pData)->operMode;
        sendDataBuf[index++] = ((P_LogicCurProductNumStruct*)m_ProOrder[parNum][parNum2].pData)->sufferOperType;
        for(int m=0;m<1;m++)
        {
            sendDataBuf[index++] = ((P_LogicCurProductNumStruct*)m_ProOrder[parNum][parNum2].pData)->ret[m];
        }
        len = index;
        break;
    case C_LOGIC_TIME:
        sendDataBuf[index++] = ((P_LogicTimeStruct*)m_ProOrder[parNum][parNum2].pData)->timeNum;
        sendDataBuf[index++] = ((P_LogicTimeStruct*)m_ProOrder[parNum][parNum2].pData)->operMode;
        for(int m=0;m<2;m++)
        {
            sendDataBuf[index++] = ((P_LogicTimeStruct*)m_ProOrder[parNum][parNum2].pData)->ret[m];
        }
        len = index;
        break;
    case C_SEARCH_STOP:
        for(int m=0;m<4;m++)
        {
            sendDataBuf[index++] = ((P_SearchStopStruct*)m_ProOrder[parNum][parNum2].pData)->res[m];
        }
        len = index;
        break;
    case C_SEARCH_AXIS_MOVE:
        sendDataBuf[index++] = (uint8_t)((P_SearchAxisMoveStruct*)m_ProOrder[parNum][parNum2].pData)->maxPos;
        sendDataBuf[index++] = (uint8_t)(((P_SearchAxisMoveStruct*)m_ProOrder[parNum][parNum2].pData)->maxPos>>8);
        sendDataBuf[index++] = (uint8_t)(((P_SearchAxisMoveStruct*)m_ProOrder[parNum][parNum2].pData)->maxPos>>16);
        sendDataBuf[index++] = (uint8_t)(((P_SearchAxisMoveStruct*)m_ProOrder[parNum][parNum2].pData)->maxPos>>24);
        sendDataBuf[index++] = (uint8_t)((P_SearchAxisMoveStruct*)m_ProOrder[parNum][parNum2].pData)->advCDis;
        sendDataBuf[index++] = (uint8_t)(((P_SearchAxisMoveStruct*)m_ProOrder[parNum][parNum2].pData)->advCDis>>8);
        sendDataBuf[index++] = (uint8_t)(((P_SearchAxisMoveStruct*)m_ProOrder[parNum][parNum2].pData)->advCDis>>16);
        sendDataBuf[index++] = (uint8_t)(((P_SearchAxisMoveStruct*)m_ProOrder[parNum][parNum2].pData)->advCDis>>24);
        sendDataBuf[index++] = (uint8_t)((P_SearchAxisMoveStruct*)m_ProOrder[parNum][parNum2].pData)->offsetDis;
        sendDataBuf[index++] = (uint8_t)(((P_SearchAxisMoveStruct*)m_ProOrder[parNum][parNum2].pData)->offsetDis>>8);
        sendDataBuf[index++] = (uint8_t)(((P_SearchAxisMoveStruct*)m_ProOrder[parNum][parNum2].pData)->offsetDis>>16);
        sendDataBuf[index++] = (uint8_t)(((P_SearchAxisMoveStruct*)m_ProOrder[parNum][parNum2].pData)->offsetDis>>24);
        sendDataBuf[index++] = ((P_SearchAxisMoveStruct*)m_ProOrder[parNum][parNum2].pData)->axis;
        sendDataBuf[index++] = ((P_SearchAxisMoveStruct*)m_ProOrder[parNum][parNum2].pData)->posStoreFlag;
        sendDataBuf[index++] = ((P_SearchAxisMoveStruct*)m_ProOrder[parNum][parNum2].pData)->reachPosAlarmFlag;
        sendDataBuf[index++] = ((P_SearchAxisMoveStruct*)m_ProOrder[parNum][parNum2].pData)->runSpeed;
        sendDataBuf[index++] = ((P_SearchAxisMoveStruct*)m_ProOrder[parNum][parNum2].pData)->advCSpeed;
        sendDataBuf[index++] = ((P_SearchAxisMoveStruct*)m_ProOrder[parNum][parNum2].pData)->searchNum;
        for(int m=0;m<2;m++)
        {
            sendDataBuf[index++] = ((P_SearchAxisMoveStruct*)m_ProOrder[parNum][parNum2].pData)->ret[m];
        }
        for(int m=0;m<SEARCH_INPORT_TOTAL_NUM;m++)
        {
            sendDataBuf[index++] = ((P_SearchAxisMoveStruct*)m_ProOrder[parNum][parNum2].pData)->inportNum[m];
        }
        for(int m=0;m<SEARCH_INPORT_TOTAL_NUM;m++)
        {
            sendDataBuf[index++] = ((P_SearchAxisMoveStruct*)m_ProOrder[parNum][parNum2].pData)->inporttype[m];
        }
        len = index;
        break;
    case C_OFFSET_AXIS:
        sendDataBuf[index++] = (uint8_t)((P_OffsetAxisStruct*)m_OperateProOrder[parNum2].pData)->offsetPos;
        sendDataBuf[index++] = (uint8_t)(((P_OffsetAxisStruct*)m_OperateProOrder[parNum2].pData)->offsetPos>>8);
        sendDataBuf[index++] = (uint8_t)(((P_OffsetAxisStruct*)m_OperateProOrder[parNum2].pData)->offsetPos>>16);
        sendDataBuf[index++] = (uint8_t)(((P_OffsetAxisStruct*)m_OperateProOrder[parNum2].pData)->offsetPos>>24);
        sendDataBuf[index++] = (uint8_t)((P_OffsetAxisStruct*)m_OperateProOrder[parNum2].pData)->advEndDis;
        sendDataBuf[index++] = (uint8_t)(((P_OffsetAxisStruct*)m_OperateProOrder[parNum2].pData)->advEndDis>>8);
        sendDataBuf[index++] = (uint8_t)(((P_OffsetAxisStruct*)m_OperateProOrder[parNum2].pData)->advEndDis>>16);
        sendDataBuf[index++] = (uint8_t)(((P_OffsetAxisStruct*)m_OperateProOrder[parNum2].pData)->advEndDis>>24);
        sendDataBuf[index++] = (uint8_t)((P_OffsetAxisStruct*)m_OperateProOrder[parNum2].pData)->advCSpeedDis;
        sendDataBuf[index++] = (uint8_t)(((P_OffsetAxisStruct*)m_OperateProOrder[parNum2].pData)->advCSpeedDis>>8);
        sendDataBuf[index++] = (uint8_t)(((P_OffsetAxisStruct*)m_OperateProOrder[parNum2].pData)->advCSpeedDis>>16);
        sendDataBuf[index++] = (uint8_t)(((P_OffsetAxisStruct*)m_OperateProOrder[parNum2].pData)->advCSpeedDis>>24);
        sendDataBuf[index++] = ((P_OffsetAxisStruct*)m_OperateProOrder[parNum2].pData)->axis;
        sendDataBuf[index++] = ((P_OffsetAxisStruct*)m_OperateProOrder[parNum2].pData)->speed;
        sendDataBuf[index++] = ((P_OffsetAxisStruct*)m_OperateProOrder[parNum2].pData)->advEndFlag;
        sendDataBuf[index++] = ((P_OffsetAxisStruct*)m_OperateProOrder[parNum2].pData)->advCSpeedFlag;
        sendDataBuf[index++] = ((P_OffsetAxisStruct*)m_OperateProOrder[parNum2].pData)->advCSpeedSpeed;
        for(int m=0;m<3;m++)
        {
            sendDataBuf[index++] = ((P_OffsetAxisStruct*)m_OperateProOrder[parNum2].pData)->ret[m];
        }
        len = index;
        break;
    case C_TORQUE_GARD:
        sendDataBuf[index++] = ((P_TorqueGardStruct*)m_ProOrder[parNum][parNum2].pData)->axis;
        sendDataBuf[index++] = ((P_TorqueGardStruct*)m_ProOrder[parNum][parNum2].pData)->torqueValue;
        for(int m=0;m<3;m++)
        {
            sendDataBuf[index++] = ((P_TorqueGardStruct*)m_ProOrder[parNum][parNum2].pData)->ret[m];
        }
        len = index;
        break;
    case C_AXIS_STOP:
        sendDataBuf[index++] = ((P_AxisStopStruct*)m_ProOrder[parNum][parNum2].pData)->axis;
        for(int m=0;m<3;m++)
        {
            sendDataBuf[index++] = ((P_AxisStopStruct*)m_ProOrder[parNum][parNum2].pData)->ret[m];
        }
        len = index;
        break;
    case C_SUN_PRO:
        sendDataBuf[index++] = ((P_SunProStruct*)m_ProOrder[parNum][parNum2].pData)->sunProNum;
        sendDataBuf[index++] = ((P_SunProStruct*)m_ProOrder[parNum][parNum2].pData)->oprMode;
        for(int m=0;m<2;m++)
        {
            sendDataBuf[index++] = ((P_SunProStruct*)m_ProOrder[parNum][parNum2].pData)->ret[m];
        }
        len = index;
        break;
    case C_PRO_END:
        for(int m=0;m<4;m++)
        {
            sendDataBuf[index++] = ((P_ProEndStruct*)m_ProOrder[parNum][parNum2].pData)->res[m];
        }
        len = index;
        break;

    default:
        break;
    }

}
#if 0
//开机参数和程序下发
uint8_t Usart::DataSyc()
{
    switch (MySync_Data.SendDataStep) {
    case SysSendIndex::CMD_OUT_TYPE://信号设置-输出类型
    {
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.out_type_State = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SIGNAL,CMD_SUN_SIGNAL_OUT_TYPE);
            MySync_Data.SendData_flag = 1;
        }
        if(MySync_Data.out_type_State == true)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
        }
        else
        {
            MySync_Data.sendDataOutTime++;//一次10ms
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {//超过1s未接收到反馈重新发送
                MySync_Data.SendData_flag = 0;
            }
        }
        break;
    }
    case SysSendIndex::CMD_INTERLOCK:
    {
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.interlock_State = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SIGNAL,CMD_SUN_SIGNAL_INTERLOCK);
            MySync_Data.SendData_flag = 1;
        }

        if(MySync_Data.interlock_State == true)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.SendData_flag = 0;
            }
        }
        break;
    }
    case SysSendIndex::CMD_IN_FUNC_DEF:
    {
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.in_func_def_State = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SIGNAL,CMD_SUN_SIGNAL_IN_FUNC_DEF);
            MySync_Data.SendData_flag = 1;
        }

        if(MySync_Data.in_func_def_State == true)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.SendData_flag = 0;
            }
        }
        break;
    }
    case SysSendIndex::CMD_OUT_FUNC_DEF:
    {
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.out_func_def_State = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SIGNAL,CMD_SUN_SIGNAL_OUT_FUNC_DEF);
            MySync_Data.SendData_flag = 1;
        }

        if(MySync_Data.out_func_def_State == true)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.SendData_flag = 0;
            }
        }
        break;
    }
    case SysSendIndex::CMD_OUT_RELEVENCY:
    {
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.out_relevency_State = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SIGNAL,CMD_SUN_SIGNAL_OUT_RELEVENCY);
            MySync_Data.SendData_flag = 1;
        }

        if(MySync_Data.out_relevency_State == true)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.SendData_flag = 0;
            }
        }
        break;
    }
    case SysSendIndex::CMD_RELATE_OUT:
    {
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.out_relate_out = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SIGNAL,CMD_SUN_SIGNAL_RELATE_OUT);
            MySync_Data.SendData_flag = 1;
        }

        if(MySync_Data.out_relate_out == true)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.SendData_flag = 0;
            }
        }
        break;
    }
    case SysSendIndex::CMD_KEY:
    {
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.out_key = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SIGNAL,CMD_SUN_SIGNAL_KEY);
            MySync_Data.SendData_flag = 1;
        }

        if(MySync_Data.out_key == true)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.SendData_flag = 0;
            }
        }
        break;
    }
    case SysSendIndex::CMD_SENIOR:
    {
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.senior = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SIGNAL,CMD_SUN_SIGNAL_SENIOR);
            MySync_Data.SendData_flag = 1;
        }

        if(MySync_Data.senior == true)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.SendData_flag = 0;
            }
        }
        break;
    }
    case SysSendIndex::CMD_SENIOR_PORT:
    {
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.senior_port = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SIGNAL,CMD_SUN_SIGNAL_SENIOR_PORT);
            MySync_Data.SendData_flag = 1;
        }

        if(MySync_Data.senior_port == true)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.SendData_flag = 0;
            }
        }
        break;
    }
    case SysSendIndex::CMD_SAVE_MACHINE:
    {

        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.save_machine = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SAVE,CMD_SUN_SAVE_MACHINE);
            MySync_Data.SendData_flag = 1;
        }

        if(MySync_Data.save_machine == true)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.SendData_flag = 0;
            }
        }
        break;
    }
    case SysSendIndex::CMD_SAVE_STACK:
    {
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.save_stack = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SAVE,CMD_SUN_SAVE_STACK);
            MySync_Data.SendData_flag = 1;
        }

        if(MySync_Data.save_stack == true)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.SendData_flag = 0;
            }
        }
        break;
    }
    case SysSendIndex::CMD_SAVE_CALW:
    {

        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.save_calw = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SAVE,CMD_SUN_SAVE_CALW,1);//一共四组先发第1组
            MySync_Data.SendData_flag = 1;
        }

        if(MySync_Data.save_calw == 1)
        {
            MySync_Data.sendDataOutTime = 0;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SAVE,CMD_SUN_SAVE_CALW,2);//一共四组先发第2组
        }
        else if(MySync_Data.save_calw == 2)
        {
            MySync_Data.sendDataOutTime = 0;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SAVE,CMD_SUN_SAVE_CALW,3);//一共四组先发第3组
        }
        else if(MySync_Data.save_calw == 3)
        {
            MySync_Data.sendDataOutTime = 0;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SAVE,CMD_SUN_SAVE_CALW,4);//一共四组先发第4组
        }
        else if(MySync_Data.save_calw == 4)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.SendData_flag = 0;
            }
        }
        break;
    }
    case SysSendIndex::CMD_SAVE_ONLINE:
    {
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.save_online = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SAVE,CMD_SUN_SAVE_ONLINE,1);
            MySync_Data.SendData_flag = 1;
        }

        if(MySync_Data.save_online == 1)
        {
            MySync_Data.sendDataOutTime = 0;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SAVE,CMD_SUN_SAVE_ONLINE,2);
        }
        if(MySync_Data.save_online == 2)
        {
            MySync_Data.sendDataOutTime = 0;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SAVE,CMD_SUN_SAVE_ONLINE,3);
        }
        if(MySync_Data.save_online == 3)
        {
            MySync_Data.sendDataOutTime = 0;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SAVE,CMD_SUN_SAVE_ONLINE,4);
        }
        else if(MySync_Data.save_online == 4)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.SendData_flag = 0;
            }
        }
        break;
    }
    case SysSendIndex::CMD_PRODUCT_PAR:
    {
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.product_par = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_PRODUCT,CMD_SUN_PRODUCT_PAR);
            MySync_Data.SendData_flag = 1;
        }

        if(MySync_Data.product_par == true)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.SendData_flag = 0;
            }
        }
        break;
    }
    case SysSendIndex::CMD_PRODUCT_SENIOR:
    {
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.product_senior = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_PRODUCT,CMD_SUN_PRODUCT_SENIOR);
            MySync_Data.SendData_flag = 1;
        }

        if(MySync_Data.product_senior == true)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.SendData_flag = 0;
            }
        }
        break;
    }
    case SysSendIndex::CMD_PRODUCT_INTERNET:
    {
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.product_internet = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_PRODUCT,CMD_SUN_PRODUCT_INTERNET);
            MySync_Data.SendData_flag = 1;
        }

        if(MySync_Data.product_internet == true)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.SendData_flag = 0;
            }
        }
        break;
    }
    case SysSendIndex::CMD_SERVO_ACC_DEC:
    {
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.servo_acc_dec = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SERVO,CMD_SUN_SERVO_ACC_DEC,1);
            MySync_Data.SendData_flag = 1;
        }

        if(MySync_Data.servo_acc_dec == 1)
        {
            MySync_Data.sendDataOutTime = 0;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SERVO,CMD_SUN_SERVO_ACC_DEC,2);
        }
        else if(MySync_Data.servo_acc_dec == 2)
        {
            MySync_Data.sendDataOutTime = 0;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SERVO,CMD_SUN_SERVO_ACC_DEC,3);
        }
        else if(MySync_Data.servo_acc_dec == 3)
        {
            MySync_Data.sendDataOutTime = 0;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SERVO,CMD_SUN_SERVO_ACC_DEC,4);
        }
        else if(MySync_Data.servo_acc_dec == 4)
        {
            MySync_Data.sendDataOutTime = 0;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SERVO,CMD_SUN_SERVO_ACC_DEC,5);
        }
        else if(MySync_Data.servo_acc_dec == 5)
        {
            MySync_Data.sendDataOutTime = 0;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SERVO,CMD_SUN_SERVO_ACC_DEC,6);
        }
        else if(MySync_Data.servo_acc_dec == 6)
        {
            MySync_Data.sendDataOutTime = 0;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SERVO,CMD_SUN_SERVO_ACC_DEC,7);
        }
        else if(MySync_Data.servo_acc_dec == 7)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.SendData_flag = 0;
            }
        }
        break;
    }
    case SysSendIndex::CMD_SERVO_MAX_SPEED:
    {
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.servo_max_speed = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SERVO,CMD_SUN_SERVO_MAX_SPEED);
            MySync_Data.SendData_flag = 1;
        }

        if(MySync_Data.servo_max_speed == true)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.SendData_flag = 0;
            }
        }
        break;
    }
    case SysSendIndex::CMD_SERVO_TOLERANCE:
    {
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.servo_tolerance = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SERVO,CMD_SUN_SERVO_TOLERANCE);
            MySync_Data.SendData_flag = 1;
        }

        if(MySync_Data.servo_tolerance == true)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.SendData_flag = 0;
            }
        }
        break;
    }
    case SysSendIndex::CMD_SP_AREA:
    {
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.sp_area = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SP,CMD_SUN_SP_AREA,1);
            MySync_Data.SendData_flag = 1;
        }

        if(MySync_Data.sp_area == 1)
        {
            MySync_Data.sendDataOutTime = 0;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SP,CMD_SUN_SP_AREA,2);
        }
        else if(MySync_Data.sp_area == 2)
        {
            MySync_Data.sendDataOutTime = 0;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SP,CMD_SUN_SP_AREA,3);
        }
        else if(MySync_Data.sp_area == 3)
        {
            MySync_Data.sendDataOutTime = 0;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SP,CMD_SUN_SP_AREA,4);
        }
        else if(MySync_Data.sp_area == 4)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.SendData_flag = 0;
            }
        }
        break;
    }
    case SysSendIndex::CMD_SP_AXIS_LIMIT:
    {
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.sp_axis_limit = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SP,CMD_SUN_SP_AXIS_LIMIT,1);
            MySync_Data.SendData_flag = 1;
        }

        if(MySync_Data.sp_axis_limit == 1)
        {
            MySync_Data.sendDataOutTime = 0;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SP,CMD_SUN_SP_AXIS_LIMIT,2);
        }
        else if(MySync_Data.sp_axis_limit == 2)
        {
            MySync_Data.sendDataOutTime = 0;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SP,CMD_SUN_SP_AXIS_LIMIT,3);
        }
        else if(MySync_Data.sp_axis_limit == 3)
        {
            MySync_Data.sendDataOutTime = 0;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SP,CMD_SUN_SP_AXIS_LIMIT,4);
        }
        else if(MySync_Data.sp_axis_limit == 4)
        {
            MySync_Data.sendDataOutTime = 0;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SP,CMD_SUN_SP_AXIS_LIMIT,5);
        }
        else if(MySync_Data.sp_axis_limit == 5)
        {
            MySync_Data.sendDataOutTime = 0;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SP,CMD_SUN_SP_AXIS_LIMIT,6);
        }
        else if(MySync_Data.sp_axis_limit == 6)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.SendData_flag = 0;
            }
        }
        break;
    }
    case SysSendIndex::CMD_SP_RAMPAGE_LIMIT:
    {
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.sp_rampage_limit = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SP,CMD_SUN_SP_RAMPAGE_LIMIT);
            MySync_Data.SendData_flag = 1;
        }

        if(MySync_Data.sp_rampage_limit == true)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.SendData_flag = 0;
            }
        }
        break;
    }
    case SysSendIndex::CMD_MAC_AXIS:
    {
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.mac_aixs = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_MAC,CMD_SUN_MAC_AXIS,1);
            MySync_Data.SendData_flag = 1;
        }

        if(MySync_Data.mac_aixs == 1)
        {
            MySync_Data.sendDataOutTime = 0;
            g_Usart->ExtendSendParDeal(CMD_MAIN_MAC,CMD_SUN_MAC_AXIS,2);
        }
        else if(MySync_Data.mac_aixs == 2)
        {
            MySync_Data.sendDataOutTime = 0;
            g_Usart->ExtendSendParDeal(CMD_MAIN_MAC,CMD_SUN_MAC_AXIS,3);
        }
        else if(MySync_Data.mac_aixs == 3)
        {
            MySync_Data.sendDataOutTime = 0;
            g_Usart->ExtendSendParDeal(CMD_MAIN_MAC,CMD_SUN_MAC_AXIS,4);
        }
        else if(MySync_Data.mac_aixs == 4)
        {
            MySync_Data.sendDataOutTime = 0;
            g_Usart->ExtendSendParDeal(CMD_MAIN_MAC,CMD_SUN_MAC_AXIS,5);
        }
        else if(MySync_Data.mac_aixs == 5)
        {
            MySync_Data.sendDataOutTime = 0;
            g_Usart->ExtendSendParDeal(CMD_MAIN_MAC,CMD_SUN_MAC_AXIS,6);
        }
        else if(MySync_Data.mac_aixs == 6)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.SendData_flag = 0;
            }
        }
        break;
    }
    case SysSendIndex::CMD_MAC_LIMIT_SWT:
    {
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.mac_limit_swt = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_MAC,CMD_SUN_MAC_LIMIT_SWT);
            MySync_Data.SendData_flag = 1;
        }

        if(MySync_Data.mac_limit_swt == true)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.SendData_flag = 0;
            }
        }
        break;
    }
    case SysSendIndex::CMD_MAC_STRUCT:
    {
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.mac_struct = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_MAC,CMD_SUN_MAC_STRUCT);
            MySync_Data.SendData_flag = 1;
        }

        if(MySync_Data.mac_struct == true)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.SendData_flag = 0;
            }
        }
        break;
    }
    case SysSendIndex::CMD_MAC_SERVO:
    {
        static uint8_t SendOldIndex = 0;
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.mac_servo = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_MAC,CMD_SUN_MAC_SERVO,1);
            MySync_Data.SendData_flag = 1;
        }

        if((MySync_Data.mac_servo>0) && (MySync_Data.mac_servo < 100) && SendOldIndex != MySync_Data.mac_servo)
        {
            SendOldIndex = MySync_Data.mac_servo;
            MySync_Data.sendDataOutTime = 0;
            g_Usart->ExtendSendParDeal(CMD_MAIN_MAC,CMD_SUN_MAC_SERVO,MySync_Data.mac_servo);
        }
        else if(MySync_Data.mac_servo == 100)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.SendData_flag = 0;
            }
        }
        break;
    }
    case SysSendIndex::CMD_MAC_ORIGIN:
    {
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.mac_origin = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_MAC,CMD_SUN_MAC_ORIGIN);
            MySync_Data.SendData_flag = 1;
        }

        if(MySync_Data.mac_origin == true)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.SendData_flag = 0;
            }
        }
        break;
    }
    case SysSendIndex::CMD_STACK_PAR:
    {
        static uint8_t SendOldIndex = 0;
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.stack_par = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_STACK,CMD_SUN_STACK_PAR,1);
            MySync_Data.SendData_flag = 1;
        }

        if((MySync_Data.stack_par>0) && (MySync_Data.stack_par < 8) && SendOldIndex != MySync_Data.stack_par)
        {
            SendOldIndex = MySync_Data.stack_par;
            MySync_Data.sendDataOutTime = 0;
            g_Usart->ExtendSendParDeal(CMD_MAIN_STACK,CMD_SUN_STACK_PAR,MySync_Data.stack_par);
        }
        if(MySync_Data.stack_par == 8)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.SendData_flag = 0;
            }
        }
        break;
    }
    case SysSendIndex::CMD_STACK_POINT:
    {
        static uint8_t SendOldIndex = 0;
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.stack_point = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_STACK,CMD_SUN_STACK_POINT,1);
            MySync_Data.SendData_flag = 1;
        }

        if((MySync_Data.stack_point >0) && (MySync_Data.stack_point < 8) && SendOldIndex != MySync_Data.stack_point)
        {
            SendOldIndex = MySync_Data.stack_point;
            MySync_Data.sendDataOutTime = 0;
            g_Usart->ExtendSendParDeal(CMD_MAIN_STACK,CMD_SUN_STACK_POINT,MySync_Data.stack_point);
        }
        else if(MySync_Data.stack_point == 8)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.SendData_flag = 0;
            }
        }
        break;
    }
    case SysSendIndex::CMD_STAC_SET:
    {
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.stack_set = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_STACK,CMD_SUN_STACK_SET);
            MySync_Data.SendData_flag = 1;
        }

        if(MySync_Data.stack_set == true)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.SendData_flag = 0;
            }
        }
        break;
    }
    case SysSendIndex::CMD_FINISH:
    {
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.sysdatafinish = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_STACK,CMD_SUN_SYSDATA_FINISH,1);
            MySync_Data.SendData_flag = 1;
        }

        if(MySync_Data.sysdatafinish == true)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.SendData_flag = 0;
            }
        }

        break;
    }
    default:
        break;
    }
    if(MySync_Data.SendDataStep == SysSendIndex::CMD_FINISH)
    {
        UsartTimer->stop();
        MySync_Data.SendData_flag = 0;
        emit DataSycStateSignal(SysSendIndex::CMD_FINISH);//参数同步到达那一步
        return 1;
    }
    else
    {
        emit DataSycStateSignal(MySync_Data.SendDataStep);//参数同步到达那一步
        return 0;
    }
}
#else
//开机参数和程序下发
uint8_t Usart::DataSyc()
{
    switch (MySync_Data.SendDataStep) {
    case SysSendIndex::CMD_OUT_TYPE://信号设置-输出类型
    {
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.out_type_State = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SIGNAL,CMD_SUN_SIGNAL_OUT_TYPE);
            MySync_Data.SendData_flag = 1;
            timer.restart();
            qDebug() << "addWidget(setWidget)：" << timer.elapsed() << "毫秒";
        }
        if(MySync_Data.out_type_State == true)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
            MySync_Data.sendDataNum=0;
        }
        else
        {
            MySync_Data.sendDataOutTime++;//一次5ms
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {//超过1s未接收到反馈重新发送
                qDebug() << "addWidget(setWidget)：" << timer.elapsed() << "毫秒";
               MySync_Data.sendDataNum++;
                if(MySync_Data.sendDataNum>=5)
                {
                    MySync_Data.SendDataStep=SysSendIndex::CMD_SENDERROR;
                }
                else
                {
                    MySync_Data.SendData_flag = 0;
                }
            }
        }
        break;
    }
    case SysSendIndex::CMD_INTERLOCK:
    {
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.interlock_State = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SIGNAL,CMD_SUN_SIGNAL_INTERLOCK);
            MySync_Data.SendData_flag = 1;
        }

        if(MySync_Data.interlock_State == true)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
            MySync_Data.sendDataNum=0;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.sendDataNum=0;
                if(MySync_Data.sendDataNum>=5)
                {
                    MySync_Data.SendDataStep=SysSendIndex::CMD_SENDERROR;
                }
                else
                {
                    MySync_Data.SendData_flag = 0;
                }

            }
        }
        break;
    }
    case SysSendIndex::CMD_IN_FUNC_DEF:
    {
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.in_func_def_State = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SIGNAL,CMD_SUN_SIGNAL_IN_FUNC_DEF);
            MySync_Data.SendData_flag = 1;
        }

        if(MySync_Data.in_func_def_State == true)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
            MySync_Data.sendDataNum=0;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.sendDataNum=0;
                if(MySync_Data.sendDataNum>=5)
                {
                    MySync_Data.SendDataStep=SysSendIndex::CMD_SENDERROR;
                }
                else
                {
                    MySync_Data.SendData_flag = 0;
                }
            }
        }
        break;
    }
    case SysSendIndex::CMD_OUT_FUNC_DEF:
    {
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.out_func_def_State = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SIGNAL,CMD_SUN_SIGNAL_OUT_FUNC_DEF);
            MySync_Data.SendData_flag = 1;
        }

        if(MySync_Data.out_func_def_State == true)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
            MySync_Data.sendDataNum=0;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.sendDataNum++;
                if(MySync_Data.sendDataNum>=5)
                {
                    MySync_Data.SendDataStep=SysSendIndex::CMD_SENDERROR;
                }
                else
                {
                    MySync_Data.SendData_flag = 0;
                }
            }
        }
        break;
    }
    case SysSendIndex::CMD_OUT_RELEVENCY:
    {
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.out_relevency_State = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SIGNAL,CMD_SUN_SIGNAL_OUT_RELEVENCY);
            MySync_Data.SendData_flag = 1;
        }

        if(MySync_Data.out_relevency_State == true)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
            MySync_Data.sendDataNum=0;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.sendDataNum++;
                if(MySync_Data.sendDataNum>=5)
                {
                    MySync_Data.SendDataStep=SysSendIndex::CMD_SENDERROR;
                }
                else
                {
                    MySync_Data.SendData_flag = 0;
                }
            }
        }
        break;
    }
    case SysSendIndex::CMD_RELATE_OUT:
    {
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.out_relate_out = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SIGNAL,CMD_SUN_SIGNAL_RELATE_OUT);
            MySync_Data.SendData_flag = 1;
        }

        if(MySync_Data.out_relate_out == true)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
            MySync_Data.sendDataNum=0;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.sendDataNum++;
                if(MySync_Data.sendDataNum>=5)
                {
                    MySync_Data.SendDataStep=SysSendIndex::CMD_SENDERROR;
                }
                else
                {
                    MySync_Data.SendData_flag = 0;
                }
            }
        }
        break;
    }
    case SysSendIndex::CMD_KEY:
    {
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.out_key = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SIGNAL,CMD_SUN_SIGNAL_KEY);
            MySync_Data.SendData_flag = 1;
        }

        if(MySync_Data.out_key == true)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
            MySync_Data.sendDataNum=0;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.sendDataNum++;
                if(MySync_Data.sendDataNum>=5)
                {
                    MySync_Data.SendDataStep=SysSendIndex::CMD_SENDERROR;
                }
                else
                {
                    MySync_Data.SendData_flag = 0;
                }
            }
        }
        break;
    }
    case SysSendIndex::CMD_SENIOR:
    {
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.senior = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SIGNAL,CMD_SUN_SIGNAL_SENIOR);
            MySync_Data.SendData_flag = 1;
        }

        if(MySync_Data.senior == true)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
            MySync_Data.sendDataNum=0;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.sendDataNum++;
                if(MySync_Data.sendDataNum>=5)
                {
                    MySync_Data.SendDataStep=SysSendIndex::CMD_SENDERROR;
                }
                else
                {
                    MySync_Data.SendData_flag = 0;
                }
            }
        }
        break;
    }
    case SysSendIndex::CMD_SENIOR_PORT:
    {
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.senior_port = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SIGNAL,CMD_SUN_SIGNAL_SENIOR_PORT);
            MySync_Data.SendData_flag = 1;
        }

        if(MySync_Data.senior_port == true)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
            MySync_Data.sendDataNum=0;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.sendDataNum++;
                if(MySync_Data.sendDataNum>=5)
                {
                    MySync_Data.SendDataStep=SysSendIndex::CMD_SENDERROR;
                }
                else
                {
                    MySync_Data.SendData_flag = 0;
                }
            }
        }
        break;
    }
    case SysSendIndex::CMD_SAVE_MACHINE:
    {

        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.save_machine = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SAVE,CMD_SUN_SAVE_MACHINE);
            MySync_Data.SendData_flag = 1;
        }

        if(MySync_Data.save_machine == true)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
            MySync_Data.sendDataNum=0;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.sendDataNum++;
                if(MySync_Data.sendDataNum>=5)
                {
                    MySync_Data.SendDataStep=SysSendIndex::CMD_SENDERROR;
                }
                else
                {
                    MySync_Data.SendData_flag = 0;
                }
            }
        }
        break;
    }
    case SysSendIndex::CMD_SAVE_STACK:
    {
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.save_stack = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SAVE,CMD_SUN_SAVE_STACK);
            MySync_Data.SendData_flag = 1;
        }

        if(MySync_Data.save_stack == true)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
            MySync_Data.sendDataNum=0;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.sendDataNum++;
                if(MySync_Data.sendDataNum>=5)
                {
                    MySync_Data.SendDataStep=SysSendIndex::CMD_SENDERROR;
                }
                else
                {
                    MySync_Data.SendData_flag = 0;
                }
            }
        }
        break;
    }
    case SysSendIndex::CMD_SAVE_CALW:
    {

        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.save_calw = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SAVE,CMD_SUN_SAVE_CALW,1);//一共四组先发第1组
            MySync_Data.SendData_flag = 1;
        }

        if(MySync_Data.save_calw == 1)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.sendDataNum=0;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SAVE,CMD_SUN_SAVE_CALW,2);//一共四组先发第2组
        }
        else if(MySync_Data.save_calw == 2)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.sendDataNum=0;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SAVE,CMD_SUN_SAVE_CALW,3);//一共四组先发第3组
        }
        else if(MySync_Data.save_calw == 3)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.sendDataNum=0;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SAVE,CMD_SUN_SAVE_CALW,4);//一共四组先发第4组
        }
        else if(MySync_Data.save_calw == 4)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
            MySync_Data.sendDataNum=0;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.sendDataNum++;
                if(MySync_Data.sendDataNum>=5)
                {
                    MySync_Data.SendDataStep=SysSendIndex::CMD_SENDERROR;
                }
                else
                {
                    MySync_Data.SendData_flag = 0;
                }
            }
        }
        break;
    }
    case SysSendIndex::CMD_SAVE_ONLINE:
    {
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.save_online = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SAVE,CMD_SUN_SAVE_ONLINE,1);
            MySync_Data.SendData_flag = 1;
        }

        if(MySync_Data.save_online == 1)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.sendDataNum=0;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SAVE,CMD_SUN_SAVE_ONLINE,2);
        }
        if(MySync_Data.save_online == 2)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.sendDataNum=0;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SAVE,CMD_SUN_SAVE_ONLINE,3);
        }
        if(MySync_Data.save_online == 3)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.sendDataNum=0;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SAVE,CMD_SUN_SAVE_ONLINE,4);
        }
        else if(MySync_Data.save_online == 4)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
            MySync_Data.sendDataNum=0;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.sendDataNum++;
                if(MySync_Data.sendDataNum>=5)
                {
                    MySync_Data.SendDataStep=SysSendIndex::CMD_SENDERROR;
                }
                else
                {
                    MySync_Data.SendData_flag = 0;
                }
            }
        }
        break;
    }
    case SysSendIndex::CMD_PRODUCT_PAR:
    {
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.product_par = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_PRODUCT,CMD_SUN_PRODUCT_PAR);
            MySync_Data.SendData_flag = 1;
        }

        if(MySync_Data.product_par == true)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
            MySync_Data.sendDataNum=0;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.sendDataNum++;
                if(MySync_Data.sendDataNum>=5)
                {
                    MySync_Data.SendDataStep=SysSendIndex::CMD_SENDERROR;
                }
                else
                {
                    MySync_Data.SendData_flag = 0;
                }
            }
        }
        break;
    }
    case SysSendIndex::CMD_PRODUCT_SENIOR:
    {
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.product_senior = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_PRODUCT,CMD_SUN_PRODUCT_SENIOR);
            MySync_Data.SendData_flag = 1;
        }

        if(MySync_Data.product_senior == true)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
            MySync_Data.sendDataNum=0;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.sendDataNum++;
                if(MySync_Data.sendDataNum>=5)
                {
                    MySync_Data.SendDataStep=SysSendIndex::CMD_SENDERROR;
                }
                else
                {
                    MySync_Data.SendData_flag = 0;
                }
            }
        }
        break;
    }
    case SysSendIndex::CMD_PRODUCT_INTERNET:
    {
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.product_internet = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_PRODUCT,CMD_SUN_PRODUCT_INTERNET);
            MySync_Data.SendData_flag = 1;
        }

        if(MySync_Data.product_internet == true)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
            MySync_Data.sendDataNum=0;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.sendDataNum++;
                if(MySync_Data.sendDataNum>=5)
                {
                    MySync_Data.SendDataStep=SysSendIndex::CMD_SENDERROR;
                }
                else
                {
                    MySync_Data.SendData_flag = 0;
                }
            }
        }
        break;
    }
    case SysSendIndex::CMD_SERVO_ACC_DEC:
    {
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.servo_acc_dec = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SERVO,CMD_SUN_SERVO_ACC_DEC,1);
            MySync_Data.SendData_flag = 1;
        }

        if(MySync_Data.servo_acc_dec == 1)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.sendDataNum=0;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SERVO,CMD_SUN_SERVO_ACC_DEC,2);
        }
        else if(MySync_Data.servo_acc_dec == 2)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.sendDataNum=0;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SERVO,CMD_SUN_SERVO_ACC_DEC,3);
        }
        else if(MySync_Data.servo_acc_dec == 3)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.sendDataNum=0;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SERVO,CMD_SUN_SERVO_ACC_DEC,4);
        }
        else if(MySync_Data.servo_acc_dec == 4)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.sendDataNum=0;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SERVO,CMD_SUN_SERVO_ACC_DEC,5);
        }
        else if(MySync_Data.servo_acc_dec == 5)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.sendDataNum=0;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SERVO,CMD_SUN_SERVO_ACC_DEC,6);
        }
        else if(MySync_Data.servo_acc_dec == 6)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.sendDataNum=0;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SERVO,CMD_SUN_SERVO_ACC_DEC,7);
        }
        else if(MySync_Data.servo_acc_dec == 7)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
            MySync_Data.sendDataNum=0;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.sendDataNum++;
                if(MySync_Data.sendDataNum>=5)
                {
                    MySync_Data.SendDataStep=SysSendIndex::CMD_SENDERROR;
                }
                else
                {
                    MySync_Data.SendData_flag = 0;
                }
            }
        }
        break;
    }
    case SysSendIndex::CMD_SERVO_MAX_SPEED:
    {
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.servo_max_speed = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SERVO,CMD_SUN_SERVO_MAX_SPEED);
            MySync_Data.SendData_flag = 1;
        }

        if(MySync_Data.servo_max_speed == true)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
            MySync_Data.sendDataNum=0;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.sendDataNum++;
                if(MySync_Data.sendDataNum>=5)
                {
                    MySync_Data.SendDataStep=SysSendIndex::CMD_SENDERROR;
                }
                else
                {
                    MySync_Data.SendData_flag = 0;
                }
            }
        }
        break;
    }
    case SysSendIndex::CMD_SERVO_TOLERANCE:
    {
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.servo_tolerance = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SERVO,CMD_SUN_SERVO_TOLERANCE);
            MySync_Data.SendData_flag = 1;
        }

        if(MySync_Data.servo_tolerance == true)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
            MySync_Data.sendDataNum=0;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.sendDataNum++;
                if(MySync_Data.sendDataNum>=5)
                {
                    MySync_Data.SendDataStep=SysSendIndex::CMD_SENDERROR;
                }
                else
                {
                    MySync_Data.SendData_flag = 0;
                }
            }
        }
        break;
    }
    case SysSendIndex::CMD_SP_AREA:
    {
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.sp_area = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SP,CMD_SUN_SP_AREA,1);
            MySync_Data.SendData_flag = 1;
        }

        if(MySync_Data.sp_area == 1)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.sendDataNum=0;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SP,CMD_SUN_SP_AREA,2);
        }
        else if(MySync_Data.sp_area == 2)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.sendDataNum=0;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SP,CMD_SUN_SP_AREA,3);
        }
        else if(MySync_Data.sp_area == 3)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.sendDataNum=0;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SP,CMD_SUN_SP_AREA,4);
        }
        else if(MySync_Data.sp_area == 4)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
            MySync_Data.sendDataNum=0;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.sendDataNum++;
                if(MySync_Data.sendDataNum>=5)
                {
                    MySync_Data.SendDataStep=SysSendIndex::CMD_SENDERROR;
                }
                else
                {
                    MySync_Data.SendData_flag = 0;
                }
            }
        }
        break;
    }
    case SysSendIndex::CMD_SP_AXIS_LIMIT:
    {
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.sp_axis_limit = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SP,CMD_SUN_SP_AXIS_LIMIT,1);
            MySync_Data.SendData_flag = 1;
        }

        if(MySync_Data.sp_axis_limit == 1)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.sendDataNum=0;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SP,CMD_SUN_SP_AXIS_LIMIT,2);
        }
        else if(MySync_Data.sp_axis_limit == 2)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.sendDataNum=0;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SP,CMD_SUN_SP_AXIS_LIMIT,3);
        }
        else if(MySync_Data.sp_axis_limit == 3)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.sendDataNum=0;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SP,CMD_SUN_SP_AXIS_LIMIT,4);
        }
        else if(MySync_Data.sp_axis_limit == 4)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.sendDataNum=0;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SP,CMD_SUN_SP_AXIS_LIMIT,5);
        }
        else if(MySync_Data.sp_axis_limit == 5)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.sendDataNum=0;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SP,CMD_SUN_SP_AXIS_LIMIT,6);
        }
        else if(MySync_Data.sp_axis_limit == 6)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
            MySync_Data.sendDataNum=0;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.sendDataNum++;
                if(MySync_Data.sendDataNum>=5)
                {
                    MySync_Data.SendDataStep=SysSendIndex::CMD_SENDERROR;
                }
                else
                {
                    MySync_Data.SendData_flag = 0;
                }
            }
        }
        break;
    }
    case SysSendIndex::CMD_SP_RAMPAGE_LIMIT:
    {
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.sp_rampage_limit = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_SP,CMD_SUN_SP_RAMPAGE_LIMIT);
            MySync_Data.SendData_flag = 1;
        }

        if(MySync_Data.sp_rampage_limit == true)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
            MySync_Data.sendDataNum=0;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.sendDataNum++;
                if(MySync_Data.sendDataNum>=5)
                {
                    MySync_Data.SendDataStep=SysSendIndex::CMD_SENDERROR;
                }
                else
                {
                    MySync_Data.SendData_flag = 0;
                }
            }
        }
        break;
    }
    case SysSendIndex::CMD_MAC_AXIS:
    {
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.mac_aixs = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_MAC,CMD_SUN_MAC_AXIS,1);
            MySync_Data.SendData_flag = 1;
        }

        if(MySync_Data.mac_aixs == 1)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.sendDataNum=0;
            g_Usart->ExtendSendParDeal(CMD_MAIN_MAC,CMD_SUN_MAC_AXIS,2);
        }
        else if(MySync_Data.mac_aixs == 2)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.sendDataNum=0;
            g_Usart->ExtendSendParDeal(CMD_MAIN_MAC,CMD_SUN_MAC_AXIS,3);
        }
        else if(MySync_Data.mac_aixs == 3)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.sendDataNum=0;
            g_Usart->ExtendSendParDeal(CMD_MAIN_MAC,CMD_SUN_MAC_AXIS,4);
        }
        else if(MySync_Data.mac_aixs == 4)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.sendDataNum=0;
            g_Usart->ExtendSendParDeal(CMD_MAIN_MAC,CMD_SUN_MAC_AXIS,5);
        }
        else if(MySync_Data.mac_aixs == 5)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.sendDataNum=0;
            g_Usart->ExtendSendParDeal(CMD_MAIN_MAC,CMD_SUN_MAC_AXIS,6);
        }
        else if(MySync_Data.mac_aixs == 6)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
            MySync_Data.sendDataNum=0;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.sendDataNum++;
                if(MySync_Data.sendDataNum>=5)
                {
                    MySync_Data.SendDataStep=SysSendIndex::CMD_SENDERROR;
                }
                else
                {
                    MySync_Data.SendData_flag = 0;
                }
            }
        }
        break;
    }
    case SysSendIndex::CMD_MAC_LIMIT_SWT:
    {
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.mac_limit_swt = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_MAC,CMD_SUN_MAC_LIMIT_SWT);
            MySync_Data.SendData_flag = 1;
        }

        if(MySync_Data.mac_limit_swt == true)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
            MySync_Data.sendDataNum=0;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.sendDataNum++;
                if(MySync_Data.sendDataNum>=5)
                {
                    MySync_Data.SendDataStep=SysSendIndex::CMD_SENDERROR;
                }
                else
                {
                    MySync_Data.SendData_flag = 0;
                }
            }
        }
        break;
    }
    case SysSendIndex::CMD_MAC_STRUCT:
    {
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.mac_struct = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_MAC,CMD_SUN_MAC_STRUCT);
            MySync_Data.SendData_flag = 1;
        }

        if(MySync_Data.mac_struct == true)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
            MySync_Data.sendDataNum=0;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.sendDataNum++;
                if(MySync_Data.sendDataNum>=5)
                {
                    MySync_Data.SendDataStep=SysSendIndex::CMD_SENDERROR;
                }
                else
                {
                    MySync_Data.SendData_flag = 0;
                }
            }
        }
        break;
    }
    case SysSendIndex::CMD_MAC_SERVO:
    {
        static uint8_t SendOldIndex = 0;
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.mac_servo = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_MAC,CMD_SUN_MAC_SERVO,1);
            MySync_Data.SendData_flag = 1;
        }

        if((MySync_Data.mac_servo>0) && (MySync_Data.mac_servo < 100) && SendOldIndex != MySync_Data.mac_servo)
        {
            SendOldIndex = MySync_Data.mac_servo;
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.sendDataNum=0;
            g_Usart->ExtendSendParDeal(CMD_MAIN_MAC,CMD_SUN_MAC_SERVO,MySync_Data.mac_servo);
        }
        else if(MySync_Data.mac_servo == 100)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
            MySync_Data.sendDataNum=0;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.sendDataNum++;
                if(MySync_Data.sendDataNum>=5)
                {
                    MySync_Data.SendDataStep=SysSendIndex::CMD_SENDERROR;
                }
                else
                {
                    MySync_Data.SendData_flag = 0;
                }
            }
        }
        break;
    }
    case SysSendIndex::CMD_MAC_ORIGIN:
    {
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.mac_origin = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_MAC,CMD_SUN_MAC_ORIGIN);
            MySync_Data.SendData_flag = 1;
        }

        if(MySync_Data.mac_origin == true)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
            MySync_Data.sendDataNum=0;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.sendDataNum++;
                if(MySync_Data.sendDataNum>=5)
                {
                    MySync_Data.SendDataStep=SysSendIndex::CMD_SENDERROR;
                }
                else
                {
                    MySync_Data.SendData_flag = 0;
                }
            }
        }
        break;
    }
    case SysSendIndex::CMD_STACK_PAR:
    {
        static uint8_t SendOldIndex = 0;
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.stack_par = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_STACK,CMD_SUN_STACK_PAR,1);
            MySync_Data.SendData_flag = 1;
        }

        if((MySync_Data.stack_par>0) && (MySync_Data.stack_par < 8) && SendOldIndex != MySync_Data.stack_par)
        {
            SendOldIndex = MySync_Data.stack_par;
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.sendDataNum=0;
            g_Usart->ExtendSendParDeal(CMD_MAIN_STACK,CMD_SUN_STACK_PAR,MySync_Data.stack_par);
        }
        if(MySync_Data.stack_par == 8)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
            MySync_Data.sendDataNum=0;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.sendDataNum++;
                if(MySync_Data.sendDataNum>=5)
                {
                    MySync_Data.SendDataStep=SysSendIndex::CMD_SENDERROR;
                }
                else
                {
                    MySync_Data.SendData_flag = 0;
                }
            }
        }
        break;
    }
    case SysSendIndex::CMD_STACK_POINT:
    {
        static uint8_t SendOldIndex = 0;
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.stack_point = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_STACK,CMD_SUN_STACK_POINT,1);
            MySync_Data.SendData_flag = 1;
        }

        if((MySync_Data.stack_point >0) && (MySync_Data.stack_point < 8) && SendOldIndex != MySync_Data.stack_point)
        {
            SendOldIndex = MySync_Data.stack_point;
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.sendDataNum=0;
            g_Usart->ExtendSendParDeal(CMD_MAIN_STACK,CMD_SUN_STACK_POINT,MySync_Data.stack_point);
        }
        else if(MySync_Data.stack_point == 8)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
            MySync_Data.sendDataNum=0;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.sendDataNum++;
                if(MySync_Data.sendDataNum>=5)
                {
                    MySync_Data.SendDataStep=SysSendIndex::CMD_SENDERROR;
                }
                else
                {
                    MySync_Data.SendData_flag = 0;
                }
            }
        }
        break;
    }
    case SysSendIndex::CMD_STAC_SET:
    {
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.stack_set = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_STACK,CMD_SUN_STACK_SET);
            MySync_Data.SendData_flag = 1;
        }

        if(MySync_Data.stack_set == true)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
            MySync_Data.sendDataNum=0;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.sendDataNum++;
                if(MySync_Data.sendDataNum>=5)
                {
                    MySync_Data.SendDataStep=SysSendIndex::CMD_SENDERROR;
                }
                else
                {
                    MySync_Data.SendData_flag = 0;
                }
            }
        }
        break;
    }
    case SysSendIndex::CMD_END:
    {
        if(MySync_Data.SendData_flag == 0)
        {
            MySync_Data.sendDataOutTime = 0;
            MySync_Data.sysdatafinish = false;
            g_Usart->ExtendSendParDeal(CMD_MAIN_READ,CMD_SUN_SYSDATA_FINISH);
            MySync_Data.SendData_flag = 1;
        }

        if(MySync_Data.sysdatafinish == true)
        {
            MySync_Data.SendData_flag = 0;
            MySync_Data.SendDataStep++;
            MySync_Data.sendDataNum=0;
        }
        else
        {
            MySync_Data.sendDataOutTime++;
            if(MySync_Data.sendDataOutTime >= MySync_Data.OutTimenum)
            {
                MySync_Data.sendDataNum++;
                if(MySync_Data.sendDataNum>=5)
                {
                    MySync_Data.SendDataStep=SysSendIndex::CMD_SENDERROR;
                }
                else
                {
                    MySync_Data.SendData_flag = 0;
                }
            }
        }

        break;
    }
    case SysSendIndex::CMD_SENDERROR:
    {
        break;
    }
    default:
        break;
    }
    if(MySync_Data.SendDataStep == SysSendIndex::CMD_FINISH)
    {
        UsartTimer->stop();
        MySync_Data.SendData_flag = 0;
        emit DataSycStateSignal(SysSendIndex::CMD_FINISH);//发送同步结果
        return 1;
    }
    else if(MySync_Data.SendDataStep == SysSendIndex::CMD_SENDERROR)
    {
        UsartTimer->stop();
        MySync_Data.SendData_flag = 0;
        emit DataSycStateSignal(SysSendIndex::CMD_SENDERROR);//发送同步结果
    }
    else
    {
        emit DataSycStateSignal(MySync_Data.SendDataStep);//参数同步到达那一步
        return 0;
    }
}
#endif
//参数同步处理函数
void Usart::sync_data_handle(void)
{
    UsartTimer->start(20);
    timer.start();
    MySync_Data.SendDataStep = 0;
    MySync_Data.SendData_flag = 0;
    MySync_Data.sendDataOutTime = 0;
    MySync_Data.SysDataFlag = 1;
    MySync_Data.sendDataNum=0;
    MySync_Data.OutTimenum=3;
    DataSyc();
}
