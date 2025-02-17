#include "iniconfig.h"
#include <QCoreApplication>
#include <QTextCodec>

/*****************命令相关参数保存*****************/
QString m_configCmdPath = "/root/Cmd_Description.txt";
QString m_configPortSettingPath = "/root/Port_Setting.txt";
QString m_configOrderjinitPath = "/root/Pro_Teach.txt";

QString m_configPortXYNamePath="/root/Port_XY_Name_CHS.txt";                //输入输出、名称定义（3）

QString m_configPortXYNameIniPath="/root/Ini_Port_XY_Name.txt";             //输入输出、名称定义配置文件（99）

QString m_defaultconfigPortXYNameIniPath="/root/Ini_Port_XY_Name_default.txt";             //输入输出、名称定义出场配置文件（99）
QString m_defaultconfigPortSettingPath = "/root/Port_Setting_default.txt";

QSettings ConfigPortDefine(m_configPortXYNameIniPath,QSettings::IniFormat);

//配置文件相对路径
QString m_configFileNamePath="/root/Ini_Para.txt";
QString m_defaultConfigFileNamePath="/root/Ini_Para_default.txt";//默认设置参数配置文件，恢复出场设置时调用

const QString SysSetConfigPath = "/Settings/systemset.ini";
const QString PasswdConfigPath = "/Settings/passwd.ini";
const QString KeyAndSignalDescriptionPath = "/Settings/key_signal_str.ini";
const QString IOPortDescriptionPath = "/Settings/IOport_describestr.ini";

const QString CustomizeNameDefPath = "/Settings/NameDef_Customize_CN.ini";
const QString GuideInfoPath = "/Settings/guide_info.ini";
//开机自动加载程序信息
const QString PowerOnReadOneProPath = "/Settings/PowerOnReadOneProInfo.ini";

QSettings Ini_Parasettings(m_configFileNamePath,QSettings::IniFormat);
/*************************************************************************
**	输入参数：prefix:节点 key:键 defaultV:默认值
**	输出参数：值
**	函数功能：获取配置文件中键对应的值
**  作者：    PXC
**  开发日期：2024/7/22
**************************************************************************/
QString getValue(QString prefix, QString key, QString defaultV)
{
    QString reValue = "";
    Ini_Parasettings.beginGroup(prefix);
    reValue=Ini_Parasettings.value(key,defaultV).toString();
    Ini_Parasettings.endGroup();
    return reValue;
}
/*************************************************************************
**	输入参数：prefix:节点 key:键 defaultV:需要设置的值
**	输出参数：无
**	函数功能：设置、更新配置文件
**  作者：    PXC
**  开发日期：2024/7/22
**************************************************************************/
void setValue(QString prefix, QString key, QString value)
{
    Ini_Parasettings.beginGroup(prefix);
    Ini_Parasettings.setValue(key,value);
    Ini_Parasettings.endGroup();
}

uint8_t getValue(QString prefix, QString key, uint8_t defaultV)
{
    uint8_t reValue = 0;
    Ini_Parasettings.beginGroup(prefix);
    reValue=Ini_Parasettings.value(key,defaultV).value<uint8_t>();
    Ini_Parasettings.endGroup();
    return reValue;
}

void setValue(QString prefix, QString key, uint8_t value)
{
    Ini_Parasettings.beginGroup(prefix);
    Ini_Parasettings.setValue(key,value);
    Ini_Parasettings.endGroup();
}

uint16_t getValue(QString prefix, QString key, uint16_t defaultV)
{
    uint16_t reValue = 0;
    Ini_Parasettings.beginGroup(prefix);
    reValue=Ini_Parasettings.value(key,defaultV).value<uint16_t>();
    Ini_Parasettings.endGroup();
    return reValue;
}

void setValue(QString prefix, QString key, uint16_t value)
{
    Ini_Parasettings.beginGroup(prefix);
    Ini_Parasettings.setValue(key,value);
    Ini_Parasettings.endGroup();
}

uint32_t getValue(QString prefix, QString key, uint32_t defaultV)
{
    uint32_t reValue = 0;
    Ini_Parasettings.beginGroup(prefix);
    reValue=Ini_Parasettings.value(key,defaultV).value<uint32_t>();
    Ini_Parasettings.endGroup();
    return reValue;
}

void setValue(QString prefix, QString key, uint32_t value)
{
    Ini_Parasettings.beginGroup(prefix);
    Ini_Parasettings.setValue(key,value);
    Ini_Parasettings.endGroup();
}

int32_t getValue(QString prefix, QString key, int32_t defaultV)
{
    int32_t reValue = 0;
    Ini_Parasettings.beginGroup(prefix);
    reValue=Ini_Parasettings.value(key,defaultV).value<int32_t>();
    Ini_Parasettings.endGroup();
    return reValue;
}

void setValue(QString prefix, QString key, int32_t value)
{
    Ini_Parasettings.beginGroup(prefix);
    Ini_Parasettings.setValue(key,value);
    Ini_Parasettings.endGroup();
}
/*************************************************************************
**	输入参数：defaultV:默认值
**	输出参数：所有值并存入相应数组
**	函数功能：获取【输出类型】配置文件中键对应的值
**  作者：    PXC
**  开发日期：2024/7/22
**************************************************************************/
void getOutPortType(uint8_t defaultV)
{
    for(int i=0;i<OUT_PORT_TYPE_NUM;i++)
    {
        m_OutPortType[i][1]=getValue("OutPortType",QString("V%1").arg(i),defaultV);
    }
}
/*************************************************************************
**	输入参数：value:需要设置的值
**	输出参数：无
**	函数功能：设置、更新【输出类型】配置文件
**  作者：    PXC
**  开发日期：2024/7/22
**************************************************************************/
void setOutPortType(uint8_t value[OUT_PORT_TYPE_NUM][2])
{
    for(int i=0;i<OUT_PORT_TYPE_NUM;i++)
    {
        setValue("OutPortType",QString("V%1").arg(i),value[i][1]);
    }
}
//互锁设置
void getOutportInterlock(uint8_t defaultV)
{
    for(int i=0;i<OUT_INTERLOCK_NUM;i++)
    {
        for(int j=0;j<4;j++)
        {
            if(getValue("OutportInterlock",QString("V%1_%2").arg(i).arg(j),defaultV) == 0)
            {
                m_OutportInterlock[i][j] = 0;
            }
            else
            {
                m_OutportInterlock[i][j] = outportInterlockIndex[i][j];
            }
        }
    }
}

void setOutportInterlock(uint8_t value[OUT_INTERLOCK_NUM][4])
{
    for(int i=0;i<OUT_INTERLOCK_NUM;i++)
    {
        for(int j=0;j<4;j++)
        {
            setValue("OutportInterlock",QString("V%1_%2").arg(i).arg(j),value[i][j]==0?0:1);
        }

    }
}
//预留关联
void getOutportRelevancy(uint8_t defaultV)
{
    for(int i=0;i<OUTPUT_TOTAL_NUM;i++)
    {
        m_OutportRelevancy[i][2]=getValue("OutportRelevancy",QString("V%1").arg(i),defaultV);
    }
}

void setOutportRelevancy(uint8_t value[OUTPUT_TOTAL_NUM][3])
{
    for(int i=0;i<OUTPUT_TOTAL_NUM;i++)
    {
        setValue("OutportRelevancy",QString("V%1").arg(i),value[i][2]);
    }
}
//预留出类型
void getReleteOut(uint8_t defaultV)
{
    for(int i=0;i<OUT_PORT_RELEOUT_NUM;i++)
    {
        m_OutportReleteOut[i][1]=getValue("ReleteOut",QString("V%1").arg(i),defaultV);
    }
}

