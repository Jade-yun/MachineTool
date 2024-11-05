#include "Orderjoint.h"
#include <cstring>
#include "stdlib.h"
#include "iniconfig.h"
#include "port_setting.h"
#include "cmd.h"
#include "program_save.h"
#include <QRegularExpression>
#include "file_preview.h"

QStringList axisMoveOrderjointList;                         //轴动作命令
QStringList clawActionOrderjointList;                         //卡爪动作命令
QStringList clawCheckOrderjointList;                         //信号检测-卡爪检测命令
QStringList reserveCheckOrderjointList;                         //信号检测-预留检测命令
QStringList machineOutOrderjointList;                         //机床-输出动作命令
QStringList stackResetZeroOrderjointList;                         //堆叠-堆叠清零命令
QStringList stackMoveOrderjointList;                         //堆叠-堆叠组运动命令
QStringList stackFollowOrderjointList;                         //堆叠-放料跟随运动命令
QStringList reserveOutOrderjointList;                         //预留输出动作命令

QStringList waitInMachineOrderjointList;                         //信号等待-等待机床命令
QStringList waitInClawOrderjointList;                         //信号等待-等待卡爪命令
QStringList waitInReserveOrderjointList;                         //信号等待-等待预留命令
QStringList otherDelayOrderjointList;                            //其他-延时动作指令
QStringList otherAlarmCustOrderjointList;                         //其他-报警自定义命令
QStringList otherCycStopOrderjointList;                         //其他-周期停止命令
QStringList OtherAlarmLampOrderjointList;                         //其他-报警灯命令
QStringList OtherAlarmSoundOrderjointList;                         //其他-报警声命令


QStringList labelOrderjointList;                              //标签命令

QStringList logicIfOrderjointList;                         //逻辑&变量-如果-如果命令
QStringList logicWhileStartOrderjointList;                         //逻辑&变量-循环开始命令
QStringList logicVarOrderjointList;                         //逻辑&变量-变量-变量命令
QStringList logicAxisOrderjointList;                        //逻辑&变量-变量-轴命令
QStringList logicStackOrderjointList;                         //逻辑&变量-变量-堆叠命令
QStringList logicCurProductNumOrderjointList;                         //逻辑&变量-变量-实际产量命令结构体
QStringList logicTimeOrderjointList;                         //逻辑&变量-定时器命令
QStringList searchAxisMoveOrderjointList;                         //搜索-轴搜索运动命令

QStringList offsetAxisOrderjointList;                         //偏移-轴偏移运动命令
QStringList torqueGardOrderjointList;                         //扭矩保护命令
QStringList axisStopOrderjointList;                         //伺服停止命令
QStringList sunProOrderjointList;                         //子程序命令
QStringList ProEndJointList;                             //结束命令

/*************************************************************************
**  函数名：  getOrderjoinIni()
**	输入参数：无
**	输出参数：无
**	函数功能：获取命令的内容拼接文件信息，用来拼接显示指令内容
**  作者：    wk
**  开发日期：2024/8/16
**************************************************************************/
void getOrderjoinIni()
{
    axisMoveOrderjointList=getIniValues(2,"P_AxisMoveCmd");                       //轴动作命令
    clawActionOrderjointList=getIniValues(2,"P_ClawActionCmd");                       //卡爪动作命令
    clawCheckOrderjointList=getIniValues(2,"P_ClawCheckCmd");                       //信号检测-卡爪检测命令
    reserveCheckOrderjointList=getIniValues(2,"P_ReserveCheckCmd");                        //信号检测-预留检测命令
    machineOutOrderjointList=getIniValues(2,"P_MachineOutCmd");                       //机床-输出动作命令
    stackResetZeroOrderjointList=getIniValues(2,"P_StackResetZeroCmd");                        //堆叠-堆叠清零命令
    stackMoveOrderjointList=getIniValues(2,"P_StackMoveCmd");                        //堆叠-堆叠组运动命令
    stackFollowOrderjointList=getIniValues(2,"P_StackFollowCmd");                         //堆叠-放料跟随运动命令
    reserveOutOrderjointList=getIniValues(2,"P_ReserveOutCmd");                         //预留输出动作命令

    waitInMachineOrderjointList=getIniValues(2,"P_WaitInMachineCmd");                         //信号等待-等待机床命令
    waitInClawOrderjointList=getIniValues(2,"P_WaitInClawCmd");                        //信号等待-等待卡爪命令
    waitInReserveOrderjointList=getIniValues(2,"P_WaitInReserveCmd");                        //信号等待-等待预留命令
    otherAlarmCustOrderjointList=getIniValues(2,"P_OtherAlarmCustCmd");                        //其他-报警自定义命令
    otherCycStopOrderjointList=getIniValues(2,"P_OtherCycStopCmd");                        //其他-周期停止命令
    otherDelayOrderjointList=getIniValues(2,"P_OtherDelayCmd");                        //其他-延时命令
    OtherAlarmLampOrderjointList=getIniValues(2,"P_OtherAlarmLampCmd");                        //其他-报警灯命令
    OtherAlarmSoundOrderjointList=getIniValues(2,"P_OtherAlarmSoundCmd");                        //其他-报警声命令
    labelOrderjointList=getIniValues(2,"P_LabelCmd");                        //标签命令

    logicIfOrderjointList=getIniValues(2,"P_LogicIfCmd");                         //逻辑&变量-如果-如果命令
    logicWhileStartOrderjointList=getIniValues(2,"P_LogicWhileStartCmd");                        //逻辑&变量-循环开始命令
    logicVarOrderjointList=getIniValues(2,"P_LogicVarCmd");                         //逻辑&变量-变量-变量命令
    logicAxisOrderjointList=getIniValues(2,"P_LogicAxisCmd");                         //逻辑&变量-变量-轴命令
    logicStackOrderjointList=getIniValues(2,"P_LogicStackCmd");                         //逻辑&变量-变量-堆叠命令
    logicCurProductNumOrderjointList=getIniValues(2,"P_LogicCurProductNumCmd");                         //逻辑&变量-变量-实际产量命令结构体
    logicTimeOrderjointList=getIniValues(2,"P_LogicTimeCmd");                        //逻辑&变量-定时器命令
    searchAxisMoveOrderjointList=getIniValues(2,"P_SearchAxisMoveCmd");                        //搜索-轴搜索运动命令

    offsetAxisOrderjointList=getIniValues(2,"P_OffsetAxisCmd");                         //偏移-轴偏移运动命令
    torqueGardOrderjointList=getIniValues(2,"P_TorqueGardCmd");                         //扭矩保护命令
    axisStopOrderjointList=getIniValues(2,"P_AxisStopCmd");                         //伺服停止命令
    sunProOrderjointList=getIniValues(2,"P_SunProCmd");                         //子程序命令
    ProEndJointList=getIniValues(2,"P_EndProCmd");                         //结束命令
}

