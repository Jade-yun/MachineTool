#ifndef CMD_H
#define CMD_H

#include "qstring.h"

class Cmd
{
public:
    Cmd();
};

#endif // CMD_H

#ifndef STRUCTPARA_H
#define STRUCTPARA_H

#endif // STRUCTPARA_H

#define AXIS_MIN_POSITION     				-9000000    //轴运动的最小坐标
#define AXIS_MAX_POSITION     				9000000    	//轴运动的最大坐标

#define PRECISION_001                       0.01                               //0.01精度

//轴个数及编号
#define AXIS_TOTAL_NUM								6			//轴总个数
#define X1_AXIS												0			//X1轴
#define Y1_AXIS												1			//Y1轴
#define Z1_AXIS												2			//Z2轴
#define C_AXIS												3			//C轴
#define Y2_AXIS												4			//Y2轴
#define Z2_AXIS												5			//Z2轴

//轴运动方向
#define AXIS_DIR_NEG									0		//负向运动
#define AXIS_DIR_POS									1		//正向运动

//普通输入输出个数
#define	INPUT_NUM											36
#define	OUTPUT_NUM										24
//扩展输入输出个数
#define	INPUT_E_NUM										24
#define	OUTPUT_E_NUM									16
//输入输出总个数
#define	INPUT_TOTAL_NUM								(INPUT_NUM + INPUT_E_NUM)
#define	OUTPUT_TOTAL_NUM							(OUTPUT_NUM + OUTPUT_E_NUM)

//堆叠组总个数
#define	STACK_TOTAL_NUM								8
//堆叠轴个数及编号
#define STACK_AXIS_NUM								3			//堆叠轴数量
#define STACK_X_AXIS									0			//横向X轴
#define STACK_Y_AXIS									1			//纵向Y轴
#define STACK_Z_AXIS									2			//垂直Z轴

//参考点总个数
#define	REFERENCE_TOTAL_NUM						100

//变量总个数
#define	VAR_TOTAL_NUM									8
//定时器总个数
#define	TIME_TOTAL_NUM								8
//逻辑&变量的条件总个数
#define	REQUIRE_TOTAL_NUM							2
//搜索运动停止信号总个数
#define	SEARCH_INPORT_TOTAL_NUM				2

//信号设置
#define OUT_PORT_TYPE_NUM							14		//输出类型个数，普通输出7个 扩展输出7个
#define OUT_INTERLOCK_NUM							12		//输出互锁个数，普通输出6个 扩展输出6个
#define OUT_PORT_RELEVANCY_NUM				14		//输出预留关联个数，普通输出8个 扩展输出6个
#define OUT_PORT_RELEOUT_NUM					14		//预留出类型个数，普通输出8个 扩展输出6个
#define OPR_KEY_NUM										16		//操作按键功能定义个数
#define OPR_LED_NUM										5			//操作指示灯功能定义个数
#define CLAW_SAVE_NUM									4			//卡爪安全区个数

#define ONLINE_SAVE_NUM								4			//联机安全区个数

#define SAVE_AREA_NUM									4			//安全区个数
#define SAVE_AXIS_NUM									3			//安全区轴个数
#define SAVE_X_AXIS										0			//安全区X轴
#define SAVE_Y_AXIS										1			//安全区Y轴
#define SAVE_Z_AXIS										2			//安全区Z轴


#define DEFINE_PORT_NUM       (INPUT_TOTAL_NUM+OUTPUT_TOTAL_NUM)    //端口自定义
#define DEFINE_NAME_NUM                                 50          //名称自定义个数

//枚举--机械手工作模式
enum WorkMode
{
    STOP_MODE = 1,			//停止模式
    HAND_MODE,					//手动模式
    AUTO_MODE,					//自动模式
};

/***********************程序相关变量定义*******************************/
//命令--cmd
#define C_AXIS_MOVE							1			//轴动作
#define C_CLAW_ACTION						2			//卡爪动作
#define C_CLAW_CHECK						3			//信号检测-卡爪检测
#define C_RESERVE_CHECK					4			//信号检测-预留检测
#define C_MACHINE_OUT						6			//机床-输出动作
#define C_STACK_RESET_ZERO			7			//堆叠-堆叠清零
#define C_STACK_MOVE						8			//堆叠-堆叠组运动
#define C_STACK_FOLLOW					9			//堆叠-放料跟随运动
#define C_RESERVE_OUT						10		//预留输出
#define C_WAIT_IN_MACHINE				11		//信号等待-等待机床
#define C_WAIT_IN_CLAW					12		//信号等待-等待卡爪
#define C_WAIT_IN_RESERVE				13		//信号等待-等待预留
#define C_OTHER_DELAY						14		//其他-延时
#define C_OTHER_ALARM_CUST			15		//其他-报警自定义
#define C_OTHER_CYC_STOP				16		//其他-周期停止
#define C_LABEL									30		//标签
#define C_LOGIC_IF							31		//逻辑&变量-如果-如果
#define C_LOGIC_ELSE						32		//逻辑&变量-如果-否则
#define C_LOGIC_END							33		//逻辑&变量-如果-结束
#define C_LOGIC_WHILE_START			34		//逻辑&变量-循环-循环开始
#define C_LOGIC_WHILE_END				35		//逻辑&变量-循环-循环结束
#define C_LOGIC_VAR							36		//逻辑&变量-变量-变量
#define C_LOGIC_AXIS						37		//逻辑&变量-变量-轴
#define C_LOGIC_STACK						38		//逻辑&变量-变量-堆叠
#define C_LOGIC_PRODUCT					39		//逻辑&变量-变量-实际产量
#define C_LOGIC_TIME						40		//逻辑&变量-定时器
#define C_SEARCH_STOP						45		//搜索-感应停止
#define C_SEARCH_AXIS_MOVE			46		//搜索-轴运动搜索
#define C_OFFSET_AXIS						50		//偏移-轴偏移
#define C_TORQUE_GARD						51		//扭矩保护
#define C_AXIS_STOP							52		//伺服停止
#define C_SUN_PRO								199		//子程序
#define C_PRO_END								200		//程序结束

#define PRO_NUM									9			//每个程序包含的进程个数
#define PRO_LINE_MAIN						999		//主进程包含的最大指令条数
#define PRO_LINE_SUN						249		//子进程包含的最大指令条数
#define PRO_MAIN_NUM						0			//主进程所在进程编号，第一个进程
#define PRO_RESET_NUM						8			//复位程序所在进程编号，在最后一个进程---子进程8
#define PRO_PARALLEL_NUM 				10		//最大运行的并行程序行数

