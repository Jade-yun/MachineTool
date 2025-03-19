#include "program_save.h"

QString m_ProgramPath="/opt/MachineTool/program";

const QString SUFFIX_PROGRAM = ".ZHpro";
const QString SUFFIX_REFER = ".ref";
QString signalSpace = " ";
QString doubleSpace = "  ";
QString lineFeed = "\n";

QStringList fileSectionList;                        //程序保存节点
QStringList programInfoList;                        //程序信息
QStringList referencePointList;                     //每个参考点表示
QStringList LablePartList;                          //标签指令节点
QStringList LableNameList[PRO_NUM];                 //标签指令名字缓存
QStringList CurrentLableNameList;                   //当前操作程序标签指令缓存
QStringList Preview_LableNameList[PRO_NUM];         //浏览指令名字缓存
QStringList programBasicCmdList;                    //基本命令

QStringList axisMoveCmdList;                         //轴动作命令
QStringList clawActionCmdList;                         //卡爪动作命令
QStringList clawCheckCmdList;                         //信号检测-卡爪检测命令
QStringList reserveCheckCmdList;                         //信号检测-预留检测命令
QStringList machineOutCmdList;                         //机床-输出动作命令
QStringList stackResetZeroCmdList;                         //堆叠-堆叠清零命令
QStringList stackMoveCmdList;                         //堆叠-堆叠组运动命令
QStringList stackFollowCmdList;                         //堆叠-放料跟随运动命令
QStringList reserveOutCmdList;                         //预留输出动作命令

QStringList waitInMachineCmdList;                         //信号等待-等待机床命令
QStringList waitInClawCmdList;                         //信号等待-等待卡爪命令
QStringList waitInReserveCmdList;                         //信号等待-等待预留命令
QStringList otherAlarmCustCmdList;                         //其他-报警自定义命令
QStringList otherCycStopCmdList;                         //其他-周期停止命令
QStringList other_alarm_lamp;                            //其他-报警灯指令
QStringList other_alarm_sound;                           //其他-报警声指令
QStringList labelCmdList;                         //标签命令

QStringList logicIfCmdList;                         //逻辑&变量-如果-如果命令
QStringList logicWhileStartCmdList;                         //逻辑&变量-循环开始命令
QStringList logicVarCmdList;                         //逻辑&变量-变量-变量命令
QStringList logicAxisCmdList;                        //逻辑&变量-变量-轴命令
QStringList logicStackCmdList;                         //逻辑&变量-变量-堆叠命令
QStringList logicCurProductNumCmdList;                         //逻辑&变量-变量-实际产量命令结构体
QStringList logicTimeCmdList;                         //逻辑&变量-定时器命令
QStringList searchAxisMoveCmdList;                         //搜索-轴搜索运动命令

QStringList offsetAxisCmdList;                         //偏移-轴偏移运动命令
QStringList torqueGardCmdList;                         //扭矩保护命令
QStringList axisStopCmdList;                         //伺服停止命令
QStringList sunProCmdList;                         //子程序命令
/*************************************************************************
**  作者：    PXC
**  开发日期：2024/7/24
**  程序保存思路：
**	1、新建程序名时，需要在Program文件夹下创建一个文件名设定名字的txt文件，同时写入最基本的程序指令
**  2、删除：删除选中的程序，并更新程序
**  3、组合：将当前程序的执行顺序调为和上一条程序执行顺序一样
**  4、分解：将当前程序的执行顺序与上一条程序执行顺序分开
**  5、编辑：对当前程序进行修改
**  6、上移下移：交换程序的执行顺序
**  7、屏蔽：当前程序不执行
**  8、全选：全选当前程序所有行
**  9、多选：能够进行多选
**  10、插入：在当前选中的程序上方进行插入
**************************************************************************/
/*************************************************************************
**  函数名：  readIniProgramStruct()
**	输入参数：无
**	输出参数：无
**	函数功能：获取命令的配置文件信息，方便进行程序保存
**  作者：    PXC
**  开发日期：2024/7/26
**************************************************************************/
void readIniProgramStruct()
{
    fileSectionList=getIniValues(0,"Section");                        //程序保存节点
    programInfoList=getIniValues(0,"ProgramInfo");                       //程序信息
    LablePartList=getIniValues(0,"LabelPart");
    referencePointList=getIniValues(0,"ReferencePoint");                    //每个参考点表示
    programBasicCmdList=getIniValues(0,"ProgramBasicCmd");                   //基本命令

    axisMoveCmdList=getIniValues(0,"P_AxisMoveCmd");                       //轴动作命令
    clawActionCmdList=getIniValues(0,"P_ClawActionCmd");                       //卡爪动作命令
    clawCheckCmdList=getIniValues(0,"P_ClawCheckCmd");                       //信号检测-卡爪检测命令
    reserveCheckCmdList=getIniValues(0,"P_ReserveCheckCmd");                        //信号检测-预留检测命令
    machineOutCmdList=getIniValues(0,"P_MachineOutCmd");                       //机床-输出动作命令
    stackResetZeroCmdList=getIniValues(0,"P_StackResetZeroCmd");                        //堆叠-堆叠清零命令
    stackMoveCmdList=getIniValues(0,"P_StackMoveCmd");                        //堆叠-堆叠组运动命令
    stackFollowCmdList=getIniValues(0,"P_StackFollowCmd");                         //堆叠-放料跟随运动命令
    reserveOutCmdList=getIniValues(0,"P_ReserveOutCmd");                         //预留输出动作命令

    waitInMachineCmdList=getIniValues(0,"P_WaitInMachineCmd");                         //信号等待-等待机床命令
    waitInClawCmdList=getIniValues(0,"P_WaitInClawCmd");                        //信号等待-等待卡爪命令
    waitInReserveCmdList=getIniValues(0,"P_WaitInReserveCmd");                        //信号等待-等待预留命令
    otherAlarmCustCmdList=getIniValues(0,"P_OtherAlarmCustCmd");                        //其他-报警自定义命令
    other_alarm_lamp=getIniValues(0,"P_OtherAlarmLampCmd");                        //其他-报警灯命令
    other_alarm_sound=getIniValues(0,"P_OtherAlarmSoundCmd");                        //其他-报警声命令
    otherCycStopCmdList=getIniValues(0,"P_OtherCycStopCmd");                        //其他-周期停止命令
    labelCmdList=getIniValues(0,"P_LabelCmd");                        //标签命令

    logicIfCmdList=getIniValues(0,"P_LogicIfCmd");                         //逻辑&变量-如果-如果命令
    logicWhileStartCmdList=getIniValues(0,"P_LogicWhileStartCmd");                        //逻辑&变量-循环开始命令
    logicVarCmdList=getIniValues(0,"P_LogicVarCmd");                         //逻辑&变量-变量-变量命令
    logicAxisCmdList=getIniValues(0,"P_LogicAxisCmd");                         //逻辑&变量-变量-轴命令
    logicStackCmdList=getIniValues(0,"P_LogicStackCmd");                         //逻辑&变量-变量-堆叠命令
    logicCurProductNumCmdList=getIniValues(0,"P_LogicCurProductNumCmd");                         //逻辑&变量-变量-实际产量命令结构体
    logicTimeCmdList=getIniValues(0,"P_LogicTimeCmd");                        //逻辑&变量-定时器命令
    searchAxisMoveCmdList=getIniValues(0,"P_SearchAxisMoveCmd");                        //搜索-轴搜索运动命令

    offsetAxisCmdList=getIniValues(0,"P_OffsetAxisCmd");                         //偏移-轴偏移运动命令
    torqueGardCmdList=getIniValues(0,"P_TorqueGardCmd");                         //扭矩保护命令
    axisStopCmdList=getIniValues(0,"P_AxisStopCmd");                         //伺服停止命令
    sunProCmdList=getIniValues(0,"P_SunProCmd");                         //子程序命令
}

