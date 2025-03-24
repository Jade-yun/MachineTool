#include "setting.h"
#include "ui_setting.h"
#include <QProcess>
#include <QMovie>
#include <QPair>
#include <QPropertyAnimation>
#include <QDebug>
#include <QElapsedTimer>
#include "RefreshKernelBuffer.h"
#include <QSignalMapper>
#include <QThread>
#include <QRadioButton>
#include <QProgressDialog>
#include <QTranslator>

#include "mainwindow.h"
#include "upgradedialog.h"

#include "errortipdialog.h"
#include "stackedit.h"
#include "beeper.h"
#include "backlighter.h"
#include "usbdisk.h"
#include "timesetter.h"

const QString noteDirPath = "/opt/MachineTool/docs/notepad/";
const QString menuStateConfigPath = "/opt/MachineTool/configs/menustate_config.ini";

QVector<QString> registerCode;

extern QTranslator translator;

MenuItem::MenuItem(int id, const QString& name) :
    id(id), name(name), state(MenuState::Operator) {}


Setting::Setting(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Setting)
{
    ui->setupUi(this);
    // this must be placed at beginning afer setupUI(this).
    for (int i = 0; i < 8; i++)
    {
        stack[i] = new StackEdit(i, ui->tabWidgetStack->widget(i));
        connect(this,&Setting::AxisTypeChange_Signal,stack[i],&StackEdit::StackAxisSelectQcomboboxRefresh);//当轴类型发生改变时，改变堆叠-轴选择复选框
    }
    init();
    initWidgets();
    setupNameDefine();
    setupMenuAuthority();

    syncParaToUI();

    pageSwitchInit();
    setupCommunicationConnections();
    ParNeedSaveCheckConnectHandle();

    connect(ui->tabWidgetSystem, &QTabWidget::tabBarClicked, this, [=](int index){
        int menuIndex = ui->tabWidgetSystem->indexOf(ui->tabMenuAuthority);
        if (menuIndex == -1) return;

        if (index == menuIndex)
        {
            ui->tabMenuAuthority->hide();

            TipPasswdDialog tip(tr("请输入菜单权限密码："));
            int reply = tip.exec();

            if (reply == QDialog::Rejected)
            {
                ui->tabWidgetSystem->setCurrentIndex(0);
                return;
            }

            else if (reply == QDialog::Accepted)
            {
                uint menuPasswd = tip.getPasswd();

                if (menuPasswd != passwd[2])
                {
                    MainWindow::pMainWindow->showErrorTip(tr("密码错误！"));
                    ui->tabWidgetSystem->setCurrentIndex(0);
                    return;
                }
                ui->tabMenuAuthority->show();
            }
        }
    });

    this->setupSystemSetting();

    connect(ui->coboxStackWay, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index){
        auto mode = (StackMode)(index);
        for (int i = 0; i < 8; i++)
        {
            stack[i]->switchStackWay(mode);
        }
    });

    connect(ui->coboxSiloType, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index){
        if (index == 0) // 平板式料仓
        {
//            auto mode = (StackMode)(index);
//            stack[i]->switchStackWay();
            //emit ui->coboxStackWay->currentIndexChanged(ui->coboxStackWay->currentIndex());
            auto mode = (StackMode)(m_StackFunc.stackType);
            for (int i = 0; i < 8; i++)
            {
                stack[i]->switchStackWay(mode);
            }
        }
        else if (index == 1) // 旋转式料仓
        {
            auto mode = StackMode::ROTATE_BIN;
            for (int i = 0; i < 8; i++)
            {
                stack[i]->switchStackWay(mode);
            }
        }
    });
    emit ui->coboxSiloType->currentIndexChanged(ui->coboxSiloType->currentIndex());

    connect(ui->coboxLimitPosX1, QOverload<int>::of(&QComboBox::activated), [=](int index) {
        // 不使用
        if (index == 0)
        {
            m_Port_X[13].functionSet = 0;
        }
        else
        {
            m_Port_X[13].functionSet = 1;
        }
    });
    connect(ui->coboxLimitNegX1, QOverload<int>::of(&QComboBox::activated), [=](int index) {
        // 不使用
        if (index == 0)
        {
            m_Port_X[19].functionSet = 0;
        }
        else
        {
            m_Port_X[19].functionSet = 1;
        }
    });
    connect(ui->coboxLimitPosY1, QOverload<int>::of(&QComboBox::activated), [=](int index) {
        if (index == 0)
        {
            m_Port_X[14].functionSet = 0;
        }
        else
        {
            m_Port_X[14].functionSet = 1;
        }
    });
    connect(ui->coboxLimitNegY1, QOverload<int>::of(&QComboBox::activated), [=](int index) {
        if (index == 0)
        {
            m_Port_X[20].functionSet = 0;
        }
        else
        {
            m_Port_X[20].functionSet = 1;
        }
    });
    connect(ui->coboxLimitPosZ1, QOverload<int>::of(&QComboBox::activated), [=](int index) {
        if (index == 0)
        {
            m_Port_X[15].functionSet = 0;
        }
        else
        {
            m_Port_X[15].functionSet = 1;
        }
    });
    connect(ui->coboxLimitNegZ1, QOverload<int>::of(&QComboBox::activated), [=](int index) {
        if (index == 0)
        {
            m_Port_X[21].functionSet = 0;
        }
        else
        {
            m_Port_X[21].functionSet = 1;
        }
    });
    connect(ui->coboxLimitPosC, QOverload<int>::of(&QComboBox::activated), [=](int index) {
        if (index == 0)
        {
            m_Port_X[16].functionSet = 0;
        }
        else
        {
            m_Port_X[16].functionSet = 1;
        }
    });
    connect(ui->coboxLimitNegC, QOverload<int>::of(&QComboBox::activated), [=](int index) {
        if (index == 0)
        {
            m_Port_X[22].functionSet = 0;
        }
        else
        {
            m_Port_X[22].functionSet = 1;
        }
    });
    connect(ui->coboxLimitPosY2, QOverload<int>::of(&QComboBox::activated), [=](int index) {
        if (index == 0)
        {
            m_Port_X[17].functionSet = 0;
        }
        else
        {
            m_Port_X[17].functionSet = 1;
        }
    });
    connect(ui->coboxLimitNegY2, QOverload<int>::of(&QComboBox::activated), [=](int index) {
        if (index == 0)
        {
            m_Port_X[29].functionSet = 0;
        }
        else
        {
            m_Port_X[29].functionSet = 1;
        }
    });
    connect(ui->coboxLimitPosZ2, QOverload<int>::of(&QComboBox::activated), [=](int index) {
        if (index == 0)
        {
            m_Port_X[18].functionSet = 0;
        }
        else
        {
            m_Port_X[18].functionSet = 1;
        }
    });
    connect(ui->coboxLimitNegZ2, QOverload<int>::of(&QComboBox::activated), [=](int index) {
        if (index == 0)
        {
            m_Port_X[30].functionSet = 0;
        }
        else
        {
            m_Port_X[30].functionSet = 1;
        }
    });
    for (int i = 0; i < AXIS_TOTAL_NUM; i++)
    {
        connect(machineParaWidgets.at(i).originType, QOverload<int>::of(&QComboBox::activated), [=](int index) {

            constexpr int originTypeInputPortMaping[AXIS_TOTAL_NUM] = {
                25, 26, 27, 28, 29, 30
            };
            int portNum = originTypeInputPortMaping[i];
            m_Port_X[portNum].functionSet = index ? 1 : 0;
        });
    }

    //PXC_240914
    setupPortDefine();

    /***************************升级与备份-信号槽连接**********************************/
    connect(ui->btnUpdateHandcontroller, &QPushButton::clicked,this,[=]() {this->UpgradeHandle(HANDHELD);});
    connect(ui->btnUpdateIOBoard, &QPushButton::clicked,this,[=]() {this->UpgradeHandle(IOBOARD);});
    connect(ui->btnUpdateMainboard, &QPushButton::clicked,this,[=]() {this->UpgradeHandle(MAINBOARD);});
    connect(ui->btnUpdateServo, &QPushButton::clicked,this,[=]() {this->UpgradeHandle(SERVO);});
    connect(ui->btnSysDataBackup,&QPushButton::clicked,this,[=](){this->UpgradeHandle(SYSTEM_DATA_COPY);});
    connect(ui->btnBackupDataRestore,&QPushButton::clicked,this,[=](){this->UpgradeHandle(COPY_DATA_REST);});
    connect(ui->btnFactoryDataReset,&QPushButton::clicked,this,[=](){this->UpgradeHandle(RESTSETTING);});

    ui->tabUserSet->installEventFilter(this);
}

Setting::~Setting()
{
    delete ui;
}

void Setting::slotSettingHome(TriMode mode)
{
//    std::map<int, QWidget*> sysTabMap = {
//        {41, ui->tabLangurageSet},
//        {42, ui->tabUserSet},
//        {43, ui->tabUpdateAndBackup},
//        {44, ui->tabNotepad},
//        {45, ui->tabPasswdSet},
//        {46, ui->tabIot},
//        {47, ui->tabSignUp}
//    };

    if (mode == TriMode::AUTO)
    {
        ui->tabWidgetSystem->clear();
        ui->tabWidgetSystem->addTab(ui->tabUpdateAndBackup, tr("升级与备份"));
        ui->tabWidgetSystem->addTab(ui->tabSystemInfo, tr("系统信息"));

        ui->stackedWidget->setCurrentWidget(ui->pageSystem);
    }
    else
    {
//        auto tabState = menuStateMap.at(id)
//        bool shouldShow = ((tabState != MenuState::Invisible) && (currentLoginState >= tabState));
        updateTabVisibility();

        ui->stackedWidget->setCurrentIndex(0);
    }

    bool enabled = mode != TriMode::AUTO;
    ui->btnBackupDataRestore->setEnabled(enabled);
    ui->btnFactoryDataReset->setEnabled(enabled);
    ui->btnUpdateHandcontroller->setEnabled(enabled);
    ui->btnUpdateMainboard->setEnabled(enabled);
    ui->btnUpdateIOBoard->setEnabled(enabled);
    ui->btnUpdateServo->setEnabled(enabled);
}

void Setting::ShowStackPage()
{
    ui->stackedWidget->setCurrentWidget(ui->pageStack);
}

void Setting::handleLoginModeChanged(LoginMode mode)
{
    MenuState tempState;

    if (mode == LoginMode::Operator)
    {
        tempState = MenuState::Operator;
//        ui->tabWidgetSystem->removeTab(ui->tabWidgetSystem->indexOf(ui->tabMenuAuthority));
    }
    else if (mode == LoginMode::Admin)
    {
        tempState = MenuState::Admin;
    }
    else if (mode == LoginMode::Advance)
    {
        tempState = MenuState::Senior;
    }

    currentLoginState = (int)mode;
    updateTabVisibility();

}


// to initial all variables in this function
void Setting::init()
{
    for (NumberEdit* edit : ui->tabWidgetProduct->findChildren<NumberEdit*>())
    {
        edit->setDecimalPlaces(1); // 精度0.1
    }
//    ui->editAutoCycleTime->setDecimalPlaces(0);
    ui->editLubricationIntervalCycle->setDecimalPlaces(0);

    // 系统设置 所有需要调用小键盘的都为整数
    for (NumberEdit* edit : ui->tabWidgetSystem->findChildren<NumberEdit*>())
    {
        edit->setDecimalPlaces(0);
    }
    // 密码设置
    for (NumberEdit* edit : ui->tabPasswdSet->findChildren<NumberEdit*>())
    {
        edit->setInputRange(0, 65535);
    }
    // 物联网
    for (NumberEdit* edit : ui->stkWidgetIPSet->findChildren<NumberEdit*>())
    {
        edit->setInputRange(0, 255);
    }

    // 伺服安全点所有编辑框精度0.01
    for (auto edit : ui->tabWgtServoSafePoint->findChildren<NumberEdit*>())
    {
        edit->setDecimalPlaces(2);
    }
    // 旋转料仓工位数字 1-255
    ui->editRotateSiloPlaceNum->setDecimalPlaces(0);
    ui->editRotateSiloPlaceNum->setInputRange(1, 255);
    // 机器参数 轴参数 单圈脉冲数
    ui->editCirclePulseNumX1->setDecimalPlaces(0);
    ui->editCirclePulseNumY1->setDecimalPlaces(0);
    ui->editCirclePulseNumZ1->setDecimalPlaces(0);
    ui->editCirclePulseNumY2->setDecimalPlaces(0);
    ui->editCirclePulseNumZ2->setDecimalPlaces(0);
    const std::vector<NumberEdit*> editsInteger = {
        ui->editCirclePulseNumX1,ui->editCirclePulseNumY1,ui->editCirclePulseNumZ1,
        ui->editCirclePulseNumC, ui->editCirclePulseNumY2,ui->editCirclePulseNumZ2,
        ui->editAccAccX1, ui->editAccAccY1, ui->editAccAccZ1, ui->editAccAccC, ui->editAccAccY2, ui->editAccAccZ2,
        ui->editDecDecX1, ui->editDecDecY1, ui->editDecDecZ1, ui->editDecDecC, ui->editDecDecY2, ui->editDecDecZ2,
        ui->editMaxSpeedX1, ui->editMaxSpeedY1, ui->editMaxSpeedZ1, ui->editMaxSpeedC, ui->editMaxSpeedY2, ui->editMaxSpeedZ2,
        ui->editFindOriginSpeedX1, ui->editFindOriginSpeedY1, ui->editFindOriginSpeedZ1, ui->editFindOriginSpeedC,
        ui->editFindOriginSpeedY2, ui->editFindOriginSpeedZ2,
        ui->editLeaveOriginSpeedX1, ui->editLeaveOriginSpeedY1, ui->editLeaveOriginSpeedZ1, ui->editLeaveOriginSpeedC,
        ui->editLeaveOriginSpeedY2, ui->editLeaveOriginSpeedZ2
    };
    for (const auto& edit : editsInteger)
    {
        edit->setDecimalPlaces(0);
    }

    // 机器参数 通信： 存放组号-参数索引-参数子索引-参数数值
    ui->editStorageGroup->setDecimalPlaces(0);
    ui->editStorageGroup->setInputRange(0, 100);
    ui->editParaIndex->setDecimalPlaces(0);
    ui->editParaSunIndex->setDecimalPlaces(0);
    ui->editParaValue->setDecimalPlaces(0);

    // set time input range
    ui->editYear->setInputRange(1970, 2038);
    ui->editMonth->setInputRange(1, 12);
    ui->editDay->setInputRange(1, 31);
    ui->editHour->setInputRange(0, 23);
    ui->editMinute->setInputRange(0, 59);
    ui->editSecond->setInputRange(0, 59);

    // font
    ui->coboxFonts->setCurrentIndex(m_SystemSet.typeFace);
    ui->coboxFontSize->setCurrentIndex(m_SystemSet.wordSize);

    // buzzer
    Beeper::instance()->setEnable(m_SystemSet.keyListen);
    ui->chboxBuzzer->setChecked(m_SystemSet.keyListen);

    // fresh backlight time dispaly
    BackLighter::instance()->setScreenOffTime(m_SystemSet.backlightTime);
    int duration = m_SystemSet.backlightTime;
    ui->editBrightTime->setText(QString::number(duration));

    // fresh backlight brightness dispaly
    BackLighter::instance()->setBrightness(m_SystemSet.backlightBrightness);
    int level = BackLighter::instance()->getBrightness();
    ui->sliderBrightness->setValue(level);
    ui->percentageBrightness->setValue(level);

    // system name
    ui->editSystemName->setText(m_SystemSet.sysName);

    // system color
    {
        std::vector<QCheckBox*> chboxColor = {
            ui->chboxColorDefault, ui->chboxColorOriange, ui->chboxColorYellow, ui->chboxColorGreen, ui->chboxBrown
        };
        auto index = m_SystemSet.sysColor;
        if (index < chboxColor.size()) {
            chboxColor[index]->setChecked(true);
        }
    }

    ui->labClawSafePic->setPixmap(QPixmap(":/images/settingPageImages/claw_safe.png"));
    ui->labOnlineSafePic->setPixmap(QPixmap(":/images/settingPageImages/online_safe.png"));

    //预留关联，预留出类型界面部分控件隐藏
    ui->labOutPortRelevancy6->hide();
    ui->labOutPortRelevancy7->hide();
    ui->labOutPortRelevancy13->hide();
    ui->coboxOutPortRelevancy6->hide();
    ui->coboxOutPortRelevancy7->hide();
    ui->coboxOutPortRelevancy13->hide();

    ui->labOutPortReleteOut6->hide();
    ui->labOutPortReleteOut7->hide();
    ui->labOutPortReleteOut13->hide();
    ui->coboxOutPortRelateOut6->hide();
    ui->coboxOutPortRelateOut7->hide();
    ui->coboxOutPortRelateOut13->hide();
    //安全区部分控件隐藏
    ui->labProcessFinishNum->hide();
    ui->coboxProcessFinishNum1->hide();
    ui->labProcessFinishNum_2->hide();
    ui->coboxProcessFinishNum1_2->hide();
    ui->labProcessFinishNum_3->hide();
    ui->coboxProcessFinishNum1_3->hide();
    ui->labProcessFinishNum_4->hide();
    ui->coboxProcessFinishNum1_4->hide();

    servoPointSafeArea[0] = SafeAreaWidget{
            ui->coboxSafeArea1, ui->coboxProcessFinishNum1,
    {ui->coboxAxisXSA1, ui->coboxAxisYSA1, ui->coboxAxisZSA1},
    {ui->editMachineA1AxisXSA1, ui->editMachineA1AxisYSA1, ui->editMachineA1AxisCSA1},
    {ui->editMachineA2AxisXSA1, ui->editMachineA2AxisYSA1, ui->editMachineA2AxisCSA1},
    {ui->editStockBinB1AxisXSA1, ui->editStockBinB1AxisYSA1},
    {ui->editStockBinB2AxisXSA1, ui->editStockBinB2AxisYSA1},
            ui->editMaxWaitPosSA1, ui->editMachineZMaxFallPosSA1,ui->editMachinInsideHeightSA1,
            ui->editMaxStartPosSA1,ui->editStockBinZMaxFallPosSA1
    };
    servoPointSafeArea[1] = SafeAreaWidget{
            ui->coboxSafeArea1_2, ui->coboxProcessFinishNum1_2,
    {ui->coboxAxisXSA1_2, ui->coboxAxisYSA1_2, ui->coboxAxisZSA1_2},
    {ui->editMachineA1AxisXSA1_2, ui->editMachineA1AxisYSA1_2, ui->editMachineA1AxisCSA1_2},
    {ui->editMachineA2AxisXSA1_2, ui->editMachineA2AxisYSA1_2,ui->editMachineA2AxisCSA1_2},
    {ui->editStockBinB1AxisXSA1_2, ui->editStockBinB1AxisYSA1_2},
    {ui->editStockBinB2AxisXSA1_2, ui->editStockBinB2AxisYSA1_2},
            ui->editMaxWaitPosSA1_2, ui->editMachineZMaxFallPosSA1_2,ui->editMachinInsideHeightSA1_2,
            ui->editMaxStartPosSA1_2,ui->editStockBinZMaxFallPosSA1_2
    };
    servoPointSafeArea[2] = SafeAreaWidget{
            ui->coboxSafeArea1_3, ui->coboxProcessFinishNum1_3,
    {ui->coboxAxisXSA1_3, ui->coboxAxisYSA1_3, ui->coboxAxisZSA1_3},
    {ui->editMachineA1AxisXSA1_3, ui->editMachineA1AxisYSA1_3, ui->editMachineA1AxisCSA1_3},
    {ui->editMachineA2AxisXSA1_3, ui->editMachineA2AxisYSA1_3, ui->editMachineA2AxisCSA1_3},
    {ui->editStockBinB1AxisXSA1_3, ui->editStockBinB1AxisYSA1_3},
    {ui->editStockBinB2AxisXSA1_3, ui->editStockBinB2AxisYSA1_3},
            ui->editMaxWaitPosSA1_3, ui->editMachineZMaxFallPosSA1_3, ui->editMachinInsideHeightSA1_3,
            ui->editMaxStartPosSA1_3, ui->editStockBinZMaxFallPosSA1_3
    };

    servoPointSafeArea[3] = SafeAreaWidget{
            ui->coboxSafeArea1_4, ui->coboxProcessFinishNum1_4,
            {ui->coboxAxisXSA1_4, ui->coboxAxisYSA1_4, ui->coboxAxisZSA1_4},
            {ui->editMachineA1AxisXSA1_4, ui->editMachineA1AxisYSA1_4,ui->editMachineA1AxisCSA1_4},
    {ui->editMachineA2AxisXSA1_4, ui->editMachineA2AxisYSA1_4,ui->editMachineA2AxisCSA1_4},
    {ui->editStockBinB1AxisXSA1_4, ui->editStockBinB1AxisYSA1_4},
    {ui->editStockBinB2AxisXSA1_4, ui->editStockBinB2AxisYSA1_4},
            ui->editMaxWaitPosSA1_4, ui->editMachineZMaxFallPosSA1_4, ui->editMachinInsideHeightSA1_4,
            ui->editMaxStartPosSA1_4, ui->editStockBinZMaxFallPosSA1_4
    };
    /*******************************************************************************/
    clawSafeWidgets[0] = ClawSafeParaWidget{
            ui->coboxClaw1KeepoutStatus, ui->coboxAxisXClawSafe, ui->coboxAxisZClawSafe,
            ui->editClawKeepoutX1Min, ui->editClawKeepoutX1Max, ui->editClawKeepoutZ1Height,
            ui->editAxisCKeepoutX1Min, ui->editAxisCKeepoutX1Max, ui->editAxisCKeepoutZHeight,
            ui->editAxisCKeepoutForwardMin, ui->editAxisCKeepoutForwardMax,
            ui->editAxisCKeepoutReverseMin, ui->editAxisCKeepoutReverseMax
    };
    clawSafeWidgets[1] = ClawSafeParaWidget{
            ui->coboxClaw1KeepoutStatus_2, ui->coboxAxisXClawSafe_2, ui->coboxAxisZClawSafe_2,
            ui->editClawKeepoutX1Min_2, ui->editClawKeepoutX1Max_2, ui->editClawKeepoutZ1Height_2,
            ui->editAxisCKeepoutX1Min_2, ui->editAxisCKeepoutX1Max_2, ui->editAxisCKeepoutZHeight_2,
            ui->editAxisCKeepoutForwardMin_2, ui->editAxisCKeepoutForwardMax_2,
            ui->editAxisCKeepoutReverseMin_2, ui->editAxisCKeepoutReverseMax_2
    };
    clawSafeWidgets[2] = ClawSafeParaWidget{
            ui->coboxClaw1KeepoutStatus_3, ui->coboxAxisXClawSafe_3, ui->coboxAxisZClawSafe_3,
            ui->editClawKeepoutX1Min_3, ui->editClawKeepoutX1Max_3, ui->editClawKeepoutZ1Height_3,
            ui->editAxisCKeepoutX1Min_3, ui->editAxisCKeepoutX1Max_3, ui->editAxisCKeepoutZHeight_3,
            ui->editAxisCKeepoutForwardMin_3, ui->editAxisCKeepoutForwardMax_3,
            ui->editAxisCKeepoutReverseMin_3, ui->editAxisCKeepoutReverseMax_3
    };
    clawSafeWidgets[3] = ClawSafeParaWidget{
            ui->coboxClaw1KeepoutStatus_4, ui->coboxAxisXClawSafe_4, ui->coboxAxisZClawSafe_4,
            ui->editClawKeepoutX1Min_4, ui->editClawKeepoutX1Max_4, ui->editClawKeepoutZ1Height_4,
            ui->editAxisCKeepoutX1Min_4, ui->editAxisCKeepoutX1Max_4, ui->editAxisCKeepoutZHeight_4,
            ui->editAxisCKeepoutForwardMin_4, ui->editAxisCKeepoutForwardMax_4,
            ui->editAxisCKeepoutReverseMin_4, ui->editAxisCKeepoutReverseMax_4
    };
    /*******************************************************************************/
    onlineSafeWidgets[0]=OnlineSafeParaWidget{
            ui->coboxMasterSlaveMode, ui->editPulseTime, ui->coboxOnlineSelect, ui->coboxOnlineAxis,
            ui->editAreaInput, ui->editAreaOutput, ui->editRequestInput, ui->editRequestOutput,
            ui->editA1A2MinPos, ui->editA1A2MaxPos};
    onlineSafeWidgets[1]=OnlineSafeParaWidget{
            ui->coboxMasterSlaveMode_2, ui->editPulseTime_2, ui->coboxOnlineSelect_2, ui->coboxOnlineAxis_2,
            ui->editAreaInput_2, ui->editAreaOutput_2, ui->editRequestInput_2, ui->editRequestOutput_2,
            ui->editA1A2MinPos_2, ui->editA1A2MaxPos_2};
    onlineSafeWidgets[2]=OnlineSafeParaWidget{
            ui->coboxMasterSlaveMode_3, ui->editPulseTime_3, ui->coboxOnlineSelect_3, ui->coboxOnlineAxis_3,
            ui->editAreaInput_3, ui->editAreaOutput_3, ui->editRequestInput_3, ui->editRequestOutput_3,
            ui->editA1A2MinPos_3, ui->editA1A2MaxPos_3};
    onlineSafeWidgets[3]=OnlineSafeParaWidget{
            ui->coboxMasterSlaveMode_4, ui->editPulseTime_4, ui->coboxOnlineSelect_4, ui->coboxOnlineAxis_4,
            ui->editAreaInput_4, ui->editAreaOutput_4, ui->editRequestInput_4, ui->editRequestOutput_4,
            ui->editA1A2MinPos_4, ui->editA1A2MaxPos_4};

    machineParaWidgets[0]=MachineParaWidget{ui->editAxisMinPosX1, ui->editAxisMaxPosX1, ui->editCirclePulseNumX1, ui->editCircleDistanceX1, ui->coboxCoordDirectX1,
            ui->editAccTimeX1, ui->editDecTimeX1, ui->editAccAccX1, ui->editDecDecX1, ui->editMaxSpeedX1,
            ui->coboxLimitPosX1, ui->coboxLimitNegX1,
            ui->coboxAxisTypeX1, ui->coboxAxisMoveModeX1,
            ui->coboxOriginTypeX1, ui->editFindOriginSpeedX1, ui->editLeaveOriginSpeedX1, ui->editOriginOffsetX1,
            ui->coboxBackOriginDirectX1, ui->coboxBackOriginOrderX1,
            ui->editMinLimitSignalX1,ui->editMaxLimitSignalX1,ui->editOriginSignalX1};
    machineParaWidgets[1]=MachineParaWidget{ui->editAxisMinPosY1, ui->editAxisMaxPosY1, ui->editCirclePulseNumY1, ui->editCircleDistanceY1, ui->coboxCoordDirectY1,
            ui->editAccTimeY1, ui->editDecTimeY1, ui->editAccAccY1, ui->editDecDecY1, ui->editMaxSpeedY1,
            ui->coboxLimitPosY1, ui->coboxLimitNegY1,
            ui->coboxAxisTypeY1, ui->coboxAxisMoveModeY1,
            ui->coboxOriginTypeY1, ui->editFindOriginSpeedY1, ui->editLeaveOriginSpeedY1, ui->editOriginOffsetY1,
            ui->coboxBackOriginDirectY1, ui->coboxBackOriginOrderY1,
            ui->editMinLimitSignalY1,ui->editMaxLimitSignalY1,ui->editOriginSignalY1};
    machineParaWidgets[2]=MachineParaWidget{ui->editAxisMinPosZ1, ui->editAxisMaxPosZ1, ui->editCirclePulseNumZ1, ui->editCircleDistanceZ1, ui->coboxCoordDirectZ1,
            ui->editAccTimeZ1, ui->editDecTimeZ1, ui->editAccAccZ1, ui->editDecDecZ1, ui->editMaxSpeedZ1,
            ui->coboxLimitPosZ1, ui->coboxLimitNegZ1,
            ui->coboxAxisTypeZ1, ui->coboxAxisMoveModeZ1,
            ui->coboxOriginTypeZ1, ui->editFindOriginSpeedZ1, ui->editLeaveOriginSpeedZ1, ui->editOriginOffsetZ1,
            ui->coboxBackOriginDirectZ1, ui->coboxBackOriginOrderZ1,
            ui->editMinLimitSignalZ1,ui->editMaxLimitSignalZ1,ui->editOriginSignalZ1};
    machineParaWidgets[3]=MachineParaWidget{ui->editAxisMinPosC, ui->editAxisMaxPosC, ui->editCirclePulseNumC, ui->editCircleDistanceC, ui->coboxCoordDirectC,
            ui->editAccTimeC, ui->editDecTimeC, ui->editAccAccC, ui->editDecDecC, ui->editMaxSpeedC,
            ui->coboxLimitPosC, ui->coboxLimitNegC,
            ui->coboxAxisTypeC, ui->coboxAxisMoveModeC,
            ui->coboxOriginTypeC, ui->editFindOriginSpeedC, ui->editLeaveOriginSpeedC, ui->editOriginOffsetC,
            ui->coboxBackOriginDirectC, ui->coboxBackOriginOrderC,
            ui->editMinLimitSignalC,ui->editMaxLimitSignalC,ui->editOriginSignalC};

    machineParaWidgets[4]=MachineParaWidget{ui->editAxisMinPosY2, ui->editAxisMaxPosY2, ui->editCirclePulseNumY2, ui->editCircleDistanceY2, ui->coboxCoordDirectY2,
            ui->editAccTimeY2, ui->editDecTimeY2, ui->editAccAccY2, ui->editDecDecY2, ui->editMaxSpeedY2,
            ui->coboxLimitPosY2, ui->coboxLimitNegY2,
            ui->coboxAxisTypeY2, ui->coboxAxisMoveModeY2,
            ui->coboxOriginTypeY2, ui->editFindOriginSpeedY2, ui->editLeaveOriginSpeedY2, ui->editOriginOffsetY2,
            ui->coboxBackOriginDirectY2, ui->coboxBackOriginOrderY2,
            ui->editMinLimitSignalY2,ui->editMaxLimitSignalY2,ui->editOriginSignalY2};

    machineParaWidgets[5]=MachineParaWidget{ui->editAxisMinPosZ2, ui->editAxisMaxPosZ2, ui->editCirclePulseNumZ2, ui->editCircleDistanceZ2, ui->coboxCoordDirectZ2,
            ui->editAccTimeZ2, ui->editDecTimeZ2, ui->editAccAccZ2, ui->editDecDecZ2, ui->editMaxSpeedZ2,
            ui->coboxLimitPosZ2, ui->coboxLimitNegZ2,
            ui->coboxAxisTypeZ2, ui->coboxAxisMoveModeZ2,
            ui->coboxOriginTypeZ2, ui->editFindOriginSpeedZ2, ui->editLeaveOriginSpeedZ2, ui->editOriginOffsetZ2,
            ui->coboxBackOriginDirectZ2, ui->coboxBackOriginOrderZ2,
            ui->editMinLimitSignalZ2,ui->editMaxLimitSignalZ2,ui->editOriginSignalZ2};
    HandwheelParaWidgets=HandwheelParaWidget{
            ui->editAccTimeHandwheel, ui->editDecTimeHandwheel,
            ui->editAccAccHandwheel, ui->editDecDecHandwheel};
#if 1
    //输出类型
    outputTypeList={ui->coboxOutPortType0,ui->coboxOutPortType1,ui->coboxOutPortType2,ui->coboxOutPortType3
                   ,ui->coboxOutPortType4,ui->coboxOutPortType5,ui->coboxOutPortType6,ui->coboxOutPortType7
                   ,ui->coboxOutPortType8,ui->coboxOutPortType9,ui->coboxOutPortType10,ui->coboxOutPortType11
                   ,ui->coboxOutPortType12,ui->coboxOutPortType13};

    //互锁设置
    outportInterlockList={ui->chbox0InterlockGroup0,ui->chbox2InterlockGroup0,ui->chbox1InterlockGroup0,ui->chbox3InterlockGroup0,
                         ui->chbox0InterlockGroup1,ui->chbox2InterlockGroup1,ui->chbox1InterlockGroup1,ui->chbox3InterlockGroup1,
                         ui->chbox0InterlockGroup2,ui->chbox2InterlockGroup2,ui->chbox1InterlockGroup2,ui->chbox3InterlockGroup2,
                         ui->chbox0InterlockGroup3,ui->chbox2InterlockGroup3,ui->chbox1InterlockGroup3,ui->chbox3InterlockGroup3,
                         ui->chbox0InterlockGroup4,ui->chbox2InterlockGroup4,ui->chbox1InterlockGroup4,ui->chbox3InterlockGroup4,
                         ui->chbox0InterlockGroup5,ui->chbox2InterlockGroup5,ui->chbox1InterlockGroup5,ui->chbox3InterlockGroup5,
                         ui->chbox0InterlockGroup6,ui->chbox2InterlockGroup6,ui->chbox1InterlockGroup6,ui->chbox3InterlockGroup6,
                         ui->chbox0InterlockGroup7,ui->chbox2InterlockGroup7,ui->chbox1InterlockGroup7,ui->chbox3InterlockGroup7,
                         ui->chbox0InterlockGroup8,ui->chbox2InterlockGroup8,ui->chbox1InterlockGroup8,ui->chbox3InterlockGroup8,
                         ui->chbox0InterlockGroup9,ui->chbox2InterlockGroup9,ui->chbox1InterlockGroup9,ui->chbox3InterlockGroup9,
                         ui->chbox0InterlockGroup10,ui->chbox2InterlockGroup10,ui->chbox1InterlockGroup10,ui->chbox3InterlockGroup10,
                         ui->chbox0InterlockGroup11,ui->chbox2InterlockGroup11,ui->chbox1InterlockGroup11,ui->chbox3InterlockGroup11};

    //预留关联
    outportRelevancyList={ui->coboxOutPortRelevancy0,ui->coboxOutPortRelevancy1,ui->coboxOutPortRelevancy2,
                         ui->coboxOutPortRelevancy3,ui->coboxOutPortRelevancy4,ui->coboxOutPortRelevancy5,
                         ui->coboxOutPortRelevancy6,ui->coboxOutPortRelevancy7,ui->coboxOutPortRelevancy8,
                         ui->coboxOutPortRelevancy9,ui->coboxOutPortRelevancy10,ui->coboxOutPortRelevancy11,
                         ui->coboxOutPortRelevancy12,ui->coboxOutPortRelevancy13};
    //预留关联出
    outportReleteOutList={ui->coboxOutPortRelateOut0,ui->coboxOutPortRelateOut1,ui->coboxOutPortRelateOut2,
                         ui->coboxOutPortRelateOut3,ui->coboxOutPortRelateOut4,ui->coboxOutPortRelateOut5,
                         ui->coboxOutPortRelateOut6,ui->coboxOutPortRelateOut7,ui->coboxOutPortRelateOut8,
                         ui->coboxOutPortRelateOut9,ui->coboxOutPortRelateOut10,ui->coboxOutPortRelateOut11,
                         ui->coboxOutPortRelateOut12,ui->coboxOutPortRelateOut13};

    //高级
    seniorFuncList={ui->coboxKnifeOrigin1Check,ui->coboxKnifeOrigin2Check,
                    ui->coboxlabChuckOriginCheck,
                    ui->coboxStockSaveIn1Check,ui->coboxStockSaveIn2Check,ui->coboxStockSaveInOutCheck,
                    ui->coboxAlarmIn1Check,ui->coboxAlarmIn2Check,
                    ui->coboxlabEmergencyStopCheck,
                    ui->coboxPauseStopCheck,ui->coboxPressureCheck,ui->coboxRemoteAuto,
                   ui->coboxRemoteStop,ui->coboxBottomOilLimit,ui->coboxOilerSwt,
                   ui->coboxLubricationPump,ui->coboxProcessSafe1,ui->coboxProcessSafe2,
                   ui->coboxEmergencySnapMotorEnable,ui->coboxEmergenceStopOutput,ui->coboxAutoLigth,
                   ui->coboxAlarmLight,ui->coboxAlarmBuzzer,ui->coboxPauseLight,
                   ui->coboxProcessFinish1,ui->coboxProcessFinish2,ui->coboxLocateFinish1,
                   ui->coboxLocateFinish2,ui->coboxStartProcess1,ui->coboxStartProcess2,
                   ui->coboxReserveAdvance1,ui->coboxReserveAdvance2,
                   ui->coboxMainAxisRotate1,ui->coboxMainAxisRotate2,
                   ui->coboxMainAxisLocate1,ui->coboxMainAxisLocate2,
                   ui->coboxBlow1,ui->coboxBlow2,
                   ui->coboxReserveAdvance3,ui->coboxReserveAdvance4,
                   ui->coboxManualChuckIn1,ui->coboxManualChuckIn2,
                   ui->coboxAutoDoorCtl1,ui->coboxAutoDoorCtl2,
                   ui->coboxReserveAdvance5,ui->coboxReserveAdvance6,ui->coboxReserveAdvance7,ui->coboxReserveAdvance8};

    //按键
    keyEdits = {
        ui->editKeyX1Sub, ui->editKeyX1Add, ui->editKeyY1Sub, ui->editKeyY1Add,
        ui->editKeyZ1Sub, ui->editKeyZ1Add, ui->editKeyCSub, ui->editKeyCAdd,
        ui->editKeyAorY2Sub, ui->editKeyAorY2ADD, ui->editKeyBorZ2Sub, ui->editKeyBorZ2Add,
        ui->editKeyF1, ui->editKeyF2, ui->editKeyF3, ui->editKeyF4
    };

    //LED
    sigEdits = {
        ui->editLED0, ui->editLED1, ui->editLED2, ui->editLED3, ui->editLED4
    };

    //安全设置
    //机床安全
    machineSafeList={ui->coboxArmDownAutoGate1Switch,ui->coboxArmDownAutoGate1SwitchStatus,
                    ui->coboxArmDownAutoGate2Switch,ui->coboxArmDownAutoGate2SwitchStatus,
                    ui->coboxFirstProductStartRun,ui->coboxFirstProductFinish};

    //料仓安全
    stackBinSafeList={ui->coboxStockIn1Recovery,ui->coboxStockIn2Recovery};

    //产品设置
    //产品设置
    productSet = {
        ui->editWaitProcessFinishTime, ui->editPulseOutputTime,
        ui->editValveLimitTime, ui->editAutoCycleTime,
        ui->editAlarmBuzzerTime, ui->editChainReachDelay,
        ui->editClampCheckFilterTime
    };

    //高级
    productSeniorEdit = {
        ui->editLubricationOpenTime, ui->editLubricationCloseTime,
        ui->editLubricationDelay, ui->editLubricationIntervalCycle
    };

    //物联网
    productIot = {
        ui->coboxMainboardComunication, ui->coboxParityCheck,ui->coboxBaudRate
    };
    //伺服参数
    //结构
    servoPara = {
        ui->coboxEncoderType, ui->coboxEncoderBitNum, ui->coboxServoBrand, ui->coboxServoModel,
        ui->coboxExtendIOBoard, ui->coboxRotateAxisUnit, ui->coboxResetSubProgram, ui->coboxTorqueLimitFlag};
    //加速时间
    accTimeList ={
        ui->editAccTimeX1,ui->editAccTimeY1,ui->editAccTimeZ1,
        ui->editAccTimeC,ui->editAccTimeY2,ui->editAccTimeZ2,ui->editAccTimeHandwheel
    };
    //减速时间
    decTimeList ={
        ui->editDecTimeX1,ui->editDecTimeY1,ui->editDecTimeZ1,
        ui->editDecTimeC,ui->editDecTimeY2,ui->editDecTimeZ2,ui->editDecTimeHandwheel
    };
    //加加速
    accAccList ={
        ui->editAccAccX1,ui->editAccAccY1,ui->editAccAccZ1,
        ui->editAccAccC,ui->editAccAccY2,ui->editAccAccZ2,ui->editAccAccHandwheel
    };
    //减减速
    decDecList ={
        ui->editDecDecX1,ui->editDecDecY1,ui->editDecDecZ1,
        ui->editDecDecC,ui->editDecDecY2,ui->editDecDecZ2,ui->editDecDecHandwheel
    };

    //伺服安全点
    //容差
    toleranceList={ui->editTolerance,ui->editOriginDegree,ui->editAcitonConfirmError};

    //横行位置限定
    horizontalPosList={ui->editSafeRampageHeightZ1,ui->editSafeRampageHeightZ2,
                       ui->editSafeRotatePosY1,ui->editSafeRotatePosY2};

    //堆叠参数
    stackCoboxs = {
        ui->coboxStackWay, ui->coboxGroupStack, ui->coboxRowStack, ui->coboxLayerStack,
        ui->coboxDividedStack, ui->coboxStackMemory, ui->coboxSiloType, ui->coboxRotateSiloFinHint
    };
#endif
}