//程序信息结构体
typedef struct
{
    uint16_t proNum[PRO_NUM];					//每个进程的命令行数
}P_ProInfoStruct;

/*命令结构体*/
typedef struct
{//12Byte
    uint16_t list;								//程序行号
    uint16_t runOrderNum;				//命令执行序号
    uint8_t  cmd;								//命令
    uint8_t  noteFlag;						//注释标志，0不注释，1注释，注释后跳过改行
    uint16_t delay;							//延时，单位0.01s
    void *pData;						//命令数据指针
}P_ProOrderStruct;

/*参考点定义结构体*/
typedef struct
{
    uint8_t  refFlag;							//参考点有效性，0无效 1有效
    uint8_t  ret[1];
    int32_t pos[AXIS_TOTAL_NUM];	//参考点位置，精度0.01mm
}P_RefStruct;
/**************************通信时所有命令的数据部分必须严格按照数据结构的顺序，包括预留字节*************************/
/*轴动作命令结构体*/
typedef struct
{//20Byte
    int32_t pos;								//位置，单位0.01mm；如果轴为气阀时，0表示不输出 1表示输出
    int32_t advEndDis;					//提前结束距离，单位0.01mm
    int32_t advCSpeedDis;				//提前变速距离，单位0.01mm
    uint8_t  axis;								//轴编号，0-X1，1-Y1，2-Z1，3-C，4-Y2，5-Z2
    uint8_t  speed;							//速度，1~100，单位1%；如果轴为气阀时，1-40表示输出口编号
    uint8_t  advEndFlag;					//提前结束标志，0不使用，1使用
    uint8_t  advCSpeedFlag;			//提前变速标志，0不使用，1提前加速，2提前减速
    uint8_t  advCSpeedSpeed;			//提前改变速度，1~100，单位1%
    uint8_t  referPointNum;			//参考点编号，0不指定，1-100指定的点编号
    uint8_t  ret[2];
}P_AxisMoveStruct;

/*卡爪动作命令结构体*/
typedef struct
{//4Byte
    uint8_t  outportNum;					//输出端口号
    uint8_t  type;								//0松开,1夹紧
    uint8_t  ret[2];
}P_ClawActionStruct;

/*信号检测-卡爪检测命令结构体*/
typedef struct
{//4Byte
    uint8_t  outportNum;					//输出端口号，互锁设置的正向阀输出编号
    uint8_t  type;								//当前设定,0-不检测,1-检测有信号,2-检测无信号
    uint8_t  ret[2];
}P_ClawCheckStruct;

/*信号检测-预留检测命令结构体*/
typedef struct
{//4Byte
    uint8_t  inportNum;					//输入端口号
    uint8_t  type;								//当前设定,0-不检测,1-检测有信号,2-检测无信号
    uint8_t  ret[2];
}P_ReserveCheckStruct;

/*机床-输出动作命令结构体*/
typedef struct
{//4Byte
    uint8_t  outportNum;					//输出端口号
    uint8_t  type;								//0松开,1夹紧
    uint8_t  ret[2];
}P_MachineOutStruct;

/*堆叠-堆叠清零命令结构体*/
typedef struct
{//4Byte
    uint16_t number;							//堆叠个数设置
    uint8_t  stackNum;						//堆叠组号号，1-8
    uint8_t  ret[1];
}P_StackResetZeroStruct;

/*堆叠-堆叠组运动命令结构体*/
typedef struct
{//40Byte，料仓命令有3个轴
    int32_t advEndDis[STACK_AXIS_NUM];			//提前结束距离，单位0.01mm
    int32_t advCSpeedDis[STACK_AXIS_NUM];		//提前变速距离，单位0.01mm
    uint8_t  stackNum;												//堆叠组号，1-8
    uint8_t  ret1[3];
    uint8_t  advEndFlag[STACK_AXIS_NUM];			//提前结束标志，0不使用，1使用
    uint8_t  ret2[1];
    uint8_t  advCSpeedFlag[STACK_AXIS_NUM];	//提前变速标志，0不使用，1提前加速，2提前减速
    uint8_t  ret3[1];
    uint8_t  advCSpeedSpeed[STACK_AXIS_NUM];	//提前改变速度，1~100，单位1%
    uint8_t  ret4[1];
}P_StackMoveStruct;

/*堆叠-放料跟随运动命令结构体*/
typedef struct
{//36Byte，料仓命令有3个轴
    int32_t advEndDis[STACK_AXIS_NUM];			//提前结束距离，单位0.01mm
    int32_t advCSpeedDis[STACK_AXIS_NUM];		//提前变速距离，单位0.01mm
    uint8_t  stackNum;												//堆叠组号，1-8
    uint8_t  ret1[3];
    uint8_t  advEndFlag[STACK_AXIS_NUM];			//提前结束标志，0不使用，1使用
    uint8_t  ret2[1];
    uint8_t  advCSpeedFlag[STACK_AXIS_NUM];	//提前变速标志，0不使用，1提前加速，2提前减速
    uint8_t  ret3[1];
    uint8_t  advCSpeedSpeed[STACK_AXIS_NUM];	//提前改变速度，1~100，单位1%
    uint8_t  ret4[1];
}P_StackFollowStruct;

/*预留输出动作命令结构体*/
typedef struct
{//8Byte
    uint16_t	interval;						//间隔次数，0-65535
    uint8_t  outportNum;					//输出端口号
    uint8_t  function;						//输出功能，0普通输出 1预留通时 2预留反转
    uint8_t  type;								//0断 1通 2反转
    uint8_t  ret[3];
}P_ReserveOutStruct;

/*信号等待-等待机床命令结构体*/
typedef struct
{//4Byte
    int16_t backListNum;				//返回步号，-999~999，指增量值
    uint8_t  inportNum;					//输入端口号，1-60
    uint8_t  inportSta;					//输入状态，0无信号 1有信号
    uint8_t  type;								//跳转类型，0返回步号，1返回标签
    uint8_t	label;							//返回标签号，0无标签，1-99标签号
    uint8_t  ret[2];
}P_WaitInMachineStruct;

/*信号等待-等待卡爪命令结构体*/
typedef struct
{//4Byte
    int16_t backListNum;				//返回步号，-999~999，指增量值
    uint8_t  outportNum;					//输出端口号，1-40
    uint8_t  inportSta;					//输入状态，0无信号 1有信号
    uint8_t  type;								//跳转类型，0返回步号，1返回标签
    uint8_t	label;							//返回标签号，0无标签，1-99标签号
    uint8_t  ret[2];
}P_WaitInClawStruct;