void setReleteOut(uint8_t value[OUT_PORT_RELEOUT_NUM][2])
{
    for(int i=0;i<OUT_PORT_RELEOUT_NUM;i++)
    {
        setValue("ReleteOut",QString("V%1").arg(i),value[i][1]);
    }
}
//按键功能
void getKeyFunc(uint8_t defaultV)
{
    for(int i=0;i<OPR_KEY_NUM;i++)
    {
        m_KeyFunc[i].keyType=getValue("KeyFunc",QString("V%1_keyType").arg(i),defaultV);
        m_KeyFunc[i].funcNum=getValue("KeyFunc",QString("V%1_funcNum").arg(i),defaultV);
        m_KeyFunc[i].oprMode=getValue("KeyFunc",QString("V%1_oprMode").arg(i),defaultV);
    }
}
void setKeyFunc(D_KeyFuncStruct* value)
{
    for(int i=0;i<OPR_KEY_NUM;i++)
    {
        getValue("KeyFunc",QString("V%1_keyType").arg(i),value[i].keyType);
        getValue("KeyFunc",QString("V%1_funcNum").arg(i),value[i].funcNum);
        getValue("KeyFunc",QString("V%1_oprMode").arg(i),value[i].oprMode);
    }
}
//LED功能
void getLedFunc(uint8_t defaultV)
{
    for(int i=0;i<OPR_LED_NUM;i++)
    {
        m_LedFunc[i].ledType=getValue("LedFunc",QString("V%1_ledType").arg(i),defaultV);
        m_LedFunc[i].funcNum=getValue("LedFunc",QString("V%1_funcNum").arg(i),defaultV);
    }
}
void setLedFunc(D_LedFuncStruct* value)
{
    for(int i=0;i<OPR_LED_NUM;i++)
    {
        setValue("LedFunc",QString("V%1_ledType").arg(i),value[i].ledType);
        setValue("LedFunc",QString("V%1_funcNum").arg(i),value[i].funcNum);
    }
}
//信号设置高级功能
void getSeniorFunc(uint8_t defaultV)
{
    m_SeniorFunc.knifeOrigin1Check=getValue("SeniorFunc","knifeOrigin1Check",defaultV);
    m_SeniorFunc.knifeOrigin2Check=getValue("SeniorFunc","knifeOrigin2Check",defaultV);
    m_SeniorFunc.chuckOriginCheck=getValue("SeniorFunc","chuckOriginCheck",defaultV);
    m_SeniorFunc.stackSaveIn1Check=getValue("SeniorFunc","stackSaveIn1Check",defaultV);
    m_SeniorFunc.stackSaveIn2Check=getValue("SeniorFunc","stackSaveIn2Check",defaultV);
    m_SeniorFunc.stackSaveOutCheck=getValue("SeniorFunc","stackSaveOutCheck",defaultV);
    m_SeniorFunc.alarmIn1Check=getValue("SeniorFunc","alarmIn1Check",defaultV);
    m_SeniorFunc.alarmIn2Check=getValue("SeniorFunc","alarmIn2Check",defaultV);

    m_SeniorFunc.emergencyStopCheck=getValue("SeniorFunc","emergencyStopCheck",defaultV);
    m_SeniorFunc.pauseStopCheck=getValue("SeniorFunc","pauseStopCheck",defaultV);
    m_SeniorFunc.pressureCheck=getValue("SeniorFunc","pressureCheck",defaultV);
    m_SeniorFunc.remoteAuto=getValue("SeniorFunc","remoteAuto",defaultV);
    m_SeniorFunc.remoteStop=getValue("SeniorFunc","remoteStop",defaultV);
    m_SeniorFunc.bottomOilLimit=getValue("SeniorFunc","bottomOilLimit",defaultV);
    m_SeniorFunc.oilerSwt=getValue("SeniorFunc","oilerSwt",defaultV);
    m_SeniorFunc.lubPump=getValue("SeniorFunc","lubPump",defaultV);

    m_SeniorFunc.processSave1=getValue("SeniorFunc","processSave1",defaultV);
    m_SeniorFunc.processSave2=getValue("SeniorFunc","processSave2",defaultV);
    m_SeniorFunc.emergencySnapMotorEnable=getValue("SeniorFunc","emergencySnapMotorEnable",defaultV);
    m_SeniorFunc.emergencyStopOut=getValue("SeniorFunc","emergencyStopOut",defaultV);
    m_SeniorFunc.autoLight=getValue("SeniorFunc","autoLight",defaultV);
    m_SeniorFunc.alarmLight=getValue("SeniorFunc","alarmLight",defaultV);
    m_SeniorFunc.alarmBuzzer=getValue("SeniorFunc","alarmBuzzer",defaultV);
    m_SeniorFunc.pauseLight=getValue("SeniorFunc","pauseLight",defaultV);

    m_SeniorFunc.processFinish1=getValue("SeniorFunc","processFinish1",defaultV);
    m_SeniorFunc.processFinish2=getValue("SeniorFunc","processFinish2",defaultV);
    m_SeniorFunc.locateFinish1=getValue("SeniorFunc","locateFinish1",defaultV);
    m_SeniorFunc.locateFinish2=getValue("SeniorFunc","locateFinish2",defaultV);
    m_SeniorFunc.startProduct1=getValue("SeniorFunc","startProduct1",defaultV);
    m_SeniorFunc.startProduct2=getValue("SeniorFunc","startProduct2",defaultV);
    for(int i=0;i<2;i++)
    {
        m_SeniorFunc.res1[i]=getValue("SeniorFunc",QString("res1_%1").arg(i),defaultV);
    }

    m_SeniorFunc.mainAxisRotate1=getValue("SeniorFunc","mainAxisRotate1",defaultV);
    m_SeniorFunc.mainAxisRotate2=getValue("SeniorFunc","mainAxisRotate2",defaultV);
    m_SeniorFunc.mainAxisLocate1=getValue("SeniorFunc","mainAxisLocate1",defaultV);
    m_SeniorFunc.mainAxisLocate2=getValue("SeniorFunc","mainAxisLocate2",defaultV);
    m_SeniorFunc.biowAir1=getValue("SeniorFunc","biowAir1",defaultV);
    m_SeniorFunc.biowAir2=getValue("SeniorFunc","biowAir2",defaultV);
    for(int i=0;i<2;i++)
    {
        m_SeniorFunc.res2[i]=getValue("SeniorFunc",QString("res2_%1").arg(i),defaultV);
    }

    m_SeniorFunc.manualChuckIn1=getValue("SeniorFunc","manualChuckIn1",defaultV);
    m_SeniorFunc.manualChuckIn2=getValue("SeniorFunc","manualChuckIn2",defaultV);
    m_SeniorFunc.autoDoorCot1=getValue("SeniorFunc","autoDoorCot1",defaultV);
    m_SeniorFunc.autoDoorCot2=getValue("SeniorFunc","autoDoorCot2",defaultV);
    for(int i=0;i<2;i++)
    {
        m_SeniorFunc.res3[i]=getValue("SeniorFunc",QString("res3_%1").arg(i),defaultV);
    }
}
void setSeniorFunc(D_SeniorFuncStruct value)
{
    setValue("SeniorFunc","knifeOrigin1Check",value.knifeOrigin1Check);
    setValue("SeniorFunc","knifeOrigin2Check",value.knifeOrigin2Check);
    setValue("SeniorFunc","chuckOriginCheck",value.chuckOriginCheck);
    setValue("SeniorFunc","stackSaveIn1Check",value.stackSaveIn1Check);
    setValue("SeniorFunc","stackSaveIn2Check",value.stackSaveIn2Check);
    setValue("SeniorFunc","stackSaveOutCheck",value.stackSaveOutCheck);
    setValue("SeniorFunc","alarmIn1Check",value.alarmIn1Check);
    setValue("SeniorFunc","alarmIn2Check",value.alarmIn2Check);

    setValue("SeniorFunc","emergencyStopCheck",value.emergencyStopCheck);
    setValue("SeniorFunc","pauseStopCheck",value.pauseStopCheck);
    setValue("SeniorFunc","pressureCheck",value.pressureCheck);
    setValue("SeniorFunc","remoteAuto",value.remoteAuto);
    setValue("SeniorFunc","remoteStop",value.remoteStop);
    setValue("SeniorFunc","bottomOilLimit",value.bottomOilLimit);
    setValue("SeniorFunc","oilerSwt",value.oilerSwt);
    setValue("SeniorFunc","lubPump",value.lubPump);

    setValue("SeniorFunc","processSave1",value.processSave1);
    setValue("SeniorFunc","processSave2",value.processSave2);
    setValue("SeniorFunc","emergencySnapMotorEnable",value.emergencySnapMotorEnable);
    setValue("SeniorFunc","emergencyStopOut",value.emergencyStopOut);
    setValue("SeniorFunc","autoLight",value.autoLight);
    setValue("SeniorFunc","alarmLight",value.alarmLight);
    setValue("SeniorFunc","alarmBuzzer",value.alarmBuzzer);
    setValue("SeniorFunc","pauseLight",value.pauseLight);

    setValue("SeniorFunc","processFinish1",value.processFinish1);
    setValue("SeniorFunc","processFinish2",value.processFinish2);
    setValue("SeniorFunc","locateFinish1",value.locateFinish1);
    setValue("SeniorFunc","locateFinish2",value.locateFinish2);
    setValue("SeniorFunc","startProduct1",value.startProduct1);
    setValue("SeniorFunc","startProduct2",value.startProduct2);
    for(int i=0;i<2;i++)
    {
        setValue("SeniorFunc",QString("res1_%1").arg(i),value.res1[i]);
    }

    setValue("SeniorFunc","mainAxisRotate1",value.mainAxisRotate1);
    setValue("SeniorFunc","mainAxisRotate2",value.mainAxisRotate2);
    setValue("SeniorFunc","mainAxisLocate1",value.mainAxisLocate1);
    setValue("SeniorFunc","mainAxisLocate2",value.mainAxisLocate2);
    setValue("SeniorFunc","biowAir1",value.biowAir1);
    setValue("SeniorFunc","biowAir2",value.biowAir2);
    for(int i=0;i<2;i++)
    {
        setValue("SeniorFunc",QString("res2_%1").arg(i),value.res2[i]);
    }

    setValue("SeniorFunc","manualChuckIn1",value.manualChuckIn1);
    setValue("SeniorFunc","manualChuckIn2",value.manualChuckIn2);
    setValue("SeniorFunc","autoDoorCot1",value.autoDoorCot1);
    setValue("SeniorFunc","autoDoorCot2",value.autoDoorCot2);
    for(int i=0;i<2;i++)
    {
        setValue("SeniorFunc",QString("res3_%1").arg(i),value.res3[i]);
    }
}
//机床安全
void getMachineSave(uint8_t defaultV)
{
    m_MachineSave.armFallAutoDoor1Swt=getValue("MachineSave","armFallAutoDoor1Swt",defaultV);
    m_MachineSave.armFallAutoDoor1SwtSta=getValue("MachineSave","armFallAutoDoor1SwtSta",defaultV);
    m_MachineSave.armFallAutoDoor2Swt=getValue("MachineSave","armFallAutoDoor2Swt",defaultV);
    m_MachineSave.armFallAutoDoor2SwtSta=getValue("MachineSave","armFallAutoDoor2SwtSta",defaultV);
}
void setMachineSave(D_MachineSaveStruct value)
{
    setValue("MachineSave","armFallAutoDoor1Swt",value.armFallAutoDoor1Swt);
    setValue("MachineSave","armFallAutoDoor1SwtSta",value.armFallAutoDoor1SwtSta);
    setValue("MachineSave","armFallAutoDoor2Swt",value.armFallAutoDoor2Swt);
    setValue("MachineSave","armFallAutoDoor2SwtSta",value.armFallAutoDoor2SwtSta);
}

