#include "cmd.h"
#include "malloc.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

Cmd::Cmd()
{

}

uint8_t m_OutPortType[OUT_PORT_TYPE_NUM][2] = {0};												//输出类型

uint8_t m_OutportInterlock[OUT_INTERLOCK_NUM][4] = {0};								//互锁设置

uint8_t m_InportFuncDefine[INPUT_TOTAL_NUM] = {0};											//输入端口自定义，0默认 1-60输入端口号
uint8_t m_OutportFuncDefine[OUTPUT_TOTAL_NUM] = {0};										//输出端口自定义，0默认 1-60输出端口号

uint8_t m_OutportRelevancy[OUT_PORT_RELEVANCY_NUM][3] = {0};						//预留关联

uint8_t m_OutportReleteOut[OUT_PORT_RELEOUT_NUM][2] = {0};							//预留出类型

D_KeyFuncStruct m_KeyFunc[OPR_KEY_NUM] = {0};											//按键功能
D_LedFuncStruct m_LedFunc[OPR_LED_NUM] = {0};											//LED功能

D_SeniorFuncStruct m_SeniorFunc = {0};														//高级功能
D_SeniorFuncPortStruct m_SeniorFuncPort = {0};										//高级功能对应端口
D_MachineSaveStruct m_MachineSave = {0};													//机床安全
D_StackSaveStruct m_StackSave = {0};															//料仓安全

D_ClawSaveStruct m_ClawSave[CLAW_SAVE_NUM] = {0};									//卡爪安全
D_OnlineSaveStruct m_OnlineSave[ONLINE_SAVE_NUM] = {0};						//联机安全

D_ProductSetStruct m_ProductSet = {0};														//产品设置
D_ProductSeniorStruct m_ProductSenior = {0};											//产品设置高级功能
D_InternetStruct m_Internet = {0};																//物联网
D_AxisParStruct m_AxisPar[AXIS_TOTAL_NUM] = {0};									//轴运动参数
D_AxisParStruct m_HandwheelPar = {0};															//手轮运动参数
D_ServoParStruct m_ServoPar = {0};																//伺服结构参数
D_DegreeParStruct m_DegreeParS = {0};															//伺服安全及控制精度参数
D_SaveAreaStruct m_SaveArea[SAVE_AREA_NUM] = {0};									//安全区参数
D_ServoCommParSetStruct m_ServoCommPar[SERVE_COMM_PAR_NUM] = {0};	//伺服通信协议参数
D_StackInfoStruct m_StackInfo[STACK_TOTAL_NUM] = {0};							//堆叠信息参数
D_StackFuncStruct m_StackFunc = {0};															//堆叠功能参数

uint8_t  m_InPortSta[INPUT_TOTAL_NUM] = {0};														//输入状态
uint8_t  m_OutPortSta[OUTPUT_TOTAL_NUM] = {0};													//输出状态
int32_t m_AxisCurPos[AXIS_TOTAL_NUM] = {0};														//轴当前位置
uint8_t  m_AxisCurDir[AXIS_TOTAL_NUM] = {0};														//轴当前运动方向
uint8_t  m_AxisMoveFlag[AXIS_TOTAL_NUM] = {0};													//轴运动标志 0结束运动 1运动中
uint8_t  m_AxisMovePauseReq[AXIS_TOTAL_NUM] = {0};											//轴运动暂停请求

uint8_t m_RobotRunSta = 0;																							//机器运行状态
uint8_t m_RobotWorkMode = 0;																						//机器工作模式
uint16_t m_AlarmNum = 0;																								//报警编号，0无故障 1-99紧急停止(急停，重新回原) 100-499急停报警(急停，不需要回原)                                                                                                                                 //500-999普通报警(停止) 1000-1499提示报警(暂停) 1500-1999提示(运行状态不变化)

uint32_t m_TotalProductNum = 0;																				//产品总产量---可用于物联网
uint32_t m_OilerProductNum = 0;																				//打油产量计数

P_ProInfoStruct m_ProInfo = {0};																	//程序信息
P_ProOrderStruct m_SUNProOrder[PRO_NUM][PRO_LINE_MAIN] = {0};	//程序命令
P_RefStruct m_RefPoint[REFERENCE_TOTAL_NUM] = {0};								//参考点

uint8_t m_OperateProNum = 0;																						//操作的程序编号
P_ProOrderStruct m_OperateProOrder[PRO_LINE_MAIN] = {0};					//操作的程序
uint16_t m_OperateProOrderListNum = {0};																//操作的程序行数