/*信号等待-等待预留命令结构体*/
typedef struct
{//4Byte
    int16_t backListNum;				//返回步号，-999~999，指增量值
    uint8_t  inportNum;					//输入端口号，1-60
    uint8_t  inportSta;					//输入状态，0无信号 1有信号
    uint8_t  type;								//跳转类型，0返回步号，1返回标签
    uint8_t	label;							//返回标签号，0无标签，1-99标签号
    uint8_t  ret[2];
}P_WaitInReserveStruct;

/*其他-延时命令结构体*/
typedef struct
{//4Byte
    uint8_t	res[4];
}P_OtherDelayStruct;

/*其他-报警自定义命令结构体*/
typedef struct
{//4Byte
    uint16_t alarmNum;							//故障编号，90-99紧急报警 490-499急停报警 950-999普通报警 1450-1499提示报警 1950-1999提示
    uint8_t  type;									//故障类型，1紧急报警 2急停报警 3普通报警 4提示报警 5提示
    uint8_t  ret[1];
}P_OtherAlarmCustStruct;

/*其他-周期停止命令结构体*/
typedef struct
{//4Byte
    uint16_t cycleNum;							//周期数，0当前周期结束后停止 1-65535按周期数停止
    uint8_t  ret[2];
}P_OtherCycStopStruct;

/*标签命令结构体*/
typedef struct
{//4Byte
    uint16_t labelNum;							//标签号，0无标签，1-99标签号
    uint8_t  ret[2];
}P_LabelStruct;

/*逻辑&变量-如果-如果命令结构体*/
typedef struct
{//20Byte，默认条件为2个
    int32_t sufferCmpValue[REQUIRE_TOTAL_NUM];		//被比较对象为常量时的常量值；被比较对象为位置时的位置值，单位0.01mm；

    uint8_t  reqSelectFlag[REQUIRE_TOTAL_NUM];			//条件选中标志，0不选择 1并且 2或者
    uint8_t  cmpType[REQUIRE_TOTAL_NUM];						//比较类型，0输入信号 1-20变量 21-40堆叠组方式1 41-60堆叠组方式2 61-80轴 81-100定时器 101位置 102实际产量

    uint8_t  inportNum[REQUIRE_TOTAL_NUM];					//比较类型为输入信号，表示输入端口号
    uint8_t  inportType[REQUIRE_TOTAL_NUM];				//比较类型为输入信号，0无信号 1有信号

    uint8_t  cmpMode[REQUIRE_TOTAL_NUM];						//比较方式，0大于，1小于，2大于等于，3小于等于，4不等于，5等于
    uint8_t  sufferCmpType[REQUIRE_TOTAL_NUM];			//被比较对象类型，0常量 1-20变量 21-40堆叠组 61-80轴 81-100定时器 101位置 102实际产量 103堆叠完成
}P_LogicIfStruct;

/*逻辑&变量-如果-否则命令结构体*/
typedef struct
{//4Byte
    uint8_t	res[4];
}P_LogicElseStruct;

/*逻辑&变量-如果-结束命令结构体*/
typedef struct
{//4Byte
    uint8_t	res[4];
}P_LogicEndStruct;

/*逻辑&变量-循环开始命令结构体*/
typedef struct
{//4Byte
    uint32_t	cycNum;								//循环次数
}P_LogicWhileStartStruct;

/*逻辑&变量-循环结束命令结构体*/
typedef struct
{//4Byte
    uint8_t	res[4];
}P_LogicWhileEndStruct;

/*逻辑&变量-变量-变量命令结构体*/
typedef struct
{//8Byte
    int32_t sufferOperValue;			//操作数为常量时的常量值；
    uint8_t	varNum;								//变量编号，1-8
    uint8_t	operMode;							//操作类型，0加 1减 2乘 3除 4赋值 5取余
    uint8_t  sufferOperType;				//操作数类型，0常量 1-20变量 21-40轴 101实际产量
    uint8_t  ret[1];
}P_LogicVarStruct;

/*逻辑&变量-变量-轴命令结构体*/
typedef struct
{//8Byte
    int32_t sufferOperValue;			//操作数为常量时的常量值，单位0.01mm；
    uint8_t	axisNum;							//轴编号，1-6
    uint8_t	operMode;							//操作类型，0加 1减 2乘 3除 4赋值 5取余
    uint8_t  sufferOperType;				//操作数类型，0常量 1-20变量
    uint8_t  ret[1];
}P_LogicAxisStruct;

/*逻辑&变量-变量-堆叠命令结构体*/
typedef struct
{//8Byte
    int32_t sufferOperValue;			//操作数为常量时的常量值；
    uint8_t	stackNum;							//堆叠组号，1-8
    uint8_t	operMode;							//操作类型，0加 1减 2乘 3除 4赋值 5取余
    uint8_t  sufferOperType;				//操作数类型，0常量 1-20堆叠计数
    uint8_t  ret[1];
}P_LogicStackStruct;

/*逻辑&变量-变量-实际产量命令结构体*/
typedef struct
{//8Byte
    int32_t sufferOperValue;			//操作数为常量时的常量值
    uint8_t	productNum;						//实际产量号，1
    uint8_t	operMode;							//操作类型，0加 1减 2乘 3除 4赋值 5取余
    uint8_t  sufferOperType;				//操作数类型，0常量 1-20变量
    uint8_t  ret[1];
}P_LogicCurProductNumStruct;

/*逻辑&变量-定时器命令结构体*/
typedef struct
{//4Byte
    uint8_t	timeNum;							//定时器编号，1-8
    uint8_t	operMode;							//操作类型，0定时开始 1定时结束 2定时清零
    uint8_t  ret[2];
}P_LogicTimeStruct;

/*搜索-感应停止命令结构体*/
typedef struct
{//4Byte
    uint8_t  res[4];
}P_SearchStopStruct;

/*搜索-轴搜索运动命令结构体*/
typedef struct
{//4Byte，默认搜索输入信号为2个
    int32_t	maxPos;								//最大位置，单位0.01mm
    int32_t	advCDis;							//提前位置，单位0.01mm
    int32_t	offsetDis;						//偏移位置，单位0.01mm
    uint8_t	axis;									//轴编号，0-X1，1-Y1，2-Z1，3-C，4-Y2，5-Z2
    uint8_t	posStoreFlag;					//位置记忆，0不使用 1使用
    uint8_t	reachPosAlarmFlag;		//到位报警，0不使用 1使用
    uint8_t	runSpeed;							//运行速度，1-100
    uint8_t	advCSpeed;						//提前速度，1-100
    uint8_t  ret[3];
    uint8_t  inportNum[SEARCH_INPORT_TOTAL_NUM];		//停止信号，0不检测 1-60输入端口号
    uint8_t  inporttype[SEARCH_INPORT_TOTAL_NUM];	//停止信号状态，0-检测无信号,1-检测有信号
}P_SearchAxisMoveStruct;

