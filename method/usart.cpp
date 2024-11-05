#include "usart.h"
Usart::Usart(QObject *parent):QObject(parent)
{
    m_serialPort=new QSerialPort(this);
    connect(m_serialPort, &QSerialPort::readyRead, this, &Usart::onReadyRead);
    connect(m_serialPort, &QSerialPort::errorOccurred,this,&Usart::onHandleError);
}

Usart::~Usart()
{
    m_serialPort->close();
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
//    m_serialPort->waitForBytesWritten(1000);
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
    uint16_t i = 0;
    uint16_t num = 0;
    uint8_t axis = 0;
    uint16_t index = 0;

    if(mainCmd == CMD_MAIN_SIGNAL)
    {//信号设置
        switch(sunCmd)
        {
        case CMD_SUN_SIGNAL_OUT_TYPE://输出类型
            for(i=0; i<OUT_PORT_TYPE_NUM; i++)
            {
                m_OutPortType[i][0] = recDataBuf[i*2];
                m_OutPortType[i][1] = recDataBuf[i*2+1];
            }
            break;
        case CMD_SUN_SIGNAL_INTERLOCK://互锁设置
            for(i=0; i<OUT_INTERLOCK_NUM; i++)
            {
                m_OutportInterlock[i][0] = recDataBuf[i*4];
                m_OutportInterlock[i][1] = recDataBuf[i*4+1];
                m_OutportInterlock[i][2] = recDataBuf[i*4+2];
                m_OutportInterlock[i][3] = recDataBuf[i*4+3];
            }
            break;
        case CMD_SUN_SIGNAL_IN_FUNC_DEF://输入端口自定义
            for(i=0; i<INPUT_TOTAL_NUM; i++)
            {
                m_InportFuncDefine[i] = recDataBuf[i];

            }
            break;
        case CMD_SUN_SIGNAL_OUT_FUNC_DEF://输出端口自定义
            for(i=0; i<OUTPUT_TOTAL_NUM; i++)
            {
                m_OutportFuncDefine[i] = recDataBuf[i];
            }
            break;
        case CMD_SUN_SIGNAL_OUT_RELEVENCY://预留关联
            for(i=0; i<OUT_PORT_RELEVANCY_NUM; i++)
            {
                m_OutportRelevancy[i][0] = recDataBuf[i*3];
                m_OutportRelevancy[i][1] = recDataBuf[i*3+1];
                m_OutportRelevancy[i][2] = recDataBuf[i*3+2];
            }
            break;
        case CMD_SUN_SIGNAL_RELATE_OUT://预留出类型
            for(i=0; i<OUT_PORT_RELEOUT_NUM; i++)
            {
                m_OutportReleteOut[i][0] = recDataBuf[i*2];
                m_OutportReleteOut[i][1] = recDataBuf[i*2+1];
            }
            break;
        case CMD_SUN_SIGNAL_KEY://按键/信号
            for(i=0; i<OPR_KEY_NUM; i++)
            {
                m_KeyFunc[i].keyType = recDataBuf[i*3];
                m_KeyFunc[i].funcNum = recDataBuf[i*3+1];
                m_KeyFunc[i].oprMode = recDataBuf[i*3+2];
            }

            for(i=0; i<OPR_LED_NUM; i++)
            {
                m_LedFunc[i].ledType = recDataBuf[OPR_KEY_NUM*3 + i*2];
                m_LedFunc[i].funcNum = recDataBuf[OPR_KEY_NUM*3 + i*2+1];
            }
            break;
        case CMD_SUN_SIGNAL_SENIOR://高级
            memcpy(&m_SeniorFunc, recDataBuf, sizeof(D_SeniorFuncStruct));					//使用拷贝函数需要注意结构体内的元素顺序以及4字节对其
            break;
        case CMD_SUN_SIGNAL_SENIOR_PORT://高级功能端口
            memcpy(&m_SeniorFuncPort, recDataBuf, sizeof(D_SeniorFuncPortStruct));	//使用拷贝函数需要注意结构体内的元素顺序以及4字节对其
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
            m_MachineSave.armFallAutoDoor1Swt = recDataBuf[0];
            m_MachineSave.armFallAutoDoor1SwtSta = recDataBuf[1];
            m_MachineSave.armFallAutoDoor2Swt = recDataBuf[2];
            m_MachineSave.armFallAutoDoor2SwtSta = recDataBuf[3];
            break;
        case CMD_SUN_SAVE_STACK://料仓安全
            m_StackSave.stackSaveIn1Roatore = recDataBuf[0];
            m_StackSave.stackSaveIn2Roatore = recDataBuf[1];
            break;
        case CMD_SUN_SAVE_CALW://卡爪安全
            num = recDataBuf[0] - 1;
            m_ClawSave[num].clawKeepoutSta = recDataBuf[1];
            index = 2;
            m_ClawSave[num].axisSelect_X = recDataBuf[index];
            index += 1;
            m_ClawSave[num].axisSelect_Z = recDataBuf[index];
            index += 1;
            m_ClawSave[num].clawKeepoutMinX = 0;
            m_ClawSave[num].clawKeepoutMinX |= (uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
            index += 4;
            m_ClawSave[num].clawKeepoutMaxX = 0;
            m_ClawSave[num].clawKeepoutMaxX |= (uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
            index += 4;
            m_ClawSave[num].clawKeepoutHighZ = 0;
            m_ClawSave[num].clawKeepoutHighZ |= (uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
            index += 4;
            m_ClawSave[num].cKeepoutMinX = 0;
            m_ClawSave[num].cKeepoutMinX |= (uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
            index += 4;
            m_ClawSave[num].cKeepoutMaxX = 0;
            m_ClawSave[num].cKeepoutMaxX |= (uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
            index += 4;
            m_ClawSave[num].cKeepoutHighZ = 0;
            m_ClawSave[num].cKeepoutHighZ |= (uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
            index += 4;
            m_ClawSave[num].cKeepoutPosMinC = 0;
            m_ClawSave[num].cKeepoutPosMinC |= (uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
            index += 4;
            m_ClawSave[num].cKeepoutPosMaxC = 0;
            m_ClawSave[num].cKeepoutPosMaxC |= (uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
            index += 4;
            m_ClawSave[num].cKeepoutNegMinC = 0;
            m_ClawSave[num].cKeepoutNegMinC |= (uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
            index += 4;
            m_ClawSave[num].cKeepoutNegMaxC = 0;
            m_ClawSave[num].cKeepoutNegMaxC |= (uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
            break;
        case CMD_SUN_SAVE_ONLINE://联机安全
            num = recDataBuf[0] - 1;
            index = 1;
            m_OnlineSave[num].mainSunMode = recDataBuf[index];
            index += 1;
            m_OnlineSave[num].pluseTime = (uint16_t)recDataBuf[index] + ((uint16_t)recDataBuf[index+1]<<8);
            index += 2;
            m_OnlineSave[num].onlineSelect = recDataBuf[index];
            index += 1;
            m_OnlineSave[num].axisNum = recDataBuf[index];
            index += 1;
            m_OnlineSave[num].areaInNum = recDataBuf[index];
            index += 1;
            m_OnlineSave[num].areaOutNum = recDataBuf[index];
            index += 1;
            m_OnlineSave[num].requestInNum = recDataBuf[index];
            index += 1;
            m_OnlineSave[num].requestOutNum = recDataBuf[index];
            index += 1;
            m_OnlineSave[num].a1A2MainPos = 0;
            m_OnlineSave[num].a1A2MainPos |= (uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
            index += 4;
            m_OnlineSave[num].a1A2SunPos = 0;
            m_OnlineSave[num].a1A2SunPos |= (uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
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
            m_ProductSet.waitProcessFinTime = recDataBuf[0];
            m_ProductSet.pluseOutTime = recDataBuf[1];
            m_ProductSet.airVlaveLimitTime = recDataBuf[2];
            m_ProductSet.autoCycTime = recDataBuf[3];
            m_ProductSet.alarmBuzzerTime = recDataBuf[4];
            m_ProductSet.chainReachDelay = recDataBuf[5];
            m_ProductSet.clampCheckFilterTime = recDataBuf[6];
            break;
        case CMD_SUN_PRODUCT_SENIOR://高级
            m_ProductSenior.lubricOpenTime = recDataBuf[0];
            m_ProductSenior.lubricCloseTime = recDataBuf[1];
            m_ProductSenior.lubricDelay = recDataBuf[2];
            m_ProductSenior.lubricGapCyc = recDataBuf[3];
            m_ProductSenior.productMemoryFlag = recDataBuf[4];
            break;
        case CMD_SUN_PRODUCT_INTERNET://物联网
            m_Internet.netCommType = recDataBuf[0];
            m_Internet.parityMode = recDataBuf[1];
            m_Internet.baudMode = recDataBuf[2];
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
            num = recDataBuf[0] - 1;
            if(num < AXIS_TOTAL_NUM)
            {//轴
                index = 1;
                m_AxisPar[num].accTime = (uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
                index += 4;
                m_AxisPar[num].decTime = (uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
                index += 4;
                m_AxisPar[num].accAcc = (uint16_t)recDataBuf[index] + ((uint16_t)recDataBuf[index+1]<<8);
                index += 2;
                m_AxisPar[num].decDec = (uint16_t)recDataBuf[index] + ((uint16_t)recDataBuf[index+1]<<8);
                index += 2;
            }
            else
            {//手轮
                index = 1;
                m_HandwheelPar.accTime = (uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
                index += 4;
                m_HandwheelPar.decTime = (uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
                index += 4;
                m_HandwheelPar.accAcc = (uint16_t)recDataBuf[index] + ((uint16_t)recDataBuf[index+1]<<8);
                index += 2;
                m_HandwheelPar.decDec = (uint16_t)recDataBuf[index] + ((uint16_t)recDataBuf[index+1]<<8);
                index += 2;
            }
            break;
        case CMD_SUN_SERVO_MAX_SPEED://最大速度
            for(i=0; i<AXIS_TOTAL_NUM; i++)
            {
                m_AxisPar[i].maxSpeed = (uint16_t)recDataBuf[i*2] + ((uint16_t)recDataBuf[i*2+1]<<8);
            }
            break;
        case CMD_SUN_SERVO_TOLERANCE://容差
            index = 0;
            m_DegreeParS.tolerance = 0;
            m_DegreeParS.tolerance |= (uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
            index += 4;
            m_DegreeParS.originDegree = 0;
            m_DegreeParS.originDegree |= (uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
            index += 4;
            m_DegreeParS.actionFinDegree = 0;
            m_DegreeParS.actionFinDegree |= (uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8);
            index += 2;
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
            num = recDataBuf[0] - 1;
            m_SaveArea[num].saveAreaSwt = recDataBuf[1];
            index = 2;
            for(i=0; i<STACK_AXIS_NUM; i++)
            {
                m_SaveArea[num].axisSelect[i] = recDataBuf[index+i];
            }
            index += STACK_AXIS_NUM;
            m_SaveArea[num].processFinishNum = recDataBuf[index];
            index += 1;
            m_SaveArea[num].machineA1Pos[STACK_X_AXIS] = 0;
            m_SaveArea[num].machineA1Pos[STACK_X_AXIS] |= (uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
            index += 4;
            m_SaveArea[num].machineA1Pos[STACK_Y_AXIS] = 0;
            m_SaveArea[num].machineA1Pos[STACK_Y_AXIS] |= (uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
            index += 4;
            m_SaveArea[num].machineA2Pos[STACK_X_AXIS] = 0;
            m_SaveArea[num].machineA2Pos[STACK_X_AXIS] |= (uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
            index += 4;
            m_SaveArea[num].machineA2Pos[STACK_Y_AXIS] = 0;
            m_SaveArea[num].machineA2Pos[STACK_Y_AXIS] |= (uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
            index += 4;
            m_SaveArea[num].starckB1Pos[STACK_X_AXIS] = 0;
            m_SaveArea[num].starckB1Pos[STACK_X_AXIS] |= (uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
            index += 4;
            m_SaveArea[num].starckB1Pos[STACK_Y_AXIS] = 0;
            m_SaveArea[num].starckB1Pos[STACK_Y_AXIS] |= (uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
            index += 4;
            m_SaveArea[num].starckB2Pos[STACK_X_AXIS] = 0;
            m_SaveArea[num].starckB2Pos[STACK_X_AXIS] |= (uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
            index += 4;
            m_SaveArea[num].starckB2Pos[STACK_Y_AXIS] = 0;
            m_SaveArea[num].starckB2Pos[STACK_Y_AXIS] |= (uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
            index += 4;
            m_SaveArea[num].machineA1Pos[STACK_Z_AXIS] = 0;
            m_SaveArea[num].machineA1Pos[STACK_Z_AXIS] |= (uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
            index += 4;
            m_SaveArea[num].machineA2Pos[STACK_Z_AXIS] = 0;
            m_SaveArea[num].machineA2Pos[STACK_Z_AXIS] |= (uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
            index += 4;
            m_SaveArea[num].machine_Z_WaitMaxPos = 0;
            m_SaveArea[num].machine_Z_WaitMaxPos |= (uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
            index += 4;
            m_SaveArea[num].machine_Z_FallMaxPos = 0;
            m_SaveArea[num].machine_Z_FallMaxPos |= (uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
            index += 4;
            m_SaveArea[num].machine_Z_InsideHigh = 0;
            m_SaveArea[num].machine_Z_InsideHigh |= (uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
            index += 4;
            m_SaveArea[num].stack_Z_StartMaxPos = 0;
            m_SaveArea[num].stack_Z_StartMaxPos |= (uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
            index += 4;
            m_SaveArea[num].stack_Z_FallMaxPos = 0;
            m_SaveArea[num].stack_Z_FallMaxPos |= (uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
            index += 4;
            break;
        case CMD_SUN_SP_AXIS_LIMIT://轴运动位置限定
            num = recDataBuf[0] - 1;
            index = 1;
            m_AxisPar[num].axisMinPos = 0;
            m_AxisPar[num].axisMinPos |= (uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
            index += 4;
            m_AxisPar[num].axisMaxPos = 0;
            m_AxisPar[num].axisMaxPos |= (uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
            index += 4;
            break;
        case CMD_SUN_SP_RAMPAGE_LIMIT://横行位置限定
            index = 0;
            m_DegreeParS.saveRampageHige_Z1 = 0;
            m_DegreeParS.saveRampageHige_Z1 |= (uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
            index += 4;
            m_DegreeParS.saveRampageHige_Z2 = 0;
            m_DegreeParS.saveRampageHige_Z2 |= (uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
            index += 4;
            m_DegreeParS.saveRotatePos_Y1 = 0;
            m_DegreeParS.saveRotatePos_Y1 |= (uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
            index += 4;
            m_DegreeParS.saveRotatePos_Y2 = 0;
            m_DegreeParS.saveRotatePos_Y2 |= (uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
            index += 4;
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
            num = recDataBuf[0] - 1;
            index = 1;
            m_AxisPar[num].circlePluseNum = (uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
            index += 4;
            m_AxisPar[num].circleDis = (uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
            index += 4;
            m_AxisPar[num].findOriginSpeed = (uint16_t)recDataBuf[index] + ((uint16_t)recDataBuf[index+1]<<8);
            index += 2;
            m_AxisPar[num].leaveOriginSpeed = (uint16_t)recDataBuf[index] + ((uint16_t)recDataBuf[index+1]<<8);
            index += 2;
            m_AxisPar[num].originOffset = (uint16_t)recDataBuf[index] + ((uint16_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
            index += 4;
            m_AxisPar[num].originDir = recDataBuf[index];
            index += 1;
            m_AxisPar[num].coordDir = recDataBuf[index];
            index += 1;
            break;
        case CMD_SUN_MAC_LIMIT_SWT://限位开关
            for(i=0; i<AXIS_TOTAL_NUM; i++)
            {
                m_AxisPar[i].limitPosSwt = recDataBuf[i*2];
                m_AxisPar[i].limitNegSwt = recDataBuf[i*2+1];
            }
            break;
        case CMD_SUN_MAC_STRUCT://结构
            for(i=0; i<AXIS_TOTAL_NUM; i++)
            {
                m_AxisPar[i].axisType = recDataBuf[i];
            }
            index = AXIS_TOTAL_NUM;
            for(i=0; i<AXIS_TOTAL_NUM; i++)
            {
                m_AxisPar[i].axisMoveMade = recDataBuf[i+index];
            }
            index += AXIS_TOTAL_NUM;
            m_ServoPar.encoderType = recDataBuf[index];
            index += 1;
            m_ServoPar.encoderBitNum = recDataBuf[index];
            index += 1;
            m_ServoPar.servoBrand = recDataBuf[index];
            index += 1;
            m_ServoPar.servoModel = recDataBuf[index];
            index += 1;
            m_ServoPar.extendIOBoard = recDataBuf[index];
            index += 1;
            m_ServoPar.rotateAxisUnit = recDataBuf[index];
            index += 1;
            break;
        case CMD_SUN_MAC_SERVO://伺服参数
            num = recDataBuf[0] - 1;
            index = 1;
            m_ServoCommPar[num].axis = recDataBuf[index];
            index += 1;
            m_ServoCommPar[num].index = (uint16_t)recDataBuf[index] + ((uint16_t)recDataBuf[index+1]<<8);
            index += 2;
            m_ServoCommPar[num].sunIndex = recDataBuf[index];
            index += 1;
            m_ServoCommPar[num].parLen = recDataBuf[index];
            index += 1;
            m_ServoCommPar[num].parValue = (uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
            index += 4;
            m_ServoCommPar[num].saveFlag = recDataBuf[index];
            index += 1;
            m_ServoCommPar[num].powerOnSet = recDataBuf[index];
            index += 1;
            break;
        case CMD_SUN_MAC_ORIGIN://原点
            for(i=0; i<AXIS_TOTAL_NUM; i++)
            {
                m_AxisPar[i].backOriginOrder = recDataBuf[i];
            }
            index = AXIS_TOTAL_NUM;
            for(i=0; i<AXIS_TOTAL_NUM; i++)
            {
                m_AxisPar[i].originType = recDataBuf[i+index];
            }
            index += AXIS_TOTAL_NUM;
            m_ServoPar.resetSunProFlag = recDataBuf[index];
            index += 1;
            m_ServoPar.torqueLimitFlag = recDataBuf[index];
            index += 1;
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
            num = recDataBuf[0] - 1;
            index = 1;
            m_StackInfo[num].stackOrder = recDataBuf[index];
            index += 1;
            m_StackInfo[num].countMode = recDataBuf[index];
            index += 1;
            m_StackInfo[num].stackFinStopFlag = recDataBuf[index];
            index += 1;
            for(i=0; i<STACK_AXIS_NUM; i++)
            {
                m_StackInfo[num].axisSelect[i] = recDataBuf[index+i];
            }
            index += STACK_AXIS_NUM;
            for(i=0; i<STACK_AXIS_NUM; i++)
            {
                m_StackInfo[num].axisSpeed[i] = recDataBuf[index+i];
            }
            index += STACK_AXIS_NUM;
            for(i=0; i<STACK_AXIS_NUM; i++)
            {
                m_StackInfo[num].stackPointNum[i] = recDataBuf[index+i];
            }
            index += STACK_AXIS_NUM;
            for(i=0; i<STACK_AXIS_NUM; i++)
            {
                m_StackInfo[num].stackDir[i] = recDataBuf[index+i];
            }
            index += STACK_AXIS_NUM;
            for(i=0; i<STACK_AXIS_NUM; i++)
            {
                m_StackInfo[num].dischangeSpeed[i] = recDataBuf[index+i];
            }
            index += STACK_AXIS_NUM;
            break;
        case CMD_SUN_STACK_POINT://堆叠点位参数
            num = recDataBuf[0] - 1;
            index = 1;
            axis = recDataBuf[index] - 1;
            index += 1;
            m_StackInfo[num].stackStartPos[axis] = 0;
            m_StackInfo[num].stackStartPos[axis] |= (uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
            index += 4;
            m_StackInfo[num].stack_X_EndPos[axis] = 0;
            m_StackInfo[num].stack_X_EndPos[axis] |= (uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
            index += 4;
            m_StackInfo[num].stack_Y_EndPos[axis] = 0;
            m_StackInfo[num].stack_Y_EndPos[axis] |= (uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
            index += 4;
            m_StackInfo[num].stackDiagonalPos[axis] = 0;
            m_StackInfo[num].stackDiagonalPos[axis] |= (uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
            index += 4;
            m_StackInfo[num].dischangeStartPos[axis] = 0;
            m_StackInfo[num].dischangeStartPos[axis] |= (uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
            index += 4;
            m_StackInfo[num].intevalDis[axis] = 0;
            m_StackInfo[num].intevalDis[axis] |= (uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
            index += 4;
            m_StackInfo[num].offsetDis[axis] = 0;
            m_StackInfo[num].offsetDis[axis] |= (uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
            index += 4;
            break;
        case CMD_SUN_STACK_SET://堆叠设置
            m_StackFunc.stackType = recDataBuf[0];
            index = 1;
            m_StackFunc.groupStack = recDataBuf[index];
            index += 1;
            m_StackFunc.rowStack = recDataBuf[index];
            index += 1;
            m_StackFunc.storeyStack = recDataBuf[index];
            index += 1;
            m_StackFunc.divideStack = recDataBuf[index];
            index += 1;
            m_StackFunc.stackMemory = recDataBuf[index];
            index += 1;
            m_StackFunc.siloType = recDataBuf[index];
            index += 1;
            m_StackFunc.rotateSiloFinHint = recDataBuf[index];
            index += 1;
            m_StackFunc.rotateSiloPlaceNum = recDataBuf[index];
            index += 1;
            for(i=0; i<STACK_TOTAL_NUM; i++)
            {
                m_StackInfo[i].groupLeaveBlank = recDataBuf[index+i];
            }
            index += STACK_TOTAL_NUM;
            for(i=0; i<STACK_TOTAL_NUM; i++)
            {
                m_StackInfo[i].stackMoveMode = recDataBuf[index+i];
            }
            index += STACK_TOTAL_NUM;
            break;
        default:
            break;
        }
    }

    //刷新界面、存储配置文件（后面添加，发射相关信号）
}


//解析——状态处理函数
void Usart::ExtendReadStaDeal(uint8_t mainCmd, uint8_t sunCmd, uint8_t *recDataBuf)
{
    uint16_t i = 0;
    uint8_t ret = 0;
    uint8_t errCode = 0;
    uint16_t index = 0;

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
                        m_InPortSta[i*8+j]=recDataBuf[i]|0x01;
                        recDataBuf[i]=recDataBuf[i]>>1;
                    }
                }
                else
                {
                    for(int j=0;j<8;j++)
                    {
                        m_InPortSta[i*8+j]=recDataBuf[i]|0x01;
                        recDataBuf[i]=recDataBuf[i]>>1;
                    }
                }

            }
            for(i=8;i<13;i++)
            {
                for(int j=0;j<8;j++)
                {
                    m_OutPortSta[i*8+j]=recDataBuf[i]|0x01;
                    recDataBuf[i]=recDataBuf[i]>>1;
                }
            }


            break;
        case CMD_SUN_STA_AXIS_POS://轴当前位置读取
            index=0;
            for(i=0;i<AXIS_TOTAL_NUM;i++)
            {
                index=i*4;
                m_AxisCurPos[i]=(uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
            }
            index=24;
            m_AxisCurSpeed=(uint16_t)recDataBuf[index] + ((uint16_t)recDataBuf[index+1]<<8);
            index=26;
            m_AxisCurTorque=(uint16_t)recDataBuf[index] + ((uint16_t)recDataBuf[index+1]<<8);

            break;
        case CMD_SUN_STA_MAC://控制器状态读取
            index = 3;
            m_ProRunInfo.proNum[0] = (uint16_t)recDataBuf[index] + ((uint16_t)recDataBuf[index+1]<<8);
            index = 5;
            for(i=1; i<PRO_NUM; i++)
            {
                m_ProRunInfo.proNum[i] = recDataBuf[index + i - 1];
            }
            index += PRO_NUM-1;
            break;
        case CMD_SUN_STA_INFO://运行信息读取
            index = 0;
            m_RunInfo.runTime = (uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
            index = 10;
            m_RunInfo.actualProductNum = (uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
            index += 4;
            break;
        case CMD_SUN_STA_PAR://运行参数读写
            index = 0;
            m_RunPar.planProductNum = (uint32_t)recDataBuf[index] + ((uint32_t)recDataBuf[index+1]<<8) + ((uint32_t)recDataBuf[index+2]<<16) + ((uint32_t)recDataBuf[index+3]<<24);
            index += 4;
            m_RunPar.globalSpeed = recDataBuf[index];
            index += 1;
            m_RunPar.breakPointFlag = recDataBuf[index];
            index += 1;
            m_RunPar.breakPointProNum = recDataBuf[index];
            index += 1;
            m_RunPar.breakPointList = (uint16_t)recDataBuf[index] + ((uint16_t)recDataBuf[index+1]<<8);
            index += 2;
            break;
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
            GetProData1(parNum, parNum2, sendDataBuf, len,PRO_FLAG_CMDLINE_03);
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
            sendDataBuf[index++] = (uint8_t)parNum;
            sendDataBuf[index++] = (uint8_t)parNum2;
            sendDataBuf[index++] = (uint8_t)(parNum2>>8);
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

    switch (m_OperateProOrder->cmd)
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
        for(int m=0;m<3;m++)
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
        sendDataBuf[index++] = ((P_SearchAxisMoveStruct*)m_OperateProOrder[parNum2].pData)->reachPosAlarmFlag;
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
    sendDataBuf[index++]=(uint8_t)m_ProOrder[parNum]->list;
    sendDataBuf[index++] = (uint8_t)(m_ProOrder[parNum]->list>>8);
    sendDataBuf[index++]=(uint8_t)m_ProOrder[parNum]->runOrderNum;
    sendDataBuf[index++] = (uint8_t)(m_ProOrder[parNum]->runOrderNum>>8);
    sendDataBuf[index++]=(uint8_t)m_ProOrder[parNum]->cmd;
    sendDataBuf[index++]=(uint8_t)m_ProOrder[parNum]->noteFlag;
    sendDataBuf[index++]=(uint8_t)m_ProOrder[parNum]->delay;
    sendDataBuf[index++] = (uint8_t)(m_ProOrder[parNum]->delay>>8);

    switch (m_ProOrder[parNum]->cmd)
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
        for(int m=0;m<3;m++)
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
        sendDataBuf[index++] = ((P_SearchAxisMoveStruct*)m_ProOrder[parNum][parNum2].pData)->reachPosAlarmFlag;
        for(int m=0;m<3;m++)
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
        sendDataBuf[index++] = (uint8_t)((P_OffsetAxisStruct*)m_ProOrder[parNum][parNum2].pData)->offsetPos;
        sendDataBuf[index++] = (uint8_t)(((P_OffsetAxisStruct*)m_ProOrder[parNum][parNum2].pData)->offsetPos>>8);
        sendDataBuf[index++] = (uint8_t)(((P_OffsetAxisStruct*)m_ProOrder[parNum][parNum2].pData)->offsetPos>>16);
        sendDataBuf[index++] = (uint8_t)(((P_OffsetAxisStruct*)m_ProOrder[parNum][parNum2].pData)->offsetPos>>24);
        sendDataBuf[index++] = ((P_OffsetAxisStruct*)m_ProOrder[parNum][parNum2].pData)->axis;
        sendDataBuf[index++] = ((P_OffsetAxisStruct*)m_ProOrder[parNum][parNum2].pData)->speed;
        for(int m=0;m<2;m++)
        {
            sendDataBuf[index++] = ((P_OffsetAxisStruct*)m_ProOrder[parNum][parNum2].pData)->ret[m];
        }
        len = index;
        break;
    case C_TORQUE_GARD:
        sendDataBuf[index++] = ((P_TorqueGardStruct*)m_ProOrder[parNum][parNum2].pData)->axis;
        sendDataBuf[index++] = ((P_TorqueGardStruct*)m_ProOrder[parNum][parNum2].pData)->torqueValue;
        for(int m=0;m<2;m++)
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