void Setting::initWidgets()
{
    for (auto stkWgt : this->findChildren<QStackedWidget*>())
    {
        stkWgt->setCurrentIndex(0);
    }

    for (auto tabWgt : this->findChildren<QTabWidget*>())
    {
        tabWgt->setCurrentIndex(0);
    }

    ui->updata_widget->hide();

    ui->tableWgtPortDef->horizontalHeader()->setVisible(true);
    ui->tableWgtPortDef->verticalHeader()->setVisible(true);
    ui->tableWgtPortDef->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWgtPortDef->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWgtPortDef->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWgtPortDef->setColumnWidth(0, 80);
    ui->tableWgtPortDef->setColumnWidth(1, 200);
    ui->tableWgtPortDef->setColumnWidth(2, 200);

    ui->tableWgtNameDef->horizontalHeader()->setVisible(true);
    ui->tableWgtNameDef->verticalHeader()->setVisible(true);
    ui->tableWgtNameDef->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWgtNameDef->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWgtNameDef->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->tableWgtNote->setColumnCount(2);
    ui->tableWgtNote->setHorizontalHeaderLabels({ tr("标题") , tr("修改时间")});
    ui->tableWgtNote->verticalHeader()->setVisible(true);
     ui->tableWgtNote->horizontalHeader()->setVisible(true);

    ui->tableWgtNote->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWgtNote->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWgtNote->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->tableWgtNote->setColumnWidth(0, 180);
    ui->tableWgtNote->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed); // 第一列宽度固定
    ui->tableWgtNote->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch); // 第二列可伸缩
    ui->tableWgtNote->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    QDir dirNote(noteDirPath);
    QFileInfoList noteInfoList = dirNote.entryInfoList({"*.txt"}, QDir::Files | QDir::NoDotAndDotDot);

    for (const auto noteInfo : noteInfoList)
    {
        QString title = noteInfo.fileName().split(".").first();
        QString noteTime = noteInfo.lastModified().toString("yyyy-MM-dd hh:mm:ss");

        int rowCount = ui->tableWgtNote->rowCount();
        ui->tableWgtNote->insertRow(rowCount);
        ui->tableWgtNote->setItem(rowCount, 0, new QTableWidgetItem(title));
        ui->tableWgtNote->setItem(rowCount, 1, new QTableWidgetItem(noteTime));

//        ui->editNoteTitle->setText(title);
//        ui->plainTextEditNote->clear();
    }

}

void Setting::setupPortDefine()
{
    auto updatePortDefTableRow = [&](int row, const D_PortDefineStruct& portInfo)
    {
        QTableWidgetItem* item1 = ui->tableWgtPortDef->item(row, 0);
        if (!item1) {
            item1 = new QTableWidgetItem();
            ui->tableWgtPortDef->setItem(row, 0, item1);
        }
        item1->setText(portInfo.definePort);

        QTableWidgetItem* item2 = ui->tableWgtPortDef->item(row, 1);
        QTableWidgetItem* item3 = ui->tableWgtPortDef->item(row, 2);
        if (!item2) {
            item2 = new QTableWidgetItem();
            ui->tableWgtPortDef->setItem(row, 1, item2);
        }
        if (!item3) {
            item3 = new QTableWidgetItem();
            ui->tableWgtPortDef->setItem(row, 2, item3);
        }

        if (portInfo.functionSet == 1) {
            item2->setText(portInfo.defineName);
            item3->setText(portInfo.modifyName);
        } else {
            item2->setText(portInfo.ResDefineName);
            item3->setText(portInfo.ResModifyName);
        }

        QTableWidgetItem* item4 = ui->tableWgtPortDef->item(row, 3);
        if (!item4) {
            item4 = new QTableWidgetItem();
            ui->tableWgtPortDef->setItem(row, 3, item4);
        }
        item4->setText(portInfo.modifyPort);
        item4->setData(Qt::UserRole, portInfo.actualPortNum); // 设置附加数据
    };

    // 遍历输入端口
    for (int i = 0; i < INPUT_TOTAL_NUM; i++) {
        updatePortDefTableRow(i, m_Port_X[i]);
    }

    // 遍历输出端口
    for (int i = 0; i < OUTPUT_TOTAL_NUM; i++) {
        updatePortDefTableRow(INPUT_TOTAL_NUM + i, m_Port_Y[i]);
    }

    connect(ui->btnSavePort,&QPushButton::clicked, [=](){
        if (!ui->btnSavePort->isParaChanged()) return;
        this->savePortDefine();
    });
    connect(ui->tableWgtPortDef,&QTableWidget::cellClicked,[=](int row, int column){
        auto res = modifyPort(row, column);

        ui->btnSavePort->setParaChangedFlag(res);
        M_SaveSetPar.SavePort = res;
    });

    connect(ui->btnRestoreNameOnePortDef, &QPushButton::clicked, [=](){
        int curRow = ui->tableWgtPortDef->currentRow();
        if (curRow < 0) return;

        auto defaultNameItem = ui->tableWgtPortDef->item(curRow, 1);
        auto revisedNameItem = ui->tableWgtPortDef->item(curRow, 2);

        if (defaultNameItem && revisedNameItem)
        {
            revisedNameItem->setText(defaultNameItem->text());
            ui->btnSavePort->setParaChangedFlag(true);
            M_SaveSetPar.SavePort = true;
        }
    });
    connect(ui->btnRestoreNameAllPortDef, &QPushButton::clicked, [=](){
        for (int row = 0; row < ui->tableWgtPortDef->rowCount(); row++)
        {
            auto defaultNameItem = ui->tableWgtPortDef->item(row, 1);
            auto revisedNameItem = ui->tableWgtPortDef->item(row, 2);

            if (defaultNameItem && revisedNameItem)
            {
                revisedNameItem->setText(defaultNameItem->text());
            }
        }
        ui->btnSavePort->setParaChangedFlag(true);
        M_SaveSetPar.SavePort = true;
    });
    connect(ui->btnRestorePortOnePortDef, &QPushButton::clicked, [=](){

        int curRow = ui->tableWgtPortDef->currentRow();
        if (curRow < 0) return;

//        auto defaultPortItem = ui->tableWgtPortDef->item(curRow, 0);
        auto revisedPortItem = ui->tableWgtPortDef->item(curRow, 3);

        if (revisedPortItem)
        {
            revisedPortItem->setText("0");

            if(curRow < INPUT_TOTAL_NUM)
            {
                revisedPortItem->setData(Qt::UserRole, m_Port_X[curRow].portNum);
            }
            else
            {
                revisedPortItem->setData(Qt::UserRole, m_Port_Y[curRow - INPUT_TOTAL_NUM].portNum);
            }
            ui->btnSavePort->setParaChangedFlag(true);
            M_SaveSetPar.SavePort = true;
        }
    });
    connect(ui->btnRestorePortAllPortDef, &QPushButton::clicked, [=](){

        for (int i = 0; i < ui->tableWgtPortDef->rowCount(); i++)
        {
            auto revisedPortItem = ui->tableWgtPortDef->item(i, 3);

            if (revisedPortItem)
            {
                revisedPortItem->setText("0");

                if(i < INPUT_TOTAL_NUM)
                {
                    revisedPortItem->setData(Qt::UserRole, m_Port_X[i].portNum);
                }
                else
                {
                    revisedPortItem->setData(Qt::UserRole, m_Port_Y[i - INPUT_TOTAL_NUM].portNum);
                }
            }
        }
        ui->btnSavePort->setParaChangedFlag(true);
        M_SaveSetPar.SavePort = true;
    });

    connect(ui->btnImportPortDef, &QPushButton::clicked, this, [=](){
        if (!UsbDisk::instance()->isInserted())
        {
            MainWindow::pMainWindow->showErrorTip(tr("请插入U盘！"), TipMode::ONLY_OK);
            return;
        }

        QFileInfo fileInfo(CustomizePortInfoPath);
        auto res = UsbDisk::instance()->copyFromUsb("HMI/" + fileInfo.fileName(), fileInfo.path());
        ::sync();

        if (res)
        {
            ErrorTipDialog *tip = new ErrorTipDialog(tr("导入成功!"), TipMode::ONLY_OK, nullptr);
            QTimer::singleShot(1000, tip, [tip]() {
                if (tip->isVisible()) {
                    tip->accept();
                }
                tip->deleteLater();
            });
            tip->exec();
        }

        QProgressDialog loadingTip(tr("数据处理中，请稍候..."), tr("取消"), 0, 0, this);
        loadingTip.setFixedSize(500, 300);
        loadingTip.setWindowModality(Qt::ApplicationModal);
        loadingTip.setCancelButton(nullptr);
        QCoreApplication::processEvents(); // 处理事件循环，确保对话框显示

        QTimer::singleShot(10, [&]() {
            // 读取导入数据并处理
            readPortDefInfo();

            for (int i = 0; i < INPUT_TOTAL_NUM; i++) {
                updatePortDefTableRow(i, m_Port_X[i]);
            }

            for (int i = 0; i < OUTPUT_TOTAL_NUM; i++) {
                updatePortDefTableRow(INPUT_TOTAL_NUM + i, m_Port_Y[i]);
            }

            g_Usart->ExtendSendParDeal(CMD_MAIN_SIGNAL, CMD_SUN_SIGNAL_IN_FUNC_DEF);
            g_Usart->ExtendSendParDeal(CMD_MAIN_SIGNAL, CMD_SUN_SIGNAL_OUT_FUNC_DEF);
            emit monitor_port_refreash();
            emit WidgetNameRefresh_signal();
            emit updatemonitorhandcontrol();
            emit updateManualformButtonName_Signal();

            loadingTip.accept();
        });

        loadingTip.exec();
        g_SafeFileHandler->rotateBackups(CustomizePortInfoPath);
        REFRESH_KERNEL_BUFFER(CustomizePortInfoPath.toLocal8Bit().data());
    });
    connect(ui->btnExportPortDef, &QPushButton::clicked, [=](){
        if (!UsbDisk::instance()->isInserted())
        {
            MainWindow::pMainWindow->showErrorTip(tr("请插入U盘！"), TipMode::ONLY_OK);
            return;
        }
//        auto res = UsbDisk::instance()->copyToUsb(m_configPortXYNamePath, "HMI/");
//        res = res && UsbDisk::instance()->copyToUsb(m_configPortXYNameIniPath, "HMI/");

        auto res = UsbDisk::instance()->copyToUsb(CustomizePortInfoPath, "HMI/");

        ::sync();

        if (res)
        {
            ErrorTipDialog *tip = new ErrorTipDialog(tr("导出成功!"), TipMode::ONLY_OK, nullptr);
            QTimer::singleShot(1000, tip, [tip]() {
                if (tip->isVisible()) {
                    tip->accept();
                }
                tip->deleteLater();
            });
            tip->exec();
        }
    });

    connect(ui->coboxGroupSelectPortDef, QOverload<int>::of(&QComboBox::activated), [=](int index){
        auto findAndScrollToItem = [this](const QString& targetName) {
            for (int row = 0; row < ui->tableWgtPortDef->rowCount(); ++row) {
                QTableWidgetItem* item = ui->tableWgtPortDef->item(row, 0);
                if (item && item->text() == targetName) {
                    ui->tableWgtPortDef->setCurrentItem(item);
                    ui->tableWgtPortDef->scrollToItem(item, QAbstractItemView::PositionAtTop);
                    return true;
                }
            }
            return false;
        };
        const QString portGroupName[] = {
            "X1", "EX1", "Y1", "EY1"
        };

        findAndScrollToItem(portGroupName[index]);
    });
}

void Setting::setupNameDefine()
{
    // 计算总行数
    int rowCnt = 2 + 8 + STACK_TOTAL_NUM + AXIS_TOTAL_NUM + VAR_TOTAL_NUM + FOLLOW_STACK_NUM + TIME_TOTAL_NUM;


    auto tableNameDef = ui->tableWgtNameDef;
    tableNameDef->setRowCount(rowCnt);
    tableNameDef->setColumnCount(2);

    // 设置表头
    tableNameDef->setHorizontalHeaderLabels({tr("默认名称"), tr("修改名称")});

    int currentRow = 0;

    // 插入管理员名称
    tableNameDef->setItem(currentRow, 0, new QTableWidgetItem(m_NameDefine[0].adminName));
    tableNameDef->setItem(currentRow++, 1, new QTableWidgetItem(m_NameDefine[1].adminName));

    // 插入操作员名称
    tableNameDef->setItem(currentRow, 0, new QTableWidgetItem(m_NameDefine[0].operatorName));
    tableNameDef->setItem(currentRow++, 1, new QTableWidgetItem(m_NameDefine[1].operatorName));

    // 插入子程序名称
    for (int i = 0; i < 8; i++) {
        tableNameDef->setItem(currentRow, 0, new QTableWidgetItem(m_NameDefine[0].subProgName[i]));
        tableNameDef->setItem(currentRow++, 1, new QTableWidgetItem(m_NameDefine[1].subProgName[i]));
    }

    // 插入轴
    for (int i = 0; i < AXIS_TOTAL_NUM; i++) {
        tableNameDef->setItem(currentRow, 0, new QTableWidgetItem(m_NameDefine[0].axisName[i]));
        tableNameDef->setItem(currentRow++, 1, new QTableWidgetItem(m_NameDefine[1].axisName[i]));
    }

    // 插入变量名称
    for (int i = 0; i < VAR_TOTAL_NUM; i++) {
        tableNameDef->setItem(currentRow, 0, new QTableWidgetItem(m_NameDefine[0].varName[i]));
        tableNameDef->setItem(currentRow++, 1, new QTableWidgetItem(m_NameDefine[1].varName[i]));
    }

    // 插入堆叠名称
    for (int i = 0; i < STACK_TOTAL_NUM; i++) {
        tableNameDef->setItem(currentRow, 0, new QTableWidgetItem(m_NameDefine[0].stackName[i]));
        tableNameDef->setItem(currentRow++, 1, new QTableWidgetItem(m_NameDefine[1].stackName[i]));
    }

    // 插入跟随堆叠名称
    for (int i = 0; i < FOLLOW_STACK_NUM; i++) {
        tableNameDef->setItem(currentRow, 0, new QTableWidgetItem(m_NameDefine[0].followStackName[i]));
        tableNameDef->setItem(currentRow++, 1, new QTableWidgetItem(m_NameDefine[1].followStackName[i]));
    }

    // 插入定时器名称
    for (int i = 0; i < TIME_TOTAL_NUM; i++) {
        tableNameDef->setItem(currentRow, 0, new QTableWidgetItem(m_NameDefine[0].timerName[i]));
        tableNameDef->setItem(currentRow++, 1, new QTableWidgetItem(m_NameDefine[1].timerName[i]));
    }

    connect(ui->tableWgtNameDef, &QTableWidget::cellClicked, this, [=](int row, int col){
        if (col == 1)
        {
            auto item = ui->tableWgtNameDef->item(row, col);
            QString originText = item->text();
            FullKeyboard* keyboard = FullKeyboard::instance();
            keyboard->setText(originText);
            keyboard->setCurrentEditObj(nullptr);
            keyboard->exec();
            auto inputText = keyboard->getInputText();
            if (originText != inputText)
            {
                item->setText(inputText);
                ui->btnSaveNameDef->setParaChangedFlag(true);
                M_SaveSetPar.SaveNameDef = true;
            }
        }
    });

    connect(ui->btnSaveNameDef, &QPushButton::clicked, this, [=](){
        if (!ui->btnSaveNameDef->isParaChanged()) return;
        saveNameDefine();
    });
    connect(ui->btnExportNameDef, &QPushButton::clicked, this, [=](){
       if (!UsbDisk::instance()->isInserted())
       {
           MainWindow::pMainWindow->showErrorTip(tr("请插入U盘！"), TipMode::ONLY_OK);
           return;
       }
       auto res = UsbDisk::instance()->copyToUsb(CustomizeNameDefPath, "HMI/");
       ::sync();

       if (res)
       {
           ErrorTipDialog *tip = new ErrorTipDialog(tr("导出成功!"), TipMode::ONLY_OK, nullptr);
           QTimer::singleShot(1000, tip, [tip]() {
               if (tip->isVisible()) {
                   tip->accept();
               }
               tip->deleteLater();
           });
           tip->exec();

       }

    });
    connect(ui->btnImportNameDef, &QPushButton::clicked, this, [=](){
       if (!UsbDisk::instance()->isInserted())
       {
           MainWindow::pMainWindow->showErrorTip(tr("请插入U盘！"), TipMode::ONLY_OK);
           return;
       }
       QFileInfo fileInfo(CustomizeNameDefPath);
       auto res = UsbDisk::instance()->copyFromUsb("HMI/" + fileInfo.fileName(), fileInfo.path());
       ::sync();

       ::readNameDefine();
       this->refreshNameDefine();

       if (res)
       {
           ErrorTipDialog *tip = new ErrorTipDialog(tr("导入成功!"), TipMode::ONLY_OK, nullptr);
           QTimer::singleShot(1000, tip, [tip]() {
               if (tip->isVisible()) {
                   tip->accept();
               }
               tip->deleteLater();
           });
           tip->exec();
       }
       M_SaveSetPar.SaveNameDef = true;
       emit coboxVarSelectVarPreOpItemSet_signal();
       g_SafeFileHandler->rotateBackups(CustomizeNameDefPath);
       REFRESH_KERNEL_BUFFER(CustomizeNameDefPath.toLocal8Bit().data());
    });
    connect(ui->btnRestoreNameOneNameDef, &QPushButton::clicked, this, [=](){
        int curRow = ui->tableWgtNameDef->currentRow();
        if (curRow < 0) return;

        auto itemRevised = ui->tableWgtNameDef->item(curRow, 1);
        auto itemDefault = ui->tableWgtNameDef->item(curRow, 0);
        if (itemRevised && itemDefault)
        {
            itemRevised->setText(itemDefault->text());
            ui->btnSaveNameDef->setParaChangedFlag(true);
            M_SaveSetPar.SaveNameDef = true;
        }
    });

    connect(ui->btnRestoreNameAllNameDef, &QPushButton::clicked, this, [=](){

        int totalRows = 8 + AXIS_TOTAL_NUM + VAR_TOTAL_NUM + STACK_TOTAL_NUM + FOLLOW_STACK_NUM + TIME_TOTAL_NUM;
        if (tableNameDef->rowCount() < totalRows) {
            tableNameDef->setRowCount(totalRows);
        }
        bool flag = false;
        for (int i = 0; i < ui->tableWgtNameDef->rowCount(); i++)
        {
            auto defaultItem = ui->tableWgtNameDef->item(i, 0);
            auto revisedItem = ui->tableWgtNameDef->item(i, 1);
            auto defaultName = defaultItem->text();
            auto revisedName = defaultItem->text();
            if (revisedName != defaultName)
            {
                revisedItem->setText(defaultName);
                flag = true;
                M_SaveSetPar.SaveNameDef = true;
            }
        }
        ui->btnSaveNameDef->setParaChangedFlag(flag);
    });

    connect(ui->coboxGroupSelectNameDef, QOverload<int>::of(&QComboBox::activated), this, [=](int index){
        auto findAndScrollToItem = [this](const QString& targetName) {
            for (int row = 0; row < ui->tableWgtNameDef->rowCount(); ++row) {
                QTableWidgetItem* item = ui->tableWgtNameDef->item(row, 0);
                if (item && item->text() == targetName) {
                    ui->tableWgtNameDef->setCurrentItem(item);
                    ui->tableWgtNameDef->scrollToItem(item, QAbstractItemView::PositionAtTop);
                    return true;
                }
            }
            return false;
        };

        switch (index) {
        case 0: // 登陆权限
            findAndScrollToItem(m_NameDefine[0].adminName);
            break;
        case 1: // 子程序
            findAndScrollToItem(m_NameDefine[0].subProgName[0]);
            break;
        case 2: // 轴
            findAndScrollToItem(m_NameDefine[0].axisName[0]);
            break;
        case 3: // 变量
            findAndScrollToItem(m_NameDefine[0].varName[0]);
            break;
        case 4: // 堆叠
            findAndScrollToItem(m_NameDefine[0].stackName[0]);
            break;
        case 5: // 放料跟随
            findAndScrollToItem(m_NameDefine[0].followStackName[0]);
            break;
        case 6: // 定时器
            findAndScrollToItem(m_NameDefine[0].timerName[0]);
            break;
        default:
            break;
        }

    });
}

void Setting::refreshNameDefine()
{
    auto tableNameDef = ui->tableWgtNameDef;
    // fresh dispaly
    for (int col = 0; col < 2; col++)
    {
        int index = 0;
        tableNameDef->item(index++, col)->setText(m_NameDefine[col].adminName);

        tableNameDef->item(index++, col)->setText(m_NameDefine[col].operatorName);

        for (int i = 0; i < 8; ++i) {
            auto item = tableNameDef->item(index, col);
            item->setText(m_NameDefine[col].subProgName[i]);
            ++index;
        }

        for (int i = 0; i < AXIS_TOTAL_NUM; ++i) {
            auto item = tableNameDef->item(index, col);
            item->setText(m_NameDefine[col].axisName[i]);
            ++index;
        }

        for (int i = 0; i < VAR_TOTAL_NUM; ++i) {
            auto item = tableNameDef->item(index, col);
            item->setText(m_NameDefine[col].varName[i]);
            ++index;
        }

        for (int i = 0; i < STACK_TOTAL_NUM; ++i) {
            auto item = tableNameDef->item(index, col);
            item->setText(m_NameDefine[col].stackName[i]);
            ++index;
        }

        for (int i = 0; i < FOLLOW_STACK_NUM; ++i) {
            auto item = tableNameDef->item(index, col);
            item->setText(m_NameDefine[col].followStackName[i]);
            ++index;
        }

        for (int i = 0; i < TIME_TOTAL_NUM; ++i) {
            auto item = tableNameDef->item(index, col);
            item->setText(m_NameDefine[col].timerName[i]);
            ++index;
        }
    }
}