P_ProInfoStruct m_ProRunInfo = {0};																//运行信息--当前行号
D_RunInfoStruct m_RunInfo = {0};																	//运行信息
D_RunParStruct m_RunPar = {0};																		//运行参数
uint32_t m_StackCurPileCnt[STACK_TOTAL_NUM] = {0};											//当前每个堆叠组的堆叠计数
uint32_t m_VariableCurValue[VAR_TOTAL_NUM] = {0};											//当前每个变量的变量值
uint32_t m_TimeCurValue[TIME_TOTAL_NUM] = {0};													//当前每个定时器的计数值




//释放内存
void VOS_FREE(void **pData)
{
    if(*pData != NULL)
    {
        free(*pData);
        *pData = NULL;
    }

    return;
}

//释放程序命令的数据指针----外部调用
void g_FreeProOrder(P_ProOrderStruct *proOrder)
{
    if(proOrder == NULL)
    {//指针参数为空
        return;
    }

    proOrder->list = 0;								//清除行号
    proOrder->runOrderNum = 0;				//清除执行序号

    if(proOrder->pData == NULL)
    {//如果命令的数据指针是空
        return;
    }

    VOS_FREE(&proOrder->pData);
}

//获取命令数据长度
uint16_t g_GetProOrderDataLen(P_ProOrderStruct *pro)
{
    uint16_t len = 0;

    switch(pro->cmd)
    {
        case C_AXIS_MOVE://轴动作命令
            len = sizeof(P_AxisMoveStruct);
            break;
        case C_CLAW_ACTION://卡爪动作命令
            len = sizeof(P_ClawActionStruct);
            break;
        case C_CLAW_CHECK://信号检测-卡爪检测
            len = sizeof(P_ClawCheckStruct);
            break;
        case C_RESERVE_CHECK://信号检测-预留检测
            len = sizeof(P_ReserveCheckStruct);
            break;
        case C_MACHINE_OUT://机床-输出动作
            len = sizeof(P_MachineOutStruct);
            break;
        case C_STACK_RESET_ZERO://堆叠-堆叠清零
            len = sizeof(P_StackResetZeroStruct);
            break;
        case C_STACK_MOVE://堆叠-堆叠组运动
            len = sizeof(P_StackMoveStruct);
            break;
        case C_STACK_FOLLOW://堆叠-放料跟随运动
            len = sizeof(P_StackFollowStruct);
            break;
        case C_RESERVE_OUT://预留输出动作
            len = sizeof(P_ReserveOutStruct);
            break;
        case C_WAIT_IN_MACHINE://信号等待-等待机床
            len = sizeof(P_WaitInMachineStruct);
            break;
        case C_WAIT_IN_CLAW://信号等待-等待卡爪
            len = sizeof(P_WaitInClawStruct);
            break;
        case C_WAIT_IN_RESERVE://信号等待-等待预留
            len = sizeof(P_WaitInReserveStruct);
            break;
        case C_OTHER_DELAY://其他-延时
            len = sizeof(P_OtherDelayStruct);
            break;
        case C_OTHER_ALARM_CUST://其他-报警自定义
            len = sizeof(P_OtherAlarmCustStruct);
            break;
        case C_OTHER_CYC_STOP://其他-周期停止
            len = sizeof(P_OtherCycStopStruct);
            break;
        case C_LOGIC_IF://逻辑&变量-如果-如果
            len = sizeof(P_LogicIfStruct);
            break;
        case C_LOGIC_ELSE://逻辑&变量-如果-否则
            len = sizeof(P_LogicElseStruct);
            break;
        case C_LOGIC_END://逻辑&变量-如果-结束
            len = sizeof(P_LogicEndStruct);
            break;
        case C_LOGIC_WHILE_START://逻辑&变量-循环-循环开始
            len = sizeof(P_LogicWhileStartStruct);
            break;
        case C_LOGIC_WHILE_END://逻辑&变量-循环-循环结束
            len = sizeof(P_LogicWhileEndStruct);
            break;
        case C_LOGIC_VAR://逻辑&变量-变量-变量
            len = sizeof(P_LogicVarStruct);
            break;
        case C_LOGIC_AXIS://逻辑&变量-变量-轴
            len = sizeof(P_LogicAxisStruct);
            break;
        case C_LOGIC_STACK://逻辑&变量-变量-堆叠
            len = sizeof(P_LogicStackStruct);
            break;
        case C_LOGIC_PRODUCT://逻辑&变量-变量-实际产量
            len = sizeof(P_LogicCurProductNumStruct);
            break;
        case C_LOGIC_TIME://逻辑&变量-定时器
            len = sizeof(P_LogicTimeStruct);
            break;
        case C_SEARCH_STOP://搜索-感应停止
            len = sizeof(P_SearchStopStruct);
            break;
        case C_SEARCH_AXIS_MOVE://搜索-轴运动搜索
            len = sizeof(P_SearchAxisMoveStruct);
            break;
        case C_OFFSET_AXIS://偏移-轴偏移
            len = sizeof(P_OffsetAxisStruct);
            break;
        case C_TORQUE_GARD://扭矩保护
            len = sizeof(P_TorqueGardStruct);
            break;
        case C_AXIS_STOP://伺服停止
        len = sizeof(P_AxisStopStruct);
            break;
        case C_SUN_PRO://子程序
            len = sizeof(P_SunProStruct);
            break;
        case C_PRO_END://程序结束
            len = sizeof(P_ProEndStruct);
            break;
        default:
            break;
    }

    return len;
}