/*************************************************************************
**  函数名：  JointStrDeal()
**	输入参数：指令信息结构体变量
**	输出参数：
**	函数功能：程序指令行号显示拼接函数
**  作者：    wukui
**  开发日期：2024/08/20
**************************************************************************/
QString JointRunOrderNum(P_ProOrderStruct OIS)
{
    QString RunOrderNum = "";
    if(OIS.cmd == C_PRO_END)
    {
        RunOrderNum = QString(ProEndJointList.at(1));//如果是结束指令，编号显示结束
    }
    else
    {
        RunOrderNum = QString::number(OIS.runOrderNum);
    }
    return RunOrderNum;
}
/*************************************************************************
**  函数名：  JointStrDeal()
**	输入参数：指令信息结构体变量
**	输出参数：
**	函数功能：拼接处理函数
**  作者：    wukui
**  开发日期：2024/08/12
**************************************************************************/
QString JointStrDeal(P_ProOrderStruct OIS)
{
    QString return_str = "";
    switch(OIS.cmd)
    {
    case C_AXIS_MOVE:	//轴动作
        return_str = AxisActJoint(OIS);
        break;
    case C_CLAW_ACTION: //卡爪指令
        return_str = ClawActionJoint(OIS);
        break;
    case C_CLAW_CHECK: //信号检测，卡爪检测
        return_str = ClawCheckJoint(OIS);
        break;
    case C_RESERVE_CHECK: //信号检测，预留
        return_str = ReserveCheckJoint(OIS);
        break;
    case C_MACHINE_OUT: //机床-输出动作
        return_str = MachineOutJoint(OIS);
        break;
    case C_STACK_RESET_ZERO: //堆叠-堆叠清零
        return_str = StackResetZeroJoint(OIS);
        break;
    case C_STACK_MOVE: //堆叠-堆叠组运动
        return_str = StackMoveJoint(OIS);
        break;
    case C_STACK_FOLLOW: //堆叠-放料跟随运动
        return_str = StackFollowJoint(OIS);
        break;
    case C_RESERVE_OUT://预留输出
        return_str = ReserveOutJoint(OIS);
        break;
    case C_WAIT_IN_MACHINE://信号等待-等待机床
        return_str = WaitInMachineJoint(OIS);
        break;
    case C_WAIT_IN_CLAW://信号等待-等待卡爪
        return_str = WaitInClawJoint(OIS);
        break;
    case C_WAIT_IN_RESERVE://信号等待-等待预留
        return_str = WaitInReserveJoint(OIS);
        break;
    case C_OTHER_DELAY://其他-延时
        return_str = OtherDelayJoint(OIS);
        break;
    case C_OTHER_ALARM_CUST://其他-报警自定义
        return_str = OtherAlarmCustJoint(OIS);
        break;
    case C_OTHER_CYC_STOP://其他-周期停止
        return_str = OtherCycStopJoint(OIS);
        break;
    case C_OTHER_ALARM_LAMP://其他-报警灯
        return_str = OtherAlarmLampJoint(OIS);
        break;
    case C_OTHER_ALARM_SOUND://其他-报警声
        return_str = OtherAlarmSoundJoint(OIS);
        break;
    case C_LABEL://标签
        return_str = LabelJoint(OIS);
        break;
    case C_LOGIC_IF: //逻辑&变量-如果-如果
        return_str = LogicIfJoint(OIS);
        break;
    case C_LOGIC_ELSE: //逻辑&变量-如果-否则
        return_str = LogicElseJoint();
        break;
    case C_LOGIC_END: //逻辑&变量-如果-结束如果
        return_str = LogicEndJoint();
        break;
    case C_LOGIC_WHILE_START: //逻辑&变量-循环-循环开始
        return_str = LogicWhileStartJoint(OIS);
        break;
    case C_LOGIC_WHILE_END: //逻辑&变量-循环-循环结束
        return_str = LogicWhileEndJoint();
        break;
    case C_LOGIC_VAR: //逻辑&变量-变量-变量
        return_str = LogicVarStructJoint(OIS);
        break;
    case C_LOGIC_AXIS: //逻辑&变量-变量-轴
        return_str = LogicAxisJoint(OIS);
        break;
    case C_LOGIC_STACK: //逻辑&变量-变量-堆叠
        return_str = LogicStackJoint(OIS);
        break;
    case C_LOGIC_PRODUCT: //逻辑&变量-变量-实际产量
        return_str = LogicCurProductNumJoint(OIS);
        break;
    case C_LOGIC_TIME: //逻辑&变量-定时器
        return_str = LogicTimeJoint(OIS);
        break;
    case C_SEARCH_AXIS_MOVE://搜索-轴运动搜索
        return_str = SearchAxisJoint(OIS);
        break;
    case C_SEARCH_STOP:     //搜索-感应停止
        return_str = SearchAxisJoint(OIS);
        break;
    case C_OFFSET_AXIS://偏移-轴偏移
        return_str = OffsetAxisJoint(OIS);
        break;
    case C_TORQUE_GARD://扭矩保护
        return_str = TorqueGardJoint(OIS);
        break;
    case C_AXIS_STOP://伺服停止
        return_str = AxisStopJoint(OIS);
        break;
    case C_SUN_PRO://子程序
        return_str = SunProJoint(OIS);
        break;
    case C_PRO_END:     //结束指令
        return_str = ProEndJoint(OIS);
         break;
    default:
        break;
    }
    if(OIS.noteFlag == 1 && return_str != "")
    {
        return_str = QString("//"+return_str);
    }
    return return_str;
}
/*************************************************************************
**  函数名：  AxisActJoint()
**	输入参数：指令信息结构体变量
**	输出参数：
**	函数功能：轴动作指令拼接显示
**  作者：    wukui
**  开发日期：2024/08/12
**************************************************************************/
QString AxisActJoint(P_ProOrderStruct OIS)
{
    QString contStr= "";
    P_AxisMoveStruct* pAxisAct = (P_AxisMoveStruct*)OIS.pData;
    uint8_t axisStrNum = 10+pAxisAct->axis;//配置文件中x轴从第十个元素开始存
    QString axisStr = m_NameDefine[axisStrNum].modifyName;//当前行运行操作轴显示
    double axispos = pAxisAct->pos;
    double advCSpeedDis = pAxisAct->advCSpeedDis;
    double AxisMovedelay = OIS.delay;
    if(pAxisAct->referPointNum == 0)
    {        
        contStr = QString(axisMoveOrderjointList.at(0)).arg(" ").arg(axisStr).arg(QString::number(axispos/100,'f',2)).arg(pAxisAct->speed).arg(QString::number(advCSpeedDis/100,'f',2)).arg(QString::number(AxisMovedelay/100,'f',2));
    }
    else
    {
        contStr = QString(axisMoveOrderjointList.at(0)).arg(" ").arg(axisStr).arg(QString::number(axispos/100,'f',2)).arg(pAxisAct->speed).arg(QString::number(advCSpeedDis/100,'f',2)).arg(QString::number(AxisMovedelay/100,'f',2));//获取参考点信息
    }
    return contStr;
}
/*************************************************************************
**  函数名：  ClawCheckAction()
**	输入参数：指令信息结构体变量
**	输出参数：
**	函数功能：卡爪动作指令拼接显示
**  作者：    wukui
**  开发日期：2024/08/26
**************************************************************************/
QString ClawActionJoint(P_ProOrderStruct OIS)
{
    QString contStr= "";
    double ClawActionDelay = OIS.delay;
    QString modifyName =Get_XYPort_Name(OIS);
    contStr =QString(clawActionOrderjointList.at(0)).arg(modifyName).arg(QString::number(ClawActionDelay/100,'f',2));
    return contStr;
}