void Setting::setupMenuAuthority()
{
    ui->treeWgt->setColumnCount(5);
    ui->treeWgt->setColumnWidth(0, 200);
    ui->treeWgt->header()->setSectionsMovable(false);
    ui->treeWgt->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    for (int i = 1; i < 5; i++)
    {
        ui->treeWgt->setColumnWidth(i, 135);
        ui->treeWgt->header()->setSectionResizeMode(i, QHeaderView::Fixed);
    }

    ui->treeWgt->setStyleSheet("QTreeWidget::item { height: 45px; }"
                               "QTreeWidget::branch:open:has-children {"
                               "   image: url(:/images/open_menu.png);"
                               "    width: 40px;"
                               "    height: 40px;"
                               "}"
                               "QTreeWidget::branch:closed:has-children {"
                               "   image: url(:/images/close_menu.png);"
                               "    width: 40px;"
                               "    height: 40px;"
                               "}"
                               "QRadioButton::indicator:unchecked {"
                               "    image: url(:/images/radio_unchecked.png);"
                               "    width: 40px;"
                               "    height: 40px;"
                               "}"
                               "QRadioButton::indicator:checked {"
                               "    image: url(:/images/radio_checked.png);"
                               "    width: 40px;"
                               "    height: 40px;"
                               "}"
                               );
    ui->treeWgt->setHeaderLabels({tr("菜单"), tr("操作员"), tr("管理员"), tr("高级"), tr("不可见")});

#if 0
    QList<MenuItem*> menuItems;
    MenuItem* sigSet = new MenuItem(1, tr("信号设置"));
    MenuItem* safeSet = new MenuItem(2, tr("安全设置"));
    MenuItem* productSet = new MenuItem(3, tr("产品设置"));
    MenuItem* systemSet = new MenuItem(4, tr("系统设置"));
    MenuItem* servoPara = new MenuItem(5, tr("伺服参数"));
    MenuItem* servoSafe = new MenuItem(6, tr("伺服安全点"));
    MenuItem* machinePara = new MenuItem(7, tr("机器参数"));
    MenuItem* stactSet = new MenuItem(8, tr("堆叠设置"));
    menuItems.append(sigSet);
    menuItems.append(safeSet);
    menuItems.append(productSet);
    menuItems.append(systemSet);
    menuItems.append(servoPara);
    menuItems.append(servoSafe);
    menuItems.append(machinePara);
    menuItems.append(stactSet);

    // Add items for sigSet, safeSet, productSet
    sigSet->children = {
            new MenuItem(11, {tr("输出类型")}),
            new MenuItem(12, {tr("互锁设置")}),
            new MenuItem(13, {tr("端口自定义")}),
            new MenuItem(14, {tr("名称自定义")}),
            new MenuItem(15, {tr("预留关联")}),
            new MenuItem(16, {tr("预留出类型")}),
            new MenuItem(17, {tr("按键/信号")}),
            new MenuItem(18, {tr("高级")})
        };

    safeSet->children = {
        new MenuItem(21, {tr("机床安全")}),
        new MenuItem(22, {tr("料仓安全")}),
        new MenuItem(23, {tr("卡爪安全")}),
        new MenuItem(24, {tr("联机安全")})
    };
    productSet->children = {
        new MenuItem(31, {tr("产品")}),
        new MenuItem(32, {tr("高级")}),
        new MenuItem(33, {tr("物联网")}),
        new MenuItem(34, {tr("联机安全")})
    };

    systemSet->children = {
        new MenuItem(41, {tr("语言设置")}),
        new MenuItem(42, {tr("用户设置")}),
        new MenuItem(43, {tr("升级与备份")}),
        new MenuItem(44, {tr("记事本")}),
        new MenuItem(45, {tr("密码设置")}),
        new MenuItem(46, {tr("物联网")}),
        new MenuItem(47, {tr("注册信息")})

    };

    servoPara->children = {
        new MenuItem(51, {tr("伺服")}),
        new MenuItem(52, {tr("轴参数")}),
        new MenuItem(53, {tr("轴速度")})
    };
    servoSafe->children = {
        new MenuItem(61, {tr("安全区1")}),
        new MenuItem(62, {tr("安全区2")}),
        new MenuItem(63, {tr("安全区3")}),
        new MenuItem(64, {tr("安全区4")}),
        new MenuItem(65, {tr("位置限定")})
    };

    machinePara->children = {
        new MenuItem(71, {tr("限位")}),
        new MenuItem(72, {tr("结构")}),
        new MenuItem(73, {tr("原点")}),
        new MenuItem(74, {tr("通信")})
    };
    stactSet->children = {
        new MenuItem(81, {tr("堆叠1组")}),
        new MenuItem(82, {tr("堆叠2组")}),
        new MenuItem(83, {tr("堆叠3组")}),
        new MenuItem(84, {tr("堆叠4组")}),
        new MenuItem(85, {tr("堆叠5组")}),
        new MenuItem(86, {tr("堆叠6组")}),
        new MenuItem(87, {tr("堆叠7组")}),
        new MenuItem(88, {tr("堆叠8组")}),
        new MenuItem(89, {tr("堆叠设置")})
    };
#endif
    QList<MenuItem*> menuItems;
    MenuItem* sigSet = new MenuItem(1, "信号设置");
    MenuItem* safeSet = new MenuItem(2, "安全设置");
    MenuItem* productSet = new MenuItem(3, "产品设置");
    MenuItem* systemSet = new MenuItem(4, "系统设置");
    MenuItem* servoPara = new MenuItem(5, "伺服参数");
    MenuItem* servoSafe = new MenuItem(6, "伺服安全点");
    MenuItem* machinePara = new MenuItem(7, "机器参数");
    MenuItem* stactSet = new MenuItem(8, "堆叠设置");
    menuItems.append(sigSet);
    menuItems.append(safeSet);
    menuItems.append(productSet);
    menuItems.append(systemSet);
    menuItems.append(servoPara);
    menuItems.append(servoSafe);
    menuItems.append(machinePara);
    menuItems.append(stactSet);

    // Add items for sigSet, safeSet, productSet
    sigSet->children = {
        new MenuItem(11, "输出类型"),
        new MenuItem(12, "互锁设置"),
        new MenuItem(13, "端口自定义"),
        new MenuItem(14, "名称自定义"),
        new MenuItem(15, "预留关联"),
        new MenuItem(16, "预留出类型"),
        new MenuItem(17, "按键/信号"),
        new MenuItem(18, "高级")
    };

    safeSet->children = {
        new MenuItem(21, "机床安全"),
        new MenuItem(22, "料仓安全"),
        new MenuItem(23, "卡爪安全"),
        new MenuItem(24, "联机安全")
    };
    productSet->children = {
        new MenuItem(31, "产品"),
        new MenuItem(32, "高级"),
        new MenuItem(33, "物联网"),
        new MenuItem(34, "联机安全")
    };

    systemSet->children = {
        new MenuItem(41, "语言设置"),
        new MenuItem(42, "用户设置"),
        new MenuItem(43, "升级与备份"),
        new MenuItem(44, "记事本"),
        new MenuItem(45, "密码设置"),
        new MenuItem(46, "物联网"),
        new MenuItem(47, "注册信息")
    };

    servoPara->children = {
        new MenuItem(51, "伺服"),
        new MenuItem(52, "轴参数"),
        new MenuItem(53, "轴速度")
    };
    servoSafe->children = {
        new MenuItem(61, "安全区1"),
        new MenuItem(62, "安全区2"),
        new MenuItem(63, "安全区3"),
        new MenuItem(64, "安全区4"),
        new MenuItem(65, "位置限定")
    };

    machinePara->children = {
        new MenuItem(71, "限位"),
        new MenuItem(72, "结构"),
        new MenuItem(73, "原点"),
        new MenuItem(74, "通信")
    };
    stactSet->children = {
        new MenuItem(81, "堆叠1组"),
        new MenuItem(82, "堆叠2组"),
        new MenuItem(83, "堆叠3组"),
        new MenuItem(84, "堆叠4组"),
        new MenuItem(85, "堆叠5组"),
        new MenuItem(86, "堆叠6组"),
        new MenuItem(87, "堆叠7组"),
        new MenuItem(88, "堆叠8组"),
        new MenuItem(89, "堆叠设置")
    };
    // 使用 QMap 存储 MenuItem 和 QTreeWidgetItem 的映射关系
    QMap<MenuItem*, QTreeWidgetItem*> itemMap;

    // 将 MenuItem 转换为 QTreeWidgetItem 并添加到树形结构中
    QList<QTreeWidgetItem*> treeItems;
    for (auto menuItem : menuItems) {
        auto itemName = QCoreApplication::translate("Setting", menuItem->name.toStdString().c_str(), nullptr);
        tabNameMap[menuItem->id] = itemName;
        tabSourceNameMap[menuItem->id] = menuItem->name;
        QTreeWidgetItem* treeItem = new QTreeWidgetItem(ui->treeWgt, {itemName});
        itemMap[menuItem] = treeItem;
        treeItems.append(treeItem);
        for (auto& child : menuItem->children) {
            auto childName = QCoreApplication::translate("Setting", child->name.toStdString().c_str(), nullptr);
            tabNameMap[child->id] = childName;
            tabSourceNameMap[child->id] = child->name;
            QTreeWidgetItem* childItem = new QTreeWidgetItem(treeItem, {childName});
            itemMap[child] = childItem;
        }
    }

    ui->treeWgt->addTopLevelItems(treeItems);

    // 从配置文件中读取每个item的状态，将对应的按钮setChecked
    QSettings settings(menuStateConfigPath, QSettings::IniFormat);
    settings.setIniCodec("utf-8");
//    QMap<MenuItem*, int> menuItemStateCache;

    // 读取 menuItem 相关配置
    for (auto menuItem : menuItems) {
        int savedState = settings.value(QString::number(menuItem->id) + "/state", static_cast<int>(MenuState::Operator)).toInt();
//        menuItemStateCache[menuItem] = savedState;
        menuItem->state = static_cast<MenuState>(savedState);

        menuStateMap[menuItem->id] = static_cast<MenuState>(savedState);
        // 子菜单项的状态
        for (auto& child : menuItem->children) {
            int savedChildState = settings.value(QString::number(child->id) + "/state", static_cast<int>(MenuState::Operator)).toInt();
//            menuItemStateCache[child] = savedChildState;
            child->state = static_cast<MenuState>(savedChildState);

            menuStateMap[child->id] = static_cast<MenuState>(savedChildState);
        }
    }

    // 配置文件没有系统设置 id = 4 的权限和信息，设置为普通权限
//    menuStateMap[4] = MenuState::Operator;

    // 遍历每个菜单项和子项，设置按钮组
    for (auto menuItem : menuItems) {

        if (menuItem != systemSet) {
            QButtonGroup* buttonGroup = new QButtonGroup(this);
            buttonGroup->setExclusive(true);

            for (int i = 1; i < ui->treeWgt->columnCount() - 1; ++i) {
                QRadioButton* button = new QRadioButton(ui->treeWgt);
                button->setFixedSize(45, 45);

                QTreeWidgetItem* item = itemMap[menuItem];
                ui->treeWgt->setItemWidget(item, i, button);

                buttonGroup->addButton(button, i);

//                int savedState = menuItemStateCache.value(menuItem);
                int savedState = static_cast<int>(menuItem->state);

                if (savedState == (i - 1)) {
                    button->setChecked(true);
                }

                connect(buttonGroup, QOverload<int>::of(&QButtonGroup::buttonPressed), this, [=](int id) {
                    MenuState newState = static_cast<MenuState>(id - 1);
                    if (menuItem->state != newState) {
                        menuItem->state = newState;
                        buttonGroup->button(id)->repaint();
                        emit menuItem->stateChanged(newState);
//                        saveMenuState(menuItems);
                    }
                });
            }
        }

        // 子菜单项
        for (auto& child : menuItem->children) {
            QButtonGroup* buttonGroup = new QButtonGroup(this);
            buttonGroup->setExclusive(true);

            for (int i = 1; i < ui->treeWgt->columnCount(); ++i) {
                QRadioButton* button = new QRadioButton(ui->treeWgt);
                button->setFixedSize(45, 45);

                QTreeWidgetItem* childItem = itemMap[child];
                ui->treeWgt->setItemWidget(childItem, i, button);

                buttonGroup->addButton(button, i);

//                int savedState = menuItemStateCache.value(menuItem);
                int savedState = static_cast<int>(child->state);
                if (savedState == (i - 1)) {
                    button->setChecked(true); // 恢复选中状态
                }

                connect(buttonGroup, QOverload<int>::of(&QButtonGroup::buttonPressed), this, [=](int id) {
                    MenuState newState = static_cast<MenuState>(id - 1);
                    if (child->state != newState) {
                        child->state = newState;
                        buttonGroup->button(id)->repaint();
                        emit child->stateChanged(newState);  // 发出信号
//                        saveMenuState(menuItems);
                    }
                });
            }
        }

        // 监听状态变化信号
        connect(menuItem, &MenuItem::stateChanged, this, &Setting::onMenuStateChanged);
        for (auto& child : menuItem->children)
        {
            connect(child, &MenuItem::stateChanged, this, &Setting::onMenuStateChanged);
        }
    }

    connect(ui->btnSaveMenuAuth, &QPushButton::clicked, [=](){
        handleLoginModeChanged(LoginDialog::getLoginMode());
        ::sync();
//        REFRESH_KERNEL_BUFFER(menuStateConfigPath.toStdString().c_str())
    });
}

void Setting::setupSystemSetting()
{
    // langurage set
    static std::vector<QCheckBox*> chboxsLang = {
        ui->chboxChinese, ui->chboxEnglish, ui->chboxKorean
    };
    if (m_SystemSet.lan < chboxsLang.size())
    {
        chboxsLang.at(m_SystemSet.lan)->setChecked(true);
    }


    static int lastLang = m_SystemSet.lan;
    for (int i = 0; i < chboxsLang.size(); i++)
    {
        auto chbox = chboxsLang[i];
        connect(chbox, &QCheckBox::clicked, [this, i](bool checked){
            bool langChanged = (lastLang != i);
            if (checked && langChanged)
            {
                ErrorTipDialog tip(tr("确认切换语言？"), TipMode::NORMAL, nullptr);
                int reply = tip.exec();

                if (reply == QDialog::Accepted)
                {
                    m_SystemSet.lan = i;
                    lastLang = i;

                    setSystemSet(m_SystemSet);
                    switchLangurage();
                }
                else
                {
                    if (m_SystemSet.lan < chboxsLang.size())
                    {
                        chboxsLang.at(m_SystemSet.lan)->setChecked(true);
                    }
                }
            }
        });
    }

    connect(ui->coboxFonts, QOverload<int>::of(&QComboBox::activated), this, [=](int index){
        if (index != m_SystemSet.typeFace) {
            updateAppFont();
            m_SystemSet.typeFace = index;
            ::setSystemSet(m_SystemSet);
        }
    });
    connect(ui->coboxFontSize, QOverload<int>::of(&QComboBox::activated), this, [=](int index){
        if (index != m_SystemSet.wordSize) {
            updateAppFont();
            m_SystemSet.wordSize = index;
            ::setSystemSet(m_SystemSet);
        }
    });

    // User Settings
    ui->editBrightTime->setInputRange(30, 65535);
    connect(ui->editBrightTime, &NumberEdit::returnPressed, [=](){
        int second = ui->editBrightTime->text().toUInt();
        BackLighter::instance()->setScreenOffTime(second);
        m_SystemSet.backlightTime = second;
        ::setSystemSet(m_SystemSet);
    });

    connect(ui->sliderBrightness, &QSlider::valueChanged, [=](int value){
//        qDebug() << "value of slider:" << value;
        BackLighter::instance()->setBrightness(value);
        ui->percentageBrightness->setValue(value);
//        qDebug() << "brightness:" << BackLighter::instance()->getBrightness();
    });
    connect(ui->sliderBrightness, &QSlider::sliderReleased, [=](){
        m_SystemSet.backlightBrightness = ui->sliderBrightness->value();
        ::setSystemSet(m_SystemSet);
    });

    connect(ui->chboxBuzzer, &QCheckBox::stateChanged, [=](int state){
        bool enable = state;
        Beeper::instance()->setEnable(enable);

        m_SystemSet.keyListen = state;
        ::setSystemSet(m_SystemSet);
    });

    //触摸屏校准按钮
    connect(ui->btnTouchCalibration,&QPushButton::clicked,this,[=]() {
        int reply = MainWindow::pMainWindow->showErrorTip(tr("触摸校准会重启系统，请确认是否校准？"));
        if (reply == QDialog::Accepted)
        {
            QProcess_execute("rm",QStringList() <<"/etc/pointercal");
            QProcess_execute("reboot",QStringList());
        }
    });

    connect(ui->btnSaveSysName, &QPushButton::clicked, [=](){
        QString sysName = ui->editSystemName->text();
        m_SystemSet.sysName = sysName;
        ::setSystemSet(m_SystemSet);
        emit sysNameChanged(sysName);
    });
    connect(ui->btnDefaultName, &QPushButton::clicked, [=](){
        const QString defaultSysName = QString("展晖机床机械手系统");
        m_SystemSet.sysName = defaultSysName;
        ::setSystemSet(m_SystemSet);
        emit sysNameChanged(defaultSysName);
    });

    connect(ui->btnSaveColor, &QPushButton::clicked, [=]() {
        static const std::vector<QString> styles = {
            ":/styleSheets/style.qss",                // 默认
            "/opt/MachineTool/configs/style/style_orange_color.qss", // 橙色
            "/opt/MachineTool/configs/style/style_yellow_color.qss", // 黄色
            "/opt/MachineTool/configs/style/style_green_color.qss",  // 绿色
            "/opt/MachineTool/configs/style/style_brown_color.qss"   // 棕色
        };

        const std::vector<QCheckBox*> colorCheckBoxes = {
            ui->chboxColorDefault, // 默认
            ui->chboxColorOriange, // 橙色
            ui->chboxColorYellow,  // 黄色
            ui->chboxColorGreen,   // 绿色
            ui->chboxBrown         // 棕色
        };

        auto it = std::find_if(colorCheckBoxes.begin(), colorCheckBoxes.end(), [](QCheckBox* checkbox) {
            return checkbox && checkbox->isChecked();
        });

//        static uint8_t setColor;
//        setColor = (it != colorCheckBoxes.end()) ? std::distance(colorCheckBoxes.begin(), it) : 0;
        uint8_t setColor = (it != colorCheckBoxes.end()) ? std::distance(colorCheckBoxes.begin(), it) : 0;

        if (m_SystemSet.sysColor == setColor) {
            return;
        }
        ErrorTipDialog tip (tr("确认修改颜色？系统将重启！"), nullptr);
        auto reply =  tip.exec();
        if (reply == QDialog::Accepted)
        {
            m_SystemSet.sysColor = setColor;
            ::setSystemSet(m_SystemSet);

            ::sync();
            ::system("reboot");
        }


#if 0
        static QProgressDialog progressDialog(tr("Please wait, applying new color settings..."), tr("取消"), 0, 0, this);
        progressDialog.setCancelButton(nullptr);
        progressDialog.setWindowModality(Qt::WindowModal);
        progressDialog.setFixedSize(500, 300);
        QCoreApplication::processEvents();
        progressDialog.setStyleSheet(
            "QProgressDialog {"
            "   background-color: #24B4A5;"
            "   border-radius: 10px;"
            "}"
            "QProgressBar {"
            "   border: 2px solid white;"
            "   border-radius: 5px;"
            "   background: #ffffff;"
            "   color: #000000;"
            "}"
        );

        QTimer::singleShot(10, [setColor]() {
            QFile file(styles[setColor]);
            if (file.open(QIODevice::ReadOnly)) {
                qApp->setStyleSheet(QString::fromLatin1(file.readAll()));
                file.close();
            }
            progressDialog.close();
        });

        progressDialog.exec();
#endif
    });

    connect(ui->btnSaveTime, &QPushButton::clicked, [=](){

        ErrorTipDialog tip(tr("确定修改系统时间？"), nullptr);
        if (tip.exec() == QDialog::Rejected) return;

        int year = ui->editYear->text().toInt();
        int month = ui->editMonth->text().toInt();
        int day = ui->editDay->text().toInt();
        int hour = ui->editHour->text().toInt();
        int minute = ui->editMinute->text().toInt();
        int second = ui->editSecond->text().toInt();

        QDate date(year, month, 1);
         int daysInMonth = date.daysInMonth();
         if (day < 1 || day > daysInMonth) {
             ErrorTipDialog tip(tr("时间格式错误！"), TipMode::ONLY_OK, nullptr);
             tip.exec();
             return;
         }

        tm t;
        t.tm_year = year - 1900;
        t.tm_isdst = 0;
        t.tm_mon = month - 1;
        t.tm_mday = day;
        t.tm_hour = hour;
        t.tm_min = minute;
        t.tm_sec = second;

        TimeSetter::instance()->setTime(t);
    });


    // Password setting
    connect(ui->btnSavePasswdAdmin, &QPushButton::clicked, [=]() {
        QList<NumberEdit*> edits = {ui->editAdminOldPasswd, ui->editAdminNewPasswd, ui->editAdminNewPasswdConfirm};
        handleSavePasswd(&passwd[0], edits, 0);
    });

    connect(ui->btnSavePasswdSuperAdmin, &QPushButton::clicked, [=]() {
        QList<NumberEdit*> edits = {ui->editSuperAdminOldPasswd, ui->editSuperAdminNewPasswd, ui->editSuperAdminNewPasswdConfirm};
        handleSavePasswd(&passwd[1], edits, 1);
    });

    connect(ui->btnSavePasswdMenu, &QPushButton::clicked, [=]() {
        QList<NumberEdit*> edits = {ui->editMenuOldPasswd, ui->editMenuNewPasswd, ui->editMenuNewPasswdConfirm};
        handleSavePasswd(&passwd[2], edits, 2);
    });

    // Notepad

    connect(ui->tableWgtNote, &QTableWidget::currentCellChanged, this, [=](int row, int){
        if (row >= 0) {
            // 获取第一列的标题内容
            QString title = ui->tableWgtNote->item(row, 0)->text();
            ui->editNoteTitle->setText(title);

            // 构建文件名
            QString fileName = noteDirPath + title + ".txt";

            // 读取文件内容
            QFile file(fileName);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream in(&file);
                QString content = in.readAll();
                ui->plainTextEditNote->setPlainText(content);
                file.close();
            } else {
                ui->plainTextEditNote->setPlainText("无法打开文件: " + fileName);
            }
        }
    });
    connect(ui->btnEditNote, &QPushButton::clicked, this, [=](){
        auto keyboard = FullKeyboard::instance();
        keyboard->clearText();
        keyboard->setCurrentEditObj(nullptr);
        keyboard->exec();

        QString input = keyboard->getInputText();

        QTextCursor cursor = ui->plainTextEditNote->textCursor();

//        cursor.clearSelection();
//        cursor.movePosition(QTextCursor::NextWord, QTextCursor::KeepAnchor);
        cursor.insertText(input);

        ui->plainTextEditNote->setTextCursor(cursor);
    });
    connect(ui->btnSaveNote, &QPushButton::clicked, this, [=](){
        int currentRow = ui->tableWgtNote->currentRow();
        if (currentRow >= 0)
        {
            QString newTitle = ui->editNoteTitle->text().trimmed();

            QRegularExpression illegalChars("[<>:\"/\\\\|?*.]");
            if (newTitle.isEmpty())
            {
                MainWindow::pMainWindow->showErrorTip(tr("标题不能为空！"), TipMode::ONLY_OK);
                return;
            }
            else if (newTitle.contains(illegalChars))
            {
                MainWindow::pMainWindow->showErrorTip(tr("文件名格式错误！"), TipMode::ONLY_OK);
                return;
            }
            QString newFileName = noteDirPath + newTitle + ".txt";

            QString title = ui->tableWgtNote->item(currentRow, 0)->text();
            QString fileName = noteDirPath + title + ".txt";

            QFile file(newFileName);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                QTextStream out(&file);
                QString content = ui->plainTextEditNote->toPlainText();
                out << content;

                file.flush();
                file.close();
                sync();

                if (fileName != newFileName)
                {
                    if (QFile::exists(fileName))
                    {
                        QFile::remove(fileName);
                    }
                }

                ui->tableWgtNote->item(currentRow, 0)->setText(newTitle);
                QString revisedTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
                ui->tableWgtNote->item(currentRow, 1)->setText(revisedTime);
            }
        }
    });
    connect(ui->btnBackspaceNote, &QPushButton::clicked, this, [=]() {
        QTextCursor cursor = ui->plainTextEditNote->textCursor();

        if (!cursor.isNull() && cursor.position() > 0) {
            cursor.deletePreviousChar();

            ui->plainTextEditNote->setTextCursor(cursor);
        }
    });

    connect(ui->btnNewNote, &QPushButton::clicked, this, [=](){
        QDir dir(noteDirPath);
         if (!dir.exists()) {
             if (!dir.mkpath(".")) {
                 qDebug() << "Failed to create directory:" << noteDirPath;
                 return;
             }
         }

       int rowCount = ui->tableWgtNote->rowCount();

       QString newTitle = QString("untitled%1").arg(rowCount + 1);
       QString newFileName = noteDirPath + newTitle + ".txt";

       int count = 1;
       while (QFile::exists(newFileName)) {
           newFileName = noteDirPath + QString("untitled%1").arg(rowCount + 1 + count) + ".txt";
           count++;
       }

       QFile file(newFileName);
       if (file.open(QIODevice::WriteOnly | QIODevice::Text))
       {
           file.close();

           QString creationTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

           ui->tableWgtNote->insertRow(rowCount);
           ui->tableWgtNote->setItem(rowCount, 0, new QTableWidgetItem(newTitle));
           ui->tableWgtNote->setItem(rowCount, 1, new QTableWidgetItem(creationTime));

           ui->editNoteTitle->setText(newTitle);
           ui->plainTextEditNote->clear();
       }
    });

    connect(ui->btnDeleteNote, &QPushButton::clicked, this, [=]() {
        int reply = MainWindow::pMainWindow->showErrorTip(tr("确定删除？"));
        if (reply == QDialog::Rejected)
            return;

        int currentRow = ui->tableWgtNote->currentRow();
        if (currentRow >= 0)
        {
            QString title = ui->tableWgtNote->item(currentRow, 0)->text();
            QString fileName = noteDirPath + title + ".txt";

            QFile file(fileName);
            if (file.exists())
            {
                if (file.remove())
                {
                    ui->tableWgtNote->removeRow(currentRow);
                }
            }
        }
    });

    /****************************************************************************/
    // Internet of Things
    connect(ui->coboxIOTSelection, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index){
    //    qDebug() << "index = " << index;
    if (index == 2)
    {
        ui->stkWidgetIPSet->setCurrentIndex(0);

        // disable these object
        ui->stkWidgetIPSet->setDisabled(true);
        ui->widgetIotDebug->setDisabled(true);
    }
    else
    {
        ui->stkWidgetIPSet->setEnabled(true);
        ui->widgetIotDebug->setEnabled(true);
    }
    ui->stkWidgetIPSet->setCurrentIndex(index);
    });

    // Registration Information
    {
            QPushButton* inputBtns[16] = {
                ui->btn0, ui->btn1, ui->btn2, ui->btn3, ui->btn4, ui->btn5, ui->btn6, ui->btn7,
                ui->btn8, ui->btn9, ui->btn10, ui->btn11, ui->btn12, ui->btn13, ui->btn14, ui->btn15
            };
            QString hexChars[16] = {
                "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"
            };
            for (int i = 0; i < 16; i++)
            {
                connect(inputBtns[i], &QPushButton::clicked, this, [=](){
                    if (registerCode.size() < 32)
                    {
                        QString hexChar = hexChars[i];
                        registerCode.append(hexChar);
                        updateRegisterCodeDisplay();
                    }
                });
            }

            connect(ui->btnClearCode, &QPushButton::clicked, this, [=]() {
                if(registerCode.isEmpty())
                    return;
                registerCode.clear();
                registerCode.squeeze();
                updateRegisterCodeDisplay();
            });

            connect(ui->btnBackspaceCode, &QPushButton::clicked, this, [=]() {
                if (!registerCode.isEmpty())
                {
                    registerCode.removeLast();
                    updateRegisterCodeDisplay();
                }
            });

    }
}

//void Setting::saveMenuState(const QList<MenuItem*>& menuItems)
//{
//    QSettings settings(menuStateConfigPath, QSettings::IniFormat);
//    for (const auto menuItem : menuItems) {
//        settings.beginGroup(menuItem->name);
//        settings.setValue("state", static_cast<int>(menuItem->state));
//        settings.endGroup();
//        for (auto& child : menuItem->children) {
//            settings.beginGroup(child->name);
//            settings.setValue("state", static_cast<int>(child->state));
//            settings.endGroup();
//        }
//    }
//}