//获取程序信息----外部调用
uint8_t g_GetProInfo(uint8_t *data)
{
    uint16_t i = 0;

    m_ProInfo.proNum[PRO_MAIN_NUM] = ((uint16_t)data[0]) | ((uint16_t)data[1] << 8);
    if(m_ProInfo.proNum[PRO_MAIN_NUM] == 0 || m_ProInfo.proNum[PRO_MAIN_NUM] > PRO_LINE_MAIN)
    {
        m_ProInfo.proNum[PRO_MAIN_NUM] = 0;
        return 1;
    }

    for(i=1; i<PRO_NUM; i++)
    {
        m_ProInfo.proNum[i] = data[i+1];
        if(m_ProInfo.proNum[i] == 0 || m_ProInfo.proNum[i] > PRO_LINE_SUN)
        {
            m_ProInfo.proNum[i] = 0;
            return 1;
        }
    }

    return 0;
}

//获取命令数据信息
uint8_t g_GreateProOrderData(P_ProOrderStruct *pro, uint8_t *data)
{
    uint16_t len = 0;

    //如果当前行命令参数指针指向有效地址，需要先释放
    g_FreeProOrder(pro);

    len = g_GetProOrderDataLen(pro);				//获取命令长度

    pro->pData = (void*)malloc(len);				//申请内存大小
    memcpy(pro->pData, data, len);					//复制命令数据

    return 0;
}

//写入程序命令内容----外部调用
uint8_t g_GetProOrderData(uint8_t *data, uint8_t writeMode)
{
    uint8_t ret = 0;
    uint8_t proNum = 0;
    uint16_t proListNum = 0;
    uint16_t proRunOrderNum = 0;
    P_ProOrderStruct *proOrder = NULL;
    uint8_t cmd = 0;

    proNum = data[0];
    if(proNum >= PRO_NUM)
    {//程序编号异常
        return 1;
    }

    proListNum =  ((uint16_t)data[1]) | ((uint16_t)data[2] << 8);
    proRunOrderNum =  ((uint16_t)data[3]) | ((uint16_t)data[4] << 8);
    if(proListNum < proRunOrderNum)
    {
        return 2;
    }
    else if(proNum == PRO_MAIN_NUM && (proListNum > PRO_LINE_MAIN || proListNum == 0 || proRunOrderNum > PRO_LINE_MAIN || proRunOrderNum == 0))
    {//主程序的程序行号或命令序号异常，即行号异常
        return 3;
    }
    else if(proListNum > PRO_LINE_SUN || proListNum == 0 || proRunOrderNum > PRO_LINE_SUN || proRunOrderNum == 0)
    {//子程序的程序行号或命令序号异常，即行号异常
        return 4;
    }

    //获取待处理的程序
    if(writeMode == 1)
    {//停止修改
        proOrder = &m_OperateProOrder[proListNum - 1];
        if(proListNum > m_OperateProOrderListNum)
        {//停止修改停止修改
                return 5;
         }
        }
        else
        {
        proOrder = &m_ProOrder[proNum][proListNum - 1];
        if(writeMode == 2 && proListNum > m_ProInfo.proNum[proNum])
        {//自动修改停止修改
            return 5;
        }
    }
    cmd = data[5];
    if(cmd == 0 || cmd > C_PRO_END)
    {//命令异常
        proOrder->pData = NULL;												//中间指针必须指向NULL，否则函数返回时会把程序命令数据给释放
        proOrder = NULL;
        return 6;
    }

    //获取命令行的基本信息
    proOrder->list = proListNum;
    proOrder->runOrderNum = proRunOrderNum;
    proOrder->cmd = cmd;
    proOrder->noteFlag = data[6];
    proOrder->delay = ((uint16_t)data[7]) | ((uint16_t)data[8] << 8);
    ret = g_GreateProOrderData(proOrder, &data[9]);
    if(ret > 0)
    {//程序内容异常
        proOrder->pData = NULL;												//中间指针必须指向NULL，否则函数返回时会把程序命令数据给释放
        return 7;
    }

    proOrder->pData = NULL;												//中间指针必须指向NULL，否则函数返回时会把程序命令数据给释放
    proOrder = NULL;
    return 0;
}

