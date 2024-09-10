#include "iniconfig.h"
#include <QCoreApplication>

//配置文件相对路径
QString m_configFileNamePath="/root/Ini_Para.txt";

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
    QSettings settings(m_configFileNamePath,QSettings::IniFormat);
    settings.beginGroup(prefix);
    reValue=settings.value(key,defaultV).toString();
    settings.endGroup();
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
    QSettings settings(m_configFileNamePath,QSettings::IniFormat);
    settings.beginGroup(prefix);
    settings.setValue(key,value);
    settings.endGroup();
}

uint8_t getValue(QString prefix, QString key, uint8_t defaultV)
{
    uint8_t reValue = 0;
    QSettings settings(m_configFileNamePath,QSettings::IniFormat);
    settings.beginGroup(prefix);
    reValue=settings.value(key,defaultV).value<uint8_t>();
    settings.endGroup();
    return reValue;
}

void setValue(QString prefix, QString key, uint8_t value)
{
    QSettings settings(m_configFileNamePath,QSettings::IniFormat);
    settings.beginGroup(prefix);
    settings.setValue(key,value);
    settings.endGroup();
}

uint16_t getValue(QString prefix, QString key, uint16_t defaultV)
{
    uint16_t reValue = 0;
    QSettings settings(m_configFileNamePath,QSettings::IniFormat);
    settings.beginGroup(prefix);
    reValue=settings.value(key,defaultV).value<uint16_t>();
    settings.endGroup();
    return reValue;
}

void setValue(QString prefix, QString key, uint16_t value)
{
    QSettings settings(m_configFileNamePath,QSettings::IniFormat);
    settings.beginGroup(prefix);
    settings.setValue(key,value);
    settings.endGroup();
}

uint32_t getValue(QString prefix, QString key, uint32_t defaultV)
{
    uint32_t reValue = 0;
    QSettings settings(m_configFileNamePath,QSettings::IniFormat);
    settings.beginGroup(prefix);
    reValue=settings.value(key,defaultV).value<uint32_t>();
    settings.endGroup();
    return reValue;
}

void setValue(QString prefix, QString key, uint32_t value)
{
    QSettings settings(m_configFileNamePath,QSettings::IniFormat);
    settings.beginGroup(prefix);
    settings.setValue(key,value);
    settings.endGroup();
}

int32_t getValue(QString prefix, QString key, int32_t defaultV)
{
    int32_t reValue = 0;
    QSettings settings(m_configFileNamePath,QSettings::IniFormat);
    settings.beginGroup(prefix);
    reValue=settings.value(key,defaultV).value<int32_t>();
    settings.endGroup();
    return reValue;
}

void setValue(QString prefix, QString key, int32_t value)
{
    QSettings settings(m_configFileNamePath,QSettings::IniFormat);
    settings.beginGroup(prefix);
    settings.setValue(key,value);
    settings.endGroup();
}
/*************************************************************************
**	输入参数：defaultV:默认值
**	输出参数：所有值并存入相应数组
**	函数功能：获取【输出类型】配置文件中键对应的值
**  作者：    PXC
**  开发日期：2024/7/22
**************************************************************************/
uint8_t* getOutPortType(uint8_t* defaultV)
{
    uint8_t* arr=defaultV;
    for(int i=0;i<OUT_PORT_TYPE_NUM;i++)
    {
        arr[i]=getValue("OutPortType",QString("V%1").arg(i),defaultV[i]);
    }
    return arr;
}
/*************************************************************************
**	输入参数：value:需要设置的值
**	输出参数：无
**	函数功能：设置、更新【输出类型】配置文件
**  作者：    PXC
**  开发日期：2024/7/22
**************************************************************************/
void setOutPortType(uint8_t *value)
{
    for(int i=0;i<OUT_PORT_TYPE_NUM;i++)
    {
        setValue("OutPortType",QString("V%1").arg(i),value[i]);
    }
}
//互锁设置
uint8_t* getOutportInterlock(uint8_t *defaultV)
{
    uint8_t* arr=defaultV;
    for(int i=0;i<OUT_INTERLOCK_NUM*4;i++)
    {
        arr[i] = getValue("OutportInterlock",QString("V%1").arg(i),defaultV[i]);
    }
    return arr;
}

void setOutportInterlock(uint8_t *value)
{
    for(int i=0;i<OUT_INTERLOCK_NUM*4;i++)
    {
        setValue("OutportInterlock",QString("V%1").arg(i),value[i]);
    }
}
//预留关联
uint8_t* getOutportRelevancy(uint8_t* defaultV)
{
    uint8_t* arr=defaultV;
    for(int i=0;i<OUTPUT_TOTAL_NUM;i++)
    {
        arr[i]=getValue("OutportRelevancy",QString("V%1").arg(i),defaultV[i]);
    }
    return arr;
}

void setOutportRelevancy(uint8_t *value)
{
    for(int i=0;i<OUTPUT_TOTAL_NUM;i++)
    {
        setValue("OutportRelevancy",QString("V%1").arg(i),value[i]);
    }
}
//预留出类型
uint8_t* getReleteOut(uint8_t* defaultV)
{
    uint8_t* arr=defaultV;
    for(int i=0;i<OUTPUT_TOTAL_NUM;i++)
    {
        arr[i]=getValue("ReleteOut",QString("V%1").arg(i),defaultV[i]);
    }
    return arr;
}