//料仓安全
void getStackSave(uint8_t defaultV)
{
    m_StackSave.stackSaveIn1Roatore=getValue("StackSave","stackSaveIn1Roatore",defaultV);
    m_StackSave.stackSaveIn2Roatore=getValue("StackSave","stackSaveIn2Roatore",defaultV);
}
void setStackSave(D_StackSaveStruct value)
{
    setValue("StackSave","stackSaveIn1Roatore",value.stackSaveIn1Roatore);
    setValue("StackSave","stackSaveIn2Roatore",value.stackSaveIn2Roatore);
}
//卡爪安全
void getClawSave(uint8_t defaultV)
{
    for(int i=0;i<CLAW_SAVE_NUM;i++)
    {
        m_ClawSave[i].clawKeepoutSta=getValue("ClawSave",QString("clawKeepoutSta_%1").arg(i),defaultV);
        m_ClawSave[i].axisSelect_X=getValue("ClawSave",QString("axisSelect_X_%1").arg(i),defaultV);
        m_ClawSave[i].axisSelect_Z=getValue("ClawSave",QString("axisSelect_Z_%1").arg(i),defaultV);
        m_ClawSave[i].clawKeepoutMinX=getValue("ClawSave",QString("clawKeepoutMinX_%1").arg(i),(int32_t)defaultV);
        m_ClawSave[i].clawKeepoutMaxX=getValue("ClawSave",QString("clawKeepoutMaxX_%1").arg(i),(int32_t)defaultV);
        m_ClawSave[i].clawKeepoutHighZ=getValue("ClawSave",QString("clawKeepoutHighZ_%1").arg(i),(int32_t)defaultV);
        m_ClawSave[i].cKeepoutMinX=getValue("ClawSave",QString("cKeepoutMinX_%1").arg(i),(int32_t)defaultV);
        m_ClawSave[i].cKeepoutMaxX=getValue("ClawSave",QString("cKeepoutMaxX_%1").arg(i),(int32_t)defaultV);
        m_ClawSave[i].cKeepoutHighZ=getValue("ClawSave",QString("cKeepoutHighZ_%1").arg(i),(int32_t)defaultV);
        m_ClawSave[i].cKeepoutPosMinC=getValue("ClawSave",QString("cKeepoutPosMinC_%1").arg(i),(int32_t)defaultV);
        m_ClawSave[i].cKeepoutPosMaxC=getValue("ClawSave",QString("cKeepoutPosMaxC_%1").arg(i),(int32_t)defaultV);
        m_ClawSave[i].cKeepoutNegMinC=getValue("ClawSave",QString("cKeepoutNegMinC_%1").arg(i),(int32_t)defaultV);
        m_ClawSave[i].cKeepoutNegMaxC=getValue("ClawSave",QString("cKeepoutNegMaxC_%1").arg(i),(int32_t)defaultV);
    }
}
void setClawSave(D_ClawSaveStruct value,uint8_t index)
{
    setValue("ClawSave",QString("clawKeepoutSta_%1").arg(index),value.clawKeepoutSta);
    setValue("ClawSave",QString("axisSelect_X_%1").arg(index),value.axisSelect_X);
    setValue("ClawSave",QString("axisSelect_Z_%1").arg(index),value.axisSelect_Z);
    setValue("ClawSave",QString("clawKeepoutMinX_%1").arg(index),value.clawKeepoutMinX);
    setValue("ClawSave",QString("clawKeepoutMaxX_%1").arg(index),value.clawKeepoutMaxX);
    setValue("ClawSave",QString("clawKeepoutHighZ_%1").arg(index),value.clawKeepoutHighZ);
    setValue("ClawSave",QString("cKeepoutMinX_%1").arg(index),value.cKeepoutMinX);
    setValue("ClawSave",QString("cKeepoutMaxX_%1").arg(index),value.cKeepoutMaxX);
    setValue("ClawSave",QString("cKeepoutHighZ_%1").arg(index),value.cKeepoutHighZ);
    setValue("ClawSave",QString("cKeepoutPosMinC_%1").arg(index),value.cKeepoutPosMinC);
    setValue("ClawSave",QString("cKeepoutPosMaxC_%1").arg(index),value.cKeepoutPosMaxC);
    setValue("ClawSave",QString("cKeepoutNegMinC_%1").arg(index),value.cKeepoutNegMinC);
    setValue("ClawSave",QString("cKeepoutNegMaxC_%1").arg(index),value.cKeepoutNegMaxC);
}
//联机安全
void getOnlineSafe(uint8_t defaultV)
{
    for(int i=0;i<ONLINE_SAVE_NUM;i++)
    {
        m_OnlineSave[i].mainSunMode=getValue("OnlineSave",QString("mainSunMode_%1").arg(i),defaultV);
        m_OnlineSave[i].pluseTime=getValue("OnlineSave",QString("pluseTime_%1").arg(i),(uint16_t)defaultV);
        m_OnlineSave[i].onlineSelect=getValue("OnlineSave",QString("onlineSelect_%1").arg(i),defaultV);
        m_OnlineSave[i].axisNum=getValue("OnlineSave",QString("axisNum_%1").arg(i),defaultV);
        m_OnlineSave[i].areaInNum=getValue("OnlineSave",QString("areaInNum_%1").arg(i),defaultV);
        m_OnlineSave[i].areaOutNum=getValue("OnlineSave",QString("areaOutNum_%1").arg(i),defaultV);
        m_OnlineSave[i].requestInNum=getValue("OnlineSave",QString("requestInNum_%1").arg(i),defaultV);
        m_OnlineSave[i].requestOutNum=getValue("OnlineSave",QString("requestOutNum_%1").arg(i),defaultV);
        m_OnlineSave[i].a1A2MainPos=getValue("OnlineSave",QString("a1A2MainPos_%1").arg(i),(int32_t)defaultV);
        m_OnlineSave[i].a1A2SunPos=getValue("OnlineSave",QString("a1A2SunPos_%1").arg(i),(int32_t)defaultV);
    }
}
void setOnlineSafe(D_OnlineSaveStruct value,uint8_t index)
{
    setValue("OnlineSave",QString("mainSunMode_%1").arg(index),value.mainSunMode);
    setValue("OnlineSave",QString("pluseTime_%1").arg(index),value.pluseTime);
    setValue("OnlineSave",QString("onlineSelect_%1").arg(index),value.onlineSelect);
    setValue("OnlineSave",QString("axisNum_%1").arg(index),value.axisNum);
    setValue("OnlineSave",QString("areaInNum_%1").arg(index),value.areaInNum);
    setValue("OnlineSave",QString("areaOutNum_%1").arg(index),value.areaOutNum);
    setValue("OnlineSave",QString("requestInNum_%1").arg(index),value.requestInNum);
    setValue("OnlineSave",QString("requestOutNum_%1").arg(index),value.requestOutNum);
    setValue("OnlineSave",QString("a1A2MainPos_%1").arg(index),value.a1A2MainPos);
    setValue("OnlineSave",QString("a1A2SunPos_%1").arg(index),value.a1A2SunPos);
}
//产品设置
void getProductSet(uint16_t defaultV)
{
    m_ProductSet.waitProcessFinTime=getValue("ProductSet","waitProcessFinTime",defaultV);
    m_ProductSet.pluseOutTime=getValue("ProductSet","pluseOutTime",defaultV);
    m_ProductSet.airVlaveLimitTime=getValue("ProductSet","airVlaveLimitTime",defaultV);
    m_ProductSet.autoCycTime=getValue("ProductSet","autoCycTime",defaultV);
    m_ProductSet.alarmBuzzerTime=getValue("ProductSet","alarmBuzzerTime",defaultV);
    m_ProductSet.chainReachDelay=getValue("ProductSet","chainReachDelay",defaultV);
    m_ProductSet.clampCheckFilterTime=getValue("ProductSet","clampCheckFilterTime",defaultV);
}
void setProductSet(D_ProductSetStruct value)
{
    setValue("ProductSet","waitProcessFinTime",value.waitProcessFinTime);
    setValue("ProductSet","pluseOutTime",value.pluseOutTime);
    setValue("ProductSet","airVlaveLimitTime",value.airVlaveLimitTime);
    setValue("ProductSet","autoCycTime",value.autoCycTime);
    setValue("ProductSet","alarmBuzzerTime",value.alarmBuzzerTime);
    setValue("ProductSet","chainReachDelay",value.chainReachDelay);
    setValue("ProductSet","clampCheckFilterTime",value.clampCheckFilterTime);
}