//程序命令数据拷贝
uint8_t g_ProOrderDataCopy(P_ProOrderStruct *proOrder_New, P_ProOrderStruct *proOrder_Old)
{
    uint16_t len = 0;

    if(proOrder_Old->pData == NULL)
    {//被拷贝的命令数据指针为空，返回
        return 1;
    }

    //如果当前行命令参数指针指向有效地址，需要先释放
    g_FreeProOrder(proOrder_New);

    proOrder_New->list = proOrder_Old->list;
    proOrder_New->runOrderNum = proOrder_Old->runOrderNum;
    proOrder_New->cmd = proOrder_Old->cmd;
    proOrder_New->noteFlag = proOrder_Old->noteFlag;
    proOrder_New->delay = proOrder_Old->delay;

    len = g_GetProOrderDataLen(proOrder_Old);									//获取命令长度
    proOrder_New->pData = (void*)malloc(len);									//申请内存大小
    memcpy(proOrder_New->pData, proOrder_Old->pData, len);		//复制命令数据

    return 0;
}

//程序命令交换
void g_ProOrderSwap(P_ProOrderStruct *proOrder_Old, P_ProOrderStruct *proOrder_New)
{
    P_ProOrderStruct proOrder;

    proOrder.list = proOrder_New->list;
    proOrder.runOrderNum = proOrder_New->runOrderNum;
    proOrder.cmd = proOrder_New->cmd;
    proOrder.noteFlag = proOrder_New->noteFlag;
    proOrder.delay = proOrder_New->delay;
    proOrder.pData = proOrder_New->pData;

    proOrder_New->list = proOrder_Old->list;
    proOrder_New->runOrderNum = proOrder_Old->runOrderNum;
    proOrder_New->cmd = proOrder_Old->cmd;
    proOrder_New->noteFlag = proOrder_Old->noteFlag;
    proOrder_New->delay = proOrder_Old->delay;
    proOrder_New->pData = proOrder_Old->pData;

    proOrder_Old->list = proOrder.list;
    proOrder_Old->runOrderNum = proOrder.runOrderNum;
    proOrder_Old->cmd = proOrder.cmd;
    proOrder_Old->noteFlag = proOrder.noteFlag;
    proOrder_Old->delay = proOrder.delay;
    proOrder_Old->pData = proOrder.pData;

    proOrder.pData = NULL;
}

//插入程序命令----外部调用
uint8_t g_InsertProOrder(uint8_t *data)
{
    uint16_t i = 0;
    uint8_t ret = 0;
    uint8_t proNum = 0;
    uint16_t proListNum = 0;
    uint16_t proRunOrderNum = 0;
    P_ProOrderStruct proOrder;
    uint8_t cmd = 0;

    proNum = data[0];
    if(proNum >= PRO_NUM)
    {//程序编号异常
       return 1;
    }
    else if(proNum != m_OperateProNum)
    {//程序号异常
       return 2;
    }

    proListNum =  ((uint16_t)data[1]) | ((uint16_t)data[2] << 8);
    proRunOrderNum =  ((uint16_t)data[3]) | ((uint16_t)data[4] << 8);
    if(proListNum < proRunOrderNum)
    {
        return 3;
    }
    else if(proNum == PRO_MAIN_NUM && (proListNum > PRO_LINE_MAIN || proListNum == 0 || proRunOrderNum > PRO_LINE_MAIN || proRunOrderNum == 0))
    {//主程序的程序行号或命令序号异常，即行号异常
        return 4;
    }
    else if(proListNum > PRO_LINE_SUN || proListNum == 0 || proRunOrderNum > PRO_LINE_SUN || proRunOrderNum == 0)
    {//子程序的程序行号或命令序号异常，即行号异常
        return 5;
    }

    if(proListNum > m_ProInfo.proNum[proNum])
    {//插入位置异常
        return 6;
    }

    cmd = data[5];
    if(cmd == 0 || cmd > C_PRO_END)
    {//命令异常
        return 7;
    }

    //获取命令行的基本信息
    proOrder.list = proListNum;
    proOrder.runOrderNum = proRunOrderNum;
    proOrder.cmd = cmd;
    proOrder.noteFlag = data[6];
    proOrder.delay = ((uint16_t)data[7]) | ((uint16_t)data[8] << 8);
    ret = g_GreateProOrderData(&proOrder, &data[9]);
    if(ret > 0)
    {//程序内容异常
        return 8;
    }

    if(m_OperateProOrderListNum >= PRO_LINE_MAIN)
    {//主程序行数已经最大，不能再插入
            g_FreeProOrder(&proOrder);
            return 9;
    }

    for(i=m_OperateProOrderListNum; i>=proOrder.list; i--)
    {//将插入位置后面的程序向后移动一行
        if(m_OperateProOrder[i-1].list > 0 && m_OperateProOrder[i-1].runOrderNum > 0)
        {//有效程序才向后移动
           g_ProOrderSwap(&m_OperateProOrder[i], &m_OperateProOrder[i-1]);		//交换命令数据
           g_FreeProOrder(&m_OperateProOrder[i-1]);												//释放被交换的命令数据指针
        }
    }

    g_ProOrderDataCopy(&m_OperateProOrder[proOrder.list-1], &proOrder);//将待插入的命令数据拷贝到待插入的行

    m_OperateProOrderListNum++;				//插入一行命令完成后，程序行数加1
    g_FreeProOrder(&proOrder);				//释放临时变量中的命令指针

    return 0;
}

