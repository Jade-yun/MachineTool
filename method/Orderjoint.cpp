#include "Orderjoint.h"
#include <cstring>
#include "stdlib.h"
#include "iniconfig.h"
#include "port_setting.h"

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
//            CodeAndContInit();
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
    if(pAxisAct->referPointNum == 0)
    {        
        contStr = QString(axisMoveOrderjointList.at(0)).arg(" ").arg(axisStr).arg(QString::number(axispos/100,'f',2)).arg(pAxisAct->speed).arg(pAxisAct->advCSpeedDis).arg(OIS.delay);
    }
    else
    {
        contStr = QString(axisMoveOrderjointList.at(0)).arg(" ").arg(axisStr).arg(QString::number(axispos/100,'f',2)).arg(pAxisAct->speed).arg(pAxisAct->advCSpeedDis).arg(0);//获取参考点信息
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
    contStr =QString(clawActionOrderjointList.at(0)).arg("修改").arg(QString::number(ClawActionDelay/100,'f',2));
    return contStr;
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
    switch(pClawCheck->type)
    {
    case 0://检测结束
        contStr = QString(clawCheckOrderjointList.at(1)).arg("修改").arg(QString::number(ClawCheckDelay/100,'f',2));
        break;
    case 1://检测开始
         contStr = QString(clawCheckOrderjointList.at(0)).arg("修改").arg(QString::number(ClawCheckDelay/100,'f',2));
        break;
    case 2://检测结束
         contStr = QString(clawCheckOrderjointList.at(0)).arg("修改").arg(QString::number(ClawCheckDelay/100,'f',2));
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
    switch(pClawCheck->type)
    {
    case 0://通检测开始
        contStr = QString(reserveCheckOrderjointList.at(0)).arg("修改").arg(QString::number(ReserveDelay/100,'f',2));
        break;
    case 1://断检测开始
         contStr = QString(reserveCheckOrderjointList.at(1)).arg("修改").arg(QString::number(ReserveDelay/100,'f',2));
        break;
    case 2://检测结束
         contStr = QString(reserveCheckOrderjointList.at(2)).arg("修改").arg(QString::number(ReserveDelay/100,'f',2));
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
    contStr = QString(machineOutOrderjointList.at(0)).arg("修改").arg(QString::number(MachineDelay/100,'f',2));
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
    contStr = QString(stackResetZeroOrderjointList.at(0)).arg(QString::number(StackDelay/100,'f',2)).arg("堆叠组号");
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
    contStr = QString(stackMoveOrderjointList.at(0)).arg(QString::number(StackMoveDelay/100,'f',2)).arg("堆叠组号");
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
    contStr = QString(stackFollowOrderjointList.at(0)).arg("放料跟随组号").arg(QString::number(StackFollowDelay/100,'f',2));
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
    if(ReserveOut->function == 0 && ReserveOut->type == 1)
    {
        contStr = QString(reserveOutOrderjointList.at(0)).arg("输出通道").arg(QString::number(ReserveDelay/100,'f',2)).arg(ReserveOut->outportNum);
    }
    else if(ReserveOut->function == 0 && ReserveOut->type == 0)
    {
        contStr = QString(reserveOutOrderjointList.at(1)).arg("输出通道").arg(QString::number(ReserveDelay/100,'f',2)).arg(ReserveOut->outportNum);
    }
    else if(ReserveOut->function == 1)
    {
        contStr = QString(reserveOutOrderjointList.at(2)).arg("输出通道").arg(QString::number(ReserveDelay/100,'f',2)).arg(ReserveOut->outportNum);
    }
    else if(ReserveOut->function == 2)
    {
        contStr = QString(reserveOutOrderjointList.at(3)).arg("输出通道").arg(OIS.delay).arg(ReserveOut->outportNum);
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
    P_WaitInMachineStruct* WaitInMachine = (P_WaitInMachineStruct*)OIS.pData;
    double WaitDelay = OIS.delay;
    contStr = QString(waitInMachineOrderjointList.at(0)).arg("输出通道").arg(QString::number(WaitDelay/100,'f',2));
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
    if(WaitInClaw->inportSta == 0)
    {//0-off
        contStr = QString(waitInClawOrderjointList.at(1)).arg("输出通道").arg(QString::number(WaitDelay/100,'f',2));
    }
    else if(WaitInClaw->inportSta == 1)
    {//1-on
        contStr = QString(waitInClawOrderjointList.at(0)).arg("输出通道").arg(QString::number(WaitDelay/100,'f',2));
    }
    else if(WaitInClaw->inportSta == 2)
    {//2-位置修正
        contStr = QString(waitInClawOrderjointList.at(2)).arg("输出通道").arg(QString::number(WaitDelay/100,'f',2));
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
    if(WaitInReserve->inportSta == 0)
    {//0-off
        contStr = QString(waitInReserveOrderjointList.at(1)).arg("输出通道").arg(QString::number(WaitDelay/100,'f',2));
    }
    else if(WaitInReserve->inportSta == 1)
    {//1-on
        contStr = QString(waitInReserveOrderjointList.at(0)).arg("输出通道").arg(QString::number(WaitDelay/100,'f',2));
    }
    else if(WaitInReserve->inportSta == 2)
    {//2-up
        contStr = QString(waitInReserveOrderjointList.at(2)).arg("输出通道").arg(QString::number(WaitDelay/100,'f',2));
    }
    else if(WaitInReserve->inportSta == 3)
    {//2-down
        contStr = QString(waitInReserveOrderjointList.at(2)).arg("输出通道").arg(QString::number(WaitDelay/100,'f',2));
    }
    else if(WaitInReserve->inportSta == 4)
    {//2-jump
        contStr = QString(waitInReserveOrderjointList.at(2)).arg("输出通道").arg(QString::number(WaitDelay/100,'f',2));
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
    QString OffsetAxisStr = m_NameDefine[10+OffsetAxis->axis].modifyName;
    contStr = QString(offsetAxisOrderjointList.at(0)).arg(OffsetAxisStr).arg(QString::number(OffsetPos/100,'f',2)).arg(OffsetAxis->speed).arg("修改").arg(QString::number(delay/100,'f',2));
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
        double OffsetPos = SearchAxis->offsetDis;
        double advCDis = SearchAxis->advCDis;//提前位置
        QString SearchAxisStr = m_NameDefine[10+SearchAxis->axis].modifyName;
        contStr = QString(searchAxisMoveOrderjointList.at(0)).arg("搜索编号").arg(SearchAxisStr).arg(QString::number(OffsetPos/100,'f',2)).arg(SearchAxis->runSpeed).arg(QString::number(advCDis/100,'f',2)).arg(QString::number(delay/100,'f',2)).arg("停止通道");
    }
    else
    {
        contStr = QString(searchAxisMoveOrderjointList.at(1)).arg(QString::number(delay/100,'f',2));
    }

    return contStr;
}
