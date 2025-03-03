#include "port_setting.h"

struct InterLockGroup {
    int forwardValuePort;       // 正向操作端口（如阀门开启）
    int reverseValuePort;       // 反向操作端口（如阀门关闭）
    int forwardDetectPort;      // 正向检测端口
    int reverseDetectPort;      // 反向检测端口
};

// 互锁设置每一组对应的端口
constexpr InterLockGroup interLockGroups[OUT_INTERLOCK_NUM] = {
    {CLAW_METERIAL_1_CLAMP, CLAW_METERIAL_1_LOOSENED, 0, 1},        // 原料1
    {CLAW_PRODUCT_1_CLAMP, CLAW_PRODUCT_1_LOOSENED, 2, 3},          // 成品1
    {CLAW_CLAW_1_CLAMP, CLAW_CLAW_1_LOOSENED, 4, 5},                // 卡爪1
    {MACHINE_AUTO_DOOR_1_OPEN, MACHINE_AUTO_DOOR_1_CLOSE, 8, 9},    // 自动门1
    {MACHINE_CHUCK_1_CLAMP, MACHINE_CHUCK_1_LOOSENED, 10, 11},      // 卡盘1
    {-1, -1, -1, -1},                                               // 预留1
    {24, 25, 36, 37},                                               // 原料2
    {26, 27, 38, 39},                                               // 成品2
    {28, 29, 40, 41},                                               // 卡爪2
    {32, 33, 44, 45},                                               // 自动门2
    {34, 35, 46, 47},                                               // 卡盘2
    {-1, -1, -1, -1}                                                // 预留2
};

struct AxisParaInputPortGroup {
    int axisLimitPostive;       // 正限位
    int axisLimitNegtive;       // 负限位
    int axisOriginType;         // 原点类型
};

// 各个轴 正限位 负限位 原点类型 对应的数组索引
constexpr AxisParaInputPortGroup axisParaPortMaping[AXIS_TOTAL_NUM] = {
    {13, 19, 24},
    {14, 20, 25},
    {15, 21, 26},
    {16, 22, 27},
    {17, 23, 28},
    {18, 24, 29}
};


//若没有对应端口号的，则索引定为99
uint8_t outputTypeIndex[OUT_PORT_TYPE_NUM]={0};                    //输出类型对应的下标集合
uint8_t outportInterlockIndex[OUT_INTERLOCK_NUM][4]={0};                //互锁设置
uint8_t outputRelevancyIndex[OUT_PORT_RELEVANCY_NUM][2]={0};                //预留关联
uint8_t outputReleteOutIndex[OUT_PORT_RELEOUT_NUM]={0};                    //预留出类型
uint8_t seniorFuncIndex[SENIOR_NUM]={0};                    //预留出类型

