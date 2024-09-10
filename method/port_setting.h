#ifndef PORT_SETTING_H
#define PORT_SETTING_H

#include <QString>
#include "iniconfig.h"
#include "cmd.h"

#define LAN_NUM             9                       //语言包个数
#define SENIOR_NUM          48                      //高级功能

//固定名称对应的相关端口号对应的默认索引——宏
#define CLAW_METERIAL_1_CLAMP          0             //卡爪动作——原料1夹紧（输出）
#define CLAW_METERIAL_1_LOOSENED       1             //卡爪动作——原料1松开（输出）
#define CLAW_PRODUCT_1_CLAMP           2             //卡爪动作——成品1夹紧（输出）
#define CLAW_PRODUCT_1_LOOSENED        3             //卡爪动作——成品1松开（输出）
#define CLAW_CLAW_1_CLAMP              4             //卡爪动作——卡爪1正转（输出）
#define CLAW_CLAW_1_LOOSENED           5             //卡爪动作——卡爪1反转（输出）

#define SIGNAL_DETECTION_METERIAL_1_CLAMP_LIMIT           0             //信号检测——原料1夹紧限（输入）
#define SIGNAL_DETECTION_METERIAL_1_LOOSENED_LIMIT         1             //信号检测——原料1松开限（输入）
#define SIGNAL_DETECTION_PRODUCT_1_CLAMP_LIMIT          2             //信号检测——成品1夹紧限（输入）
#define SIGNAL_DETECTION_PRODUCT_1_LOOSENED_LIMIT         3             //信号检测——成品1松开限（输入）
#define SIGNAL_DETECTION_CHUCK_1_CLAMP_LIMIT           10             //信号检测——卡盘1夹紧限（输入）
#define SIGNAL_DETECTION_CHUCK_1_LOOSENED_LIMIT         11             //信号检测——卡盘1松开限（输入）

#define MACHINE_START_PROCESS_1        16            //机床——启动加工1（输出）
#define MACHINE_SPINDLE_ROTATE_1        20            //机床——主轴旋转1（输出）
#define MACHINE_SPINDLE_FIXED_POS_1        17            //机床——主轴定位1（输出）
#define MACHINE_AUTO_DOOR_1_OPEN        8            //机床——自动门1开（输出）
#define MACHINE_AUTO_DOOR_1_CLOSE        9            //机床——自动门1关（输出）
#define MACHINE_CHUCK_1_CLAMP          19             //机床——卡盘1夹紧（输出）
#define MACHINE_CHUCK_1_LOOSENED       11             //机床——卡盘1松开（输出）
#define MACHINE_GASS_1_LOOSENED       10             //机床——吹气1（输出）


#define SIGNAL_WAITING_PROCESS_COMPLETE_1        31            //信号等待——加工完成1（输入）
#define SIGNAL_WAITING_FIXED_POS_COMPLETE_1        32            //信号等待——定位完成1（输入）
#define SIGNAL_WAITING_METERIAL_1_CLAMP_LIMIT           0            //信号等待——原料1夹紧限（输入）
#define SIGNAL_WAITING_METERIAL_1_LOOSENED_LIMIT         1            //信号等待——原料1松开限（输入）
#define SIGNAL_WAITING_PRODUCT_1_CLAMP_LIMIT           2            //信号等待——成品1夹紧限（输入）
#define SIGNAL_WAITING_PRODUCT_1_LOOSENED_LIMIT         3            //信号等待——成品1松开限（输入）
#define SIGNAL_WAITING_CHUCK_1_CLAMP_LIMIT           10            //信号等待——卡盘1夹紧限（输入）
#define SIGNAL_WAITING_CHUCK_1_LOOSENED_LIMIT         11            //信号等待——卡盘1松开限（输入）
#define SIGNAL_WAITING_METERIAL_1_CLAMP           0            //信号等待——原料1夹紧（输出）
#define SIGNAL_WAITING_PRODUCT_1_CLAMP           2            //信号等待——成品1夹紧（输出）
#define SIGNAL_WAITING_CHUCK_1_CLAMP           19            //信号等待——卡盘1夹紧（输出）

#define OTHER_ALARM_LAMP                        7               //其他——报警灯（输出）
#define OTHER_ALARM_SOUND                        14               //其他——报警声（输出）


extern uint8_t outputTypeIndex[OUT_PORT_TYPE_NUM];                    //输出类型对应的下标集合
extern uint8_t outportInterlockIndex[OUT_INTERLOCK_NUM][4];                //互锁设置
extern uint8_t outputRelevancyIndex[OUT_PORT_RELEVANCY_NUM][2];                //预留关联
extern uint8_t outputReleteOutIndex[OUT_PORT_RELEOUT_NUM];                    //预留出类型
extern uint8_t seniorFuncIndex[SENIOR_NUM];                    //高级功能

extern void getCmdIni();                                       //读取配置文件中所有的端口



#endif // PORT_SETTING_H