QString Get_XYPort_Name(P_ProOrderStruct OIS)
{
    QString NameStr = "";
    switch (OIS.cmd) {
    case C_CLAW_ACTION:
    {
        P_ClawActionStruct* ClawAction = (P_ClawActionStruct*) OIS.pData;
        for(uint16_t i=0;i<OUTPUT_TOTAL_NUM;i++)
        {
            if(m_Port_Y[i].portNum == ClawAction->outportNum)
            {
                NameStr = m_Port_Y[i].modifyName;
                break;
            }
        }
        break;
    }
    case C_CLAW_CHECK:
    {
        P_ClawCheckStruct* ClawCheck = (P_ClawCheckStruct*) OIS.pData;
        for(uint16_t i=0;i<INPUT_TOTAL_NUM;i++)
        {
            if(m_Port_X[i].portNum == ClawCheck->outportNum)
            {
                NameStr = m_Port_X[i].modifyName;
                break;
            }
        }
        break;
    }
    case C_RESERVE_CHECK:
    {
        P_ReserveCheckStruct* ReserveCheck = (P_ReserveCheckStruct*) OIS.pData;
        if(ReserveCheck->inportNum>=1 && ReserveCheck->inportNum<=36)
        {
            NameStr = "[X"+QString::number(ReserveCheck->inportNum);
        }
        else if(ReserveCheck->inportNum>36 && ReserveCheck->inportNum<=60)
        {
            NameStr = "[EX"+QString::number(ReserveCheck->inportNum-36);
        }
        break;
    }
    case C_MACHINE_OUT:
    {
        P_MachineOutStruct* MachineOut = (P_MachineOutStruct*) OIS.pData;
        for(uint16_t i=0;i<OUTPUT_TOTAL_NUM;i++)
        {
            if(m_Port_Y[i].portNum == MachineOut->outportNum)
            {
                NameStr = m_Port_Y[i].modifyName;
                if(i!=MACHINE_AUTO_DOOR_1_OPEN && i!=MACHINE_AUTO_DOOR_1_CLOSE && i!=MACHINE_CHUCK_1_CLAMP && i!=MACHINE_CHUCK_1_LOOSENED)
                {
                    if(MachineOut->type == 0)
                    {
                        NameStr = NameStr+"通";
                    }
                    else if(MachineOut->type == 1)
                    {
                        NameStr = NameStr+"断";
                    }
                }
                break;
            }
        }

        break;
    }
    case C_RESERVE_OUT:
    {
        P_ReserveOutStruct* ReserveOut = (P_ReserveOutStruct*) OIS.pData;
        if(ReserveOut->outportNum>=1 && ReserveOut->outportNum<=24)
        {
            NameStr = "[Y"+QString::number(ReserveOut->outportNum);
        }
        else if(ReserveOut->outportNum>24 && ReserveOut->outportNum<=40)
        {
            NameStr = "[EY"+QString::number(ReserveOut->outportNum-24);
        }
        break;
    }
    case C_WAIT_IN_MACHINE:
    {
        P_WaitInMachineStruct* WaitInMachine = (P_WaitInMachineStruct*) OIS.pData;
        for(uint16_t i=0;i<INPUT_TOTAL_NUM;i++)
        {
            if(m_Port_X[i].portNum == WaitInMachine->inportNum)
            {
                NameStr = m_Port_X[i].modifyName;
                break;
            }
        }
        break;
    }
    case C_WAIT_IN_CLAW:
    {
        P_WaitInClawStruct* WaitInClaw = (P_WaitInClawStruct*) OIS.pData;
        if(WaitInClaw->inportSta == 2)
        {
            for(uint16_t i=0;i<OUTPUT_TOTAL_NUM;i++)
            {
                if(m_Port_Y[i].portNum == WaitInClaw->inportNum)
                {
                    NameStr = m_Port_Y[i].modifyName;
                    break;
                }
            }
        }
        else
        {
            for(uint16_t i=0;i<INPUT_TOTAL_NUM;i++)
            {
                if(m_Port_X[i].portNum == WaitInClaw->inportNum)
                {
                    NameStr = m_Port_X[i].modifyName;
                    break;
                }
            }
        }

        break;
    }
    case C_WAIT_IN_RESERVE:
    {
        P_WaitInReserveStruct* WaitInReserve = (P_WaitInReserveStruct*) OIS.pData;
        if(WaitInReserve->inportNum>=1 && WaitInReserve->inportNum<=36)
        {
            NameStr = "X"+QString::number(WaitInReserve->inportNum);
        }
        else if(WaitInReserve->inportNum>36 && WaitInReserve->inportNum<=60)
        {
            NameStr = "EX"+QString::number(WaitInReserve->inportNum-36);
        }
        break;
    }
    case C_SEARCH_AXIS_MOVE:
    {
        P_SearchAxisMoveStruct* SearchAxisMove = (P_SearchAxisMoveStruct*) OIS.pData;
        if(SearchAxisMove->inportNum[0]>=1 && SearchAxisMove->inportNum[0]<=36)
        {
            NameStr = "X"+QString::number(SearchAxisMove->inportNum[0]);
        }
        else if(SearchAxisMove->inportNum[0]>36 && SearchAxisMove->inportNum[0]<=60)
        {
            NameStr = "EX"+QString::number(SearchAxisMove->inportNum[0]-36);
        }
        break;
    }
    default:
        break;

    }
    return NameStr;
}
/*************************************************************************
**  函数名：  ClawCheckJoint()
**	输入参数：指令信息结构体变量
**	输出参数：
**	函数功能：卡爪检测指令拼接显示
**  作者：    wukui
**  开发日期：2024/08/26
**************************************************************************/
QString ClawCheckJoint(P_ProOrderStruct OIS)
{
    QString contStr= "";
    P_ClawCheckStruct* pClawCheck = (P_ClawCheckStruct*)OIS.pData;
    double ClawCheckDelay = OIS.delay;
    QString modifyName =Get_XYPort_Name(OIS);
    switch(pClawCheck->type)
    {//0-夹紧检测结束，1-夹紧检测开始，2-松开检测开始
    case 0:
        contStr = QString(clawCheckOrderjointList.at(1)).arg(modifyName).arg(QString::number(ClawCheckDelay/100,'f',2));
        break;
    case 1:
         contStr = QString(clawCheckOrderjointList.at(0)).arg(modifyName).arg(QString::number(ClawCheckDelay/100,'f',2));
        break;
    case 2:
         contStr = QString(clawCheckOrderjointList.at(0)).arg(modifyName).arg(QString::number(ClawCheckDelay/100,'f',2));
        break;
    default:
        break;
    }
    return contStr;
}
/*************************************************************************
**  函数名：  ReserveCheckJoint()
**	输入参数：指令信息结构体变量
**	输出参数：
**	函数功能：信号检测-预留检测指令拼接显示
**  作者：    wukui
**  开发日期：2024/08/26
**************************************************************************/
QString ReserveCheckJoint(P_ProOrderStruct OIS)
{
    QString contStr= "";
    P_ClawCheckStruct* pClawCheck = (P_ClawCheckStruct*)OIS.pData;
    double ReserveDelay = OIS.delay;
    QString modifyName =Get_XYPort_Name(OIS);
    switch(pClawCheck->type)
    {
    case 0://通检测开始
        contStr = QString(reserveCheckOrderjointList.at(0)).arg(modifyName).arg(QString::number(ReserveDelay/100,'f',2));
        break;
    case 1://断检测开始
         contStr = QString(reserveCheckOrderjointList.at(1)).arg(modifyName).arg(QString::number(ReserveDelay/100,'f',2));
        break;
    case 2://检测结束
         contStr = QString(reserveCheckOrderjointList.at(2)).arg(modifyName).arg(QString::number(ReserveDelay/100,'f',2));
        break;
    default:
        break;
    }
    return contStr;
}
/*************************************************************************
**  函数名：  MachineOutJoint()
**	输入参数：指令信息结构体变量
**	输出参数：
**	函数功能：机床-输出动作指令拼接显示
**  作者：    wukui
**  开发日期：2024/08/26
**************************************************************************/
QString MachineOutJoint(P_ProOrderStruct OIS)
{
    QString contStr= "";
    double MachineDelay = OIS.delay;
    QString modifyName =Get_XYPort_Name(OIS);
    contStr = QString(machineOutOrderjointList.at(0)).arg(modifyName).arg(QString::number(MachineDelay/100,'f',2));
    return contStr;
}