void Setting::syncParaToUI()
{
    SetWidgetAxisQcomboboxHandel();
    emit AxisTypeChange_Signal();
    /****************************系统设置********************************************/
//    m_SystemSet.lan;
//    ui->coboxFonts->setCurrentIndex(m_SystemSet.typeFace);
//    ui->coboxFontSize->setCurrentIndex(m_SystemSet.wordSize);

//    ui->chboxBuzzer->setChecked(m_SystemSet.keyListen);
//    ui->editBrightTime->setText(QString::number(m_SystemSet.backlightTime));
//    ui->sliderBrightness->setValue(m_SystemSet.backlightBrightness);

    /****************************输出类型********************************************/
    for (int i = 0;i < OUT_PORT_TYPE_NUM;i++)
    {
        outputTypeList[i]->setCurrentIndex(m_OutPortType[i][1]);
    }
    /****************************互锁设置********************************************/
    for (int i = 0; i < OUT_INTERLOCK_NUM; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            bool checked = m_OutportInterlock[i][j] != 0;
            outportInterlockList[4*i+j]->setChecked(checked);
        }
    }

    /****************************预留关联********************************************/
    for (int i=0;i<OUT_PORT_RELEVANCY_NUM;i++)
    {
        outportRelevancyList[i]->setCurrentIndex(m_OutportRelevancy[i][2]);
    }
    /****************************预留出类型********************************************/
    for (int i=0;i<OUT_PORT_RELEOUT_NUM;i++)
    {
        outportReleteOutList[i]->setCurrentIndex(m_OutportReleteOut[i][1]);
    }
    /****************************按键和信号********************************************/
    ::readKeySetStrFromConfig(keyFunDesription);

    for (int i = 0; i < OPR_KEY_NUM; i++)
    {
        keyEdits.at(i)->setKeyFunc(m_KeyFunc[i].keyType, m_KeyFunc[i].funcNum, m_KeyFunc[i].oprMode);
        //
//        QString keyFunStr = getKeyFuncDescription(m_KeyFunc[i].keyType, m_KeyFunc[i].funcNum, m_KeyFunc[i].oprMode);
//        keyEdits.at(i)->setText(keyFunStr);
        keyEdits.at(i)->setText(keyFunDesription[i]);
    }

    ::readSigSetStrFromConfig(sigSetDesription);
    for (int i = 0; i < OPR_LED_NUM; i++)
    {
        sigEdits.at(i)->setSigTypeAndPort(m_LedFunc[i].ledType, m_LedFunc[i].funcNum);

//        QString sigStr = getSigDescription(m_LedFunc[i].ledType, m_LedFunc[i].funcNum);
//        sigEdits.at(i)->setText(sigStr);
        sigEdits.at(i)->setText(sigSetDesription[i]);
    }

    /****************************高级********************************************/
    {
        int index = 0;
        seniorFuncList[index++]->setCurrentIndex(m_SeniorFunc.knifeOrigin1Check);
        seniorFuncList[index++]->setCurrentIndex(m_SeniorFunc.knifeOrigin2Check);
        seniorFuncList[index++]->setCurrentIndex(m_SeniorFunc.chuckOriginCheck);
        seniorFuncList[index++]->setCurrentIndex(m_SeniorFunc.stackSaveIn1Check);
        seniorFuncList[index++]->setCurrentIndex(m_SeniorFunc.stackSaveIn2Check);
        seniorFuncList[index++]->setCurrentIndex(m_SeniorFunc.stackSaveOutCheck);
        seniorFuncList[index++]->setCurrentIndex(m_SeniorFunc.alarmIn1Check);
        seniorFuncList[index++]->setCurrentIndex(m_SeniorFunc.alarmIn2Check);

        seniorFuncList[index++]->setCurrentIndex(m_SeniorFunc.emergencyStopCheck);
        seniorFuncList[index++]->setCurrentIndex(m_SeniorFunc.pauseStopCheck);
        seniorFuncList[index++]->setCurrentIndex(m_SeniorFunc.pressureCheck);
        seniorFuncList[index++]->setCurrentIndex(m_SeniorFunc.remoteAuto);
        seniorFuncList[index++]->setCurrentIndex(m_SeniorFunc.remoteStop);
        seniorFuncList[index++]->setCurrentIndex(m_SeniorFunc.bottomOilLimit);
        seniorFuncList[index++]->setCurrentIndex(m_SeniorFunc.oilerSwt);
        seniorFuncList[index++]->setCurrentIndex(m_SeniorFunc.lubPump);

        seniorFuncList[index++]->setCurrentIndex(m_SeniorFunc.processSave1);
        seniorFuncList[index++]->setCurrentIndex(m_SeniorFunc.processSave2);
        seniorFuncList[index++]->setCurrentIndex(m_SeniorFunc.emergencySnapMotorEnable);
        seniorFuncList[index++]->setCurrentIndex(m_SeniorFunc.emergencyStopOut);
        seniorFuncList[index++]->setCurrentIndex(m_SeniorFunc.autoLight);
        seniorFuncList[index++]->setCurrentIndex(m_SeniorFunc.alarmLight);
        seniorFuncList[index++]->setCurrentIndex(m_SeniorFunc.alarmBuzzer);
        seniorFuncList[index++]->setCurrentIndex(m_SeniorFunc.pauseLight);

        seniorFuncList[index++]->setCurrentIndex(m_SeniorFunc.processFinish1);
        seniorFuncList[index++]->setCurrentIndex(m_SeniorFunc.processFinish2);
        seniorFuncList[index++]->setCurrentIndex(m_SeniorFunc.locateFinish1);
        seniorFuncList[index++]->setCurrentIndex(m_SeniorFunc.locateFinish2);
        seniorFuncList[index++]->setCurrentIndex(m_SeniorFunc.startProduct1);
        seniorFuncList[index++]->setCurrentIndex(m_SeniorFunc.startProduct2);
        seniorFuncList[index++]->setCurrentIndex(m_SeniorFunc.res1[0]);
        seniorFuncList[index++]->setCurrentIndex(m_SeniorFunc.res1[1]);

        seniorFuncList[index++]->setCurrentIndex(m_SeniorFunc.mainAxisRotate1);
        seniorFuncList[index++]->setCurrentIndex(m_SeniorFunc.mainAxisRotate2);
        seniorFuncList[index++]->setCurrentIndex(m_SeniorFunc.mainAxisLocate1);
        seniorFuncList[index++]->setCurrentIndex(m_SeniorFunc.mainAxisLocate2);
        seniorFuncList[index++]->setCurrentIndex(m_SeniorFunc.biowAir1);
        seniorFuncList[index++]->setCurrentIndex(m_SeniorFunc.biowAir2);
        seniorFuncList[index++]->setCurrentIndex(m_SeniorFunc.res2[0]);
        seniorFuncList[index++]->setCurrentIndex(m_SeniorFunc.res2[1]);

        seniorFuncList[index++]->setCurrentIndex(m_SeniorFunc.manualChuckIn1);
        seniorFuncList[index++]->setCurrentIndex(m_SeniorFunc.manualChuckIn2);
        seniorFuncList[index++]->setCurrentIndex(m_SeniorFunc.autoDoorCot1);
        seniorFuncList[index++]->setCurrentIndex(m_SeniorFunc.autoDoorCot2);
        seniorFuncList[index++]->setCurrentIndex(m_SeniorFunc.res3[0]);
        seniorFuncList[index++]->setCurrentIndex(m_SeniorFunc.res3[1]);
        seniorFuncList[index++]->setCurrentIndex(m_SeniorFunc.res3[2]);
        seniorFuncList[index++]->setCurrentIndex(m_SeniorFunc.res3[3]);
    }

    /****************************机床安全和料仓安全********************************************/
    machineSafeList[0]->setCurrentIndex(m_MachineSave.armFallAutoDoor1Swt);
    machineSafeList[1]->setCurrentIndex(m_MachineSave.armFallAutoDoor1SwtSta);
    machineSafeList[2]->setCurrentIndex(m_MachineSave.armFallAutoDoor2Swt);
    machineSafeList[3]->setCurrentIndex(m_MachineSave.armFallAutoDoor2SwtSta);
    machineSafeList[4]->setCurrentIndex(m_MachineSave.firstProductStartRun);
    machineSafeList[5]->setCurrentIndex(m_MachineSave.firstProductFinish);

    stackBinSafeList[0]->setCurrentIndex(m_StackSave.stackSaveIn1Roatore);
    stackBinSafeList[1]->setCurrentIndex(m_StackSave.stackSaveIn2Roatore);

    /****************************卡爪安全********************************************/

    for (size_t i = 0; i < clawSafeWidgets.size(); i++)
    {
        UpdateClawSafepage(i);
    }

    /****************************联机安全********************************************/
    for (size_t i = 0; i < onlineSafeWidgets.size(); i++)
    {
        UpdateOnlineSafepage(i);
    }

    /****************************产品设置********************************************/
    productSet.at(0)->setText(QString::number(m_ProductSet.waitProcessFinTime / 10.0, 'f', 1));
    productSet.at(1)->setText(QString::number(m_ProductSet.pluseOutTime / 10.0, 'f', 1));
    productSet.at(2)->setText(QString::number(m_ProductSet.airVlaveLimitTime / 10.0, 'f', 1));
    productSet.at(3)->setText(QString::number(m_ProductSet.autoCycTime / 10.0, 'f', 1));
    productSet.at(4)->setText(QString::number(m_ProductSet.alarmBuzzerTime / 10.0, 'f', 1));
    productSet.at(5)->setText(QString::number(m_ProductSet.chainReachDelay / 10.0, 'f', 1));
    productSet.at(6)->setText(QString::number(m_ProductSet.clampCheckFilterTime / 10.0, 'f', 1));

    // 高级
    productSeniorEdit.at(0)->setText(QString::number(m_ProductSenior.lubricOpenTime / 10.0, 'f', 1));
    productSeniorEdit.at(1)->setText(QString::number(m_ProductSenior.lubricCloseTime / 10.0, 'f', 1));
    productSeniorEdit.at(2)->setText(QString::number(m_ProductSenior.lubricDelay / 10.0, 'f', 1));
    productSeniorEdit.at(3)->setText(QString::number(m_ProductSenior.lubricGapCyc));
    ui->coboxProductMemory->setCurrentIndex(m_ProductSenior.productMemoryFlag);

    // 物联网
    productIot.at(0)->setCurrentIndex(m_Internet.netCommType);
    productIot.at(1)->setCurrentIndex(m_Internet.parityMode);
    productIot.at(2)->setCurrentIndex(m_Internet.baudMode);

    /****************************伺服参数********************************************/
    // 伺服
    updateServoParaPage();

    /****************************机器参数********************************************/

    readLimitSigDescription(0, minLimitStrs);
    readLimitSigDescription(1, maxLimitStrs);
    readLimitSigDescription(2, originSigStrs);

    updateAxisParPage();

    /****************************安全区********************************************/
    for (int index = 0; index < SAVE_AREA_NUM; index++)
    {
        updateSafeAreaPage(index);
    }

    /****************************伺服安全点--位置限定********************************************/
    updatehorizontalPosList();

    /****************************堆叠设置********************************************/
    for (int i = 0; i < 8; i++)
    {
        stack[i]->syncParaToUI();
//        stack[i]->switchStackWay((StackMode)m_StackFunc.stackType);
    }

    stackCoboxs.at(0)->setCurrentIndex(m_StackFunc.stackType);
    stackCoboxs.at(1)->setCurrentIndex(m_StackFunc.groupStack);
    stackCoboxs.at(2)->setCurrentIndex(m_StackFunc.rowStack);
    stackCoboxs.at(3)->setCurrentIndex(m_StackFunc.storeyStack);
    stackCoboxs.at(4)->setCurrentIndex(m_StackFunc.divideStack);
    stackCoboxs.at(5)->setCurrentIndex(m_StackFunc.stackMemory);
    stackCoboxs.at(6)->setCurrentIndex(m_StackFunc.siloType);
    stackCoboxs.at(7)->setCurrentIndex(m_StackFunc.rotateSiloFinHint);
    ui->editRotateSiloPlaceNum->setText(QString::number(m_StackFunc.rotateSiloPlaceNum));

}
//更新卡爪安全界面参数 index：卡爪安全1-卡爪安全4
void Setting::UpdateClawSafepage(int index)
{
    for (size_t i = 0; i < clawSafeWidgets.size(); i++)
    {
        clawSafeWidgets.at(index).clawKeepoutSta->setCurrentIndex(m_ClawSave[index].clawKeepoutSta);

        int axisXItemIndex = 0;
        if (m_ClawSave[index].axisSelect_X == X1_AXIS+1 && m_AxisPar[X1_AXIS].axisType == 1)
        {
            axisXItemIndex = 1;
        }
        clawSafeWidgets.at(index).axisSelect_X->setCurrentIndex(axisXItemIndex);

        int axisZItemIndex = 0;
        if(m_ClawSave[index].axisSelect_Z == Z1_AXIS + 1  && m_AxisPar[Z1_AXIS].axisType == 1)
        {
            axisZItemIndex = 1;
        }
        else if (m_ClawSave[index].axisSelect_Z == Z2_AXIS + 1  && m_AxisPar[Z2_AXIS].axisType == 1)
        {
            axisZItemIndex = 2;
        }
//        clawSafeWidgets.at(i).axisSelect_X->setCurrentIndex(axisZItemIndex);

        clawSafeWidgets.at(index).clawKeepoutMinX->setText(QString::number(m_ClawSave[index].clawKeepoutMinX / 100.0, 'f', 2));
        clawSafeWidgets.at(index).clawKeepoutMaxX->setText(QString::number(m_ClawSave[index].clawKeepoutMaxX / 100.0, 'f', 2));
        clawSafeWidgets.at(index).clawKeepoutHighZ->setText(QString::number(m_ClawSave[index].clawKeepoutHighZ / 100.0, 'f', 2));

        clawSafeWidgets.at(index).cKeepoutMinX->setText(QString::number(m_ClawSave[index].cKeepoutMinX / 100.0, 'f', 2));
        clawSafeWidgets.at(index).cKeepoutMaxX->setText(QString::number(m_ClawSave[index].cKeepoutMaxX / 100.0, 'f', 2));
        clawSafeWidgets.at(index).cKeepoutHighZ->setText(QString::number(m_ClawSave[index].cKeepoutHighZ / 100.0, 'f', 2));
        clawSafeWidgets.at(index).cKeepoutPosMinC->setText(QString::number(m_ClawSave[index].cKeepoutPosMinC / 100.0, 'f', 2));
        clawSafeWidgets.at(index).cKeepoutPosMaxC->setText(QString::number(m_ClawSave[index].cKeepoutPosMaxC / 100.0, 'f', 2));
        clawSafeWidgets.at(index).cKeepoutNegMinC->setText(QString::number(m_ClawSave[index].cKeepoutNegMinC / 100.0, 'f', 2));
        clawSafeWidgets.at(index).cKeepoutNegMaxC->setText(QString::number(m_ClawSave[index].cKeepoutNegMaxC / 100.0, 'f', 2));

    }
}
//更新联机安全界面参数 index：联机区域1-联机区域4
void Setting::UpdateOnlineSafepage(int index)
{
    onlineSafeWidgets.at(index).mainSunMode->setCurrentIndex(m_OnlineSave[index].mainSunMode);
    onlineSafeWidgets.at(index).pluseTime->setText(QString::number(m_OnlineSave[index].pluseTime / 100.0, 'f', 2));
    onlineSafeWidgets.at(index).onlineSelect->setCurrentIndex(m_OnlineSave[index].onlineSelect);

    int onlineAxisIndex = 0;
    if (m_OnlineSave[index].axisNum == 1)
    {
        onlineAxisIndex = 1;
    }
    onlineSafeWidgets.at(index).axisNum->setCurrentIndex(onlineAxisIndex);

    onlineSafeWidgets.at(index).areaInNum->setCurrentPort(m_OnlineSave[index].areaInNum);
    onlineSafeWidgets.at(index).areaOutNum->setCurrentPort(m_OnlineSave[index].areaOutNum);
    onlineSafeWidgets.at(index).requestInNum->setCurrentPort(m_OnlineSave[index].requestInNum);
    onlineSafeWidgets.at(index).requestOutNum->setCurrentPort(m_OnlineSave[index].requestOutNum);

    onlineSafeWidgets.at(index).a1A2MainPos->setText(QString::number(m_OnlineSave[index].a1A2MainPos / 100.0, 'f', 2));
    onlineSafeWidgets.at(index).a1A2SunPos->setText(QString::number(m_OnlineSave[index].a1A2SunPos / 100.0, 'f', 2));

    std::vector<QString> tempStrs;
    readOnlineSafeInOutDescription(index, tempStrs);
    onlineSafeWidgets.at(index).areaInNum->setText(tempStrs[0]);
    onlineSafeWidgets.at(index).areaOutNum->setText(tempStrs[1]);
    onlineSafeWidgets.at(index).requestInNum->setText(tempStrs[2]);
    onlineSafeWidgets.at(index).requestOutNum->setText(tempStrs[3]);
}
//更新伺服界面参数
void Setting::updateServoParaPage()
{
    servoPara.at(0)->setCurrentIndex(m_ServoPar.encoderType);
    servoPara.at(1)->setCurrentIndex(m_ServoPar.encoderBitNum);
    servoPara.at(2)->setCurrentIndex(m_ServoPar.servoBrand);
    servoPara.at(3)->setCurrentIndex(m_ServoPar.servoModel);
    servoPara.at(4)->setCurrentIndex(m_ServoPar.extendIOBoard);
    servoPara.at(5)->setCurrentIndex(m_ServoPar.rotateAxisUnit);
    servoPara.at(6)->setCurrentIndex(m_ServoPar.resetSunProFlag);
    servoPara.at(7)->setCurrentIndex(m_ServoPar.torqueLimitFlag);
}
//更新轴参数界面参数
void Setting::updateAxisParPage()
{
    for (int i = 0; i < AXIS_TOTAL_NUM; i++)
    {
        machineParaWidgets.at(i).axisType->setCurrentIndex(m_AxisPar[i].axisType);
        machineParaWidgets.at(i).axisMoveMade->setCurrentIndex(m_AxisPar[i].axisMoveMade);
        machineParaWidgets.at(i).accTime->setText(QString::number(m_AxisPar[i].accTime / 100.0, 'f', 2));
        machineParaWidgets.at(i).decTime->setText(QString::number(m_AxisPar[i].decTime / 100.0, 'f', 2));
        machineParaWidgets.at(i).accAcc->setText(QString::number(m_AxisPar[i].accAcc));
        machineParaWidgets.at(i).decDec->setText(QString::number(m_AxisPar[i].decDec));
        machineParaWidgets.at(i).maxSpeed->setText(QString::number(m_AxisPar[i].maxSpeed));
        machineParaWidgets.at(i).axisMinPos->setText(QString::number(m_AxisPar[i].axisMinPos / 100.0, 'f', 2));
        machineParaWidgets.at(i).axisMaxPos->setText(QString::number(m_AxisPar[i].axisMaxPos / 100.0, 'f', 2));
        machineParaWidgets.at(i).limitMin->setCurrentPort(m_AxisPar[i].minPosSignal);
        machineParaWidgets.at(i).limitMax->setCurrentPort(m_AxisPar[i].maxPosSignal);
        machineParaWidgets.at(i).circlePluseNum->setText(QString::number(m_AxisPar[i].circlePluseNum));
        machineParaWidgets.at(i).circleDis->setText(QString::number(m_AxisPar[i].circleDis / 100.0, 'f', 2));
        machineParaWidgets.at(i).findOriginSpeed->setText(QString::number(m_AxisPar[i].findOriginSpeed));
        machineParaWidgets.at(i).leaveOriginSpeed->setText(QString::number(m_AxisPar[i].leaveOriginSpeed));
        machineParaWidgets.at(i).originOffset->setText(QString::number(m_AxisPar[i].originOffset / 100.0, 'f', 2));
        machineParaWidgets.at(i).originDir->setCurrentIndex(m_AxisPar[i].originDir);
        machineParaWidgets.at(i).originSignal->setCurrentPort(m_AxisPar[i].originSignal);
        machineParaWidgets.at(i).backOriginOrder->setCurrentIndex(m_AxisPar[i].backOriginOrder);
        machineParaWidgets.at(i).originType->setCurrentIndex(m_AxisPar[i].originType);
        machineParaWidgets.at(i).limitPosSwt->setCurrentIndex(m_AxisPar[i].limitPosSwt);
        machineParaWidgets.at(i).limitNegSwt->setCurrentIndex(m_AxisPar[i].limitNegSwt);
        machineParaWidgets.at(i).coordDir->setCurrentIndex(m_AxisPar[i].coordDir);

        machineParaWidgets.at(i).limitMin->setText(minLimitStrs[i]);
        machineParaWidgets.at(i).limitMax->setText(maxLimitStrs[i]);
        machineParaWidgets.at(i).originSignal->setText(originSigStrs[i]);

    }
    HandwheelParaWidgets.accTime->setText(QString::number(m_HandwheelPar.accTime / 100.0, 'f', 2));
    HandwheelParaWidgets.decTime->setText(QString::number(m_HandwheelPar.decTime / 100.0, 'f', 2));
    HandwheelParaWidgets.accAcc->setText(QString::number(m_HandwheelPar.accAcc));
    HandwheelParaWidgets.decDec->setText(QString::number(m_HandwheelPar.decDec));
}
//更新安全区界面参数
void Setting::updateSafeAreaPage(int index)
{
    // 设置开关和完成进程数
    servoPointSafeArea.at(index).safeAreaSwt->setCurrentIndex(m_SaveArea[index].saveAreaSwt);
    servoPointSafeArea.at(index).processFinishNum->setCurrentIndex(m_SaveArea[index].processFinishNum);
    if(m_SaveArea[index].saveAreaSwt == 1)
    {
        if(index == 0){
            ui->grboxXYParaSA1->setEnabled(true);
            ui->grboxZAxisMachineParaSA1->setEnabled(true);
            ui->grboxZAxisStockBinParaSA1->setEnabled(true);
        }else if(index == 1)
        {
            ui->grboxXYParaSA1_2->setEnabled(true);
            ui->grboxZAxisMachineParaSA1_2->setEnabled(true);
            ui->grboxZAxisStockBinParaSA1_2->setEnabled(true);
        }
        else if(index == 2)
        {
            ui->grboxXYParaSA1_3->setEnabled(true);
            ui->grboxZAxisMachineParaSA1_3->setEnabled(true);
            ui->grboxZAxisStockBinParaSA1_3->setEnabled(true);
        }
        else if(index == 3)
        {
            ui->grboxXYParaSA1_4->setEnabled(true);
            ui->grboxZAxisMachineParaSA1_4->setEnabled(true);
            ui->grboxZAxisStockBinParaSA1_4->setEnabled(true);
        }
    }
    else
    {
        if(index == 0){
            ui->grboxXYParaSA1->setEnabled(false);
            ui->grboxZAxisMachineParaSA1->setEnabled(false);
            ui->grboxZAxisStockBinParaSA1->setEnabled(false);
        }else if(index == 1)
        {
            ui->grboxXYParaSA1_2->setEnabled(false);
            ui->grboxZAxisMachineParaSA1_2->setEnabled(false);
            ui->grboxZAxisStockBinParaSA1_2->setEnabled(false);
        }
        else if(index == 2)
        {
            ui->grboxXYParaSA1_3->setEnabled(false);
            ui->grboxZAxisMachineParaSA1_3->setEnabled(false);
            ui->grboxZAxisStockBinParaSA1_3->setEnabled(false);
        }
        else if(index == 3)
        {
            ui->grboxXYParaSA1_4->setEnabled(false);
            ui->grboxZAxisMachineParaSA1_4->setEnabled(false);
            ui->grboxZAxisStockBinParaSA1_4->setEnabled(false);
        }
    }
    // X 轴
    int xAxis = m_SaveArea[index].axisSelect[0];
    if(xAxis == 0)
    {
        if(servoPointSafeArea.at(index).axisSelect[0]->count()>0)
        {
            servoPointSafeArea.at(index).axisSelect[0]->setCurrentIndex(0);
            if(index == 0){
                ui->labAxisXSelectSA1->setEnabled(false);
                ui->labAxisCSelectSA1->setEnabled(false);
                ui->editMachineA1AxisXSA1->setEnabled(false);
                ui->editMachineA2AxisXSA1->setEnabled(false);
                ui->editStockBinB1AxisXSA1->setEnabled(false);
                ui->editStockBinB2AxisXSA1->setEnabled(false);
                ui->editMachineA1AxisCSA1->setEnabled(false);
                ui->editMachineA2AxisCSA1->setEnabled(false);
            }else if(index == 1)
            {
                ui->labAxisXSelectSA1_2->setEnabled(false);
                ui->labAxisCSelectSA1_2->setEnabled(false);
                ui->editMachineA1AxisXSA1_2->setEnabled(false);
                ui->editMachineA2AxisXSA1_2->setEnabled(false);
                ui->editStockBinB1AxisXSA1_2->setEnabled(false);
                ui->editStockBinB2AxisXSA1_2->setEnabled(false);
                ui->editMachineA1AxisCSA1_2->setEnabled(false);
                ui->editMachineA2AxisCSA1_2->setEnabled(false);
            }
            else if(index == 2)
            {
                ui->labAxisXSelectSA1_3->setEnabled(false);
                ui->labAxisCSelectSA1_3->setEnabled(false);
                ui->editMachineA1AxisXSA1_3->setEnabled(false);
                ui->editMachineA2AxisXSA1_3->setEnabled(false);
                ui->editStockBinB1AxisXSA1_3->setEnabled(false);
                ui->editStockBinB2AxisXSA1_3->setEnabled(false);
                ui->editMachineA1AxisCSA1_3->setEnabled(false);
                ui->editMachineA2AxisCSA1_3->setEnabled(false);
            }
            else if(index == 3)
            {
                ui->labAxisXSelectSA1_4->setEnabled(false);
                ui->labAxisCSelectSA1_4->setEnabled(false);
                ui->editMachineA1AxisXSA1_4->setEnabled(false);
                ui->editMachineA2AxisXSA1_4->setEnabled(false);
                ui->editStockBinB1AxisXSA1_4->setEnabled(false);
                ui->editStockBinB2AxisXSA1_4->setEnabled(false);
                ui->editMachineA1AxisCSA1_4->setEnabled(false);
                ui->editMachineA2AxisCSA1_4->setEnabled(false);
            }
        }
    }
    else if(m_AxisPar[0].axisType == 1 && servoPointSafeArea.at(index).axisSelect[0]->count()>1)
    {
        servoPointSafeArea.at(index).axisSelect[0]->setCurrentIndex(1);
        if(index == 0){
            ui->labAxisXSelectSA1->setEnabled(true);
            ui->labAxisCSelectSA1->setEnabled(true);
            ui->editMachineA1AxisXSA1->setEnabled(true);
            ui->editMachineA2AxisXSA1->setEnabled(true);
            ui->editStockBinB1AxisXSA1->setEnabled(true);
            ui->editStockBinB2AxisXSA1->setEnabled(true);
            ui->editMachineA1AxisCSA1->setEnabled(true);
            ui->editMachineA2AxisCSA1->setEnabled(true);
        }else if(index == 1)
        {
            ui->labAxisXSelectSA1_2->setEnabled(true);
            ui->labAxisCSelectSA1_2->setEnabled(true);
            ui->editMachineA1AxisXSA1_2->setEnabled(true);
            ui->editMachineA2AxisXSA1_2->setEnabled(true);
            ui->editStockBinB1AxisXSA1_2->setEnabled(true);
            ui->editStockBinB2AxisXSA1_2->setEnabled(true);
            ui->editMachineA1AxisCSA1_2->setEnabled(true);
            ui->editMachineA2AxisCSA1_2->setEnabled(true);
        }
        else if(index == 2)
        {
            ui->labAxisXSelectSA1_3->setEnabled(true);
            ui->labAxisCSelectSA1_3->setEnabled(true);
            ui->editMachineA1AxisXSA1_3->setEnabled(true);
            ui->editMachineA2AxisXSA1_3->setEnabled(true);
            ui->editStockBinB1AxisXSA1_3->setEnabled(true);
            ui->editStockBinB2AxisXSA1_3->setEnabled(true);
            ui->editMachineA1AxisCSA1_3->setEnabled(true);
            ui->editMachineA2AxisCSA1_3->setEnabled(true);
        }
        else if(index == 3)
        {
            ui->labAxisXSelectSA1_4->setEnabled(true);
            ui->labAxisCSelectSA1_4->setEnabled(true);
            ui->editMachineA1AxisXSA1_4->setEnabled(true);
            ui->editMachineA2AxisXSA1_4->setEnabled(true);
            ui->editStockBinB1AxisXSA1_4->setEnabled(true);
            ui->editStockBinB2AxisXSA1_4->setEnabled(true);
            ui->editMachineA1AxisCSA1_4->setEnabled(true);
            ui->editMachineA2AxisCSA1_4->setEnabled(true);
        }
    }

    // Y 轴
    int yAxis = m_SaveArea[index].axisSelect[1];
    if (yAxis == 0) {
        if(servoPointSafeArea.at(index).axisSelect[1]->count()>0)
        {
            servoPointSafeArea.at(index).axisSelect[1]->setCurrentIndex(0);
        }
    }
    else if (yAxis == 2) { // Y1轴
        if(servoPointSafeArea.at(index).axisSelect[1]->count()>1)
        {
            servoPointSafeArea.at(index).axisSelect[1]->setCurrentIndex(1);
        }
    }
    else if (yAxis == 5) { // Y2轴
        if(servoPointSafeArea.at(index).axisSelect[1]->count()>2)
        {
            servoPointSafeArea.at(index).axisSelect[1]->setCurrentIndex(2);
        }
    }

    // Z 轴
    int zAxis = m_SaveArea[index].axisSelect[2];
    if (zAxis == 0) {
        if(servoPointSafeArea.at(index).axisSelect[2]->count()>0)
        {
            servoPointSafeArea.at(index).axisSelect[2]->setCurrentIndex(0);
            if(index == 0){
                ui->editMachinInsideHeightSA1->setEnabled(false);
                ui->editMachineZMaxFallPosSA1->setEnabled(false);
                ui->editMaxWaitPosSA1->setEnabled(false);
                ui->editMaxStartPosSA1->setEnabled(false);
                ui->editStockBinZMaxFallPosSA1->setEnabled(false);
            }else if(index == 1)
            {
                ui->editMachinInsideHeightSA1_2->setEnabled(false);
                ui->editMachineZMaxFallPosSA1_2->setEnabled(false);
                ui->editMaxWaitPosSA1_2->setEnabled(false);
                ui->editMaxStartPosSA1_2->setEnabled(false);
                ui->editStockBinZMaxFallPosSA1_2->setEnabled(false);
            }
            else if(index == 2)
            {
                ui->editMachinInsideHeightSA1_3->setEnabled(false);
                ui->editMachineZMaxFallPosSA1_3->setEnabled(false);
                ui->editMaxWaitPosSA1_3->setEnabled(false);
                ui->editMaxStartPosSA1_3->setEnabled(false);
                ui->editStockBinZMaxFallPosSA1_3->setEnabled(false);
            }
            else if(index == 3)
            {
                ui->editMachinInsideHeightSA1_4->setEnabled(false);
                ui->editMachineZMaxFallPosSA1_4->setEnabled(false);
                ui->editMaxWaitPosSA1_4->setEnabled(false);
                ui->editMaxStartPosSA1_4->setEnabled(false);
                ui->editStockBinZMaxFallPosSA1_4->setEnabled(false);
            }
        }
    }
    else if (zAxis == 3) {
        if(servoPointSafeArea.at(index).axisSelect[2]->count()>1)
        {
            servoPointSafeArea.at(index).axisSelect[2]->setCurrentIndex(1);
            if(index == 0){
                ui->editMachinInsideHeightSA1->setEnabled(true);
                ui->editMachineZMaxFallPosSA1->setEnabled(true);
                ui->editMaxWaitPosSA1->setEnabled(true);
                ui->editMaxStartPosSA1->setEnabled(true);
                ui->editStockBinZMaxFallPosSA1->setEnabled(true);
            }else if(index == 1)
            {
                ui->editMachinInsideHeightSA1_2->setEnabled(true);
                ui->editMachineZMaxFallPosSA1_2->setEnabled(true);
                ui->editMaxWaitPosSA1_2->setEnabled(true);
                ui->editMaxStartPosSA1_2->setEnabled(true);
                ui->editStockBinZMaxFallPosSA1_2->setEnabled(true);
            }
            else if(index == 2)
            {
                ui->editMachinInsideHeightSA1_3->setEnabled(true);
                ui->editMachineZMaxFallPosSA1_3->setEnabled(true);
                ui->editMaxWaitPosSA1_3->setEnabled(true);
                ui->editMaxStartPosSA1_3->setEnabled(true);
                ui->editStockBinZMaxFallPosSA1_3->setEnabled(true);
            }
            else if(index == 3)
            {
                ui->editMachinInsideHeightSA1_4->setEnabled(true);
                ui->editMachineZMaxFallPosSA1_4->setEnabled(true);
                ui->editMaxWaitPosSA1_4->setEnabled(true);
                ui->editMaxStartPosSA1_4->setEnabled(true);
                ui->editStockBinZMaxFallPosSA1_4->setEnabled(true);
            }
        }
    }
    else if (zAxis == 6) {
        if(servoPointSafeArea.at(index).axisSelect[2]->count()>2)
        {
            servoPointSafeArea.at(index).axisSelect[2]->setCurrentIndex(2);
            if(index == 0){
                ui->editMachinInsideHeightSA1->setEnabled(true);
                ui->editMachineZMaxFallPosSA1->setEnabled(true);
                ui->editMaxWaitPosSA1->setEnabled(true);
                ui->editMaxStartPosSA1->setEnabled(true);
                ui->editStockBinZMaxFallPosSA1->setEnabled(true);
            }else if(index == 1)
            {
                ui->editMachinInsideHeightSA1_2->setEnabled(true);
                ui->editMachineZMaxFallPosSA1_2->setEnabled(true);
                ui->editMaxWaitPosSA1_2->setEnabled(true);
                ui->editMaxStartPosSA1_2->setEnabled(true);
                ui->editStockBinZMaxFallPosSA1_2->setEnabled(true);
            }
            else if(index == 2)
            {
                ui->editMachinInsideHeightSA1_3->setEnabled(true);
                ui->editMachineZMaxFallPosSA1_3->setEnabled(true);
                ui->editMaxWaitPosSA1_3->setEnabled(true);
                ui->editMaxStartPosSA1_3->setEnabled(true);
                ui->editStockBinZMaxFallPosSA1_3->setEnabled(true);
            }
            else if(index == 3)
            {
                ui->editMachinInsideHeightSA1_4->setEnabled(true);
                ui->editMachineZMaxFallPosSA1_4->setEnabled(true);
                ui->editMaxWaitPosSA1_4->setEnabled(true);
                ui->editMaxStartPosSA1_4->setEnabled(true);
                ui->editStockBinZMaxFallPosSA1_4->setEnabled(true);
            }
        }
    }

    // 第三组用不上 默认传为0
    for (int i = 0; i < 2; i++)
    {
        servoPointSafeArea.at(index).machineA1Pos[i]->setText(
                    QString::number(m_SaveArea[index].machineA1Pos[i] / 100.0, 'f', 2));
        servoPointSafeArea.at(index).machineA2Pos[i]->setText(
                    QString::number(m_SaveArea[index].machineA2Pos[i] / 100.0, 'f', 2));
        servoPointSafeArea.at(index).starckB1Pos[i]->setText(
                    QString::number(m_SaveArea[index].starckB1Pos[i] / 100.0, 'f', 2));
        servoPointSafeArea.at(index).starckB2Pos[i]->setText(
                    QString::number(m_SaveArea[index].starckB2Pos[i] / 100.0, 'f', 2));
    }
    servoPointSafeArea.at(index).machineA1Pos[SAVE_Z_AXIS]->setText(
                QString::number(m_SaveArea[index].machineA1Pos[SAVE_Z_AXIS] / 100.0, 'f', 2));
    servoPointSafeArea.at(index).machineA2Pos[SAVE_Z_AXIS]->setText(
                QString::number(m_SaveArea[index].machineA2Pos[SAVE_Z_AXIS] / 100.0, 'f', 2));

    servoPointSafeArea.at(index).machine_Z_WaitMaxPos->setText(
                QString::number(m_SaveArea[index].machine_Z_WaitMaxPos / 100.0, 'f', 2));
    servoPointSafeArea.at(index).machine_Z_FallMaxPos->setText(
                QString::number(m_SaveArea[index].machine_Z_FallMaxPos / 100.0, 'f', 2));
    servoPointSafeArea.at(index).machine_Z_InsideHigh->setText(
                QString::number(m_SaveArea[index].machine_Z_InsideHigh / 100.0, 'f', 2));
    servoPointSafeArea.at(index).stack_Z_StartMaxPos->setText(
                QString::number(m_SaveArea[index].stack_Z_StartMaxPos / 100.0, 'f', 2));
    servoPointSafeArea.at(index).stack_Z_FallMaxPos->setText(
                QString::number(m_SaveArea[index].stack_Z_FallMaxPos / 100.0, 'f', 2));
}
//更新位置限定界面参数
void Setting::updatehorizontalPosList()
{
    toleranceList[0]->setText(QString::number(m_DegreeParS.tolerance / 100.0, 'f', 2));
    toleranceList[1]->setText(QString::number(m_DegreeParS.originDegree / 100.0, 'f', 2));
    toleranceList[2]->setText(QString::number(m_DegreeParS.actionFinDegree / 100.0, 'f', 2));

    horizontalPosList[0]->setText(QString::number(m_DegreeParS.saveRampageHige_Z1 / 100.0, 'f', 2));
    horizontalPosList[1]->setText(QString::number(m_DegreeParS.saveRampageHige_Z2 / 100.0, 'f', 2));
    horizontalPosList[2]->setText(QString::number(m_DegreeParS.saveRotatePos_Y1 / 100.0, 'f', 2));
    horizontalPosList[3]->setText(QString::number(m_DegreeParS.saveRotatePos_Y2 / 100.0, 'f', 2));
}
void Setting::updateRegisterCodeDisplay()
{
    QLineEdit* edits[8] = {
        ui->editCode, ui->editCode_2, ui->editCode_3,ui->editCode_4,
        ui->editCode_5, ui->editCode_6, ui->editCode_7, ui->editCode_8
    };


    for (int i = 0; i < 8; i++) {
        // Create a string of 4 characters for the current group (a group is 4 characters)
        QString group = "";

        // We want to display the last 4 characters in each QLineEdit
        for (int j = 0; j < 4; j++) {
            int index = i * 4 + j;
            if (index < registerCode.size()) {
                group += registerCode.at(index);
            } else {
                group += " ";  // Fill in empty spaces if we haven't reached 4 characters for this group
            }
        }

        // Assign the group to the corresponding QLineEdit
        edits[i]->setText(group);
    }
}

void Setting::handleSavePasswd(uint* passwd, const QList<NumberEdit*>& edits, const int suffix)
{
    if (edits[1]->text().isEmpty() || edits[2]->text().isEmpty()) {
        MainWindow::pMainWindow->showErrorTip(tr("密码不能为空！"), TipMode::ONLY_OK);
        return;
    }

    uint oldPasswd = edits[0]->text().toUInt();
    uint newPasswd = edits[1]->text().toUInt();
    uint confirmPasswd = edits[2]->text().toUInt();

    if (*passwd != oldPasswd) {
        MainWindow::pMainWindow->showErrorTip(tr("旧密码错误！"), TipMode::ONLY_OK);
        return;
    }

    if (newPasswd != confirmPasswd) {
        MainWindow::pMainWindow->showErrorTip(tr("两次输入密码不一致！"), TipMode::ONLY_OK);
        return;
    }

    if (*passwd != newPasswd)
    {
        *passwd = newPasswd;

        // save the passwd to configuration file
        ::savePasswdToConfig(suffix, *passwd);
    }

    MainWindow::pMainWindow->showErrorTip(tr("密码修改成功！"), TipMode::ONLY_OK);
    for (auto edit : edits) {
        edit->clear();
    }
}

