#ifndef USART_H
#define USART_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTimer>
#include "program_popup.h"
#include <cmd.h>
#include <QElapsedTimer>
enum SysSendIndex{
    CMD_OUT_TYPE,    //输出类型
    CMD_INTERLOCK,   //互锁设置
    CMD_IN_FUNC_DEF, //
    CMD_OUT_FUNC_DEF,
    CMD_OUT_RELEVENCY,
    CMD_RELATE_OUT,
    CMD_KEY,
    CMD_SENIOR,
    CMD_SENIOR_PORT,
    CMD_SAVE_MACHINE,
    CMD_SAVE_STACK,
    CMD_SAVE_CALW,
    CMD_SAVE_ONLINE,
    CMD_PRODUCT_PAR,
    CMD_PRODUCT_SENIOR,
    CMD_PRODUCT_INTERNET,
    CMD_SERVO_ACC_DEC,
    CMD_SERVO_MAX_SPEED,
    CMD_SERVO_TOLERANCE,
    CMD_SP_AREA,
    CMD_SP_AXIS_LIMIT,
    CMD_SP_RAMPAGE_LIMIT,
    CMD_MAC_AXIS,
    CMD_MAC_LIMIT_SWT,
    CMD_MAC_STRUCT,
    CMD_MAC_SERVO,
    CMD_MAC_ORIGIN,
    CMD_STACK_PAR,
    CMD_STACK_POINT,
    CMD_STAC_SET,
    CMD_END,
    CMD_FINISH,
    CMD_SENDERROR
};
typedef struct
{
    int32_t Pos_x;
    int32_t Pos_y;
    int32_t Pos_z;
    int32_t Pos_c;
    int32_t Pos_y2;
    int32_t Pos_z2;
}AxisCurPos;
extern AxisCurPos m_AxisCurPos;
//表示命令头的字节数
#define HEAD_BYTE  2
//表示长度的字节数
#define COUNT_BYTE 2
//表示命令的字节数
#define CMD_BYTE   2
//表示校验和字节数
#define CHECK_BYTE    1
/*数据头*/
#define EXTEND_HEAD_1										0xA5        //协议头1
#define EXTEND_HEAD_2										0x5A        //协议头2
/*UART接收数据最大的数据帧数以及每帧数据最大的长度*/
#define UART_REV_MAX_FRAME 8
#define EXTEND_BUF_LEN 127
/*函数返回值*/
#define UART_OK 0
#define UART_ERROR -1

//读取及应答命令宏定义
#define CMD_MAIN_READ												0x80      	//读取及应答主命令
#define CMD_SUN_READ_PAR										0x01      	//读参数、状态、实时数据、程序数据命令
#define CMD_SUN_READ_PAR_ANSWER							0x02      	//读参数、状态、实时数据命令应答
#define CMD_SUN_READ_PAR_WR									0x03      	//写参数、控制、Flash写入命令应答
#define CMD_SUN_READ_FLASH									0x04      	//读FLASH命令
#define CMD_SUN_READ_FLASH_ANSWER						0x05      	//读FLASH命令应答
//参数读写-信号设置
#define CMD_MAIN_SIGNAL											0x01      	//信号设置
#define CMD_SUN_SIGNAL_OUT_TYPE							0x01      	//输出类型
#define CMD_SUN_SIGNAL_INTERLOCK						0x02      	//互锁设置
#define CMD_SUN_SIGNAL_IN_FUNC_DEF					0x03      	//输入端口自定义
#define CMD_SUN_SIGNAL_OUT_FUNC_DEF					0x04      	//输出端口自定义
#define CMD_SUN_SIGNAL_OUT_RELEVENCY				0x05      	//预留关联
#define CMD_SUN_SIGNAL_RELATE_OUT						0x06      	//预留出类型
#define CMD_SUN_SIGNAL_KEY									0x07      	//按键/信号
#define CMD_SUN_SIGNAL_SENIOR								0x08      	//高级
#define CMD_SUN_SIGNAL_SENIOR_PORT					0x09      	//高级功能端口

//参数读写-安全设置
#define CMD_MAIN_SAVE												0x02      	//安全设置
#define CMD_SUN_SAVE_MACHINE								0x01      	//机床安全
#define CMD_SUN_SAVE_STACK									0x02      	//料仓安全
#define CMD_SUN_SAVE_CALW										0x03      	//卡爪安全
#define CMD_SUN_SAVE_ONLINE									0x04      	//联机安全

