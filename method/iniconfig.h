#ifndef INICONFIG_H
#define INICONFIG_H
#include "QString"
#include "QSettings"
#include "cmd.h"


extern QString m_configFileNamePath;

QString getValue(QString prefix, QString key, QString defaultV);
void   setValue(QString prefix, QString key, QString value);

uint8_t getValue(QString prefix, QString key, uint8_t defaultV);
void   setValue(QString prefix, QString key, uint8_t value);

uint16_t getValue(QString prefix, QString key, uint16_t defaultV);
void   setValue(QString prefix, QString key, uint16_t value);

uint32_t getValue(QString prefix, QString key, uint32_t defaultV);
void   setValue(QString prefix, QString key, uint32_t value);

int32_t getValue(QString prefix, QString key, int32_t defaultV);
void   setValue(QString prefix, QString key, int32_t value);


/*****************相关参数保存*****************/
//输出类型
extern uint8_t* getOutPortType(uint8_t* defaultV);
extern void setOutPortType(uint8_t* value);
//互锁设置
extern uint8_t** getOutportInterlock(uint8_t** defaultV);
extern void setOutportInterlock(uint8_t** value);
//端口自定义
extern uint8_t* getInportFuncDefine(uint8_t* defaultV);
extern void setInportFuncDefine(uint8_t* value);
extern uint8_t* getOutportFuncDefine(uint8_t* defaultV);
extern void setOutportFuncDefine(uint8_t* value);
//预留关联
extern uint8_t* getOutportRelevancy(uint8_t* defaultV);
extern void setOutportRelevancy(uint8_t* value);
//预留出类型
extern uint8_t* getReleteOut(uint8_t* defaultV);
extern void setReleteOut(uint8_t* value);
//按键功能
extern D_KeyFuncStruct* getKeyFunc(D_KeyFuncStruct* defaultV);
extern void setKeyFunc(D_KeyFuncStruct* value);
//LED功能
extern D_LedFuncStruct* getLedFunc(D_LedFuncStruct* defaultV);
extern void setLedFunc(D_LedFuncStruct* value);
//信号设置高级功能
extern D_SeniorFuncStruct getSeniorFunc(D_SeniorFuncStruct defaultV);
extern void setSeniorFunc(D_SeniorFuncStruct value);
//机床安全
extern D_MachineSaveStruct getMachineSave(D_MachineSaveStruct defaultV);
extern void setMachineSave(D_MachineSaveStruct value);
//料仓安全
extern D_StackSaveStruct getStackSave(D_StackSaveStruct defaultV);
extern void setStackSave(D_StackSaveStruct value);
//卡爪安全
extern D_ClawSaveStruct* getClawSave(D_ClawSaveStruct* defaultV);
extern void setClawSave(D_ClawSaveStruct* value);
//产品设置
extern D_ProductSetStruct getProductSet(D_ProductSetStruct defaultV);
extern void setProductSet(D_ProductSetStruct value);
//产品设置高级功能
extern D_ProductSeniorStruct getProductSenior(D_ProductSeniorStruct defaultV);
extern void setProductSenior(D_ProductSeniorStruct value);
//物联网
extern D_InternetStruct getInternet(D_InternetStruct defaultV);
extern void setInternet(D_InternetStruct value);
//系统参数
extern D_SystemSetStruct getSystemSet(D_SystemSetStruct defaultV);
extern void setSystemSet(D_SystemSetStruct value);
//轴运动参数
extern D_AxisParStruct* getAxisPar(D_AxisParStruct* defaultV);
extern void setAxisPar(D_AxisParStruct* value);
//首轮运动参数
extern D_AxisParStruct getHandwheelPar(D_AxisParStruct defaultV);
extern void setHandwheelPar(D_AxisParStruct value);
//伺服结构参数
extern D_ServoParStruct getServoPar(D_ServoParStruct defaultV);
extern void setServoPar(D_ServoParStruct value);
//伺服安全及控制精度参数
extern D_DegreeParStruct getDegreeParS(D_DegreeParStruct defaultV);
extern void setDegreeParS(D_DegreeParStruct value);
//安全区参数
extern D_SaveAreaStruct* getSaveArea(D_SaveAreaStruct* defaultV);
extern void setSaveArea(D_SaveAreaStruct* value);
//伺服通信协议参数
extern D_ServoCommParSetStruct* getServoCommPar(D_ServoCommParSetStruct* defaultV);
extern void setServoCommPar(D_ServoCommParSetStruct* value);
//堆叠信息参数
extern D_StackInfoStruct* getStackInfo(D_StackInfoStruct* defaultV);
extern void setStackInfo(D_StackInfoStruct* value);
//堆叠功能参数
extern D_StackFuncStruct getStackFunc(D_StackFuncStruct defaultV);
extern void setStackFunc(D_StackFuncStruct value);

//程序文件存储
extern QList<D_ProgramNameAndPathStruct> getProgramNameAndPath();
extern void setProgramNameAndPath(QList<D_ProgramNameAndPathStruct> value);


/*****************命令相关参数保存*****************/
extern QStringList fileSectionList;                        //程序保存节点
extern QStringList programInfoList;                        //程序信息
extern QStringList referencePointList;                     //每个参考点表示
extern QStringList programBasicCmdList;                    //基本命令

extern QStringList axisMoveCmdList;                         //轴动作命令
extern QStringList clawActionCmdList;                         //卡爪动作命令
extern QStringList clawCheckCmdList;                         //信号检测-卡爪检测命令
extern QStringList reserveCheckCmdList;                         //信号检测-预留检测命令
extern QStringList machineOutCmdList;                         //机床-输出动作命令
extern QStringList stackResetZeroCmdList;                         //堆叠-堆叠清零命令
extern QStringList stackMoveCmdList;                         //堆叠-堆叠组运动命令
extern QStringList stackFollowCmdList;                         //堆叠-放料跟随运动命令
extern QStringList reserveOutCmdList;                         //预留输出动作命令

extern QStringList waitInMachineCmdList;                         //信号等待-等待机床命令
extern QStringList waitInClawCmdList;                         //信号等待-等待卡爪命令
extern QStringList waitInReserveCmdList;                         //信号等待-等待预留命令
extern QStringList otherAlarmCustCmdList;                         //其他-报警自定义命令
extern QStringList otherCycStopCmdList;                         //其他-周期停止命令
extern QStringList labelCmdList;                         //标签命令

extern QStringList logicIfCmdList;                         //逻辑&变量-如果-如果命令
extern QStringList logicWhileStartCmdList;                         //逻辑&变量-循环开始命令
extern QStringList logicVarCmdList;                         //逻辑&变量-变量-变量命令
extern QStringList logicStackCmdList;                         //逻辑&变量-变量-堆叠命令
extern QStringList logicTimeCmdList;                         //逻辑&变量-定时器命令
extern QStringList searchAxisMoveCmdList;                         //搜索-轴搜索运动命令

extern QStringList offsetAxisCmdList;                         //偏移-轴偏移运动命令
extern QStringList torqueGardCmdList;                         //扭矩保护命令
extern QStringList axisStopCmdList;                         //伺服停止命令
extern QStringList sunProCmdList;                         //子程序命令

extern QString m_configCmdPath;                             //命令的配置文件路径
extern QString m_configPortSettingPath;                     //端口号设置的配置文件路径
extern QStringList getIniValues(uint8_t type, QString node);                  //获取某个节点下所有键对应的值（键进行排序后的值）

#endif // INICONFIG_H