void Setting::updateTabVisibility()
{
    static const std::map<int, QTabWidget*> tabWidgetMap = {
        {1, ui->tabWidgetSig},
        {2, ui->tabWgtSafe  },
        {3, ui->tabWidgetProduct},
        {4, ui->tabWidgetSystem},
        {5, ui->tabWgtServoPara},
        {6, ui->tabWgtServoSafePoint},
        {7, ui->tabWidgetMachinePara},
        {8, ui->tabWidgetStack}
    };
    static const std::map<int, QWidget*> tabContentMap = {
        {11, ui->tabOutType},
        {12, ui->tabSetInterlock},
        {13, ui->tabPortCustomize},
        {14, ui->tabNameCustomize},
        {15, ui->tabReserveAssociation},
        {16, ui->tabReserveOutType},
        {17, ui->tabKeyAndSig},
        {18, ui->tabAdvance},

        {21, ui->tabMachineToolSafety},
        {22, ui->tabBinSafety},
        {23, ui->tabClawSafety},
        {24, ui->tabOnlineSafe},

        {31, ui->tabProduct},
        {32, ui->tabAdvanceProduct},
        {33, ui->tabIOTProduct},

        {41, ui->tabLangurageSet},
        {42, ui->tabUserSet},
        {43, ui->tabUpdateAndBackup},
        {44, ui->tabNotepad},
        {45, ui->tabPasswdSet},
        {46, ui->tabIot},
        {47, ui->tabSignUp},

        {51, ui->tabServo},
        {52, ui->tabAxisPara},
        {53, ui->tabAxisSpeed},

        {61, ui->tabSafeArea1},
        {62, ui->tabSafeArea2},
        {63, ui->tabSafeArea3},
        {64, ui->tabSafeArea4},
        {65, ui->tabPosLimit},

        {71, ui->tabLimitPos},
        {72, ui->tabMachineStruct},
        {73, ui->tabOrigin},
        {74, ui->tabCommunication},

        {81, ui->tabStack1},
        {82, ui->tabStack2},
        {83, ui->tabStack3},
        {84, ui->tabStack4},
        {85, ui->tabStack5},
        {86, ui->tabStack6},
        {87, ui->tabStack7},
        {88, ui->tabStack8},
        {89, ui->tabStackSet}
    };

    const std::vector<QPushButton*> settingButtons = {
        ui->btnSigSet, ui->btnSafetySet, ui->btnProductSet, ui->btnSystemSet,
        ui->btnServoSpeed, ui->btnServoSafePoint, ui->btnMachinePara, ui->btnStackSet
    };

//    tabNameMap need to be initialized
    // tab
#if 0
    for (const auto& pair : menuStateMap) {
        int id = pair.first;
        MenuState tabState = pair.second;

        // 分解 ID：第一个数字是 QTabWidget 的组，第二个数字是 Tab 的索引
        int tabWidgetGroup = id / 10; // 第一位表示 TabWidget 组
        int tabIndex = id % 10 - 1;   // 第二位表示 Tab 索引（从 0 开始）

//        bool shouldShow = tabState == MenuState::Invisible ? false : (currentLoginState >= tabState);
        bool shouldShow = (tabState != MenuState::Invisible) && (currentLoginState >= tabState);

        // 查找对应的 QTabWidget 和内容tab QWidget
        if (tabWidgetMap.count(tabWidgetGroup) && tabContentMap.count(id)) {
            QTabWidget* tabWidget = tabWidgetMap.at(tabWidgetGroup);
            QWidget* contentWidget = tabContentMap.at(id);

            int currentTabIndex = tabWidget->indexOf(contentWidget);
            QString tabName = tabNameMap.at(id);

            if (shouldShow) {
                // 如果需要显示，检查是否已存在
                if (currentTabIndex == -1) {
                    tabWidget->insertTab(tabIndex, contentWidget, tabName);
                }
            }
            else {
                if (currentTabIndex != -1) {
                    tabWidget->removeTab(currentTabIndex);
                }
            }
        }
    }
#else

    std::map<int, std::vector<int>> groupTabs;

    for (const auto& pair : menuStateMap) {
        int id = pair.first;
        MenuState tabState = pair.second;

        int tabWidgetGroupID = id / 10;  // 第一位表示 TabWidget 组
        if (tabWidgetGroupID > 0 && tabWidgetGroupID < 9)
        {
            bool shouldShow = ((tabState != MenuState::Invisible) && (currentLoginState >= tabState));

            if (shouldShow && tabContentMap.count(id)) {
                groupTabs[tabWidgetGroupID].push_back(id);
            }
        }
    }

    // clear all tab for each tabWidget
    for (size_t i = 1; i < tabWidgetMap.size() + 1; i++)
    {
        auto tabWidget = tabWidgetMap.at(i);
        tabWidget->clear();
        tabWidget->clearFocus();
    }

    for (auto& group : groupTabs) {
        int groupId = group.first;
        std::vector<int> idsToShow = group.second;

        // 排序ID
        std::sort(idsToShow.begin(), idsToShow.end());

        if (tabWidgetMap.count(groupId)) {
            QTabWidget* tabWidget = tabWidgetMap.at(groupId);

            // 按顺序插入Tab
            for (size_t i = 0; i < idsToShow.size(); ++i) {
                int id = idsToShow[i];
                if (tabContentMap.count(id)) {
                    QWidget* contentWidget = tabContentMap.at(id);
                    QString tabName = tabNameMap.at(id);

                    // 使用排序后的索引作为 tabIndex
                    int tabIndex = static_cast<int>(i);

                    tabWidget->insertTab(tabIndex, contentWidget, tabName);
                }
            }
        }
    }

    ui->tabWidgetSystem->removeTab(ui->tabWidgetSystem->indexOf(ui->tabSystemInfo));
    ui->tabWidgetSystem->addTab(ui->tabSystemInfo, tr("系统信息"));

#endif
    ui->tabWidgetSystem->removeTab(ui->tabWidgetSystem->indexOf(ui->tabMenuAuthority));
    if (currentLoginState){
        ui->tabWidgetSystem->addTab(ui->tabMenuAuthority, tr("菜单权限"));
    }

    for (size_t i = 0; i < settingButtons.size(); i++)
    {
        auto btn = settingButtons[i];
//        auto id = groupIDs[i];
        int id = i + 1;
        const auto& state = menuStateMap.at(id);

        bool hasOneTab = tabWidgetMap.at(id)->count();

        bool enable = hasOneTab && (currentLoginState >= state);
        btn->setEnabled(enable);
    }
}

void Setting::switchLangurage()
{
    const QString BASE_PATH = "/opt/MachineTool/resources/lang/";
    const QStringList LANGUAGE_FILES = {
        "Translation_Ch.qm", // Chinese
        "Translation_En.qm", // English
        "Translation_Kr.qm"  // Korean
    };

    if (m_SystemSet.lan < 0 || m_SystemSet.lan >= LANGUAGE_FILES.size()) {
        return; // Return early if language index is out of bounds
    }

    QString languageFilePath = BASE_PATH + LANGUAGE_FILES.at(m_SystemSet.lan);

    if (!translator.load(languageFilePath))
    {
        qDebug() << "fail to load langurage file:" << languageFilePath;
    }


    qApp->installTranslator(&translator);

    this->retranslate();

}

void Setting::retranslate()
{
    ui->retranslateUi(this);

    // 需要重新同步参数
    this->syncParaToUI();

    ui->tableWgtNote->setHorizontalHeaderLabels({ tr("标题") , tr("修改时间")});
    ui->treeWgt->setHeaderLabels({tr("菜单"), tr("操作员"), tr("管理员"), tr("高级"), tr("不可见")});

    // update translation in tabNameMap
    for (auto& pair : tabNameMap)
    {
        const char* sourceText = tabSourceNameMap.at(pair.first).toStdString().c_str();
        pair.second = tr(sourceText);
    }


    for (int i = 0; i < ui->treeWgt->topLevelItemCount(); ++i) {
        QTreeWidgetItem* topLevelItem = ui->treeWgt->topLevelItem(i);
        int groupId = i + 1;
        auto it = tabNameMap.find(groupId);
        if (it != tabNameMap.end())
        {
            topLevelItem->setText(0, it->second);
        }

        // 遍历每个顶级项的所有子项
        for (int j = 0; j < topLevelItem->childCount(); ++j) {
            QTreeWidgetItem* childItem = topLevelItem->child(j);

            int childId = groupId * 10 + (j + 1);
            auto childIt = tabNameMap.find(childId);
            if (childIt != tabNameMap.end())
            {
                childItem->setText(0, childIt->second);
            }
        }
    }

    // retranslate customize name
    ui->tableWgtNameDef->setHorizontalHeaderLabels({tr("默认名称"), tr("修改名称")});
    ::reloadTranslateNameDefine();
    this->refreshNameDefine();
    // retranslate customize port
    ::reloadTranslatePortDefInfo();
    this->RefreshPortDefine();
}

void Setting::onMenuStateChanged(MenuState newState)
{
    MenuItem* item = qobject_cast<MenuItem*>(sender());
    if (!item)
        return;

    qDebug() << "ID:" << item->id << "," << item->name << " new state:" << newState;

    if (menuStateMap.count(item->id))
    {
        menuStateMap[item->id] = newState;
        QSettings settings(menuStateConfigPath, QSettings::IniFormat);
        settings.setIniCodec("utf-8");
        const QString prefix = QString::number(item->id);
        settings.setValue(prefix + "/state", static_cast<int>(newState));
        settings.setValue(prefix + "/name", item->name);
    }

//    updateTabVisibility();
//    handleLoginModeChanged(LoginDialog::getLoginMode());
}

void Setting::updateAppFont()
{
    const std::vector<QString> fontType = {
        "WenQuanYi Zen Hei",            // 默认字体
        "SimSun",                      // 宋体
        "KaiTi",                        // 楷体
        "DejaVu Sans"
    };
    const int defaultPixelSize = 20;

    QString selectedFont = fontType[ui->coboxFonts->currentIndex()];
    int selectedPixelSize = (ui->coboxFontSize->currentIndex() == 0)
                            ? defaultPixelSize
                            : ui->coboxFontSize->currentText().toInt();

    QFont font(selectedFont);
    font.setPixelSize(selectedPixelSize);

    qApp->setFont(font);
    for (auto widget : MainWindow::pMainWindow->findChildren<QWidget*>())
    {
        widget->setFont(font);
//        widget->update();
    }
}

void Setting::pageSwitchInit()
{
    /******************************************************************************/
    for (QPushButton* btn : ui->pageSettingHome->findChildren<QPushButton*>()) {
        QIcon originalIcon = btn->icon();
        QSize originalSize = btn->iconSize();
        QSize smallerSize(originalSize.width() * 0.8, originalSize.height() * 0.8);

        QObject::connect(btn, &QPushButton::pressed, [btn, originalIcon, smallerSize]() {
            QPixmap pixmap = originalIcon.pixmap(smallerSize);
            btn->setIcon(QIcon(pixmap));
            btn->setIconSize(smallerSize);
        });

        QObject::connect(btn, &QPushButton::released, [btn, originalIcon, originalSize]() {
            QPixmap pixmap = originalIcon.pixmap(originalSize);
            btn->setIcon(QIcon(pixmap));
            btn->setIconSize(originalSize);
        });
    }
    /******************************************************************************/
    connect(ui->btnSigSet, &QPushButton::clicked, this, [=](){
        // jump to signal set page
        ui->stackedWidget->setCurrentWidget(ui->pageSignal);
    });
    connect(ui->btnSafetySet, &QPushButton::clicked, this, [=](){
        ui->stackedWidget->setCurrentWidget(ui->pageSafety);
    });
    connect(ui->btnProductSet, &QPushButton::clicked, this, [=](){
        ui->stackedWidget->setCurrentWidget(ui->pageProduct);
    });
    connect(ui->btnSystemSet, &QPushButton::clicked, this, [=](){
       ui->stackedWidget->setCurrentWidget(ui->pageSystem);
       emit ui->tabWidgetSystem->currentChanged(ui->tabWidgetSystem->currentIndex());
    });
    connect(ui->btnServoSpeed, &QPushButton::clicked, this, [=](){

        ui->stackedWidget->setCurrentWidget(ui->pageServoPara);
    });
    connect(ui->btnServoSafePoint, &QPushButton::clicked, this, [=](){
        ui->stackedWidget->setCurrentWidget(ui->pageServoSafePoint);
    });
    connect(ui->btnMachinePara, &QPushButton::clicked, this, [=](){
        ui->stackedWidget->setCurrentWidget(ui->pageMachinePara);
    });
    connect(ui->btnStackSet, &QPushButton::clicked, this, [=](){
        ui->stackedWidget->setCurrentWidget(ui->pageStack);
        ui->tabWidgetStack->setCurrentIndex(0);
//        stack[0]->updateGroupIndex(0);
//        stack[0]->syncParaToUI();
    });
     /******************************************************************************/
    connect(ui->btnLastAdvance, &QPushButton::clicked, this, [=](){
        ui->stkWgtAdvance->setCurrentIndex((ui->stkWgtAdvance->currentIndex() - 1 + ui->stkWgtAdvance->count())
                                                 % ui->stkWgtAdvance->count());
        ui->labPageNumAdvance->setText(QString("%1/2").arg(ui->stkWgtAdvance->currentIndex() + 1));
    });
    connect(ui->btnNextAdvance, &QPushButton::clicked, this, [=](){
        ui->stkWgtAdvance->setCurrentIndex((ui->stkWgtAdvance->currentIndex() + 1 + ui->stkWgtAdvance->count())
                                                 % ui->stkWgtAdvance->count());
        ui->labPageNumAdvance->setText(QString("%1/2").arg(ui->stkWgtAdvance->currentIndex() + 1));
    });
//    connect(ui->btnLastPageServoPara, &QPushButton::clicked, this, [=](){
//        ui->stkWidgetServoPara->setCurrentIndex((ui->stkWidgetServoPara->currentIndex() - 1 + ui->stkWidgetServoPara->count())
//                                                 % ui->stkWidgetServoPara->count());
//        ui->labelPageNum->setText(QString("%1/2").arg(ui->stkWidgetServoPara->currentIndex() + 1));
////        ui->grboxAxisSelect->show();
//    });
//    connect(ui->btnNextPageServoPara, &QPushButton::clicked, this, [=](){
//        ui->stkWidgetServoPara->setCurrentIndex((ui->stkWidgetServoPara->currentIndex() + 1 + ui->stkWidgetServoPara->count())
//                                                 % ui->stkWidgetServoPara->count());
//        ui->labelPageNum->setText(QString("%1/2").arg(ui->stkWidgetServoPara->currentIndex() + 1));
////        ui->grboxAxisSelect->show();
//    });


    /***************************************信号设置************************************************/
    // 互锁设置逻辑
    auto setCheckBoxesState = [&](const QVector<QCheckBox*>& checkBoxes, bool enabled){
        for (auto checkBox : checkBoxes)
        {
            checkBox->setEnabled(enabled);
            if (!enabled)
            {
                checkBox->setChecked(false);
            }
        }
    };
    auto setupCheckBoxGroup = [&](QCheckBox* masterCheckBox, const QVector<QCheckBox*>& checkBoxes){
        connect(masterCheckBox, QOverload<int>::of(&QCheckBox::stateChanged), [=](int state){
            setCheckBoxesState(checkBoxes, state == Qt::Checked);
        });
    };

    // 6+6组 依次为： 原料1夹紧 成品1夹紧 卡爪1正转 自动门1开 卡盘1夹紧 预留1
    QVector<QPair<QCheckBox*, QVector<QCheckBox*>>> checkBoxGroups = {
        {ui->chbox0InterlockGroup0, {ui->chbox1InterlockGroup0, ui->chbox2InterlockGroup0, ui->chbox3InterlockGroup0}},
        {ui->chbox0InterlockGroup1, {ui->chbox1InterlockGroup1, ui->chbox2InterlockGroup1, ui->chbox3InterlockGroup1}},
        {ui->chbox0InterlockGroup2, {ui->chbox1InterlockGroup2, ui->chbox2InterlockGroup2, ui->chbox3InterlockGroup2}},
        {ui->chbox0InterlockGroup3, {ui->chbox1InterlockGroup3, ui->chbox2InterlockGroup3, ui->chbox3InterlockGroup3}},
        {ui->chbox0InterlockGroup4, {ui->chbox1InterlockGroup4, ui->chbox2InterlockGroup4, ui->chbox3InterlockGroup4}},
        {ui->chbox0InterlockGroup5, {ui->chbox1InterlockGroup5, ui->chbox2InterlockGroup5, ui->chbox3InterlockGroup5}},
        {ui->chbox0InterlockGroup6, {ui->chbox1InterlockGroup6, ui->chbox2InterlockGroup6, ui->chbox3InterlockGroup6}},
        {ui->chbox0InterlockGroup7, {ui->chbox1InterlockGroup7, ui->chbox2InterlockGroup7, ui->chbox3InterlockGroup7}},
        {ui->chbox0InterlockGroup8, {ui->chbox1InterlockGroup8, ui->chbox2InterlockGroup8, ui->chbox3InterlockGroup8}},
        {ui->chbox0InterlockGroup9, {ui->chbox1InterlockGroup9, ui->chbox2InterlockGroup9, ui->chbox3InterlockGroup9}},
        {ui->chbox0InterlockGroup10, {ui->chbox1InterlockGroup10, ui->chbox2InterlockGroup10, ui->chbox3InterlockGroup10}},
        {ui->chbox0InterlockGroup11, {ui->chbox1InterlockGroup11, ui->chbox2InterlockGroup11, ui->chbox3InterlockGroup11}}
    };

    for (const auto& group : checkBoxGroups) {
        setupCheckBoxGroup(group.first, group.second);

        QCheckBox* chbox = group.first;
        chbox->stateChanged(chbox->checkState());
    }

    connect(ui->btnLastInterLock, &QPushButton::clicked, this, [=](){
        ui->stkWgtInterLock->setCurrentIndex((ui->stkWgtInterLock->currentIndex() - 1 + ui->stkWgtInterLock->count())
                                                 % ui->stkWgtInterLock->count());
        ui->labPageNumInterLock->setText(QString("%1/2").arg(ui->stkWgtInterLock->currentIndex() + 1));
    });
    connect(ui->btnNextInterLock, &QPushButton::clicked, this, [=](){
        ui->stkWgtInterLock->setCurrentIndex((ui->stkWgtInterLock->currentIndex() + 1 + ui->stkWgtInterLock->count())
                                                 % ui->stkWgtInterLock->count());
        ui->labPageNumInterLock->setText(QString("%1/2").arg(ui->stkWgtInterLock->currentIndex() + 1));
    });

    /***************************************Safe setting part**********************************************************/

    auto setupOnlineEnable = [&](QComboBox* comboBox, const QList<QWidget*>& widgets) {
        connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [widgets](int state) {
            bool enabled = (state == 1);
            for (QWidget* widget : widgets)
            {
                widget->setEnabled(enabled);
            }
        });
    };
    setupOnlineEnable(ui->coboxOnlineAxis, {ui->labAreaInput, ui->editAreaInput, ui->labAreaOutput, ui->editAreaOutput,
                      ui->labRequestInput, ui->editRequestInput, ui->labRequestOutput, ui->editRequestOutput,
                      ui->labA1A2Pos, ui->editA1A2MinPos, ui->editA1A2MaxPos, ui->labA1A2PosUnit});
    setupOnlineEnable(ui->coboxOnlineAxis_2, {ui->labAreaInput_2, ui->editAreaInput_2, ui->labAreaOutput_2, ui->editAreaOutput_2,
                          ui->labRequestInput_2, ui->editRequestInput_2, ui->labRequestOutput_2, ui->editRequestOutput_2,
                          ui->labA1A2Pos_2, ui->editA1A2MinPos_2, ui->editA1A2MaxPos_2, ui->labA1A2PosUnit_2});
    setupOnlineEnable(ui->coboxOnlineAxis_3, {ui->labAreaInput_3, ui->editAreaInput_3, ui->labAreaOutput_3, ui->editAreaOutput_3,
                          ui->labRequestInput_3, ui->editRequestInput_3, ui->labRequestOutput_3, ui->editRequestOutput_3,
                          ui->labA1A2Pos_3, ui->editA1A2MinPos_3, ui->editA1A2MaxPos_3, ui->labA1A2PosUnit_3});

    setupOnlineEnable(ui->coboxOnlineAxis_4, {ui->labAreaInput_4, ui->editAreaInput_4, ui->labAreaOutput_4, ui->editAreaOutput_4,
                          ui->labRequestInput_4, ui->editRequestInput_4, ui->labRequestOutput_4, ui->editRequestOutput_4,
                          ui->labA1A2Pos_4, ui->editA1A2MinPos_4, ui->editA1A2MaxPos_4, ui->labA1A2PosUnit_4});

    /***************************************Product setting part**********************************************************/

    connect(ui->btnEditProcuctionBatch, &QPushButton::clicked, this, [=](){
        ui->editProcuctionBatch->setEnabled(true);
    });
    connect(ui->btnSaveProcuctionBatch, &QPushButton::clicked, this, [=](){
        ui->editProcuctionBatch->setEnabled(false);
        // to do ...
        // save parameter

    });

    /******************************Servo Safe Point************************************************************/
    // 是否使用安全区
    auto connectComboBoxToGroupBoxes = [&](QComboBox* comboBox, QGroupBox* grbox1, QGroupBox* grbox2,
            QGroupBox* grbox3, QGroupBox* grbox4, QComboBox* comboBoxz)
    {

        connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index){
            bool enabled = false;
            grbox1->setEnabled(index);
            grbox2->setEnabled(index);

            if(comboBoxz->currentIndex()!=0)
            {
                enabled = true;
            }
            grbox3->setEnabled(index & enabled);
            grbox4->setEnabled(index & enabled);
        });
    };

    // Helper function to connect QComboBox index changes to QLabel and QLineEdit enabling
    auto connectAxisX = [&](QComboBox* comboBox, QLabel* labelX, QLabel* labelC, QLineEdit* edit1,
            QLineEdit* edit2, QLineEdit* edit3, QLineEdit* edit4, QLineEdit* editC1, QLineEdit* editC2)
    {
        connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index){

            QString text = comboBox->currentText();
            labelX->setText(text);
            labelC->setText(text == "无" || text == "None" ? text : tr("C1C2"));

            labelX->setEnabled(index);
            labelC->setEnabled(index);
            edit1->setEnabled(index);
            edit2->setEnabled(index);
            edit3->setEnabled(index);
            edit4->setEnabled(index);
            editC1->setEnabled(index);
            editC2->setEnabled(index);
        });
    };

    // Helper function to connect QComboBox index changes to QLabel and QLineEdit enabling
    auto connectAxisY = [&](QComboBox* comboBox, QLabel* labelY, QLineEdit* edit1, QLineEdit* edit2,
            QLineEdit* edit3, QLineEdit* edit4)
    {
        connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index){
            labelY->setEnabled(index);
            edit1->setEnabled(index);
            edit2->setEnabled(index);
            edit3->setEnabled(index);
            edit4->setEnabled(index);
            labelY->setText(comboBox->currentText());
        });
    };

    // Helper function to connect QComboBox index changes to QGroupBox enabling
    auto connectAxisZ = [&](QComboBox* comboBox, QGroupBox* groupBox1, QGroupBox* groupBox2,QLineEdit* edit1,
            QLineEdit* edit2, QLineEdit* edit3, QLineEdit* edit4, QLineEdit* edit5)
    {
        connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index){
            groupBox1->setEnabled(index);
            groupBox2->setEnabled(index);
            edit1->setEnabled(index);
            edit2->setEnabled(index);
            edit3->setEnabled(index);
            edit4->setEnabled(index);
            edit5->setEnabled(index);
        });
    };

    // 安全区1
    connectComboBoxToGroupBoxes(ui->coboxSafeArea1, ui->grboxAxisCombineSA1Arm2, ui->grboxXYParaSA1,
                                ui->grboxZAxisMachineParaSA1, ui->grboxZAxisStockBinParaSA1,
                                ui->coboxAxisZSA1);
    connectAxisX(ui->coboxAxisXSA1, ui->labAxisXSelectSA1, ui->labAxisCSelectSA1,
                 ui->editMachineA1AxisXSA1, ui->editMachineA2AxisXSA1,
                 ui->editStockBinB1AxisXSA1, ui->editStockBinB2AxisXSA1,
                 ui->editMachineA1AxisCSA1, ui->editMachineA2AxisCSA1);
    connectAxisY(ui->coboxAxisYSA1, ui->labAxisYSelectSA1,
                 ui->editMachineA1AxisYSA1, ui->editMachineA2AxisYSA1,
                 ui->editStockBinB1AxisYSA1, ui->editStockBinB2AxisYSA1);
    connectAxisZ(ui->coboxAxisZSA1, ui->grboxZAxisMachineParaSA1, ui->grboxZAxisStockBinParaSA1,
                 ui->editMachinInsideHeightSA1,ui->editMachineZMaxFallPosSA1,ui->editMaxWaitPosSA1,
                 ui->editMaxStartPosSA1,ui->editStockBinZMaxFallPosSA1);

    // 安全区2
    connectComboBoxToGroupBoxes(ui->coboxSafeArea1_2, ui->grboxAxisCombineSA1Arm2_2, ui->grboxXYParaSA1_2,
                                ui->grboxZAxisMachineParaSA1_2, ui->grboxZAxisStockBinParaSA1_2,
                                ui->coboxAxisZSA1_2);
    connectAxisX(ui->coboxAxisXSA1_2, ui->labAxisXSelectSA1_2, ui->labAxisCSelectSA1_2,
                 ui->editMachineA1AxisXSA1_2, ui->editMachineA2AxisXSA1_2,
                 ui->editStockBinB1AxisXSA1_2, ui->editStockBinB2AxisXSA1_2,
                 ui->editMachineA1AxisCSA1_2, ui->editMachineA2AxisCSA1_2);
    connectAxisY(ui->coboxAxisYSA1_2, ui->labAxisYSelectSA1_2,
                 ui->editMachineA1AxisYSA1_2, ui->editMachineA2AxisYSA1_2,
                 ui->editStockBinB1AxisYSA1_2, ui->editStockBinB2AxisYSA1_2);
    connectAxisZ(ui->coboxAxisZSA1_2, ui->grboxZAxisMachineParaSA1_2, ui->grboxZAxisStockBinParaSA1_2,
            ui->editMachinInsideHeightSA1_2,ui->editMachineZMaxFallPosSA1_2,ui->editMaxWaitPosSA1_2,
            ui->editMaxStartPosSA1_2,ui->editStockBinZMaxFallPosSA1_2);

    // 安全区3
    connectComboBoxToGroupBoxes(ui->coboxSafeArea1_3, ui->grboxAxisCombineSA1Arm2_3, ui->grboxXYParaSA1_3,
                                ui->grboxZAxisMachineParaSA1_3, ui->grboxZAxisStockBinParaSA1_3,
                                ui->coboxAxisZSA1_3);
    connectAxisX(ui->coboxAxisXSA1_3, ui->labAxisXSelectSA1_3, ui->labAxisCSelectSA1_3,
                 ui->editMachineA1AxisXSA1_3, ui->editMachineA2AxisXSA1_3,
                 ui->editStockBinB1AxisXSA1_3, ui->editStockBinB2AxisXSA1_3,
                 ui->editMachineA1AxisCSA1_3, ui->editMachineA2AxisCSA1_3);
    connectAxisY(ui->coboxAxisYSA1_3, ui->labAxisYSelectSA1_3,
                 ui->editMachineA1AxisYSA1_3, ui->editMachineA2AxisYSA1_3,
                 ui->editStockBinB1AxisYSA1_3, ui->editStockBinB2AxisYSA1_3);
    connectAxisZ(ui->coboxAxisZSA1_3, ui->grboxZAxisMachineParaSA1_3, ui->grboxZAxisStockBinParaSA1_3,
                 ui->editMachinInsideHeightSA1_3,ui->editMachineZMaxFallPosSA1_3,ui->editMaxWaitPosSA1_3,
                 ui->editMaxStartPosSA1_3,ui->editStockBinZMaxFallPosSA1_3);

    // 安全区4
    connectComboBoxToGroupBoxes(ui->coboxSafeArea1_4, ui->grboxAxisCombineSA1Arm2_4, ui->grboxXYParaSA1_4,
                                ui->grboxZAxisMachineParaSA1_4, ui->grboxZAxisStockBinParaSA1_4,
                                ui->coboxAxisZSA1_4);
    connectAxisX(ui->coboxAxisXSA1_4, ui->labAxisXSelectSA1_4, ui->labAxisCSelectSA1_4,
                 ui->editMachineA1AxisXSA1_4, ui->editMachineA2AxisXSA1_4,
                 ui->editStockBinB1AxisXSA1_4, ui->editStockBinB2AxisXSA1_4,
                 ui->editMachineA1AxisCSA1_4, ui->editMachineA2AxisCSA1_4);
    connectAxisY(ui->coboxAxisYSA1_4, ui->labAxisYSelectSA1_4,
                 ui->editMachineA1AxisYSA1_4, ui->editMachineA2AxisYSA1_4,
                 ui->editStockBinB1AxisYSA1_4, ui->editStockBinB2AxisYSA1_4);
    connectAxisZ(ui->coboxAxisZSA1_4, ui->grboxZAxisMachineParaSA1_4, ui->grboxZAxisStockBinParaSA1_4,
                 ui->editMachinInsideHeightSA1_4,ui->editMachineZMaxFallPosSA1_4,ui->editMaxWaitPosSA1_4,
                 ui->editMaxStartPosSA1_4,ui->editStockBinZMaxFallPosSA1_4);

    auto setSafeAreaPic = [&](QComboBox* coboxAxisY, QLabel* labSafeAreaPic){
        connect(coboxAxisY, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index){
//            qDebug() << "coboxAxisY index:" << index;
            if (index == 0)
            {
                QPixmap pic(":/images/settingPageImages/safe_area_XZ.png");
                labSafeAreaPic->setPixmap(pic);

            }
            else {
                QPixmap pic(":/images/settingPageImages/safe_area_XYZ.png");
                labSafeAreaPic->setPixmap(pic);
            }
        });
        emit coboxAxisY->currentIndexChanged(coboxAxisY->currentIndex());
    };

    setSafeAreaPic(ui->coboxAxisYSA1, ui->labSafeArea1Pic);
    setSafeAreaPic(ui->coboxAxisYSA1_2, ui->labSafeArea2Pic);
    setSafeAreaPic(ui->coboxAxisYSA1_3, ui->labSafeArea3Pic);
    setSafeAreaPic(ui->coboxAxisYSA1_4, ui->labSafeArea4Pic);



    /******************************Stack Setting************************************************************/
    // 按组堆叠
    connect(ui->coboxGroupStack, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index){
        ui->coboxLayerStack->setEnabled(!index);
        ui->coboxRowStack->setEnabled(!index);
    });
    // 按行堆叠
    connect(ui->coboxRowStack, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index){
        ui->coboxLayerStack->setEnabled(!index);
        ui->coboxGroupStack->setEnabled(!index);
    });
    // 按层堆叠
    connect(ui->coboxLayerStack, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index){
        ui->coboxRowStack->setEnabled(!index);
        ui->coboxGroupStack->setEnabled(!index);
    });
    // 料仓类型
    connect(ui->coboxSiloType, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index){
        bool state = index == 1;

        ui->labRotateSiloFinHint->setVisible(index);
        ui->coboxRotateSiloFinHint->setVisible(index);
        ui->labRotateSiloPlaceNum->setVisible(index);
        ui->editRotateSiloPlaceNum->setVisible(index);

        ui->labStackWay->setVisible(!state);
        ui->coboxStackWay->setVisible(!state);
    });
}