/*************************************************************************
**  函数名：  StackResetZeroJoint()
**	输入参数：指令信息结构体变量
**	输出参数：
**	函数功能：堆叠-堆叠清零拼接显示
**  作者：    wukui
**  开发日期：2024/8/27
**************************************************************************/
QString StackResetZeroJoint(P_ProOrderStruct OIS)
{
    QString contStr= "";
    double StackDelay = OIS.delay;
    P_StackResetZeroStruct* StackResetZero = (P_StackResetZeroStruct*)OIS.pData;
    QString stacknumStr = "";
    if(StackResetZero->stackNum>0)
    {
        stacknumStr = m_NameDefine[StackResetZero->stackNum-1+35].modifyName;
    }
    contStr = QString(stackResetZeroOrderjointList.at(0)).arg(QString::number(StackDelay/100,'f',2)).arg(stacknumStr);
    return contStr;
}

/*************************************************************************
**  函数名：  StackMoveJoint()
**	输入参数：指令信息结构体变量
**	输出参数：
**	函数功能：堆叠-堆叠组运动拼接显示
**  作者：    wukui
**  开发日期：2024/8/27
**************************************************************************/
QString StackMoveJoint(P_ProOrderStruct OIS)
{
    QString contStr= "";
    double StackMoveDelay = OIS.delay;
    P_StackMoveStruct* StackMove = (P_StackMoveStruct*)OIS.pData;
    QString stacknumStr = "";
    if(StackMove->stackNum>0)
    {
        stacknumStr = m_NameDefine[StackMove->stackNum-1+35].modifyName;
    }
    contStr = QString(stackMoveOrderjointList.at(0)).arg(QString::number(StackMoveDelay/100,'f',2)).arg(stacknumStr);
    return contStr;
}

/*************************************************************************
**  函数名：  StackResetZeroJoint()
**	输入参数：指令信息结构体变量
**	输出参数：
**	函数功能：堆叠-放料跟随运动拼接显示
**  作者：    wukui
**  开发日期：2024/8/27
**************************************************************************/
QString StackFollowJoint(P_ProOrderStruct OIS)
{
    QString contStr= "";
    double StackFollowDelay = OIS.delay;
    P_StackFollowStruct* StackFollow = (P_StackFollowStruct*)OIS.pData;
    QString stacknumStr = "";
    if(StackFollow->stackNum>0)
    {
        stacknumStr = m_NameDefine[StackFollow->stackNum-1+46].modifyName;
    }
    contStr = QString(stackFollowOrderjointList.at(0)).arg(stacknumStr).arg(QString::number(StackFollowDelay/100,'f',2));
    return contStr;
}

/*************************************************************************
**  函数名：  ProEndJoint()
**	输入参数：指令信息结构体变量
**	输出参数：
**	函数功能：程序结束指令拼接显示
**  作者：    wukui
**  开发日期：2024/8/20
**************************************************************************/
QString ProEndJoint(P_ProOrderStruct OIS)
{
    QString contStr= "";
    contStr = QString(ProEndJointList.at(0));
    return contStr;
}

/*************************************************************************
**  函数名：  ReserveOutJoint()
**	输入参数：指令信息结构体变量
**	输出参数：
**	函数功能：预留输出动作命令拼接显示
**  作者：    wukui
**  开发日期：2024/8/28
**************************************************************************/
QString ReserveOutJoint(P_ProOrderStruct OIS)
{
    QString contStr= "";
    P_ReserveOutStruct* ReserveOut = (P_ReserveOutStruct*)OIS.pData;
    double ReserveDelay = OIS.delay;
    QString OutName =Get_XYPort_Name(OIS);
    if(ReserveOut->function == 0 && ReserveOut->type == 1)
    {
        contStr = QString(reserveOutOrderjointList.at(0)).arg(OutName).arg(QString::number(ReserveDelay/100,'f',2)).arg(ReserveOut->interval);
    }
    else if(ReserveOut->function == 0 && ReserveOut->type == 0)
    {
        contStr = QString(reserveOutOrderjointList.at(1)).arg(OutName).arg(QString::number(ReserveDelay/100,'f',2)).arg(ReserveOut->interval);
    }
    else if(ReserveOut->function == 1)
    {
        contStr = QString(reserveOutOrderjointList.at(2)).arg(OutName).arg(QString::number(ReserveDelay/100,'f',2)).arg(ReserveOut->interval);
    }
    else if(ReserveOut->function == 2)
    {
        contStr = QString(reserveOutOrderjointList.at(3)).arg(OutName).arg(QString::number(ReserveDelay/100,'f',2)).arg(ReserveOut->interval);
    }
    return contStr;
}

/*************************************************************************
**  函数名：  WaitInMachineJoint()
**	输入参数：指令信息结构体变量
**	输出参数：
**	函数功能：信号等待-等待机床命令拼接显示
**  作者：    wukui
**  开发日期：2024/8/29
**************************************************************************/
QString WaitInMachineJoint(P_ProOrderStruct OIS)
{
    QString contStr= "";
    double WaitDelay = OIS.delay;
    QString WaitInName =Get_XYPort_Name(OIS);
    contStr = QString(waitInMachineOrderjointList.at(0)).arg(WaitInName).arg(QString::number(WaitDelay/100,'f',2));
    return contStr;
}
/*************************************************************************
**  函数名：  WaitInMachineJoint()
**	输入参数：指令信息结构体变量
**	输出参数：
**	函数功能：信号等待-等待卡爪命令拼接显示
**  作者：    wukui
**  开发日期：2024/8/29
**************************************************************************/
QString WaitInClawJoint(P_ProOrderStruct OIS)
{
    QString contStr= "";
    P_WaitInClawStruct* WaitInClaw = (P_WaitInClawStruct*)OIS.pData;
    double WaitDelay = OIS.delay;
    QString WaitInClawName =Get_XYPort_Name(OIS);
    if(WaitInClaw->inportSta == 0)
    {//0-off
        contStr = QString(waitInClawOrderjointList.at(1)).arg(WaitInClawName).arg(QString::number(WaitDelay/100,'f',2));
    }
    else if(WaitInClaw->inportSta == 1)
    {//1-on
        contStr = QString(waitInClawOrderjointList.at(0)).arg(WaitInClawName).arg(QString::number(WaitDelay/100,'f',2));
    }
    else if(WaitInClaw->inportSta == 2)
    {//2-位置修正
        contStr = QString(waitInClawOrderjointList.at(2)).arg(WaitInClawName).arg(QString::number(WaitDelay/100,'f',2));
    }
    if(WaitInClaw->type == 0)
    {
        contStr = contStr + " 返回步号 "+QString::number(WaitInClaw->backListNum);
    }
    else if(WaitInClaw->type ==1)
    {
        if(WaitInClaw->label>0)
        {
            QStringList labelText = LableNameList[m_OperateProNum][WaitInClaw->label-1].split(signalSpace);
            if(labelText.length()>1)
            {
                contStr = contStr + " 返回 ["+labelText[0]+"]";
            }
        }
    }
    return contStr;
}
/*************************************************************************
**  函数名：  WaitInMachineJoint()
**	输入参数：指令信息结构体变量
**	输出参数：
**	函数功能：信号等待-等待预留命令拼接显示
**  作者：    wukui
**  开发日期：2024/8/29
**************************************************************************/
QString WaitInReserveJoint(P_ProOrderStruct OIS)
{
    QString contStr= "";
    P_WaitInReserveStruct* WaitInReserve = (P_WaitInReserveStruct*)OIS.pData;
    double WaitDelay = OIS.delay;
    QString WaitInReserveName =Get_XYPort_Name(OIS);
    if(WaitInReserve->inportSta == 0)
    {//0-off
        contStr = QString(waitInReserveOrderjointList.at(1)).arg(WaitInReserveName).arg(QString::number(WaitDelay/100,'f',2));
    }
    else if(WaitInReserve->inportSta == 1)
    {//1-on
        contStr = QString(waitInReserveOrderjointList.at(0)).arg(WaitInReserveName).arg(QString::number(WaitDelay/100,'f',2));
    }
    else if(WaitInReserve->inportSta == 2)
    {//2-up
        contStr = QString(waitInReserveOrderjointList.at(2)).arg(WaitInReserveName).arg(QString::number(WaitDelay/100,'f',2));
    }
    else if(WaitInReserve->inportSta == 3)
    {//2-down
        contStr = QString(waitInReserveOrderjointList.at(2)).arg(WaitInReserveName).arg(QString::number(WaitDelay/100,'f',2));
    }
    else if(WaitInReserve->inportSta == 4)
    {//2-jump
        contStr = QString(waitInReserveOrderjointList.at(2)).arg(WaitInReserveName).arg(QString::number(WaitDelay/100,'f',2));
    }
    if(WaitInReserve->type == 0)
    {
        contStr = contStr + " 返回步号 "+QString::number(WaitInReserve->backListNum);
    }
    else if(WaitInReserve->type ==1)
    {
        if(WaitInReserve->label>0)
        {
            QStringList labelText = LableNameList[m_OperateProNum][WaitInReserve->label-1].split(signalSpace);
            if(labelText.length()>1)
            {
                contStr = contStr + " 返回 ["+labelText[0]+"]";
            }
        }
    }
    return contStr;
}