/*************************************************************************
**  函数名：  readSigSetPara()
**	输入参数：无
**	输出参数：无
**	函数功能：读取端口自定义的所有信息
**  作者：    PXC
**  开发日期：2024/11/1
**************************************************************************/
void readSigSetPara()
{
    getOutPortType();
    getOutportInterlock();
    getOutportRelevancy();
    getReleteOut();
    getKeyFunc();
    getLedFunc();
    getSeniorFunc();

    // 端口自定义
    ::readPortDefInfo();
    //名称自定义
    ::readNameDefine();


    for(int i = 0;i < INPUT_TOTAL_NUM;i++)
    {
        m_Port_X[i].isReserve = !(i < INPUT_NUM);
    }

    //Y
    for(int i=0;i<OUTPUT_TOTAL_NUM;i++)
    {
        m_Port_X[i].isReserve = !(i < OUTPUT_NUM);
    }


    uint8_t tempIndex=0;
    //输出类型
    auto tempList11=getIniValues(1,"OutPortType");
    for(int i=0;i<OUT_PORT_TYPE_NUM;i++)
    {
        tempIndex=tempList11[i].toUInt();
        if(tempIndex==0)
        {
            outputTypeIndex[i]=99;
            m_OutPortType[i][0]=0;
        }
        else
        {
            outputTypeIndex[i]=tempIndex-1;
            m_OutPortType[i][0]=tempIndex/*m_Port_Y[tempIndex-1].portNum*/;
        }

    }
    //互锁设置
    tempList11=getIniValues(1,"OutportInterlock");
    for(int i=0;i<OUT_INTERLOCK_NUM;i++)
    {
        for(int j=0;j<4;j++)
        {
            tempIndex=tempList11[4*i+j].toUInt();
            if(tempIndex==0)
            {
                outportInterlockIndex[i][j]=99;
                m_OutportInterlock[i][j]=0;
            }
            else
            {
                outportInterlockIndex[i][j]=tempIndex-1;
                switch (j)
                {
                case 0:
                    m_OutportInterlock[i][j]=tempIndex/*m_Port_Y[tempIndex-1].portNum*/;
                    break;
                case 1:
                    m_OutportInterlock[i][j]=tempIndex/*m_Port_X[tempIndex-1].portNum*/;
                    break;
                case 2:
                    m_OutportInterlock[i][j]=tempIndex/*m_Port_Y[tempIndex-1].portNum*/;
                    break;
                case 3:
                    m_OutportInterlock[i][j]=tempIndex/*m_Port_X[tempIndex-1].portNum*/;
                    break;
                }
            }
        }
    }
    //预留关联
    tempList11=getIniValues(1,"OutputRelevancy");
    for(int i=0;i<OUT_PORT_RELEVANCY_NUM;i++)
    {
        for(int j=0;j<2;j++)
        {
            tempIndex=tempList11[2*i+j].toUInt();
            if(tempIndex==0)
            {
                outputRelevancyIndex[i][j]=99;
                m_OutportRelevancy[i][j]=0;
            }
            else
            {
                outputRelevancyIndex[i][j]=tempIndex-1;
                m_OutportRelevancy[i][j]=tempIndex/*m_Port_Y[tempIndex-1].portNum*/;
            }
        }
    }
    //预留出类型
    tempList11=getIniValues(1,"OutputReleteOut");
    for(int i=0;i<OUT_PORT_RELEOUT_NUM;i++)
    {
        tempIndex=tempList11[i].toUInt();
        if(tempIndex==0)
        {
            outputReleteOutIndex[i]=99;
            m_OutportReleteOut[i][0]=0;
        }
        else
        {
            outputReleteOutIndex[i]=tempIndex-1;
            m_OutportReleteOut[i][0]=tempIndex/*m_Port_Y[tempIndex-1].portNum*/;
        }
    }

    //高级功能对应的端口号的定义
    tempList11=getIniValues(1,"SeniorFunc");

    uint8_t index=0;
    m_SeniorFuncPort.knifeOrigin1CheckPort=tempList11[index++].toUInt();
    m_SeniorFuncPort.knifeOrigin2CheckPort=tempList11[index++].toUInt();
    m_SeniorFuncPort.chuckOriginCheckPort=tempList11[index++].toUInt();
    m_SeniorFuncPort.stackSaveIn1CheckPort=tempList11[index++].toUInt();
    m_SeniorFuncPort.stackSaveIn2CheckPort=tempList11[index++].toUInt();
    m_SeniorFuncPort.stackSaveOutCheckPort=tempList11[index++].toUInt();
    m_SeniorFuncPort.alarmIn1CheckPort=tempList11[index++].toUInt();
    m_SeniorFuncPort.alarmIn2CheckPort=tempList11[index++].toUInt();

    m_SeniorFuncPort.emergencyStopCheckPort=tempList11[index++].toUInt();
    m_SeniorFuncPort.pauseStopCheckPort=tempList11[index++].toUInt();
    m_SeniorFuncPort.pressureCheckPort=tempList11[index++].toUInt();
    m_SeniorFuncPort.remoteAutoPort=tempList11[index++].toUInt();
    m_SeniorFuncPort.remoteStopPort=tempList11[index++].toUInt();
    m_SeniorFuncPort.bottomOilLimitPort=tempList11[index++].toUInt();
    m_SeniorFuncPort.oilerSwtPort=tempList11[index++].toUInt();
    m_SeniorFuncPort.lubPumpPort=tempList11[index++].toUInt();

    m_SeniorFuncPort.processSave1Port=tempList11[index++].toUInt();
    m_SeniorFuncPort.processSave2Port=tempList11[index++].toUInt();
    m_SeniorFuncPort.emergencySnapMotorEnablePort=tempList11[index++].toUInt();
    m_SeniorFuncPort.emergencyStopOutPort=tempList11[index++].toUInt();
    m_SeniorFuncPort.autoLightPort=tempList11[index++].toUInt();
    m_SeniorFuncPort.alarmLightPort=tempList11[index++].toUInt();
    m_SeniorFuncPort.alarmBuzzerPort=tempList11[index++].toUInt();
    m_SeniorFuncPort.pauseLightPort=tempList11[index++].toUInt();

    m_SeniorFuncPort.processFinish1Port=tempList11[index++].toUInt();
    m_SeniorFuncPort.processFinish2Port=tempList11[index++].toUInt();
    m_SeniorFuncPort.locateFinish1Port=tempList11[index++].toUInt();
    m_SeniorFuncPort.locateFinish2Port=tempList11[index++].toUInt();
    m_SeniorFuncPort.startProduct1Port=tempList11[index++].toUInt();
    m_SeniorFuncPort.startProduct2Port=tempList11[index++].toUInt();
    m_SeniorFuncPort.res1[0]=tempList11[index++].toUInt();
    m_SeniorFuncPort.res1[1]=tempList11[index++].toUInt();

    m_SeniorFuncPort.mainAxisRotate1Port=tempList11[index++].toUInt();
    m_SeniorFuncPort.mainAxisRotate2Port=tempList11[index++].toUInt();
    m_SeniorFuncPort.mainAxisLocate1Port=tempList11[index++].toUInt();
    m_SeniorFuncPort.mainAxisLocate2Port=tempList11[index++].toUInt();
    m_SeniorFuncPort.biowAir1Port=tempList11[index++].toUInt();
    m_SeniorFuncPort.biowAir2Port=tempList11[index++].toUInt();
    m_SeniorFuncPort.res2[0]=tempList11[index++].toUInt();
    m_SeniorFuncPort.res2[1]=tempList11[index++].toUInt();

    m_SeniorFuncPort.manualChuckIn1Port=tempList11[index++].toUInt();
    m_SeniorFuncPort.manualChuckIn2Port=tempList11[index++].toUInt();
    m_SeniorFuncPort.autoDoorCot1Port=tempList11[index++].toUInt();
    m_SeniorFuncPort.autoDoorCot2Port=tempList11[index++].toUInt();
    m_SeniorFuncPort.res3[0]=tempList11[index++].toUInt();
    m_SeniorFuncPort.res3[1]=tempList11[index++].toUInt();
    m_SeniorFuncPort.res3[2]=tempList11[index++].toUInt();
    m_SeniorFuncPort.res3[3]=tempList11[index++].toUInt();
}