//PXC_240826    通信
void Setting::setupCommunicationConnections()
{
    //信号设置
    //输出类型
    for (int i=0;i<outputTypeList.count();i++)
    {
        connect(outputTypeList[i], QOverload<int>::of(&QComboBox::activated),[=](int){
            outputTypeSlots();
        } );
    }

    //互锁设置
    for (int i=0;i<outportInterlockList.count();i++)
    {
        connect(outportInterlockList[i], QOverload<int>::of(&QCheckBox::stateChanged), [=](int){
            outportInterlockSlots();
        } );
    }

    //预留关联
    for (int i=0;i<outportRelevancyList.count();i++)
    {
        connect(outportRelevancyList[i], QOverload<int>::of(&QComboBox::activated), [=](int){
            outportRelevancySlots();
        } );
    }

    //预留关联出
    for (int i=0;i<outportReleteOutList.count();i++)
    {
        connect(outportReleteOutList[i], QOverload<int>::of(&QComboBox::activated), [=](int){
            outportReleteOutSlots();
        } );
    }

    //高级
    for (int i=0;i<seniorFuncList.count();i++)
    {
        connect(seniorFuncList[i], QOverload<int>::of(&QComboBox::activated), [=](int){
            seniorFuncSlots();
        } );
    }

    //按键
    for (int i=0;i<OPR_KEY_NUM;i++) {
        connect(keyEdits[i],&KeyEdit::returnPressed,[=](){
            auto text = keyEdits[i]->text();

            saveKeyAndLEDFuncDefine();
            writeKeySetStrToConfig(i, text);
            keyFunDesription[i]=text;
        });
    }

    //LED
    for (int i=0;i<OPR_LED_NUM;i++) {
        connect(sigEdits[i],&SigLEDEdit::returnPressed,[=](){
            auto text = sigEdits[i]->text();
            saveKeyAndLEDFuncDefine();
            writeSigSetStrToConfig(i, text);
            sigSetDesription[i]=text;
        });
    }


    //安全设置
    //机床安全
    for (int i=0;i<machineSafeList.count();i++)
    {
        connect(machineSafeList[i], QOverload<int>::of(&QComboBox::activated), [=](int){
            saveMachineSafePara();
        } );
    }

    //料仓安全
    for (int i=0;i<stackBinSafeList.count();i++)
    {
        connect(stackBinSafeList[i], QOverload<int>::of(&QComboBox::activated), [=](int){
            saveStackBinSafePara();
        } );
    }

    //卡爪安全
    const QVector<QPushButton*> clawSafeList={ui->btnClawSafe1,ui->btnClawSafe2,ui->btnClawSafe3,ui->btnClawSafe4};
    for (int i=0;i<clawSafeList.count();i++)
    {
        connect(clawSafeList[i], &QPushButton::clicked, [=](){
            saveClawSafePara(i);
        });
    }

    //联机安全
    const QVector<QPushButton*> onlineSafeList= {
        ui->btnOnlineSafe1,ui->btnOnlineSafe2,ui->btnOnlineSafe3,ui->btnOnlineSafe4
    };
    for (int i=0;i<onlineSafeList.count();i++)
    {
        connect(onlineSafeList[i], &QPushButton::clicked, [=](){
            saveOnlineSafePara(i);

            std::vector<QString> tempStrs = {
                onlineSafeWidgets.at(i).areaInNum->text(),
                onlineSafeWidgets.at(i).areaOutNum->text(),
                onlineSafeWidgets.at(i).requestInNum->text(),
                onlineSafeWidgets.at(i).requestOutNum->text()
            };
            writeOnlineSafeInOutDescription(i, tempStrs);
        });
    }

    //产品设置
    for (int i=0;i<7;i++) {
        connect(productSet[i],&NumberEdit::returnPressed,[=](){
            saveProductSetPara();
        });
    }

    //高级
    for (int i=0;i<4;i++) {
        connect(productSeniorEdit[i],&NumberEdit::returnPressed,[=](){
            saveProductSetSeniorPara();
        });
    }
    connect(ui->coboxProductMemory,QOverload<int>::of(&QComboBox::activated),[=](int){
        saveProductSetSeniorPara();
    });
    //物联网
    for (int i=0;i<3;i++) {
        connect(productIot[i],QOverload<int>::of(&QComboBox::activated),[=](int){
            saveProductSetInternetPara();
        });
    }

    //伺服参数
    //结构
//    for (int i=0;i<8;i++) {
//        connect(servoPara[i],QOverload<int>::of(&QComboBox::activated),[=](int index){
//            saveServoPara();
//        });
//    }
    for(int i=0;i<AXIS_TOTAL_NUM+1;i++)
    {
        accAccList[i]->setDecimalPlaces(0);
        decDecList[i]->setDecimalPlaces(0);
        connect(accTimeList[i],&NumberEdit::returnPressed,[=](){
            saveAccDecTimeSpeed(i);
        });
        connect(decTimeList[i],&NumberEdit::returnPressed,[=](){
            saveAccDecTimeSpeed(i);
        });
        connect(accAccList[i],&NumberEdit::returnPressed,[=](){
            saveAccDecTimeSpeed(i);
        });
        connect(decDecList[i],&NumberEdit::returnPressed,[=](){
            saveAccDecTimeSpeed(i);
        });
    }
    //安全区（四组）
    connect(ui->btnSaveServoSafePoint,&QPushButton::clicked,[=](){
        int index=ui->tabWgtServoSafePoint->currentIndex();
        saveSafeAreaPara(index);
    });

    //伺服安全点
    //容差
//    for (int i=0;i<3;i++) {
//        connect(toleranceList[i],&NumberEdit::returnPressed,[=](){
//            saveTolerancePara();
//        });
//    }

//    //横行位置限定
//    for (int i=0;i<4;i++) {
//        connect(horizontalPosList[i],&NumberEdit::returnPressed,[=](){
//            saveDegreePara();
//        });
//    }

    //机器参数
    const QVector<QPushButton*> machineParaList={
        ui->btnSaveMachinePara_1,ui->btnSaveMachinePara_2,ui->btnSaveMachinePara_3,
        ui->btnSendMachineCommunicate,ui->btnSaveServoPara,ui->btnSaveAxisPara};

    for (int i=0;i<machineParaList.count();i++)
    {
        connect(machineParaList[i], &QPushButton::clicked, [=](){
            saveMachineAllPara(i);
        });
    }

    //堆叠参数
    for (int i=0;i<8;i++) {
        connect(stackCoboxs[i],QOverload<int>::of(&QComboBox::activated),[=](int){
            saveStackFunc();
        });
    }
    connect(ui->editRotateSiloPlaceNum,&NumberEdit::returnPressed,[=](){
        saveStackFunc();
    });
//    QSignalMapper *outputTypeMapper = new QSignalMapper(this);
//    for (int i=0;i<outputTypeList.count();i++)
//    {
//        connect(outputTypeList[i], QOverload<int>::of(&QComboBox::activated), outputTypeMapper,QOverload<>::of(&QSignalMapper::map));
//        outputTypeMapper->setMapping(outputTypeList[i],i);
//    }
//    connect(outputTypeMapper,QOverload<int>::of(&QSignalMapper::mapped),this,&Setting::outputTypeSlots);



}
//设置所有界面带保存按钮界面参数检测是否需要保存处理
void Setting::ParNeedSaveCheckConnectHandle()
{
    /****信号设置子界面****/
    connect(ui->tabWidgetSig,&QTabWidget::currentChanged,this,[=](){
       SwitchPageParSaveHandle(ParSaveIndex::SavePort);
       SwitchPageParSaveHandle(ParSaveIndex::SaveNameDef);
    });
    /****安全设置子界面****/
    connect(ui->tabWgtClawSafety,&QTabWidget::currentChanged,this,[=](){
        SwitchPageParSaveHandle(ParSaveIndex::ClawSafe);
    });
    connect(ui->tabWgtOnlineSafe,&QTabWidget::currentChanged,this,[=](){
        SwitchPageParSaveHandle(ParSaveIndex::OnlineSafe);
    });
    connect(ui->tabWgtSafe,&QTabWidget::currentChanged,this,[=](){
        SwitchPageParSaveHandle(ParSaveIndex::ClawSafe);//卡爪安全
        SwitchPageParSaveHandle(ParSaveIndex::OnlineSafe);//联机安全
    });

#if 0
    for (int i=0;i<static_cast<int>(clawSafeWidgets.size());i++)
    {//卡爪安全1～卡爪安全4
        connect(clawSafeWidgets[i].clawKeepoutSta,QOverload<int>::of(&QComboBox::currentIndexChanged),[=](){
            M_SaveSetPar.ClawSafeFlag[i] = true;
        });
        connect(clawSafeWidgets[i].axisSelect_X,QOverload<int>::of(&QComboBox::currentIndexChanged),[=](){
            M_SaveSetPar.ClawSafeFlag[i] = true;
        });
        connect(clawSafeWidgets[i].axisSelect_Z,QOverload<int>::of(&QComboBox::currentIndexChanged),[=](){
            M_SaveSetPar.ClawSafeFlag[i] = true;
        });
        connect(clawSafeWidgets[i].clawKeepoutMinX,&NumberEdit::returnPressed,[=](){
            M_SaveSetPar.ClawSafeFlag[i] = true;
        });
        connect(clawSafeWidgets[i].clawKeepoutMaxX,&NumberEdit::returnPressed,[=](){
            M_SaveSetPar.ClawSafeFlag[i] = true;
        });
        connect(clawSafeWidgets[i].clawKeepoutHighZ,&NumberEdit::returnPressed,[=](){
            M_SaveSetPar.ClawSafeFlag[i] = true;
        });
        connect(clawSafeWidgets[i].cKeepoutMinX,&NumberEdit::returnPressed,[=](){
            M_SaveSetPar.ClawSafeFlag[i] = true;
        });
        connect(clawSafeWidgets[i].cKeepoutMaxX,&NumberEdit::returnPressed,[=](){
            M_SaveSetPar.ClawSafeFlag[i] = true;
        });
        connect(clawSafeWidgets[i].cKeepoutHighZ,&NumberEdit::returnPressed,[=](){
            M_SaveSetPar.ClawSafeFlag[i] = true;
        });
        connect(clawSafeWidgets[i].cKeepoutPosMinC,&NumberEdit::returnPressed,[=](){
            M_SaveSetPar.ClawSafeFlag[i] = true;
        });
        connect(clawSafeWidgets[i].cKeepoutPosMaxC,&NumberEdit::returnPressed,[=](){
            M_SaveSetPar.ClawSafeFlag[i] = true;
        });
        connect(clawSafeWidgets[i].cKeepoutNegMinC,&NumberEdit::returnPressed,[=](){
            M_SaveSetPar.ClawSafeFlag[i] = true;
        });
        connect(clawSafeWidgets[i].cKeepoutNegMaxC,&NumberEdit::returnPressed,[=](){
            M_SaveSetPar.ClawSafeFlag[i] = true;
        });
    }

    for (int i=0;i<static_cast<int>(onlineSafeWidgets.size());i++)
    {//联机安全1～联机安全4
        connect(onlineSafeWidgets[i].mainSunMode,QOverload<int>::of(&QComboBox::activated),[=](){
            M_SaveSetPar.OnlineSafe[i] = true;
        });
        connect(onlineSafeWidgets[i].onlineSelect,QOverload<int>::of(&QComboBox::activated),[=](){
            M_SaveSetPar.OnlineSafe[i] = true;
        });
        connect(onlineSafeWidgets[i].axisNum,QOverload<int>::of(&QComboBox::activated),[=](){
            M_SaveSetPar.OnlineSafe[i] = true;
        });
        connect(onlineSafeWidgets[i].pluseTime,&NumberEdit::returnPressed,[=](){
            M_SaveSetPar.OnlineSafe[i] = true;
        });
        connect(onlineSafeWidgets[i].areaInNum,&QLineEdit::returnPressed,[=](){
            M_SaveSetPar.OnlineSafe[i] = true;
        });
        connect(onlineSafeWidgets[i].requestInNum,&QLineEdit::returnPressed,[=](){
            M_SaveSetPar.OnlineSafe[i] = true;
        });
        connect(onlineSafeWidgets[i].requestOutNum,&QLineEdit::returnPressed,[=](){
            M_SaveSetPar.OnlineSafe[i] = true;
        });
        connect(onlineSafeWidgets[i].a1A2MainPos,&NumberEdit::returnPressed,[=](){
            M_SaveSetPar.OnlineSafe[i] = true;
        });
        connect(onlineSafeWidgets[i].a1A2SunPos,&NumberEdit::returnPressed,[=](){
            M_SaveSetPar.OnlineSafe[i] = true;
        });
    }
#else
    for (int i=0;i<static_cast<int>(clawSafeWidgets.size());i++)
    {//卡爪安全1～卡爪安全4
        connect(clawSafeWidgets[i].clawKeepoutSta,QOverload<int>::of(&QComboBox::activated),[=](){
            M_SaveSetPar.ClawSafeFlag[i] = true;
        });
        connect(clawSafeWidgets[i].axisSelect_X,QOverload<int>::of(&QComboBox::activated),[=](){
            M_SaveSetPar.ClawSafeFlag[i] = true;
        });
        connect(clawSafeWidgets[i].axisSelect_Z,QOverload<int>::of(&QComboBox::activated),[=](){
            M_SaveSetPar.ClawSafeFlag[i] = true;
        });
        connect(clawSafeWidgets[i].clawKeepoutMinX,&QLineEdit::editingFinished,[=](){
            M_SaveSetPar.ClawSafeFlag[i] = true;
        });
        connect(clawSafeWidgets[i].clawKeepoutMaxX,&QLineEdit::editingFinished,[=](){
            M_SaveSetPar.ClawSafeFlag[i] = true;
        });
        connect(clawSafeWidgets[i].clawKeepoutHighZ,&QLineEdit::editingFinished,[=](){
            M_SaveSetPar.ClawSafeFlag[i] = true;
        });
        connect(clawSafeWidgets[i].cKeepoutMinX,&QLineEdit::editingFinished,[=](){
            M_SaveSetPar.ClawSafeFlag[i] = true;
        });
        connect(clawSafeWidgets[i].cKeepoutMaxX,&QLineEdit::editingFinished,[=](){
            M_SaveSetPar.ClawSafeFlag[i] = true;
        });
        connect(clawSafeWidgets[i].cKeepoutHighZ,&QLineEdit::editingFinished,[=](){
            M_SaveSetPar.ClawSafeFlag[i] = true;
        });
        connect(clawSafeWidgets[i].cKeepoutPosMinC,&QLineEdit::editingFinished,[=](){
            M_SaveSetPar.ClawSafeFlag[i] = true;
        });
        connect(clawSafeWidgets[i].cKeepoutPosMaxC,&QLineEdit::editingFinished,[=](){
            M_SaveSetPar.ClawSafeFlag[i] = true;
        });
        connect(clawSafeWidgets[i].cKeepoutNegMinC,&QLineEdit::editingFinished,[=](){
            M_SaveSetPar.ClawSafeFlag[i] = true;
        });
        connect(clawSafeWidgets[i].cKeepoutNegMaxC,&QLineEdit::editingFinished,[=](){
            M_SaveSetPar.ClawSafeFlag[i] = true;
        });
    }

    for (int i=0;i<static_cast<int>(onlineSafeWidgets.size());i++)
    {//联机安全1～联机安全4
        connect(onlineSafeWidgets[i].mainSunMode,QOverload<int>::of(&QComboBox::activated),[=](){
            M_SaveSetPar.OnlineSafe[i] = true;
        });
        connect(onlineSafeWidgets[i].onlineSelect,QOverload<int>::of(&QComboBox::activated),[=](){
            M_SaveSetPar.OnlineSafe[i] = true;
        });
        connect(onlineSafeWidgets[i].axisNum,QOverload<int>::of(&QComboBox::activated),[=](){
            M_SaveSetPar.OnlineSafe[i] = true;
        });
        connect(onlineSafeWidgets[i].pluseTime,&QLineEdit::editingFinished,[=](){
            M_SaveSetPar.OnlineSafe[i] = true;
        });
        connect(onlineSafeWidgets[i].areaInNum,&QLineEdit::editingFinished,[=](){
            M_SaveSetPar.OnlineSafe[i] = true;
        });
        connect(onlineSafeWidgets[i].requestInNum,&QLineEdit::editingFinished,[=](){
            M_SaveSetPar.OnlineSafe[i] = true;
        });
        connect(onlineSafeWidgets[i].requestOutNum,&QLineEdit::editingFinished,[=](){
            M_SaveSetPar.OnlineSafe[i] = true;
        });
        connect(onlineSafeWidgets[i].a1A2MainPos,&QLineEdit::editingFinished,[=](){
            M_SaveSetPar.OnlineSafe[i] = true;
        });
        connect(onlineSafeWidgets[i].a1A2SunPos,&QLineEdit::editingFinished,[=](){
            M_SaveSetPar.OnlineSafe[i] = true;
        });
    }
#endif
    /****伺服参数子界面****/
    connect(ui->tabWgtServoPara,&QTabWidget::currentChanged,this,[=](){
        SwitchPageParSaveHandle(ParSaveIndex::AllMachinePara);
    });
    for(int i=0;i<8;i++)
    {//伺服参数
        connect(servoPara.at(i),QOverload<int>::of(&QComboBox::activated),[=](){
            M_SaveSetPar.SaveServoPara = true;
        });
    }
    for(int i=0;i<AXIS_TOTAL_NUM;i++)
    {//轴参数
        connect(machineParaWidgets.at(i).axisMinPos,&NumberEdit::returnPressed,[=](){
            M_SaveSetPar.SaveAxisPara = true;
        });
        connect(machineParaWidgets.at(i).axisMaxPos,&NumberEdit::returnPressed,[=](){
            M_SaveSetPar.SaveAxisPara = true;
        });
        connect(machineParaWidgets.at(i).circlePluseNum,&NumberEdit::returnPressed,[=](){
            M_SaveSetPar.SaveAxisPara = true;
        });
        connect(machineParaWidgets.at(i).circleDis,&NumberEdit::returnPressed,[=](){
            M_SaveSetPar.SaveAxisPara = true;
        });
        connect(machineParaWidgets.at(i).maxSpeed,&NumberEdit::returnPressed,[=](){
            M_SaveSetPar.SaveAxisPara = true;
        });
        connect(machineParaWidgets.at(i).coordDir,QOverload<int>::of(&QComboBox::activated),[=](){
            M_SaveSetPar.SaveAxisPara = true;
        });
    }
    /****伺服安全点子界面****/
    connect(ui->tabWgtServoSafePoint,&QTabWidget::currentChanged,this,[=](){
        SwitchPageParSaveHandle(ParSaveIndex::ServoSafePoint);
        SwitchPageParSaveHandle(ParSaveIndex::horizontalPosList);
    });
    for(int i=0;i<static_cast<int>(servoPointSafeArea.size());i++)
    {
        connect(servoPointSafeArea.at(i).safeAreaSwt,QOverload<int>::of(&QComboBox::activated),[=](){
            M_SaveSetPar.SafeArea[i] = true;
        });
        connect(servoPointSafeArea.at(i).processFinishNum,QOverload<int>::of(&QComboBox::activated),[=](){
            M_SaveSetPar.SafeArea[i] = true;
        });
        connect(servoPointSafeArea.at(i).axisSelect[0],QOverload<int>::of(&QComboBox::activated),[=](){
            M_SaveSetPar.SafeArea[i] = true;
        });
        connect(servoPointSafeArea.at(i).axisSelect[1],QOverload<int>::of(&QComboBox::activated),[=](){
            M_SaveSetPar.SafeArea[i] = true;
        });
        connect(servoPointSafeArea.at(i).axisSelect[2],QOverload<int>::of(&QComboBox::activated),[=](){
            M_SaveSetPar.SafeArea[i] = true;
        });
        connect(servoPointSafeArea.at(i).machineA1Pos[0],&NumberEdit::returnPressed,[=](){
            M_SaveSetPar.SafeArea[i] = true;
        });
        connect(servoPointSafeArea.at(i).machineA1Pos[1],&NumberEdit::returnPressed,[=](){
            M_SaveSetPar.SafeArea[i] = true;
        });
        connect(servoPointSafeArea.at(i).machineA1Pos[2],&NumberEdit::returnPressed,[=](){
            M_SaveSetPar.SafeArea[i] = true;
        });
        connect(servoPointSafeArea.at(i).machineA2Pos[0],&NumberEdit::returnPressed,[=](){
            M_SaveSetPar.SafeArea[i] = true;
        });
        connect(servoPointSafeArea.at(i).machineA2Pos[1],&NumberEdit::returnPressed,[=](){
            M_SaveSetPar.SafeArea[i] = true;
        });
        connect(servoPointSafeArea.at(i).machineA2Pos[2],&NumberEdit::returnPressed,[=](){
            M_SaveSetPar.SafeArea[i] = true;
        });
        connect(servoPointSafeArea.at(i).starckB1Pos[0],&NumberEdit::returnPressed,[=](){
            M_SaveSetPar.SafeArea[i] = true;
        });
        connect(servoPointSafeArea.at(i).starckB1Pos[1],&NumberEdit::returnPressed,[=](){
            M_SaveSetPar.SafeArea[i] = true;
        });
        connect(servoPointSafeArea.at(i).starckB2Pos[0],&NumberEdit::returnPressed,[=](){
            M_SaveSetPar.SafeArea[i] = true;
        });
        connect(servoPointSafeArea.at(i).starckB2Pos[1],&NumberEdit::returnPressed,[=](){
            M_SaveSetPar.SafeArea[i] = true;
        });
        connect(servoPointSafeArea.at(i).machine_Z_WaitMaxPos,&NumberEdit::returnPressed,[=](){
            M_SaveSetPar.SafeArea[i] = true;
        });
        connect(servoPointSafeArea.at(i).machine_Z_FallMaxPos,&NumberEdit::returnPressed,[=](){
            M_SaveSetPar.SafeArea[i] = true;
        });
        connect(servoPointSafeArea.at(i).machine_Z_InsideHigh,&NumberEdit::returnPressed,[=](){
            M_SaveSetPar.SafeArea[i] = true;
        });
        connect(servoPointSafeArea.at(i).stack_Z_StartMaxPos,&NumberEdit::returnPressed,[=](){
            M_SaveSetPar.SafeArea[i] = true;
        });
        connect(servoPointSafeArea.at(i).stack_Z_FallMaxPos,&NumberEdit::returnPressed,[=](){
            M_SaveSetPar.SafeArea[i] = true;
        });
        connect(toleranceList[0],&NumberEdit::returnPressed,[=](){
            M_SaveSetPar.SaveServoSafePoint = true;
        });
        connect(toleranceList[1],&NumberEdit::returnPressed,[=](){
            M_SaveSetPar.SaveServoSafePoint = true;
        });
        connect(toleranceList[2],&NumberEdit::returnPressed,[=](){
            M_SaveSetPar.SaveServoSafePoint = true;
        });
        connect(horizontalPosList[0],&NumberEdit::returnPressed,[=](){
            M_SaveSetPar.SaveServoSafePoint = true;
        });
        connect(horizontalPosList[1],&NumberEdit::returnPressed,[=](){
            M_SaveSetPar.SaveServoSafePoint = true;
        });
        connect(horizontalPosList[2],&NumberEdit::returnPressed,[=](){
            M_SaveSetPar.SaveServoSafePoint = true;
        });
        connect(horizontalPosList[3],&NumberEdit::returnPressed,[=](){
            M_SaveSetPar.SaveServoSafePoint = true;
        });
    }
    /****机器参数子界面****/
    connect(ui->tabWidgetMachinePara,&QTabWidget::currentChanged,this,[=](){
        SwitchPageParSaveHandle(ParSaveIndex::AllMachinePara);
    });
    for(int i=0;i<AXIS_TOTAL_NUM;i++)
    {
        connect(machineParaWidgets.at(i).limitPosSwt,QOverload<int>::of(&QComboBox::activated),[=](){
            M_SaveSetPar.SaveMachineParaLimit = true;
        });
        connect(machineParaWidgets.at(i).limitNegSwt,QOverload<int>::of(&QComboBox::activated),[=](){
            M_SaveSetPar.SaveMachineParaLimit = true;
        });
        connect(machineParaWidgets.at(i).axisType,QOverload<int>::of(&QComboBox::activated),[=](){
            M_SaveSetPar.SaveMachineParaStruct = true;
        });
        connect(machineParaWidgets.at(i).axisMoveMade,QOverload<int>::of(&QComboBox::activated),[=](){
            M_SaveSetPar.SaveMachineParaStruct = true;
        });

        connect(machineParaWidgets.at(i).originType,QOverload<int>::of(&QComboBox::activated),[=](){
            M_SaveSetPar.SaveMachineParaOrigin = true;
        });

        connect(machineParaWidgets.at(i).findOriginSpeed,&NumberEdit::returnPressed,[=](){
            M_SaveSetPar.SaveMachineParaOrigin = true;
        });

        connect(machineParaWidgets.at(i).leaveOriginSpeed,&NumberEdit::returnPressed,[=](){
            M_SaveSetPar.SaveMachineParaOrigin = true;
        });

        connect(machineParaWidgets.at(i).originOffset,&NumberEdit::returnPressed,[=](){
            M_SaveSetPar.SaveMachineParaOrigin = true;
        });

        connect(machineParaWidgets.at(i).originDir,QOverload<int>::of(&QComboBox::activated),[=](){
            M_SaveSetPar.SaveMachineParaOrigin = true;
        });

        connect(machineParaWidgets.at(i).backOriginOrder,QOverload<int>::of(&QComboBox::activated),[=](){
            M_SaveSetPar.SaveMachineParaOrigin = true;
        });
    }

}
//切换界面时参数保存处理
void Setting::SwitchPageParSaveHandle(ParSaveIndex SaveIndex)
{

    switch(SaveIndex) {
    case ParSaveIndex::SavePort:
    {
        if(M_SaveSetPar.SavePort == true)
        {
            int reply = MainWindow::pMainWindow->showErrorTip(tr("配置参数有修改，是否需要保存？"),TipMode::NORMAL);
            if(reply == QDialog::Accepted)
            {
                this->savePortDefine();
            }
            else if(reply == QDialog::Rejected)
            {
                RefreshPortDefine();
                M_SaveSetPar.SavePort = false;
                ui->btnSavePort->setParaChangedFlag(false);
            }
        }
        break;
    }
    case ParSaveIndex::SaveNameDef:
    {
        if(M_SaveSetPar.SaveNameDef == true)
        {
            int reply = MainWindow::pMainWindow->showErrorTip(tr("配置参数有修改，是否需要保存？"),TipMode::NORMAL);
            if(reply == QDialog::Accepted)
            {
                this->saveNameDefine();
            }
            else if(reply == QDialog::Rejected)
            {
                RefreshNameDefine();
                M_SaveSetPar.SaveNameDef = false;
                ui->btnSaveNameDef->setParaChangedFlag(false);
            }
        }
        break;
    }
    case ParSaveIndex::ClawSafe:
    {
        for(int i=0;i<static_cast<int>(clawSafeWidgets.size());i++)
        {
            if(M_SaveSetPar.ClawSafeFlag[i]==true)
            {
                int reply = MainWindow::pMainWindow->showErrorTip(tr("配置参数有修改，是否需要保存？"),TipMode::NORMAL);
                if(reply == QDialog::Accepted)
                {
                    saveClawSafePara(i);
                }
                else if(reply == QDialog::Rejected)
                {
                    UpdateClawSafepage(i);
                    M_SaveSetPar.ClawSafeFlag[i] = false;
                }
            }
        }
        break;
    }
    case ParSaveIndex::OnlineSafe:
    {
        for(int i=0;i<static_cast<int>(onlineSafeWidgets.size());i++)
        {
            if(M_SaveSetPar.OnlineSafe[i]==true)
            {
                int reply = MainWindow::pMainWindow->showErrorTip(tr("配置参数有修改，是否需要保存？"),TipMode::NORMAL);
                if(reply == QDialog::Accepted)
                {
                    saveOnlineSafePara(i);
                }
                else if(reply == QDialog::Rejected)
                {
                    UpdateOnlineSafepage(i);
                    M_SaveSetPar.OnlineSafe[i] = false;
                }
            }
        }
        break;
    }
    case ParSaveIndex::AllMachinePara:
    {
        if(M_SaveSetPar.SaveMachineParaLimit == true)
        {
            int reply = MainWindow::pMainWindow->showErrorTip(tr("配置参数有修改，是否需要保存？"),TipMode::NORMAL);
            if(reply == QDialog::Accepted)
            {
                saveMachineAllPara(0);
            }
            else if(reply == QDialog::Rejected)
            {
                updateAxisParPage();
            }
            M_SaveSetPar.SaveMachineParaLimit = false;
        }
        if(M_SaveSetPar.SaveMachineParaStruct == true)
        {
            int reply = MainWindow::pMainWindow->showErrorTip(tr("配置参数有修改，是否需要保存？"),TipMode::NORMAL);
            if(reply == QDialog::Accepted)
            {
                saveMachineAllPara(1);
            }
            else if(reply == QDialog::Rejected)
            {
                updateAxisParPage();
            }
            M_SaveSetPar.SaveMachineParaStruct = false;
        }
        if(M_SaveSetPar.SaveMachineParaOrigin == true)
        {
            int reply = MainWindow::pMainWindow->showErrorTip(tr("配置参数有修改，是否需要保存？"),TipMode::NORMAL);
            if(reply == QDialog::Accepted)
            {
                saveMachineAllPara(2);
            }
            else if(reply == QDialog::Rejected)
            {
                updateAxisParPage();
            }
            M_SaveSetPar.SaveMachineParaOrigin = false;
        }
        if(M_SaveSetPar.SaveServoPara == true)
        {
            int reply = MainWindow::pMainWindow->showErrorTip(tr("配置参数有修改，是否需要保存？"),TipMode::NORMAL);
            if(reply == QDialog::Accepted)
            {
                saveMachineAllPara(4);
            }
            else if(reply == QDialog::Rejected)
            {
                updateServoParaPage();
            }
            M_SaveSetPar.SaveServoPara = false;
        }
        if(M_SaveSetPar.SaveAxisPara == true)
        {
            int reply = MainWindow::pMainWindow->showErrorTip(tr("配置参数有修改，是否需要保存？"),TipMode::NORMAL);
            if(reply == QDialog::Accepted)
            {
                saveMachineAllPara(5);
            }
            else if(reply == QDialog::Rejected)
            {
                updateAxisParPage();
            }
            M_SaveSetPar.SaveAxisPara = false;
        }
        break;
    }
    case ParSaveIndex::ServoSafePoint:
    {
        for(int i=0;i<static_cast<int>(servoPointSafeArea.size());i++)
        {
            if(M_SaveSetPar.SafeArea[i]==true)
            {
                int reply = MainWindow::pMainWindow->showErrorTip(tr("配置参数有修改，是否需要保存？"),TipMode::NORMAL);
                if(reply == QDialog::Accepted)
                {
                    saveSafeAreaPara(i);
                }
                else if(reply == QDialog::Rejected)
                {
                    updateSafeAreaPage(i);
                }
                M_SaveSetPar.SafeArea[i] = false;
            }
        }
        break;
    }
    case ParSaveIndex::horizontalPosList:
    {
        for(int i=0;i<static_cast<int>(toleranceList.size());i++)
        {
            if(M_SaveSetPar.SaveServoSafePoint==true)
            {
                int reply = MainWindow::pMainWindow->showErrorTip(tr("配置参数有修改，是否需要保存？"),TipMode::NORMAL);
                if(reply == QDialog::Accepted)
                {
                    saveTolerancePara();
                    QThread::msleep(10);
                    saveDegreePara();
                }
                else if(reply == QDialog::Rejected)
                {
                    updatehorizontalPosList();
                }
                M_SaveSetPar.SaveServoSafePoint = false;
            }
        }
        break;
    }
    default:
        break;
    }

}
void Setting::outputTypeSlots()
{
    //先将最新的参数存入结构体中
    for (int i=0;i<OUT_PORT_TYPE_NUM;i++)
    {
        m_OutPortType[i][1]=outputTypeList[i]->currentIndex();
    }
    g_Usart->ExtendSendParDeal(CMD_MAIN_SIGNAL,CMD_SUN_SIGNAL_OUT_TYPE);

    setOutPortType(m_OutPortType);
}

void Setting::outportInterlockSlots()
{
    //先将最新的参数存入结构体中
    for (int i=0;i<OUT_INTERLOCK_NUM;i++)
    {
        for (int j=0;j<4;j++)
        {
            if(outportInterlockList[4*i+j]->isChecked())
            {
                m_OutportInterlock[i][j] = outportInterlockIndex[i][j];
            }
            else
            {
                m_OutportInterlock[i][j] = 0;
            }
        }
    }

    g_Usart->ExtendSendParDeal(CMD_MAIN_SIGNAL,CMD_SUN_SIGNAL_INTERLOCK);
    setOutportInterlock(m_OutportInterlock);

    ::updateInterLockPortFlag();

    emit RefreshPortDefineSignals();
    emit refreshManualReserve(); // 更新手动预留界面的按钮可用性
    emit WidgetNameRefresh_signal();//更新教导界面控件相关内容
}

void Setting::outportRelevancySlots()
{
    //先将最新的参数存入结构体中
    for (int i=0;i<OUT_PORT_RELEVANCY_NUM;i++)
    {
        m_OutportRelevancy[i][2]=outportRelevancyList[i]->currentIndex();
    }
    g_Usart->ExtendSendParDeal(CMD_MAIN_SIGNAL,CMD_SUN_SIGNAL_OUT_RELEVENCY);
    setOutportRelevancy(m_OutportRelevancy);
}

void Setting::outportReleteOutSlots()
{
    //先将最新的参数存入结构体中
    for (int i=0;i<OUT_PORT_RELEOUT_NUM;i++)
    {
        m_OutportReleteOut[i][1]=outportReleteOutList[i]->currentIndex();
    }
    g_Usart->ExtendSendParDeal(CMD_MAIN_SIGNAL,CMD_SUN_SIGNAL_RELATE_OUT);
    setReleteOut(m_OutportReleteOut);
}