/*************************************************************************
**  函数名：  OtherDelayJoint()
**	输入参数：指令信息结构体变量
**	输出参数：
**	函数功能：其他-延时命令拼接显示
**  作者：    wukui
**  开发日期：2024/8/29
**************************************************************************/
QString OtherDelayJoint(P_ProOrderStruct OIS)
{
    QString contStr= "";
    double OtherDelay = OIS.delay;
    contStr = QString(otherDelayOrderjointList.at(0)).arg(QString::number(OtherDelay/100,'f',2));
    return contStr;
}

/*************************************************************************
**  函数名：  OtherAlarmCustJoint()
**	输入参数：指令信息结构体变量
**	输出参数：
**	函数功能：其他-报警自定义命令拼接显示
**  作者：    wukui
**  开发日期：2024/8/29
**************************************************************************/
QString OtherAlarmCustJoint(P_ProOrderStruct OIS)
{
    QString contStr= "";
    P_OtherAlarmCustStruct* OtherAlarmCust = (P_OtherAlarmCustStruct*)OIS.pData;
    contStr = QString(otherAlarmCustOrderjointList.at(0)).arg(OtherAlarmCust->alarmNum);
    return contStr;
}

/*************************************************************************
**  函数名：  OtherCycStopJoint()
**	输入参数：指令信息结构体变量
**	输出参数：
**	函数功能：其他-周期停止命令拼接显示
**  作者：    wukui
**  开发日期：2024/8/29
**************************************************************************/
QString OtherCycStopJoint(P_ProOrderStruct OIS)
{
    QString contStr= "";
    contStr = QString(otherCycStopOrderjointList.at(0));
    return contStr;
}

/*************************************************************************
**  函数名：  OtherAlarmLampJoint()
**	输入参数：指令信息结构体变量
**	输出参数：
**	函数功能：其他-报警灯命令拼接显示
**  作者：    wukui
**  开发日期：2024/8/29
**************************************************************************/
QString OtherAlarmLampJoint(P_ProOrderStruct OIS)
{
    QString contStr= "";
    P_OtherAlarmLampStruct* OtherAlarmLamp = (P_OtherAlarmLampStruct*)OIS.pData;
    double Delay = OIS.delay;
    if(OtherAlarmLamp->type == 0)
    {//关
        contStr = QString(OtherAlarmLampOrderjointList.at(1)).arg(QString::number(Delay/100,'f',2));
    }
    else if(OtherAlarmLamp->type == 1)
    {//开
        contStr = QString(OtherAlarmLampOrderjointList.at(0)).arg(QString::number(Delay/100,'f',2));
    }
    return contStr;
}

/*************************************************************************
**  函数名：  OtherAlarmSoundJoint()
**	输入参数：指令信息结构体变量
**	输出参数：
**	函数功能：其他-报警声命令拼接显示
**  作者：    wukui
**  开发日期：2024/8/29
**************************************************************************/
QString OtherAlarmSoundJoint(P_ProOrderStruct OIS)
{
    QString contStr= "";
    P_OtherAlarmSoundStruct* OtherAlarmSound = (P_OtherAlarmSoundStruct*)OIS.pData;
    double Delay = OIS.delay;
    if(OtherAlarmSound->type == 0)
    {//关
        contStr = QString(OtherAlarmSoundOrderjointList.at(1)).arg(QString::number(Delay/100,'f',2));
    }
    else if(OtherAlarmSound->type == 1)
    {//开
        contStr = QString(OtherAlarmSoundOrderjointList.at(0)).arg(QString::number(Delay/100,'f',2));
    }
    return contStr;
}

/*************************************************************************
**  函数名：  SunProJoint()
**	输入参数：指令信息结构体变量
**	输出参数：
**	函数功能：子程序命令拼接显示
**  作者：    wukui
**  开发日期：2024/8/30
**************************************************************************/
QString SunProJoint(P_ProOrderStruct OIS)
{
    QString contStr= "";
    P_SunProStruct* SunPro = (P_SunProStruct*)OIS.pData;
    double delay = OIS.delay;
    QString SunNumStr = m_NameDefine[1+SunPro->sunProNum].modifyName;//子程序名称
    if(SunPro->oprMode == 0)
    {//标准
        contStr = QString(sunProOrderjointList.at(0)).arg(SunNumStr);
    }
    else if(SunPro->oprMode == 1)
    {//自动
        contStr = QString(sunProOrderjointList.at(1)).arg(SunNumStr);
    }
    else if(SunPro->oprMode == 2)
    {//并行一次
        contStr = QString(sunProOrderjointList.at(2)).arg(SunNumStr);
    }
    else if(SunPro->oprMode == 3)
    {//等待子程序完成
        contStr = QString(sunProOrderjointList.at(3)).arg(SunNumStr).arg(QString::number(delay/100,'f',2));
    }
    return contStr;
}

/*************************************************************************
**  函数名：  AxisStopJoint()
**	输入参数：指令信息结构体变量
**	输出参数：
**	函数功能：伺服停止命令拼接显示
**  作者：    wukui
**  开发日期：2024/8/30
**************************************************************************/
QString AxisStopJoint(P_ProOrderStruct OIS)
{
    QString contStr= "";
    P_AxisStopStruct* AxisStop = (P_AxisStopStruct*)OIS.pData;
    double delay = OIS.delay;
    QString AxisStopStr = m_NameDefine[10+AxisStop->axis].modifyName;
    contStr = QString(axisStopOrderjointList.at(0)).arg(AxisStopStr).arg(QString::number(delay/100,'f',2));
    return contStr;
}

/*************************************************************************
**  函数名：  TorqueGardJoint()
**	输入参数：指令信息结构体变量
**	输出参数：
**	函数功能：扭矩保护命令拼接显示
**  作者：    wukui
**  开发日期：2024/8/30
**************************************************************************/
QString TorqueGardJoint(P_ProOrderStruct OIS)
{
    QString contStr= "";
    P_TorqueGardStruct* TorqueGard = (P_TorqueGardStruct*)OIS.pData;
    QString TorqueGardStr = m_NameDefine[10+TorqueGard->axis].modifyName;
    contStr = QString(torqueGardOrderjointList.at(0)).arg(TorqueGardStr).arg(TorqueGard->torqueValue);
    return contStr;
}

