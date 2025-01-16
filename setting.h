#ifndef SETTING_H
#define SETTING_H

#include <QWidget>
#include <QCheckBox>
#include "QLabel"
#include "QLineEdit"
#include "QComboBox"
#include "QPushButton"
#include "QTableWidgetItem"
#include "tippasswddialog.h"
#include "customkeyboard.h"
#include "method/iniconfig.h"
#include "method/port_setting.h"
#include "customedit.h"
#include "ioportdialog.h"
#include "keydefinedialog.h"
#include "sigdefinedialog.h"
#include "upgradedialog.h"
#include "logindialog.h"

enum MenuState {
    Operator = 0,  // 操作员
    Admin,         // 管理员
    Senior,        // 高级
    Invisible      // 不可见
};

class MenuItem : public QObject {
    Q_OBJECT

public:
    int id;                 // 确定每个项唯一的标识符
    QString name;           // 菜单项的名称
    MenuState state;        // 菜单项的状态
    QList<MenuItem*> children; // 子菜单项列表

    MenuItem(int id, const QString& name);

signals:
    void stateChanged(MenuState newState);
};

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
//    void machineParaLogic();
    void setupCommunicationConnections();

public slots:
    void slotSettingHome();
    void ShowStackPage();
    void handleLoginModeChanged(LoginMode mode);

private:
    Ui::Setting *ui;
    upgradedialog *UpgradeDialog;

private:
    std::map<int, MenuState> menuStateMap; // store the authority of every MenuItem id
//    std::map<int, QTabWidget*> tabWidgetMap; // store the mapping of each QTabWidget group about MenuItem id/10
//    std::map<int, QWidget*> tabContentMap;     // store the mapping of each tab about MenuItem id
    std::map<int, QString> tabNameMap;
    int currentLoginState = MenuState::Operator;

private:
    void init();
    void initWidgets();

    void setupPortDefine(); // 端口自定义
    void setupNameDefine();
    void setupMenuAuthority();
//    void saveMenuState(const QList<MenuItem *> &menuItems);

    void syncParaToUI(); // 初始化时同步结构体参数到ui上显示

    void updateRegisterCodeDisplay();
    void handleSavePasswd(uint* passwd, const QList<NumberEdit*>& edits, const int suffix);

    void updateTabVisibility();
    //
//    void updateInterLockPortFlag(int index, bool isUsed, bool)

    void SeniorFuncPortSet();
private slots:
    void onMenuStateChanged(MenuState newState);
    void updateAppFont();

public:
    void UpgradeHandle(int click_type);

signals:
    void LOGO_Refresh();
    void monitor_port_refreash();
    void RefreshPortDefineSignals();
    void refreshManualReserve();
    void sysNameChanged(const QString& name);
    void updatemonitorhandcontrol();//更新监视界面手控器界面内容
    void WidgetNameRefresh_signal();//教导界面控件名称需要刷新信号
    void coboxVarSelectVarPreOpItemSet_signal();//教导界面变量类型复选框刷新信号
private:
    //信号设置
    QVector<QComboBox*> outputTypeList;                 //输出类型
    QVector<QCheckBox*> outportInterlockList;           //互锁设置
    QVector<QComboBox*> outportRelevancyList;           //预留关联
    QVector<QComboBox*> outportReleteOutList;           //预留出类型
    QVector<QComboBox*> seniorFuncList;                 //高级设置
    std::array<KeyEdit*, 16> keyEdits;            //按键
    std::array<SigLEDEdit*, 5> sigEdits;            //LED



    //安全设置
    QVector<QComboBox*> machineSafeList;                 //机床安全
    QVector<QComboBox*> stackBinSafeList;                 //料仓安全
//    QVector<QPushButton*> clawSafeList;                     //卡爪安全
//    QVector<QPushButton*> onlineSafeList;                     //联机安全

    //产品设置
    std::array<NumberEdit*, 7> productSet;                  //产品设置
    std::array<NumberEdit*, 4> productSeniorEdit;           //高级
    std::array<QComboBox*, 3> productIot;                   //物联网

    //伺服参数
    std::array<QComboBox*, 8> servoPara;              //结构
    std::array<NumberEdit*, 7> accTimeList;                        //加速时间
    std::array<NumberEdit*, 7> decTimeList;                        //减速时间
    std::array<NumberEdit*, 7> accAccList;                        //加加速
    std::array<NumberEdit*, 7> decDecList;                        //减减速

    //伺服安全点
    QVector<NumberEdit*> toleranceList;                     //容差
    QVector<NumberEdit*> horizontalPosList;                     //横行位置限定

    //机器参数