//删除程序命令----外部调用
uint8_t g_DeleteProOrder(uint8_t proNum, uint16_t list)
{
    uint16_t i = 0;

    if(proNum >= PRO_NUM)
    {
        return 1;
    }
    else if(list > m_OperateProOrderListNum || list == 0)
    {//子程序的程序行号或命令序号异常，即行号异常
        return 2;
    }

    else if(proNum != m_OperateProNum)
    {//程序号异常
        return 3;
    }

    if(m_OperateProOrderListNum == 0)
    {//程序行数为空，无法删除
        return 4;
    }

    for(i=list; i<m_OperateProOrderListNum; i++)
    {//将删除程序后面的程序前移动一行
        if(m_OperateProOrder[i].list > 0 && m_OperateProOrder[i].runOrderNum > 0)
        {//有效程序才向前移动
            g_ProOrderSwap(&m_OperateProOrder[i-1], &m_OperateProOrder[i]);	//交换命令数据
            g_FreeProOrder(&m_OperateProOrder[i]);												//释放被删除的命令数据指针
        }
        else
        {//后面都是无效程序，结束处理
            break;
        }
    }

    m_OperateProOrderListNum--;				//删除一行命令完成后，程序行数减1

    return 0;
}

//交换程序命令----外部调用
uint8_t g_SwapProOrder(uint8_t proNum, uint16_t list1, uint16_t list2)
{
    if(proNum >= PRO_NUM)
    {
        return 1;
    }
    else if(list2 == 0 || list1 == 0 || list1 > m_OperateProOrderListNum || list2 > m_OperateProOrderListNum)
    {//程序行号或命令序号异常，即行号异常
        return 2;
    }

    if(m_OperateProOrder[list1].list > 0 && m_OperateProOrder[list2].list > 0)
    {
        if(m_OperateProOrder[list1].runOrderNum > 0 && m_OperateProOrder[list2].runOrderNum > 0)
        {
            g_ProOrderSwap(&m_OperateProOrder[list1], &m_OperateProOrder[list2]);	//交换命令数据
        }
        else
        {//交换异常
            return 3;
        }
    }
    else
    {//交换异常
        return 4;
    }

    return 0;
}

//程序命令行操作-组合
uint8_t g_ProOrderCombination(uint8_t proNum, uint16_t proListNum, uint16_t type)
{
    uint16_t i = 0;

    if(proListNum == 0 || proListNum > m_OperateProOrderListNum)
    {//当前行号比上移行数还要小，异常
        return 1;
    }

    if(type == 0)
    {//向下组合
       if(proListNum == m_OperateProOrderListNum)
       {//最后一行不能向下组合
           return 1;
       }

       if(m_OperateProOrder[proListNum - 1].runOrderNum != m_OperateProOrder[proListNum].runOrderNum)
       {//如果当前行跟下一行的执行顺序一样
           for(i=proListNum; i<m_OperateProOrderListNum; i++)
           {//当前行后面的命令行都需要减1
                if(m_OperateProOrder[i].list > 0 && m_OperateProOrder[i].runOrderNum > 0)
                {//执行顺序都要减1
                    m_OperateProOrder[i].runOrderNum -= 1;
                }
                else
                {//后面都是无效程序，结束处理
                    break;
                }
            }
        }
        else
        {
            return 1;
        }
   }
   else
   {//向上组合
       if(proListNum == 1)
       {//第一行不能向上组合
            return 1;
       }

       if(m_OperateProOrder[proListNum - 1].runOrderNum != m_OperateProOrder[proListNum - 2].runOrderNum)
       {//如果当前行跟上一行的执行顺序一样
            for(i=proListNum-1; i<m_OperateProOrderListNum; i++)
            {//当前行及其后面的命令行
                if(m_OperateProOrder[i].list > 0 && m_OperateProOrder[i].runOrderNum > 0)
                {//执行顺序都要减1
                    m_OperateProOrder[i].runOrderNum -= 1;
                }
                else
                {//后面都是无效程序，结束处理
                    break;
                }
            }
        }
        else
        {
            return 1;
        }
   }

   return 0;
}