/*************************************************************************
**  函数名：  OffsetAxisJoint()
**	输入参数：指令信息结构体变量
**	输出参数：
**	函数功能：轴偏移运动命令拼接显示
**  作者：    wukui
**  开发日期：2024/8/30
**************************************************************************/
QString OffsetAxisJoint(P_ProOrderStruct OIS)
{
    QString contStr= "";
    P_OffsetAxisStruct* OffsetAxis = (P_OffsetAxisStruct*)OIS.pData;
    double delay = OIS.delay;
    double OffsetPos = OffsetAxis->offsetPos;
    double AdvEndPos = OffsetAxis->advEndDis;
    QString OffsetAxisStr = m_NameDefine[10+OffsetAxis->axis].modifyName;
    contStr = QString(offsetAxisOrderjointList.at(0)).arg(OffsetAxisStr).arg(QString::number(OffsetPos/100,'f',2)).arg(OffsetAxis->speed).arg(QString::number(AdvEndPos/100,'f',2)).arg(QString::number(delay/100,'f',2));
    return contStr;
}
/*************************************************************************
**  函数名：  SearchAxisJoint()
**	输入参数：指令信息结构体变量
**	输出参数：
**	函数功能：搜索命令拼接显示
**  作者：    wukui
**  开发日期：2024/8/31
**************************************************************************/
QString SearchAxisJoint(P_ProOrderStruct OIS)
{
    QString contStr= "";

    double delay = OIS.delay;

    if(OIS.cmd ==C_SEARCH_AXIS_MOVE)
    {
        P_SearchAxisMoveStruct* SearchAxis = (P_SearchAxisMoveStruct*)OIS.pData;
        QString SearchinportNum =Get_XYPort_Name(OIS);
        double OffsetPos = SearchAxis->offsetDis;
        double advCDis = SearchAxis->advCDis;//提前位置
        QString SearchAxisStr = m_NameDefine[10+SearchAxis->axis].modifyName;
        contStr = QString(searchAxisMoveOrderjointList.at(0)).arg("搜索"+QString::number(SearchAxis->searchNum)).arg(SearchAxisStr).arg(QString::number(OffsetPos/100,'f',2)).arg(SearchAxis->runSpeed).arg(QString::number(advCDis/100,'f',2)).arg(QString::number(delay/100,'f',2)).arg(SearchinportNum);
    }
    else
    {
        contStr = QString(searchAxisMoveOrderjointList.at(1)).arg(QString::number(delay/100,'f',2));
    }

    return contStr;
}
/*************************************************************************
**  函数名：  LogicIfJoint()
**	输入参数：指令信息结构体变量
**	输出参数：
**	函数功能：如果命令拼接显示
**  作者：    wukui
**  开发日期：2024/9/3
**************************************************************************/
QString LogicIfJoint(P_ProOrderStruct OIS)
{
    QString contStr= "";
    P_LogicIfStruct* LogicIf = (P_LogicIfStruct*)OIS.pData;
    QString LogicIfStr = "";
    if(LogicIf->reqSelectFlag[0] == 0)
    {//只有一个条件
        contStr = QString(logicIfOrderjointList.at(0)).arg(GetLogicIfStr(0,LogicIf));
    }
    if(LogicIf->reqSelectFlag[0] == 1 || LogicIf->reqSelectFlag[0] == 2)
    {//并且
        if(LogicIf->reqSelectFlag[0] == 1){
            contStr = QString(logicIfOrderjointList.at(1)).arg(GetLogicIfStr(0,LogicIf)).arg(GetLogicIfStr(1,LogicIf));
        }
        else {
            contStr = QString(logicIfOrderjointList.at(2)).arg(GetLogicIfStr(0,LogicIf)).arg(GetLogicIfStr(1,LogicIf));
        }
    }

    return contStr;
}

/*************************************************************************
**  函数名：  cmpModeTar()
**	输入参数：cmpmode：传入比较方式
**	输出参数：
**	函数功能：返回比较方式符号
**  作者：    wukui
**  开发日期：2024/9/4
**************************************************************************/
QString cmpModeTar(uint8_t cmpmode)
{
    QString returnstr = "";
    if(cmpmode == 0)
    {
        returnstr = ">";
    }
    else if(cmpmode == 1)
    {
        returnstr = "<";
    }
    else if(cmpmode == 2)
    {
        returnstr = ">=";
    }
    else if(cmpmode == 3)
    {
        returnstr = "<=";
    }
    else if(cmpmode == 4)
    {
        returnstr = "≠";
    }
    else if(cmpmode == 5)
    {
        returnstr = "==";
    }
    return returnstr;
}