/*************************************************************************
**  函数名：  readIniPara()
**	输入参数：无
**	输出参数：无
**	函数功能：读取整个工程中所有配置文件的信息，将参数读至所有的结构体
**  作者：    PXC
**  开发日期：2024/11/1
**************************************************************************/
void readIniPara()
{
    getMachineSave();
    getStackSave();
    getClawSave();
    getOnlineSafe();
    getProductSet();
    getProductSenior();
    getInternet();
    getAxisPar();
    getHandwheelPar();
    getServoPar();
    getDegreeParS();
    getSaveArea();
    getServoCommPar();
    getStackInfo();
    getStackFunc();
    getManualAxis();
    getProgramNameAndPath();//读取所有程序文件信息
    getSystemSet();
    ::readPasswdFromConfig();

    // 读取完所有参数后根据参数更新参数对应 X 端口 或者 Y 端口的标志位
    updateAdvancedFuncPortFlag();
    updateInterLockPortFlag();
    updateAxidParameterPortFlag();
}

void updateInterLockPortFlag()
{
    for (int i = 0; i < OUT_INTERLOCK_NUM; i++) {
        bool useForwardValue = m_OutportInterlock[i][0] != 0;
        bool useReverseValue = m_OutportInterlock[i][2] != 0;
        bool forwardCheck = m_OutportInterlock[i][1] != 0;
        bool reverseCheck = m_OutportInterlock[i][3] != 0;

        const auto &group = interLockGroups[i];

        // 如果是预留的组，跳过
        if (group.forwardValuePort == -1) continue;

        m_Port_Y[group.forwardValuePort].functionSet = useForwardValue;
        m_Port_Y[group.reverseValuePort].functionSet = useForwardValue && useReverseValue;

        m_Port_X[group.forwardDetectPort].functionSet = useForwardValue && forwardCheck;
        m_Port_X[group.reverseDetectPort].functionSet = useForwardValue && reverseCheck;
    }
}