/*偏移-轴偏移运动命令结构体*/
typedef struct
{//4Byte
    int32_t offsetPos;					//偏移位置，单位0.01mm
    uint8_t  axis;								//轴编号，0-X1，1-Y1，2-Z1，3-C，4-Y2，5-Z2
    uint8_t  speed;							//速度，1~100，单位1%
    uint8_t  ret[2];
}P_OffsetAxisStruct;

/*扭矩保护命令结构体*/
typedef struct
{//4Byte
    uint8_t  axis;								//轴编号，0-X1，1-Y1，2-Z1，3-C，4-Y2，5-Z2
    uint8_t  torqueValue;				//扭矩值，1~100，单位1%
    uint8_t  ret[2];
}P_TorqueGardStruct;

/*伺服停止命令结构体*/
typedef struct
{//4Byte
    uint8_t  axis;								//轴编号，0-X1，1-Y1，2-Z1，3-C，4-Y2，5-Z2
    uint8_t  ret[3];
}P_AxisStopStruct;

/*子程序命令结构体*/
typedef struct
{//4Byte
    uint8_t  sunProNum;					//子程序编号，1-8
    uint8_t  oprMode;						//操作类型，0标准 1自动 2并行一次 3等待子程序完成
    uint8_t  ret[2];
}P_SunProStruct;

/*程序结束命令结构体*/
typedef struct
{//4Byte
    uint8_t  res[4];
}P_ProEndStruct;




/*********************************参数结构体定义********************************/
/**********信号设置**********/
extern uint8_t m_OutPortType[OUT_PORT_TYPE_NUM][2];								//字节1为输出端口号：0不使用 1-40输出口编号
                                                                                //字节2为输出类型：0电平 1单脉冲 2双脉冲
extern uint8_t m_OutportInterlock[OUT_INTERLOCK_NUM][4];				//互锁设置
                                                                        //字节1为正向阀：0不使用 1-40输出口编号
                                                                        //字节2为正向检测：0不使用 1-60输入口编号
                                                                        //字节3为反向阀：0不使用 1-40输出口编号
                                                                        //字节4为反向检测：0不使用 1-60输入口编号
extern uint8_t m_InportFuncDefine[INPUT_TOTAL_NUM];                  //36路输入，24路扩展输入，0默认 1-60输入端口号
extern uint8_t m_OutportFuncDefine[OUTPUT_TOTAL_NUM];                               //24路输出，16路扩展输出，0默认 1-60输出端口号

extern uint8_t m_OutportRelevancy[OUT_PORT_RELEVANCY_NUM][3];								//预留关联
                                                                                    //字节1预留关联输出口1：0不使用 1-40输出口编号
                                                                                    //字节2预留关联输出口2：0不使用 1-40输出口编号
                                                                                    //字节3预留关联方式：0不使用 1正关联 2反关联

extern uint8_t m_OutportReleteOut[OUT_PORT_RELEOUT_NUM][2];									//预留出类型
                                                                                    //字节1预输出类型输出口：0不使用 1-40输出口编号
                                                                                    //字节2预输出类型：0不使用 1停止断开 2链条斜道 3转盘模式
//按键功能定义结构体
typedef struct
{
    uint8_t  keyType;										//按键功能类型，0输出 1输入 2轴
    uint8_t  funcNum;										//功能编号，输出类型时1-40，输入类型时1-60，轴类型时1-6
    uint8_t  oprMode;										//功能状态，输出类型时0断 1通 2翻转，输入类型时0无信号 1有信号，轴类型时0正向 1反向
}D_KeyFuncStruct;
//LED功能定义结构体
typedef struct
{
    uint8_t  ledType;										//按键功能类型，0输出 1输入
    uint8_t  funcNum;										//功能编号，输出类型时1-40，输入类型时1-60
}D_LedFuncStruct;
//高级功能定义结构体
typedef struct
{
    uint8_t  knifeOrigin1Check;					//刀座原点1检测 0关 1开
    uint8_t  knifeOrigin2Check;					//刀座原点2检测 0关 1开
    uint8_t  chuckOriginCheck;						//卡盘复归检测 0关 1开
    uint8_t  stackSaveIn1Check;					//料仓安全入1检测 0关 1开
    uint8_t  stackSaveIn2Check;					//料仓安全入2检测 0关 1开
    uint8_t  stackSaveOutCheck;					//料仓安全出检测 0关 1开
    uint8_t  alarmIn1Check;							//报警输入1检测 0关 1开
    uint8_t  alarmIn2Check;							//报警输入2检测 0关 1开

    uint8_t  emergencyStopCheck;					//急停检测 0关 1开
    uint8_t  pauseStopCheck;							//暂停检测 0关 1常开 2常闭
    uint8_t  pressureCheck;							//气压检测 0关 1开
    uint8_t  remoteAuto;									//远程自动 0关 1开
    uint8_t  remoteStop;									//远程停止 0关 1周期停
    uint8_t  bottomOilLimit;							//底油限 0关 1开
    uint8_t  oilerSwt;										//注油器 0关 1开
    uint8_t  lubPump;										//润滑泵 0关 1开

    uint8_t  processSave1;								//加工安全1 0关 1常开 2常闭
    uint8_t  processSave2;								//加工安全2 0关 1常开 2常闭
    uint8_t  emergencySnapMotorEnable;		//急停断使能 0关 1开
    uint8_t  emergencyStopOut;						//急停输出 0关 1开
    uint8_t  autoLight;									//自动灯 0关 1开
    uint8_t  alarmLight;									//报警灯 0关 1开
    uint8_t  alarmBuzzer;								//报警声 0关 1开
    uint8_t  pauseLight;									//暂停灯 0关 1开

    uint8_t  processFinish1;							//加工完成1 0关 1开
    uint8_t  processFinish2;							//加工完成2 0关 1开
    uint8_t  locateFinish1;							//定位完成1 0关 1开
    uint8_t  locateFinish2;							//定位完成2 0关 1开
    uint8_t  startFinish1;								//启动完成1 0关 1开
    uint8_t  startFinish2;								//启动完成2 0关 1开
    uint8_t  res1[2];										//预留1-2

    uint8_t  mainAxisRotate1;						//主轴旋转1 0关 1开
    uint8_t  mainAxisRotate2;						//主轴旋转2 0关 1开
    uint8_t  mainAxisLocate1;						//主轴定位1 0关 1开
    uint8_t  mainAxisLocate2;						//主轴定位2 0关 1开
    uint8_t  biowAir1;										//吹气1 0关 1开
    uint8_t  biowAir2;										//吹气2 0关 1开
    uint8_t  res2[2];										//预留3-4

    uint8_t  manualChuckIn1;							//手动卡盘1入 0关 1开
    uint8_t  manualChuckIn2;							//手动卡盘2入 0关 1开
    uint8_t  autoDoorCot1;								//自动门控制1 0关 1开
    uint8_t  autoDoorCot2;								//自动门控制2 0关 1开
    uint8_t  res3[4];										//预留5-8
}D_SeniorFuncStruct;