//程序命令行操作-分解
uint8_t g_ProOrderDecompose(uint8_t proNum, uint16_t proListNum)
{
    uint16_t i = 0;

    if(proListNum == 0 || proListNum > m_OperateProOrderListNum)
    {//当前行号比上移行数还要小，异常
        return 1;
    }

    if(proListNum > 1)
    {//当前行号大于1
        if(m_OperateProOrder[proListNum - 1].runOrderNum == m_OperateProOrder[proListNum - 2].runOrderNum)
        {//如果当前行跟上一行的执行顺序一样
            for(i=proListNum - 1; i<m_OperateProOrderListNum; i++)
            {//当前行及其后面的命令行
                if(m_OperateProOrder[i].list > 0 && m_OperateProOrder[i].runOrderNum > 0)
                {//执行顺序都要加1
                    m_OperateProOrder[i].runOrderNum += 1;
                }
                else
                {//后面都是无效程序，结束处理
                    break;
                }
            }
        }
        else if(m_OperateProOrder[proListNum - 1].runOrderNum == m_OperateProOrder[proListNum].runOrderNum)
        {//如果当前行跟上一行的执行顺序一样
            for(i=proListNum; i<m_OperateProOrderListNum; i++)
            {//当前行后面的命令行
                if(m_OperateProOrder[i].list > 0 && m_OperateProOrder[i].runOrderNum > 0)
                {//执行顺序都要加1
                    m_OperateProOrder[i].runOrderNum += 1;
                }
                else
                {//后面都是无效程序，结束处理
                    break;
                }
            }
        }
        else
        {
            return 1;
        }
    }
    else
    {
        if(m_OperateProOrder[proListNum - 1].runOrderNum == m_OperateProOrder[proListNum].runOrderNum)
        {//如果当前行跟上一行的执行顺序一样
            for(i=proListNum; i<m_OperateProOrderListNum; i++)
            {//当前行后面的命令行
                if(m_OperateProOrder[i].list > 0 && m_OperateProOrder[i].runOrderNum > 0)
                {//执行顺序都要加1
                    m_OperateProOrder[i].runOrderNum += 1;
                }
                else
                {//后面都是无效程序，结束处理
                    break;
                }
            }
        }
        else
        {
            return 1;
        }
    }

    return 0;
}

//程序命令行操作-上移
uint8_t g_ProOrderUpMove(uint8_t proNum, uint16_t proListNum, uint16_t len)
{
    uint16_t i = 0;
    P_ProOrderStruct proOrder;
    P_ProOrderStruct *p_proOrder;

    if(len == 0 || proListNum == 0)
    {
        return 1;
    }
    else if(proListNum <= len || proListNum > m_OperateProOrderListNum)
    {//当前行号比上移行数还要小，异常
        return 1;
    }

    //获取待处理的程序
    p_proOrder = &m_OperateProOrder[proListNum - 1];
    g_ProOrderDataCopy(&proOrder, p_proOrder);						//当前行的信息拷贝给中间变量
    g_FreeProOrder(p_proOrder);														//释放需要移动的原有命令数据指针

    for(i=proListNum-1; i>=proListNum-len; i--)
    {//将当前行之前的程序向后移动一行
        if(m_OperateProOrder[i-1].list > 0 && m_OperateProOrder[i-1].runOrderNum > 0)
        {//有效程序才向后移动
            g_ProOrderSwap(&m_OperateProOrder[i], &m_OperateProOrder[i-1]);	//交换命令数据
            g_FreeProOrder(&m_OperateProOrder[i-1]);													//释放被交换的命令数据指针
        }
    }

    g_ProOrderSwap(&m_OperateProOrder[proListNum-len-1], &proOrder);

    p_proOrder->pData = NULL;												//中间指针必须指向NULL，否则函数返回时会把程序命令数据给释放
    p_proOrder = NULL;
    g_FreeProOrder(&proOrder);											//释放中间变量的命令数据指针

    return 0;
}