/*************************************************************************
**  函数名：  GetLogicIfStr()
**	输入参数：IfIndex:0-条件1 1-条件2 LogicIf：如果结构体
**	输出参数：
**	函数功能：获取条件指令显示内容
**  作者：    wukui
**  开发日期：2024/9/4
**************************************************************************/
QString GetLogicIfStr(uint16_t IfIndex,P_LogicIfStruct* LogicIf)
{
    QString LogicIfStr = "";
    double sufferCmpValue = LogicIf->sufferCmpValue[IfIndex];
    if(LogicIf->cmpType[IfIndex] == 0)
    {//比较类型是输入信号
        if(LogicIf->inportNum[IfIndex]>=1 && LogicIf->inportNum[IfIndex]<=36)
        {
            LogicIfStr = "X"+QString::number(LogicIf->inportNum[IfIndex]);
        }
        else if(LogicIf->inportNum[IfIndex]>36 && LogicIf->inportNum[IfIndex]<=60)
        {
            LogicIfStr = "EX"+QString::number(LogicIf->inportNum[IfIndex]-36);
        }
        if(LogicIf->inportType[IfIndex] == 0)
        {//断
            LogicIfStr = LogicIfStr+"断";
        }
        else
        {
            LogicIfStr = LogicIfStr+"通";
        }
    }
    else if(LogicIf->cmpType[IfIndex] == 103)
    {//比较类型为输出信号
        if(LogicIf->inportNum[IfIndex] >=1 && LogicIf->inportNum[IfIndex] <=24)
        {
            LogicIfStr = "[Y"+QString::number(LogicIf->inportNum[IfIndex]);
        }
        else if(LogicIf->inportNum[IfIndex]>24 && LogicIf->inportNum[IfIndex]<=44)
        {
            LogicIfStr = "[EY"+QString::number(LogicIf->inportNum[IfIndex]-24);
        }
        if(LogicIf->inportType[IfIndex] == 0)
        {//断
            LogicIfStr = LogicIfStr+"断";
        }
        else
        {
            LogicIfStr = LogicIfStr+"通";
        }
    }
    else if(LogicIf->cmpType[IfIndex]>=1 && LogicIf->cmpType[IfIndex]<=20)
    {//比较类型是变量
        LogicIfStr = m_NameDefine[LogicIf->cmpType[IfIndex] + 17].modifyName;
        LogicIfStr += cmpModeTar(LogicIf->cmpMode[IfIndex]);
        if(LogicIf->sufferCmpType[IfIndex] == 0)
        {//被比较对象类型-常量
            if(m_VariableType[LogicIf->cmpType[IfIndex]-1]==0)
            {
                LogicIfStr = LogicIfStr+QString::number(sufferCmpValue);
            }
            else if(m_VariableType[LogicIf->cmpType[IfIndex]-1]==1)
            {
                LogicIfStr = LogicIfStr+QString::number(sufferCmpValue/10,'f',1);
            }
            else if(m_VariableType[LogicIf->cmpType[IfIndex]-1]==2)
            {
                LogicIfStr = LogicIfStr+QString::number(sufferCmpValue/100,'f',2);
            }
        }
        else if(LogicIf->sufferCmpType[IfIndex] >= 1 && LogicIf->sufferCmpType[IfIndex] <= 20)
        {//被比较对象类型-变量
            LogicIfStr = LogicIfStr+m_NameDefine[LogicIf->sufferCmpType[IfIndex] + 17].modifyName;
        }
        else if(LogicIf->sufferCmpType[IfIndex] == 102)
        {//被比较对象类型-实际产量
            LogicIfStr = LogicIfStr+"实际产量";
        }
    }
    else if(LogicIf->cmpType[IfIndex] == 102)
    {//比较对象类型-实际产量
        LogicIfStr = "实际产量";
        LogicIfStr += cmpModeTar(LogicIf->cmpMode[IfIndex]);
        if(LogicIf->sufferCmpType[IfIndex] == 0)
        {//被比较对象类型-常量
            LogicIfStr = LogicIfStr+QString::number(sufferCmpValue,'f',0);
        }
        else if(LogicIf->sufferCmpType[IfIndex] >= 1 && LogicIf->sufferCmpType[IfIndex] <= 20)
        {//被比较对象类型-变量
            LogicIfStr = LogicIfStr+m_NameDefine[LogicIf->sufferCmpType[IfIndex] + 17].modifyName;
        }
    }
    else if(LogicIf->cmpType[IfIndex]>=21 && LogicIf->cmpType[IfIndex]<=40)
    {//堆叠组方式1
        LogicIfStr = m_NameDefine[LogicIf->cmpType[IfIndex] + 14].modifyName;

        LogicIfStr += cmpModeTar(LogicIf->cmpMode[IfIndex]);
        if(LogicIf->sufferCmpType[IfIndex] == 0)
        {//被比较对象类型-常量
            LogicIfStr = LogicIfStr+QString::number(sufferCmpValue,'f',0);
        }
        else if(LogicIf->sufferCmpType[IfIndex] >= 21 && LogicIf->sufferCmpType[IfIndex] <= 40)
        {//被比较对象类型-堆叠
            LogicIfStr = LogicIfStr+m_NameDefine[LogicIf->sufferCmpType[IfIndex] + 14].modifyName;
        }
    }
    else if(LogicIf->cmpType[IfIndex]>=41 && LogicIf->cmpType[IfIndex]<=60)
    {//堆叠组方式2
        LogicIfStr = m_NameDefine[LogicIf->cmpType[IfIndex] + 5].modifyName;
        LogicIfStr += cmpModeTar(LogicIf->cmpMode[IfIndex]);
        LogicIfStr = LogicIfStr+"堆叠完成";
    }
    else if(LogicIf->cmpType[IfIndex]>=61 && LogicIf->cmpType[IfIndex]<=80)
    {//轴-位置
        LogicIfStr = m_NameDefine[LogicIf->cmpType[IfIndex] - 51].modifyName;
        LogicIfStr += cmpModeTar(LogicIf->cmpMode[IfIndex]);

        if(LogicIf->sufferCmpType[IfIndex] == 0)
        {//被比较对象类型-位置
            LogicIfStr = LogicIfStr+QString::number(sufferCmpValue/100,'f',2);
        }
        else if(LogicIf->sufferCmpType[IfIndex] >= 1 && LogicIf->sufferCmpType[IfIndex] <= 20)
        {//被比较对象类型-变量
            LogicIfStr = LogicIfStr+m_NameDefine[LogicIf->sufferCmpType[IfIndex] + 17].modifyName;
        }
        else if(LogicIf->sufferCmpType[IfIndex] >= 61 && LogicIf->sufferCmpType[IfIndex] <= 80)
        {//被比较对象类型-轴
            LogicIfStr = LogicIfStr+m_NameDefine[LogicIf->sufferCmpType[IfIndex] - 51].modifyName;
        }
    }
    else if(LogicIf->cmpType[IfIndex]>=81 && LogicIf->cmpType[IfIndex]<=100)
    {//定时器
        LogicIfStr = m_NameDefine[LogicIf->cmpType[IfIndex]-31].modifyName;
        LogicIfStr += cmpModeTar(LogicIf->cmpMode[IfIndex]);

        if(LogicIf->sufferCmpType[IfIndex] == 0)
        {//被比较对象类型-常量
            LogicIfStr = LogicIfStr+QString::number(sufferCmpValue,'f',0);
        }
        else if(LogicIf->cmpType[IfIndex]>=81 && LogicIf->cmpType[IfIndex]<=100)
        {//被比较对象类型-定时器
            LogicIfStr = m_NameDefine[LogicIf->cmpType[IfIndex]-31].modifyName;
        }
    }
    return LogicIfStr;
}

/*************************************************************************
**  函数名：  LogicElseJoint()
**	输入参数：指令信息结构体变量
**	输出参数：
**	函数功能：否则命令拼接显示
**  作者：    wukui
**  开发日期：2024/9/4
**************************************************************************/
QString LogicElseJoint(void)
{
    QString contStr= "";
    contStr = QString(logicIfOrderjointList.at(3));
    return contStr;
}

/*************************************************************************
**  函数名：  LogicEndJoint()
**	输入参数：指令信息结构体变量
**	输出参数：
**	函数功能：结束命令拼接显示
**  作者：    wukui
**  开发日期：2024/9/4
**************************************************************************/
QString LogicEndJoint(void)
{
    QString contStr= "";
    contStr = QString(logicIfOrderjointList.at(4));
    return contStr;
}

/*************************************************************************
**  函数名：  LogicWhileStartJoint()
**	输入参数：指令信息结构体变量
**	输出参数：
**	函数功能：循环开始命令拼接显示
**  作者：    wukui
**  开发日期：2024/9/4
**************************************************************************/
QString LogicWhileStartJoint(P_ProOrderStruct OIS)
{
    P_LogicWhileStartStruct* LogicWhileStart = (P_LogicWhileStartStruct*) OIS.pData;
    QString contStr= "";
    contStr = QString(logicWhileStartOrderjointList.at(0)).arg(LogicWhileStart->cycNum);
    return contStr;
}

/*************************************************************************
**  函数名：  LogicWhileEndJoint()
**	输入参数：指令信息结构体变量
**	输出参数：
**	函数功能：循环结束命令拼接显示
**  作者：    wukui
**  开发日期：2024/9/4
**************************************************************************/
QString LogicWhileEndJoint(void)
{
    QString contStr= "";
    contStr = QString(logicWhileStartOrderjointList.at(1));
    return contStr;
}

/*************************************************************************
**  函数名：  LogicVarStructJoint()
**	输入参数：指令信息结构体变量
**	输出参数：
**	函数功能：逻辑&变量-变量-变量命令拼接显示
**  作者：    wukui
**  开发日期：2024/9/4
**************************************************************************/
QString LogicVarStructJoint(P_ProOrderStruct OIS)
{
    P_LogicVarStruct* LogicVarStart = (P_LogicVarStruct*) OIS.pData;
    QString contStr= "";
    QString OperStr = GetLogicVarStr(OIS);
    contStr = QString(logicVarOrderjointList.at(LogicVarStart->operMode)).arg(m_NameDefine[17+LogicVarStart->varNum].modifyName).arg(OperStr);
    return contStr;
}


/*************************************************************************
**  函数名：  LogicAxisStructJoint()
**	输入参数：指令信息结构体变量
**	输出参数：
**	函数功能：逻辑&变量-变量-轴命令拼接显示
**  作者：    wukui
**  开发日期：2024/9/4
**************************************************************************/
QString LogicAxisJoint(P_ProOrderStruct OIS)
{
    P_LogicAxisStruct* LogicAxis = (P_LogicAxisStruct*) OIS.pData;
    QString contStr= "";
    QString OperStr = GetLogicVarStr(OIS);
    contStr = QString(logicAxisOrderjointList.at(LogicAxis->operMode)).arg(m_NameDefine[9+LogicAxis->axisNum].modifyName).arg(OperStr);
    return contStr;
}
/*************************************************************************
**  函数名：  LogicStackJoint()
**	输入参数：指令信息结构体变量
**	输出参数：
**	函数功能：逻辑&变量-变量-堆叠命令拼接显示
**  作者：    wukui
**  开发日期：2024/9/4
**************************************************************************/
QString LogicStackJoint(P_ProOrderStruct OIS)
{
    P_LogicStackStruct* LogicStack = (P_LogicStackStruct*) OIS.pData;
    QString contStr= "";
    QString OperStr = GetLogicVarStr(OIS);
    contStr = QString(logicStackOrderjointList.at(LogicStack->operMode)).arg(m_NameDefine[34+LogicStack->stackNum].modifyName).arg(OperStr);
    return contStr;
}