/*高级功能对应的端口号的定义结构体，必须保证4字节对其，便于直接拷贝*/
typedef struct
{
    uint8_t  knifeOrigin1CheckPort;					//刀座原点1检测 0关 1开
    uint8_t  knifeOrigin2CheckPort;					//刀座原点2检测 0关 1开
    uint8_t  chuckOriginCheckPort;						//卡盘复归检测 0关 1开
    uint8_t  stackSaveIn1CheckPort;					//料仓安全入1检测 0关 1开
    uint8_t  stackSaveIn2CheckPort;					//料仓安全入2检测 0关 1开
    uint8_t  stackSaveOutCheckPort;					//料仓安全出检测 0关 1开
    uint8_t  alarmIn1CheckPort;							//报警输入1检测 0关 1开
    uint8_t  alarmIn2CheckPort;							//报警输入2检测 0关 1开

    uint8_t  emergencyStopCheckPort;					//急停检测 0关 1开
    uint8_t  pauseStopCheckPort;							//暂停检测 0关 1常开 2常闭
    uint8_t  pressureCheckPort;							//气压检测 0关 1开
    uint8_t  remoteAutoPort;									//远程自动 0关 1开
    uint8_t  remoteStopPort;									//远程停止 0关 1周期停
    uint8_t  bottomOilLimitPort;							//底油限 0关 1开
    uint8_t  oilerSwtPort;										//注油器 0关 1开
    uint8_t  lubPumpPort;										//润滑泵 0关 1开

    uint8_t  processSave1Port;								//加工安全1 0关 1常开 2常闭
    uint8_t  processSave2Port;								//加工安全2 0关 1常开 2常闭
    uint8_t  emergencySnapMotorEnablePort;		//急停断使能 0关 1开
    uint8_t  emergencyStopOutPort;						//急停输出 0关 1开
    uint8_t  autoLightPort;									//自动灯 0关 1开
    uint8_t  alarmLightPort;									//报警灯 0关 1开
    uint8_t  alarmBuzzerPort;								//报警声 0关 1开
    uint8_t  pauseLightPort;									//暂停灯 0关 1开

    uint8_t  processFinish1Port;							//加工完成1 0关 1开
    uint8_t  processFinish2Port;							//加工完成2 0关 1开
    uint8_t  locateFinish1Port;							//定位完成1 0关 1开
    uint8_t  locateFinish2Port;							//定位完成2 0关 1开
    uint8_t  startFinish1Port;								//启动完成1 0关 1开
    uint8_t  startFinish2Port;								//启动完成2 0关 1开
    uint8_t  res1[2];										//预留1-2

    uint8_t  mainAxisRotate1Port;						//主轴旋转1 0关 1开
    uint8_t  mainAxisRotate2Port;						//主轴旋转2 0关 1开
    uint8_t  mainAxisLocate1Port;						//主轴定位1 0关 1开
    uint8_t  mainAxisLocate2Port;						//主轴定位2 0关 1开
    uint8_t  biowAir1Port;										//吹气1 0关 1开
    uint8_t  biowAir2Port;										//吹气2 0关 1开
    uint8_t  res2[2];										//预留3-4

    uint8_t  manualChuckIn1Port;							//手动卡盘1入 0关 1开
    uint8_t  manualChuckIn2Port;							//手动卡盘2入 0关 1开
    uint8_t  autoDoorCot1Port;								//自动门控制1 0关 1开
    uint8_t  autoDoorCot2Port;								//自动门控制2 0关 1开
    uint8_t  res3[4];										//预留5-8
}D_SeniorFuncPortStruct;

/**********安全设置**********/
//机床安全定义结构体
typedef struct
{
    uint8_t  armFallAutoDoor1Swt;					//手臂下降状态自动门1开关 0禁止 1允许
    uint8_t  armFallAutoDoor1SwtSta;				//手臂下降状态自动门1开关状态 0不限制 1开 2关
    uint8_t  armFallAutoDoor2Swt;					//手臂下降状态自动门2开关 0禁止 1允许
    uint8_t  armFallAutoDoor2SwtSta;				//手臂下降状态自动门2开关状态 0不限制 1开 2关
}D_MachineSaveStruct;
//料仓安全定义结构体
typedef struct
{
    uint8_t  stackSaveIn1Roatore;					//料仓安全入1恢复，0自动启动 1手动启动
    uint8_t  stackSaveIn2Roatore;					//料仓安全入2恢复，0自动启动 1手动启动
}D_StackSaveStruct;
//卡爪安全定义结构体
typedef struct
{
    uint8_t  clawKeepoutSta;						//卡爪禁止区状态，0不使用 1正传 2反转 3不限制
    uint8_t  axisSelect_X;							//卡爪1的X轴选择，0不选择 1-6轴编号
    uint8_t  axisSelect_Z;							//卡爪1的Z轴选择，0不选择 1-6轴编号
    int32_t clawKeepoutMinX;					//卡爪禁止区X1轴范围的最小坐标，精度0.01mm
    int32_t clawKeepoutMaxX;					//卡爪禁止区X1轴范围的最大坐标，精度0.01mm
    int32_t clawKeepoutHighZ;					//卡爪禁止区Z1高度，精度0.01mm

    int32_t cKeepoutMinX;							//C轴禁止区X1轴范围的最小坐标，精度0.01mm
    int32_t cKeepoutMaxX;							//C轴禁止区X1轴范围的最大坐标，精度0.01mm
    int32_t cKeepoutHighZ;						//C轴禁止区Z1高度，精度0.01mm
    int32_t cKeepoutPosMinC;					//C轴禁止区正转范围的最小坐标，精度0.01mm
    int32_t cKeepoutPosMaxC;					//C轴禁止区正传范围的最大坐标，精度0.01mm
    int32_t cKeepoutNegMinC;					//C轴禁止区反转范围的最小坐标，精度0.01°/mm
    int32_t cKeepoutNegMaxC;					//C轴禁止区反传范围的最大坐标，精度0.01°/mm
}D_ClawSaveStruct;

