#ifndef PROGRAM_SAVE_H
#define PROGRAM_SAVE_H
#include "QString"

#include "program_popup.h"
#include "QDir"
#include <QTextStream>
#include "iniconfig.h"
#include <QDateTime>


extern QString signalSpace;                            //单个空格
extern QString doubleSpace;                            //两个空格
extern QString lineFeed;                               //换行符

extern QString m_ProgramPath;                   //存放程序文件的文件夹


extern QStringList P_List[9];                              //记录当前操作程序9个进程程序指令行数
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
extern QStringList logicAxisCmdList;                        //逻辑&变量-变量-轴命令
extern QStringList logicStackCmdList;                         //逻辑&变量-变量-堆叠命令
extern QStringList logicCurProductNumCmdList;                         //逻辑&变量-变量-实际产量命令结构体
extern QStringList logicTimeCmdList;                         //逻辑&变量-定时器命令
extern QStringList searchAxisMoveCmdList;                         //搜索-轴搜索运动命令

extern QStringList offsetAxisCmdList;                         //偏移-轴偏移运动命令
extern QStringList torqueGardCmdList;                         //扭矩保护命令
extern QStringList axisStopCmdList;                         //伺服停止命令
extern QStringList sunProCmdList;                         //子程序命令




void getCmdIni1();
uint8_t newBuildProgram(const QString& fileName);
bool writeBasicProgram(D_ProgramNameAndPathStruct pro_temp);

bool saveProgram(D_ProgramNameAndPathStruct pro_temp);
bool readProgram(D_ProgramNameAndPathStruct pro_temp);


#endif // PROGRAM_SAVE_H