void Setting::seniorFuncSlots()
{
    uint8_t index=0;
    m_SeniorFunc.knifeOrigin1Check = seniorFuncList[index++]->currentIndex();
    m_SeniorFunc.knifeOrigin2Check = seniorFuncList[index++]->currentIndex();
    m_SeniorFunc.chuckOriginCheck = seniorFuncList[index++]->currentIndex();
    m_SeniorFunc.stackSaveIn1Check = seniorFuncList[index++]->currentIndex();
    m_SeniorFunc.stackSaveIn2Check = seniorFuncList[index++]->currentIndex();
    m_SeniorFunc.stackSaveOutCheck = seniorFuncList[index++]->currentIndex();
    m_SeniorFunc.alarmIn1Check = seniorFuncList[index++]->currentIndex();
    m_SeniorFunc.alarmIn2Check = seniorFuncList[index++]->currentIndex();

    m_SeniorFunc.emergencyStopCheck = seniorFuncList[index++]->currentIndex();
    m_SeniorFunc.pauseStopCheck = seniorFuncList[index++]->currentIndex();
    m_SeniorFunc.pressureCheck = seniorFuncList[index++]->currentIndex();
    m_SeniorFunc.remoteAuto = seniorFuncList[index++]->currentIndex();
    m_SeniorFunc.remoteStop = seniorFuncList[index++]->currentIndex();
    m_SeniorFunc.bottomOilLimit = seniorFuncList[index++]->currentIndex();
    m_SeniorFunc.oilerSwt = seniorFuncList[index++]->currentIndex();
    m_SeniorFunc.lubPump = seniorFuncList[index++]->currentIndex();

    m_SeniorFunc.processSave1 = seniorFuncList[index++]->currentIndex();
    m_SeniorFunc.processSave2 = seniorFuncList[index++]->currentIndex();
    m_SeniorFunc.emergencySnapMotorEnable = seniorFuncList[index++]->currentIndex();
    m_SeniorFunc.emergencyStopOut = seniorFuncList[index++]->currentIndex();
    m_SeniorFunc.autoLight = seniorFuncList[index++]->currentIndex();
    m_SeniorFunc.alarmLight = seniorFuncList[index++]->currentIndex();
    m_SeniorFunc.alarmBuzzer = seniorFuncList[index++]->currentIndex();
    m_SeniorFunc.pauseLight = seniorFuncList[index++]->currentIndex();

    m_SeniorFunc.processFinish1 = seniorFuncList[index++]->currentIndex();
    m_SeniorFunc.processFinish2 = seniorFuncList[index++]->currentIndex();
    m_SeniorFunc.locateFinish1 = seniorFuncList[index++]->currentIndex();
    m_SeniorFunc.locateFinish2 = seniorFuncList[index++]->currentIndex();
    m_SeniorFunc.startProduct1 = seniorFuncList[index++]->currentIndex();
    m_SeniorFunc.startProduct2 = seniorFuncList[index++]->currentIndex();
    m_SeniorFunc.res1[0] = seniorFuncList[index++]->currentIndex();
    m_SeniorFunc.res1[1] = seniorFuncList[index++]->currentIndex();

    m_SeniorFunc.mainAxisRotate1 = seniorFuncList[index++]->currentIndex();
    m_SeniorFunc.mainAxisRotate2 = seniorFuncList[index++]->currentIndex();
    m_SeniorFunc.mainAxisLocate1 = seniorFuncList[index++]->currentIndex();
    m_SeniorFunc.mainAxisLocate2 = seniorFuncList[index++]->currentIndex();
    m_SeniorFunc.biowAir1 = seniorFuncList[index++]->currentIndex();
    m_SeniorFunc.biowAir2 = seniorFuncList[index++]->currentIndex();
    m_SeniorFunc.res2[0] = seniorFuncList[index++]->currentIndex();
    m_SeniorFunc.res2[1] = seniorFuncList[index++]->currentIndex();

    m_SeniorFunc.manualChuckIn1 = seniorFuncList[index++]->currentIndex();
    m_SeniorFunc.manualChuckIn2 = seniorFuncList[index++]->currentIndex();
    m_SeniorFunc.autoDoorCot1 = seniorFuncList[index++]->currentIndex();
    m_SeniorFunc.autoDoorCot2 = seniorFuncList[index++]->currentIndex();
    m_SeniorFunc.res3[0] = seniorFuncList[index++]->currentIndex();
    m_SeniorFunc.res3[1] = seniorFuncList[index++]->currentIndex();
    m_SeniorFunc.res3[2] = seniorFuncList[index++]->currentIndex();
    m_SeniorFunc.res3[3] = seniorFuncList[index++]->currentIndex();
    g_Usart->ExtendSendParDeal(CMD_MAIN_SIGNAL,CMD_SUN_SIGNAL_SENIOR);
//    _sleep(10);
//    QThread::msleep(10);
    g_Usart->ExtendSendParDeal(CMD_MAIN_SIGNAL,CMD_SUN_SIGNAL_SENIOR_PORT);

    setSeniorFunc(m_SeniorFunc);
    SeniorFuncPortSet();

    emit RefreshPortDefineSignals();
    emit refreshManualReserve(); // 更新手动预留界面的按钮可用性
    emit WidgetNameRefresh_signal();//更新教导界面控件相关内容

    emit refreshManualReserve();
    emit WidgetNameRefresh_signal();
}
//根据高级功能中功能使用情况，设置端口是否做预留端口使用
void Setting::SeniorFuncPortSet()
{
    //输入端口
    m_Port_X[6].functionSet = m_SeniorFunc.stackSaveIn1Check;
    m_Port_X[7].functionSet = m_SeniorFunc.emergencyStopCheck;
    m_Port_X[12].functionSet = m_SeniorFunc.pressureCheck;
//    m_Port_X[14].functionSet = m_SeniorFunc.remoteStop;//这几个IO与轴限位功能相冲突，暂时不明确需求
//    m_Port_X[15].functionSet = m_SeniorFunc.remoteAuto;
//    m_Port_X[17].functionSet = m_SeniorFunc.manualChuckIn1;
    m_Port_X[31].functionSet = m_SeniorFunc.processFinish1;
    m_Port_X[32].functionSet = m_SeniorFunc.locateFinish1;
    m_Port_X[33].functionSet = m_SeniorFunc.knifeOrigin1Check;
    m_Port_X[34].functionSet = m_SeniorFunc.alarmIn1Check;
    m_Port_X[35].functionSet = m_SeniorFunc.pauseStopCheck;
    m_Port_X[42].functionSet = m_SeniorFunc.stackSaveIn2Check;
    m_Port_X[43].functionSet = m_SeniorFunc.manualChuckIn2;
    m_Port_X[52].functionSet = m_SeniorFunc.processFinish2;
    m_Port_X[53].functionSet = m_SeniorFunc.locateFinish2;
    m_Port_X[54].functionSet = m_SeniorFunc.knifeOrigin2Check;
    m_Port_X[55].functionSet = m_SeniorFunc.alarmIn2Check;

    //输出端口
    // m_Port_Y[0] -> Y1 原料1夹紧
    // m_Port_Y[1] -> Y2 原料1夹紧
    // m_Port_Y[2] -> Y3 成品1夹紧
    // m_Port_Y[3] -> Y4 成品1夹紧
    // m_Port_Y[4] -> Y5 卡爪1夹紧
    // m_Port_Y[5] -> Y6 卡爪1夹紧
    m_Port_Y[6].functionSet = m_SeniorFunc.autoLight;//根据某个端口功能是否使用，置端口预留标志位
    m_Port_Y[7].functionSet = m_SeniorFunc.alarmLight;
    m_Port_Y[8].functionSet = m_SeniorFunc.autoDoorCot1;
    m_Port_Y[9].functionSet = m_SeniorFunc.autoDoorCot1;
    m_Port_Y[10].functionSet = m_SeniorFunc.biowAir1;
    m_Port_Y[12].functionSet = m_SeniorFunc.emergencyStopOut;
    m_Port_Y[13].functionSet = m_SeniorFunc.lubPump;
    m_Port_Y[14].functionSet = m_SeniorFunc.alarmBuzzer;
    m_Port_Y[15].functionSet = m_SeniorFunc.pauseLight;
    m_Port_Y[16].functionSet = m_SeniorFunc.startProduct1;
    m_Port_Y[17].functionSet = m_SeniorFunc.mainAxisLocate1;
    m_Port_Y[18].functionSet = m_SeniorFunc.processSave1;
    m_Port_Y[20].functionSet = m_SeniorFunc.mainAxisRotate1;


    m_Port_Y[OUTPUT_NUM +  7].functionSet = m_SeniorFunc.biowAir2;
    m_Port_Y[OUTPUT_NUM +  8].functionSet = m_SeniorFunc.autoDoorCot2;
    m_Port_Y[OUTPUT_NUM +  9].functionSet = m_SeniorFunc.autoDoorCot2;
    m_Port_Y[OUTPUT_NUM + 16].functionSet = m_SeniorFunc.startProduct2;
    m_Port_Y[OUTPUT_NUM + 17].functionSet = m_SeniorFunc.mainAxisLocate2;
    m_Port_Y[OUTPUT_NUM + 18].functionSet = m_SeniorFunc.processSave2;
}
void Setting::saveKeyAndLEDFuncDefine()
{
    for (int i = 0; i < OPR_KEY_NUM; i++)
    {
        m_KeyFunc[i].keyType = keyEdits.at(i)->getKeyType();
        m_KeyFunc[i].funcNum = keyEdits.at(i)->getPortNum();
        m_KeyFunc[i].oprMode = keyEdits.at(i)->getKeyFuncStatus();
    }
    for (int i = 0; i < OPR_LED_NUM; i++)
    {
        m_LedFunc[i].ledType = sigEdits.at(i)->getSigType();
        m_LedFunc[i].funcNum = sigEdits.at(i)->getSigPort();
    }
    g_Usart->ExtendSendParDeal(CMD_MAIN_SIGNAL,CMD_SUN_SIGNAL_KEY);
    setKeyFunc(m_KeyFunc);
    setLedFunc(m_LedFunc);
}

void Setting::saveMachineSafePara()
{
    m_MachineSave.armFallAutoDoor1Swt = machineSafeList[0]->currentIndex();
    m_MachineSave.armFallAutoDoor1SwtSta = machineSafeList[1]->currentIndex();
    m_MachineSave.armFallAutoDoor2Swt = machineSafeList[2]->currentIndex();
    m_MachineSave.armFallAutoDoor2SwtSta = machineSafeList[3]->currentIndex();
    m_MachineSave.firstProductStartRun = machineSafeList[4]->currentIndex();
    m_MachineSave.firstProductFinish = machineSafeList[5]->currentIndex();
    g_Usart->ExtendSendParDeal(CMD_MAIN_SAVE,CMD_SUN_SAVE_MACHINE);
    setMachineSave(m_MachineSave);
}

void Setting::saveStackBinSafePara()
{
    m_StackSave.stackSaveIn1Roatore = stackBinSafeList[0]->currentIndex();
    m_StackSave.stackSaveIn2Roatore = stackBinSafeList[1]->currentIndex();
    g_Usart->ExtendSendParDeal(CMD_MAIN_SAVE,CMD_SUN_SAVE_STACK);
    setStackSave(m_StackSave);
}

void Setting::saveClawSafePara(int index)
{
    m_ClawSave[index].clawKeepoutSta = clawSafeWidgets.at(index).clawKeepoutSta->currentIndex();
    int selectIndex_X = 0;
    int selectIndex_Z = 0;
    for(int i=0;i<AXIS_TOTAL_NUM;i++)
    {
        if(clawSafeWidgets.at(index).axisSelect_X->currentText() == m_NameDefine[1].axisName[i])
        {
            selectIndex_X = i+1;
        }
        if(clawSafeWidgets.at(index).axisSelect_Z->currentText() == m_NameDefine[1].axisName[i])
        {
            selectIndex_Z = i+1;
        }
    }
    m_ClawSave[index].axisSelect_X = selectIndex_X;
    m_ClawSave[index].axisSelect_Z = selectIndex_Z;

    m_ClawSave[index].clawKeepoutMinX = clawSafeWidgets.at(index).clawKeepoutMinX->text().toFloat()*100;
    m_ClawSave[index].clawKeepoutMaxX = clawSafeWidgets.at(index).clawKeepoutMaxX->text().toFloat()*100;
    m_ClawSave[index].clawKeepoutHighZ = clawSafeWidgets.at(index).clawKeepoutHighZ->text().toFloat()*100;

    m_ClawSave[index].cKeepoutMinX = clawSafeWidgets.at(index).cKeepoutMinX->text().toFloat()*100;
    m_ClawSave[index].cKeepoutMaxX = clawSafeWidgets.at(index).cKeepoutMaxX->text().toFloat()*100;
    m_ClawSave[index].cKeepoutHighZ = clawSafeWidgets.at(index).cKeepoutHighZ->text().toFloat()*100;
    m_ClawSave[index].cKeepoutPosMinC = clawSafeWidgets.at(index).cKeepoutPosMinC->text().toFloat()*100;
    m_ClawSave[index].cKeepoutPosMaxC = clawSafeWidgets.at(index).cKeepoutPosMaxC->text().toFloat()*100;
    m_ClawSave[index].cKeepoutNegMinC = clawSafeWidgets.at(index).cKeepoutNegMinC->text().toFloat()*100;
    m_ClawSave[index].cKeepoutNegMaxC = clawSafeWidgets.at(index).cKeepoutNegMaxC->text().toFloat()*100;
    g_Usart->ExtendSendParDeal(CMD_MAIN_SAVE,CMD_SUN_SAVE_CALW,index+1);
    setClawSave(m_ClawSave[index],index);
    M_SaveSetPar.ClawSafeFlag[index] = false;
}

void Setting::saveOnlineSafePara(int index)
{
    m_OnlineSave[index].mainSunMode = onlineSafeWidgets.at(index).mainSunMode->currentIndex();
    m_OnlineSave[index].pluseTime = onlineSafeWidgets.at(index).pluseTime->text().toFloat()*100;
    m_OnlineSave[index].onlineSelect = onlineSafeWidgets.at(index).onlineSelect->currentIndex();
    m_OnlineSave[index].axisNum = onlineSafeWidgets.at(index).axisNum->currentIndex();
    m_OnlineSave[index].areaInNum = onlineSafeWidgets.at(index).areaInNum->getCurrentPort();
    m_OnlineSave[index].areaOutNum = onlineSafeWidgets.at(index).areaOutNum->getCurrentPort();
    m_OnlineSave[index].requestInNum = onlineSafeWidgets.at(index).requestInNum->getCurrentPort();
    m_OnlineSave[index].requestOutNum = onlineSafeWidgets.at(index).requestOutNum->getCurrentPort();
    m_OnlineSave[index].a1A2MainPos = onlineSafeWidgets.at(index).a1A2MainPos->text().toFloat()*100;
    m_OnlineSave[index].a1A2SunPos = onlineSafeWidgets.at(index).a1A2SunPos->text().toFloat()*100;
    g_Usart->ExtendSendParDeal(CMD_MAIN_SAVE,CMD_SUN_SAVE_ONLINE,index+1);
    setOnlineSafe(m_OnlineSave[index],index);
    M_SaveSetPar.OnlineSafe[index] = false;
}

void Setting::saveProductSetPara()
{
    m_ProductSet.waitProcessFinTime = productSet.at(0)->text().toFloat() * 10;
    m_ProductSet.pluseOutTime = productSet.at(1)->text().toFloat() * 10;
    m_ProductSet.airVlaveLimitTime = productSet.at(2)->text().toFloat() * 10;
    m_ProductSet.autoCycTime = productSet.at(3)->text().toFloat() * 10;
    m_ProductSet.alarmBuzzerTime = productSet.at(4)->text().toFloat() * 10;
    m_ProductSet.chainReachDelay = productSet.at(5)->text().toFloat() * 10;
    m_ProductSet.clampCheckFilterTime = productSet.at(6)->text().toFloat() * 10;
    g_Usart->ExtendSendParDeal(CMD_MAIN_PRODUCT,CMD_SUN_PRODUCT_PAR);
    setProductSet(m_ProductSet);
}

void Setting::saveProductSetSeniorPara()
{
    m_ProductSenior.lubricOpenTime = productSeniorEdit.at(0)->text().toFloat() * 10;
    m_ProductSenior.lubricCloseTime = productSeniorEdit.at(1)->text().toFloat() * 10;
    m_ProductSenior.lubricDelay = productSeniorEdit.at(2)->text().toFloat() * 10;
    m_ProductSenior.lubricGapCyc = productSeniorEdit.at(3)->text().toInt();
    m_ProductSenior.productMemoryFlag = ui->coboxProductMemory->currentIndex();
    g_Usart->ExtendSendParDeal(CMD_MAIN_PRODUCT,CMD_SUN_PRODUCT_SENIOR);
    setProductSenior(m_ProductSenior);
}

void Setting::saveProductSetInternetPara()
{
    m_Internet.netCommType = productIot.at(0)->currentIndex();
    m_Internet.parityMode = productIot.at(1)->currentIndex();
    m_Internet.baudMode = productIot.at(2)->currentIndex();
    g_Usart->ExtendSendParDeal(CMD_MAIN_PRODUCT,CMD_SUN_PRODUCT_INTERNET);
    setInternet(m_Internet);
}

void Setting::saveServoPara()
{
//    m_ServoPar.encoderType = servoPara.at(0)->currentIndex();
//    m_ServoPar.encoderBitNum = servoPara.at(1)->currentIndex();
//    m_ServoPar.servoBrand = servoPara.at(2)->currentIndex();
//    m_ServoPar.servoModel = servoPara.at(3)->currentIndex();
//    m_ServoPar.extendIOBoard = servoPara.at(4)->currentIndex();
//    m_ServoPar.rotateAxisUnit = servoPara.at(5)->currentIndex();
//    m_ServoPar.resetSunProFlag = servoPara.at(6)->currentIndex();
//    m_ServoPar.torqueLimitFlag = servoPara.at(7)->currentIndex();
//    g_Usart->ExtendSendParDeal(CMD_MAIN_MAC,CMD_SUN_MAC_STRUCT);
}

void Setting::saveAccDecTimeSpeed(int index)
{
    if(index < AXIS_TOTAL_NUM)
    {
        m_AxisPar[index].accTime = accTimeList[index]->text().toFloat()*100;
        m_AxisPar[index].decTime = decTimeList[index]->text().toFloat()*100;
        m_AxisPar[index].accAcc = accAccList[index]->text().toInt();
        m_AxisPar[index].decDec = decDecList[index]->text().toInt();
    }
    else
    {
        m_HandwheelPar.accTime = accTimeList[index]->text().toFloat()*100;
        m_HandwheelPar.decTime = decTimeList[index]->text().toFloat()*100;
        m_HandwheelPar.accAcc = accAccList[index]->text().toInt();
        m_HandwheelPar.decDec = decDecList[index]->text().toInt();
    }

    g_Usart->ExtendSendParDeal(CMD_MAIN_SERVO,CMD_SUN_SERVO_ACC_DEC,index+1);
    setAxisPar(m_AxisPar[index],index);
    setHandwheelPar(m_HandwheelPar);
}

void Setting::saveSafeAreaPara(int index)
{
    uint8_t AxisSelect_X=0;//选择的轴编号
    uint8_t AxisSelect_Y=0;
    uint8_t AxisSelect_Z=0;
    if(index>3)
    {
        saveTolerancePara();
        QThread::msleep(10);
        saveDegreePara();
        M_SaveSetPar.SaveServoSafePoint = false;
        return;
    }
    m_SaveArea[index].saveAreaSwt = servoPointSafeArea.at(index).safeAreaSwt->currentIndex();
    m_SaveArea[index].processFinishNum = servoPointSafeArea.at(index).processFinishNum->currentIndex();

    for(auto i=0;i<AXIS_TOTAL_NUM;i++)
    {
        if(m_NameDefine[1].axisName[i] == servoPointSafeArea.at(index).axisSelect[0]->currentText())
        {
            AxisSelect_X = i+1;
        }
        if(m_NameDefine[1].axisName[i] == servoPointSafeArea.at(index).axisSelect[1]->currentText())
        {
            AxisSelect_Y = i+1;
        }
        if(m_NameDefine[1].axisName[i] == servoPointSafeArea.at(index).axisSelect[2]->currentText())
        {
            AxisSelect_Z = i+1;
        }
    }
    m_SaveArea[index].axisSelect[0] = AxisSelect_X;
    m_SaveArea[index].axisSelect[1] = AxisSelect_Y;
    m_SaveArea[index].axisSelect[2] = AxisSelect_Z;

    // 第三组用不上 默认传为0
    for (int i = 0; i < 2; i++)
    {
        m_SaveArea[index].machineA1Pos[i] = servoPointSafeArea.at(index).machineA1Pos[i]->text().toFloat()*100;
        m_SaveArea[index].machineA2Pos[i] = servoPointSafeArea.at(index).machineA2Pos[i]->text().toFloat()*100;
        //m_SaveArea[index].machineA1Pos[i] = servoPointSafeArea.at(index).machineA1Pos[i]->text().toFloat()*100;
        m_SaveArea[index].starckB1Pos[i] = servoPointSafeArea.at(index).starckB1Pos[i]->text().toFloat()*100;
        m_SaveArea[index].starckB2Pos[i] = servoPointSafeArea.at(index).starckB2Pos[i]->text().toFloat()*100;
    }
    m_SaveArea[index].machineA1Pos[SAVE_Z_AXIS] = servoPointSafeArea.at(index).machineA1Pos[SAVE_Z_AXIS]->text().toFloat()*100;
    m_SaveArea[index].machineA2Pos[SAVE_Z_AXIS] = servoPointSafeArea.at(index).machineA2Pos[SAVE_Z_AXIS]->text().toFloat()*100;

    m_SaveArea[index].machine_Z_WaitMaxPos = servoPointSafeArea.at(index).machine_Z_WaitMaxPos->text().toFloat()*100;
    m_SaveArea[index].machine_Z_FallMaxPos = servoPointSafeArea.at(index).machine_Z_FallMaxPos->text().toFloat()*100;
    m_SaveArea[index].machine_Z_InsideHigh = servoPointSafeArea.at(index).machine_Z_InsideHigh->text().toFloat()*100;
    m_SaveArea[index].stack_Z_StartMaxPos = servoPointSafeArea.at(index).stack_Z_StartMaxPos->text().toFloat()*100;
    m_SaveArea[index].stack_Z_FallMaxPos = servoPointSafeArea.at(index).stack_Z_FallMaxPos->text().toFloat()*100;
    g_Usart->ExtendSendParDeal(CMD_MAIN_SP,CMD_SUN_SP_AREA,index+1);
    setSaveArea(m_SaveArea[index],index);
    M_SaveSetPar.SafeArea[index] = false;
}

void Setting::saveTolerancePara()
{
    m_DegreeParS.tolerance = toleranceList[0]->text().toFloat()*100;
    m_DegreeParS.originDegree = toleranceList[1]->text().toFloat()*100;
    m_DegreeParS.actionFinDegree = toleranceList[2]->text().toFloat()*100;
    g_Usart->ExtendSendParDeal(CMD_MAIN_SERVO,CMD_SUN_SERVO_TOLERANCE);
    setDegreeParS(m_DegreeParS);
}

void Setting::saveDegreePara()
{
    m_DegreeParS.saveRampageHige_Z1 = horizontalPosList[0]->text().toFloat()*100;
    m_DegreeParS.saveRampageHige_Z2 = horizontalPosList[1]->text().toFloat()*100;
    m_DegreeParS.saveRotatePos_Y1 = horizontalPosList[2]->text().toFloat()*100;
    m_DegreeParS.saveRotatePos_Y2 = horizontalPosList[3]->text().toFloat()*100;
    g_Usart->ExtendSendParDeal(CMD_MAIN_SP,CMD_SUN_SP_RAMPAGE_LIMIT);
    setDegreeParS(m_DegreeParS);
}

void Setting::saveStackFunc()
{
    m_StackFunc.stackType = stackCoboxs.at(0)->currentIndex();
    m_StackFunc.groupStack =stackCoboxs.at(1)->currentIndex();
    m_StackFunc.rowStack = stackCoboxs.at(2)->currentIndex();
    m_StackFunc.storeyStack =stackCoboxs.at(3)->currentIndex();
    m_StackFunc.divideStack = stackCoboxs.at(4)->currentIndex();
    m_StackFunc.stackMemory =stackCoboxs.at(5)->currentIndex();
    m_StackFunc.siloType = stackCoboxs.at(6)->currentIndex();
    m_StackFunc.rotateSiloFinHint = stackCoboxs.at(7)->currentIndex();
    m_StackFunc.rotateSiloPlaceNum = ui->editRotateSiloPlaceNum->text().toInt();
    g_Usart->ExtendSendParDeal(CMD_MAIN_STACK,CMD_SUN_STACK_SET);
    setStackFunc(m_StackFunc);
}

void Setting::saveMachinePara()
{
    m_ServoPar.encoderType = servoPara.at(0)->currentIndex();
    m_ServoPar.encoderBitNum = servoPara.at(1)->currentIndex();
    m_ServoPar.servoBrand = servoPara.at(2)->currentIndex();
    m_ServoPar.servoModel = servoPara.at(3)->currentIndex();
    m_ServoPar.extendIOBoard = servoPara.at(4)->currentIndex();
    m_ServoPar.rotateAxisUnit = servoPara.at(5)->currentIndex();
    m_ServoPar.resetSunProFlag = servoPara.at(6)->currentIndex();
    m_ServoPar.torqueLimitFlag = servoPara.at(7)->currentIndex();
    for (int i = 0; i < AXIS_TOTAL_NUM; i++)
    {
        m_AxisPar[i].axisType = machineParaWidgets.at(i).axisType->currentIndex();
        m_AxisPar[i].axisMoveMade = machineParaWidgets.at(i).axisMoveMade->currentIndex();
        m_AxisPar[i].accTime = machineParaWidgets.at(i).accTime->text().toFloat()*100;
        m_AxisPar[i].decTime = machineParaWidgets.at(i).decTime->text().toFloat()*100;
        m_AxisPar[i].accAcc = machineParaWidgets.at(i).accAcc->text().toInt();
        m_AxisPar[i].decDec = machineParaWidgets.at(i).decDec->text().toInt();
        m_AxisPar[i].maxSpeed = machineParaWidgets.at(i).maxSpeed->text().toInt();
        m_AxisPar[i].axisMinPos = machineParaWidgets.at(i).axisMinPos->text().toFloat()*100;
        m_AxisPar[i].axisMaxPos = machineParaWidgets.at(i).axisMaxPos->text().toFloat()*100;
        m_AxisPar[i].minPosSignal = machineParaWidgets.at(i).limitMin->getCurrentPort();
        m_AxisPar[i].maxPosSignal = machineParaWidgets.at(i).limitMax->getCurrentPort();
        m_AxisPar[i].circlePluseNum = machineParaWidgets.at(i).circlePluseNum->text().toInt();
        m_AxisPar[i].circleDis = machineParaWidgets.at(i).circleDis->text().toFloat()*100;
        m_AxisPar[i].findOriginSpeed = machineParaWidgets.at(i).findOriginSpeed->text().toInt();
        m_AxisPar[i].leaveOriginSpeed = machineParaWidgets.at(i).leaveOriginSpeed->text().toInt();
        m_AxisPar[i].originOffset = machineParaWidgets.at(i).originOffset->text().toFloat()*100;
        m_AxisPar[i].originDir = machineParaWidgets.at(i).originDir->currentIndex();
        m_AxisPar[i].originSignal = machineParaWidgets.at(i).originSignal->getCurrentPort();
        m_AxisPar[i].backOriginOrder = machineParaWidgets.at(i).backOriginOrder->currentIndex();
        m_AxisPar[i].originType = machineParaWidgets.at(i).originType->currentIndex();
        m_AxisPar[i].limitPosSwt = machineParaWidgets.at(i).limitPosSwt->currentIndex();
        m_AxisPar[i].limitNegSwt = machineParaWidgets.at(i).limitNegSwt->currentIndex();
        m_AxisPar[i].coordDir = machineParaWidgets.at(i).coordDir->currentIndex();
        setAxisPar(m_AxisPar[i],i);

    }
    setServoPar(m_ServoPar);

    std::array<std::vector<QString>, 3> tempStr;
    for(auto &vec : tempStr)
    {
        vec.reserve(AXIS_TOTAL_NUM);
    }
    for (int i = 0; i < AXIS_TOTAL_NUM; i++)
    {
        tempStr[0].emplace_back( machineParaWidgets.at(i).limitMin->text());
        tempStr[1].emplace_back(machineParaWidgets.at(i).limitMax->text());
        tempStr[2].emplace_back(machineParaWidgets.at(i).originSignal->text());
    }

    writeLimitSigDescription(0, tempStr[0]);    // 最大限位
    writeLimitSigDescription(1, tempStr[1]);    // 最小限位
    writeLimitSigDescription(2, tempStr[2]);    // 原点信号
    emit RefreshPortDefineSignals();//端口自定义界面刷新
    emit refreshManualReserve(); // 更新手动预留界面的按钮可用性
    emit WidgetNameRefresh_signal();//更新教导界面控件相关内容
    emit ManualDebugMachineRefresh_Signal(0);
    emit AxisTypeChange_Signal();
//    SetWidgetAxisQcomboboxHandel();
}

void Setting::saveMachineAllPara(int index)
{
    saveMachinePara();
    //btn1轴运动位置限定+轴参数
    //btn2加减速时间+最大速度
    //btn3限位开关+轴运动位置限定
    //btn4结构
    //btn5轴参数

    //index=0:限位开关（1条）+轴运动位置限定（6条）+轴参数（6条）
    //index=1:结构（1条）
    //index=2:原点（1条）+轴参数（6条）
    //index=3:伺服通信（1条）
    //index=4:结构（1条）
    //index=5:轴运动位置限定（6条）+最大速度+轴参数（6条）

    int i;
    switch(index)
    {
    case 0:
        g_Usart->ExtendSendParDeal(CMD_MAIN_MAC,CMD_SUN_MAC_LIMIT_SWT);
        QThread::msleep(10);
        for(i=0;i<AXIS_TOTAL_NUM;i++)
        {
            g_Usart->ExtendSendParDeal(CMD_MAIN_SP,CMD_SUN_SP_AXIS_LIMIT,i+1);
            QThread::msleep(10);
        }
        for(i=0;i<AXIS_TOTAL_NUM;i++)
        {
            g_Usart->ExtendSendParDeal(CMD_MAIN_MAC,CMD_SUN_MAC_AXIS,i+1);
            QThread::msleep(10);
        }
        M_SaveSetPar.SaveMachineParaLimit = false;
        break;
    case 1:
        g_Usart->ExtendSendParDeal(CMD_MAIN_MAC,CMD_SUN_MAC_STRUCT);
        SetWidgetAxisQcomboboxHandel();
        M_SaveSetPar.SaveMachineParaStruct = false;
        for(int i=0;i<4;i++)
        {
            M_SaveSetPar.ClawSafeFlag[i]=false;
            M_SaveSetPar.OnlineSafe[i]=false;
            M_SaveSetPar.SafeArea[i]=false;
        }
        break;
    case 2:
        for(i=0;i<AXIS_TOTAL_NUM;i++)
        {
            g_Usart->ExtendSendParDeal(CMD_MAIN_MAC,CMD_SUN_MAC_AXIS,i+1);
            QThread::msleep(10);
        }
        g_Usart->ExtendSendParDeal(CMD_MAIN_MAC,CMD_SUN_MAC_ORIGIN);
        M_SaveSetPar.SaveMachineParaOrigin = false;
        break;
    case 3:
    {
        uint16_t storageGroup=ui->editStorageGroup->text().toUInt();
        if (storageGroup <100)
        {
            for(int i=0;i<AXIS_TOTAL_NUM;i++)
            {
                if(m_NameDefine[1].axisName[i] == ui->coboxAxisNum->currentText())
                {
                    m_ServoCommPar[storageGroup].axis=i+1;
                }
            }
            m_ServoCommPar[storageGroup].index=ui->editParaIndex->text().toUInt();
            m_ServoCommPar[storageGroup].sunIndex=ui->editParaSunIndex->text().toUInt();
            m_ServoCommPar[storageGroup].parLen=ui->coboxParaLen->currentIndex()+1;
            m_ServoCommPar[storageGroup].parValue=ui->editParaValue->text().toInt();
            m_ServoCommPar[storageGroup].saveFlag=ui->coboxIsSave->currentIndex();
            m_ServoCommPar[storageGroup].powerOnSet=ui->coboxIsPowerSet->currentIndex();
            g_Usart->ExtendSendParDeal(CMD_MAIN_MAC,CMD_SUN_MAC_SERVO,storageGroup+1);
        }
        setServoCommPar(m_ServoCommPar);
    }
        break;
    case 4:
        g_Usart->ExtendSendParDeal(CMD_MAIN_MAC,CMD_SUN_MAC_STRUCT);
        M_SaveSetPar.SaveServoPara = false;
        break;
    case 5:
        for(i=0;i<AXIS_TOTAL_NUM;i++)
        {
            g_Usart->ExtendSendParDeal(CMD_MAIN_SP,CMD_SUN_SP_AXIS_LIMIT,i+1);
            QThread::msleep(10);
        }
        g_Usart->ExtendSendParDeal(CMD_MAIN_SERVO,CMD_SUN_SERVO_MAX_SPEED);
        QThread::msleep(10);
        for(i=0;i<AXIS_TOTAL_NUM;i++)
        {
            g_Usart->ExtendSendParDeal(CMD_MAIN_MAC,CMD_SUN_MAC_AXIS,i+1);
            QThread::msleep(10);
        }
        M_SaveSetPar.SaveAxisPara = false;
        break;
    default:
        break;
    }
}