/*联机安全定义结构体*/
typedef struct
{
    uint8_t  mainSunMode;							//主从模式，0主机 1从机
    uint16_t pluseTime;								//脉冲时间，精度0.01s
    uint8_t  onlineSelect;							//联机选择，0外部联机 1内部联机
    uint8_t  axisNum;									//轴编号，0不选择 1-6轴编号
    uint8_t  areaInNum;								//区域输入编号，1-60
    uint8_t  areaOutNum;								//区域输出编号，1-40
    uint8_t  requestInNum;							//请求输入编号，1-60
    uint8_t  requestOutNum;						//请求输出编号，1-40
    int32_t a1A2MainPos;							//A1A2范围靠近小坐标位置，精度0.01mm
    int32_t a1A2SunPos;								//A1A2范围靠近大坐标位置，精度0.01mm
}D_OnlineSaveStruct;
/**********产品设置**********/
//产品设置定义结构体
typedef struct
{
    uint16_t waitProcessFinTime;				//等待加工完成时间，精度0.1s
    uint16_t pluseOutTime;							//脉冲输出时间，精度0.1s
    uint16_t airVlaveLimitTime;				//阀限时间，精度0.1s
    uint16_t autoCycTime;							//自动周期时间，精度0.1s
    uint16_t alarmBuzzerTime;					//报警响时间，精度0.1s
    uint16_t chainReachDelay;					//链条到位延时，精度0.1s
    uint16_t clampCheckFilterTime;			//夹检测滤波时间，精度0.1s
}D_ProductSetStruct;
//产品设置高级功能定义结构体
typedef struct
{
    uint16_t lubricOpenTime;						//润滑开启时间，精度0.1s
    uint16_t lubricCloseTime;					//润滑关闭时间，精度0.1s
    uint16_t lubricDelay;							//润滑延时，精度0.1s
    uint16_t lubricGapCyc;							//润滑间隔周期，1个
    uint8_t  productMemoryFlag;				//产品记忆，0不使用 1使用
}D_ProductSeniorStruct;
//物联网功能定义结构体
typedef struct
{
    uint8_t  netCommType;							//主板通信，0物联网 1机床通信
    uint8_t  parityMode;								//奇偶校验，0无校验 1奇校验 2偶校验
    uint8_t  baudMode;									//波特率，0:2400 1:4800 2:9600 3:19200 4:38400 5:57600 6:115200
}D_InternetStruct;

/**********伺服速度、伺服安全点、机器参数，该3个模块中参数以功能进行定义，有混用情况**********/
/*轴运动参数定义结构体*/
typedef struct
{
    uint8_t  axisType;						//轴类型，0无 1伺服 2气阀
    uint8_t  axisMoveMade;				//轴运动模式，0绝对位移 1相对位移

    uint16_t accTime;						//加速时间，精度0.01s
    uint16_t decTime;						//减速时间，精度0.01s
    uint16_t accAcc;							//加加速，精度1
    uint16_t decDec;							//减减速，精度1
    uint16_t maxSpeed;						//最大速度，精度1rpm
    int32_t axisMinPos;					//轴最小坐标，精度0.01mm
    int32_t axisMaxPos;					//轴最大坐标，精度0.01mm

    int32_t circlePluseNum;			//单圈脉冲数，精度1p
    int32_t circleDis;					//单圈距离，精度0.01mm

    uint16_t findOriginSpeed;		//找原点速度，1rpm
    uint16_t leaveOriginSpeed;		//离开原点速度，1rpm
    int32_t originOffset;				//原点偏移，精度0.01mm
    uint8_t  originDir;					//原点方向，0正向 1反向

    uint8_t  backOriginOrder;		//回原点顺序，0不回原点 1-6回原点顺序
    uint8_t  originType;					//原点类型，0无原点 1长原点 2短原点

    uint8_t  limitPosSwt;				//正限位开关，0不使用 1正向 2反向
    uint8_t  limitNegSwt;				//负限位开关，0不使用 1正向 2反向
    uint8_t  coordDir;						//坐标方向，0正向 1反向
}D_AxisParStruct;
/*伺服结构参数定义结构体*/
typedef struct
{
    uint8_t  encoderType;												//编码器类型，0增量式 1绝对式
    uint8_t  encoderBitNum;											//编码器位数，0:2500线 1-8:16位-23位
    uint8_t  servoBrand;													//伺服品牌，0默认 1汇川 2禾川 3迈信 4雷赛 5信捷 6睿能 7台邦 8超川 9纳川 10迪威迅 11科昇
                                                                                    //12红森 13通航 14台达 15普菲得 16埃斯顿 17三菱 18西门子 19时代超群 20正弦 21久同 22大族
                                                                                    //23日鼎 24施耐德 25麦格米特
    uint8_t  servoModel;													//伺服型号，0默认型号 个别伺服有多个型号

    uint8_t  extendIOBoard;											//扩展IO开关，0无 1:1块扩展板
    uint8_t  rotateAxisUnit;											//旋转轴单位，0:mm 1:°
    uint8_t  resetSunProFlag;										//复位子程序使用，0不使用 1使用
    uint8_t  torqueLimitFlag;										//扭矩限制，0不使用 1使用
}D_ServoParStruct;
/*伺服安全及控制精度参数定义结构体*/
typedef struct
{
    int32_t tolerance;													//容差，精度0.01mm
    int32_t originDegree;												//原点精度，精度0.01mm
    uint16_t actionFinDegree;										//动作确认，精度0.01mm

    int32_t saveRampageHige_Z1;									//Z1轴安全横行高度，精度0.01mm
    int32_t saveRampageHige_Z2;									//Z1轴安全横行高度，精度0.01mm
    int32_t saveRotatePos_Y1;										//旋转安全Y1轴位置，精度0.01mm
    int32_t saveRotatePos_Y2;										//旋转安全Y2轴位置，精度0.01mm
}D_DegreeParStruct;
/*安全区参数定义结构体*/
typedef struct
{
    uint16_t saveAreaSwt;										//安全区开关，0关 1开
    uint8_t  processFinishNum;								//加工完成输入口，0不使用 1-60输入口编号
    uint8_t  axisSelect[SAVE_AXIS_NUM];			//安全区轴选择，0不选择 1-6轴编号
    int32_t machineA1Pos[SAVE_AXIS_NUM];		//机床点A1坐标，精度0.01mm
    int32_t machineA2Pos[SAVE_AXIS_NUM];		//机床点A2坐标，精度0.01mm
    int32_t starckB1Pos[SAVE_AXIS_NUM];			//料仓点B1坐标，精度0.01mm
    int32_t starckB2Pos[SAVE_AXIS_NUM];			//料仓点B2坐标，精度0.01mm

    int32_t machine_Z_WaitMaxPos;						//Z轴机床区最大等待位置，精度0.01mm
    int32_t machine_Z_FallMaxPos;						//Z轴机床区最大下降位置，精度0.01mm
    int32_t machine_Z_InsideHigh;						//Z轴机床内高度，精度0.01mm
    int32_t stack_Z_StartMaxPos;						//Z轴料仓区最大允许启动位置，精度0.01mm
    int32_t stack_Z_FallMaxPos;							//Z轴料仓区最大下降位置，精度0.01mm
}D_SaveAreaStruct;
/*伺服通信协议参数定义结构体*/
#define SERVE_COMM_PAR_NUM			100		//伺服通信参数最大个数
typedef struct
{
    uint8_t 	saveFlag;				//参数保存，0不保存 1保存
    uint8_t 	powerOnSet;			//开机设置，0不设置 1设置
    uint8_t 	axis;						//轴编号，0无效 1-6轴编号
    uint16_t index;					//索引
    uint8_t  sunIndex;				//子索引
    uint8_t  parLen;					//参数长度
    int32_t parValue;				//参数数值
}D_ServoCommParSetStruct;
/*堆叠信息参数定义结构体，注：如果式旋转料仓，那么C轴的参数存在X轴参数上*/
typedef struct
{
    uint8_t 	stackOrder;														//堆叠顺序，0:Y-X-Z 1:X-Y-Z 2:Z-Y-X 3:Z-X-Y
    uint8_t 	countMode;														//计数方式，计数方式，0:不使用 1程序结束 2命令结束
    uint8_t 	stackFinStopFlag;											//堆叠完成停止，0不使用 1使用
    uint8_t  groupLeaveBlank;											//堆叠组留空方式，0不使用 1首位不取 2末位不放
    uint8_t  stackMoveMode;												//堆叠运动方式，0不联动 1三轴联动

    int32_t axisSelect[STACK_AXIS_NUM];						//堆叠轴选择，0不选择 1-6轴编号
    int32_t axisSpeed[STACK_AXIS_NUM];						//堆叠轴速度，1-100
    int32_t stackPointNum[STACK_AXIS_NUM];				//堆叠点数，1-100
    int32_t stackDir[STACK_AXIS_NUM];							//堆叠方向，0正向 1反向
    int32_t dischangeSpeed[STACK_AXIS_NUM];				//放料速度，1-100

    int32_t stackStartPos[STACK_AXIS_NUM];				//起点(A)，精度0.01mm
    int32_t stack_X_EndPos[STACK_AXIS_NUM];				//横向-X方向终点(B)，精度0.01mm
    int32_t stack_Y_EndPos[STACK_AXIS_NUM];				//纵向-Y方向终点(C)，精度0.01mm
    int32_t stackDiagonalPos[STACK_AXIS_NUM];			//起点对角点(D)，精度0.01mm

    int32_t dischangeStartPos[STACK_AXIS_NUM];		//放料起始点，精度0.01mm
    int32_t intevalDis[STACK_AXIS_NUM];						//间距，精度0.01mm
    int32_t offsetDis[STACK_AXIS_NUM];						//偏移，精度0.01mm
}D_StackInfoStruct;