//程序命令行操作-下移
uint8_t g_ProOrderDownMove(uint8_t proNum, uint16_t proListNum, uint16_t len)
{
    uint16_t i = 0;
    P_ProOrderStruct proOrder;
    P_ProOrderStruct *p_proOrder;

    if(len == 0 || proListNum == 0)
    {
        return 1;
    }
    else if(proListNum + len > m_OperateProOrderListNum)
    {//当前行号下移后超过行数，异常
        return 1;
    }

    //获取待处理的程序
    p_proOrder = &m_OperateProOrder[proListNum - 1];
    g_ProOrderDataCopy(&proOrder, p_proOrder);						//当前行的信息拷贝给中间变量
    g_FreeProOrder(p_proOrder);														//释放需要移动的原有命令数据指针


    for(i=proListNum; i<proListNum+len; i++)
    {//将当前行之前的程序向后移动一行
        if(m_OperateProOrder[i].list > 0 && m_OperateProOrder[i].runOrderNum > 0)
        {//有效程序才向后移动
            g_ProOrderSwap(&m_OperateProOrder[i-1], &m_OperateProOrder[i]);	//交换命令数据
            g_FreeProOrder(&m_OperateProOrder[i]);												//释放被交换的命令数据指针
        }
    }

    g_ProOrderSwap(&m_OperateProOrder[proListNum+len], &proOrder);

    p_proOrder->pData = NULL;												//中间指针必须指向NULL，否则函数返回时会把程序命令数据给释放
    p_proOrder = NULL;
    g_FreeProOrder(&proOrder);											//释放中间变量的命令数据指针

    return 0;
}


//程序命令行操作-屏蔽或取消屏蔽
uint8_t g_ProOrderShield(uint8_t proNum, uint16_t proListNum, uint16_t type)
{
    if(proListNum == 0 || proListNum > m_OperateProOrderListNum)
    {//当前行号异常
        return 1;
    }

    //获取待处理的程序
    if(type == 0)
    {//取消屏蔽行
        m_OperateProOrder[proListNum - 1].noteFlag = 0;
    }
    else
    {//屏蔽行
        m_OperateProOrder[proListNum - 1].noteFlag = 1;
    }

    return 0;
}

//程序命令行操作，删除、分解、组合、上移、下移、屏蔽、取消屏蔽----外部调用
uint8_t g_ProOrderOperate(uint8_t proNum, uint16_t proListNum, uint8_t type, uint16_t data)
{
    uint8_t ret = 0;

    if(proNum >= PRO_NUM)
    {
        return 1;
    }
    else if(proListNum == 0 || proListNum > m_OperateProOrderListNum)
    {//程序行号或命令序号异常，即行号异常
        return 2;
    }
    else if(proNum != m_OperateProNum)
    {//程序号异常
        return 3;
    }

    switch(type)
    {
        case 1://删除
            ret = g_DeleteProOrder(proNum, proListNum);
            break;
        case 2://组合
            ret = g_ProOrderCombination(proNum, proListNum, data);
            break;
        case 3://分解
            ret = g_ProOrderDecompose(proNum, proListNum);
            break;
        case 4://上移
            ret = g_ProOrderUpMove(proNum, proListNum, data);
            break;
        case 5://下移
            ret = g_ProOrderDownMove(proNum, proListNum, data);
            break;
        case 6://屏蔽
            ret = g_ProOrderShield(proNum, proListNum, data);
            break;
        default:
            break;
    }

    return ret;
}