//产品设置高级功能
void getProductSenior(uint8_t defaultV)
{
    m_ProductSenior.lubricOpenTime=getValue("ProductSenior","lubricOpenTime",(uint16_t)defaultV);
    m_ProductSenior.lubricCloseTime=getValue("ProductSenior","lubricCloseTime",(uint16_t)defaultV);
    m_ProductSenior.lubricDelay=getValue("ProductSenior","lubricDelay",(uint16_t)defaultV);
    m_ProductSenior.lubricGapCyc=getValue("ProductSenior","lubricGapCyc",(uint16_t)defaultV);
    m_ProductSenior.productMemoryFlag=getValue("ProductSenior","productMemoryFlag",defaultV);
}
void setProductSenior(D_ProductSeniorStruct value)
{
    setValue("ProductSenior","lubricOpenTime",value.lubricOpenTime);
    setValue("ProductSenior","lubricCloseTime",value.lubricCloseTime);
    setValue("ProductSenior","lubricDelay",value.lubricDelay);
    setValue("ProductSenior","lubricGapCyc",value.lubricGapCyc);
    setValue("ProductSenior","productMemoryFlag",value.productMemoryFlag);
}
//物联网
void getInternet(uint8_t defaultV)
{
    m_Internet.netCommType=getValue("Internet","netCommType",defaultV);
    m_Internet.parityMode=getValue("Internet","parityMode",defaultV);
    m_Internet.baudMode=getValue("Internet","baudMode",defaultV);
}
void setInternet(D_InternetStruct value)
{
    setValue("Internet","netCommType",value.netCommType);
    setValue("Internet","parityMode",value.parityMode);
    setValue("Internet","baudMode",value.baudMode);
}
//系统参数
void getSystemSet()
{
    D_SystemSetStruct defaultV{0, 0, 0, 0, 0, 0, "", 0};

    QSettings settings(SysSetConfigPath, QSettings::IniFormat);
    settings.setIniCodec(QTextCodec::codecForName("utf-8"));

    settings.beginGroup("SystemSet");

    m_SystemSet.lan = settings.value("lan",defaultV.lan).toUInt();
    m_SystemSet.typeFace = settings.value("typeFace", defaultV.typeFace).toUInt();
    m_SystemSet.wordSize = settings.value("wordSize", defaultV.wordSize).toUInt();
    m_SystemSet.keyListen = settings.value("keyListen", defaultV.keyListen).toUInt();
    m_SystemSet.backlightTime = settings.value("backlightTime", defaultV.backlightTime).toUInt();
    m_SystemSet.backlightBrightness = settings.value("backlightBrightness", defaultV.backlightBrightness).toUInt();
    m_SystemSet.sysName = settings.value("sysName", defaultV.sysName).toString();
    m_SystemSet.sysColor = settings.value("sysColor", defaultV.sysColor).toUInt();

    settings.endGroup();
}
void setSystemSet(D_SystemSetStruct value)
{
    QSettings settings(SysSetConfigPath, QSettings::IniFormat);
    settings.setIniCodec(QTextCodec::codecForName("utf-8"));

    settings.beginGroup("SystemSet");

    settings.setValue("lan", value.lan);
    settings.setValue("typeFace", value.typeFace);
    settings.setValue("wordSize", value.wordSize);
    settings.setValue("keyListen", value.keyListen);
    settings.setValue("backlightTime", value.backlightTime);
    settings.setValue("backlightBrightness", value.backlightBrightness);
    settings.setValue("sysName", value.sysName);
    settings.setValue("sysColor", value.sysColor);

    settings.endGroup();
}
//轴运动参数
void getAxisPar(uint8_t defaultV)
{
    for(int i=0;i<AXIS_TOTAL_NUM;i++)
    {
        m_AxisPar[i].axisType=getValue("AxisPar",QString("axisType_%1").arg(i),defaultV);
        m_AxisPar[i].axisMoveMade=getValue("AxisPar",QString("axisMoveMade_%1").arg(i),defaultV);

        m_AxisPar[i].accTime=getValue("AxisPar",QString("accTime_%1").arg(i),(uint16_t)defaultV);
        m_AxisPar[i].decTime=getValue("AxisPar",QString("decTime_%1").arg(i),(uint16_t)defaultV);
        m_AxisPar[i].accAcc=getValue("AxisPar",QString("accAcc_%1").arg(i),(uint16_t)defaultV);
        m_AxisPar[i].decDec=getValue("AxisPar",QString("decDec_%1").arg(i),(uint16_t)defaultV);
        m_AxisPar[i].maxSpeed=getValue("AxisPar",QString("maxSpeed_%1").arg(i),(uint16_t)defaultV);
        m_AxisPar[i].axisMinPos=getValue("AxisPar",QString("axisMinPos_%1").arg(i),(int32_t)defaultV);
        m_AxisPar[i].axisMaxPos=getValue("AxisPar",QString("axisMaxPos_%1").arg(i),(int32_t)defaultV);

        m_AxisPar[i].minPosSignal=getValue("AxisPar",QString("minPosSignal_%1").arg(i),defaultV);
        m_AxisPar[i].maxPosSignal=getValue("AxisPar",QString("maxPosSignal_%1").arg(i),defaultV);

        m_AxisPar[i].circlePluseNum=getValue("AxisPar",QString("circlePluseNum_%1").arg(i),(int32_t)defaultV);
        m_AxisPar[i].circleDis=getValue("AxisPar",QString("circleDis_%1").arg(i),(int32_t)defaultV);

        m_AxisPar[i].findOriginSpeed=getValue("AxisPar",QString("findOriginSpeed_%1").arg(i),(uint16_t)defaultV);
        m_AxisPar[i].leaveOriginSpeed=getValue("AxisPar",QString("leaveOriginSpeed_%1").arg(i),(uint16_t)defaultV);
        m_AxisPar[i].originOffset=getValue("AxisPar",QString("originOffset_%1").arg(i),(int32_t)defaultV);
        m_AxisPar[i].originDir=getValue("AxisPar",QString("originDir_%1").arg(i),defaultV);

        m_AxisPar[i].originSignal=getValue("AxisPar",QString("originSignal_%1").arg(i),defaultV);

        m_AxisPar[i].backOriginOrder=getValue("AxisPar",QString("backOriginOrder_%1").arg(i),defaultV);
        m_AxisPar[i].originType=getValue("AxisPar",QString("originType_%1").arg(i),defaultV);

        m_AxisPar[i].limitPosSwt=getValue("AxisPar",QString("limitPosSwt_%1").arg(i),defaultV);
        m_AxisPar[i].limitNegSwt=getValue("AxisPar",QString("limitNegSwt_%1").arg(i),defaultV);
        m_AxisPar[i].coordDir=getValue("AxisPar",QString("coordDir_%1").arg(i),defaultV);
    }
}
void setAxisPar(D_AxisParStruct value, uint8_t index)
{
//    for(int i=0;i<AXIS_TOTAL_NUM;i++)
    {
        setValue("AxisPar",QString("axisType_%1").arg(index),value.axisType);
        setValue("AxisPar",QString("axisMoveMade_%1").arg(index),value.axisMoveMade);

        setValue("AxisPar",QString("accTime_%1").arg(index),value.accTime);
        setValue("AxisPar",QString("decTime_%1").arg(index),value.decTime);
        setValue("AxisPar",QString("accAcc_%1").arg(index),value.accAcc);
        setValue("AxisPar",QString("decDec_%1").arg(index),value.decDec);
        setValue("AxisPar",QString("maxSpeed_%1").arg(index),value.maxSpeed);
        setValue("AxisPar",QString("axisMinPos_%1").arg(index),value.axisMinPos);
        setValue("AxisPar",QString("axisMaxPos_%1").arg(index),value.axisMaxPos);

        setValue("AxisPar",QString("minPosSignal_%1").arg(index),value.minPosSignal);
        setValue("AxisPar",QString("maxPosSignal_%1").arg(index),value.maxPosSignal);

        setValue("AxisPar",QString("circlePluseNum_%1").arg(index),value.circlePluseNum);
        setValue("AxisPar",QString("circleDis_%1").arg(index),value.circleDis);

        setValue("AxisPar",QString("findOriginSpeed_%1").arg(index),value.findOriginSpeed);
        setValue("AxisPar",QString("leaveOriginSpeed_%1").arg(index),value.leaveOriginSpeed);
        setValue("AxisPar",QString("originOffset_%1").arg(index),value.originOffset);
        setValue("AxisPar",QString("originDir_%1").arg(index),value.originDir);

        setValue("AxisPar",QString("originSignal_%1").arg(index),value.originSignal);

        setValue("AxisPar",QString("backOriginOrder_%1").arg(index),value.backOriginOrder);
        setValue("AxisPar",QString("originType_%1").arg(index),value.originType);

        setValue("AxisPar",QString("limitPosSwt_%1").arg(index),value.limitPosSwt);
        setValue("AxisPar",QString("limitNegSwt_%1").arg(index),value.limitNegSwt);
        setValue("AxisPar",QString("coordDir_%1").arg(index),value.coordDir);
    }
}
//首轮运动参数
void getHandwheelPar(uint8_t defaultV)
{
    m_HandwheelPar.axisType=getValue("HandwheelPar","axisType",defaultV);
    m_HandwheelPar.axisMoveMade=getValue("HandwheelPar","axisMoveMade",defaultV);

    m_HandwheelPar.accTime=getValue("HandwheelPar","accTime",(uint16_t)defaultV);
    m_HandwheelPar.decTime=getValue("HandwheelPar","decTime",(uint16_t)defaultV);
    m_HandwheelPar.accAcc=getValue("HandwheelPar","accAcc",(uint16_t)defaultV);
    m_HandwheelPar.decDec=getValue("HandwheelPar","decDec",(uint16_t)defaultV);
    m_HandwheelPar.maxSpeed=getValue("HandwheelPar","maxSpeed",(uint16_t)defaultV);
    m_HandwheelPar.axisMinPos=getValue("HandwheelPar","axisMinPos",(int32_t)defaultV);
    m_HandwheelPar.axisMaxPos=getValue("HandwheelPar","axisMaxPos",(int32_t)defaultV);

    m_HandwheelPar.circlePluseNum=getValue("HandwheelPar","circlePluseNum",(int32_t)defaultV);
    m_HandwheelPar.circleDis=getValue("HandwheelPar","circleDis",(int32_t)defaultV);

    m_HandwheelPar.findOriginSpeed=getValue("HandwheelPar","findOriginSpeed",(uint16_t)defaultV);
    m_HandwheelPar.leaveOriginSpeed=getValue("HandwheelPar","leaveOriginSpeed",(uint16_t)defaultV);
    m_HandwheelPar.originOffset=getValue("HandwheelPar","originOffset",(int32_t)defaultV);
    m_HandwheelPar.originDir=getValue("HandwheelPar","originDir",defaultV);

    m_HandwheelPar.backOriginOrder=getValue("HandwheelPar","backOriginOrder",defaultV);
    m_HandwheelPar.originType=getValue("HandwheelPar","originType",defaultV);

    m_HandwheelPar.limitPosSwt=getValue("HandwheelPar","limitPosSwt",defaultV);
    m_HandwheelPar.limitNegSwt=getValue("HandwheelPar","limitNegSwt",defaultV);
    m_HandwheelPar.coordDir=getValue("HandwheelPar","coordDir",defaultV);
}
void setHandwheelPar(D_AxisParStruct value)
{
    setValue("HandwheelPar","axisType",value.axisType);
    setValue("HandwheelPar","axisMoveMade",value.axisMoveMade);

    setValue("HandwheelPar","accTime",value.accTime);
    setValue("HandwheelPar","decTime",value.decTime);
    setValue("HandwheelPar","accAcc",value.accAcc);
    setValue("HandwheelPar","decDec",value.decDec);
    setValue("HandwheelPar","maxSpeed",value.maxSpeed);
    setValue("HandwheelPar","axisMinPos",value.axisMinPos);
    setValue("HandwheelPar","axisMaxPos",value.axisMaxPos);

    setValue("HandwheelPar","circlePluseNum",value.circlePluseNum);
    setValue("HandwheelPar","circleDis",value.circleDis);

    setValue("HandwheelPar","findOriginSpeed",value.findOriginSpeed);
    setValue("HandwheelPar","leaveOriginSpeed",value.leaveOriginSpeed);
    setValue("HandwheelPar","originOffset",value.originOffset);
    setValue("HandwheelPar","originDir",value.originDir);

    setValue("HandwheelPar","backOriginOrder",value.backOriginOrder);
    setValue("HandwheelPar","originType",value.originType);

    setValue("HandwheelPar","limitPosSwt",value.limitPosSwt);
    setValue("HandwheelPar","limitNegSwt",value.limitNegSwt);
    setValue("HandwheelPar","coordDir",value.coordDir);
}
//伺服结构参数
void getServoPar(uint8_t defaultV)
{
    m_ServoPar.encoderType=getValue("ServoPar","encoderType",defaultV);
    m_ServoPar.encoderBitNum=getValue("ServoPar","encoderBitNum",defaultV);
    m_ServoPar.servoBrand=getValue("ServoPar","servoBrand",defaultV);
    m_ServoPar.servoModel=getValue("ServoPar","servoModel",defaultV);
    m_ServoPar.extendIOBoard=getValue("ServoPar","extendIOBoard",defaultV);
    m_ServoPar.rotateAxisUnit=getValue("ServoPar","rotateAxisUnit",defaultV);
    m_ServoPar.resetSunProFlag=getValue("ServoPar","resetSunProFlag",defaultV);
    m_ServoPar.torqueLimitFlag=getValue("ServoPar","torqueLimitFlag",defaultV);
}
void setServoPar(D_ServoParStruct value)
{
    setValue("ServoPar","encoderType",value.encoderType);
    setValue("ServoPar","encoderBitNum",value.encoderBitNum);
    setValue("ServoPar","servoBrand",value.servoBrand);
    setValue("ServoPar","servoModel",value.servoModel);
    setValue("ServoPar","extendIOBoard",value.extendIOBoard);
    setValue("ServoPar","rotateAxisUnit",value.rotateAxisUnit);
    setValue("ServoPar","resetSunProFlag",value.resetSunProFlag);
    setValue("ServoPar","torqueLimitFlag",value.torqueLimitFlag);
}
//伺服安全及控制精度参数
void getDegreeParS(uint8_t defaultV)
{
    m_DegreeParS.tolerance=getValue("DegreeParS","tolerance",(int32_t)defaultV);
    m_DegreeParS.originDegree=getValue("DegreeParS","originDegree",(int32_t)defaultV);
    m_DegreeParS.actionFinDegree=getValue("DegreeParS","actionFinDegree",(uint16_t)defaultV);
    m_DegreeParS.saveRampageHige_Z1=getValue("DegreeParS","saveRampageHige_Z1",(int32_t)defaultV);
    m_DegreeParS.saveRampageHige_Z2=getValue("DegreeParS","saveRampageHige_Z2",(int32_t)defaultV);
    m_DegreeParS.saveRotatePos_Y1=getValue("DegreeParS","saveRotatePos_Y1",(int32_t)defaultV);
    m_DegreeParS.saveRotatePos_Y2=getValue("DegreeParS","saveRotatePos_Y2",(int32_t)defaultV);
}
void setDegreeParS(D_DegreeParStruct value)
{
    setValue("DegreeParS","tolerance",value.tolerance);
    setValue("DegreeParS","originDegree",value.originDegree);
    setValue("DegreeParS","actionFinDegree",value.actionFinDegree);
    setValue("DegreeParS","saveRampageHige_Z1",value.saveRampageHige_Z1);
    setValue("DegreeParS","saveRampageHige_Z2",value.saveRampageHige_Z2);
    setValue("DegreeParS","saveRotatePos_Y1",value.saveRotatePos_Y1);
    setValue("DegreeParS","saveRotatePos_Y2",value.saveRotatePos_Y2);
}
//安全区参数
void getSaveArea(uint8_t defaultV)
{
    for(int i=0;i<SAVE_AREA_NUM;i++)
    {
        m_SaveArea[i].saveAreaSwt=getValue("SaveArea",QString("saveAreaSwt_%1").arg(i),(uint16_t)defaultV);
//        m_SaveArea[i].processFinishNum=getValue("SaveArea",QString("saveAreaSwt_%1").arg(i),(uint16_t)defaultV);//这个参数为啥子不存
        for(int j=0;j<SAVE_AXIS_NUM;j++)
        {
            m_SaveArea[i].axisSelect[j]=getValue("SaveArea",QString("axisSelect_%1_%2").arg(i).arg(j),defaultV);
            m_SaveArea[i].machineA1Pos[j]=getValue("SaveArea",QString("machineA1Pos_%1_%2").arg(i).arg(j),(int32_t)defaultV);
            m_SaveArea[i].machineA2Pos[j]=getValue("SaveArea",QString("machineA2Pos_%1_%2").arg(i).arg(j),(int32_t)defaultV);
            m_SaveArea[i].starckB1Pos[j]=getValue("SaveArea",QString("starckB1Pos_%1_%2").arg(i).arg(j),(int32_t)defaultV);
            m_SaveArea[i].starckB2Pos[j]=getValue("SaveArea",QString("starckB2Pos_%1_%2").arg(i).arg(j),(int32_t)defaultV);
        }
        m_SaveArea[i].machine_Z_WaitMaxPos=getValue("SaveArea",QString("machine_Z_WaitMaxPos_%1").arg(i),(int32_t)defaultV);
        m_SaveArea[i].machine_Z_FallMaxPos=getValue("SaveArea",QString("machine_Z_FallMaxPos_%1").arg(i),(int32_t)defaultV);
        m_SaveArea[i].machine_Z_InsideHigh=getValue("SaveArea",QString("machine_Z_InsideHigh_%1").arg(i),(int32_t)defaultV);
        m_SaveArea[i].stack_Z_StartMaxPos=getValue("SaveArea",QString("stack_Z_StartMaxPos_%1").arg(i),(int32_t)defaultV);
        m_SaveArea[i].stack_Z_FallMaxPos=getValue("SaveArea",QString("stack_Z_FallMaxPos_%1").arg(i),(int32_t)defaultV);
    }
}
void setSaveArea(D_SaveAreaStruct value, uint8_t index)
{
    setValue("SaveArea",QString("saveAreaSwt_%1").arg(index),value.saveAreaSwt);
    for(int j=0;j<SAVE_AXIS_NUM;j++)
    {
        setValue("SaveArea",QString("axisSelect_%1_%2").arg(index).arg(j),value.axisSelect[j]);
        setValue("SaveArea",QString("machineA1Pos_%1_%2").arg(index).arg(j),value.machineA1Pos[j]);
        setValue("SaveArea",QString("machineA2Pos_%1_%2").arg(index).arg(j),value.machineA2Pos[j]);
        setValue("SaveArea",QString("starckB1Pos_%1_%2").arg(index).arg(j),value.starckB1Pos[j]);
        setValue("SaveArea",QString("starckB2Pos_%1_%2").arg(index).arg(j),value.starckB2Pos[j]);
    }
    setValue("SaveArea",QString("machine_Z_WaitMaxPos_%1").arg(index),value.machine_Z_WaitMaxPos);
    setValue("SaveArea",QString("machine_Z_FallMaxPos_%1").arg(index),value.machine_Z_FallMaxPos);
    setValue("SaveArea",QString("machine_Z_InsideHigh_%1").arg(index),value.machine_Z_InsideHigh);
    setValue("SaveArea",QString("stack_Z_StartMaxPos_%1").arg(index),value.stack_Z_StartMaxPos);
    setValue("SaveArea",QString("stack_Z_FallMaxPos_%1").arg(index),value.stack_Z_FallMaxPos);
}
//伺服通信协议参数
void getServoCommPar(uint8_t defaultV)
{
    for(int i=0;i<SERVE_COMM_PAR_NUM;i++)
    {
        m_ServoCommPar[i].saveFlag=getValue("ServoCommPar",QString("saveFlag_%1").arg(i),defaultV);
        m_ServoCommPar[i].powerOnSet=getValue("ServoCommPar",QString("powerOnSet_%1").arg(i),defaultV);
        m_ServoCommPar[i].axis=getValue("ServoCommPar",QString("axis_%1").arg(i),defaultV);
        m_ServoCommPar[i].index=getValue("ServoCommPar",QString("index_%1").arg(i),(uint16_t)defaultV);
        m_ServoCommPar[i].sunIndex=getValue("ServoCommPar",QString("sunIndex_%1").arg(i),defaultV);
        m_ServoCommPar[i].parLen=getValue("ServoCommPar",QString("parLen_%1").arg(i),defaultV);
        m_ServoCommPar[i].parValue=getValue("ServoCommPar",QString("parValue_%1").arg(i),(int32_t)defaultV);
    }
}
void setServoCommPar(D_ServoCommParSetStruct* value)
{
    for(int i=0;i<SERVE_COMM_PAR_NUM;i++)
    {
        setValue("ServoCommPar",QString("saveFlag_%1").arg(i),value[i].saveFlag);
        setValue("ServoCommPar",QString("powerOnSet_%1").arg(i),value[i].powerOnSet);
        setValue("ServoCommPar",QString("axis_%1").arg(i),value[i].axis);
        setValue("ServoCommPar",QString("index_%1").arg(i),value[i].index);
        setValue("ServoCommPar",QString("sunIndex_%1").arg(i),value[i].sunIndex);
        setValue("ServoCommPar",QString("parLen_%1").arg(i),value[i].parLen);
        setValue("ServoCommPar",QString("parValue_%1").arg(i),value[i].parValue);
    }

}
//堆叠信息参数
void getStackInfo(uint8_t defaultV)
{
    for(int i=0;i<STACK_TOTAL_NUM;i++)
    {
        m_StackInfo[i].stackOrder=getValue("StackInfo",QString("stackOrder_%1").arg(i),defaultV);
        m_StackInfo[i].countMode=getValue("StackInfo",QString("countMode_%1").arg(i),defaultV);
        m_StackInfo[i].stackFinStopFlag=getValue("StackInfo",QString("stackFinStopFlag_%1").arg(i),defaultV);
        m_StackInfo[i].groupLeaveBlank=getValue("StackInfo",QString("groupLeaveBlank_%1").arg(i),defaultV);
        m_StackInfo[i].stackMoveMode=getValue("StackInfo",QString("stackMoveMode_%1").arg(i),defaultV);
        for(int j=0;j<STACK_AXIS_NUM;j++)
        {
            m_StackInfo[i].axisSelect[j]=getValue("StackInfo",QString("axisSelect_%1_%2").arg(i).arg(j),defaultV);
            m_StackInfo[i].axisSpeed[j]=getValue("StackInfo",QString("axisSpeed_%1_%2").arg(i).arg(j),defaultV);
            m_StackInfo[i].stackPointNum[j]=getValue("StackInfo",QString("stackPointNum_%1_%2").arg(i).arg(j),defaultV);
            m_StackInfo[i].stackDir[j]=getValue("StackInfo",QString("stackDir_%1_%2").arg(i).arg(j),defaultV);
            m_StackInfo[i].dischangeSpeed[j]=getValue("StackInfo",QString("dischangeSpeed_%1_%2").arg(i).arg(j),defaultV);

            m_StackInfo[i].stackStartPos[j]=getValue("StackInfo",QString("stackStartPos_%1_%2").arg(i).arg(j),(int32_t)defaultV);
            m_StackInfo[i].stack_X_EndPos[j]=getValue("StackInfo",QString("stack_X_EndPos_%1_%2").arg(i).arg(j),(int32_t)defaultV);
            m_StackInfo[i].stack_Y_EndPos[j]=getValue("StackInfo",QString("stack_Y_EndPos_%1_%2").arg(i).arg(j),(int32_t)defaultV);
            m_StackInfo[i].stackDiagonalPos[j]=getValue("StackInfo",QString("stackDiagonalPos_%1_%2").arg(i).arg(j),(int32_t)defaultV);

            m_StackInfo[i].dischangeStartPos[j]=getValue("StackInfo",QString("dischangeStartPos_%1_%2").arg(i).arg(j),(int32_t)defaultV);
            m_StackInfo[i].intevalDis[j]=getValue("StackInfo",QString("intevalDis_%1_%2").arg(i).arg(j),(int32_t)defaultV);
            m_StackInfo[i].offsetDis[j]=getValue("StackInfo",QString("offsetDis_%1_%2").arg(i).arg(j),(int32_t)defaultV);
        }
    }
}
void setStackInfo(D_StackInfoStruct value, uint8_t index)
{
    setValue("StackInfo",QString("stackOrder_%1").arg(index),value.stackOrder);
    setValue("StackInfo",QString("countMode_%1").arg(index),value.countMode);
    setValue("StackInfo",QString("stackFinStopFlag_%1").arg(index),value.stackFinStopFlag);
    setValue("StackInfo",QString("groupLeaveBlank_%1").arg(index),value.groupLeaveBlank);
    setValue("StackInfo",QString("stackMoveMode_%1").arg(index),value.stackMoveMode);
    for(int j=0;j<STACK_AXIS_NUM;j++)
    {
        setValue("StackInfo",QString("axisSelect_%1_%2").arg(index).arg(j),value.axisSelect[j]);
        setValue("StackInfo",QString("axisSpeed_%1_%2").arg(index).arg(j),value.axisSpeed[j]);
        setValue("StackInfo",QString("stackPointNum_%1_%2").arg(index).arg(j),value.stackPointNum[j]);
        setValue("StackInfo",QString("stackDir_%1_%2").arg(index).arg(j),value.stackDir[j]);
        setValue("StackInfo",QString("dischangeSpeed_%1_%2").arg(index).arg(j),value.dischangeSpeed[j]);

        setValue("StackInfo",QString("stackStartPos_%1_%2").arg(index).arg(j),value.stackStartPos[j]);
        setValue("StackInfo",QString("stack_X_EndPos_%1_%2").arg(index).arg(j),value.stack_X_EndPos[j]);
        setValue("StackInfo",QString("stack_Y_EndPos_%1_%2").arg(index).arg(j),value.stack_Y_EndPos[j]);
        setValue("StackInfo",QString("stackDiagonalPos_%1_%2").arg(index).arg(j),value.stackDiagonalPos[j]);

        setValue("StackInfo",QString("dischangeStartPos_%1_%2").arg(index).arg(j),value.dischangeStartPos[j]);
        setValue("StackInfo",QString("intevalDis_%1_%2").arg(index).arg(j),value.intevalDis[j]);
        setValue("StackInfo",QString("offsetDis_%1_%2").arg(index).arg(j),value.offsetDis[j]);
    }
}
//堆叠功能参数
void getStackFunc(uint8_t defaultV)
{
    m_StackFunc.stackType=getValue("StackFunc","stackType",defaultV);
    m_StackFunc.groupStack=getValue("StackFunc","groupStack",defaultV);
    m_StackFunc.rowStack=getValue("StackFunc","rowStack",defaultV);
    m_StackFunc.storeyStack=getValue("StackFunc","storeyStack",defaultV);
    m_StackFunc.divideStack=getValue("StackFunc","divideStack",defaultV);
    m_StackFunc.stackMemory=getValue("StackFunc","stackMemory",defaultV);
    m_StackFunc.siloType=getValue("StackFunc","siloType",defaultV);
    m_StackFunc.rotateSiloFinHint=getValue("StackFunc","rotateSiloFinHint",defaultV);
    m_StackFunc.rotateSiloPlaceNum=getValue("StackFunc","rotateSiloPlaceNum",defaultV);
}
void setStackFunc(D_StackFuncStruct value)
{
    setValue("StackFunc","stackType",value.stackType);
    setValue("StackFunc","groupStack",value.groupStack);
    setValue("StackFunc","rowStack",value.rowStack);
    setValue("StackFunc","storeyStack",value.storeyStack);
    setValue("StackFunc","divideStack",value.divideStack);
    setValue("StackFunc","stackMemory",value.stackMemory);
    setValue("StackFunc","siloType",value.siloType);
    setValue("StackFunc","rotateSiloFinHint",value.rotateSiloFinHint);
    setValue("StackFunc","rotateSiloPlaceNum",value.rotateSiloPlaceNum);
}
void getManualAxis(uint8_t defaultV)
{
    m_manualAxis.axis = getValue("ManualAxis","axis",defaultV);
    m_manualAxis.speed = getValue("ManualAxis","speed",(uint16_t)defaultV);
    m_manualAxis.pos_pos = getValue("ManualAxis","pos_pos",(uint32_t)defaultV);
    m_manualAxis.sub_pos = getValue("ManualAxis","sub_pos",(uint32_t)defaultV);
    m_manualAxis.ZDrop = getValue("ManualAxis","ZDrop",defaultV);
    m_manualAxis.handwheelMode = getValue("ManualAxis","handwheelMode",defaultV);
    m_manualAxis.multiply = getValue("ManualAxis","multiply",defaultV);
    m_manualAxis.handwheelAxis = getValue("ManualAxis","handwheelAxis",defaultV);
}
void setManualAxis(D_ManualAxis value)
{
    setValue("ManualAxis","axis",value.axis);
    setValue("ManualAxis","speed",value.speed);
    setValue("ManualAxis","pos_pos",value.pos_pos);
    setValue("ManualAxis","sub_pos",value.sub_pos);
    setValue("ManualAxis","ZDrop",value.ZDrop);
    setValue("ManualAxis","handwheelMode",value.handwheelMode);
    setValue("ManualAxis","multiply",value.multiply);
    setValue("ManualAxis","handwheelAxis",value.handwheelAxis);
}
//设置端口自定义界面修改端口名称和修改端口号
void setPortDefineNameOrPortNum()
{
    ConfigPortDefine.setIniCodec("UTF-8");
    for(int i=0;i<INPUT_TOTAL_NUM;i++)
    {
        ConfigPortDefine.beginGroup("Port_X_Name_Modify");
        ConfigPortDefine.setValue(QString("%1").arg(i),m_Port_X[i].modifyName);
        ConfigPortDefine.endGroup();

        ConfigPortDefine.beginGroup("Port_X_Number_Modify");
        ConfigPortDefine.setValue(QString("%1").arg(i),m_Port_X[i].modifyPort);
        ConfigPortDefine.endGroup();
    }
    for(int i=0;i<OUTPUT_TOTAL_NUM;i++)
    {
        ConfigPortDefine.beginGroup("Port_Y_Name_Modify");
        ConfigPortDefine.setValue(QString("%1").arg(i),m_Port_Y[i].modifyName);
        ConfigPortDefine.endGroup();

        ConfigPortDefine.beginGroup("Port_Y_Number_Modify");
        ConfigPortDefine.setValue(QString("%1").arg(i),m_Port_Y[i].modifyPort);
        ConfigPortDefine.endGroup();

        ConfigPortDefine.beginGroup("Port_Y_functionset");
        ConfigPortDefine.setValue(QString("%1").arg(i),m_Port_Y[i].functionSet);
        ConfigPortDefine.endGroup();
    }
}
/**********************************
***********程序存储*****************
***********************************/