//    QVector<QPushButton*> machineParaList;                     //机器参数

    //堆叠设置
    std::array<QComboBox*, 8> stackCoboxs;
    //
    struct SafeAreaWidget{
        QComboBox* safeAreaSwt;				//安全区开关，0关 1开
        QComboBox*  processFinishNum;		//加工完成输入口，0不使用 1-60输入口编号
        QComboBox*  axisSelect[3];			//安全区轴选择，0不选择 1-6轴编号 依次为X1、Y1、Z1、C、A/Y2、B/Z2
        NumberEdit* machineA1Pos[3];		//机床点A1坐标，精度0.01mm
        NumberEdit* machineA2Pos[3];		//机床点A2坐标，精度0.01mm
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

    struct MachineParaWidget
    {
        NumberEdit* axisMinPos;					//轴最小坐标，精度0.01mm
        NumberEdit* axisMaxPos;					//轴最大坐标，精度0.01mm
        NumberEdit* circlePluseNum;			//单圈脉冲数，精度1p
        NumberEdit* circleDis;					//单圈距离，精度0.01mm
        QComboBox* coordDir;						//坐标方向，0正向 1反向

        NumberEdit* accTime;						//加速时间，精度0.01s
        NumberEdit* decTime;						//减速时间，精度0.01s
        NumberEdit* accAcc;							//加加速，精度1
        NumberEdit* decDec;							//减减速，精度1
        NumberEdit* maxSpeed;						//最大速度，精度1rpm

        QComboBox* limitPosSwt;				//正限位开关，0不使用 1正向 2反向
        QComboBox* limitNegSwt;				//负限位开关，0不使用 1正向 2反向

        QComboBox* axisType;						//轴类型，0无 1伺服 2气阀
        QComboBox* axisMoveMade;				//轴运动模式，0绝对位移 1相对位移

        QComboBox* originType;					//原点类型，0无原点 1长原点 2短原点
        NumberEdit* findOriginSpeed;		//找原点速度，1rpm
        NumberEdit* leaveOriginSpeed;		//离开原点速度，1rpm
        NumberEdit* originOffset;				//原点偏移，精度0.01mm

        QComboBox* originDir;					//原点方向，0正向 1反向
        QComboBox* backOriginOrder;		//回原点顺序，0不回原点 1-6回原点顺序

        IOOnlineInEdit* limitMin;               //最小限位信号
        IOOnlineInEdit* limitMax;               //最大限位信号
        IOOnlineInEdit* originSignal;               //原点信号
    };
   std::array<MachineParaWidget, 6> machineParaWidgets;


private slots:
   void outputTypeSlots();
   void outportInterlockSlots();
   void outportRelevancySlots();
   void outportReleteOutSlots();
   void seniorFuncSlots();

   void saveKeyAndLEDFuncDefine();            //按键\LED灯

   void saveMachineSafePara(); // 机床安全
   void saveStackBinSafePara(); // 料仓安全
   void saveClawSafePara(int index); // 卡爪安全 index 为组号（从0开始，共四组）
   void saveOnlineSafePara(int index); // 联机安全
   void saveProductSetPara();              //产品设置
   void saveProductSetSeniorPara();              //产品设置——高级
   void saveProductSetInternetPara();              //产品设置——物联网
   void saveServoPara();                   //伺服结构
   void saveAccDecTimeSpeed(int index);              //加减速时间和速度
   void saveSafeAreaPara(); // 保存安全区参数到结构体，index 为安全区的组号（从0开始，共四组）
   void saveTolerancePara();    //容差
   void saveDegreePara(); // 保存伺服安全位置限定参数

   void saveStackFunc(); // 堆叠设置功能
   void saveMachinePara();     //机器参数保存至结构体
   void saveMachineAllPara(int index);      //机器参数保存并下发

   void savePortDefine();    //保存端口自定义

   bool modifyPort(int row, int column);           //端口自定义界面修改端口号
   void on_btnLogoUpdate_clicked();
   // QObject interface
public:
   bool eventFilter(QObject *watched, QEvent *event);
   void RefreshPortDefine();//端口自定义刷新
};

#endif // SETTING_H
