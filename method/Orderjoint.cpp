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
QStringList otherAlarmCustOrderjointList;                         //其他-报警自定义命令
QStringList otherCycStopOrderjointList;                         //其他-周期停止命令
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
    case C_PRO_END:     //结束指令
        return_str = ProEndJoint(OIS);
         break;
        default:
//            CodeAndContInit();
            break;
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
    QString axisStr = NameDefine_CHSList.at(axisStrNum);//当前行运行操作轴显示
    if(pAxisAct->referPointNum == 0)
    {        
        contStr = QString(axisMoveOrderjointList.at(0)).arg(" ").arg(axisStr).arg(pAxisAct->pos/100).arg(pAxisAct->speed).arg(pAxisAct->advCSpeedDis).arg(OIS.delay);
    }
    else
    {
        contStr = QString(axisMoveOrderjointList.at(0)).arg(" ").arg(axisStr).arg(pAxisAct->pos/100).arg(pAxisAct->speed).arg(pAxisAct->advCSpeedDis).arg(0);//获取参考点信息
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
    contStr =QString(clawActionOrderjointList.at(0)).arg("修改").arg(OIS.delay);
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
    switch(pClawCheck->type)
    {
    case 0://检测结束
        contStr = QString(clawCheckOrderjointList.at(1)).arg("修改").arg(OIS.delay);
        break;
    case 1://检测开始
         contStr = QString(clawCheckOrderjointList.at(0)).arg("修改").arg(OIS.delay);
        break;
    case 2://检测结束
         contStr = QString(clawCheckOrderjointList.at(0)).arg("修改").arg(OIS.delay);
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
    switch(pClawCheck->type)
    {
    case 0://通检测开始
        contStr = QString(reserveCheckOrderjointList.at(0)).arg("修改").arg(OIS.delay);
        break;
    case 1://断检测开始
         contStr = QString(reserveCheckOrderjointList.at(1)).arg("修改").arg(OIS.delay);
        break;
    case 2://检测结束
         contStr = QString(reserveCheckOrderjointList.at(2)).arg("修改").arg(OIS.delay);
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
    contStr = QString(machineOutOrderjointList.at(0)).arg("修改").arg(OIS.delay);
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
    contStr = QString(stackResetZeroOrderjointList.at(0)).arg(OIS.delay).arg("堆叠组号");
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
    contStr = QString(stackMoveOrderjointList.at(0)).arg(OIS.delay).arg("堆叠组号");
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
    contStr = QString(stackFollowOrderjointList.at(0)).arg("放料跟随组号").arg(OIS.delay);
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