//读取所有文件程序信息
void getProgramNameAndPath()
{
    D_ProgramNameAndPathStruct P_NamePathTemp;
    QStringList str_name=getValue("File","name","").split(";");
    QStringList str_path=getValue("File","path","").split(";");
    QStringList str_index=getValue("File","index","").split(";");
    QStringList str_Permission=getValue("File","Permission","").split(";");
    QStringList str_time=getValue("File","time","").split(";");
    if(str_name.count()==1&&str_name[0]=="")
        return;
    if(str_name.count() == str_path.count() && str_name.count() == str_index.count()  && str_name.count() == str_Permission.count()  && str_name.count() == str_time.count())
    {
        for(int i=0;i<str_name.count();i++)
        {
            P_NamePathTemp.fileName=str_name[i];
            P_NamePathTemp.filePath=str_path[i];
            P_NamePathTemp.index=str_index[i].toUInt();
            P_NamePathTemp.filePermission=str_Permission[i].toUInt();
            P_NamePathTemp.changeTime=str_time[i];
            m_ProgramNameAndPath.append(P_NamePathTemp);
        }
    }
}
//保存所有文件程序信息
void setProgramNameAndPath(QList<D_ProgramNameAndPathStruct> value)
{
    QString str_name;
    QString str_path;
    QString str_index;
    QString str_Permission;
    QString str_time;
    for(int i=0;i<value.count();i++)
    {
        if(i==0)
        {
            str_name+=value[i].fileName;
            str_path+=value[i].filePath;
            str_index+=QString::number(value[i].index);
            str_Permission+=QString::number(value[i].filePermission);
            str_time+=value[i].changeTime;
        }
        else
        {
            str_name=str_name+";"+value[i].fileName;
            str_path=str_path+";"+value[i].filePath;
            str_index=str_index+";"+QString::number(value[i].index);
            str_Permission=str_Permission+";"+QString::number(value[i].filePermission);
            str_time=str_time+";"+value[i].changeTime;
        }
    }
    setValue("File","name",str_name);
    setValue("File","path",str_path);
    setValue("File","index",str_index);
    setValue("File","Permission",str_Permission);
    setValue("File","time",str_time);
}
//保存开机加载程序信息
void savePowerOnReadOneProInfo(D_ProgramNameAndPathStruct value)
{
    QSettings settings(PowerOnReadOneProPath, QSettings::IniFormat);
    settings.beginGroup("PowerOnFileInfo");
    settings.setValue("name",value.fileName);
    settings.setValue("path",value.filePath);
    settings.setValue("index",value.index);
    settings.setValue("Permission",value.filePermission);
    settings.setValue("time",value.changeTime);
    settings.endGroup();
}
//读取开机加载程序信息
D_ProgramNameAndPathStruct readPowerOnReadOneProInfo()
{
    D_ProgramNameAndPathStruct ProgramNameAndPath_Temp = {"","",0,0,""};
    QSettings settings(PowerOnReadOneProPath, QSettings::IniFormat);
    settings.beginGroup("PowerOnFileInfo");
    ProgramNameAndPath_Temp.fileName = settings.value("name").toString();
    ProgramNameAndPath_Temp.filePath = settings.value("path").toString();
    ProgramNameAndPath_Temp.index = settings.value("index").toUInt();
    ProgramNameAndPath_Temp.filePermission = settings.value("Permission").toUInt();
    ProgramNameAndPath_Temp.changeTime = settings.value("time").toString();
    settings.endGroup();
    return ProgramNameAndPath_Temp;
}