void setReleteOut(uint8_t *value)
{
    for(int i=0;i<OUTPUT_TOTAL_NUM;i++)
    {
        setValue("ReleteOut",QString("V%1").arg(i),value[i]);
    }
}
//按键功能
D_KeyFuncStruct* getKeyFunc(D_KeyFuncStruct* defaultV)
{
    D_KeyFuncStruct* arr=defaultV;
    for(int i=0;i<OPR_KEY_NUM;i++)
    {
        arr[i].keyType=getValue("KeyFunc",QString("V%1_keyType").arg(i),defaultV[i].keyType);
        arr[i].funcNum=getValue("KeyFunc",QString("V%1_funcNum").arg(i),defaultV[i].funcNum);
        arr[i].oprMode=getValue("KeyFunc",QString("V%1_oprMode").arg(i),defaultV[i].oprMode);
    }
    return arr;
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
D_LedFuncStruct* getLedFunc(D_LedFuncStruct* defaultV)
{
    D_LedFuncStruct* arr=defaultV;
    for(int i=0;i<OPR_LED_NUM;i++)
    {
        arr[i].ledType=getValue("LedFunc",QString("V%1_ledType").arg(i),defaultV[i].ledType);
        arr[i].funcNum=getValue("LedFunc",QString("V%1_funcNum").arg(i),defaultV[i].funcNum);
    }
    return arr;
}
void setLedFunc(D_LedFuncStruct* value)
{
    for(int i=0;i<OPR_LED_NUM;i++)
    {
        getValue("LedFunc",QString("V%1_ledType").arg(i),value[i].ledType);
        getValue("LedFunc",QString("V%1_funcNum").arg(i),value[i].funcNum);
    }
}
//信号设置高级功能
D_SeniorFuncStruct getSeniorFunc(D_SeniorFuncStruct defaultV)
{
    D_SeniorFuncStruct arr=defaultV;
    arr.knifeOrigin1Check=getValue("SeniorFunc","knifeOrigin1Check",defaultV.knifeOrigin1Check);
    arr.knifeOrigin2Check=getValue("SeniorFunc","knifeOrigin2Check",defaultV.knifeOrigin2Check);
    arr.chuckOriginCheck=getValue("SeniorFunc","chuckOriginCheck",defaultV.chuckOriginCheck);
    arr.stackSaveIn1Check=getValue("SeniorFunc","stackSaveIn1Check",defaultV.stackSaveIn1Check);
    arr.stackSaveIn2Check=getValue("SeniorFunc","stackSaveIn2Check",defaultV.stackSaveIn2Check);
    arr.stackSaveOutCheck=getValue("SeniorFunc","stackSaveOutCheck",defaultV.stackSaveOutCheck);
    arr.alarmIn1Check=getValue("SeniorFunc","alarmIn1Check",defaultV.alarmIn1Check);
    arr.alarmIn2Check=getValue("SeniorFunc","alarmIn2Check",defaultV.alarmIn2Check);

    arr.emergencyStopCheck=getValue("SeniorFunc","emergencyStopCheck",defaultV.emergencyStopCheck);
    arr.pauseStopCheck=getValue("SeniorFunc","pauseStopCheck",defaultV.pauseStopCheck);
    arr.pressureCheck=getValue("SeniorFunc","pressureCheck",defaultV.pressureCheck);
    arr.remoteAuto=getValue("SeniorFunc","remoteAuto",defaultV.remoteAuto);
    arr.remoteStop=getValue("SeniorFunc","remoteStop",defaultV.remoteStop);
    arr.bottomOilLimit=getValue("SeniorFunc","bottomOilLimit",defaultV.bottomOilLimit);
    arr.oilerSwt=getValue("SeniorFunc","oilerSwt",defaultV.oilerSwt);
    arr.lubPump=getValue("SeniorFunc","lubPump",defaultV.lubPump);

    arr.processSave1=getValue("SeniorFunc","processSave1",defaultV.processSave1);
    arr.processSave2=getValue("SeniorFunc","processSave2",defaultV.processSave2);
    arr.emergencySnapMotorEnable=getValue("SeniorFunc","emergencySnapMotorEnable",defaultV.emergencySnapMotorEnable);
    arr.emergencyStopOut=getValue("SeniorFunc","emergencyStopOut",defaultV.emergencyStopOut);
    arr.autoLight=getValue("SeniorFunc","autoLight",defaultV.autoLight);
    arr.alarmLight=getValue("SeniorFunc","alarmLight",defaultV.alarmLight);
    arr.alarmBuzzer=getValue("SeniorFunc","alarmBuzzer",defaultV.alarmBuzzer);
    arr.pauseLight=getValue("SeniorFunc","pauseLight",defaultV.pauseLight);

    arr.processFinish1=getValue("SeniorFunc","processFinish1",defaultV.processFinish1);
    arr.processFinish2=getValue("SeniorFunc","processFinish2",defaultV.processFinish2);
    arr.locateFinish1=getValue("SeniorFunc","locateFinish1",defaultV.locateFinish1);
    arr.locateFinish2=getValue("SeniorFunc","locateFinish2",defaultV.locateFinish2);
    arr.startFinish1=getValue("SeniorFunc","startFinish1",defaultV.startFinish1);
    arr.startFinish2=getValue("SeniorFunc","startFinish2",defaultV.startFinish2);
    for(int i=0;i<2;i++)
    {
        arr.res1[i]=getValue("SeniorFunc",QString("res1_%1").arg(i),defaultV.res1[i]);
    }

    arr.mainAxisRotate1=getValue("SeniorFunc","mainAxisRotate1",defaultV.mainAxisRotate1);
    arr.mainAxisRotate2=getValue("SeniorFunc","mainAxisRotate2",defaultV.mainAxisRotate2);
    arr.mainAxisLocate1=getValue("SeniorFunc","mainAxisLocate1",defaultV.mainAxisLocate1);
    arr.mainAxisLocate2=getValue("SeniorFunc","mainAxisLocate2",defaultV.mainAxisLocate2);
    arr.biowAir1=getValue("SeniorFunc","biowAir1",defaultV.biowAir1);
    arr.biowAir2=getValue("SeniorFunc","biowAir2",defaultV.biowAir2);
    for(int i=0;i<2;i++)
    {
        arr.res2[i]=getValue("SeniorFunc",QString("res2_%1").arg(i),defaultV.res2[i]);
    }

    arr.manualChuckIn1=getValue("SeniorFunc","manualChuckIn1",defaultV.manualChuckIn1);
    arr.manualChuckIn2=getValue("SeniorFunc","manualChuckIn2",defaultV.manualChuckIn2);
    arr.autoDoorCot1=getValue("SeniorFunc","autoDoorCot1",defaultV.autoDoorCot1);
    arr.autoDoorCot2=getValue("SeniorFunc","autoDoorCot2",defaultV.autoDoorCot2);
    for(int i=0;i<2;i++)
    {
        arr.res3[i]=getValue("SeniorFunc",QString("res3_%1").arg(i),defaultV.res3[i]);
    }
    return arr;
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
    setValue("SeniorFunc","startFinish1",value.startFinish1);
    setValue("SeniorFunc","startFinish2",value.startFinish2);
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
D_MachineSaveStruct getMachineSave(D_MachineSaveStruct defaultV)
{
    D_MachineSaveStruct arr=defaultV;
    arr.armFallAutoDoor1Swt=getValue("MachineSave","armFallAutoDoor1Swt",defaultV.armFallAutoDoor1Swt);
    arr.armFallAutoDoor1SwtSta=getValue("MachineSave","armFallAutoDoor1SwtSta",defaultV.armFallAutoDoor1SwtSta);
    arr.armFallAutoDoor2Swt=getValue("MachineSave","armFallAutoDoor2Swt",defaultV.armFallAutoDoor2Swt);
    arr.armFallAutoDoor2SwtSta=getValue("MachineSave","armFallAutoDoor2SwtSta",defaultV.armFallAutoDoor2SwtSta);
    return arr;
}
void setMachineSave(D_MachineSaveStruct value)
{
    setValue("MachineSave","armFallAutoDoor1Swt",value.armFallAutoDoor1Swt);
    setValue("MachineSave","armFallAutoDoor1SwtSta",value.armFallAutoDoor1SwtSta);
    setValue("MachineSave","armFallAutoDoor2Swt",value.armFallAutoDoor2Swt);
    setValue("MachineSave","armFallAutoDoor2SwtSta",value.armFallAutoDoor2SwtSta);
}

//料仓安全
D_StackSaveStruct getStackSave(D_StackSaveStruct defaultV)
{
    D_StackSaveStruct arr=defaultV;
    arr.stackSaveIn1Roatore=getValue("StackSave","stackSaveIn1Roatore",defaultV.stackSaveIn1Roatore);
    arr.stackSaveIn2Roatore=getValue("StackSave","stackSaveIn2Roatore",defaultV.stackSaveIn2Roatore);
    return arr;
}
void setStackSave(D_StackSaveStruct value)
{
    setValue("StackSave","stackSaveIn1Roatore",value.stackSaveIn1Roatore);
    setValue("StackSave","stackSaveIn2Roatore",value.stackSaveIn2Roatore);
}
//卡爪安全
D_ClawSaveStruct* getClawSave(D_ClawSaveStruct* defaultV)
{
    D_ClawSaveStruct* arr=defaultV;
    for(int i=0;i<CLAW_SAVE_NUM;i++)
    {
        arr[i].clawKeepoutSta=getValue("ClawSave",QString("clawKeepoutSta_%1").arg(i),defaultV[i].clawKeepoutSta);
        arr[i].axisSelect_X=getValue("ClawSave",QString("axisSelect_X_%1").arg(i),defaultV[i].axisSelect_X);
        arr[i].axisSelect_Z=getValue("ClawSave",QString("axisSelect_Z_%1").arg(i),defaultV[i].axisSelect_Z);
        arr[i].clawKeepoutMinX=getValue("ClawSave",QString("clawKeepoutMinX_%1").arg(i),defaultV[i].clawKeepoutMinX);
        arr[i].clawKeepoutMaxX=getValue("ClawSave",QString("clawKeepoutMaxX_%1").arg(i),defaultV[i].clawKeepoutMaxX);
        arr[i].clawKeepoutHighZ=getValue("ClawSave",QString("clawKeepoutHighZ_%1").arg(i),defaultV[i].clawKeepoutHighZ);
        arr[i].cKeepoutMinX=getValue("ClawSave",QString("cKeepoutMinX_%1").arg(i),defaultV[i].cKeepoutMinX);
        arr[i].cKeepoutMaxX=getValue("ClawSave",QString("cKeepoutMaxX_%1").arg(i),defaultV[i].cKeepoutMaxX);
        arr[i].cKeepoutHighZ=getValue("ClawSave",QString("cKeepoutHighZ_%1").arg(i),defaultV[i].cKeepoutHighZ);
        arr[i].cKeepoutPosMinC=getValue("ClawSave",QString("cKeepoutPosMinC_%1").arg(i),defaultV[i].cKeepoutPosMinC);
        arr[i].cKeepoutPosMaxC=getValue("ClawSave",QString("cKeepoutPosMaxC_%1").arg(i),defaultV[i].cKeepoutPosMaxC);
        arr[i].cKeepoutNegMinC=getValue("ClawSave",QString("cKeepoutNegMinC_%1").arg(i),defaultV[i].cKeepoutNegMinC);
        arr[i].cKeepoutNegMaxC=getValue("ClawSave",QString("cKeepoutNegMaxC_%1").arg(i),defaultV[i].cKeepoutNegMaxC);
    }

    return arr;
}
void setClawSave(D_ClawSaveStruct* value)
{
    for(int i=0;i<CLAW_SAVE_NUM;i++)
    {
        setValue("ClawSave",QString("clawKeepoutSta_%1").arg(i),value[i].clawKeepoutSta);
        setValue("ClawSave",QString("axisSelect_X_%1").arg(i),value[i].axisSelect_X);
        setValue("ClawSave",QString("axisSelect_Z_%1").arg(i),value[i].axisSelect_Z);
        setValue("ClawSave",QString("clawKeepoutMinX_%1").arg(i),value[i].clawKeepoutMinX);
        setValue("ClawSave",QString("clawKeepoutMaxX_%1").arg(i),value[i].clawKeepoutMaxX);
        setValue("ClawSave",QString("clawKeepoutHighZ_%1").arg(i),value[i].clawKeepoutHighZ);
        setValue("ClawSave",QString("cKeepoutMinX_%1").arg(i),value[i].cKeepoutMinX);
        setValue("ClawSave",QString("cKeepoutMaxX_%1").arg(i),value[i].cKeepoutMaxX);
        setValue("ClawSave",QString("cKeepoutHighZ_%1").arg(i),value[i].cKeepoutHighZ);
        setValue("ClawSave",QString("cKeepoutPosMinC_%1").arg(i),value[i].cKeepoutPosMinC);
        setValue("ClawSave",QString("cKeepoutPosMaxC_%1").arg(i),value[i].cKeepoutPosMaxC);
        setValue("ClawSave",QString("cKeepoutNegMinC_%1").arg(i),value[i].cKeepoutNegMinC);
        setValue("ClawSave",QString("cKeepoutNegMaxC_%1").arg(i),value[i].cKeepoutNegMaxC);
    }
}
//产品设置
D_ProductSetStruct getProductSet(D_ProductSetStruct defaultV)
{
    D_ProductSetStruct arr=defaultV;
    arr.waitProcessFinTime=getValue("ProductSet","waitProcessFinTime",defaultV.waitProcessFinTime);
    arr.pluseOutTime=getValue("ProductSet","pluseOutTime",defaultV.pluseOutTime);
    arr.airVlaveLimitTime=getValue("ProductSet","airVlaveLimitTime",defaultV.airVlaveLimitTime);
    arr.autoCycTime=getValue("ProductSet","autoCycTime",defaultV.autoCycTime);
    arr.alarmBuzzerTime=getValue("ProductSet","alarmBuzzerTime",defaultV.alarmBuzzerTime);
    arr.chainReachDelay=getValue("ProductSet","chainReachDelay",defaultV.chainReachDelay);
    arr.clampCheckFilterTime=getValue("ProductSet","clampCheckFilterTime",defaultV.clampCheckFilterTime);
    return arr;
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
D_ProductSeniorStruct getProductSenior(D_ProductSeniorStruct defaultV)
{
    D_ProductSeniorStruct arr=defaultV;
    arr.lubricOpenTime=getValue("ProductSenior","lubricOpenTime",defaultV.lubricOpenTime);
    arr.lubricCloseTime=getValue("ProductSenior","lubricCloseTime",defaultV.lubricCloseTime);
    arr.lubricDelay=getValue("ProductSenior","lubricDelay",defaultV.lubricDelay);
    arr.lubricGapCyc=getValue("ProductSenior","lubricGapCyc",defaultV.lubricGapCyc);
    arr.productMemoryFlag=getValue("ProductSenior","productMemoryFlag",defaultV.productMemoryFlag);
    return arr;
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
D_InternetStruct getInternet(D_InternetStruct defaultV)
{
    D_InternetStruct arr=defaultV;
    arr.netCommType=getValue("Internet","netCommType",defaultV.netCommType);
    arr.parityMode=getValue("Internet","parityMode",defaultV.parityMode);
    arr.baudMode=getValue("Internet","baudMode",defaultV.baudMode);
    return arr;
}
void setInternet(D_InternetStruct value)
{
    setValue("Internet","netCommType",value.netCommType);
    setValue("Internet","parityMode",value.parityMode);
    setValue("Internet","baudMode",value.baudMode);
}
//系统参数
D_SystemSetStruct getSystemSet(D_SystemSetStruct defaultV)
{
    D_SystemSetStruct arr=defaultV;
    arr.lan=getValue("SystemSet","lan",defaultV.lan);
    arr.typeFace=getValue("SystemSet","typeFace",defaultV.typeFace);
    arr.wordSize=getValue("SystemSet","wordSize",defaultV.wordSize);
    arr.keyListen=getValue("SystemSet","keyListen",defaultV.keyListen);
    arr.backlightTime=getValue("SystemSet","backlightTime",defaultV.backlightTime);
    arr.backlightBrightness=getValue("SystemSet","backlightBrightness",defaultV.backlightBrightness);
    arr.sysName=getValue("SystemSet","sysName",defaultV.sysName);
    arr.sysColor=getValue("SystemSet","sysColor",defaultV.sysColor);
    return arr;
}
void setSystemSet(D_SystemSetStruct value)
{
    setValue("SystemSet","lan",value.lan);
    setValue("SystemSet","typeFace",value.typeFace);
    setValue("SystemSet","wordSize",value.wordSize);
    setValue("SystemSet","keyListen",value.keyListen);
    setValue("SystemSet","backlightTime",value.backlightTime);
    setValue("SystemSet","backlightBrightness",value.backlightBrightness);
    setValue("SystemSet","sysName",value.sysName);
    setValue("SystemSet","sysColor",value.sysColor);
}
//轴运动参数
D_AxisParStruct* getAxisPar(D_AxisParStruct* defaultV)
{
    D_AxisParStruct* arr=defaultV;
    for(int i=0;i<AXIS_TOTAL_NUM;i++)
    {
        arr[i].axisType=getValue("AxisPar",QString("axisType_%1").arg(i),defaultV[i].axisType);
        arr[i].axisMoveMade=getValue("AxisPar",QString("axisMoveMade_%1").arg(i),defaultV[i].axisMoveMade);

        arr[i].accTime=getValue("AxisPar",QString("accTime_%1").arg(i),defaultV[i].accTime);
        arr[i].decTime=getValue("AxisPar",QString("decTime_%1").arg(i),defaultV[i].decTime);
        arr[i].accAcc=getValue("AxisPar",QString("accAcc_%1").arg(i),defaultV[i].accAcc);
        arr[i].decDec=getValue("AxisPar",QString("decDec_%1").arg(i),defaultV[i].decDec);
        arr[i].maxSpeed=getValue("AxisPar",QString("maxSpeed_%1").arg(i),defaultV[i].maxSpeed);
        arr[i].axisMinPos=getValue("AxisPar",QString("axisMinPos_%1").arg(i),defaultV[i].axisMinPos);
        arr[i].axisMaxPos=getValue("AxisPar",QString("axisMaxPos_%1").arg(i),defaultV[i].axisMaxPos);

        arr[i].circlePluseNum=getValue("AxisPar",QString("circlePluseNum_%1").arg(i),defaultV[i].circlePluseNum);
        arr[i].circleDis=getValue("AxisPar",QString("circleDis_%1").arg(i),defaultV[i].circleDis);

        arr[i].findOriginSpeed=getValue("AxisPar",QString("findOriginSpeed_%1").arg(i),defaultV[i].findOriginSpeed);
        arr[i].leaveOriginSpeed=getValue("AxisPar",QString("leaveOriginSpeed_%1").arg(i),defaultV[i].leaveOriginSpeed);
        arr[i].originOffset=getValue("AxisPar",QString("originOffset_%1").arg(i),defaultV[i].originOffset);
        arr[i].originDir=getValue("AxisPar",QString("originDir_%1").arg(i),defaultV[i].originDir);

        arr[i].backOriginOrder=getValue("AxisPar",QString("backOriginOrder_%1").arg(i),defaultV[i].backOriginOrder);
        arr[i].originType=getValue("AxisPar",QString("originType_%1").arg(i),defaultV[i].originType);

        arr[i].limitPosSwt=getValue("AxisPar",QString("limitPosSwt_%1").arg(i),defaultV[i].limitPosSwt);
        arr[i].limitNegSwt=getValue("AxisPar",QString("limitNegSwt_%1").arg(i),defaultV[i].limitNegSwt);
        arr[i].coordDir=getValue("AxisPar",QString("coordDir_%1").arg(i),defaultV[i].coordDir);
    }

    return arr;
}
void setAxisPar(D_AxisParStruct* value)
{
    for(int i=0;i<AXIS_TOTAL_NUM;i++)
    {
        setValue("AxisPar",QString("axisType_%1").arg(i),value[i].axisType);
        setValue("AxisPar",QString("axisMoveMade_%1").arg(i),value[i].axisMoveMade);

        setValue("AxisPar",QString("accTime_%1").arg(i),value[i].accTime);
        setValue("AxisPar",QString("decTime_%1").arg(i),value[i].decTime);
        setValue("AxisPar",QString("accAcc_%1").arg(i),value[i].accAcc);
        setValue("AxisPar",QString("decDec_%1").arg(i),value[i].decDec);
        setValue("AxisPar",QString("maxSpeed_%1").arg(i),value[i].maxSpeed);
        setValue("AxisPar",QString("axisMinPos_%1").arg(i),value[i].axisMinPos);
        setValue("AxisPar",QString("axisMaxPos_%1").arg(i),value[i].axisMaxPos);

        setValue("AxisPar",QString("circlePluseNum_%1").arg(i),value[i].circlePluseNum);
        setValue("AxisPar",QString("circleDis_%1").arg(i),value[i].circleDis);

        setValue("AxisPar",QString("findOriginSpeed_%1").arg(i),value[i].findOriginSpeed);
        setValue("AxisPar",QString("leaveOriginSpeed_%1").arg(i),value[i].leaveOriginSpeed);
        setValue("AxisPar",QString("originOffset_%1").arg(i),value[i].originOffset);
        setValue("AxisPar",QString("originDir_%1").arg(i),value[i].originDir);

        setValue("AxisPar",QString("backOriginOrder_%1").arg(i),value[i].backOriginOrder);
        setValue("AxisPar",QString("originType_%1").arg(i),value[i].originType);

        setValue("AxisPar",QString("limitPosSwt_%1").arg(i),value[i].limitPosSwt);
        setValue("AxisPar",QString("limitNegSwt_%1").arg(i),value[i].limitNegSwt);
        setValue("AxisPar",QString("coordDir_%1").arg(i),value[i].coordDir);
    }
}
//首轮运动参数
D_AxisParStruct getHandwheelPar(D_AxisParStruct defaultV)
{
    D_AxisParStruct arr=defaultV;
    arr.axisType=getValue("HandwheelPar","axisType",defaultV.axisType);
    arr.axisMoveMade=getValue("HandwheelPar","axisMoveMade",defaultV.axisMoveMade);

    arr.accTime=getValue("HandwheelPar","accTime",defaultV.accTime);
    arr.decTime=getValue("HandwheelPar","decTime",defaultV.decTime);
    arr.accAcc=getValue("HandwheelPar","accAcc",defaultV.accAcc);
    arr.decDec=getValue("HandwheelPar","decDec",defaultV.decDec);
    arr.maxSpeed=getValue("HandwheelPar","maxSpeed",defaultV.maxSpeed);
    arr.axisMinPos=getValue("HandwheelPar","axisMinPos",defaultV.axisMinPos);
    arr.axisMaxPos=getValue("HandwheelPar","axisMaxPos",defaultV.axisMaxPos);

    arr.circlePluseNum=getValue("HandwheelPar","circlePluseNum",defaultV.circlePluseNum);
    arr.circleDis=getValue("HandwheelPar","circleDis",defaultV.circleDis);

    arr.findOriginSpeed=getValue("HandwheelPar","findOriginSpeed",defaultV.findOriginSpeed);
    arr.leaveOriginSpeed=getValue("HandwheelPar","leaveOriginSpeed",defaultV.leaveOriginSpeed);
    arr.originOffset=getValue("HandwheelPar","originOffset",defaultV.originOffset);
    arr.originDir=getValue("HandwheelPar","originDir",defaultV.originDir);

    arr.backOriginOrder=getValue("HandwheelPar","backOriginOrder",defaultV.backOriginOrder);
    arr.originType=getValue("HandwheelPar","originType",defaultV.originType);

    arr.limitPosSwt=getValue("HandwheelPar","limitPosSwt",defaultV.limitPosSwt);
    arr.limitNegSwt=getValue("HandwheelPar","limitNegSwt",defaultV.limitNegSwt);
    arr.coordDir=getValue("HandwheelPar","coordDir",defaultV.coordDir);

    return arr;
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
D_ServoParStruct getServoPar(D_ServoParStruct defaultV)
{
    D_ServoParStruct arr=defaultV;
    arr.encoderType=getValue("ServoPar","encoderType",defaultV.encoderType);
    arr.encoderBitNum=getValue("ServoPar","encoderBitNum",defaultV.encoderBitNum);
    arr.servoBrand=getValue("ServoPar","servoBrand",defaultV.servoBrand);
    arr.servoModel=getValue("ServoPar","servoModel",defaultV.servoModel);
    arr.extendIOBoard=getValue("ServoPar","extendIOBoard",defaultV.extendIOBoard);
    arr.rotateAxisUnit=getValue("ServoPar","rotateAxisUnit",defaultV.rotateAxisUnit);
    arr.resetSunProFlag=getValue("ServoPar","resetSunProFlag",defaultV.resetSunProFlag);
    arr.torqueLimitFlag=getValue("ServoPar","torqueLimitFlag",defaultV.torqueLimitFlag);

    return arr;
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
D_DegreeParStruct getDegreeParS(D_DegreeParStruct defaultV)
{
    D_DegreeParStruct arr=defaultV;
    arr.tolerance=getValue("DegreeParS","tolerance",defaultV.tolerance);
    arr.originDegree=getValue("DegreeParS","originDegree",defaultV.originDegree);
    arr.actionFinDegree=getValue("DegreeParS","actionFinDegree",defaultV.actionFinDegree);
    arr.saveRampageHige_Z1=getValue("DegreeParS","saveRampageHige_Z1",defaultV.saveRampageHige_Z1);
    arr.saveRampageHige_Z2=getValue("DegreeParS","saveRampageHige_Z2",defaultV.saveRampageHige_Z2);
    arr.saveRotatePos_Y1=getValue("DegreeParS","saveRotatePos_Y1",defaultV.saveRotatePos_Y1);
    arr.saveRotatePos_Y2=getValue("DegreeParS","saveRotatePos_Y2",defaultV.saveRotatePos_Y2);

    return arr;
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
D_SaveAreaStruct* getSaveArea(D_SaveAreaStruct* defaultV)
{
    D_SaveAreaStruct* arr=defaultV;
    for(int i=0;i<SAVE_AREA_NUM;i++)
    {
        arr[i].saveAreaSwt=getValue("SaveArea",QString("saveAreaSwt_%1").arg(i),defaultV[i].saveAreaSwt);
        for(int j=0;j<SAVE_AXIS_NUM;j++)
        {
            arr[i].axisSelect[j]=getValue("SaveArea",QString("axisSelect_%1_%2").arg(i).arg(j),defaultV[i].axisSelect[j]);
            arr[i].machineA1Pos[j]=getValue("SaveArea",QString("machineA1Pos_%1_%2").arg(i).arg(j),defaultV[i].machineA1Pos[j]);
            arr[i].machineA2Pos[j]=getValue("SaveArea",QString("machineA2Pos_%1_%2").arg(i).arg(j),defaultV[i].machineA2Pos[j]);
            arr[i].starckB1Pos[j]=getValue("SaveArea",QString("starckB1Pos_%1_%2").arg(i).arg(j),defaultV[i].starckB1Pos[j]);
            arr[i].starckB2Pos[j]=getValue("SaveArea",QString("starckB2Pos_%1_%2").arg(i).arg(j),defaultV[i].starckB2Pos[j]);
        }
        arr[i].machine_Z_WaitMaxPos=getValue("SaveArea",QString("machine_Z_WaitMaxPos_%1").arg(i),defaultV[i].machine_Z_WaitMaxPos);
        arr[i].machine_Z_FallMaxPos=getValue("SaveArea",QString("machine_Z_FallMaxPos_%1").arg(i),defaultV[i].machine_Z_FallMaxPos);
        arr[i].machine_Z_InsideHigh=getValue("SaveArea",QString("machine_Z_InsideHigh_%1").arg(i),defaultV[i].machine_Z_InsideHigh);
        arr[i].stack_Z_StartMaxPos=getValue("SaveArea",QString("stack_Z_StartMaxPos_%1").arg(i),defaultV[i].stack_Z_StartMaxPos);
        arr[i].stack_Z_FallMaxPos=getValue("SaveArea",QString("stack_Z_FallMaxPos_%1").arg(i),defaultV[i].stack_Z_FallMaxPos);
    }

    return arr;
}
void setSaveArea(D_SaveAreaStruct* value)
{
    for(int i=0;i<SAVE_AREA_NUM;i++)
    {
        setValue("SaveArea",QString("saveAreaSwt_%1").arg(i),value[i].saveAreaSwt);
        for(int j=0;j<SAVE_AXIS_NUM;j++)
        {
            setValue("SaveArea",QString("axisSelect_%1_%2").arg(i).arg(j),value[i].axisSelect[j]);
            setValue("SaveArea",QString("machineA1Pos_%1_%2").arg(i).arg(j),value[i].machineA1Pos[j]);
            setValue("SaveArea",QString("machineA2Pos_%1_%2").arg(i).arg(j),value[i].machineA2Pos[j]);
            setValue("SaveArea",QString("starckB1Pos_%1_%2").arg(i).arg(j),value[i].starckB1Pos[j]);
            setValue("SaveArea",QString("starckB2Pos_%1_%2").arg(i).arg(j),value[i].starckB2Pos[j]);
        }
        setValue("SaveArea",QString("machine_Z_WaitMaxPos_%1").arg(i),value[i].machine_Z_WaitMaxPos);
        setValue("SaveArea",QString("machine_Z_FallMaxPos_%1").arg(i),value[i].machine_Z_FallMaxPos);
        setValue("SaveArea",QString("machine_Z_InsideHigh_%1").arg(i),value[i].machine_Z_InsideHigh);
        setValue("SaveArea",QString("stack_Z_StartMaxPos_%1").arg(i),value[i].stack_Z_StartMaxPos);
        setValue("SaveArea",QString("stack_Z_FallMaxPos_%1").arg(i),value[i].stack_Z_FallMaxPos);
    }
}
//伺服通信协议参数
D_ServoCommParSetStruct* getServoCommPar(D_ServoCommParSetStruct* defaultV)
{
    D_ServoCommParSetStruct* arr=defaultV;
    for(int i=0;i<SERVE_COMM_PAR_NUM;i++)
    {
        arr[i].saveFlag=getValue("ServoCommPar",QString("saveFlag_%1").arg(i),defaultV[i].saveFlag);
        arr[i].powerOnSet=getValue("ServoCommPar",QString("powerOnSet_%1").arg(i),defaultV[i].powerOnSet);
        arr[i].axis=getValue("ServoCommPar",QString("axis_%1").arg(i),defaultV[i].axis);
        arr[i].index=getValue("ServoCommPar",QString("index_%1").arg(i),defaultV[i].index);
        arr[i].sunIndex=getValue("ServoCommPar",QString("sunIndex_%1").arg(i),defaultV[i].sunIndex);
        arr[i].parLen=getValue("ServoCommPar",QString("parLen_%1").arg(i),defaultV[i].parLen);
        arr[i].parValue=getValue("ServoCommPar",QString("parValue_%1").arg(i),defaultV[i].parValue);
    }
    return arr;
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
D_StackInfoStruct* getStackInfo(D_StackInfoStruct* defaultV)
{
    D_StackInfoStruct* arr=defaultV;
    for(int i=0;i<STACK_TOTAL_NUM;i++)
    {
        arr[i].stackOrder=getValue("StackInfo",QString("stackOrder_%1").arg(i),defaultV[i].stackOrder);
        arr[i].countMode=getValue("StackInfo",QString("countMode_%1").arg(i),defaultV[i].countMode);
        arr[i].stackFinStopFlag=getValue("StackInfo",QString("stackFinStopFlag_%1").arg(i),defaultV[i].stackFinStopFlag);
        arr[i].groupLeaveBlank=getValue("StackInfo",QString("groupLeaveBlank_%1").arg(i),defaultV[i].groupLeaveBlank);
        arr[i].stackMoveMode=getValue("StackInfo",QString("stackMoveMode_%1").arg(i),defaultV[i].stackMoveMode);
        for(int j=0;j<STACK_AXIS_NUM;j++)
        {
            arr[i].axisSelect[j]=getValue("StackInfo",QString("axisSelect_%1_%2").arg(i).arg(j),defaultV[i].axisSelect[j]);
            arr[i].axisSpeed[j]=getValue("StackInfo",QString("axisSpeed_%1_%2").arg(i).arg(j),defaultV[i].axisSpeed[j]);
            arr[i].stackPointNum[j]=getValue("StackInfo",QString("stackPointNum_%1_%2").arg(i).arg(j),defaultV[i].stackPointNum[j]);
            arr[i].stackDir[j]=getValue("StackInfo",QString("stackDir_%1_%2").arg(i).arg(j),defaultV[i].stackDir[j]);
            arr[i].dischangeSpeed[j]=getValue("StackInfo",QString("dischangeSpeed_%1_%2").arg(i).arg(j),defaultV[i].dischangeSpeed[j]);

            arr[i].stackStartPos[j]=getValue("StackInfo",QString("stackStartPos_%1_%2").arg(i).arg(j),defaultV[i].stackStartPos[j]);
            arr[i].stack_X_EndPos[j]=getValue("StackInfo",QString("stack_X_EndPos_%1_%2").arg(i).arg(j),defaultV[i].stack_X_EndPos[j]);
            arr[i].stack_Y_EndPos[j]=getValue("StackInfo",QString("stack_Y_EndPos_%1_%2").arg(i).arg(j),defaultV[i].stack_Y_EndPos[j]);
            arr[i].stackDiagonalPos[j]=getValue("StackInfo",QString("stackDiagonalPos_%1_%2").arg(i).arg(j),defaultV[i].stackDiagonalPos[j]);

            arr[i].dischangeStartPos[j]=getValue("StackInfo",QString("dischangeStartPos_%1_%2").arg(i).arg(j),defaultV[i].dischangeStartPos[j]);
            arr[i].intevalDis[j]=getValue("StackInfo",QString("intevalDis_%1_%2").arg(i).arg(j),defaultV[i].intevalDis[j]);
            arr[i].offsetDis[j]=getValue("StackInfo",QString("offsetDis_%1_%2").arg(i).arg(j),defaultV[i].offsetDis[j]);
        }
    }
    return arr;
}
void setStackInfo(D_StackInfoStruct* value)
{
    for(int i=0;i<STACK_TOTAL_NUM;i++)
    {
        setValue("StackInfo",QString("stackOrder_%1").arg(i),value[i].stackOrder);
        setValue("StackInfo",QString("countMode_%1").arg(i),value[i].countMode);
        setValue("StackInfo",QString("stackFinStopFlag_%1").arg(i),value[i].stackFinStopFlag);
        setValue("StackInfo",QString("groupLeaveBlank_%1").arg(i),value[i].groupLeaveBlank);
        setValue("StackInfo",QString("stackMoveMode_%1").arg(i),value[i].stackMoveMode);
        for(int j=0;j<STACK_AXIS_NUM;j++)
        {
            setValue("StackInfo",QString("axisSelect_%1_%2").arg(i).arg(j),value[i].axisSelect[j]);
            setValue("StackInfo",QString("axisSpeed_%1_%2").arg(i).arg(j),value[i].axisSpeed[j]);
            setValue("StackInfo",QString("stackPointNum_%1_%2").arg(i).arg(j),value[i].stackPointNum[j]);
            setValue("StackInfo",QString("stackDir_%1_%2").arg(i).arg(j),value[i].stackDir[j]);
            setValue("StackInfo",QString("dischangeSpeed_%1_%2").arg(i).arg(j),value[i].dischangeSpeed[j]);

            setValue("StackInfo",QString("stackStartPos_%1_%2").arg(i).arg(j),value[i].stackStartPos[j]);
            setValue("StackInfo",QString("stack_X_EndPos_%1_%2").arg(i).arg(j),value[i].stack_X_EndPos[j]);
            setValue("StackInfo",QString("stack_Y_EndPos_%1_%2").arg(i).arg(j),value[i].stack_Y_EndPos[j]);
            setValue("StackInfo",QString("stackDiagonalPos_%1_%2").arg(i).arg(j),value[i].stackDiagonalPos[j]);

            setValue("StackInfo",QString("dischangeStartPos_%1_%2").arg(i).arg(j),value[i].dischangeStartPos[j]);
            setValue("StackInfo",QString("intevalDis_%1_%2").arg(i).arg(j),value[i].intevalDis[j]);
            setValue("StackInfo",QString("offsetDis_%1_%2").arg(i).arg(j),value[i].offsetDis[j]);
        }
    }
}
//堆叠功能参数
D_StackFuncStruct getStackFunc(D_StackFuncStruct defaultV)
{
    D_StackFuncStruct arr=defaultV;
    arr.stackType=getValue("StackFunc","stackType",defaultV.stackType);
    arr.groupStack=getValue("StackFunc","groupStack",defaultV.groupStack);
    arr.rowStack=getValue("StackFunc","rowStack",defaultV.rowStack);
    arr.storeyStack=getValue("StackFunc","storeyStack",defaultV.storeyStack);
    arr.divideStack=getValue("StackFunc","divideStack",defaultV.divideStack);
    arr.stackMemory=getValue("StackFunc","stackMemory",defaultV.stackMemory);
    arr.siloType=getValue("StackFunc","siloType",defaultV.siloType);
    arr.rotateSiloFinHint=getValue("StackFunc","rotateSiloFinHint",defaultV.rotateSiloFinHint);
    arr.rotateSiloPlaceNum=getValue("StackFunc","rotateSiloPlaceNum",defaultV.rotateSiloPlaceNum);

    return arr;
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

//程序文件存储
QList<D_ProgramNameAndPathStruct> getProgramNameAndPath()
{
    QList<D_ProgramNameAndPathStruct> namePathTempList;
    QStringList str_name=getValue("File","name","").split(";");
    QStringList str_path=getValue("File","path","").split(";");
    QStringList str_index=getValue("File","index","").split(";");
    QStringList str_time=getValue("File","time","").split(";");
    if(str_name.count()==1&&str_name[0]=="")
        return namePathTempList;
    for(int i=0;i<str_name.count();i++)
    {
        D_ProgramNameAndPathStruct P_NamePathTemp;
        P_NamePathTemp.fileName=str_name[i];
        P_NamePathTemp.filePath=str_path[i];
        P_NamePathTemp.index=str_index[i].toUInt();
        P_NamePathTemp.changeTime=str_time[i];
        namePathTempList.append(P_NamePathTemp);
    }
    return namePathTempList;
}
void setProgramNameAndPath(QList<D_ProgramNameAndPathStruct> value)
{
    QString str_name;
    QString str_path;
    QString str_index;
    QString str_time;
    for(int i=0;i<value.count();i++)
    {
        if(i==0)
        {
            str_name+=value[i].fileName;
            str_path+=value[i].filePath;
            str_index+=QString::number(value[i].index);
            str_time+=value[i].changeTime;
        }
        else
        {
            str_name=str_name+";"+value[i].fileName;
            str_path=str_path+";"+value[i].filePath;
            str_index=str_index+";"+QString::number(value[i].index);
            str_time=str_time+";"+value[i].changeTime;
        }
    }
    setValue("File","name",str_name);
    setValue("File","path",str_path);
    setValue("File","index",str_index);
    setValue("File","time",str_time);
}


/*****************命令相关参数保存*****************/
QString m_configCmdPath = "/root/Cmd_Description.txt";
QString m_configPortSettingPath = "/root/Port_Setting.txt";
QString m_configOrderjinitPath = "/root/Pro_Teach.txt";

QString m_configPortXYNamePath="/root/Port_XY_Name_CHS.txt";                //输入输出、名称定义（3）

QString m_configPortXYNameIniPath="/root/Ini_Port_XY_Name.txt";             //输入输出、名称定义配置文件（99）

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