//参数读写-产品设置
#define CMD_MAIN_PRODUCT										0x03      	//产品设置
#define CMD_SUN_PRODUCT_PAR									0x01      	//产品参数
#define CMD_SUN_PRODUCT_SENIOR							0x02      	//高级
#define CMD_SUN_PRODUCT_INTERNET						0x03      	//物联网

//参数读写-伺服速度
#define CMD_MAIN_SERVO											0x04      	//伺服速度
#define CMD_SUN_SERVO_ACC_DEC								0x01      	//加减速时间
#define CMD_SUN_SERVO_MAX_SPEED							0x02      	//最大速度
#define CMD_SUN_SERVO_TOLERANCE							0x03      	//容差

//参数读写-伺服安全点
#define CMD_MAIN_SP													0x05      	//伺服安全点
#define CMD_SUN_SP_AREA											0x01      	//安全区
#define CMD_SUN_SP_AXIS_LIMIT								0x02      	//轴运动位置限定
#define CMD_SUN_SP_RAMPAGE_LIMIT						0x03      	//横行位置限定

//参数读写-机器参数
#define CMD_MAIN_MAC												0x06      	//机器参数
#define CMD_SUN_MAC_AXIS										0x01      	//轴参数
#define CMD_SUN_MAC_LIMIT_SWT								0x02      	//限位开关
#define CMD_SUN_MAC_STRUCT									0x03      	//结构
#define CMD_SUN_MAC_SERVO										0x04      	//伺服参数
#define CMD_SUN_MAC_ORIGIN									0x05      	//原点

//参数读写-堆叠参数
#define CMD_MAIN_STACK											0x07      	//堆叠参数
#define CMD_SUN_STACK_PAR										0x01      	//堆叠基础参数
#define CMD_SUN_STACK_POINT									0x02      	//堆叠点位参数
#define CMD_SUN_STACK_SET										0x03      	//堆叠设置

//运行状态读取及相关参数设置
#define CMD_MAIN_STA												0x10      	//运行状态读取及相关参数设置
#define CMD_SUN_STA_IN_OUT									0x01      	//输入输出状态读取
#define CMD_SUN_STA_AXIS_POS								0x02      	//轴当前位置读取
#define CMD_SUN_STA_MAC											0x03      	//控制器状态读取
#define CMD_SUN_STA_INFO										0x04      	//运行信息读取
#define CMD_SUN_STA_PAR											0x05      	//运行参数读写
#define CMD_SUN_STA_STACK										0x06      	//堆叠实时参数读写
#define CMD_SUN_STA_VAR											0x07      	//变量值读写
#define CMD_SUN_STA_TIME										0x08      	//定时器读写
#define CMD_SUN_STA_CLEAR_ALARM                                 0x09        //清除报警
#define CMD_SUN_STA_UPGRADE_STATE                               0x0D        //控制器升级状态读取

//手动操作
#define CMD_MAIN_MANUAL											0x11      	//手动操作
#define CMD_SUN_MANUAL_OUT									0x01      	//输出控制
#define CMD_SUN_MANUAL_AXIS									0x02      	//轴连动
#define CMD_SUN_MANUAL_KEY									0x03      	//按键操作
#define CMD_SUN_MANUAL_HAND_WHEEL						0x04      	//手轮
#define CMD_SUN_MANUAL_INCREMENT						0x05      	//增量
#define CMD_SUN_MANUAL_STACK								0x06      	//移至堆叠点
#define CMD_SUN_MANUAL_UPGRADE_CONTROL                          0x07        //主控板升级控制指令
#define CMD_SUN_MANUAL_UPGRADE_DATE                              0x08        //主控板升级数据指令
#define CMD_SUN_SYSDATA_FINISH                                   0x09        //开机参数同步完成下发
#define CMD_SUN_PAR_RESET                                        0x0A        //自动界面参数清零
#define CMD_SUN_ROTAT_SILO_SET                                   0x0B        //旋转料仓设置指令
#define CMD_SUN_VAR_AUTO_RESET                                   0x0C        //启动自动时变量清零
#define CMD_SUN_VAR_POWER_OFF_MEMORY                             0x0D        //变量断电记忆设置