/*************************************************************************
**  函数名：  ProgramIndexUpdate()
**	输入参数：
**	输出参数：返回一个程序编号,从1开始
**	函数功能：设置程序编号
**  作者：    wk
**  开发日期：2024/12/30
**************************************************************************/
uint16_t SetProgramIndex()
{
    for(int i=0;i<m_FileNameNum;i++)
    {
        m_ProgramNameAndPath[i].index = i+1;
    }
}
/*************************************************************************
**  函数名：  newBuildProgram(QString fileName)
**	输入参数：输入的文件名
**	输出参数：弹窗显示输出
**	函数功能：创建新的文件（单机新建时触发）
**  作者：    PXC
**  开发日期：2024/7/24
**************************************************************************/
uint8_t newBuildProgram(const QString& fileName)
{
    if(fileName=="")
        return PROGRAM_TEXT_EMPTY;
    //判断文件是否存在
    for(int i=0;i<m_ProgramNameAndPath.count();i++)
    {
        if(m_ProgramNameAndPath[i].fileName.compare(fileName)==0)
        {
            return PROGRAM_FILENAME_EXIST;
        }
    }
    D_ProgramNameAndPathStruct P_NamePathTemp;
    P_NamePathTemp.fileName=fileName;
    m_FileNameNum++;
    //开始将程序写出（基本程序）
    QString filePath=m_ProgramPath+"/"+fileName+SUFFIX_PROGRAM;
    P_NamePathTemp.filePath=filePath;
    P_NamePathTemp.index=m_FileNameNum;
    P_NamePathTemp.filePermission=0;//文件操作权限默认为：0-允许
    QDateTime currentDate=QDateTime::currentDateTime();
    P_NamePathTemp.changeTime=currentDate.toString("yyyy-MM-dd hh:mm:ss");
    m_ProgramNameAndPath.append(P_NamePathTemp);
    SetProgramIndex();//设置程序文件编号
    if(!writeBasicProgram(P_NamePathTemp))
    {
        return PROGRAM_OPEN_WRITE;
    }
    else
    {
        setProgramNameAndPath(m_ProgramNameAndPath);//新建文件成功，存储所有文件信息
        const QString filePath = m_ProgramPath+"/"+fileName + SUFFIX_REFER;
        QFile file(filePath);
        // 尝试以写模式打开文件（这将创建文件，如果它不存在的话）
        if (!file.open(QIODevice::WriteOnly)) {
            return -1; // 或者其他错误处理
        }
        // 文件现在已经创建（并且是空的），可以关闭它了
        file.close();
    }
    return 0;
}
//新建基础程序
bool writeBasicProgram(D_ProgramNameAndPathStruct pro_temp)
{
    /*************************************************************************
    **  [程序信息]
            主程序行数=1
            子程序1行数=1
            子程序2行数=1
            子程序3行数=1
            子程序4行数=1
            子程序5行数=1
            子程序6行数=1
            子程序7行数=1
            子程序8行数=1
        [参考点]
            参考点1有效性=0 参考点坐标轴1=0.00 参考点坐标轴2=0.00 参考点坐标轴3=0.00
            参考点1有效性=0 参考点坐标轴1=0.00 参考点坐标轴2=0.00 参考点坐标轴3=0.00
            100个参考点
        [主程序]
            命令行号=1 命令执行序号=1 命令=200=程序结束'注释标志=0='不注释' 延时=0.01...
        [子程序1]
            命令行号=1 命令执行序号=1 命令=200=程序结束'注释标志=0='不注释' 延时=0.01...
        [子程序2]
            命令行号=1 命令执行序号=1 命令=200=程序结束'注释标志=0='不注释' 延时=0.01...
        [子程序3]
            命令行号=1 命令执行序号=1 命令=200=程序结束'注释标志=0='不注释' 延时=0.01...
        [子程序4]
            命令行号=1 命令执行序号=1 命令=200=程序结束'注释标志=0='不注释' 延时=0.01...
        [子程序5]
            命令行号=1 命令执行序号=1 命令=200=程序结束'注释标志=0='不注释' 延时=0.01...
        [子程序6]
            命令行号=1 命令执行序号=1 命令=200=程序结束'注释标志=0='不注释' 延时=0.01...
        [子程序7]
            命令行号=1 命令执行序号=1 命令=200=程序结束'注释标志=0='不注释' 延时=0.01...
        [子程序8]
            命令行号=1 命令执行序号=1 命令=200=程序结束'注释标志=0='不注释' 延时=0.01...
        [主标签]
        [子标签1]
        [子标签2]
        [子标签3]
        [子标签4]
        [子标签5]
        [子标签6]
        [子标签7]
        [子标签8]
    **************************************************************************/
    QDir dir;
    if(!dir.exists(m_ProgramPath)){//如果没有当前目录，创建目录
        dir.mkdir(m_ProgramPath);
    }

    QFile file(pro_temp.filePath);
    if(!file.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Truncate))
        return false;
    QTextStream out(&file);
    out << fileSectionList[0] << lineFeed;
    for(int i=0;i<PRO_NUM;i++)
    {
        out<< doubleSpace << QString(programInfoList[i]).arg(QString::number(1))  << lineFeed;
    }
    out << fileSectionList[1] << lineFeed;
    for(int i=0;i<REFERENCE_TOTAL_NUM;i++)
    {
        out<< doubleSpace;
        for(int j=0;j<7;j++)
        {
            if(j==0)
            {
                out << QString(referencePointList[j]).arg(QString::number(i)).arg("0");
            }
            else
            {
                out << signalSpace << QString(referencePointList[j]).arg("0.00");
            }
        }
        out << lineFeed;
    }
    for(int i=0;i<PRO_NUM;i++)
    {
        out << fileSectionList[i+2] << lineFeed;
        out<< doubleSpace;
        out << QString(programBasicCmdList[0]).arg("1");
        out << signalSpace << QString(programBasicCmdList[1]).arg("1");
        out << signalSpace << QString(programBasicCmdList[2]).arg(C_PRO_END);
        out << signalSpace << QString(programBasicCmdList[3]).arg("0");
        out << signalSpace << QString(programBasicCmdList[4]).arg("0.00");
        out << lineFeed;
    }
    out << fileSectionList[11] << lineFeed;//变量
    for(int i=0;i<VAR_TOTAL_NUM;i++)
    {
        out << m_NameDefine[0].varName[i]<< "=0" << lineFeed;//采用默认变量名存储，新建文件或载入文件时，之改变变量的类型
    }
    for(int i=0;i<PRO_NUM;i++)
    {
        out << LablePartList[i] << lineFeed;
    }
    file.close();
    return true;
}
/*************************************************************************
**  函数名：  saveProgram(QString fileName)
**	输入参数：输入的文件
**	输出参数：是否保存成功
**	函数功能：保存程序（程序保存时触发）
**  作者：    PXC
**  开发日期：2024/7/26
**************************************************************************/
bool saveProgram(D_ProgramNameAndPathStruct pro_temp)
{
    QFile file(pro_temp.filePath);
    if(!file.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Truncate))
        return false;
    QTextStream out(&file);
    out << fileSectionList[0] << lineFeed;
    for(int i=0;i<PRO_NUM;i++)
    {
        out<< doubleSpace << QString(programInfoList[i]).arg(QString::number(m_ProInfo.proNum[i]))  << lineFeed;
    }
    out << fileSectionList[1] << lineFeed;
    for(int i=0;i<REFERENCE_TOTAL_NUM;i++)
    {
        out<< doubleSpace;
        for(int j=0;j<7;j++)
        {
            if(j==0)
            {
                out << QString(referencePointList[j]).arg(QString::number(i)).arg(QString::number(m_RefPoint[i].refFlag));
            }
            else
            {
                out << signalSpace << QString(referencePointList[j]).arg(QString::number(m_RefPoint[i].pos[j-1]*PRECISION_001, 'f', 2));
            }
        }
        out << lineFeed;
    }

    for(int i=0;i<PRO_NUM;i++)
    {
        out << fileSectionList[i+2] << lineFeed;
        for(int j=0;j<PRO_LINE_MAIN;j++)                //最多999条，并且检索到程序结束就跳出循环，不再往下进行检索了
        {
            //获取是否是程序结束
            uint8_t tempCmd = m_ProOrder[i][j].cmd;
            out<< doubleSpace;
            out << QString(programBasicCmdList[0]).arg(QString::number(m_ProOrder[i][j].list));
            out << signalSpace << QString(programBasicCmdList[1]).arg(QString::number(m_ProOrder[i][j].runOrderNum));
            out << signalSpace << QString(programBasicCmdList[2]).arg(QString::number(tempCmd));
            out << signalSpace << QString(programBasicCmdList[3]).arg(QString::number(m_ProOrder[i][j].noteFlag));
            out << signalSpace << QString(programBasicCmdList[4]).arg(QString::number(m_ProOrder[i][j].delay*PRECISION_001, 'f', 2));
            if(tempCmd==C_PRO_END)
            {
                break;
            }
            //添加非基本指令
            switch (tempCmd)
            {
            case C_AXIS_MOVE:
                out << signalSpace << QString(axisMoveCmdList[0]).arg(QString::number(((P_AxisMoveStruct*)m_ProOrder[i][j].pData)->pos*PRECISION_001, 'f', 2));
                out << signalSpace << QString(axisMoveCmdList[1]).arg(QString::number(((P_AxisMoveStruct*)m_ProOrder[i][j].pData)->advEndDis*PRECISION_001, 'f', 2));
                out << signalSpace << QString(axisMoveCmdList[2]).arg(QString::number(((P_AxisMoveStruct*)m_ProOrder[i][j].pData)->advCSpeedDis*PRECISION_001, 'f', 2));
                out << signalSpace << QString(axisMoveCmdList[3]).arg(QString::number(((P_AxisMoveStruct*)m_ProOrder[i][j].pData)->axis));
                out << signalSpace << QString(axisMoveCmdList[4]).arg(QString::number(((P_AxisMoveStruct*)m_ProOrder[i][j].pData)->speed));
                out << signalSpace << QString(axisMoveCmdList[5]).arg(QString::number(((P_AxisMoveStruct*)m_ProOrder[i][j].pData)->advEndFlag));
                out << signalSpace << QString(axisMoveCmdList[6]).arg(QString::number(((P_AxisMoveStruct*)m_ProOrder[i][j].pData)->advCSpeedFlag));
                out << signalSpace << QString(axisMoveCmdList[7]).arg(QString::number(((P_AxisMoveStruct*)m_ProOrder[i][j].pData)->advCSpeedSpeed));
                out << signalSpace << QString(axisMoveCmdList[8]).arg(QString::number(((P_AxisMoveStruct*)m_ProOrder[i][j].pData)->referPointNum));
                break;
            case C_CLAW_ACTION:
                out << signalSpace << QString(clawActionCmdList[0]).arg(QString::number(((P_ClawActionStruct*)m_ProOrder[i][j].pData)->outportNum));
                out << signalSpace << QString(clawActionCmdList[1]).arg(QString::number(((P_ClawActionStruct*)m_ProOrder[i][j].pData)->type));
                break;
            case C_CLAW_CHECK:
                out << signalSpace << QString(clawCheckCmdList[0]).arg(QString::number(((P_ClawCheckStruct*)m_ProOrder[i][j].pData)->outportNum));
                out << signalSpace << QString(clawCheckCmdList[1]).arg(QString::number(((P_ClawCheckStruct*)m_ProOrder[i][j].pData)->type));
                break;
            case C_RESERVE_CHECK:
                out << signalSpace << QString(reserveCheckCmdList[0]).arg(QString::number(((P_ReserveCheckStruct*)m_ProOrder[i][j].pData)->inportNum));
                out << signalSpace << QString(reserveCheckCmdList[1]).arg(QString::number(((P_ReserveCheckStruct*)m_ProOrder[i][j].pData)->type));
                break;
            case C_MACHINE_OUT:
                out << signalSpace << QString(machineOutCmdList[0]).arg(QString::number(((P_MachineOutStruct*)m_ProOrder[i][j].pData)->outportNum));
                out << signalSpace << QString(machineOutCmdList[1]).arg(QString::number(((P_MachineOutStruct*)m_ProOrder[i][j].pData)->type));
                break;
            case C_STACK_RESET_ZERO:
                out << signalSpace << QString(stackResetZeroCmdList[0]).arg(QString::number(((P_StackResetZeroStruct*)m_ProOrder[i][j].pData)->number));
                out << signalSpace << QString(stackResetZeroCmdList[1]).arg(QString::number(((P_StackResetZeroStruct*)m_ProOrder[i][j].pData)->stackNum));
                break;
            case C_STACK_MOVE:
                for(int m=0;m<STACK_AXIS_NUM;m++)
                {
                    out << signalSpace << QString(stackMoveCmdList[0]).arg(QString::number(m)).
                            arg(QString::number(((P_StackMoveStruct*)m_ProOrder[i][j].pData)->advEndDis[m]*PRECISION_001, 'f', 2));
                }
                for(int m=0;m<STACK_AXIS_NUM;m++)
                {
                    out << signalSpace << QString(stackMoveCmdList[1]).arg(QString::number(m)).
                            arg(QString::number(((P_StackMoveStruct*)m_ProOrder[i][j].pData)->advCSpeedDis[m]*PRECISION_001, 'f', 2));
                }
                out << signalSpace << QString(stackMoveCmdList[2]).arg(QString::number(((P_StackMoveStruct*)m_ProOrder[i][j].pData)->stackNum));
                for(int m=0;m<STACK_AXIS_NUM;m++)
                {
                    out << signalSpace << QString(stackMoveCmdList[3]).arg(QString::number(m)).
                            arg(QString::number(((P_StackMoveStruct*)m_ProOrder[i][j].pData)->advEndFlag[m]));
                }
                for(int m=0;m<STACK_AXIS_NUM;m++)
                {
                    out << signalSpace << QString(stackMoveCmdList[4]).arg(QString::number(m)).
                            arg(QString::number(((P_StackMoveStruct*)m_ProOrder[i][j].pData)->advCSpeedFlag[m]));
                }
                for(int m=0;m<STACK_AXIS_NUM;m++)
                {
                    out << signalSpace << QString(stackMoveCmdList[5]).arg(QString::number(m)).
                            arg(QString::number(((P_StackMoveStruct*)m_ProOrder[i][j].pData)->advCSpeedSpeed[m]));
                }
                break;
            case C_STACK_FOLLOW:
                for(int m=0;m<STACK_AXIS_NUM;m++)
                {
                    out << signalSpace << QString(stackFollowCmdList[0]).arg(QString::number(m)).
                            arg(QString::number(((P_StackFollowStruct*)m_ProOrder[i][j].pData)->advEndDis[m]*PRECISION_001, 'f', 2));
                }
                for(int m=0;m<STACK_AXIS_NUM;m++)
                {
                    out << signalSpace << QString(stackFollowCmdList[1]).arg(QString::number(m)).
                            arg(QString::number(((P_StackFollowStruct*)m_ProOrder[i][j].pData)->advCSpeedDis[m]*PRECISION_001, 'f', 2));
                }
                out << signalSpace << QString(stackFollowCmdList[2]).arg(QString::number(((P_StackFollowStruct*)m_ProOrder[i][j].pData)->stackNum));
                for(int m=0;m<STACK_AXIS_NUM;m++)
                {
                    out << signalSpace << QString(stackFollowCmdList[3]).arg(QString::number(m)).
                            arg(QString::number(((P_StackFollowStruct*)m_ProOrder[i][j].pData)->advEndFlag[m]));
                }
                for(int m=0;m<STACK_AXIS_NUM;m++)
                {
                    out << signalSpace << QString(stackFollowCmdList[4]).arg(QString::number(m)).
                            arg(QString::number(((P_StackFollowStruct*)m_ProOrder[i][j].pData)->advCSpeedFlag[m]));
                }
                for(int m=0;m<STACK_AXIS_NUM;m++)
                {
                    out << signalSpace << QString(stackFollowCmdList[5]).arg(QString::number(m)).
                            arg(QString::number(((P_StackFollowStruct*)m_ProOrder[i][j].pData)->advCSpeedSpeed[m]));
                }
                break;
            case C_RESERVE_OUT:
                out << signalSpace << QString(reserveOutCmdList[0]).arg(QString::number(((P_ReserveOutStruct*)m_ProOrder[i][j].pData)->interval));
                out << signalSpace << QString(reserveOutCmdList[1]).arg(QString::number(((P_ReserveOutStruct*)m_ProOrder[i][j].pData)->outportNum));
                out << signalSpace << QString(reserveOutCmdList[2]).arg(QString::number(((P_ReserveOutStruct*)m_ProOrder[i][j].pData)->function));
                out << signalSpace << QString(reserveOutCmdList[3]).arg(QString::number(((P_ReserveOutStruct*)m_ProOrder[i][j].pData)->type));
                break;
            case C_WAIT_IN_MACHINE:
                out << signalSpace << QString(waitInMachineCmdList[0]).arg(QString::number(((P_WaitInMachineStruct*)m_ProOrder[i][j].pData)->backListNum));
                out << signalSpace << QString(waitInMachineCmdList[1]).arg(QString::number(((P_WaitInMachineStruct*)m_ProOrder[i][j].pData)->inportNum));
                out << signalSpace << QString(waitInMachineCmdList[2]).arg(QString::number(((P_WaitInMachineStruct*)m_ProOrder[i][j].pData)->inportSta));
                out << signalSpace << QString(waitInMachineCmdList[3]).arg(QString::number(((P_WaitInMachineStruct*)m_ProOrder[i][j].pData)->type));
                out << signalSpace << QString(waitInMachineCmdList[4]).arg(QString::number(((P_WaitInMachineStruct*)m_ProOrder[i][j].pData)->label));
                break;
            case C_WAIT_IN_CLAW:
                out << signalSpace << QString(waitInClawCmdList[0]).arg(QString::number(((P_WaitInClawStruct*)m_ProOrder[i][j].pData)->backListNum));
                out << signalSpace << QString(waitInClawCmdList[1]).arg(QString::number(((P_WaitInClawStruct*)m_ProOrder[i][j].pData)->inportNum));
                out << signalSpace << QString(waitInClawCmdList[2]).arg(QString::number(((P_WaitInClawStruct*)m_ProOrder[i][j].pData)->inportSta));
                out << signalSpace << QString(waitInClawCmdList[3]).arg(QString::number(((P_WaitInClawStruct*)m_ProOrder[i][j].pData)->type));
                out << signalSpace << QString(waitInClawCmdList[4]).arg(QString::number(((P_WaitInClawStruct*)m_ProOrder[i][j].pData)->label));
                break;
            case C_WAIT_IN_RESERVE:
                out << signalSpace << QString(waitInReserveCmdList[0]).arg(QString::number(((P_WaitInReserveStruct*)m_ProOrder[i][j].pData)->backListNum));
                out << signalSpace << QString(waitInReserveCmdList[1]).arg(QString::number(((P_WaitInReserveStruct*)m_ProOrder[i][j].pData)->inportNum));
                out << signalSpace << QString(waitInReserveCmdList[2]).arg(QString::number(((P_WaitInReserveStruct*)m_ProOrder[i][j].pData)->inportSta));
                out << signalSpace << QString(waitInReserveCmdList[3]).arg(QString::number(((P_WaitInReserveStruct*)m_ProOrder[i][j].pData)->type));
                out << signalSpace << QString(waitInReserveCmdList[4]).arg(QString::number(((P_WaitInReserveStruct*)m_ProOrder[i][j].pData)->label));
                break;
            case C_OTHER_DELAY:

                break;
            case C_OTHER_ALARM_CUST:
                out << signalSpace << QString(otherAlarmCustCmdList[0]).arg(QString::number(((P_OtherAlarmCustStruct*)m_ProOrder[i][j].pData)->alarmNum));
                out << signalSpace << QString(otherAlarmCustCmdList[1]).arg(QString::number(((P_OtherAlarmCustStruct*)m_ProOrder[i][j].pData)->type));
                break;
            case C_OTHER_CYC_STOP:
                out << signalSpace << QString(otherCycStopCmdList[0]).arg(QString::number(((P_OtherCycStopStruct*)m_ProOrder[i][j].pData)->cycleNum));
                break;
            case C_OTHER_ALARM_LAMP:
                out << signalSpace << QString(other_alarm_lamp[0]).arg(QString::number(((P_OtherAlarmLampStruct*)m_ProOrder[i][j].pData)->outportNum));
                out << signalSpace << QString(other_alarm_lamp[1]).arg(QString::number(((P_OtherAlarmLampStruct*)m_ProOrder[i][j].pData)->type));
                break;
            case C_OTHER_ALARM_SOUND:
                out << signalSpace << QString(other_alarm_sound[0]).arg(QString::number(((P_OtherAlarmSoundStruct*)m_ProOrder[i][j].pData)->outportNum));
                out << signalSpace << QString(other_alarm_sound[1]).arg(QString::number(((P_OtherAlarmSoundStruct*)m_ProOrder[i][j].pData)->type));
                break;
            case C_LABEL:
                out << signalSpace << QString(labelCmdList[0]).arg(QString::number(((P_LabelStruct*)m_ProOrder[i][j].pData)->labelNum));
                out << signalSpace << QString(labelCmdList[1]).arg(QString::number(((P_LabelStruct*)m_ProOrder[i][j].pData)->type));
                break;
            case C_LOGIC_IF:
                for(int m=0;m<REQUIRE_TOTAL_NUM;m++)
                {
                    out << signalSpace << QString(logicIfCmdList[0]).arg(QString::number(m)).
                            arg(QString::number(((P_LogicIfStruct*)m_ProOrder[i][j].pData)->sufferCmpValue[m]));
                }
                for(int m=0;m<REQUIRE_TOTAL_NUM;m++)
                {
                    out << signalSpace << QString(logicIfCmdList[1]).arg(QString::number(m)).
                            arg(QString::number(((P_LogicIfStruct*)m_ProOrder[i][j].pData)->reqSelectFlag[m]));
                }
                for(int m=0;m<REQUIRE_TOTAL_NUM;m++)
                {
                    out << signalSpace << QString(logicIfCmdList[2]).arg(QString::number(m)).
                            arg(QString::number(((P_LogicIfStruct*)m_ProOrder[i][j].pData)->cmpType[m]));
                }
                for(int m=0;m<REQUIRE_TOTAL_NUM;m++)
                {
                    out << signalSpace << QString(logicIfCmdList[3]).arg(QString::number(m)).
                            arg(QString::number(((P_LogicIfStruct*)m_ProOrder[i][j].pData)->inportNum[m]));
                }
                for(int m=0;m<REQUIRE_TOTAL_NUM;m++)
                {
                    out << signalSpace << QString(logicIfCmdList[4]).arg(QString::number(m)).
                            arg(QString::number(((P_LogicIfStruct*)m_ProOrder[i][j].pData)->inportType[m]));
                }
                for(int m=0;m<REQUIRE_TOTAL_NUM;m++)
                {
                    out << signalSpace << QString(logicIfCmdList[5]).arg(QString::number(m)).
                            arg(QString::number(((P_LogicIfStruct*)m_ProOrder[i][j].pData)->cmpMode[m]));
                }
                for(int m=0;m<REQUIRE_TOTAL_NUM;m++)
                {
                    out << signalSpace << QString(logicIfCmdList[6]).arg(QString::number(m)).
                            arg(QString::number(((P_LogicIfStruct*)m_ProOrder[i][j].pData)->sufferCmpType[m]));
                }
                break;
            case C_LOGIC_ELSE:

                break;
            case C_LOGIC_END:

                break;
            case C_LOGIC_WHILE_START:
                out << signalSpace << QString(logicWhileStartCmdList[0]).arg(QString::number(((P_LogicWhileStartStruct*)m_ProOrder[i][j].pData)->cycNum));
                break;
            case C_LOGIC_WHILE_END:

                break;
            case C_LOGIC_VAR:
                out << signalSpace << QString(logicVarCmdList[0]).arg(QString::number(((P_LogicVarStruct*)m_ProOrder[i][j].pData)->sufferOperValue));
                out << signalSpace << QString(logicVarCmdList[1]).arg(QString::number(((P_LogicVarStruct*)m_ProOrder[i][j].pData)->varNum));
                out << signalSpace << QString(logicVarCmdList[2]).arg(QString::number(((P_LogicVarStruct*)m_ProOrder[i][j].pData)->operMode));
                out << signalSpace << QString(logicVarCmdList[3]).arg(QString::number(((P_LogicVarStruct*)m_ProOrder[i][j].pData)->sufferOperType));
                break;
            case C_LOGIC_AXIS:
                out << signalSpace << QString(logicAxisCmdList[0]).arg(QString::number(((P_LogicAxisStruct*)m_ProOrder[i][j].pData)->sufferOperValue));
                out << signalSpace << QString(logicAxisCmdList[1]).arg(QString::number(((P_LogicAxisStruct*)m_ProOrder[i][j].pData)->axisNum));
                out << signalSpace << QString(logicAxisCmdList[2]).arg(QString::number(((P_LogicAxisStruct*)m_ProOrder[i][j].pData)->operMode));
                out << signalSpace << QString(logicAxisCmdList[3]).arg(QString::number(((P_LogicAxisStruct*)m_ProOrder[i][j].pData)->sufferOperType));
                break;
            case C_LOGIC_STACK:
                out << signalSpace << QString(logicStackCmdList[0]).arg(QString::number(((P_LogicStackStruct*)m_ProOrder[i][j].pData)->sufferOperValue));
                out << signalSpace << QString(logicStackCmdList[1]).arg(QString::number(((P_LogicStackStruct*)m_ProOrder[i][j].pData)->stackNum));
                out << signalSpace << QString(logicStackCmdList[2]).arg(QString::number(((P_LogicStackStruct*)m_ProOrder[i][j].pData)->operMode));
                out << signalSpace << QString(logicStackCmdList[3]).arg(QString::number(((P_LogicStackStruct*)m_ProOrder[i][j].pData)->sufferOperType));
                break;
            case C_LOGIC_PRODUCT:
                out << signalSpace << QString(logicCurProductNumCmdList[0]).arg(QString::number(((P_LogicCurProductNumStruct*)m_ProOrder[i][j].pData)->sufferOperValue));
                out << signalSpace << QString(logicCurProductNumCmdList[1]).arg(QString::number(((P_LogicCurProductNumStruct*)m_ProOrder[i][j].pData)->productNum));
                out << signalSpace << QString(logicCurProductNumCmdList[2]).arg(QString::number(((P_LogicCurProductNumStruct*)m_ProOrder[i][j].pData)->operMode));
                out << signalSpace << QString(logicCurProductNumCmdList[3]).arg(QString::number(((P_LogicCurProductNumStruct*)m_ProOrder[i][j].pData)->sufferOperType));
                break;
            case C_LOGIC_TIME:
                out << signalSpace << QString(logicTimeCmdList[0]).arg(QString::number(((P_LogicTimeStruct*)m_ProOrder[i][j].pData)->timeNum));
                out << signalSpace << QString(logicTimeCmdList[1]).arg(QString::number(((P_LogicTimeStruct*)m_ProOrder[i][j].pData)->operMode));
                break;
            case C_SEARCH_STOP:

                break;
            case C_SEARCH_AXIS_MOVE:
                out << signalSpace << QString(searchAxisMoveCmdList[0]).arg(QString::number(((P_SearchAxisMoveStruct*)m_ProOrder[i][j].pData)->maxPos*PRECISION_001, 'f', 2));
                out << signalSpace << QString(searchAxisMoveCmdList[1]).arg(QString::number(((P_SearchAxisMoveStruct*)m_ProOrder[i][j].pData)->advCDis*PRECISION_001, 'f', 2));
                out << signalSpace << QString(searchAxisMoveCmdList[2]).arg(QString::number(((P_SearchAxisMoveStruct*)m_ProOrder[i][j].pData)->offsetDis*PRECISION_001, 'f', 2));
                out << signalSpace << QString(searchAxisMoveCmdList[3]).arg(QString::number(((P_SearchAxisMoveStruct*)m_ProOrder[i][j].pData)->axis));
                out << signalSpace << QString(searchAxisMoveCmdList[4]).arg(QString::number(((P_SearchAxisMoveStruct*)m_ProOrder[i][j].pData)->posStoreFlag));
                out << signalSpace << QString(searchAxisMoveCmdList[5]).arg(QString::number(((P_SearchAxisMoveStruct*)m_ProOrder[i][j].pData)->reachPosAlarmFlag));
                out << signalSpace << QString(searchAxisMoveCmdList[6]).arg(QString::number(((P_SearchAxisMoveStruct*)m_ProOrder[i][j].pData)->runSpeed));
                out << signalSpace << QString(searchAxisMoveCmdList[7]).arg(QString::number(((P_SearchAxisMoveStruct*)m_ProOrder[i][j].pData)->advCSpeed));
                out << signalSpace << QString(searchAxisMoveCmdList[8]).arg(QString::number(((P_SearchAxisMoveStruct*)m_ProOrder[i][j].pData)->searchNum));
                for(int m=0;m<SEARCH_INPORT_TOTAL_NUM;m++)
                {
                    out << signalSpace << QString(searchAxisMoveCmdList[9]).arg(QString::number(m)).
                            arg(QString::number(((P_SearchAxisMoveStruct*)m_ProOrder[i][j].pData)->inportNum[m]));
                }
                for(int m=0;m<SEARCH_INPORT_TOTAL_NUM;m++)
                {
                    out << signalSpace << QString(searchAxisMoveCmdList[10]).arg(QString::number(m)).
                            arg(QString::number(((P_SearchAxisMoveStruct*)m_ProOrder[i][j].pData)->inporttype[m]));
                }
                break;
            case C_OFFSET_AXIS:
                out << signalSpace << QString(offsetAxisCmdList[0]).arg(QString::number(((P_OffsetAxisStruct*)m_ProOrder[i][j].pData)->offsetPos*PRECISION_001, 'f', 2));
                out << signalSpace << QString(offsetAxisCmdList[1]).arg(QString::number(((P_OffsetAxisStruct*)m_ProOrder[i][j].pData)->advEndDis*PRECISION_001, 'f', 2));
                out << signalSpace << QString(offsetAxisCmdList[2]).arg(QString::number(((P_OffsetAxisStruct*)m_ProOrder[i][j].pData)->advCSpeedDis*PRECISION_001, 'f', 2));
                out << signalSpace << QString(offsetAxisCmdList[3]).arg(QString::number(((P_OffsetAxisStruct*)m_ProOrder[i][j].pData)->axis));
                out << signalSpace << QString(offsetAxisCmdList[4]).arg(QString::number(((P_OffsetAxisStruct*)m_ProOrder[i][j].pData)->speed));
                out << signalSpace << QString(offsetAxisCmdList[5]).arg(QString::number(((P_OffsetAxisStruct*)m_ProOrder[i][j].pData)->advEndFlag));
                out << signalSpace << QString(offsetAxisCmdList[6]).arg(QString::number(((P_OffsetAxisStruct*)m_ProOrder[i][j].pData)->advCSpeedFlag));
                out << signalSpace << QString(offsetAxisCmdList[7]).arg(QString::number(((P_OffsetAxisStruct*)m_ProOrder[i][j].pData)->advCSpeedSpeed));
                break;
            case C_TORQUE_GARD:
                out << signalSpace << QString(torqueGardCmdList[0]).arg(QString::number(((P_TorqueGardStruct*)m_ProOrder[i][j].pData)->axis));
                out << signalSpace << QString(torqueGardCmdList[1]).arg(QString::number(((P_TorqueGardStruct*)m_ProOrder[i][j].pData)->torqueValue));
                break;
            case C_AXIS_STOP:
                out << signalSpace << QString(axisStopCmdList[0]).arg(QString::number(((P_AxisStopStruct*)m_ProOrder[i][j].pData)->axis));
                break;
            case C_SUN_PRO:
                out << signalSpace << QString(sunProCmdList[0]).arg(QString::number(((P_SunProStruct*)m_ProOrder[i][j].pData)->sunProNum));
                out << signalSpace << QString(sunProCmdList[1]).arg(QString::number(((P_SunProStruct*)m_ProOrder[i][j].pData)->oprMode));
                break;
            default:
                break;

            }
            out << lineFeed;

        }
        out << lineFeed;
    }
    //变量类型存储
    out << fileSectionList[11] << lineFeed;
    for(int i=0;i<VAR_TOTAL_NUM;i++)
    {
        out << m_NameDefine[0].varName[i] << "=" << m_VariableType[i] << lineFeed;//保存变量小数类型
    }
    LableNameList[m_OperateProNum] = CurrentLableNameList;
    for(int i=0;i<PRO_NUM;i++)
    {//存储标签指令名称
        out << LablePartList[i] << lineFeed;
        for(int j=0;j<LableNameList[i].count();j++)
        {
            out<< doubleSpace;
            out<< LableNameList[i][j] << lineFeed;
        }
    }
    out << lineFeed;
    file.close();

    return true;
}
/*************************************************************************
**  函数名：  readLableOrderName()
**	输入参数：文件路径
**	输出参数：是否读取成功
**	函数功能：读取程序中标签指令名称
**  作者：    wukui
**  开发日期：2024/9/22
**************************************************************************/
bool readLableOrderName(D_ProgramNameAndPathStruct pro_temp)
{
    QFile file(pro_temp.filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
       // 处理文件打开失败的情况
       return false;
    }
    uint8_t intFlag = 0;
    QStringList LableNameList_Temp[PRO_NUM];
    QTextStream in(&file);
    for(uint8_t i=0;i<PRO_NUM;i++)
    {
        LableNameList[i].clear();//先清空列表
    }
    while(!in.atEnd())
    {
        QString line=in.readLine();
        if(line.compare(LablePartList[0])==0)            //主标签
        {
            intFlag = 1;
            continue;
        }
        else if(line.compare(LablePartList[1])==0)            //子标签1
        {
            intFlag = 2;
            continue;
        }
        else if(line.compare(LablePartList[2])==0)            //子标签2
        {
            intFlag = 3;
            continue;
        }
        else if(line.compare(LablePartList[3])==0)            //子标签3
        {
            intFlag = 4;
            continue;
        }
        else if(line.compare(LablePartList[4])==0)            //子标签4
        {
            intFlag = 5;
            continue;
        }
        else if(line.compare(LablePartList[5])==0)            //子标签5
        {
            intFlag = 6;
            continue;
        }
        else if(line.compare(LablePartList[6])==0)            //子标签6
        {
            intFlag = 7;
            continue;
        }
        else if(line.compare(LablePartList[7])==0)            //子标签7
        {
            intFlag = 8;
            continue;
        }
        else if(line.compare(LablePartList[8])==0)            //子标签8
        {
            intFlag = 9;
            continue;
        }
        else if(line==""||line==NULL)
        {
            continue;
        }

        switch (intFlag)
        {
        case 1:
            LableNameList_Temp[0].append(line);
        break;
        case 2:
            LableNameList_Temp[1].append(line);
        break;
        case 3:
            LableNameList_Temp[2].append(line);
        break;
        case 4:
            LableNameList_Temp[3].append(line);
        break;
        case 5:
            LableNameList_Temp[4].append(line);
        break;
        case 6:
            LableNameList_Temp[5].append(line);
        break;
        case 7:
            LableNameList_Temp[6].append(line);
        break;
        case 8:
            LableNameList_Temp[7].append(line);
        break;
        case 9:
            LableNameList_Temp[8].append(line);
        break;
        default:
        break;
        }
    }
    for(int i=0;i<PRO_NUM;i++)
    {
        for(int j=0;j<LableNameList_Temp[i].count();j++)
        {
            QStringList tempList=LableNameList_Temp[i][j].split(signalSpace);
            tempList.removeAt(0);
            tempList.removeAt(0);
            LableNameList[i].append(tempList[0]+ " " +tempList[1]);
        }
    }
    CurrentLableNameList.clear();
    CurrentLableNameList = LableNameList[m_OperateProNum];
}
/*************************************************************************
**  函数名：  readProgram()
**	输入参数：文件路径
**	输出参数：是否读取成功
**	函数功能：读文件第一行数据（读所有程序）
**  作者：    PXC
**  开发日期：2024/7/26
**************************************************************************/
#if 1
bool readProgram(D_ProgramNameAndPathStruct pro_temp)
{
    QFile file(pro_temp.filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
       // 处理文件打开失败的情况
       return false;
    }
    QTextStream in(&file);
    QStringList programInfoList;
    QStringList refList;
    QStringList P_List[9];
    QStringList Var_List;

    int intFlag=0;              //设置一个标志位，区分当前读到什么程序了
    while(!in.atEnd())
    {
        QString line=in.readLine();
        if(line.compare(fileSectionList[0])==0)                 //程序行数
        {
            for(int i=0;i<PRO_NUM;i++)
            {
                programInfoList.append(in.readLine());
            }
        }
        else if(line.compare(fileSectionList[1])==0)            //参考点
        {
            for(int i=0;i<REFERENCE_TOTAL_NUM;i++)
            {
                refList.append(in.readLine());
            }
        }
        else if(line.compare(fileSectionList[2])==0)            //主程序
        {
            intFlag=2;
            continue;
        }
        else if(line.compare(fileSectionList[3])==0)            //子程序1
        {
            intFlag=3;
            continue;
        }
        else if(line.compare(fileSectionList[4])==0)            //子程序2
        {
            intFlag=4;
            continue;
        }
        else if(line.compare(fileSectionList[5])==0)            //子程序3
        {
            intFlag=5;
            continue;
        }
        else if(line.compare(fileSectionList[6])==0)            //子程序4
        {
            intFlag=6;
            continue;
        }
        else if(line.compare(fileSectionList[7])==0)            //子程序5
        {
            intFlag=7;
            continue;
        }
        else if(line.compare(fileSectionList[8])==0)            //子程序6
        {
            intFlag=8;
            continue;
        }
        else if(line.compare(fileSectionList[9])==0)            //子程序7
        {
            intFlag=9;
            continue;
        }
        else if(line.compare(fileSectionList[10])==0)            //子程序8
        {
            intFlag=10;
            continue;
        }
        else if(line.compare(fileSectionList[11])==0)
        {
            intFlag=11;
            continue;
        }
        else if(line.compare(LablePartList[0])==0 || line.compare(LablePartList[1])==0||line.compare(LablePartList[2])==0||line.compare(LablePartList[3])==0
                ||line.compare(LablePartList[4])==0 || line.compare(LablePartList[5])==0||line.compare(LablePartList[6])==0||line.compare(LablePartList[7])==0)
        {//标签部分暂不读取
            intFlag = 0;
            continue;
        }
        else if(line==""||line==NULL)
        {
            continue;
        }

        switch (intFlag)
        {
        case 2:
            P_List[0].append(line);
        break;
        case 3:
            P_List[1].append(line);
        break;
        case 4:
            P_List[2].append(line);
        break;
        case 5:
            P_List[3].append(line);
        break;
        case 6:
            P_List[4].append(line);
        break;
        case 7:
            P_List[5].append(line);
        break;
        case 8:
            P_List[6].append(line);
        break;
        case 9:
            P_List[7].append(line);
        break;
        case 10:
            P_List[8].append(line);
        break;
        case 11:
            Var_List.append(line);
            break;
        default:
        break;
        }

    }
    file.close();

    //开始解析
    for(int i=0;i<PRO_NUM;i++)                          //行号
    {
        m_ProInfo.proNum[i]=(uint16_t)(programInfoList[i].mid(programInfoList[i].indexOf("=")+1).toUInt());
    }
    for(int i=0;i<REFERENCE_TOTAL_NUM;i++)              //参考点
    {
        QStringList refTempList=refList[i].split(signalSpace);
        m_RefPoint[i].refFlag=(uint8_t)(refTempList[0].mid(refTempList[0].indexOf("=")+1).toUInt());
        for(int j=0;j<AXIS_TOTAL_NUM;j++)
        {
            m_RefPoint[i].pos[j]=(int32_t)(refTempList[j+1].mid(refTempList[j+1].indexOf("=")+1).toDouble()/PRECISION_001);
        }
    }
    for(int i=0;i<9;i++)
    {
        for(int j=0;j<P_List[i].count();j++)
        {
            QStringList tempList=P_List[i][j].split(signalSpace);
            tempList.removeAt(0);
            tempList.removeAt(0);
            uint16_t index=0;
            m_ProOrder[i][j].list=(uint16_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            m_ProOrder[i][j].runOrderNum=(uint16_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            m_ProOrder[i][j].cmd=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            m_ProOrder[i][j].noteFlag=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            m_ProOrder[i][j].delay=(uint16_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toDouble()/PRECISION_001);
            index++;

            switch (m_ProOrder[i][j].cmd)
            {
            case C_AXIS_MOVE:
                P_AxisMoveStruct temp_C_AXIS_MOVE;
                temp_C_AXIS_MOVE.pos=(int32_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toDouble()/PRECISION_001);
                index++;
                temp_C_AXIS_MOVE.advEndDis=(int32_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toDouble()/PRECISION_001);
                index++;
                temp_C_AXIS_MOVE.advCSpeedDis=(int32_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toDouble()/PRECISION_001);
                index++;
                temp_C_AXIS_MOVE.axis=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
                temp_C_AXIS_MOVE.speed=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
                temp_C_AXIS_MOVE.advEndFlag=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
                temp_C_AXIS_MOVE.advCSpeedFlag=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
                temp_C_AXIS_MOVE.advCSpeedSpeed=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
                temp_C_AXIS_MOVE.referPointNum=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                for(int m=0;m<2;m++)
                {
                    temp_C_AXIS_MOVE.ret[m]=0;
                }
                m_ProOrder[i][j].pData = (void *)malloc(sizeof(P_AxisMoveStruct));
                memcpy(m_ProOrder[i][j].pData,&temp_C_AXIS_MOVE,sizeof(P_AxisMoveStruct));
                break;

            case C_CLAW_ACTION:
                P_ClawActionStruct temp_C_CLAW_ACTION;
                temp_C_CLAW_ACTION.outportNum=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
                temp_C_CLAW_ACTION.type=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                for(int m=0;m<2;m++)
                {
                    temp_C_CLAW_ACTION.ret[m]=0;
                }
                m_ProOrder[i][j].pData = (void *)malloc(sizeof(P_ClawActionStruct));
                memcpy(m_ProOrder[i][j].pData,&temp_C_CLAW_ACTION,sizeof(P_ClawActionStruct));
                break;
            case C_CLAW_CHECK:
                P_ClawCheckStruct temp_C_CLAW_CHECK;
                temp_C_CLAW_CHECK.outportNum=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
                temp_C_CLAW_CHECK.type=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                for(int m=0;m<2;m++)
                {
                    temp_C_CLAW_CHECK.ret[m]=0;
                }
                m_ProOrder[i][j].pData = (void *)malloc(sizeof(P_ClawCheckStruct));
                memcpy(m_ProOrder[i][j].pData,&temp_C_CLAW_CHECK,sizeof(P_ClawCheckStruct));
                break;

            case C_RESERVE_CHECK:
                P_ReserveCheckStruct temp_C_RESERVE_CHECK;
                temp_C_RESERVE_CHECK.inportNum=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
                temp_C_RESERVE_CHECK.type=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                for(int m=0;m<2;m++)
                {
                    temp_C_RESERVE_CHECK.ret[m]=0;
                }
                m_ProOrder[i][j].pData = (void *)malloc(sizeof(P_ReserveCheckStruct));
                memcpy(m_ProOrder[i][j].pData,&temp_C_RESERVE_CHECK,sizeof(P_ReserveCheckStruct));
                break;
            case C_MACHINE_OUT:
                P_MachineOutStruct temp_C_MACHINE_OUT;
                temp_C_MACHINE_OUT.outportNum=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
                temp_C_MACHINE_OUT.type=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                for(int m=0;m<2;m++)
                {
                    temp_C_MACHINE_OUT.ret[m]=0;
                }
                m_ProOrder[i][j].pData = (void *)malloc(sizeof(P_MachineOutStruct));
                memcpy(m_ProOrder[i][j].pData,&temp_C_MACHINE_OUT,sizeof(P_MachineOutStruct));
                break;
            case C_STACK_RESET_ZERO:
                P_StackResetZeroStruct temp_C_STACK_RESET_ZERO;
                temp_C_STACK_RESET_ZERO.number=(uint16_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
                temp_C_STACK_RESET_ZERO.stackNum=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                for(int m=0;m<1;m++)
                {
                    temp_C_STACK_RESET_ZERO.ret[m]=0;
                }
                m_ProOrder[i][j].pData = (void *)malloc(sizeof(P_StackResetZeroStruct));
                memcpy(m_ProOrder[i][j].pData,&temp_C_STACK_RESET_ZERO,sizeof(P_StackResetZeroStruct));
                break;
            case C_STACK_MOVE:
                P_StackMoveStruct temp_C_STACK_MOVE;
                for(int m=0;m<STACK_AXIS_NUM;m++)
                {
                    temp_C_STACK_MOVE.advEndDis[m]=(int32_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toDouble()/PRECISION_001);
                    index++;
                }
                for(int m=0;m<STACK_AXIS_NUM;m++)
                {
                    temp_C_STACK_MOVE.advCSpeedDis[m]=(int32_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toDouble()/PRECISION_001);
                    index++;
                }
                temp_C_STACK_MOVE.stackNum=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
                for(int m=0;m<3;m++)
                {
                    temp_C_STACK_MOVE.ret1[m]=0;
                }
                for(int m=0;m<STACK_AXIS_NUM;m++)
                {
                    temp_C_STACK_MOVE.advEndFlag[m]=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                    index++;
                }
                for(int m=0;m<1;m++)
                {
                    temp_C_STACK_MOVE.ret2[m]=0;
                }
                for(int m=0;m<STACK_AXIS_NUM;m++)
                {
                    temp_C_STACK_MOVE.advCSpeedFlag[m]=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                    index++;
                }
                for(int m=0;m<1;m++)
                {
                    temp_C_STACK_MOVE.ret3[m]=0;
                }
                for(int m=0;m<STACK_AXIS_NUM;m++)
                {
                    temp_C_STACK_MOVE.advCSpeedSpeed[m]=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                    index++;
                }
                for(int m=0;m<1;m++)
                {
                    temp_C_STACK_MOVE.ret4[m]=0;
                }
                m_ProOrder[i][j].pData = (void *)malloc(sizeof(P_StackMoveStruct));
                memcpy(m_ProOrder[i][j].pData,&temp_C_STACK_MOVE,sizeof(P_StackMoveStruct));
                break;
            case C_STACK_FOLLOW:
                P_StackFollowStruct temp_C_STACK_FOLLOW;
                for(int m=0;m<STACK_AXIS_NUM;m++)
                {
                    temp_C_STACK_FOLLOW.advEndDis[m]=(int32_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toDouble()/PRECISION_001);
                    index++;
                }
                for(int m=0;m<STACK_AXIS_NUM;m++)
                {
                    temp_C_STACK_FOLLOW.advCSpeedDis[m]=(int32_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toDouble()/PRECISION_001);
                    index++;
                }
                temp_C_STACK_FOLLOW.stackNum=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
                for(int m=0;m<3;m++)
                {
                    temp_C_STACK_FOLLOW.ret1[m]=0;
                }
                for(int m=0;m<STACK_AXIS_NUM;m++)
                {
                    temp_C_STACK_FOLLOW.advEndFlag[m]=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                    index++;
                }
                for(int m=0;m<1;m++)
                {
                    temp_C_STACK_FOLLOW.ret2[m]=0;
                }
                for(int m=0;m<STACK_AXIS_NUM;m++)
                {
                    temp_C_STACK_FOLLOW.advCSpeedFlag[m]=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                    index++;
                }
                for(int m=0;m<1;m++)
                {
                    temp_C_STACK_FOLLOW.ret3[m]=0;
                }
                for(int m=0;m<STACK_AXIS_NUM;m++)
                {
                    temp_C_STACK_FOLLOW.advCSpeedSpeed[m]=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                    index++;
                }
                for(int m=0;m<1;m++)
                {
                    temp_C_STACK_FOLLOW.ret4[m]=0;
                }
                m_ProOrder[i][j].pData = (void *)malloc(sizeof(P_StackFollowStruct));
                memcpy(m_ProOrder[i][j].pData,&temp_C_STACK_FOLLOW,sizeof(P_StackFollowStruct));
                break;
            case C_RESERVE_OUT:
                P_ReserveOutStruct temp_C_RESERVE_OUT;
                temp_C_RESERVE_OUT.interval=(uint16_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
                temp_C_RESERVE_OUT.outportNum=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
                temp_C_RESERVE_OUT.function=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
                temp_C_RESERVE_OUT.type=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                for(int m=0;m<3;m++)
                {
                    temp_C_RESERVE_OUT.ret[m]=0;
                }
                m_ProOrder[i][j].pData = (void *)malloc(sizeof(P_ReserveOutStruct));
                memcpy(m_ProOrder[i][j].pData,&temp_C_RESERVE_OUT,sizeof(P_ReserveOutStruct));

                break;
            case C_WAIT_IN_MACHINE:
                P_WaitInMachineStruct temp_C_WAIT_IN_MACHINE;
                temp_C_WAIT_IN_MACHINE.backListNum=(uint16_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
                temp_C_WAIT_IN_MACHINE.inportNum=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
                temp_C_WAIT_IN_MACHINE.inportSta=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
                temp_C_WAIT_IN_MACHINE.type=(uint16_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
                temp_C_WAIT_IN_MACHINE.label=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                for(int m=0;m<2;m++)
                {
                    temp_C_WAIT_IN_MACHINE.ret[m]=0;
                }
                m_ProOrder[i][j].pData = (void *)malloc(sizeof(P_WaitInMachineStruct));
                memcpy(m_ProOrder[i][j].pData,&temp_C_WAIT_IN_MACHINE,sizeof(P_WaitInMachineStruct));
                break;
            case C_WAIT_IN_CLAW:
                P_WaitInClawStruct temp_C_WAIT_IN_CLAW;
                temp_C_WAIT_IN_CLAW.backListNum=(uint16_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
                temp_C_WAIT_IN_CLAW.inportNum=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
                temp_C_WAIT_IN_CLAW.inportSta=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
                temp_C_WAIT_IN_CLAW.type=(uint16_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
                temp_C_WAIT_IN_CLAW.label=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                for(int m=0;m<3;m++)
                {
                    temp_C_WAIT_IN_CLAW.ret[m]=0;
                }
                m_ProOrder[i][j].pData = (void *)malloc(sizeof(P_WaitInClawStruct));
                memcpy(m_ProOrder[i][j].pData,&temp_C_WAIT_IN_CLAW,sizeof(P_WaitInClawStruct));
                break;
            case C_WAIT_IN_RESERVE:
                P_WaitInReserveStruct temp_C_WAIT_IN_RESERVE;
                temp_C_WAIT_IN_RESERVE.backListNum=(uint16_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
                temp_C_WAIT_IN_RESERVE.inportNum=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
                temp_C_WAIT_IN_RESERVE.inportSta=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
                temp_C_WAIT_IN_RESERVE.type=(uint16_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
                temp_C_WAIT_IN_RESERVE.label=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                for(int m=0;m<3;m++)
                {
                    temp_C_WAIT_IN_RESERVE.ret[m]=0;
                }
                m_ProOrder[i][j].pData = (void *)malloc(sizeof(P_WaitInReserveStruct));
                memcpy(m_ProOrder[i][j].pData,&temp_C_WAIT_IN_RESERVE,sizeof(P_WaitInReserveStruct));
                break;
            case C_OTHER_DELAY:
                P_OtherDelayStruct temp_C_OTHER_DELAY;
                for(int m=0;m<4;m++)
                {
                    temp_C_OTHER_DELAY.res[m]=0;
                }
                m_ProOrder[i][j].pData = (void *)malloc(sizeof(P_OtherDelayStruct));
                memcpy(m_ProOrder[i][j].pData,&temp_C_OTHER_DELAY,sizeof(P_OtherDelayStruct));
                break;
            case C_OTHER_ALARM_CUST:
                P_OtherAlarmCustStruct temp_C_OTHER_ALARM_CUST;
                temp_C_OTHER_ALARM_CUST.alarmNum=(uint16_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
                temp_C_OTHER_ALARM_CUST.type=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                for(int m=0;m<1;m++)
                {
                    temp_C_OTHER_ALARM_CUST.ret[m]=0;
                }
                m_ProOrder[i][j].pData = (void *)malloc(sizeof(P_OtherAlarmCustStruct));
                memcpy(m_ProOrder[i][j].pData,&temp_C_OTHER_ALARM_CUST,sizeof(P_OtherAlarmCustStruct));
                break;
            case C_OTHER_CYC_STOP:
                P_OtherCycStopStruct temp_C_OTHER_CYC_STOP;
                temp_C_OTHER_CYC_STOP.cycleNum=(uint16_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                for(int m=0;m<2;m++)
                {
                    temp_C_OTHER_CYC_STOP.ret[m]=0;
                }
                m_ProOrder[i][j].pData = (void *)malloc(sizeof(P_OtherCycStopStruct));
                memcpy(m_ProOrder[i][j].pData,&temp_C_OTHER_CYC_STOP,sizeof(P_OtherCycStopStruct));
                break;
            case C_OTHER_ALARM_LAMP:
                P_OtherAlarmLampStruct temp_C_OTHER_ALARM_LAMP;
                temp_C_OTHER_ALARM_LAMP.outportNum=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
                temp_C_OTHER_ALARM_LAMP.type=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                for(int m=0;m<2;m++)
                {
                    temp_C_OTHER_ALARM_LAMP.ret[m]=0;
                }
                m_ProOrder[i][j].pData = (void *)malloc(sizeof(P_OtherAlarmLampStruct));
                memcpy(m_ProOrder[i][j].pData,&temp_C_OTHER_ALARM_LAMP,sizeof(P_OtherAlarmLampStruct));
                break;
            case C_OTHER_ALARM_SOUND:
                P_OtherAlarmSoundStruct temp_C_OTHER_ALARM_SOUND;
                temp_C_OTHER_ALARM_SOUND.outportNum=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
                temp_C_OTHER_ALARM_SOUND.type=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                for(int m=0;m<2;m++)
                {
                    temp_C_OTHER_ALARM_SOUND.ret[m]=0;
                }
                m_ProOrder[i][j].pData = (void *)malloc(sizeof(P_OtherAlarmSoundStruct));
                memcpy(m_ProOrder[i][j].pData,&temp_C_OTHER_ALARM_SOUND,sizeof(P_OtherAlarmSoundStruct));
                break;
            case C_LABEL:
                P_LabelStruct temp_C_LABEL;
                temp_C_LABEL.labelNum=(uint16_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
                temp_C_LABEL.type=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                for(int m=0;m<2;m++)
                {
                    temp_C_LABEL.ret[m]=0;
                }
                m_ProOrder[i][j].pData = (void *)malloc(sizeof(P_LabelStruct));
                memcpy(m_ProOrder[i][j].pData,&temp_C_LABEL,sizeof(P_LabelStruct));
                break;
            case C_LOGIC_IF:
                P_LogicIfStruct temp_C_LOGIC_IF;
                for(int m=0;m<REQUIRE_TOTAL_NUM;m++)
                {
                    temp_C_LOGIC_IF.sufferCmpValue[m]=(int32_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toDouble());
                    index++;
                }
                for(int m=0;m<REQUIRE_TOTAL_NUM;m++)
                {
                    temp_C_LOGIC_IF.reqSelectFlag[m]=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                    index++;
                }
                for(int m=0;m<REQUIRE_TOTAL_NUM;m++)
                {
                    temp_C_LOGIC_IF.cmpType[m]=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                    index++;
                }
                for(int m=0;m<REQUIRE_TOTAL_NUM;m++)
                {
                    temp_C_LOGIC_IF.inportNum[m]=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                    index++;
                }
                for(int m=0;m<REQUIRE_TOTAL_NUM;m++)
                {
                    temp_C_LOGIC_IF.inportType[m]=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                    index++;
                }
                for(int m=0;m<REQUIRE_TOTAL_NUM;m++)
                {
                    temp_C_LOGIC_IF.cmpMode[m]=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                    index++;
                }
                for(int m=0;m<REQUIRE_TOTAL_NUM;m++)
                {
                    temp_C_LOGIC_IF.sufferCmpType[m]=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                    index++;
                }
                m_ProOrder[i][j].pData = (void *)malloc(sizeof(P_LogicIfStruct));
                memcpy(m_ProOrder[i][j].pData,&temp_C_LOGIC_IF,sizeof(P_LogicIfStruct));
                break;
            case C_LOGIC_ELSE:
                P_LogicElseStruct temp_C_LOGIC_ELSE;
                for(int m=0;m<4;m++)
                {
                    temp_C_LOGIC_ELSE.res[m]=0;
                }
                m_ProOrder[i][j].pData = (void *)malloc(sizeof(P_LogicElseStruct));
                memcpy(m_ProOrder[i][j].pData,&temp_C_LOGIC_ELSE,sizeof(P_LogicElseStruct));
                break;
            case C_LOGIC_END:
                P_LogicEndStruct temp_C_LOGIC_END;
                for(int m=0;m<4;m++)
                {
                    temp_C_LOGIC_END.res[m]=0;
                }
                m_ProOrder[i][j].pData = (void *)malloc(sizeof(P_LogicEndStruct));
                memcpy(m_ProOrder[i][j].pData,&temp_C_LOGIC_END,sizeof(P_LogicEndStruct));
                break;
            case C_LOGIC_WHILE_START:
                P_LogicWhileStartStruct temp_C_LOGIC_WHILE_START;
                temp_C_LOGIC_WHILE_START.cycNum=(uint32_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                m_ProOrder[i][j].pData = (void *)malloc(sizeof(P_LogicWhileStartStruct));
                memcpy(m_ProOrder[i][j].pData,&temp_C_LOGIC_WHILE_START,sizeof(P_LogicWhileStartStruct));
                break;
            case C_LOGIC_WHILE_END:
                P_LogicWhileEndStruct temp_C_LOGIC_WHILE_END;
                for(int m=0;m<4;m++)
                {
                    temp_C_LOGIC_WHILE_END.res[m]=0;
                }
                m_ProOrder[i][j].pData = (void *)malloc(sizeof(P_LogicWhileEndStruct));
                memcpy(m_ProOrder[i][j].pData,&temp_C_LOGIC_WHILE_END,sizeof(P_LogicWhileEndStruct));
                break;
            case C_LOGIC_VAR:
                P_LogicVarStruct temp_C_LOGIC_VAR;
                temp_C_LOGIC_VAR.sufferOperValue=(int32_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
                temp_C_LOGIC_VAR.varNum=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
                temp_C_LOGIC_VAR.operMode=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
                temp_C_LOGIC_VAR.sufferOperType=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                for(int m=0;m<1;m++)
                {
                    temp_C_LOGIC_VAR.ret[m]=0;
                }
                m_ProOrder[i][j].pData = (void *)malloc(sizeof(P_LogicVarStruct));
                memcpy(m_ProOrder[i][j].pData,&temp_C_LOGIC_VAR,sizeof(P_LogicVarStruct));
                break;
            case C_LOGIC_AXIS:
                P_LogicAxisStruct temp_C_LOGIC_AXIS;
                temp_C_LOGIC_AXIS.sufferOperValue=(int32_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
                temp_C_LOGIC_AXIS.axisNum=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
                temp_C_LOGIC_AXIS.operMode=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
                temp_C_LOGIC_AXIS.sufferOperType=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                for(int m=0;m<1;m++)
                {
                    temp_C_LOGIC_AXIS.ret[m]=0;
                }
                m_ProOrder[i][j].pData = (void *)malloc(sizeof(P_LogicAxisStruct));
                memcpy(m_ProOrder[i][j].pData,&temp_C_LOGIC_AXIS,sizeof(P_LogicAxisStruct));
                break;
            case C_LOGIC_STACK:
                P_LogicStackStruct temp_C_LOGIC_STACK;
                temp_C_LOGIC_STACK.sufferOperValue=(int32_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
                temp_C_LOGIC_STACK.stackNum=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
                temp_C_LOGIC_STACK.operMode=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
                temp_C_LOGIC_STACK.sufferOperType=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                for(int m=0;m<1;m++)
                {
                    temp_C_LOGIC_STACK.ret[m]=0;
                }
                m_ProOrder[i][j].pData = (void *)malloc(sizeof(P_LogicStackStruct));
                memcpy(m_ProOrder[i][j].pData,&temp_C_LOGIC_STACK,sizeof(P_LogicStackStruct));
                break;
            case C_LOGIC_PRODUCT:
                P_LogicCurProductNumStruct temp_C_LOGIC_PRODUCT;
                temp_C_LOGIC_PRODUCT.sufferOperValue=(int32_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
                temp_C_LOGIC_PRODUCT.productNum=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
                temp_C_LOGIC_PRODUCT.operMode=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
                temp_C_LOGIC_PRODUCT.sufferOperType=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                for(int m=0;m<1;m++)
                {
                    temp_C_LOGIC_PRODUCT.ret[m]=0;
                }
                m_ProOrder[i][j].pData = (void *)malloc(sizeof(P_LogicCurProductNumStruct));
                memcpy(m_ProOrder[i][j].pData,&temp_C_LOGIC_PRODUCT,sizeof(P_LogicCurProductNumStruct));
                break;
            case C_LOGIC_TIME:
                P_LogicTimeStruct temp_C_LOGIC_TIME;
                temp_C_LOGIC_TIME.timeNum=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
                temp_C_LOGIC_TIME.operMode=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                for(int m=0;m<2;m++)
                {
                    temp_C_LOGIC_TIME.ret[m]=0;
                }
                m_ProOrder[i][j].pData = (void *)malloc(sizeof(P_LogicTimeStruct));
                memcpy(m_ProOrder[i][j].pData,&temp_C_LOGIC_TIME,sizeof(P_LogicTimeStruct));
                break;
            case C_SEARCH_STOP:
                P_SearchStopStruct temp_C_SEARCH_STOP;
                for(int m=0;m<4;m++)
                {
                    temp_C_SEARCH_STOP.res[m]=0;
                }
                m_ProOrder[i][j].pData = (void *)malloc(sizeof(P_SearchStopStruct));
                memcpy(m_ProOrder[i][j].pData,&temp_C_SEARCH_STOP,sizeof(P_SearchStopStruct));
                break;
            case C_SEARCH_AXIS_MOVE:
                P_SearchAxisMoveStruct temp_C_SEARCH_AXIS_MOVE;
                temp_C_SEARCH_AXIS_MOVE.maxPos=(int32_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toDouble()/PRECISION_001);
                index++;
                temp_C_SEARCH_AXIS_MOVE.advCDis=(int32_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toDouble()/PRECISION_001);
                index++;
                temp_C_SEARCH_AXIS_MOVE.offsetDis=(int32_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toDouble()/PRECISION_001);
                index++;
                temp_C_SEARCH_AXIS_MOVE.axis=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
                temp_C_SEARCH_AXIS_MOVE.posStoreFlag=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
                temp_C_SEARCH_AXIS_MOVE.reachPosAlarmFlag=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
                temp_C_SEARCH_AXIS_MOVE.runSpeed=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
                temp_C_SEARCH_AXIS_MOVE.advCSpeed=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
                temp_C_SEARCH_AXIS_MOVE.searchNum=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
                for(int m=0;m<3;m++)
                {
                    temp_C_SEARCH_AXIS_MOVE.ret[m]=0;
                }
                for(int m=0;m<SEARCH_INPORT_TOTAL_NUM;m++)
                {
                    temp_C_SEARCH_AXIS_MOVE.inportNum[m]=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                    index++;
                }
                for(int m=0;m<SEARCH_INPORT_TOTAL_NUM;m++)
                {
                    temp_C_SEARCH_AXIS_MOVE.inporttype[m]=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                    index++;
                }
                m_ProOrder[i][j].pData = (void *)malloc(sizeof(P_SearchAxisMoveStruct));
                memcpy(m_ProOrder[i][j].pData,&temp_C_SEARCH_AXIS_MOVE,sizeof(P_SearchAxisMoveStruct));
                break;
            case C_OFFSET_AXIS:
                P_OffsetAxisStruct temp_C_OFFSET_AXIS;
                temp_C_OFFSET_AXIS.offsetPos=(int32_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toDouble()/PRECISION_001);
                index++;
                temp_C_OFFSET_AXIS.advEndDis=(int32_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toDouble()/PRECISION_001);
                index++;
                temp_C_OFFSET_AXIS.advCSpeedDis=(int32_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toDouble()/PRECISION_001);
                index++;
                temp_C_OFFSET_AXIS.axis=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
                temp_C_OFFSET_AXIS.speed=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
                temp_C_OFFSET_AXIS.advEndFlag=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
                temp_C_OFFSET_AXIS.advCSpeedFlag=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
                temp_C_OFFSET_AXIS.advCSpeedSpeed=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                for(int m=0;m<3;m++)
                {
                    temp_C_OFFSET_AXIS.ret[m]=0;
                }
                m_ProOrder[i][j].pData = (void *)malloc(sizeof(P_OffsetAxisStruct));
                memcpy(m_ProOrder[i][j].pData,&temp_C_OFFSET_AXIS,sizeof(P_OffsetAxisStruct));
                break;
            case C_TORQUE_GARD:
                P_TorqueGardStruct temp_C_TORQUE_GARD;
                temp_C_TORQUE_GARD.axis=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
                temp_C_TORQUE_GARD.torqueValue=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                for(int m=0;m<2;m++)
                {
                    temp_C_TORQUE_GARD.ret[m]=0;
                }
                m_ProOrder[i][j].pData = (void *)malloc(sizeof(P_TorqueGardStruct));
                memcpy(m_ProOrder[i][j].pData,&temp_C_TORQUE_GARD,sizeof(P_TorqueGardStruct));
                break;
            case C_AXIS_STOP:
                P_AxisStopStruct temp_C_AXIS_STOP;
                temp_C_AXIS_STOP.axis=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                for(int m=0;m<3;m++)
                {
                    temp_C_AXIS_STOP.ret[m]=0;
                }
                m_ProOrder[i][j].pData = (void *)malloc(sizeof(P_AxisStopStruct));
                memcpy(m_ProOrder[i][j].pData,&temp_C_AXIS_STOP,sizeof(P_AxisStopStruct));
                break;
            case C_SUN_PRO:
                P_SunProStruct temp_C_SUN_PRO;
                temp_C_SUN_PRO.sunProNum=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
                temp_C_SUN_PRO.oprMode=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                for(int m=0;m<2;m++)
                {
                    temp_C_SUN_PRO.ret[m]=0;
                }
                m_ProOrder[i][j].pData = (void *)malloc(sizeof(P_SunProStruct));
                memcpy(m_ProOrder[i][j].pData,&temp_C_SUN_PRO,sizeof(P_SunProStruct));
                break;
            case C_PRO_END:
                P_ProEndStruct temp_C_PRO_END;
                for(int m=0;m<4;m++)
                {
                    temp_C_PRO_END.res[m]=0xff;
                }
                m_ProOrder[i][j].pData = (void *)malloc(sizeof(P_ProEndStruct));
                memcpy(m_ProOrder[i][j].pData,&temp_C_PRO_END,sizeof(P_ProEndStruct));
                break;
            default:
                break;

            }
        }
    }
    //变量读取
    if(Var_List.count() == VAR_TOTAL_NUM)
    {
        for(int i=0;i<VAR_TOTAL_NUM;i++)
        {

            m_VariableType[i] = (uint8_t)(Var_List[i].mid(Var_List[i].indexOf("=")+1).toUInt());
        }
        std::copy(std::begin(m_VariableType),std::end(m_VariableType),std::begin(m_VariableTypeLod));
    }
    return true;
}
#endif

/*************************************************************************
**  函数名：  readPreviewLableOrderName()
**	输入参数：文件路径
**	输出参数：是否读取成功
**	函数功能：读取浏览程序中标签指令名称
**  作者：    wukui
**  开发日期：2024/10/25
**************************************************************************/
bool readPreviewLableOrderName(D_ProgramNameAndPathStruct pro_temp)
{
    QFile file(pro_temp.filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
       // 处理文件打开失败的情况
       return false;
    }
    uint8_t intFlag = 0;
    QStringList LableNameList_Temp[PRO_NUM];
    QTextStream in(&file);
    for(uint8_t i=0;i<PRO_NUM;i++)
    {
        Preview_LableNameList[i].clear();//先清空列表
    }
    while(!in.atEnd())
    {
        QString line=in.readLine();
        if(line.compare(LablePartList[0])==0)            //主标签
        {
            intFlag = 1;
            continue;
        }
        else if(line.compare(LablePartList[1])==0)            //子标签1
        {
            intFlag = 0;
            continue;
        }
        else if(line.compare(LablePartList[2])==0)            //子标签2
        {
            intFlag = 0;
            continue;
        }
        else if(line.compare(LablePartList[3])==0)            //子标签3
        {
            intFlag = 0;
            continue;
        }
        else if(line.compare(LablePartList[4])==0)            //子标签4
        {
            intFlag = 0;
            continue;
        }
        else if(line.compare(LablePartList[5])==0)            //子标签5
        {
            intFlag = 0;
            continue;
        }
        else if(line.compare(LablePartList[6])==0)            //子标签6
        {
            intFlag = 0;
            continue;
        }
        else if(line.compare(LablePartList[7])==0)            //子标签7
        {
            intFlag = 0;
            continue;
        }
        else if(line.compare(LablePartList[8])==0)            //子标签8
        {
            intFlag = 0;
            continue;
        }
        else if(line==""||line==NULL)
        {
            continue;
        }

        switch (intFlag)
        {
        case 1:
            LableNameList_Temp[0].append(line);
        break;
        case 2:
            LableNameList_Temp[1].append(line);
        break;
        case 3:
            LableNameList_Temp[2].append(line);
        break;
        case 4:
            LableNameList_Temp[3].append(line);
        break;
        case 5:
            LableNameList_Temp[4].append(line);
        break;
        case 6:
            LableNameList_Temp[5].append(line);
        break;
        case 7:
            LableNameList_Temp[6].append(line);
        break;
        case 8:
            LableNameList_Temp[7].append(line);
        break;
        case 9:
            LableNameList_Temp[8].append(line);
        break;
        default:
        break;
        }
    }
    for(int j=0;j<LableNameList_Temp[0].count();j++)
    {
        QStringList tempList=LableNameList_Temp[0][j].split(signalSpace);
        tempList.removeAt(0);
        tempList.removeAt(0);
        Preview_LableNameList[0].append(tempList[0]+ " " +tempList[1]);
    }

}

/*************************************************************************
**  函数名：  readPreviewProgram()
**	输入参数：文件路径
**	输出参数：主程序行数
**	函数功能：读取文件浏览程序,并返回主程序行数
**  作者：    wukui
**  开发日期：2024/10/25
**************************************************************************/
int readPreviewProgram(D_ProgramNameAndPathStruct pro_temp,P_ProOrderStruct *m_ProOrder)
{
    QFile file(pro_temp.filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
       // 处理文件打开失败的情况
       return false;
    }
    QTextStream in(&file);
    QStringList programInfoList;
    QStringList P_List;

    int intFlag=0;              //设置一个标志位，区分当前读到什么程序了
    while(!in.atEnd())
    {
        QString line=in.readLine();
        if(line.compare(fileSectionList[0])==0)                 //程序行数
        {
            programInfoList.append(in.readLine());
        }
        else if(line.compare(fileSectionList[2])==0)            //主程序
        {
            intFlag=2;
            continue;
        }
        else if(line.compare(LablePartList[0])==0 || line.compare(fileSectionList[3])==0 || line.compare(fileSectionList[4])==0
                 || line.compare(fileSectionList[5])==0 || line.compare(fileSectionList[6])==0 || line.compare(fileSectionList[7])==0
                 || line.compare(fileSectionList[8])==0 || line.compare(fileSectionList[9])==0)
        {//标签和子程序暂时不读取
            intFlag = 0;
            continue;
        }
        else if(line==""||line==NULL)
        {
            continue;
        }

        switch (intFlag)
        {
            case 2:
                P_List.append(line);
                break;
            default:
                break;
        }

    }
    file.close();

    //开始解析

    for(int j=0;j<P_List.count();j++)
    {
        QStringList tempList=P_List[j].split(signalSpace);
        tempList.removeAt(0);
        tempList.removeAt(0);
        uint16_t index=0;
        m_ProOrder[j].list=(uint16_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
        index++;
        m_ProOrder[j].runOrderNum=(uint16_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
        index++;
        m_ProOrder[j].cmd=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
        index++;
        m_ProOrder[j].noteFlag=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
        index++;
        m_ProOrder[j].delay=(uint16_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toDouble()/PRECISION_001);
        index++;

        switch (m_ProOrder[j].cmd)
        {
        case C_AXIS_MOVE:
            P_AxisMoveStruct temp_C_AXIS_MOVE;
            temp_C_AXIS_MOVE.pos=(int32_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toDouble()/PRECISION_001);
            index++;
            temp_C_AXIS_MOVE.advEndDis=(int32_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toDouble()/PRECISION_001);
            index++;
            temp_C_AXIS_MOVE.advCSpeedDis=(int32_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toDouble()/PRECISION_001);
            index++;
            temp_C_AXIS_MOVE.axis=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            temp_C_AXIS_MOVE.speed=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            temp_C_AXIS_MOVE.advEndFlag=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            temp_C_AXIS_MOVE.advCSpeedFlag=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            temp_C_AXIS_MOVE.advCSpeedSpeed=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            temp_C_AXIS_MOVE.referPointNum=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            for(int m=0;m<2;m++)
            {
                temp_C_AXIS_MOVE.ret[m]=0;
            }
            m_ProOrder[j].pData = (void *)malloc(sizeof(P_AxisMoveStruct));
            memcpy(m_ProOrder[j].pData,&temp_C_AXIS_MOVE,sizeof(P_AxisMoveStruct));
            break;

        case C_CLAW_ACTION:
            P_ClawActionStruct temp_C_CLAW_ACTION;
            temp_C_CLAW_ACTION.outportNum=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            temp_C_CLAW_ACTION.type=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            for(int m=0;m<2;m++)
            {
                temp_C_CLAW_ACTION.ret[m]=0;
            }
            m_ProOrder[j].pData = (void *)malloc(sizeof(P_ClawActionStruct));
            memcpy(m_ProOrder[j].pData,&temp_C_CLAW_ACTION,sizeof(P_ClawActionStruct));
            break;
        case C_CLAW_CHECK:
            P_ClawCheckStruct temp_C_CLAW_CHECK;
            temp_C_CLAW_CHECK.outportNum=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            temp_C_CLAW_CHECK.type=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            for(int m=0;m<2;m++)
            {
                temp_C_CLAW_CHECK.ret[m]=0;
            }
            m_ProOrder[j].pData = (void *)malloc(sizeof(P_ClawCheckStruct));
            memcpy(m_ProOrder[j].pData,&temp_C_CLAW_CHECK,sizeof(P_ClawCheckStruct));
            break;

        case C_RESERVE_CHECK:
            P_ReserveCheckStruct temp_C_RESERVE_CHECK;
            temp_C_RESERVE_CHECK.inportNum=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            temp_C_RESERVE_CHECK.type=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            for(int m=0;m<2;m++)
            {
                temp_C_RESERVE_CHECK.ret[m]=0;
            }
            m_ProOrder[j].pData = (void *)malloc(sizeof(P_ReserveCheckStruct));
            memcpy(m_ProOrder[j].pData,&temp_C_RESERVE_CHECK,sizeof(P_ReserveCheckStruct));
            break;
        case C_MACHINE_OUT:
            P_MachineOutStruct temp_C_MACHINE_OUT;
            temp_C_MACHINE_OUT.outportNum=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            temp_C_MACHINE_OUT.type=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            for(int m=0;m<2;m++)
            {
                temp_C_MACHINE_OUT.ret[m]=0;
            }
            m_ProOrder[j].pData = (void *)malloc(sizeof(P_MachineOutStruct));
            memcpy(m_ProOrder[j].pData,&temp_C_MACHINE_OUT,sizeof(P_MachineOutStruct));
            break;
        case C_STACK_RESET_ZERO:
            P_StackResetZeroStruct temp_C_STACK_RESET_ZERO;
            temp_C_STACK_RESET_ZERO.number=(uint16_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            temp_C_STACK_RESET_ZERO.stackNum=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            for(int m=0;m<1;m++)
            {
                temp_C_STACK_RESET_ZERO.ret[m]=0;
            }
            m_ProOrder[j].pData = (void *)malloc(sizeof(P_StackResetZeroStruct));
            memcpy(m_ProOrder[j].pData,&temp_C_STACK_RESET_ZERO,sizeof(P_StackResetZeroStruct));
            break;
        case C_STACK_MOVE:
            P_StackMoveStruct temp_C_STACK_MOVE;
            for(int m=0;m<STACK_AXIS_NUM;m++)
            {
                temp_C_STACK_MOVE.advEndDis[m]=(int32_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toDouble()/PRECISION_001);
                index++;
            }
            for(int m=0;m<STACK_AXIS_NUM;m++)
            {
                temp_C_STACK_MOVE.advCSpeedDis[m]=(int32_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toDouble()/PRECISION_001);
                index++;
            }
            temp_C_STACK_MOVE.stackNum=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            for(int m=0;m<3;m++)
            {
                temp_C_STACK_MOVE.ret1[m]=0;
            }
            for(int m=0;m<STACK_AXIS_NUM;m++)
            {
                temp_C_STACK_MOVE.advEndFlag[m]=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
            }
            for(int m=0;m<1;m++)
            {
                temp_C_STACK_MOVE.ret2[m]=0;
            }
            for(int m=0;m<STACK_AXIS_NUM;m++)
            {
                temp_C_STACK_MOVE.advCSpeedFlag[m]=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
            }
            for(int m=0;m<1;m++)
            {
                temp_C_STACK_MOVE.ret3[m]=0;
            }
            for(int m=0;m<STACK_AXIS_NUM;m++)
            {
                temp_C_STACK_MOVE.advCSpeedSpeed[m]=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
            }
            for(int m=0;m<1;m++)
            {
                temp_C_STACK_MOVE.ret4[m]=0;
            }
            m_ProOrder[j].pData = (void *)malloc(sizeof(P_StackMoveStruct));
            memcpy(m_ProOrder[j].pData,&temp_C_STACK_MOVE,sizeof(P_StackMoveStruct));
            break;
        case C_STACK_FOLLOW:
            P_StackFollowStruct temp_C_STACK_FOLLOW;
            for(int m=0;m<STACK_AXIS_NUM;m++)
            {
                temp_C_STACK_FOLLOW.advEndDis[m]=(int32_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toDouble()/PRECISION_001);
                index++;
            }
            for(int m=0;m<STACK_AXIS_NUM;m++)
            {
                temp_C_STACK_FOLLOW.advCSpeedDis[m]=(int32_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toDouble()/PRECISION_001);
                index++;
            }
            temp_C_STACK_FOLLOW.stackNum=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            for(int m=0;m<3;m++)
            {
                temp_C_STACK_FOLLOW.ret1[m]=0;
            }
            for(int m=0;m<STACK_AXIS_NUM;m++)
            {
                temp_C_STACK_FOLLOW.advEndFlag[m]=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
            }
            for(int m=0;m<1;m++)
            {
                temp_C_STACK_FOLLOW.ret2[m]=0;
            }
            for(int m=0;m<STACK_AXIS_NUM;m++)
            {
                temp_C_STACK_FOLLOW.advCSpeedFlag[m]=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
            }
            for(int m=0;m<1;m++)
            {
                temp_C_STACK_FOLLOW.ret3[m]=0;
            }
            for(int m=0;m<STACK_AXIS_NUM;m++)
            {
                temp_C_STACK_FOLLOW.advCSpeedSpeed[m]=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
            }
            for(int m=0;m<1;m++)
            {
                temp_C_STACK_FOLLOW.ret4[m]=0;
            }
            m_ProOrder[j].pData = (void *)malloc(sizeof(P_StackFollowStruct));
            memcpy(m_ProOrder[j].pData,&temp_C_STACK_FOLLOW,sizeof(P_StackFollowStruct));
            break;
        case C_RESERVE_OUT:
            P_ReserveOutStruct temp_C_RESERVE_OUT;
            temp_C_RESERVE_OUT.interval=(uint16_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            temp_C_RESERVE_OUT.outportNum=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            temp_C_RESERVE_OUT.function=(uint16_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            temp_C_RESERVE_OUT.type=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            for(int m=0;m<3;m++)
            {
                temp_C_RESERVE_OUT.ret[m]=0;
            }
            m_ProOrder[j].pData = (void *)malloc(sizeof(P_ReserveOutStruct));
            memcpy(m_ProOrder[j].pData,&temp_C_RESERVE_OUT,sizeof(P_ReserveOutStruct));

            break;
        case C_WAIT_IN_MACHINE:
            P_WaitInMachineStruct temp_C_WAIT_IN_MACHINE;
            temp_C_WAIT_IN_MACHINE.backListNum=(uint16_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            temp_C_WAIT_IN_MACHINE.inportNum=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            temp_C_WAIT_IN_MACHINE.inportSta=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            temp_C_WAIT_IN_MACHINE.type=(uint16_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            temp_C_WAIT_IN_MACHINE.label=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            for(int m=0;m<2;m++)
            {
                temp_C_WAIT_IN_MACHINE.ret[m]=0;
            }
            m_ProOrder[j].pData = (void *)malloc(sizeof(P_WaitInMachineStruct));
            memcpy(m_ProOrder[j].pData,&temp_C_WAIT_IN_MACHINE,sizeof(P_WaitInMachineStruct));
            break;
        case C_WAIT_IN_CLAW:
            P_WaitInClawStruct temp_C_WAIT_IN_CLAW;
            temp_C_WAIT_IN_CLAW.backListNum=(uint16_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            temp_C_WAIT_IN_CLAW.inportNum=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            temp_C_WAIT_IN_CLAW.inportSta=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            temp_C_WAIT_IN_CLAW.type=(uint16_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            temp_C_WAIT_IN_CLAW.label=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            for(int m=0;m<3;m++)
            {
                temp_C_WAIT_IN_CLAW.ret[m]=0;
            }
            m_ProOrder[j].pData = (void *)malloc(sizeof(P_WaitInClawStruct));
            memcpy(m_ProOrder[j].pData,&temp_C_WAIT_IN_CLAW,sizeof(P_WaitInClawStruct));
            break;
        case C_WAIT_IN_RESERVE:
            P_WaitInReserveStruct temp_C_WAIT_IN_RESERVE;
            temp_C_WAIT_IN_RESERVE.backListNum=(uint16_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            temp_C_WAIT_IN_RESERVE.inportNum=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            temp_C_WAIT_IN_RESERVE.inportSta=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            temp_C_WAIT_IN_RESERVE.type=(uint16_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            temp_C_WAIT_IN_RESERVE.label=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            for(int m=0;m<3;m++)
            {
                temp_C_WAIT_IN_RESERVE.ret[m]=0;
            }
            m_ProOrder[j].pData = (void *)malloc(sizeof(P_WaitInReserveStruct));
            memcpy(m_ProOrder[j].pData,&temp_C_WAIT_IN_RESERVE,sizeof(P_WaitInReserveStruct));
            break;
        case C_OTHER_DELAY:
            P_OtherDelayStruct temp_C_OTHER_DELAY;
            for(int m=0;m<4;m++)
            {
                temp_C_OTHER_DELAY.res[m]=0;
            }
            m_ProOrder[j].pData = (void *)malloc(sizeof(P_OtherDelayStruct));
            memcpy(m_ProOrder[j].pData,&temp_C_OTHER_DELAY,sizeof(P_OtherDelayStruct));
            break;
        case C_OTHER_ALARM_CUST:
            P_OtherAlarmCustStruct temp_C_OTHER_ALARM_CUST;
            temp_C_OTHER_ALARM_CUST.alarmNum=(uint16_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            temp_C_OTHER_ALARM_CUST.type=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            for(int m=0;m<1;m++)
            {
                temp_C_OTHER_ALARM_CUST.ret[m]=0;
            }
            m_ProOrder[j].pData = (void *)malloc(sizeof(P_OtherAlarmCustStruct));
            memcpy(m_ProOrder[j].pData,&temp_C_OTHER_ALARM_CUST,sizeof(P_OtherAlarmCustStruct));
            break;
        case C_OTHER_CYC_STOP:
            P_OtherCycStopStruct temp_C_OTHER_CYC_STOP;
            temp_C_OTHER_CYC_STOP.cycleNum=(uint16_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            for(int m=0;m<2;m++)
            {
                temp_C_OTHER_CYC_STOP.ret[m]=0;
            }
            m_ProOrder[j].pData = (void *)malloc(sizeof(P_OtherCycStopStruct));
            memcpy(m_ProOrder[j].pData,&temp_C_OTHER_CYC_STOP,sizeof(P_OtherCycStopStruct));
            break;
        case C_OTHER_ALARM_LAMP:
            P_OtherAlarmLampStruct temp_C_OTHER_ALARM_LAMP;
            temp_C_OTHER_ALARM_LAMP.outportNum=(uint16_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            temp_C_OTHER_ALARM_LAMP.type=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            for(int m=0;m<2;m++)
            {
                temp_C_OTHER_ALARM_LAMP.ret[m]=0;
            }
            m_ProOrder[j].pData = (void *)malloc(sizeof(P_OtherAlarmLampStruct));
            memcpy(m_ProOrder[j].pData,&temp_C_OTHER_ALARM_LAMP,sizeof(P_OtherAlarmLampStruct));
            break;
        case C_OTHER_ALARM_SOUND:
            P_OtherAlarmSoundStruct temp_C_OTHER_ALARM_SOUND;
            temp_C_OTHER_ALARM_SOUND.outportNum=(uint16_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            temp_C_OTHER_ALARM_SOUND.type=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            for(int m=0;m<2;m++)
            {
                temp_C_OTHER_ALARM_SOUND.ret[m]=0;
            }
            m_ProOrder[j].pData = (void *)malloc(sizeof(P_OtherAlarmSoundStruct));
            memcpy(m_ProOrder[j].pData,&temp_C_OTHER_ALARM_SOUND,sizeof(P_OtherAlarmSoundStruct));
            break;
        case C_LABEL:
            P_LabelStruct temp_C_LABEL;
            temp_C_LABEL.labelNum=(uint16_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            temp_C_LABEL.type=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            for(int m=0;m<2;m++)
            {
                temp_C_LABEL.ret[m]=0;
            }
            m_ProOrder[j].pData = (void *)malloc(sizeof(P_LabelStruct));
            memcpy(m_ProOrder[j].pData,&temp_C_LABEL,sizeof(P_LabelStruct));
            break;
        case C_LOGIC_IF:
            P_LogicIfStruct temp_C_LOGIC_IF;
            for(int m=0;m<REQUIRE_TOTAL_NUM;m++)
            {
                temp_C_LOGIC_IF.sufferCmpValue[m]=(int32_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toDouble());
                index++;
            }
            for(int m=0;m<REQUIRE_TOTAL_NUM;m++)
            {
                temp_C_LOGIC_IF.reqSelectFlag[m]=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
            }
            for(int m=0;m<REQUIRE_TOTAL_NUM;m++)
            {
                temp_C_LOGIC_IF.cmpType[m]=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
            }
            for(int m=0;m<REQUIRE_TOTAL_NUM;m++)
            {
                temp_C_LOGIC_IF.inportNum[m]=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
            }
            for(int m=0;m<REQUIRE_TOTAL_NUM;m++)
            {
                temp_C_LOGIC_IF.inportType[m]=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
            }
            for(int m=0;m<REQUIRE_TOTAL_NUM;m++)
            {
                temp_C_LOGIC_IF.cmpMode[m]=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
            }
            for(int m=0;m<REQUIRE_TOTAL_NUM;m++)
            {
                temp_C_LOGIC_IF.sufferCmpType[m]=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
            }
            m_ProOrder[j].pData = (void *)malloc(sizeof(P_LogicIfStruct));
            memcpy(m_ProOrder[j].pData,&temp_C_LOGIC_IF,sizeof(P_LogicIfStruct));
            break;
        case C_LOGIC_ELSE:
            P_LogicElseStruct temp_C_LOGIC_ELSE;
            for(int m=0;m<4;m++)
            {
                temp_C_LOGIC_ELSE.res[m]=0;
            }
            m_ProOrder[j].pData = (void *)malloc(sizeof(P_LogicElseStruct));
            memcpy(m_ProOrder[j].pData,&temp_C_LOGIC_ELSE,sizeof(P_LogicElseStruct));
            break;
        case C_LOGIC_END:
            P_LogicEndStruct temp_C_LOGIC_END;
            for(int m=0;m<4;m++)
            {
                temp_C_LOGIC_END.res[m]=0;
            }
            m_ProOrder[j].pData = (void *)malloc(sizeof(P_LogicEndStruct));
            memcpy(m_ProOrder[j].pData,&temp_C_LOGIC_END,sizeof(P_LogicEndStruct));
            break;
        case C_LOGIC_WHILE_START:
            P_LogicWhileStartStruct temp_C_LOGIC_WHILE_START;
            temp_C_LOGIC_WHILE_START.cycNum=(uint32_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            m_ProOrder[j].pData = (void *)malloc(sizeof(P_LogicWhileStartStruct));
            memcpy(m_ProOrder[j].pData,&temp_C_LOGIC_WHILE_START,sizeof(P_LogicWhileStartStruct));
            break;
        case C_LOGIC_WHILE_END:
            P_LogicWhileEndStruct temp_C_LOGIC_WHILE_END;
            for(int m=0;m<4;m++)
            {
                temp_C_LOGIC_WHILE_END.res[m]=0;
            }
            m_ProOrder[j].pData = (void *)malloc(sizeof(P_LogicWhileEndStruct));
            memcpy(m_ProOrder[j].pData,&temp_C_LOGIC_WHILE_END,sizeof(P_LogicWhileEndStruct));
            break;
        case C_LOGIC_VAR:
            P_LogicVarStruct temp_C_LOGIC_VAR;
            temp_C_LOGIC_VAR.sufferOperValue=(int32_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            temp_C_LOGIC_VAR.varNum=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            temp_C_LOGIC_VAR.operMode=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            temp_C_LOGIC_VAR.sufferOperType=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            for(int m=0;m<1;m++)
            {
                temp_C_LOGIC_VAR.ret[m]=0;
            }
            m_ProOrder[j].pData = (void *)malloc(sizeof(P_LogicVarStruct));
            memcpy(m_ProOrder[j].pData,&temp_C_LOGIC_VAR,sizeof(P_LogicVarStruct));
            break;
        case C_LOGIC_AXIS:
            P_LogicAxisStruct temp_C_LOGIC_AXIS;
            temp_C_LOGIC_AXIS.sufferOperValue=(int32_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            temp_C_LOGIC_AXIS.axisNum=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            temp_C_LOGIC_AXIS.operMode=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            temp_C_LOGIC_AXIS.sufferOperType=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            for(int m=0;m<1;m++)
            {
                temp_C_LOGIC_AXIS.ret[m]=0;
            }
            m_ProOrder[j].pData = (void *)malloc(sizeof(P_LogicAxisStruct));
            memcpy(m_ProOrder[j].pData,&temp_C_LOGIC_AXIS,sizeof(P_LogicAxisStruct));
            break;
        case C_LOGIC_STACK:
            P_LogicStackStruct temp_C_LOGIC_STACK;
            temp_C_LOGIC_STACK.sufferOperValue=(int32_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            temp_C_LOGIC_STACK.stackNum=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            temp_C_LOGIC_STACK.operMode=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            temp_C_LOGIC_STACK.sufferOperType=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            for(int m=0;m<1;m++)
            {
                temp_C_LOGIC_STACK.ret[m]=0;
            }
            m_ProOrder[j].pData = (void *)malloc(sizeof(P_LogicStackStruct));
            memcpy(m_ProOrder[j].pData,&temp_C_LOGIC_STACK,sizeof(P_LogicStackStruct));
            break;
        case C_LOGIC_PRODUCT:
            P_LogicCurProductNumStruct temp_C_LOGIC_PRODUCT;
            temp_C_LOGIC_PRODUCT.sufferOperValue=(int32_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            temp_C_LOGIC_PRODUCT.productNum=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            temp_C_LOGIC_PRODUCT.operMode=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            temp_C_LOGIC_PRODUCT.sufferOperType=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            for(int m=0;m<1;m++)
            {
                temp_C_LOGIC_PRODUCT.ret[m]=0;
            }
            m_ProOrder[j].pData = (void *)malloc(sizeof(P_LogicCurProductNumStruct));
            memcpy(m_ProOrder[j].pData,&temp_C_LOGIC_PRODUCT,sizeof(P_LogicCurProductNumStruct));
            break;
        case C_LOGIC_TIME:
            P_LogicTimeStruct temp_C_LOGIC_TIME;
            temp_C_LOGIC_TIME.timeNum=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            temp_C_LOGIC_TIME.operMode=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            for(int m=0;m<2;m++)
            {
                temp_C_LOGIC_TIME.ret[m]=0;
            }
            m_ProOrder[j].pData = (void *)malloc(sizeof(P_LogicTimeStruct));
            memcpy(m_ProOrder[j].pData,&temp_C_LOGIC_TIME,sizeof(P_LogicTimeStruct));
            break;
        case C_SEARCH_STOP:
            P_SearchStopStruct temp_C_SEARCH_STOP;
            for(int m=0;m<4;m++)
            {
                temp_C_SEARCH_STOP.res[m]=0;
            }
            m_ProOrder[j].pData = (void *)malloc(sizeof(P_SearchStopStruct));
            memcpy(m_ProOrder[j].pData,&temp_C_SEARCH_STOP,sizeof(P_SearchStopStruct));
            break;
        case C_SEARCH_AXIS_MOVE:
            P_SearchAxisMoveStruct temp_C_SEARCH_AXIS_MOVE;
            temp_C_SEARCH_AXIS_MOVE.maxPos=(int32_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toDouble()/PRECISION_001);
            index++;
            temp_C_SEARCH_AXIS_MOVE.advCDis=(int32_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toDouble()/PRECISION_001);
            index++;
            temp_C_SEARCH_AXIS_MOVE.offsetDis=(int32_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toDouble()/PRECISION_001);
            index++;
            temp_C_SEARCH_AXIS_MOVE.axis=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            temp_C_SEARCH_AXIS_MOVE.posStoreFlag=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            temp_C_SEARCH_AXIS_MOVE.reachPosAlarmFlag=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            temp_C_SEARCH_AXIS_MOVE.runSpeed=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            temp_C_SEARCH_AXIS_MOVE.advCSpeed=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            temp_C_SEARCH_AXIS_MOVE.searchNum=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            for(int m=0;m<2;m++)
            {
                temp_C_SEARCH_AXIS_MOVE.ret[m]=0;
            }
            for(int m=0;m<SEARCH_INPORT_TOTAL_NUM;m++)
            {
                temp_C_SEARCH_AXIS_MOVE.inportNum[m]=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
            }
            for(int m=0;m<SEARCH_INPORT_TOTAL_NUM;m++)
            {
                temp_C_SEARCH_AXIS_MOVE.inporttype[m]=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
                index++;
            }
            m_ProOrder[j].pData = (void *)malloc(sizeof(P_SearchAxisMoveStruct));
            memcpy(m_ProOrder[j].pData,&temp_C_SEARCH_AXIS_MOVE,sizeof(P_SearchAxisMoveStruct));
            break;
        case C_OFFSET_AXIS:
            P_OffsetAxisStruct temp_C_OFFSET_AXIS;
            temp_C_OFFSET_AXIS.offsetPos=(int32_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toDouble()/PRECISION_001);
            index++;
            temp_C_OFFSET_AXIS.advEndDis=(int32_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toDouble()/PRECISION_001);
            index++;
            temp_C_OFFSET_AXIS.advCSpeedDis=(int32_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toDouble()/PRECISION_001);
            index++;
            temp_C_OFFSET_AXIS.axis=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            temp_C_OFFSET_AXIS.speed=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            temp_C_OFFSET_AXIS.advEndFlag=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            temp_C_OFFSET_AXIS.advCSpeedFlag=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            temp_C_OFFSET_AXIS.advCSpeedSpeed=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            for(int m=0;m<3;m++)
            {
                temp_C_OFFSET_AXIS.ret[m]=0;
            }
            m_ProOrder[j].pData = (void *)malloc(sizeof(P_OffsetAxisStruct));
            memcpy(m_ProOrder[j].pData,&temp_C_OFFSET_AXIS,sizeof(P_OffsetAxisStruct));
            break;
        case C_TORQUE_GARD:
            P_TorqueGardStruct temp_C_TORQUE_GARD;
            temp_C_TORQUE_GARD.axis=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            temp_C_TORQUE_GARD.torqueValue=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            for(int m=0;m<2;m++)
            {
                temp_C_TORQUE_GARD.ret[m]=0;
            }
            m_ProOrder[j].pData = (void *)malloc(sizeof(P_TorqueGardStruct));
            memcpy(m_ProOrder[j].pData,&temp_C_TORQUE_GARD,sizeof(P_TorqueGardStruct));
            break;
        case C_AXIS_STOP:
            P_AxisStopStruct temp_C_AXIS_STOP;
            temp_C_AXIS_STOP.axis=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            for(int m=0;m<3;m++)
            {
                temp_C_AXIS_STOP.ret[m]=0;
            }
            m_ProOrder[j].pData = (void *)malloc(sizeof(P_AxisStopStruct));
            memcpy(m_ProOrder[j].pData,&temp_C_AXIS_STOP,sizeof(P_AxisStopStruct));
            break;
        case C_SUN_PRO:
            P_SunProStruct temp_C_SUN_PRO;
            temp_C_SUN_PRO.sunProNum=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            index++;
            temp_C_SUN_PRO.oprMode=(uint8_t)(tempList[index].mid(tempList[index].indexOf("=")+1).toUInt());
            for(int m=0;m<2;m++)
            {
                temp_C_SUN_PRO.ret[m]=0;
            }
            m_ProOrder[j].pData = (void *)malloc(sizeof(P_SunProStruct));
            memcpy(m_ProOrder[j].pData,&temp_C_SUN_PRO,sizeof(P_SunProStruct));
            break;
        case C_PRO_END:
            P_ProEndStruct temp_C_PRO_END;
            for(int m=0;m<4;m++)
            {
                temp_C_PRO_END.res[m]=0xff;
            }
            m_ProOrder[j].pData = (void *)malloc(sizeof(P_ProEndStruct));
            memcpy(m_ProOrder[j].pData,&temp_C_PRO_END,sizeof(P_ProEndStruct));
            break;
        default:
            break;

        }
    }
    return (programInfoList[0].mid(programInfoList[0].indexOf("=")+1).toUInt());
}


/*************************************************************************
**  函数名：  	Load_Program_Handle()
**	输入参数：	ProgramName:需要加载的程序名称
**	输出参数：	return 0:失败  1:成功
**	函数功能：	指令控制函数
**	备注：	  	无
**  作者：    	wukui
**  开发日期：	2024.12.24
**************************************************************************/
bool Load_Program_Handle(QString fileName)
{
    int curProgramIndex = -1;
    for(int i=0;i<m_ProgramNameAndPath.count();i++)
    {
        if(m_ProgramNameAndPath[i].fileName == fileName)
        {
            curProgramIndex = i;
            break;
        }
    }
    if (curProgramIndex == -1)
    {
        return false;
    }
    //如果选择的文件不是当前程序，进行载入
    if(m_CurrentProgramNameAndPath.fileName == m_ProgramNameAndPath[curProgramIndex].fileName)
    {
        return false;
    }
    else
    {
        for(int i=0;i<PRO_NUM;i++)
        {
            for(int j=0;j<PRO_LINE_MAIN;j++)
            {
                g_FreeProOrder(&m_ProOrder[i][j]);//释放程序命令的数据指针
            }
        }
        memset(&m_CurrentProgramNameAndPath,0,sizeof(D_ProgramNameAndPathStruct));//清除当前程序信息
        memcpy(&m_CurrentProgramNameAndPath,&m_ProgramNameAndPath[curProgramIndex],sizeof(D_ProgramNameAndPathStruct));//改变当前程序信息
        readProgram(m_CurrentProgramNameAndPath);//读取当前程序指令
        readLableOrderName(m_CurrentProgramNameAndPath);//读取当前程序中标签名称列表
        m_OperateProOrderListNum = m_ProInfo.proNum[m_OperateProNum];

        g_TotalProCopy(m_OperateProOrder,m_ProOrder[m_OperateProNum]);//将读取的程序赋给当前操作程序
        CurrentLableNameList = LableNameList[m_OperateProNum];
        savePowerOnReadOneProInfo(m_CurrentProgramNameAndPath);
    }
    m_RunPar.startRunLineNum =1;//加载程序时起始行号设置为1
    //发送程序
    g_Usart->ExtendSendProDeal(CMD_MAIN_PRO,CMD_SUN_PRO_INFO);
    for(int i=0;i<PRO_NUM;i++)
    {
        for(int j=0;j<m_ProInfo.proNum[i];j++)
        {
            g_Usart->ExtendSendProDeal(CMD_MAIN_PRO,CMD_SUN_PRO_ORDER,i,j,0);
        }
    }
    g_Usart->ExtendSendProDeal(CMD_MAIN_PRO,CMD_SUN_PRO_SAVE,0,2);
    g_Usart->ExtendSendProDeal(CMD_MAIN_PRO,CMD_SUN_STA_VAR_TYPE,0,0);
    m_RunPar.startRunLineFlag = true;
    m_RunPar.planProductNum = 0;
    m_RunPar.startRunLineNum = 1;//加载程序时起始行号设置为1
    m_RunPar.breakPointFlag = false;
    m_RunPar.breakPointProNum = 0;
    m_RunPar.breakPointList = 1;
    return true;
}











