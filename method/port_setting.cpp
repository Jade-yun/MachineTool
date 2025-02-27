#include "port_setting.h"

struct InterLockGroup {
    int forwardValuePort;       // 正向操作端口号（如阀门开启）
    int reverseValuePort;       // 反向操作端口号（如阀门关闭）
    int forwardDetectPort;      // 正向检测端口号
    int reverseDetectPort;      // 反向检测端口号
};

// 互锁设置每一组对应的端口号
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

//若没有对应端口号的，则索引定为99
//uint8_t outportInterlockIndex[OUT_INTERLOCK_NUM][4]={0};                //互锁设置,存储的实际端口号

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

//        // 如果存的是0，则表示名称没有修改，为默认名称
//        if (m_Port_X[i].modifyName == "0")
//        {
//            m_Port_X[i].modifyName = m_Port_X[i].defineName;
//        }

//        if (m_Port_X[i].modifyPort == "0")
//        {
//            m_Port_X[i].actualPortNum = m_Port_X[i].portNum;
//        }

//        if (m_Port_X[i].ResModifyName == "0")
//        {
//            m_Port_X[i].ResModifyName = m_Port_X[i].ResDefineName;
//        }
    }

    //Y
    for(int i=0;i<OUTPUT_TOTAL_NUM;i++)
    {

        m_Port_X[i].isReserve = !(i < OUTPUT_NUM);

//        // 如果存的是0，则表示名称没有修改，为默认名称
//        if (m_Port_Y[i].modifyName == "0")
//        {
//            m_Port_Y[i].modifyName = m_Port_Y[i].defineName;
//        }

//        if (m_Port_Y[i].modifyPort == "0")
//        {
//            m_Port_Y[i].actualPortNum = m_Port_Y[i].portNum;
//        }

//        if (m_Port_Y[i].ResModifyName == "0")
//        {
//            m_Port_Y[i].ResModifyName = m_Port_Y[i].ResDefineName;
//        }
    }

    updateAdvancedFuncPortFlag();
    updateInterLockPortFlag();
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
