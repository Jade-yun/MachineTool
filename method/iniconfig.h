#ifndef INICONFIG_H
#define INICONFIG_H
#include "QString"
#include "QSettings"
#include "cmd.h"
#include "port_setting.h"


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

// 保存密码
/**
 * @brief savePasswdToConfig
 * @param authority 0-为管理员权限键值后缀 1-超级管理员权限键值后缀 2-菜单权限键值后缀
 * @param value 密码
 */
void savePasswdToConfig(int authority, uint32_t value);
void readPasswdFromConfig();

// 保存读取按键编辑框的字符用于ui显示
void writeKeySetStrToConfig(int index, const QString& text);
void readKeySetStrFromConfig(std::vector<QString> &keyStrs);
// 信号LED
void writeSigSetStrToConfig(int index, const QString& text);
void readSigSetStrFromConfig(std::vector<QString> &sigStrs);

// 保存读取联机安全区域输入输出编辑框的字符
void writeOnlineSafeInOutDescription(int group, const std::vector<QString> &describeStrs);
void readOnlineSafeInOutDescription(int group, std::vector<QString> &describeStrs);

// 限位 index 0～2 分别对应最小限位、最大限位、原点信号
void writeLimitSigDescription(int index, const std::vector<QString> &describeStrs);
void readLimitSigDescription(int index, std::vector<QString> &describeStrs);

/**
 * @brief writeGuideInfo 存储操作指引参数到配置文件
 */
void writeGuideInfo();
void readGuideInfo();

/*****************相关参数保存*****************/
//输出类型
extern void getOutPortType(uint8_t defaultV = 0);
extern void setOutPortType(uint8_t value[OUT_PORT_TYPE_NUM][2]);
//互锁设置
extern void getOutportInterlock(uint8_t defaultV = 0);
extern void setOutportInterlock(uint8_t value[OUT_INTERLOCK_NUM][4]);
//端口自定义
extern void setPortDefineNameOrPortNum();
//预留关联
extern void getOutportRelevancy(uint8_t defaultV = 0);
extern void setOutportRelevancy(uint8_t value[OUTPUT_TOTAL_NUM][3]);
//预留出类型
extern void getReleteOut(uint8_t defaultV = 0);
extern void setReleteOut(uint8_t value[OUT_PORT_RELEOUT_NUM][2]);
//按键功能
extern void getKeyFunc(uint8_t defaultV = 0);
extern void setKeyFunc(D_KeyFuncStruct* value);
//LED功能
extern void getLedFunc(uint8_t defaultV = 0);
extern void setLedFunc(D_LedFuncStruct* value);
//信号设置高级功能
extern void getSeniorFunc(uint8_t defaultV = 0);
extern void setSeniorFunc(D_SeniorFuncStruct value);
//机床安全
extern void getMachineSave(uint8_t defaultV = 0);
extern void setMachineSave(D_MachineSaveStruct value);
//料仓安全
extern void getStackSave(uint8_t defaultV = 0);
extern void setStackSave(D_StackSaveStruct value);
//卡爪安全
extern void getClawSave(uint8_t defaultV = 0);
extern void setClawSave(D_ClawSaveStruct value, uint8_t index);
//联机安全
extern void getOnlineSafe(uint8_t defaultV = 0);
extern void setOnlineSafe(D_OnlineSaveStruct value, uint8_t index);
//产品设置
extern void getProductSet(uint16_t defaultV = 0);
extern void setProductSet(D_ProductSetStruct value);
//产品设置高级功能
extern void getProductSenior(uint8_t defaultV = 0);
extern void setProductSenior(D_ProductSeniorStruct value);
//物联网
extern void getInternet(uint8_t defaultV = 0);
extern void setInternet(D_InternetStruct value);
//系统参数
extern void getSystemSet();
extern void setSystemSet(D_SystemSetStruct value);
//轴运动参数
extern void getAxisPar(uint8_t defaultV = 0);
extern void setAxisPar(D_AxisParStruct value, uint8_t index);
//首轮运动参数
extern void getHandwheelPar(uint8_t defaultV = 0);
extern void setHandwheelPar(D_AxisParStruct value);
//伺服结构参数
extern void getServoPar(uint8_t defaultV = 0);
extern void setServoPar(D_ServoParStruct value);
//伺服安全及控制精度参数
extern void getDegreeParS(uint8_t defaultV = 0);
extern void setDegreeParS(D_DegreeParStruct value);
//安全区参数
extern void getSaveArea(uint8_t defaultV = 0);
extern void setSaveArea(D_SaveAreaStruct value, uint8_t index);
//伺服通信协议参数
extern void getServoCommPar(uint8_t defaultV = 0);
extern void setServoCommPar(D_ServoCommParSetStruct* value);
//堆叠信息参数
extern void getStackInfo(uint8_t defaultV = 0);
extern void setStackInfo(D_StackInfoStruct value, uint8_t index);
//堆叠功能参数
extern void getStackFunc(uint8_t defaultV = 0);
extern void setStackFunc(D_StackFuncStruct value);
//手动轴控制参数
extern void getManualAxis(uint8_t defaultV = 0);
extern void setManualAxis(D_ManualAxis value);

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
extern QString m_configOrderjinitPath;                      //程序指令内容拼接文件路径
extern QStringList getIniValues(uint8_t type, QString node);                  //获取某个节点下所有键对应的值（键进行排序后的值）

#endif // INICONFIG_H
