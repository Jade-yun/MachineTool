#ifndef ORDERJOINT_H
#define ORDERJOINT_H
#include "stdint.h"
#include "./method/cmd.h"

extern uint16_t CheckJumpLabelLedal(uint16_t index);
extern void ClearListLabel();
extern void getOrderjoinIni();  //获取命令的内容拼接文件信息，用来拼接显示指令内容
extern uint16_t getLabelOrderIndex();
extern uint16_t ReturnLableListIndex(uint16_t lableNum);
extern uint16_t ReturnLabelnum(QString LabelText);
extern void CodeAndContInit(void);
extern void LeaveBlankSpace(uint8_t size, uint8_t *value);
extern QString JointStrDeal(P_ProOrderStruct OIS);
extern QString AxisActJoint(P_ProOrderStruct OIS);
extern QString ClawActionJoint(P_ProOrderStruct OIS);
extern QString ClawCheckJoint(P_ProOrderStruct OIS);
extern QString ReserveCheckJoint(P_ProOrderStruct OIS);
extern QString MachineOutJoint(P_ProOrderStruct OIS);
extern QString StackResetZeroJoint(P_ProOrderStruct OIS);
extern QString StackMoveJoint(P_ProOrderStruct OIS);
extern QString StackFollowJoint(P_ProOrderStruct OIS);
extern QString ReserveOutJoint(P_ProOrderStruct OIS);
extern QString ProEndJoint(P_ProOrderStruct OIS);
extern QString JointRunOrderNum(P_ProOrderStruct OIS);
extern QString WaitInMachineJoint(P_ProOrderStruct OIS);
extern QString WaitInClawJoint(P_ProOrderStruct OIS);
extern QString WaitInReserveJoint(P_ProOrderStruct OIS);
extern QString OtherDelayJoint(P_ProOrderStruct OIS);
extern QString OtherAlarmCustJoint(P_ProOrderStruct OIS);
extern QString OtherCycStopJoint(P_ProOrderStruct OIS);
extern QString OtherAlarmLampJoint(P_ProOrderStruct OIS);
extern QString OtherAlarmSoundJoint(P_ProOrderStruct OIS);
extern QString SunProJoint(P_ProOrderStruct OIS);
extern QString AxisStopJoint(P_ProOrderStruct OIS);
extern QString TorqueGardJoint(P_ProOrderStruct OIS);
extern QString OffsetAxisJoint(P_ProOrderStruct OIS);
extern QString SearchAxisJoint(P_ProOrderStruct OIS);
extern QString LogicIfJoint(P_ProOrderStruct OIS);
extern QString GetLogicIfStr(uint16_t IfIndex,P_LogicIfStruct* LogicIf);
extern QString LogicElseJoint(void);
extern QString LogicEndJoint(void);
extern QString LogicWhileStartJoint(P_ProOrderStruct OIS);
extern QString LogicWhileEndJoint(void);
extern QString GetLogicVarStr(P_ProOrderStruct OIS);
extern QString LogicVarStructJoint(P_ProOrderStruct OIS);
extern QString LogicAxisJoint(P_ProOrderStruct OIS);
extern QString LogicStackJoint(P_ProOrderStruct OIS);
extern QString LogicCurProductNumJoint(P_ProOrderStruct OIS);
extern QString LogicTimeJoint(P_ProOrderStruct OIS);
extern QString Get_XYPort_Name(P_ProOrderStruct OIS);
extern QString LabelJoint(P_ProOrderStruct OIS);

extern QStringList axisMoveOrderjointList;                         //轴动作命令
extern QStringList clawActionOrderjointList;                         //卡爪动作命令
extern QStringList clawCheckOrderjointList;                         //信号检测-卡爪检测命令
extern QStringList reserveCheckOrderjointList;                         //信号检测-预留检测命令
extern QStringList machineOutOrderjointList;                         //机床-输出动作命令
extern QStringList stackResetZeroOrderjointList;                         //堆叠-堆叠清零命令
extern QStringList stackMoveOrderjointList;                         //堆叠-堆叠组运动命令
extern QStringList stackFollowOrderjointList;                         //堆叠-放料跟随运动命令
extern QStringList reserveOutOrderjointList;                         //预留输出动作命令

extern QStringList waitInMachineOrderjointList;                         //信号等待-等待机床命令
extern QStringList waitInClawOrderjointList;                         //信号等待-等待卡爪命令
extern QStringList waitInReserveOrderjointList;                         //信号等待-等待预留命令
extern QStringList otherAlarmCustOrderjointList;                         //其他-报警自定义命令
extern QStringList otherDelayOrderjointList;                            //其他-延时动作指令
extern QStringList otherCycStopOrderjointList;                         //其他-周期停止命令
extern QStringList labelOrderjointList;                         //标签命令
extern QStringList OtherAlarmLampOrderjointList;                         //其他-报警灯命令
extern QStringList OtherAlarmSoundOrderjointList;                         //其他-报警声命令


extern QStringList logicIfOrderjointList;                         //逻辑&变量-如果-如果命令
extern QStringList logicWhileStartOrderjointList;                         //逻辑&变量-循环开始命令
extern QStringList logicVarOrderjointList;                         //逻辑&变量-变量-变量命令
extern QStringList logicAxisOrderjointList;                        //逻辑&变量-变量-轴命令
extern QStringList logicStackOrderjointList;                         //逻辑&变量-变量-堆叠命令
extern QStringList logicCurProductNumOrderjointList;                         //逻辑&变量-变量-实际产量命令结构体
extern QStringList logicTimeOrderjointList;                         //逻辑&变量-定时器命令
extern QStringList searchAxisMoveOrderjointList;                         //搜索-轴搜索运动命令

extern QStringList offsetAxisOrderjointList;                         //偏移-轴偏移运动命令
extern QStringList torqueGardOrderjointList;                         //扭矩保护命令
extern QStringList axisStopOrderjointList;                         //伺服停止命令
extern QStringList sunProOrderjointList;                         //子程序命令
extern QStringList ProEndJointList;                             //结束命令

extern QString cmpModeTar(uint8_t cmpmode);
#endif // ORDERJOINT_H