void updateAdvancedFuncPortFlag()
{
    m_Port_X[6].functionSet = static_cast<bool>(m_SeniorFunc.stackSaveIn1Check);
    m_Port_X[7].functionSet = static_cast<bool>(m_SeniorFunc.emergencyStopCheck);
    m_Port_X[12].functionSet = static_cast<bool>(m_SeniorFunc.pressureCheck);
//    m_Port_X[14].functionSet = static_cast<bool>(m_SeniorFunc.remoteStop);//这几个IO与轴限位功能相冲突，暂时不明确需求
//    m_Port_X[15].functionSet = static_cast<bool>(m_SeniorFunc.remoteAuto);
//    m_Port_X[17].functionSet = static_cast<bool>(m_SeniorFunc.manualChuckIn1);
    m_Port_X[31].functionSet = static_cast<bool>(m_SeniorFunc.processFinish1);
    m_Port_X[32].functionSet = static_cast<bool>(m_SeniorFunc.locateFinish1);
    m_Port_X[33].functionSet = static_cast<bool>(m_SeniorFunc.knifeOrigin1Check);
    m_Port_X[34].functionSet = static_cast<bool>(m_SeniorFunc.alarmIn1Check);
    m_Port_X[35].functionSet = static_cast<bool>(m_SeniorFunc.pauseStopCheck);

//    m_Port_X[36].functionSet
//    m_Port_X[37].functionSet
//    m_Port_X[38].functionSet
//    m_Port_X[39].functionSet
//    m_Port_X[40].functionSet
//    m_Port_X[41].functionSet

    m_Port_X[42].functionSet = static_cast<bool>(m_SeniorFunc.stackSaveIn2Check);
    m_Port_X[43].functionSet = static_cast<bool>(m_SeniorFunc.manualChuckIn2);

    //    m_Port_X[44].functionSet
    //    m_Port_X[45].functionSet
    //    m_Port_X[46].functionSet
    //    m_Port_X[47].functionSet
    //    m_Port_X[48].functionSet
    //    m_Port_X[49].functionSet
    //    m_Port_X[50].functionSet
    //    m_Port_X[51].functionSet

    m_Port_X[52].functionSet = static_cast<bool>(m_SeniorFunc.processFinish2);
    m_Port_X[53].functionSet = static_cast<bool>(m_SeniorFunc.locateFinish2);
    m_Port_X[54].functionSet = static_cast<bool>(m_SeniorFunc.knifeOrigin2Check);
    m_Port_X[55].functionSet = static_cast<bool>(m_SeniorFunc.alarmIn2Check);

    //    m_Port_X[56].functionSet
    //    m_Port_X[57].functionSet
    //    m_Port_X[58].functionSet
    //    m_Port_X[59].functionSet

    //输出端口
    // m_Port_Y[0]  -> Y1   原料1夹紧
    // m_Port_Y[1]  -> Y2   原料1夹紧
    // m_Port_Y[2]  -> Y3   成品1夹紧
    // m_Port_Y[3]  -> Y4   成品1夹紧
    // m_Port_Y[4]  -> Y5   卡爪1夹紧
    // m_Port_Y[5]  -> Y6   卡爪1夹紧
    // m_Port_Y[8]  -> Y9   自动门1开 | 自动门控制1 冲突
    // m_Port_Y[9]  -> Y10  自动门1开 冲突
    // m_Port_Y[32] -> EY9  自动门2开 冲突
    // m_Port_Y[33] -> EY10 自动门2开 冲突
    m_Port_Y[6].functionSet = static_cast<bool>(m_SeniorFunc.autoLight);//根据某个端口功能是否使用，置端口预留标志位
    m_Port_Y[7].functionSet = static_cast<bool>(m_SeniorFunc.alarmLight);
    m_Port_Y[8].functionSet = static_cast<bool>(m_SeniorFunc.autoDoorCot1);
    m_Port_Y[9].functionSet = static_cast<bool>(m_SeniorFunc.autoDoorCot1);
    m_Port_Y[10].functionSet = static_cast<bool>(m_SeniorFunc.biowAir1);
    m_Port_Y[12].functionSet = static_cast<bool>(m_SeniorFunc.emergencyStopOut);
    m_Port_Y[13].functionSet = static_cast<bool>(m_SeniorFunc.lubPump);
    m_Port_Y[14].functionSet = static_cast<bool>(m_SeniorFunc.alarmBuzzer);
    m_Port_Y[15].functionSet = static_cast<bool>(m_SeniorFunc.pauseLight);
    m_Port_Y[16].functionSet = static_cast<bool>(m_SeniorFunc.startProduct1);
    m_Port_Y[17].functionSet = static_cast<bool>(m_SeniorFunc.mainAxisLocate1);
    m_Port_Y[18].functionSet = static_cast<bool>(m_SeniorFunc.processSave1);
    m_Port_Y[20].functionSet = static_cast<bool>(m_SeniorFunc.mainAxisRotate1);

//    m_Port_Y[21].functionSet = m_SeniorFunc.;
//    m_Port_Y[22].functionSet = m_SeniorFunc.;
//    m_Port_Y[23].functionSet = m_SeniorFunc.;


    m_Port_Y[OUTPUT_NUM +  7].functionSet = (m_SeniorFunc.biowAir2) ? 1 : 0;
    m_Port_Y[OUTPUT_NUM +  8].functionSet = (m_SeniorFunc.autoDoorCot2) ? 1 : 0;
    m_Port_Y[OUTPUT_NUM +  9].functionSet = (m_SeniorFunc.autoDoorCot2) ? 1 : 0;

//    m_Port_Y[OUTPUT_NUM + 10].functionSet = ;
//    m_Port_Y[OUTPUT_NUM + 11].functionSet = ;
//    m_Port_Y[OUTPUT_NUM + 12].functionSet = ;
//    m_Port_Y[OUTPUT_NUM + 13].functionSet = ;
//    m_Port_Y[OUTPUT_NUM + 14].functionSet = ;
//    m_Port_Y[OUTPUT_NUM + 15].functionSet = ;

    m_Port_Y[OUTPUT_NUM + 16].functionSet = (m_SeniorFunc.startProduct2) ? 1 : 0;
    m_Port_Y[OUTPUT_NUM + 17].functionSet = (m_SeniorFunc.mainAxisLocate2) ? 1 : 0;
    m_Port_Y[OUTPUT_NUM + 18].functionSet = (m_SeniorFunc.processSave2) ? 1 : 0;

//    m_Port_Y[OUTPUT_NUM + 19].functionSet = ;
}

void updateAxidParameterPortFlag()
{

    for (int i = 0; i < AXIS_TOTAL_NUM; i++)
    {
        int index1 = axisParaPortMaping[i].axisLimitPostive;
        int index2 = axisParaPortMaping[i].axisLimitNegtive;
        int index3 = axisParaPortMaping[i].axisOriginType;

        m_Port_X[index1].functionSet = m_AxisPar[i].limitPosSwt ? 1 : 0;
        m_Port_X[index2].functionSet = m_AxisPar[i].limitNegSwt ? 1 : 0;
        m_Port_X[index3].functionSet = m_AxisPar[i].originType ? 1 : 0;
    }
}