//程序轴速度教导----外部调用
uint8_t g_ProOrderSpeedOpe(uint8_t proNum, uint16_t speed)
{
    uint16_t i = 0;
    P_ProOrderStruct *proOrder = NULL;

    if(speed == 0 || speed > 100)
    {//速度值异常
        return 1;
    }
    else if(proNum != m_OperateProNum)
    {//程序号异常
        return 2;
    }

    for(i=0; i<m_OperateProOrderListNum; i++)
    {
        proOrder = &m_OperateProOrder[i];
        if(proOrder->list > 0 && proOrder->runOrderNum > 0)
        {//命令有效
            switch(proOrder->cmd)
            {
                case C_AXIS_MOVE://轴动作命令
                    ((P_AxisMoveStruct*)proOrder->pData)->speed = speed;
                    ((P_AxisMoveStruct*)proOrder->pData)->advCSpeedSpeed = speed;
                    break;
                case C_STACK_MOVE://堆叠-堆叠组运动
                    ((P_StackMoveStruct*)proOrder->pData)->advCSpeedSpeed[STACK_X_AXIS] = speed;
                    ((P_StackMoveStruct*)proOrder->pData)->advCSpeedSpeed[STACK_Y_AXIS] = speed;
                    ((P_StackMoveStruct*)proOrder->pData)->advCSpeedSpeed[STACK_Z_AXIS] = speed;
                    break;
                case C_STACK_FOLLOW://堆叠-放料跟随运动
                    ((P_StackFollowStruct*)proOrder->pData)->advCSpeedSpeed[STACK_X_AXIS] = speed;
                    ((P_StackFollowStruct*)proOrder->pData)->advCSpeedSpeed[STACK_Y_AXIS] = speed;
                    ((P_StackFollowStruct*)proOrder->pData)->advCSpeedSpeed[STACK_Z_AXIS] = speed;
                    break;
                case C_SEARCH_AXIS_MOVE://搜索-轴运动搜索
                    ((P_SearchAxisMoveStruct*)proOrder->pData)->runSpeed = speed;
                    ((P_SearchAxisMoveStruct*)proOrder->pData)->advCSpeed = speed;
                    break;
                case C_OFFSET_AXIS://偏移-轴偏移
                    ((P_OffsetAxisStruct*)proOrder->pData)->speed = speed;
                    break;
                default:
                    break;
            }
        }
        else
        {
            break;
        }
    }

    proOrder->pData = NULL;
    proOrder = NULL;

    return 0;
}

//读出程序命令内容----外部调用
uint8_t g_ReadProOrderData(uint8_t proNum, uint16_t proListNum, uint8_t *data, uint16_t *dataLen)
{
    P_ProOrderStruct *proOrder = NULL;
    uint16_t len = 0;

    if(data == NULL)
    {//数据异常
        return 1;
    }
    else if(proNum >= PRO_NUM)
    {//程序编号异常
        return 2;
    }
    else if(proListNum > m_ProInfo.proNum[proNum])
    {
        return 3;
    }

    //获取待处理的程序
    proOrder = &m_ProOrder[proNum][proListNum - 1];

    if(proOrder->list != proListNum || proOrder->runOrderNum == 0 || proOrder->runOrderNum > m_ProInfo.proNum[proNum])
    {//命令行号异常
        return 4;
    }

    //获取命令行的基本信息
    data[0] = proNum;
    data[1] = proListNum;
    data[2] = proOrder->runOrderNum;
    data[3] = proOrder->cmd;
    data[4] = proOrder->noteFlag;
    data[5] = (uint8_t)proOrder->delay;
    data[6] = (uint8_t)(proOrder->delay>>8);

    len = g_GetProOrderDataLen(proOrder);
    memcpy(&data[7], proOrder->pData, len);				//复制命令数据

    *dataLen = len + 7;

    proOrder->pData = NULL;												//中间指针必须指向NULL，否则函数返回时会把程序命令数据给释放
    proOrder = NULL;

    return 0;
}

//整个程序拷贝---外调函数
uint8_t g_TotalProCopy(P_ProOrderStruct *proOrder_Old, P_ProOrderStruct *proOrder_New)
{
    uint16_t i = 0;

    if(m_OperateProNum >= PRO_NUM)
    {//程序号异常
        return 1;
    }
    else if(m_OperateProOrderListNum == 0 && m_OperateProOrderListNum > PRO_LINE_MAIN)
    {//程序行数异常
        return 2;
    }

    if(proOrder_New[m_OperateProOrderListNum - 1].cmd != C_PRO_END)
    {//最后一行命令必须是程序结束
        return 3;
    }
    else
    {
        for(i=0; i<m_OperateProOrderListNum; i++)
        {
            if(proOrder_New[i].list != i + 1)
            {//程序行号异常
                return 4;
            }
            else if(proOrder_New[i].runOrderNum == 0 || proOrder_New[i].runOrderNum > proOrder_New[i].list)
            {//程序执行序号异常
                return 5;
            }
        }
    }

    for(i=0; i<PRO_LINE_MAIN; i++)
    {//清除原有程序
        g_FreeProOrder(&proOrder_Old[i]);
    }

    for(i=0; i<m_OperateProOrderListNum; i++)
    {//程序拷贝
        g_ProOrderDataCopy(&proOrder_Old[i], &proOrder_New[i]);
    }

    m_ProInfo.proNum[m_OperateProNum] = m_OperateProOrderListNum;

    return 0;
}