/*堆叠功能参数定义结构体*/
typedef struct
{
    uint8_t 	stackType;											//堆叠方式，0三点式 1四点式 2普通式
    uint8_t  groupStack;											//按组堆叠，0不使用 1-5模式
    uint8_t  rowStack;												//按行堆叠，0不使用 1-2模式
    uint8_t  storeyStack;										//按层堆叠，0不使用 1-2模式
    uint8_t  divideStack;										//分组堆叠，0不使用 1-2模式
    uint8_t  stackMemory;										//堆叠记忆，0不使用 1使用
    uint8_t  siloType;												//料仓类型，0平板式料仓 1旋转式料仓
    uint8_t  rotateSiloFinHint;							//旋转料仓完成提示，0不使用 1使用
    uint8_t  rotateSiloPlaceNum;							//旋转料仓工位数，1-255
}D_StackFuncStruct;
/*运行信息定义结构体*/
typedef struct
{
    uint32_t runTime;						//运行时间，精度0.1s
    uint32_t preShootCyc;				//前排周期，精度0.1s
    uint32_t takeShapeCyc;				//成型周期，精度0.1s
    uint32_t fetchTime;					//取物时间，精度0.01s
    uint32_t actualProductNum;		//实际产品个数，1件
}D_RunInfoStruct;

/*运行参数定义结构体*/
typedef struct
{
    uint32_t planProductNum;			//计划产品个数，1件
    uint8_t 	globalSpeed;				//全局速度，1-100
    uint8_t 	breakPointFlag;			//断点开关，0不使用 1使用
    uint8_t 	breakPointProNum;		//断点所在程序，0主程序 1-8子程序
    uint16_t breakPointList;			//断点所在程序行
}D_RunParStruct;



/**********************************变量定义********************************/
extern D_KeyFuncStruct m_KeyFunc[OPR_KEY_NUM];				//按键功能
extern D_LedFuncStruct m_LedFunc[OPR_LED_NUM];				//LED功能

extern D_SeniorFuncStruct m_SeniorFunc;								//高级功能
extern D_SeniorFuncPortStruct m_SeniorFuncPort;				//高级功能对应端口

extern D_MachineSaveStruct m_MachineSave;							//机床安全
extern D_StackSaveStruct m_StackSave;									//料仓安全
extern D_ClawSaveStruct m_ClawSave[CLAW_SAVE_NUM];		//卡爪安全
extern D_OnlineSaveStruct m_OnlineSave[ONLINE_SAVE_NUM];//联机安全

extern D_ProductSetStruct m_ProductSet;								//产品设置
extern D_ProductSeniorStruct m_ProductSenior;					//产品设置高级功能
extern D_InternetStruct m_Internet;										//物联网



extern D_AxisParStruct m_AxisPar[AXIS_TOTAL_NUM];			//轴运动参数
extern D_AxisParStruct m_HandwheelPar;								//手轮运动参数
extern D_ServoParStruct m_ServoPar;										//伺服结构参数
extern D_DegreeParStruct m_DegreeParS;								//伺服安全及控制精度参数
extern D_SaveAreaStruct m_SaveArea[SAVE_AREA_NUM];		//安全区参数

extern D_ServoCommParSetStruct m_ServoCommPar[SERVE_COMM_PAR_NUM];                          //伺服通信协议参数
extern D_StackInfoStruct m_StackInfo[STACK_TOTAL_NUM];                                      //堆叠信息参数
extern D_StackFuncStruct m_StackFunc;														//堆叠功能参数