//端口自定义刷新
void Setting::RefreshPortDefine()
{
    for(int i=0;i<INPUT_TOTAL_NUM;i++)
    {
        QTableWidgetItem *defineNameItem = ui->tableWgtPortDef->item(i,1);//默认名称栏
        QTableWidgetItem *modifyNameItem = ui->tableWgtPortDef->item(i,2);
        QTableWidgetItem *modifyPortItem = ui->tableWgtPortDef->item(i,3);
        if(m_Port_X[i].functionSet == 1)
        {//如果端口使用了指定功能，显示功能端口名称，如果未开启对应端口功能，则显示预留名称
             if(defineNameItem)
             {
                 defineNameItem->setText(m_Port_X[i].defineName);
             }
             if(modifyNameItem)
             {
                 modifyNameItem->setText(m_Port_X[i].modifyName);
             }
        }
        else if(m_Port_X[i].functionSet == 0)
        {
            if(defineNameItem)
            {
                defineNameItem->setText(m_Port_X[i].ResDefineName);
            }
            if(modifyNameItem)
            {
                modifyNameItem->setText(m_Port_X[i].ResModifyName);
            }
        }
        if(modifyPortItem)
        {
            modifyPortItem->setText(m_Port_X[i].modifyPort);
        }
    }
    for(int i=0;i<OUTPUT_TOTAL_NUM;i++)
    {
        QTableWidgetItem *defineNameItem = ui->tableWgtPortDef->item(INPUT_TOTAL_NUM+i,1);//默认名称栏
        QTableWidgetItem *modifyNameItem = ui->tableWgtPortDef->item(INPUT_TOTAL_NUM+i,2);//修改名称栏
        QTableWidgetItem *modifyPortItem = ui->tableWgtPortDef->item(INPUT_TOTAL_NUM+i,3);//修改端口栏
        if(m_Port_Y[i].functionSet == 1)
        {//如果端口使用了指定功能，显示功能端口名称，如果未开启对应端口功能，则显示预留名称
             if(defineNameItem)
             {
                 defineNameItem->setText(m_Port_Y[i].defineName);
             }
             if(modifyNameItem)
             {
                 modifyNameItem->setText(m_Port_Y[i].modifyName);
             }
        }
        else if(m_Port_Y[i].functionSet == 0)
        {
            if(defineNameItem)
            {
                defineNameItem->setText(m_Port_Y[i].ResDefineName);
            }
            if(modifyNameItem)
            {
                modifyNameItem->setText(m_Port_Y[i].ResModifyName);
            }
        }
        if(modifyPortItem)
        {
            modifyPortItem->setText(m_Port_Y[i].modifyPort);
        }
    }
    ui->tableWgtPortDef->repaint();//强制刷新列表
}
//名称自定义刷新
void Setting::RefreshNameDefine()
{
    for (int col = 0; col < 2; col++)
    {
        int index = 0;
        ui->tableWgtNameDef->item(index++, col)->setText(m_NameDefine[col].adminName);

        ui->tableWgtNameDef->item(index++, col)->setText(m_NameDefine[col].operatorName);

        for (int i = 0; i < 8; ++i) {
            auto item = ui->tableWgtNameDef->item(index, col);
            item->setText(m_NameDefine[col].subProgName[i]);
            ++index;
        }

        for (int i = 0; i < AXIS_TOTAL_NUM; ++i) {
            auto item = ui->tableWgtNameDef->item(index, col);
            item->setText(m_NameDefine[col].axisName[i]);
            ++index;
        }

        for (int i = 0; i < VAR_TOTAL_NUM; ++i) {
            auto item = ui->tableWgtNameDef->item(index, col);
            item->setText(m_NameDefine[col].varName[i]);
            ++index;
        }

        for (int i = 0; i < STACK_TOTAL_NUM; ++i) {
            auto item = ui->tableWgtNameDef->item(index, col);
            item->setText(m_NameDefine[col].stackName[i]);
            ++index;
        }

        for (int i = 0; i < FOLLOW_STACK_NUM; ++i) {
            auto item = ui->tableWgtNameDef->item(index, col);
            item->setText(m_NameDefine[col].followStackName[i]);
            ++index;
        }

        for (int i = 0; i < TIME_TOTAL_NUM; ++i) {
            auto item = ui->tableWgtNameDef->item(index, col);
            item->setText(m_NameDefine[col].timerName[i]);
            ++index;
        }
    }
}
//保存端口自定义
void Setting::savePortDefine()
{
    for(int i=0;i<INPUT_TOTAL_NUM;i++)
    {
        QTableWidgetItem *XmodifyNameItem = ui->tableWgtPortDef->item(i,2);
        QTableWidgetItem *XmodifyPortItem = ui->tableWgtPortDef->item(i,3);
        if(XmodifyNameItem)
        {
            if(m_Port_X[i].functionSet == 1)
            {
                m_Port_X[i].modifyName = XmodifyNameItem->text();
            }
            else
            {
                m_Port_X[i].ResModifyName = XmodifyNameItem->text();
            }
        }
        if(XmodifyPortItem)
        {
            m_Port_X[i].modifyPort = XmodifyPortItem->text();
            m_Port_X[i].actualPortNum = XmodifyPortItem->data(Qt::UserRole).toUInt();
        }
        m_InportFuncDefine[i]=m_Port_X[i].actualPortNum;
    }
    for(int i=0;i<OUTPUT_TOTAL_NUM;i++)
    {
        QTableWidgetItem *YmodifyNameItem = ui->tableWgtPortDef->item(INPUT_TOTAL_NUM+i,2);
        QTableWidgetItem *YmodifyPortItem = ui->tableWgtPortDef->item(INPUT_TOTAL_NUM+i,3);
        if(YmodifyNameItem)
        {
            if(m_Port_Y[i].functionSet == 1)
            {
                m_Port_Y[i].modifyName = YmodifyNameItem->text();
            }
            else
            {
                m_Port_Y[i].ResModifyName = YmodifyNameItem->text();
            }
        }
        if(YmodifyPortItem)
        {
            m_Port_Y[i].modifyPort = YmodifyPortItem->text();
            m_Port_Y[i].actualPortNum = YmodifyPortItem->data(Qt::UserRole).toUInt();
        }
        m_OutportFuncDefine[i] = m_Port_Y[i].actualPortNum;
    }
//    setPortDefineNameOrPortNum();
    ::writePortDefInfo();
    g_Usart->ExtendSendParDeal(CMD_MAIN_SIGNAL,CMD_SUN_SIGNAL_IN_FUNC_DEF);
    g_Usart->ExtendSendParDeal(CMD_MAIN_SIGNAL,CMD_SUN_SIGNAL_OUT_FUNC_DEF);
    emit monitor_port_refreash();
    emit WidgetNameRefresh_signal();
    emit updatemonitorhandcontrol();
    emit updateManualformButtonName_Signal();//更新手动界面按钮显示

    ui->btnSavePort->setParaChangedFlag(false);
    M_SaveSetPar.SavePort = false;
}

//保存名称自定义
void Setting::saveNameDefine()
{
    int index = 0;
    m_NameDefine[1].adminName = ui->tableWgtNameDef->item(index++, 1)->text();
    m_NameDefine[1].operatorName = ui->tableWgtNameDef->item(index++, 1)->text();
    for (int i = 0; i < 8; ++i) {
        auto item = ui->tableWgtNameDef->item(index, 1);
        m_NameDefine[1].subProgName[i] = item->text();
        ++index;
    }

    for (int i = 0; i < AXIS_TOTAL_NUM; ++i) {
        auto item = ui->tableWgtNameDef->item(index, 1);
        m_NameDefine[1].axisName[i] = item->text();
        ++index;
    }

    for (int i = 0; i < VAR_TOTAL_NUM; ++i) {
        auto item = ui->tableWgtNameDef->item(index, 1);
        m_NameDefine[1].varName[i] = item->text();
        ++index;
    }

    for (int i = 0; i < STACK_TOTAL_NUM; ++i) {
        auto item = ui->tableWgtNameDef->item(index, 1);
        m_NameDefine[1].stackName[i] = item->text();
        ++index;
    }

    for (int i = 0; i < FOLLOW_STACK_NUM; ++i) {
        auto item = ui->tableWgtNameDef->item(index, 1);
        m_NameDefine[1].followStackName[i] = item->text();
        ++index;
    }

    for (int i = 0; i < TIME_TOTAL_NUM; ++i) {
        auto item = ui->tableWgtNameDef->item(index, 1);
        m_NameDefine[1].timerName[i] = item->text();
        ++index;
    }
    ::writeNameDefine();
    // to fresh all name display in every windows.
    emit coboxVarSelectVarPreOpItemSet_signal();

    ui->btnSaveNameDef->setParaChangedFlag(false);
    M_SaveSetPar.SaveNameDef = false;
}
bool Setting::modifyPort(int row, int column)
{
    if(column == 2)
    {
        auto item = ui->tableWgtPortDef->item(row, column);
        QString originText = item->text();
        FullKeyboard* keyboard = FullKeyboard::instance();
        keyboard->setText(originText);
        keyboard->setCurrentEditObj(nullptr);
        keyboard->exec();
        auto inputText = keyboard->getInputText();
        if (originText != inputText)
        {
            item->setText(inputText);
            return true;
        }
    }
    else if(column == 3)
    {
        if(row < INPUT_TOTAL_NUM)
        {
            IOPortDialog dialog(nullptr, IOPortMode::IN);
            if (dialog.exec() == QDialog::Accepted)
            {
//                QString text = dialog.getIOOnlineIn();
//                m_Port_X[row].actualPortNum = static_cast<uint8_t>(dialog.getInputPort());
//                ui->tableWgtPortDef->item(row, column)->setText(text);
                QString portName = dialog.getIOOnlineIn();
                auto portNum = static_cast<uint8_t>(dialog.get_IOInEdit_InPort());

                auto item = ui->tableWgtPortDef->item(row, column);
                if (portNum == 0)
                {
                    portName = "0";
                    portNum = m_Port_X[row].portNum;
                }
                if (portNum != m_Port_X[row].actualPortNum)
                {
                    item->setText(portName);
                    item->setData(Qt::UserRole, portNum);
                    return true;
                }
            }
        }
        else
        {
            IOPortDialog dialog(nullptr, IOPortMode::OUT);
            if (dialog.exec() == QDialog::Accepted)
            {
                QString text = dialog.getIOOnlineOut();
//                m_Port_Y[row-INPUT_TOTAL_NUM].actualPortNum = static_cast<uint8_t>(dialog.getOutputPort());
//                ui->tableWgtPortDef->item(row, column)->setText(text);

                QString portName = dialog.getIOOnlineOut();
                auto portNum = static_cast<uint8_t>(dialog.get_IOInEdit_OutPort());
                auto item = ui->tableWgtPortDef->item(row, column);
                int index = row - INPUT_TOTAL_NUM;
                if (portNum == 0)
                {
                    portName = "0";
                    portNum = m_Port_Y[index].portNum;
                }
                if (portNum != m_Port_X[index].actualPortNum)
                {
                    item->setText(portName);
                    item->setData(Qt::UserRole, portNum);
                    return true;
                }
            }
        }
    }
    return false;
}

//替换logo处理
void Setting::on_btnLogoUpdate_clicked()
{
    uint8_t update_error = 0;
    if(Is_Udisk_In())
    {
        QString mountPoint = UsbDisk::instance()->getUsbMountPoint();
        QString bootlogoName = "bootlogo.bmp";
        QString stoplogoName = "stop.jpg";
        if(mountPoint != "")
        {

            QString result = UsbDisk::instance()->findFileAndGetDirectory(mountPoint, bootlogoName);
            if (result.isEmpty()) {
                qDebug()<<"no found file:"<<bootlogoName;
                update_error = 1;
            }
            else
            {
                QProcess_execute("mount",QStringList() <<"/dev/mmcblk0p1"<< "/mnt/");
                QProcess_execute("cp",QStringList() << result <<"/mnt/bootlogo.bmp");
                QProcess_execute("sync",QStringList());
                QProcess_execute("umount",QStringList() <<"/dev/mmcblk0p1");
            }

            QString result1 = UsbDisk::instance()->findFileAndGetDirectory(mountPoint, stoplogoName);
            if (result1.isEmpty()) {
                qDebug()<<"no found file:"<<stoplogoName;
                if(update_error == 0)
                {
                    update_error = 2;
                }
                else
                {
                    update_error = 3;
                }
            }
            else
            {
                QProcess_execute("cp",QStringList() << result1 <<"/opt/MachineTool/resources/pics/");
                QProcess_execute("sync",QStringList());
                emit LOGO_Refresh();
            }
            if(update_error == 1)
            {
                MainWindow::pMainWindow->showErrorTip(tr("未检测到bootlogo.bmp文件"));
                update_error = 0;
            }
            else if(update_error == 2)
            {
                MainWindow::pMainWindow->showErrorTip(tr("未检测到stop.jpg文件"));
                update_error = 0;
            }
            else if(update_error == 3)
            {
                MainWindow::pMainWindow->showErrorTip(tr("未检测到bootlogo.bmp和stop.jpg文件"));
                update_error = 0;
            }
        }
    }
    else
    {//提示u盘未插入
        MainWindow::pMainWindow->showErrorTip(tr("请插入U盘！"));
    }
}
/*************************************************************************
**  函数名：  UpgradeHandle()
**	输入参数：
**	输出参数：
**	函数功能：升级程序处理函数
**  作者：    wukui
**  开发日期：2024/10/24
**/
void Setting::UpgradeHandle(int click_type)
{
    if(Is_Udisk_In() || click_type == RESTSETTING)
    {
        UpgradeDialog = new upgradedialog(click_type);
        UpgradeDialog->setStyleSheet(this->styleSheet());
        UpgradeDialog->setModal(true);
        UpgradeDialog->setAttribute(Qt::WA_DeleteOnClose,true);//当对话框关闭时自动删除对话框
        UpgradeDialog->show();
        connect(UpgradeDialog,&upgradedialog::ImplementFinishSignal,this,[=](){
            switch(UpgradeDialog->sure_handle_type){
            case HANDHELD:
            {
                MainWindow::pMainWindow->showErrorTip(tr("手持器升级完成,正在重启中..."),TipMode::ONLY_OK);
                break;
            }
            case MAINBOARD:
            {
                MainWindow::pMainWindow->showErrorTip(tr("主板升级完成,正在重启中..."),TipMode::ONLY_OK);
                break;
            }
            case IOBOARD:
            {
                MainWindow::pMainWindow->showErrorTip(tr("IO板升级完成,正在重启中..."),TipMode::ONLY_OK);
                break;
            }
            case SERVO:
            {
                MainWindow::pMainWindow->showErrorTip(tr("伺服升级完成,正在重启中..."),TipMode::ONLY_OK);
                break;
            }
            case SYSTEM_DATA_COPY:
            {
                MainWindow::pMainWindow->showErrorTip(tr("数据备份成功!"),TipMode::ONLY_OK);
                break;
            }
            case COPY_DATA_REST:
            {
                int reply =   MainWindow::pMainWindow->showErrorTip(tr("数据还原成功!按确认按钮重启机器！"),TipMode::ONLY_OK);
                if (reply == QDialog::Accepted)
                {
                    ::sync();
                    system("reboot");
                }
                break;
            }
            default:
                break;
            }
            UpgradeDialog->close();
            ui->updata_widget->hide();
        });
        connect(UpgradeDialog,&upgradedialog::updataWidgetShowSignal,this,[=](){
            switch(UpgradeDialog->sure_handle_type){
            case HANDHELD:
            {
                ui->updata_label->setText(tr("提示 : 手持器升级中,切勿断电,请等待自动重启!"));
                break;
            }
            case MAINBOARD:
            {
                ui->updata_label->setText(tr("提示 : 主板升级中,切勿断电,请等待自动重启!"));
                break;
            }
            case IOBOARD:
            {
                ui->updata_label->setText(tr("提示 : IO板升级中,切勿断电,请等待自动重启!"));
                break;
            }
            case SERVO:
            {
                ui->updata_label->setText(tr("提示 : 伺服升级中,切勿断电,请等待自动重启!"));
                break;
            }
            case SYSTEM_DATA_COPY:
            {
                ui->updata_label->setText(tr("提示 : 数据备份中,切勿断电!"));
                break;
            }
            case COPY_DATA_REST:
            {
                ui->updata_label->setText(tr("提示 : 数据还原中,切勿断电!"));
                break;
            }
            default:
                break;
            }
            ui->updata_widget->show();
            this->update();
            QCoreApplication::processEvents();//调用该函数处理事件循环,立即显示
        });
        connect(UpgradeDialog,&upgradedialog::Upgrade_Main_State_signal,this,&Setting::Upgrade_Main_State_handle);
        connect(ui->Progress_bar,SIGNAL(valueChanged(int)),ui->Progress_num,SLOT(setValue(int)));
        connect(UpgradeDialog,&upgradedialog::UpProgressRefreshSignal,this,[=](){
            ui->Progress_bar->setValue(UpgradeDialog->Upgrade_Progress);
            this->update();
            QCoreApplication::processEvents();
        });
        connect(UpgradeDialog,&upgradedialog::Upgrade_Main_Progress_RefreshSignal,this,[=](){
            QTimer *ProgressTime = new QTimer();
            ProgressTime->start(100);
            connect(ProgressTime,&QTimer::timeout,this,[=](){
                ui->Progress_bar->setValue((static_cast<double>(M_MainUpdate.Current_Upgrade_Rate)/static_cast<double>(M_MainUpdate.Upgrade_all_Rate))*100.0);
            });
        });
    }
    else
    {
        MainWindow::pMainWindow->showErrorTip(tr("请插入U盘！"));
    }

}
//主控板升级状态处理函数
void Setting::Upgrade_Main_State_handle(uint8_t state)
{
    UpgradeDialog->close();
    ui->updata_widget->hide();
    static bool execute_status = false;
    if(state == 3 && execute_status == false)
    {
        execute_status = true;
        int reply = MainWindow::pMainWindow->showErrorTip(tr("升级成功！等待重启"));
        if(reply ==  QDialog::Accepted)
        {
            g_Usart->m_serialPort->clear(QSerialPort::AllDirections);// 清空缓冲区
            // 2. 关闭串口连接
            if (g_Usart->m_serialPort->isOpen()) {
                g_Usart->m_serialPort->close();
            }
            // 3. 断开所有相关信号槽
            disconnect(g_Usart->m_serialPort, nullptr, nullptr, nullptr);
            system("reboot");
        }
    }
    else if(state == 4 && execute_status== false)
    {
        execute_status = true;
        MainWindow::pMainWindow->showErrorTip(tr("升级失败，请重启机器再次升级！"),TipMode::NULL_BUTTON);
        ui->updata_widget->hide();
    }

}

bool Setting::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->tabUserSet && event->type() == QEvent::Show)
    {
        // fresh time display
        QDateTime currentDateTime = QDateTime::currentDateTime();

        int year = currentDateTime.date().year();
        int month = currentDateTime.date().month();
        int day = currentDateTime.date().day();
        int hour = currentDateTime.time().hour();
        int minute = currentDateTime.time().minute();
        int second = currentDateTime.time().second();

        ui->editYear->setText(QString::number(year));
        ui->editMonth->setText(QString::number(month));
        ui->editDay->setText(QString::number(day));
        ui->editHour->setText(QString::number(hour));
        ui->editMinute->setText(QString::number(minute));
        ui->editSecond->setText(QString::number(second));

        ui->editSystemName->setText(m_SystemSet.sysName);
        auto duration = m_SystemSet.backlightTime;
        ui->editBrightTime->setText(QString::number(duration));

        const std::array<QCheckBox*, 5> chboxColor = {
            ui->chboxColorDefault, ui->chboxColorOriange, ui->chboxColorYellow, ui->chboxColorGreen, ui->chboxBrown
        };
        auto index = m_SystemSet.sysColor;
        if (index < chboxColor.size()) {
            chboxColor[index]->setChecked(true);
        }

        return true;
    }

    return QWidget::eventFilter(watched, event);
}
//堆叠界面切换堆叠组
void Setting::on_tabWidgetStack_currentChanged(int index)
{
    // 这样写不行，后面写不通。tabWidgetStack 改变时的 index（对应界面上的顺序）不一定是 stack[i] 中对应的 i
    // 每个 stack[i] 中对应的 i 为它的成员变量 groupIndex，在初始化时已经赋值，后面不会修改
    // stack[index]->syncParaToUI() 为什么要重复调用该函数？初始化时参数已经调用进行了同步。
//    if(index<8)
//    {
//        stack[index]->updateGroupIndex(index);
//        stack[index]->syncParaToUI();
//    }
}
//轴参数界面参数刷新
void Setting::AxisParRefresh(uint8_t index)
{
    machineParaWidgets.at(index).axisType->setCurrentIndex(m_AxisPar[index].axisType);
    machineParaWidgets.at(index).axisMoveMade->setCurrentIndex(m_AxisPar[index].axisMoveMade);
    machineParaWidgets.at(index).accTime->setText(QString::number(m_AxisPar[index].accTime / 100.0, 'f', 2));
    machineParaWidgets.at(index).decTime->setText(QString::number(m_AxisPar[index].decTime / 100.0, 'f', 2));
    machineParaWidgets.at(index).accAcc->setText(QString::number(m_AxisPar[index].accAcc));
    machineParaWidgets.at(index).decDec->setText(QString::number(m_AxisPar[index].decDec));
    machineParaWidgets.at(index).maxSpeed->setText(QString::number(m_AxisPar[index].maxSpeed));
    machineParaWidgets.at(index).axisMinPos->setText(QString::number(m_AxisPar[index].axisMinPos / 100.0, 'f', 2));
    machineParaWidgets.at(index).axisMaxPos->setText(QString::number(m_AxisPar[index].axisMaxPos / 100.0, 'f', 2));
    machineParaWidgets.at(index).limitMin->setCurrentPort(m_AxisPar[index].minPosSignal);
    machineParaWidgets.at(index).limitMax->setCurrentPort(m_AxisPar[index].maxPosSignal);
    machineParaWidgets.at(index).circlePluseNum->setText(QString::number(m_AxisPar[index].circlePluseNum));
    machineParaWidgets.at(index).circleDis->setText(QString::number(m_AxisPar[index].circleDis / 100.0, 'f', 2));
    machineParaWidgets.at(index).findOriginSpeed->setText(QString::number(m_AxisPar[index].findOriginSpeed));
    machineParaWidgets.at(index).leaveOriginSpeed->setText(QString::number(m_AxisPar[index].leaveOriginSpeed));
    machineParaWidgets.at(index).originOffset->setText(QString::number(m_AxisPar[index].originOffset / 100.0, 'f', 2));
    machineParaWidgets.at(index).originDir->setCurrentIndex(m_AxisPar[index].originDir);
    machineParaWidgets.at(index).originSignal->setCurrentPort(m_AxisPar[index].originSignal);
    machineParaWidgets.at(index).backOriginOrder->setCurrentIndex(m_AxisPar[index].backOriginOrder);
    machineParaWidgets.at(index).originType->setCurrentIndex(m_AxisPar[index].originType);
    machineParaWidgets.at(index).limitPosSwt->setCurrentIndex(m_AxisPar[index].limitPosSwt);
    machineParaWidgets.at(index).limitNegSwt->setCurrentIndex(m_AxisPar[index].limitNegSwt);
    machineParaWidgets.at(index).coordDir->setCurrentIndex(m_AxisPar[index].coordDir);

    machineParaWidgets.at(index).limitMin->setText(minLimitStrs[index]);
    machineParaWidgets.at(index).limitMax->setText(maxLimitStrs[index]);
    machineParaWidgets.at(index).originSignal->setText(originSigStrs[index]);
}
//设置界面轴编号选择控件处理函数
void Setting::SetWidgetAxisQcomboboxHandel()
{
    ui->coboxAxisNum->clear();//机器参数界面-通信界面轴选择复选框
    ui->coboxAxisXClawSafe->clear();//卡爪安全界面-卡爪安全1-轴选择复选框
    ui->coboxAxisXClawSafe_2->clear();//卡爪安全界面-卡爪安全2-X轴选择复选框
    ui->coboxAxisXClawSafe_3->clear();//卡爪安全界面-卡爪安全3-X轴选择复选框
    ui->coboxAxisXClawSafe_4->clear();//卡爪安全界面-卡爪安全4-X轴选择复选框
    ui->coboxAxisZClawSafe->clear();//卡爪安全界面-卡爪安全1-Z轴选择复选框
    ui->coboxAxisZClawSafe_2->clear();//卡爪安全界面-卡爪安全2-Z轴选择复选框
    ui->coboxAxisZClawSafe_3->clear();//卡爪安全界面-卡爪安全3-Z轴选择复选框
    ui->coboxAxisZClawSafe_4->clear();//卡爪安全界面-卡爪安全4-Z轴选择复选框
    ui->coboxOnlineAxis->clear();//联机安全-联机区域1-联机轴选择复选框
    ui->coboxOnlineAxis_2->clear();//联机安全-联机区域2-联机轴选择复选框
    ui->coboxOnlineAxis_3->clear();//联机安全-联机区域3-联机轴选择复选框
    ui->coboxOnlineAxis_4->clear();//联机安全-联机区域4-联机轴选择复选框
    ui->coboxAxisXSA1->clear();//伺服安全点-安全区1-X轴-轴选择复选框
    ui->coboxAxisYSA1->clear();//伺服安全点-安全区1-Y轴-轴选择复选框
    ui->coboxAxisZSA1->clear();//伺服安全点-安全区1-Z轴-轴选择复选框
    ui->coboxAxisXSA1_2->clear();//伺服安全点-安全区2-X轴-轴选择复选框
    ui->coboxAxisYSA1_2->clear();//伺服安全点-安全区2-Y轴-轴选择复选框
    ui->coboxAxisZSA1_2->clear();//伺服安全点-安全区2-Z轴-轴选择复选框
    ui->coboxAxisXSA1_3->clear();//伺服安全点-安全区3-X轴-轴选择复选框
    ui->coboxAxisYSA1_3->clear();//伺服安全点-安全区3-Y轴-轴选择复选框
    ui->coboxAxisZSA1_3->clear();//伺服安全点-安全区3-Z轴-轴选择复选框
    ui->coboxAxisXSA1_4->clear();//伺服安全点-安全区4-X轴-轴选择复选框
    ui->coboxAxisYSA1_4->clear();//伺服安全点-安全区4-Y轴-轴选择复选框
    ui->coboxAxisZSA1_4->clear();//伺服安全点-安全区4-Z轴-轴选择复选框

    ui->coboxAxisXClawSafe->addItem(tr("无"));
    ui->coboxAxisXClawSafe_2->addItem(tr("无"));
    ui->coboxAxisXClawSafe_3->addItem(tr("无"));
    ui->coboxAxisXClawSafe_4->addItem(tr("无"));

    ui->coboxAxisZClawSafe->addItem(tr("无"));
    ui->coboxAxisZClawSafe_2->addItem(tr("无"));
    ui->coboxAxisZClawSafe_3->addItem(tr("无"));
    ui->coboxAxisZClawSafe_4->addItem(tr("无"));

    ui->coboxOnlineAxis->addItem(tr("无"));
    ui->coboxOnlineAxis_2->addItem(tr("无"));
    ui->coboxOnlineAxis_3->addItem(tr("无"));
    ui->coboxOnlineAxis_4->addItem(tr("无"));

    ui->coboxAxisXSA1->addItem(tr("无"));
    ui->coboxAxisYSA1->addItem(tr("无"));
    ui->coboxAxisZSA1->addItem(tr("无"));
    ui->coboxAxisXSA1_2->addItem(tr("无"));
    ui->coboxAxisYSA1_2->addItem(tr("无"));
    ui->coboxAxisZSA1_2->addItem(tr("无"));
    ui->coboxAxisXSA1_3->addItem(tr("无"));
    ui->coboxAxisYSA1_3->addItem(tr("无"));
    ui->coboxAxisZSA1_3->addItem(tr("无"));
    ui->coboxAxisXSA1_4->addItem(tr("无"));
    ui->coboxAxisYSA1_4->addItem(tr("无"));
    ui->coboxAxisZSA1_4->addItem(tr("无"));
    if(m_AxisPar[0].axisType == 1)
    {//如果轴类型为伺服，则显示对应轴
        ui->coboxAxisXClawSafe->addItem(m_NameDefine[1].axisName[0]);
        ui->coboxAxisXClawSafe_2->addItem(m_NameDefine[1].axisName[0]);
        ui->coboxAxisXClawSafe_3->addItem(m_NameDefine[1].axisName[0]);
        ui->coboxAxisXClawSafe_4->addItem(m_NameDefine[1].axisName[0]);

        ui->coboxOnlineAxis->addItem(m_NameDefine[1].axisName[0]);
        ui->coboxOnlineAxis_2->addItem(m_NameDefine[1].axisName[0]);
        ui->coboxOnlineAxis_3->addItem(m_NameDefine[1].axisName[0]);
        ui->coboxOnlineAxis_4->addItem(m_NameDefine[1].axisName[0]);

        ui->coboxAxisXSA1->addItem(m_NameDefine[1].axisName[0]);
        ui->coboxAxisXSA1_2->addItem(m_NameDefine[1].axisName[0]);
        ui->coboxAxisXSA1_3->addItem(m_NameDefine[1].axisName[0]);
        ui->coboxAxisXSA1_4->addItem(m_NameDefine[1].axisName[0]);

        ui->coboxAxisNum->addItem(m_NameDefine[1].axisName[0]);
    }
    if(m_AxisPar[1].axisType == 1)
    {//如果轴类型为伺服，则显示对应轴
        ui->coboxAxisYSA1->addItem(m_NameDefine[1].axisName[1]);
        ui->coboxAxisYSA1_2->addItem(m_NameDefine[1].axisName[1]);
        ui->coboxAxisYSA1_3->addItem(m_NameDefine[1].axisName[1]);
        ui->coboxAxisYSA1_4->addItem(m_NameDefine[1].axisName[1]);

        ui->coboxAxisNum->addItem(m_NameDefine[1].axisName[1]);
    }
    if(m_AxisPar[2].axisType == 1)
    {//如果轴类型为伺服，则显示对应轴
        ui->coboxAxisZClawSafe->addItem(m_NameDefine[1].axisName[2]);
        ui->coboxAxisZClawSafe_2->addItem(m_NameDefine[1].axisName[2]);
        ui->coboxAxisZClawSafe_3->addItem(m_NameDefine[1].axisName[2]);
        ui->coboxAxisZClawSafe_4->addItem(m_NameDefine[1].axisName[2]);

        ui->coboxAxisZSA1->addItem(m_NameDefine[1].axisName[2]);
        ui->coboxAxisZSA1_2->addItem(m_NameDefine[1].axisName[2]);
        ui->coboxAxisZSA1_3->addItem(m_NameDefine[1].axisName[2]);
        ui->coboxAxisZSA1_4->addItem(m_NameDefine[1].axisName[2]);

        ui->coboxAxisNum->addItem(m_NameDefine[1].axisName[2]);
    }
    if(m_AxisPar[3].axisType == 1)
    {//如果轴类型为伺服，则显示对应轴
        ui->coboxAxisNum->addItem(m_NameDefine[1].axisName[3]);
    }
    if(m_AxisPar[4].axisType == 1)
    {//如果轴类型为伺服，则显示对应轴
        ui->coboxAxisNum->addItem(m_NameDefine[1].axisName[4]);
    }
    if(m_AxisPar[5].axisType == 1)
    {//如果轴类型为伺服，则显示对应轴
        ui->coboxAxisZClawSafe->addItem(m_NameDefine[1].axisName[5]);
        ui->coboxAxisZClawSafe_2->addItem(m_NameDefine[1].axisName[5]);
        ui->coboxAxisZClawSafe_3->addItem(m_NameDefine[1].axisName[5]);
        ui->coboxAxisZClawSafe_4->addItem(m_NameDefine[1].axisName[5]);

        ui->coboxAxisZSA1->addItem(m_NameDefine[1].axisName[5]);
        ui->coboxAxisZSA1_2->addItem(m_NameDefine[1].axisName[5]);
        ui->coboxAxisZSA1_3->addItem(m_NameDefine[1].axisName[5]);
        ui->coboxAxisZSA1_4->addItem(m_NameDefine[1].axisName[5]);

        ui->coboxAxisNum->addItem(m_NameDefine[1].axisName[5]);
    }
}