//程序编辑
#define CMD_MAIN_PRO												0x15      	//程序编辑
#define CMD_SUN_PRO_INFO										0x01      	//程序信息读写
#define CMD_SUN_PRO_REF											0x02      	//参考点读写
#define CMD_SUN_PRO_ORDER										0x03      	//命令行读写
#define CMD_SUN_PRO_INSERT									0x04      	//命令行插入、修改
#define CMD_SUN_PRO_DELET										0x05      	//命令行删除、分解、组合、上移、下移、屏蔽、取消屏蔽
#define CMD_SUN_PRO_SPEED										0x06      	//程序轴速度教导
#define CMD_SUN_PRO_OUT											0x07      	//程序信号输出
#define CMD_SUN_PRO_DEBUG										0x08      	//程序试行控制
#define CMD_SUN_PRO_START										0x09      	//程序启停控制
#define CMD_SUN_PRO_MODE										0x0A      	//设备模式切换
#define CMD_SUN_PRO_SAVE										0x0B      	//保存、切换操作的程序
#define CMD_SUN_STA_VAR_TYPE                                    0x0C        //变量类型读写
//程序标志
#define PRO_FLAG_CMDLINE_03                                     3              //程序命令行读写
#define PRO_FLAG_CMDLINE_04                                     4              //程序命令行插入、停止修改、自动修改
#define PRO_FLAG_CMDLINE_04_INSERT                                         0               //插入
#define PRO_FLAG_CMDLINE_04_STOP_MODIFY                                    1               //停止修改
#define PRO_FLAG_CMDLINE_04_AUTO_MODIFY                                    2               //自动修改
#define PRO_FLAG_CMDLINE_05                                     5              //程序行删除/组合/分解/上移/下移/屏蔽
#define PRO_FLAG_CMDLINE_05_DEL                                 1               //删除
#define PRO_FLAG_CMDLINE_05_COMBIN                                 2               //组合
#define PRO_FLAG_CMDLINE_05_DECOMPOSE                                 3               //分解
#define PRO_FLAG_CMDLINE_05_UP                                 4               //上移
#define PRO_FLAG_CMDLINE_05_DOWN                                 5               //下移
#define PRO_FLAG_CMDLINE_05_SHIELD                                 6               //屏蔽



class Usart:public QObject
{
    Q_OBJECT
public:
    Usart(QObject *parent = nullptr);
    ~Usart();

public:
    uint16_t openSerialPort();
    void closeSerialPort();
    QSerialPort *m_serialPort;
    QTimer *UsartTimer;

signals:
    void receivedData(const QByteArray &data);
    void DataSycStateSignal(uint8_t SysSend_Index);
    void SysNextDataSignal();
    void posflashsignal(AxisCurPos data);
    void robotstaRefreshsignal();//机器状态参数更新
public slots:
    void onReadyRead();
    void onHandleError(QSerialPort::SerialPortError error);
    void APP_Uart_SendData(const QByteArray &data);
    void ExtendReadParDeal(char mainCmd, char sunCmd, const QByteArray &recDataBuf,int dataLen);                   //解析参数处理函数
public:
   void ExtendSendParDeal(uint8_t mainCmd, uint8_t sunCmd, uint16_t parNum = 0, uint16_t parNum2 = 0);     //下发参数处理函数

   void ExtendSendManualOperationDeal(uint8_t mainCmd, uint8_t sunCmd, uint16_t parNum = 0, uint16_t parNum2 = 0);      //手动操作下发处理函数


   void ExtendReadStaDeal(uint8_t mainCmd, uint8_t sunCmd, uint8_t *recDataBuf);     //读取状态处理函数

   void ExtendSendProDeal(uint8_t mainCmd, uint8_t sunCmd, uint16_t parNum = 0, uint16_t parNum2 = 0, uint16_t parNum3 = 0);     //下发程序编辑处理函数

   uint8_t  DataSyc();
   void sync_data_handle(void);
public:

    uint16_t SUM_CHK(uint8_t *data, uint16_t len, uint8_t sum);
    uint16_t ExtendSendDataDeal(uint8_t *data, uint16_t len);
    void ExtendSendParProReadAnswer(uint8_t mainCmd, uint8_t sunCmd, uint8_t *recDataBuf, uint8_t dataLen);

    void GetProData(uint16_t parNum, uint16_t parNum2, uint8_t* sendDataBuf, uint16_t &len, uint16_t cmdLineType, uint16_t operateType = 0);
    void GetProData1(uint16_t parNum, uint16_t parNum2, uint8_t* sendDataBuf, uint16_t &len, uint16_t cmdLineType, uint16_t operateType = 0);

};

#endif // USART_H