extern uint8_t m_InPortSta[INPUT_TOTAL_NUM];								//输入状态
extern uint8_t m_OutPortSta[OUTPUT_TOTAL_NUM];							//输出状态
extern int32_t m_AxisCurPos[AXIS_TOTAL_NUM];							//轴当前位置
extern uint8_t  m_AxisCurDir[AXIS_TOTAL_NUM];							//轴当前运动方向
extern uint8_t  m_AxisMoveFlag[AXIS_TOTAL_NUM];						//轴运动标志 0结束运动 1运动中
extern uint8_t  m_AxisMovePauseReq[AXIS_TOTAL_NUM];				//轴运动暂停请求 0无请求 1暂停请求

//机器运行状态宏定义
#define MAC_STA_STOP_INVALID									0				//停止-无效状态(未回原)
#define MAC_STA_STOP_STANDBY									1				//停止-待机中(回原完成)
#define MAC_STA_STOP_TEST											2				//停止-程序调试中

#define MAC_STA_BACK_ORIGIN										10				//机器回原中
#define MAC_STA_ALARM													11				//报警中

#define MAC_STA_MANUAL_INVALID								20				//手动-无效状态(未回原)
#define MAC_STA_MANUAL_STANDBY								21				//手动-待机中(回原完成)
#define MAC_STA_MANUAL_TEST										22				//手动测试中(进入手动测试状态)
#define MAC_STA_MANUAL_PRO_DEBUG							23				//手动-程序调试中

#define MAC_STA_AUTO_INVALID									30				//自动-无效状态(未回原)
#define MAC_STA_AUTO_STANDBY									31				//自动-待机中(回原完成)
#define MAC_STA_RESET													32			//自动-机器复位中
#define MAC_STA_RUN														33			//自动-运行中
#define MAC_STA_PAUSE													34			//自动-暂停中


extern uint8_t m_RobotRunSta;															//机器运行状态
extern uint8_t m_RobotWorkMode;														//机器工作模式
extern uint16_t m_AlarmNum;																//报警编号，0无故障 1-99紧急停止(急停，重新回原)
                                                                                    //100-499急停报警(急停，不需要回原)
extern uint32_t m_TotalProductNum;													//产品总产量---可用于物联网
extern uint32_t m_OilerProductNum;													//打油产量计数                                                                                //500-999普通报警(停止)
                                                                                    //1000-1499提示报警(暂停)
                                                                                    //1500-1999提示(运行状态不变化)

extern P_ProInfoStruct m_ProInfo;																	//程序信息

extern P_ProOrderStruct m_ProOrder[PRO_NUM][PRO_LINE_MAIN];				//程序命令
extern P_RefStruct m_RefPoint[REFERENCE_TOTAL_NUM];								//参考点

extern uint8_t m_OperateProNum;																				//操作的程序编号
extern P_ProOrderStruct m_OperateProOrder[PRO_LINE_MAIN];					//操作的程序
extern uint16_t m_OperateProOrderListNum;															//操作的程序行数

extern P_ProInfoStruct m_ProRunInfo;									//运行信息--当前行号
extern D_RunInfoStruct m_RunInfo;											//运行信息
extern D_RunParStruct m_RunPar;												//运行参数
extern uint32_t m_StackCurPileCnt[STACK_TOTAL_NUM];				//当前每个堆叠组的堆叠计数
extern uint32_t m_VariableCurValue[VAR_TOTAL_NUM];					//当前每个变量的变量值
extern uint32_t m_TimeCurValue[TIME_TOTAL_NUM];						//当前每个定时器的计数值


//释放程序命令的数据指针
extern void g_FreeProOrder(P_ProOrderStruct *proOrder);
//获取程序信息
extern uint8_t g_GetProInfo(uint8_t *data);
//写入程序命令内容
extern uint8_t g_GetProOrderData(uint8_t *data, uint8_t writeMode);
//插入、修改程序命令
extern uint8_t g_InsertProOrder(uint8_t *data);
//删除程序命令
extern uint8_t g_DeleteProOrder(uint8_t proNum, uint16_t list);
//交换程序命令
extern uint8_t g_SwapProOrder(uint8_t proNum, uint16_t list1, uint16_t list2);
//程序命令行操作，删除、分解、组合、上移、下移、屏蔽、取消屏蔽
extern uint8_t g_ProOrderOperate(uint8_t proNum, uint16_t proListNum, uint8_t type, uint16_t data);
//程序轴速度教导
extern uint8_t g_ProOrderSpeedOpe(uint8_t proNum, uint16_t speed);
//读出程序命令内容
extern uint8_t g_ReadProOrderData(uint8_t proNum, uint16_t proListNum, uint8_t *data, uint16_t *dataLen);
//整个程序拷贝
extern uint8_t g_TotalProCopy(P_ProOrderStruct *proOrder_Old, P_ProOrderStruct *proOrder_New);




/**********************************其他需要用到的结构体********************************/
/**********系统设置**********/
typedef struct
{
    uint8_t lan;                         //语言
    uint8_t typeFace;                   //字体
    uint8_t wordSize;                   //字号
    uint8_t keyListen;                  //按键音设置
    uint32_t  backlightTime;               //背光时间
    uint8_t backlightBrightness;        //背光亮度
    QString sysName;                        //系统名称
    uint8_t sysColor;                   //系统颜色
}D_SystemSetStruct;
/*程序名称相关的结构体*/
typedef struct
{
    QString     fileName;               //程序名称(不包含后缀)
    QString 	filePath;				//程序路径
    uint16_t 	index;                  //程序编号
    QString     changeTime;             //修改时间
}D_ProgramNameAndPathStruct;
/*端口自定义*/
typedef struct
{
    QString     modifyName;             //修改名称
    QString 	modifyPort;				//修改端口
}D_PortDefineStruct;
/*名称自定义*/
typedef struct
{
    QString     modifyName;             //修改名称
}D_NameDefineStruct;



extern D_SystemSetStruct m_SystemSet;                       //系统参数

extern QList<D_ProgramNameAndPathStruct> m_ProgramNameAndPath;                  //所有文件信息
extern D_ProgramNameAndPathStruct m_CurrentProgramNameAndPath;                  //当前选中的文件信息
extern uint16_t m_FileNameNum;                                                  //程序文件个数

extern D_PortDefineStruct m_PortDefine[DEFINE_PORT_NUM];                         //端口自定义
extern D_NameDefineStruct m_NameDefine[DEFINE_NAME_NUM];                        //名称自定义




