QStringList getIniValues(uint8_t type, QString node)
{
    QStringList valueList;
    QString configPathTemp;
    switch (type)
    {
    case 0:
        configPathTemp=m_configCmdPath;
        break;
    case 1:
        configPathTemp=m_configPortSettingPath;
        break;
    case 2:
        configPathTemp=m_configOrderjinitPath;
        break;
    case 3:
        configPathTemp=m_configPortXYNamePath;
        break;
    case 99:
        configPathTemp=m_configPortXYNameIniPath;
        break;
    default:
        return valueList;
        break;
    }
    QSettings settings(configPathTemp,QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    if(!node.isEmpty())
    {
        settings.beginGroup(node);
    }
    QStringList keys=settings.childKeys();
    QList<int> keyList;
    for(int i=0;i<keys.count();i++)
    {
        keyList.append(keys[i].toInt());
    }
    std::sort(keyList.begin(),keyList.end());
    // 遍历键，获取对应的值
    for(int key : keyList)
    {
        valueList.append(settings.value(QString::number(key),"NULL").toString());
    }
    if(!node.isEmpty())
    {
        settings.beginGroup(node);
    }
    return valueList;
}













void savePasswdToConfig(int authority, uint32_t value)
{
    QSettings settings(PasswdConfigPath, QSettings::IniFormat);
    settings.beginGroup("Passwd");
    settings.setValue(QString("Authority_%1").arg(authority), value);
    settings.endGroup();
}

void readPasswdFromConfig()
{
    QSettings settings(PasswdConfigPath, QSettings::IniFormat);
    settings.beginGroup("Passwd");
    for (int i = 0; i < 3; i++)
    {
        passwd[i] = settings.value(QString("Authority_%1").arg(i), 12345).toUInt();
    }
    passwd[3] = settings.value(QString("Restore_Passwd").arg(3), 12345).toUInt();
    settings.endGroup();
}

void writeKeySetStrToConfig(int index, const QString &text)
{
    QSettings settings(KeyAndSignalDescriptionPath, QSettings::IniFormat);
    settings.setIniCodec(QTextCodec::codecForName("utf-8"));

    settings.beginGroup("KeyFuncDescription");
    settings.setValue(QString("Key_%1").arg(index), text);
    settings.endGroup();
}

void readKeySetStrFromConfig(std::vector<QString> &keyStrs)
{
    QSettings settings(KeyAndSignalDescriptionPath, QSettings::IniFormat);
    settings.setIniCodec(QTextCodec::codecForName("utf-8"));

    settings.beginGroup("KeyFuncDescription");

    keyStrs.resize(OPR_KEY_NUM);
    for (int i = 0; i < OPR_KEY_NUM; i++)
    {
        keyStrs[i] = settings.value(QString("Key_%1").arg(i), "0").toString();
    }
    settings.endGroup();
}


void writeSigSetStrToConfig(int index, const QString &text)
{
    QSettings settings(KeyAndSignalDescriptionPath, QSettings::IniFormat);
    settings.setIniCodec(QTextCodec::codecForName("utf-8"));

    settings.beginGroup("SignalDescription");
    settings.setValue(QString("Sig_%1").arg(index), text);
    settings.endGroup();
}

void readSigSetStrFromConfig(std::vector<QString> &sigStrs)
{
    QSettings settings(KeyAndSignalDescriptionPath, QSettings::IniFormat);
    settings.setIniCodec(QTextCodec::codecForName("utf-8"));

    settings.beginGroup("SignalDescription");

    sigStrs.resize(OPR_LED_NUM);
    for (int i = 0; i < OPR_LED_NUM; i++)
    {
        sigStrs[i] = settings.value(QString("Sig_%1").arg(i), "0").toString();
    }
    settings.endGroup();
}

void writeOnlineSafeInOutDescription(int group, const std::vector<QString> &describeStrs)
{
    QSettings settings(IOPortDescriptionPath, QSettings::IniFormat);
    settings.setIniCodec(QTextCodec::codecForName("utf-8"));

    settings.beginGroup(QString("OnlineSafe%1").arg(group));

    for (size_t i = 0; i < 4; i++)
    {
        settings.setValue(QString("Sig_%1").arg(i), describeStrs[i]);
    }
    settings.endGroup();
}

void readOnlineSafeInOutDescription(int group, std::vector<QString> &describeStrs)
{

    QSettings settings(IOPortDescriptionPath, QSettings::IniFormat);
    settings.setIniCodec(QTextCodec::codecForName("utf-8"));

    settings.beginGroup(QString("OnlineSafe%1").arg(group));

    describeStrs.resize(4);
    for (size_t i = 0; i < 4; i++)
    {
       describeStrs[i] = (settings.value(QString("Sig_%1").arg(i), "null").toString());
    }
    settings.endGroup();
}

void writeLimitSigDescription(int index, const std::vector<QString> &describeStrs)
{
    QSettings settings(IOPortDescriptionPath, QSettings::IniFormat);
    settings.setIniCodec(QTextCodec::codecForName("utf-8"));

    settings.beginGroup(QString("LimitSignal%1").arg(index));

    for (size_t i = 0; i < AXIS_TOTAL_NUM; i++)
    {
        settings.setValue(QString("Axis_%1").arg(i), describeStrs[i]);
    }
    settings.endGroup();
}

void readLimitSigDescription(int index, std::vector<QString> &describeStrs)
{
    QSettings settings(IOPortDescriptionPath, QSettings::IniFormat);
    settings.setIniCodec(QTextCodec::codecForName("utf-8"));

    settings.beginGroup(QString("LimitSignal%1").arg(index));

    describeStrs.resize(AXIS_TOTAL_NUM);
    for (size_t i = 0; i < AXIS_TOTAL_NUM; i++)
    {
        describeStrs[i] = settings.value(QString("Axis_%1").arg(i), "null").toString();
    }
    settings.endGroup();
}

void writeGuideInfo()
{
    QSettings settings(GuideInfoPath, QSettings::IniFormat);
    settings.setIniCodec(QTextCodec::codecForName("utf-8"));

    for (int i = 0; i < GUIDE_TOTAL_NUM; i++)
    {
        // Create a group for each guide entry
        QString groupName = QString("Guide%1").arg(i);  // You can use the index to create a unique group name
        settings.beginGroup(groupName);

        // Store each field in the corresponding group
        settings.setValue("guideFlag", m_Guide[i].guideFlag);
        settings.setValue("guideName", m_Guide[i].guideName);
        settings.setValue("keyDefStr", m_Guide[i].keyDefStr);
        settings.setValue("btnPos_x", m_Guide[i].btnPos.x());
        settings.setValue("btnPos_y", m_Guide[i].btnPos.y());
        settings.setValue("keyType", m_Guide[i].keyType);
        settings.setValue("portNum", m_Guide[i].portNum);
        settings.setValue("status", m_Guide[i].status);

        // End the group
        settings.endGroup();
    }
}

void readGuideInfo()
{
    QSettings settings(GuideInfoPath, QSettings::IniFormat);
    settings.setIniCodec(QTextCodec::codecForName("utf-8"));

    for (int i = 0; i < GUIDE_TOTAL_NUM; i++)
    {
        // Construct the group name for each guide entry
        QString groupName = QString("Guide%1").arg(i);
        settings.beginGroup(groupName);

        // Read each field from the corresponding group
        m_Guide[i].guideFlag = settings.value("guideFlag", 0).toUInt();
        m_Guide[i].guideName = settings.value("guideName", "").toString();
        m_Guide[i].keyDefStr = settings.value("keyDefStr", "").toString();
        m_Guide[i].btnPos.setX(settings.value("btnPos_x", 0).toInt());
        m_Guide[i].btnPos.setY(settings.value("btnPos_y", 0).toInt());
        m_Guide[i].keyType = settings.value("keyType", 0).toUInt();
        m_Guide[i].portNum = settings.value("portNum", 0).toUInt();
        m_Guide[i].status = settings.value("status", 0).toUInt();
        // End the group
        settings.endGroup();
    }
}


void readNameDefine()
{
    QSettings settings(CustomizeNameDefPath, QSettings::IniFormat);
    settings.setIniCodec(QTextCodec::codecForName("utf-8"));
    const QStringList groups = {"Default", "Modify"};

    for (int groupIndex = 0; groupIndex < groups.size(); ++groupIndex) {
        settings.beginGroup(groups[groupIndex]);

        m_NameDefine[groupIndex].adminName = settings.value("MGR").toString();
        m_NameDefine[groupIndex].operatorName = settings.value("OPR").toString();

        for (int i = 0; i < 8; i++) {
            m_NameDefine[groupIndex].subProgName[i] = settings.value(QString("SQ%1").arg(i+1)).toString();
        }
        for (int i = 0; i < AXIS_TOTAL_NUM; i++) {
            m_NameDefine[groupIndex].axisName[i] = settings.value(QString("AXIS%1").arg(i+1)).toString();
        }
        for (int i = 0; i < VAR_TOTAL_NUM; i++) {
            m_NameDefine[groupIndex].varName[i] = settings.value(QString("VAR%1").arg(i+1)).toString();
        }
        for (int i = 0; i < STACK_TOTAL_NUM; i++) {
            m_NameDefine[groupIndex].stackName[i] = settings.value(QString("STACK%1").arg(i+1)).toString();
        }
        for (int i = 0; i < FOLLOW_STACK_NUM; i++) {
            m_NameDefine[groupIndex].followStackName[i] = settings.value(QString("FOLLOW%1").arg(i+1)).toString();
        }
        for (int i = 0; i < TIME_TOTAL_NUM; i++) {
            m_NameDefine[groupIndex].timerName[i] = settings.value(QString("TIMER%1").arg(i+1)).toString();
        }

        settings.endGroup();
    }
}

void writeNameDefine()
{
    QSettings settings(CustomizeNameDefPath, QSettings::IniFormat);
    settings.setIniCodec(QTextCodec::codecForName("utf-8"));
    settings.beginGroup("Modify");

    settings.setValue("MGR", m_NameDefine[1].adminName);
    settings.setValue("OPR", m_NameDefine[1].operatorName);

    for (int i = 0; i < 8; i++) {
        settings.setValue(QString("SQ%1").arg(i+1), m_NameDefine[1].subProgName[i]);
    }
    for (int i = 0; i < AXIS_TOTAL_NUM; i++) {
        settings.setValue(QString("AXIS%1").arg(i+1), m_NameDefine[1].axisName[i]);
    }
    for (int i = 0; i < VAR_TOTAL_NUM; i++) {
        settings.setValue(QString("VAR%1").arg(i+1), m_NameDefine[1].varName[i]);
    }
    for (int i = 0; i < STACK_TOTAL_NUM; i++) {
        settings.setValue(QString("STACK%1").arg(i+1), m_NameDefine[1].stackName[i]);
    }
    for (int i = 0; i < FOLLOW_STACK_NUM; i++) {
        settings.setValue(QString("FOLLOW%1").arg(i+1), m_NameDefine[1].followStackName[i]);
    }
    for (int i = 0; i < TIME_TOTAL_NUM; i++) {
        settings.setValue(QString("TIMER%1").arg(i+1), m_NameDefine[1].timerName[i]);
    }

    settings.endGroup();
}