/*************************************************************************
**  函数名：  LogicCurProductNumJoint()
**	输入参数：指令信息结构体变量
**	输出参数：
**	函数功能：逻辑&变量-变量-实际产量命令拼接显示
**  作者：    wukui
**  开发日期：2024/9/4
**************************************************************************/
QString LogicCurProductNumJoint(P_ProOrderStruct OIS)
{
    P_LogicCurProductNumStruct* LogicCurProductNum = (P_LogicCurProductNumStruct*) OIS.pData;
    QString contStr= "";
    QString OperStr = GetLogicVarStr(OIS);
    contStr = QString(logicCurProductNumOrderjointList.at(LogicCurProductNum->operMode)).arg("实际产量").arg(OperStr);
    return contStr;
}

/*************************************************************************
**  函数名：  LogicTimeJoint()
**	输入参数：指令信息结构体变量
**	输出参数：
**	函数功能：逻辑&变量-变量-定时器命令拼接显示
**  作者：    wukui
**  开发日期：2024/9/4
**************************************************************************/
QString LogicTimeJoint(P_ProOrderStruct OIS)
{
    P_LogicTimeStruct* LogicTime = (P_LogicTimeStruct*) OIS.pData;
    QString contStr= "";
    contStr = QString(logicTimeOrderjointList.at(LogicTime->operMode)).arg(m_NameDefine[49+LogicTime->timeNum].modifyName);
    return contStr;
}
/*************************************************************************
**  函数名：  GetLogicVarStr()
**	输入参数：指令信息结构体变量
**	输出参数：
**	函数功能：逻辑&变量-变量-获取操作数拼接内容
**  作者：    wukui
**  开发日期：2024/9/4
**************************************************************************/
QString GetLogicVarStr(P_ProOrderStruct OIS)
{
    QString  returnstr = "";
    switch (OIS.cmd)
    {
    case C_LOGIC_VAR:
    {
        P_LogicVarStruct* LogicVarStart = (P_LogicVarStruct*) OIS.pData;
        if(LogicVarStart->sufferOperType == 0)
        {//常量
            double sufferOperValue = LogicVarStart->sufferOperValue;
            if(m_VariableType[LogicVarStart->varNum-1]==0)
            {
                returnstr = QString::number(sufferOperValue);
            }
            else if(m_VariableType[LogicVarStart->varNum-1]==1)
            {
                returnstr = QString::number(sufferOperValue/10,'f',1);
            }
            else if(m_VariableType[LogicVarStart->varNum-1]==2)
            {
                returnstr = QString::number(sufferOperValue/100,'f',2);
            }
        }
        else if(LogicVarStart->sufferOperType>=1 && LogicVarStart->sufferOperType<=20)
        {//1-20变量
            returnstr = m_NameDefine[17+LogicVarStart->sufferOperType].modifyName;
        }
        else if(LogicVarStart->sufferOperType>=21 && LogicVarStart->sufferOperType<=40)
        {//21-40轴
            returnstr = m_NameDefine[LogicVarStart->sufferOperType-11].modifyName;
        }
        else if(LogicVarStart->sufferOperType == 101)
        {//101实际产量
            returnstr = "实际产量";
        }
        break;
    }
    case C_LOGIC_AXIS:
    {
        P_LogicAxisStruct* LogicAxisStart = (P_LogicAxisStruct*) OIS.pData;
        if(LogicAxisStart->sufferOperType == 0)
        {//常量
            double sufferOperValue = LogicAxisStart->sufferOperValue;
            returnstr = QString::number(sufferOperValue/100,'f',2);
        }
        else if(LogicAxisStart->sufferOperType>=1 && LogicAxisStart->sufferOperType<=20)
        {//1-20变量
            returnstr = m_NameDefine[17+LogicAxisStart->sufferOperType].modifyName;
        }
        break;
    }
    case C_LOGIC_STACK:
    {
        P_LogicStackStruct* LogicStackStart = (P_LogicStackStruct*) OIS.pData;
        if(LogicStackStart->sufferOperType == 0)
        {//常量
            returnstr = QString::number(LogicStackStart->sufferOperValue);
        }
        else if(LogicStackStart->sufferOperType>=1 && LogicStackStart->sufferOperType<=20)
        {//1-20堆叠计数
            returnstr = m_NameDefine[34+LogicStackStart->sufferOperType].modifyName;
        }
        break;
    }
    case C_LOGIC_PRODUCT:
    {
        P_LogicCurProductNumStruct* LogicCurProductNumStruct = (P_LogicCurProductNumStruct*) OIS.pData;
        if(LogicCurProductNumStruct->sufferOperType == 0)
        {//常量
            returnstr = QString::number(LogicCurProductNumStruct->sufferOperValue);
        }
        else if(LogicCurProductNumStruct->sufferOperType>=1 && LogicCurProductNumStruct->sufferOperType<=20)
        {//1-20变量
            returnstr = m_NameDefine[17+LogicCurProductNumStruct->sufferOperType].modifyName;
        }
        break;
    }
    default:
        break;
    }
    return returnstr;
}

/*************************************************************************
**  函数名：  LabelJoint()
**	输入参数：指令信息结构体变量
**	输出参数：
**	函数功能：标签指令拼接内容
**  作者：    wukui
**  开发日期：2024/9/26
**************************************************************************/
QString LabelJoint(P_ProOrderStruct OIS)
{
    P_LabelStruct* LabelStruct = (P_LabelStruct*) OIS.pData;
    QString contStr = "";
    uint16_t LabelIndex = 0;
    if(File_Preview_Label)
    {
        if(Preview_LableNameList[m_OperateProNum].count()>0)
        {
            QString Itemtext = Preview_LableNameList[m_OperateProNum][LabelStruct->labelNum-1];
            QRegularExpression re("标签(\\d+)"); // 匹配“标签”后面的数字
            QRegularExpressionMatch match = re.match(Itemtext);
            if (match.hasMatch()) {
                LabelIndex = match.captured(1).toUInt(); // 提取匹配到的数字
            }
            QStringList labeltext = Preview_LableNameList[m_OperateProNum][LabelIndex-1].split(signalSpace);
            if(labeltext.length()>1)
            {
                contStr= "[" + labeltext[0] +"] "+labeltext[1];
            }
            if(LabelStruct->type == 1)
            {
                contStr = "跳转到 "+contStr;
            }
        }
    }
    else
    {
        if(LableNameList[m_OperateProNum].count()>0)
        {
            QString Itemtext = LableNameList[m_OperateProNum][LabelStruct->labelNum-1];
            QRegularExpression re("标签(\\d+)"); // 匹配“标签”后面的数字
            QRegularExpressionMatch match = re.match(Itemtext);
            if (match.hasMatch()) {
                LabelIndex = match.captured(1).toUInt(); // 提取匹配到的数字
            }
            QStringList labeltext = LableNameList[m_OperateProNum][LabelIndex-1].split(signalSpace);
            if(labeltext.length()>1)
            {
                contStr= "[" + labeltext[0] +"] "+labeltext[1];
            }
            if(LabelStruct->type == 1)
            {
                contStr = "跳转到 "+contStr;
            }
        }
    }
    return contStr;
}
