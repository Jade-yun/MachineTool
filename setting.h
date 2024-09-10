#ifndef SETTING_H
#define SETTING_H

#include <QWidget>
#include <QCheckBox>
#include "QLabel"
#include "QLineEdit"
#include "QComboBox"
#include "QPushButton"
#include "QTableWidgetItem"

#include "customkeyboard.h"
#include "method/iniconfig.h"
#include "method/port_setting.h"
#include "customedit.h"
#include "ioportdialog.h"

namespace Ui {
class Setting;
}

class Setting : public QWidget
{
    Q_OBJECT

public:
    explicit Setting(QWidget *parent = nullptr);
    ~Setting();

private:

    void pageSwitchInit();
    void machineParaLogic();
    void controlInitialization();               //控件初始化

public slots:
    void slotSettingHome();
    void ShowStackPage();

private:
    Ui::Setting *ui;

private:
    void init();
    void readFromConfigFile();
    void writeToConfigFile();

private:
    //信号设置
    QVector<QComboBox*> outputTypeList;                 //输出类型
    QVector<QCheckBox*> outportInterlockList;           //互锁设置
    QVector<QComboBox*> outportRelevancyList;           //预留关联
    QVector<QComboBox*> outportReleteOutList;           //预留出类型
    QVector<QComboBox*> seniorFuncList;                 //高级设置
    //
    struct SafeAreaWidget{
        QComboBox* safeAreaSwt;				//安全区开关，0关 1开
        QComboBox*  processFinishNum;		//加工完成输入口，0不使用 1-60输入口编号
        QComboBox*  axisSelect[3];			//安全区轴选择，0不选择 1-6轴编号 依次为X1、Y1、Z1、C、A/Y2、B/Z2
        NumberEdit* machineA1Pos[2];		//机床点A1坐标，精度0.01mm
        NumberEdit* machineA2Pos[2];		//机床点A2坐标，精度0.01mm
        NumberEdit* starckB1Pos[2];			//料仓点B1坐标，精度0.01mm
        NumberEdit* starckB2Pos[2];			//料仓点B2坐标，精度0.01mm

        NumberEdit* machine_Z_WaitMaxPos;   //Z轴机床区最大等待位置，精度0.01mm
        NumberEdit* machine_Z_FallMaxPos;   //Z轴机床区最大下降位置，精度0.01mm
        NumberEdit* machine_Z_InsideHigh;   //Z轴机床内高度，精度0.01mm
        NumberEdit* stack_Z_StartMaxPos;    //Z轴料仓区最大允许启动位置，精度0.01mm
        NumberEdit* stack_Z_FallMaxPos;
    };
    std::array<SafeAreaWidget, 4> servoPointSafeArea;

    struct ClawSafeParaWidget {
        QComboBox*  clawKeepoutSta;						//卡爪禁止区状态，0不使用 1正传 2反转 3不限制
        QComboBox*  axisSelect_X;							//卡爪1的X轴选择，0不选择 1-6轴编号
        QComboBox*  axisSelect_Z;							//卡爪1的Z轴选择，0不选择 1-6轴编号
        NumberEdit* clawKeepoutMinX;					//卡爪禁止区X1轴范围的最小坐标，精度0.01mm
        NumberEdit* clawKeepoutMaxX;					//卡爪禁止区X1轴范围的最大坐标，精度0.01mm
        NumberEdit* clawKeepoutHighZ;					//卡爪禁止区Z1高度，精度0.01mm

        NumberEdit* cKeepoutMinX;							//C轴禁止区X1轴范围的最小坐标，精度0.01mm
        NumberEdit* cKeepoutMaxX;							//C轴禁止区X1轴范围的最大坐标，精度0.01mm
        NumberEdit* cKeepoutHighZ;						//C轴禁止区Z1高度，精度0.01mm
        NumberEdit* cKeepoutPosMinC;					//C轴禁止区正转范围的最小坐标，精度0.01mm
        NumberEdit* cKeepoutPosMaxC;					//C轴禁止区正传范围的最大坐标，精度0.01mm
        NumberEdit* cKeepoutNegMinC;					//C轴禁止区反转范围的最小坐标，精度0.01°/mm
        NumberEdit* cKeepoutNegMaxC;					//C轴禁止区反传范围的最大坐标，精度0.01°/mm
    };
    std::array<ClawSafeParaWidget, 4> clawSafeWidgets;

    struct OnlineSafeParaWidget {
        QComboBox*  mainSunMode;							//主从模式，0主机 1从机
        NumberEdit* pluseTime;								//脉冲时间，精度0.01s
        QComboBox*  onlineSelect;							//联机选择，0外部联机 1内部联机
        QComboBox*  axisNum;									//轴编号，0不选择 1-6轴编号
        IOOnlineInEdit*  areaInNum;								//区域输入编号，1-60
        IOOnlineOutEdit*  areaOutNum;								//区域输出编号，1-40
        IOOnlineInEdit*  requestInNum;							//请求输入编号，1-60
        IOOnlineOutEdit*  requestOutNum;						//请求输出编号，1-40
        NumberEdit* a1A2MainPos;							//A1A2范围靠近小坐标位置，精度0.01mm
        NumberEdit* a1A2SunPos;								//A1A2范围靠近大坐标位置，精度0.01mm
    };
    std::array<OnlineSafeParaWidget, 4> onlineSafeWidgets;

private slots:
    void outputTypeSlots(int id);
    void outportInterlockSlots(int id);
    void outportRelevancySlots(int id);
    void outportReleteOutSlots(int id);
    void seniorFuncSlots(int id);

    void saveKeyFuncDefine();
    void saveLEDSignal();
    void saveMachineSafePara(); // 机床安全
    void saveStackBinSafePara(); // 料仓安全
    void saveClawSafePara(int index); // 卡爪安全 index 为组号（从0开始，共四组）
    void saveOnlineSafePara(int index); // 联机安全
    void saveProductSetPara();
    void saveServoPara();
    void saveSafeAreaPara(int index); // 保存安全区参数到结构体，index 为安全区的组号（从0开始，共四组）
    void saveDegreePara(); // 保存伺服安全位置限定参数

    void saveStackFunc(); // 堆叠设置功能
};

#endif // SETTING_H
