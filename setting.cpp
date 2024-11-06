#include "setting.h"
#include "ui_setting.h"

#include <QMovie>
#include <QPair>
#include <QPropertyAnimation>
#include <QDebug>
#include <QSignalMapper>
#include <QThread>

#include "mainwindow.h"
#include "upgradedialog.h"

#include "errortipdialog.h"
#include "stackedit.h"
#include "beeper.h"
#include "backlighter.h"
#include "usbdisk.h"
#include "timesetter.h"

const QString notePath = "/root/notepad/";

QVector<QString> registerCode;

Setting::Setting(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Setting)
{
    ui->setupUi(this);
    // this must be placed at beginning afer setupUI(this).
    for (int i = 0; i < 8; i++)
    {
        stack[i] = new StackEdit(ui->tabWidgetStack->widget(i));
        stack[i]->updateGroupIndex(i);
    }

    init();
    initWidgets();

    syncParaToUI();

    pageSwitchInit();
    setupCommunicationConnections();

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

//    connect(ui->btnSavePasswdAdmin, &QPushButton::clicked, [=]() {
//       NumberEdit* const edits[] = {ui->editAdminOldPasswd, ui->editAdminNewPasswd, ui->editAdminNewPasswdConfirm};
//        if (edits[1]->text().isEmpty())
//            return;
//        uint oldPasswd = edits[0]->text().toUInt();
//        uint newPasswd = edits[1]->text().toUInt();
//        uint confirmPasswd = edits[2]->text().toUInt();

//        if (passwd[0] != oldPasswd)
//        {
//            MainWindow::pMainWindow->showErrorTip(tr("旧密码错误！"), TipMode::ONLY_OK);
//            return;
//        }

//        if (newPasswd != confirmPasswd)
//        {
//            MainWindow::pMainWindow->showErrorTip(tr("两次输入密码不一致！"), TipMode::ONLY_OK);
//            return;
//        }

//        if (passwd[0] == oldPasswd && newPasswd == confirmPasswd)
//        {
//            passwd[0] = newPasswd;

//            // save the passwd to configuration file

//            MainWindow::pMainWindow->showErrorTip(tr("密码修改成功！"), TipMode::ONLY_OK);
//            for (auto edit : edits)
//            {
//                edit->clear();
//            }
//        }

//    });
//    connect(ui->btnSavePasswdSuperAdmin, &QPushButton::clicked, [=]() {
//       NumberEdit* const edits[] = {ui->editSuperAdminOldPasswd, ui->editSuperAdminNewPasswd, ui->editSuperAdminNewPasswdConfirm};
//        if (edits[1]->text().isEmpty())
//            return;
//        uint oldPasswd = edits[0]->text().toUInt();
//        uint newPasswd = edits[1]->text().toUInt();
//        uint confirmPasswd = edits[2]->text().toUInt();

//        if (passwd[1] != oldPasswd)
//        {
//            MainWindow::pMainWindow->showErrorTip(tr("旧密码错误！"), TipMode::ONLY_OK);
//            return;
//        }

//        if (newPasswd != confirmPasswd)
//        {
//            MainWindow::pMainWindow->showErrorTip(tr("两次输入密码不一致！"), TipMode::ONLY_OK);
//            return;
//        }

//        if (passwd[1] == oldPasswd && newPasswd == confirmPasswd)
//        {
//            passwd[1] = newPasswd;

//            // save the passwd to configuration file

//            MainWindow::pMainWindow->showErrorTip(tr("密码修改成功！"), TipMode::ONLY_OK);
//            for (auto edit : edits)
//            {
//                edit->clear();
//            }
//        }

//    });
//    connect(ui->btnSavePasswdMenu, &QPushButton::clicked, [=]() {
//       NumberEdit* const edits[] = {ui->editMenuOldPasswd, ui->editMenuNewPasswd, ui->editMenuNewPasswdConfirm};
//        if (edits[1]->text().isEmpty())
//            return;
//        uint oldPasswd = edits[0]->text().toUInt();
//        uint newPasswd = edits[1]->text().toUInt();
//        uint confirmPasswd = edits[2]->text().toUInt();

//        if (passwd[2] != oldPasswd)
//        {
//            MainWindow::pMainWindow->showErrorTip(tr("旧密码错误！"), TipMode::ONLY_OK);
//            return;
//        }

//        if (newPasswd != confirmPasswd)
//        {
//            MainWindow::pMainWindow->showErrorTip(tr("两次输入密码不一致！"), TipMode::ONLY_OK);
//            return;
//        }

//        if (passwd[2] == oldPasswd && newPasswd == confirmPasswd)
//        {
//            passwd[2] = newPasswd;

//            // save the passwd to configuration file

//            MainWindow::pMainWindow->showErrorTip(tr("密码修改成功！"), TipMode::ONLY_OK);
//            for (auto edit : edits)
//            {
//                edit->clear();
//            }
//        }

//    });

    ui->editBrightTime->setInputRange(30, 65535);
    connect(ui->editBrightTime, &NumberEdit::textChanged, [=](const QString& val){
        int second = val.toInt();
        if (second < 30 && second > 65535)
            return;
        BackLighter::instance()->setScreenOffTime(second);
//        qDebug() << "time of screenOff:" << val;
    });
    connect(ui->sliderBrightness, &QSlider::valueChanged, [=](int value){
//        qDebug() << "value of slider:" << value;
        BackLighter::instance()->setBrightness(value);
        ui->percentageBrightness->setValue(value);
//        qDebug() << "brightness:" << BackLighter::instance()->getBrightness();
    });

    connect(ui->chboxBuzzer, &QCheckBox::stateChanged, [=](int state){
        bool enable = state;
        Beeper::instance()->setEnable(enable);
    });

    connect(ui->btnImportPortDef, &QPushButton::clicked, [=](){
        if (UsbDisk::instance()->isInserted() == false)
        {
            ErrorTipDialog tip(this);
            tip.setMessage(tr("请插入U盘！"));
            tip.exec();
            return;
        }
//       UsbDisk::instance()->copyFromUsb("test.sh", "/root/test.sh");
    });
    ui->tabUserSet->installEventFilter(this);
    connect(ui->btnSaveTime, &QPushButton::clicked, [=](){

        ErrorTipDialog tip(tr("确定修改系统时间？"), this);
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
             ErrorTipDialog tip(tr("时间格式错误！"), TipMode::ONLY_OK, this);
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
            auto mode = (StackMode)(index);
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

    //PXC_240914
    showPortDefine();
    connect(ui->btnSavePort,&QPushButton::clicked,this,&Setting::savePortDefine);
    connect(ui->tableWgtPortDef,&QTableWidget::cellClicked,[=](int row, int column){
        modifyPort(row, column);
    });
    /***************************升级与备份-信号槽连接**********************************/
    connect(ui->btnUpdateHandcontroller, &QPushButton::clicked,this,[=]() {this->UpgradeHandle(HANDHELD);});
    connect(ui->btnUpdateIOBoard, &QPushButton::clicked,this,[=]() {this->UpgradeHandle(IOBOARD);});
    connect(ui->btnUpdateMainboard, &QPushButton::clicked,this,[=]() {this->UpgradeHandle(MAINBOARD);});
    connect(ui->btnUpdateServo, &QPushButton::clicked,this,[=]() {this->UpgradeHandle(SERVO);});
    connect(ui->btnSysDataBackup,&QPushButton::clicked,this,[=](){this->UpgradeHandle(SYSTEM_DATA_COPY);});
    connect(ui->btnBackupDataRestore,&QPushButton::clicked,this,[=](){this->UpgradeHandle(COPY_DATA_REST);});
    connect(ui->btnFactoryDataReset,&QPushButton::clicked,this,[=](){this->UpgradeHandle(RESTSETTING);});
}

Setting::~Setting()
{
    delete ui;
}

void Setting::slotSettingHome()
{
    ui->stackedWidget->setCurrentIndex(0);
//    ui->stackedWidget->setCurrentWidget(ui->pageSettingHome);
    //    qDebug() << "slotSettingHome";
}

void Setting::ShowStackPage()
{
    ui->stackedWidget->setCurrentWidget(ui->pageStack);
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

    // fresh backlight time dispaly
    int duration = BackLighter::instance()->getScreenOffTime();
    ui->editBrightTime->setText(QString::number(duration));

    // fresh backlight brightness dispaly
    int level = BackLighter::instance()->getBrightness();
    ui->sliderBrightness->setValue(level);

    ui->labClawSafePic->setPixmap(QPixmap(":/images/settingPageImages/claw_safe.png"));
    ui->labOnlineSafePic->setPixmap(QPixmap(":/images/settingPageImages/online_safe.png"));


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

    ui->updata_widget->hide();

    ui->tableWgtPortDef->horizontalHeader()->setVisible(true);
    ui->tableWgtPortDef->verticalHeader()->setVisible(true);
    ui->tableWgtPortDef->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWgtPortDef->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWgtPortDef->setEditTriggers(QAbstractItemView::NoEditTriggers);

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

    QDir dirNote(notePath);
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

void Setting::readFromConfigFile()
{
    // 信号设置
}

void Setting::writeToConfigFile()
{

//    setOutPortType(m_OutPortType[0]);
    //    setOutPortType(m_OutPortType[OUT_PORT_TYPE_NUM]);
}

void Setting::syncParaToUI()
{
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
//            if (m_OutportInterlock[i][j])
//            {
//                outportInterlockList[4*i+j]->setChecked(true);
//            }
//            else
//            {
//                outportInterlockList[4*i+j]->setChecked(false);
//            }
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
    for (int i = 0; i < OPR_KEY_NUM; i++)
    {
        keyEdits.at(i)->setKeyFunc(m_KeyFunc[i].keyType, m_KeyFunc[i].funcNum, m_KeyFunc[i].oprMode);
        //
//        QString keyFunStr = getKeyFuncDescription(m_KeyFunc[i].keyType, m_KeyFunc[i].funcNum, m_KeyFunc[i].oprMode);
//        keyEdits.at(i)->setText(keyFunStr);
    }
    for (int i = 0; i < OPR_LED_NUM; i++)
    {
        sigEdits.at(i)->setSigTypeAndPort(m_LedFunc[i].ledType, m_LedFunc[i].funcNum);

//        QString sigStr = getSigDescription(m_LedFunc[i].ledType, m_LedFunc[i].funcNum);
//        sigEdits.at(i)->setText(sigStr);
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
        clawSafeWidgets.at(i).clawKeepoutSta->setCurrentIndex(m_ClawSave[i].clawKeepoutSta);

        int axisXItemIndex = 0;
        if (m_ClawSave[i].axisSelect_X == X1_AXIS+1)
        {
            axisXItemIndex = 1;
        }
        clawSafeWidgets.at(i).axisSelect_X->setCurrentIndex(axisXItemIndex);

        int axisZItemIndex = 0;
        if(m_ClawSave[i].axisSelect_Z == Z1_AXIS + 1)
        {
            axisZItemIndex = 1;
        }
        else if (m_ClawSave[i].axisSelect_Z == Z2_AXIS + 1)
        {
            axisZItemIndex = 2;
        }
        clawSafeWidgets.at(i).axisSelect_X->setCurrentIndex(axisZItemIndex);

        clawSafeWidgets.at(i).clawKeepoutMinX->setText(QString::number(m_ClawSave[i].clawKeepoutMinX / 100.0, 'f', 2));
        clawSafeWidgets.at(i).clawKeepoutMaxX->setText(QString::number(m_ClawSave[i].clawKeepoutMaxX / 100.0, 'f', 2));
        clawSafeWidgets.at(i).clawKeepoutHighZ->setText(QString::number(m_ClawSave[i].clawKeepoutHighZ / 100.0, 'f', 2));

        clawSafeWidgets.at(i).cKeepoutMinX->setText(QString::number(m_ClawSave[i].cKeepoutMinX / 100.0, 'f', 2));
        clawSafeWidgets.at(i).cKeepoutMaxX->setText(QString::number(m_ClawSave[i].cKeepoutMaxX / 100.0, 'f', 2));
        clawSafeWidgets.at(i).cKeepoutHighZ->setText(QString::number(m_ClawSave[i].cKeepoutHighZ / 100.0, 'f', 2));
        clawSafeWidgets.at(i).cKeepoutPosMinC->setText(QString::number(m_ClawSave[i].cKeepoutPosMinC / 100.0, 'f', 2));
        clawSafeWidgets.at(i).cKeepoutPosMaxC->setText(QString::number(m_ClawSave[i].cKeepoutPosMaxC / 100.0, 'f', 2));
        clawSafeWidgets.at(i).cKeepoutNegMinC->setText(QString::number(m_ClawSave[i].cKeepoutNegMinC / 100.0, 'f', 2));
        clawSafeWidgets.at(i).cKeepoutNegMaxC->setText(QString::number(m_ClawSave[i].cKeepoutNegMaxC / 100.0, 'f', 2));

    }

    /****************************联机安全********************************************/
    for (size_t i = 0; i < onlineSafeWidgets.size(); i++)
    {
        onlineSafeWidgets.at(i).mainSunMode->setCurrentIndex(m_OnlineSave[i].mainSunMode);
        onlineSafeWidgets.at(i).pluseTime->setText(QString::number(m_OnlineSave[i].pluseTime / 100.0, 'f', 2));
        onlineSafeWidgets.at(i).onlineSelect->setCurrentIndex(m_OnlineSave[i].onlineSelect);

        int onlineAxisIndex = 0;
        if (m_OnlineSave[i].axisNum == 1)
        {
            onlineAxisIndex = 1;
        }
        onlineSafeWidgets.at(i).axisNum->setCurrentIndex(onlineAxisIndex);

        onlineSafeWidgets.at(i).areaInNum->setCurrentPort(m_OnlineSave[i].areaInNum);
        onlineSafeWidgets.at(i).areaOutNum->setCurrentPort(m_OnlineSave[i].areaOutNum);
        onlineSafeWidgets.at(i).requestInNum->setCurrentPort(m_OnlineSave[i].requestInNum);
        onlineSafeWidgets.at(i).requestOutNum->setCurrentPort(m_OnlineSave[i].requestOutNum);

        onlineSafeWidgets.at(i).a1A2MainPos->setText(QString::number(m_OnlineSave[i].a1A2MainPos / 100.0, 'f', 2));
        onlineSafeWidgets.at(i).a1A2SunPos->setText(QString::number(m_OnlineSave[i].a1A2SunPos / 100.0, 'f', 2));
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
    servoPara.at(0)->setCurrentIndex(m_ServoPar.encoderType);
    servoPara.at(1)->setCurrentIndex(m_ServoPar.encoderBitNum);
    servoPara.at(2)->setCurrentIndex(m_ServoPar.servoBrand);
    servoPara.at(3)->setCurrentIndex(m_ServoPar.servoModel);
    servoPara.at(4)->setCurrentIndex(m_ServoPar.extendIOBoard);
    servoPara.at(5)->setCurrentIndex(m_ServoPar.rotateAxisUnit);
    servoPara.at(6)->setCurrentIndex(m_ServoPar.resetSunProFlag);
    servoPara.at(7)->setCurrentIndex(m_ServoPar.torqueLimitFlag);

    /****************************机器参数********************************************/
    for (int i = 0; i < AXIS_TOTAL_NUM; i++)
    {
        machineParaWidgets.at(i).axisType->setCurrentIndex(m_AxisPar[i].axisType);
        machineParaWidgets.at(i).axisMoveMade->setCurrentIndex(m_AxisPar[i].axisMoveMade);
        machineParaWidgets.at(i).accTime->setText(QString::number(m_AxisPar[i].accTime / 100.0));
        machineParaWidgets.at(i).decTime->setText(QString::number(m_AxisPar[i].decTime / 100.0));
        machineParaWidgets.at(i).accAcc->setText(QString::number(m_AxisPar[i].accAcc));
        machineParaWidgets.at(i).decDec->setText(QString::number(m_AxisPar[i].decDec));
        machineParaWidgets.at(i).maxSpeed->setText(QString::number(m_AxisPar[i].maxSpeed));
        machineParaWidgets.at(i).axisMinPos->setText(QString::number(m_AxisPar[i].axisMinPos / 100.0));
        machineParaWidgets.at(i).axisMaxPos->setText(QString::number(m_AxisPar[i].axisMaxPos / 100.0));
        machineParaWidgets.at(i).limitMin->setCurrentPort(m_AxisPar[i].minPosSignal);
        machineParaWidgets.at(i).limitMax->setCurrentPort(m_AxisPar[i].maxPosSignal);
        machineParaWidgets.at(i).circlePluseNum->setText(QString::number(m_AxisPar[i].circlePluseNum));
        machineParaWidgets.at(i).circleDis->setText(QString::number(m_AxisPar[i].circleDis / 100.0));
        machineParaWidgets.at(i).findOriginSpeed->setText(QString::number(m_AxisPar[i].findOriginSpeed));
        machineParaWidgets.at(i).leaveOriginSpeed->setText(QString::number(m_AxisPar[i].leaveOriginSpeed));
        machineParaWidgets.at(i).originOffset->setText(QString::number(m_AxisPar[i].originOffset / 100.0));
        machineParaWidgets.at(i).originDir->setCurrentIndex(m_AxisPar[i].originDir);
        machineParaWidgets.at(i).originSignal->setCurrentPort(m_AxisPar[i].originSignal);
        machineParaWidgets.at(i).backOriginOrder->setCurrentIndex(m_AxisPar[i].backOriginOrder);
        machineParaWidgets.at(i).originType->setCurrentIndex(m_AxisPar[i].originType);
        machineParaWidgets.at(i).limitPosSwt->setCurrentIndex(m_AxisPar[i].limitPosSwt);
        machineParaWidgets.at(i).limitNegSwt->setCurrentIndex(m_AxisPar[i].limitNegSwt);
        machineParaWidgets.at(i).coordDir->setCurrentIndex(m_AxisPar[i].coordDir);

    }

    /****************************安全区********************************************/
    for (int index = 0; index < SAVE_AREA_NUM; index++)
    {
        // 设置开关和完成进程数
        servoPointSafeArea.at(index).safeAreaSwt->setCurrentIndex(m_SaveArea[index].saveAreaSwt);
        servoPointSafeArea.at(index).processFinishNum->setCurrentIndex(m_SaveArea[index].processFinishNum);

        // X 轴
        servoPointSafeArea.at(index).axisSelect[0]->setCurrentIndex(m_SaveArea[index].axisSelect[0]);

        // Y 轴
        int yAxis = m_SaveArea[index].axisSelect[1];
        if (yAxis == 0) {
            servoPointSafeArea.at(index).axisSelect[1]->setCurrentIndex(0);
        }
        else if (yAxis == 2) { // Y1轴
            servoPointSafeArea.at(index).axisSelect[1]->setCurrentIndex(1);
        }
        else if (yAxis == 5) { // Y2轴
            servoPointSafeArea.at(index).axisSelect[1]->setCurrentIndex(2);
        }

        // Z 轴
        int zAxis = m_SaveArea[index].axisSelect[2];
        if (zAxis == 0) {
            servoPointSafeArea.at(index).axisSelect[2]->setCurrentIndex(0);
        }
        else if (zAxis == 3) {
            servoPointSafeArea.at(index).axisSelect[2]->setCurrentIndex(1);
        }
        else if (zAxis == 6) {
            servoPointSafeArea.at(index).axisSelect[2]->setCurrentIndex(2);
        }

        // 第三组用不上 默认传为0
        for (int i = 0; i < 2; i++)
        {
            servoPointSafeArea.at(index).machineA1Pos[i]->setText(
                        QString::number(m_SaveArea[index].machineA1Pos[i] / 100.0));
            servoPointSafeArea.at(index).machineA2Pos[i]->setText(
                        QString::number(m_SaveArea[index].machineA2Pos[i] / 100.0));
            servoPointSafeArea.at(index).starckB1Pos[i]->setText(
                        QString::number(m_SaveArea[index].starckB1Pos[i] / 100.0));
            servoPointSafeArea.at(index).starckB2Pos[i]->setText(
                        QString::number(m_SaveArea[index].starckB2Pos[i] / 100.0));
        }
        servoPointSafeArea.at(index).machineA1Pos[SAVE_Z_AXIS]->setText(
                    QString::number(m_SaveArea[index].machineA1Pos[SAVE_Z_AXIS] / 100.0));
        servoPointSafeArea.at(index).machineA2Pos[SAVE_Z_AXIS]->setText(
                    QString::number(m_SaveArea[index].machineA2Pos[SAVE_Z_AXIS] / 100.0));

        servoPointSafeArea.at(index).machine_Z_WaitMaxPos->setText(
                    QString::number(m_SaveArea[index].machine_Z_WaitMaxPos / 100.0));
        servoPointSafeArea.at(index).machine_Z_FallMaxPos->setText(
                    QString::number(m_SaveArea[index].machine_Z_FallMaxPos / 100.0));
        servoPointSafeArea.at(index).machine_Z_InsideHigh->setText(
                    QString::number(m_SaveArea[index].machine_Z_InsideHigh / 100.0));
        servoPointSafeArea.at(index).stack_Z_StartMaxPos->setText(
                    QString::number(m_SaveArea[index].stack_Z_StartMaxPos / 100.0));
        servoPointSafeArea.at(index).stack_Z_FallMaxPos->setText(
                    QString::number(m_SaveArea[index].stack_Z_FallMaxPos / 100.0));
    }

    /****************************伺服安全点--位置限定********************************************/
    toleranceList[0]->setText(QString::number(m_DegreeParS.tolerance / 100.0));
    toleranceList[1]->setText(QString::number(m_DegreeParS.originDegree / 100.0));
    toleranceList[2]->setText(QString::number(m_DegreeParS.actionFinDegree / 100.0));

    horizontalPosList[0]->setText(QString::number(m_DegreeParS.saveRampageHige_Z1 / 100.0));
    horizontalPosList[1]->setText(QString::number(m_DegreeParS.saveRampageHige_Z2 / 100.0));
    horizontalPosList[2]->setText(QString::number(m_DegreeParS.saveRotatePos_Y1 / 100.0));
    horizontalPosList[3]->setText(QString::number(m_DegreeParS.saveRotatePos_Y2 / 100.0));

    /****************************堆叠设置********************************************/
    for (int i = 0; i < 8; i++)
    {
        stack[i]->syncParaToUI();
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

    *passwd = newPasswd;

    // save the passwd to configuration file
    ::savePasswdToConfig(suffix, *passwd);

    MainWindow::pMainWindow->showErrorTip(tr("密码修改成功！"), TipMode::ONLY_OK);
    for (auto edit : edits) {
        edit->clear();
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
    });
    /***************************升级与备份-信号槽连接**********************************/
    connect(ui->btnUpdateHandcontroller, &QPushButton::clicked,this,[=]() {this->UpgradeHandle(0);});
    connect(ui->btnUpdateIOBoard, &QPushButton::clicked,this,[=]() {this->UpgradeHandle(2);});
    connect(ui->btnUpdateMainboard, &QPushButton::clicked,this,[=]() {this->UpgradeHandle(1);});
    connect(ui->btnUpdateServo, &QPushButton::clicked,this,[=]() {this->UpgradeHandle(3);});
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

//    {
//        QCheckBox* chboxs[] = {
//            ui->chbox0InterlockGroup0,
//            ui->chbox0InterlockGroup1,
//            ui->chbox0InterlockGroup2,
//            ui->chbox0InterlockGroup3,
//            ui->chbox0InterlockGroup4,
//            ui->chbox0InterlockGroup5,
//            ui->chbox0InterlockGroup6,
//            ui->chbox0InterlockGroup7,
//            ui->chbox0InterlockGroup8,
//            ui->chbox0InterlockGroup9,
//            ui->chbox0InterlockGroup10,
//            ui->chbox0InterlockGroup11,
//        };
//        for (auto chbox : chboxs)
//        {
//            chbox->stateChanged(chbox->checkState());
//        }
//    }


    /***************************************System setting part**********************************************************/

    connect(ui->tableWgtNote, &QTableWidget::currentCellChanged, this, [=](int row, int){
        if (row >= 0) {
            // 获取第一列的标题内容
            QString title = ui->tableWgtNote->item(row, 0)->text();
            ui->editNoteTitle->setText(title);

            // 构建文件名
            QString fileName = notePath + title + ".txt";

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

            QRegularExpression illegalChars("[<>:\"/\\|?*]");
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
            QString newFileName = notePath + newTitle + ".txt";

            QString title = ui->tableWgtNote->item(currentRow, 0)->text();
            QString fileName = notePath + title + ".txt";

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

       int rowCount = ui->tableWgtNote->rowCount();

       QString newTitle = QString("untitled%1").arg(rowCount + 1);
       QString newFileName = notePath + newTitle + ".txt";

       int count = 1;
       while (QFile::exists(newFileName)) {
           newFileName = notePath + QString("untitled%1").arg(rowCount + 1 + count) + ".txt";
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
            QString fileName = notePath + title + ".txt";

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
    /************************************************************************/

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
            QGroupBox* grbox3, QGroupBox* grbox4, bool enabled)
    {
        connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index){

            grbox1->setEnabled(index);
            grbox2->setEnabled(index);

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
    auto connectAxisZ = [&](QComboBox* comboBox, QGroupBox* groupBox1, QGroupBox* groupBox2)
    {
        connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index){
            groupBox1->setEnabled(index);
            groupBox2->setEnabled(index);
        });
    };

    // 安全区1
    connectComboBoxToGroupBoxes(ui->coboxSafeArea1, ui->grboxAxisCombineSA1Arm2, ui->grboxXYParaSA1,
                                ui->grboxZAxisMachineParaSA1, ui->grboxZAxisStockBinParaSA1,
                                ui->coboxAxisZSA1->currentIndex() != 0);
    connectAxisX(ui->coboxAxisXSA1, ui->labAxisXSelectSA1, ui->labAxisCSelectSA1,
                 ui->editMachineA1AxisXSA1, ui->editMachineA2AxisXSA1,
                 ui->editStockBinB1AxisXSA1, ui->editStockBinB2AxisXSA1,
                 ui->editMachineA1AxisCSA1, ui->editMachineA2AxisCSA1);
    connectAxisY(ui->coboxAxisYSA1, ui->labAxisYSelectSA1,
                 ui->editMachineA1AxisYSA1, ui->editMachineA2AxisYSA1,
                 ui->editStockBinB1AxisYSA1, ui->editStockBinB2AxisYSA1);
    connectAxisZ(ui->coboxAxisZSA1, ui->grboxZAxisMachineParaSA1, ui->grboxZAxisStockBinParaSA1);

    // 安全区2
    connectComboBoxToGroupBoxes(ui->coboxSafeArea1_2, ui->grboxAxisCombineSA1Arm2_2, ui->grboxXYParaSA1_2,
                                ui->grboxZAxisMachineParaSA1_2, ui->grboxZAxisStockBinParaSA1_2,
                                ui->coboxAxisZSA1_2->currentIndex() == 0);
    connectAxisX(ui->coboxAxisXSA1_2, ui->labAxisXSelectSA1_2, ui->labAxisCSelectSA1_2,
                 ui->editMachineA1AxisXSA1_2, ui->editMachineA2AxisXSA1_2,
                 ui->editStockBinB1AxisXSA1_2, ui->editStockBinB2AxisXSA1_2,
                 ui->editMachineA1AxisCSA1_2, ui->editMachineA2AxisCSA1_2);
    connectAxisY(ui->coboxAxisYSA1_2, ui->labAxisYSelectSA1_2,
                 ui->editMachineA1AxisYSA1_2, ui->editMachineA2AxisYSA1_2,
                 ui->editStockBinB1AxisYSA1_2, ui->editStockBinB2AxisYSA1_2);
    connectAxisZ(ui->coboxAxisZSA1_2, ui->grboxZAxisMachineParaSA1_2, ui->grboxZAxisStockBinParaSA1_2);

    // 安全区3
    connectComboBoxToGroupBoxes(ui->coboxSafeArea1_3, ui->grboxAxisCombineSA1Arm2_3, ui->grboxXYParaSA1_3,
                                ui->grboxZAxisMachineParaSA1_3, ui->grboxZAxisStockBinParaSA1_3,
                                ui->coboxAxisZSA1_3->currentIndex() == 0);
    connectAxisX(ui->coboxAxisXSA1_3, ui->labAxisXSelectSA1_3, ui->labAxisCSelectSA1_3,
                 ui->editMachineA1AxisXSA1_3, ui->editMachineA2AxisXSA1_3,
                 ui->editStockBinB1AxisXSA1_3, ui->editStockBinB2AxisXSA1_3,
                 ui->editMachineA1AxisCSA1_3, ui->editMachineA2AxisCSA1_3);
    connectAxisY(ui->coboxAxisYSA1_3, ui->labAxisYSelectSA1_3,
                 ui->editMachineA1AxisYSA1_3, ui->editMachineA2AxisYSA1_3,
                 ui->editStockBinB1AxisYSA1_3, ui->editStockBinB2AxisYSA1_3);
    connectAxisZ(ui->coboxAxisZSA1_3, ui->grboxZAxisMachineParaSA1_3, ui->grboxZAxisStockBinParaSA1_3);

    // 安全区4
    connectComboBoxToGroupBoxes(ui->coboxSafeArea1_4, ui->grboxAxisCombineSA1Arm2_4, ui->grboxXYParaSA1_4,
                                ui->grboxZAxisMachineParaSA1_4, ui->grboxZAxisStockBinParaSA1_4,
                                ui->coboxAxisZSA1_4->currentIndex() == 0);
    connectAxisX(ui->coboxAxisXSA1_4, ui->labAxisXSelectSA1_4, ui->labAxisCSelectSA1_4,
                 ui->editMachineA1AxisXSA1_4, ui->editMachineA2AxisXSA1_4,
                 ui->editStockBinB1AxisXSA1_4, ui->editStockBinB2AxisXSA1_4,
                 ui->editMachineA1AxisCSA1_4, ui->editMachineA2AxisCSA1_4);
    connectAxisY(ui->coboxAxisYSA1_4, ui->labAxisYSelectSA1_4,
                 ui->editMachineA1AxisYSA1_4, ui->editMachineA2AxisYSA1_4,
                 ui->editStockBinB1AxisYSA1_4, ui->editStockBinB2AxisYSA1_4);
    connectAxisZ(ui->coboxAxisZSA1_4, ui->grboxZAxisMachineParaSA1_4, ui->grboxZAxisStockBinParaSA1_4);

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

# if 0
void Setting::machineParaLogic()
{
    auto setupAxisTypeConnections = [&](QComboBox* comboBox, const QList<QWidget*>& widgets) {
        connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [widgets](int state) {
            bool visible = (state == 1);
            for (QWidget* widget : widgets)
            {
                widget->setVisible(visible);
            }
        });
    };

    setupAxisTypeConnections(ui->coboxAxisTypeX1, {
                                 ui->labX1AxisPara,
                                 ui->editAxisMinPosX1,
                                 ui->editAxisMaxPosX1,
                                 ui->editCirclePulseNumX1,
                                 ui->editCircleDistanceX1,
                                 ui->coboxCoordDirectX1,

                                 ui->labX1AxisSpeed,
                                 ui->editAccTimeX1,
                                 ui->editDecTimeX1,
                                 ui->editAccAccX1,
                                 ui->editDecDecX1,
                                 ui->editMaxSpeedX1,

                                 ui->labX1Origin,
                                 ui->coboxOriginTypeX1,
                                 ui->editFindOriginSpeedX1,
                                 ui->editLeaveOriginSpeedX1,
                                 ui->editOriginOffsetX1,
                                 ui->coboxBackOriginDirectX1,
                                 ui->coboxBackOriginOrderX1
                             });

    setupAxisTypeConnections(ui->coboxAxisTypeY1, {
                                 ui->labY1AxisPara,
                                 ui->editAxisMinPosY1,
                                 ui->editAxisMaxPosY1,
                                 ui->editCirclePulseNumY1,
                                 ui->editCircleDistanceY1,
                                 ui->coboxCoordDirectY1,

                                 ui->labY1AxisSpeed,
                                 ui->editAccTimeY1,
                                 ui->editDecTimeY1,
                                 ui->editAccAccY1,
                                 ui->editDecDecY1,
                                 ui->editMaxSpeedY1,

                                 ui->labY1Origin,
                                 ui->coboxOriginTypeY1,
                                 ui->editFindOriginSpeedY1,
                                 ui->editLeaveOriginSpeedY1,
                                 ui->editOriginOffsetY1,
                                 ui->coboxBackOriginDirectY1,
                                 ui->coboxBackOriginOrderY1
                             });

    setupAxisTypeConnections(ui->coboxAxisTypeZ1, {
                                 ui->labZ1AxisPara,
                                 ui->editAxisMinPosZ1,
                                 ui->editAxisMaxPosZ1,
                                 ui->editCirclePulseNumZ1,
                                 ui->editCircleDistanceZ1,
                                 ui->coboxCoordDirectZ1,

                                 ui->labZ1AxisSpeed,
                                 ui->editAccTimeZ1,
                                 ui->editDecTimeZ1,
                                 ui->editAccAccZ1,
                                 ui->editDecDecZ1,
                                 ui->editMaxSpeedZ1,

                                 ui->labZ1Origin,
                                 ui->coboxOriginTypeZ1,
                                 ui->editFindOriginSpeedZ1,
                                 ui->editLeaveOriginSpeedZ1,
                                 ui->editOriginOffsetZ1,
                                 ui->coboxBackOriginDirectZ1,
                                 ui->coboxBackOriginOrderZ1
                             });

    setupAxisTypeConnections(ui->coboxAxisTypeC, {
                                 ui->labCAxisPara,
                                 ui->editAxisMinPosC,
                                 ui->editAxisMaxPosC,
                                 ui->editCirclePulseNumC,
                                 ui->editCircleDistanceC,
                                 ui->coboxCoordDirectC,

                                 ui->labCAxisSpeed,
                                 ui->editAccTimeC,
                                 ui->editDecTimeC,
                                 ui->editAccAccC,
                                 ui->editDecDecC,
                                 ui->editMaxSpeedC,

                                 ui->labCOrigin,
                                 ui->coboxOriginTypeC,
                                 ui->editFindOriginSpeedC,
                                 ui->editLeaveOriginSpeedC,
                                 ui->editOriginOffsetC,
                                 ui->coboxBackOriginDirectC,
                                 ui->coboxBackOriginOrderC
                             });

    setupAxisTypeConnections(ui->coboxAxisTypeY2, {
                                 ui->labY2AxisPara,
                                 ui->editAxisMinPosY2,
                                 ui->editAxisMaxPosY2,
                                 ui->editCirclePulseNumY2,
                                 ui->editCircleDistanceY2,
                                 ui->coboxCoordDirectY2,

                                 ui->labY2AxisSpeed,
                                 ui->editAccTimeY2,
                                 ui->editDecTimeY2,
                                 ui->editAccAccY2,
                                 ui->editDecDecY2,
                                 ui->editMaxSpeedY2,

                                 ui->labY2Origin,
                                 ui->coboxOriginTypeY2,
                                 ui->editFindOriginSpeedY2,
                                 ui->editLeaveOriginSpeedY2,
                                 ui->editOriginOffsetY2,
                                 ui->coboxBackOriginDirectY2,
                                 ui->coboxBackOriginOrderY2
                            });

    setupAxisTypeConnections(ui->coboxAxisTypeZ2, {
                                 ui->labZ2AxisPara,
                                 ui->editAxisMinPosZ2,
                                 ui->editAxisMaxPosZ2,
                                 ui->editCirclePulseNumZ2,
                                 ui->editCircleDistanceZ2,
                                 ui->coboxCoordDirectZ2,

                                 ui->labZ2AxisSpeed,
                                 ui->editAccTimeZ2,
                                 ui->editDecTimeZ2,
                                 ui->editAccAccZ2,
                                 ui->editDecDecZ2,
                                 ui->editMaxSpeedZ2,

                                 ui->labZ2Origin,
                                 ui->coboxOriginTypeZ2,
                                 ui->editFindOriginSpeedZ2,
                                 ui->editLeaveOriginSpeedZ2,
                                 ui->editOriginOffsetZ2,
                                 ui->coboxBackOriginDirectZ2,
                                 ui->coboxBackOriginOrderZ2
                             });
}
#endif

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
        connect(keyEdits[i],&KeyEdit::textChanged,[=](const QString &){
            saveKeyAndLEDFuncDefine();
        });
    }

    //LED
    for (int i=0;i<OPR_LED_NUM;i++) {
        connect(sigEdits[i],&SigLEDEdit::textChanged,[=](const QString &){
            saveKeyAndLEDFuncDefine();
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
        });
    }

    //产品设置
    for (int i=0;i<7;i++) {
        connect(productSet[i],&NumberEdit::textChanged,[=](const QString &){
            saveProductSetPara();
        });
    }

    //高级
    for (int i=0;i<4;i++) {
        connect(productSeniorEdit[i],&NumberEdit::textChanged,[=](const QString &){
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
        connect(accTimeList[i],&NumberEdit::textChanged,[=](const QString &){
            saveAccDecTimeSpeed(i);
        });
        connect(decTimeList[i],&NumberEdit::textChanged,[=](const QString &){
            saveAccDecTimeSpeed(i);
        });
        connect(accAccList[i],&NumberEdit::textChanged,[=](const QString &){
            saveAccDecTimeSpeed(i);
        });
        connect(decDecList[i],&NumberEdit::textChanged,[=](const QString &){
            saveAccDecTimeSpeed(i);
        });
    }
    //安全区（四组）
    connect(ui->btnSaveServoSafePoint,&QPushButton::clicked,[=](){
        saveSafeAreaPara();
    });

    //伺服安全点
    //容差
    for (int i=0;i<3;i++) {
        connect(toleranceList[i],&NumberEdit::textChanged,[=](const QString &){
            saveTolerancePara();
        });
    }

    //横行位置限定
    for (int i=0;i<4;i++) {
        connect(horizontalPosList[i],&NumberEdit::textChanged,[=](const QString &){
            saveDegreePara();
        });
    }

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
    connect(ui->editRotateSiloPlaceNum,&NumberEdit::textChanged,[=](const QString &){
        saveStackFunc();
    });

    for (int i = 0; i < 8; i++)
    {
        stack[i]->saveInfoConnections();
    }

//    QSignalMapper *outputTypeMapper = new QSignalMapper(this);
//    for (int i=0;i<outputTypeList.count();i++)
//    {
//        connect(outputTypeList[i], QOverload<int>::of(&QComboBox::activated), outputTypeMapper,QOverload<>::of(&QSignalMapper::map));
//        outputTypeMapper->setMapping(outputTypeList[i],i);
//    }
//    connect(outputTypeMapper,QOverload<int>::of(&QSignalMapper::mapped),this,&Setting::outputTypeSlots);



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
                switch (j)
                {
                case 0:
                    m_OutportInterlock[i][j]=outportInterlockIndex[i][j]+1/*m_Port_Y[outportInterlockIndex[i][j]].portNum*/;
                    break;
                case 1:
                    m_OutportInterlock[i][j]=outportInterlockIndex[i][j]+1/*m_Port_X[outportInterlockIndex[i][j]].portNum*/;
                    break;
                case 2:
                    m_OutportInterlock[i][j]=outportInterlockIndex[i][j]+1/*m_Port_Y[outportInterlockIndex[i][j]].portNum*/;
                    break;
                case 3:
                    m_OutportInterlock[i][j]=outportInterlockIndex[i][j]+1/*m_Port_X[outportInterlockIndex[i][j]].portNum*/;
                    break;
                }
            }
            else
            {
                m_OutportInterlock[i][j] = 0;
            }

        }
    }
    g_Usart->ExtendSendParDeal(CMD_MAIN_SIGNAL,CMD_SUN_SIGNAL_INTERLOCK);
    setOutportInterlock(m_OutportInterlock);
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
    QThread::msleep(10);
    g_Usart->ExtendSendParDeal(CMD_MAIN_SIGNAL,CMD_SUN_SIGNAL_SENIOR_PORT);

    setSeniorFunc(m_SeniorFunc);

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
    int selectIndex = clawSafeWidgets.at(index).axisSelect_X->currentIndex();
    if(selectIndex == 0)
    {
        m_ClawSave[index].axisSelect_X = 0;
    }
    else
    {
        m_ClawSave[index].axisSelect_X = X1_AXIS+1;
    }
    selectIndex = clawSafeWidgets.at(index).axisSelect_Z->currentIndex();
    if(selectIndex == 0)
    {
        m_ClawSave[index].axisSelect_Z = 0;
    }
    else if(selectIndex == 1)
    {
        m_ClawSave[index].axisSelect_Z = Z1_AXIS + 1;
    }
    else
    {
        m_ClawSave[index].axisSelect_Z = Z2_AXIS + 1;
    }
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

void Setting::saveSafeAreaPara()
{
    int index=ui->tabWgtServoSafePoint->currentIndex();
    if(index>3)
    {
        saveTolerancePara();
        QThread::msleep(10);
        saveDegreePara();
        return;
    }
    m_SaveArea[index].saveAreaSwt = servoPointSafeArea.at(index).safeAreaSwt->currentIndex();
    m_SaveArea[index].processFinishNum = servoPointSafeArea.at(index).processFinishNum->currentIndex();

    // X 轴
    m_SaveArea[index].axisSelect[0] = servoPointSafeArea.at(index).axisSelect[0]->currentIndex();
    // Y 轴
    int indexY = servoPointSafeArea.at(index).axisSelect[1]->currentIndex() ;
    if (indexY == 0){
        m_SaveArea[index].axisSelect[1] = 0;
    }
    else if (indexY == 1) { // Y1轴
        m_SaveArea[index].axisSelect[1] = 2;
    }
    else if (indexY == 2) { // Y2轴
        m_SaveArea[index].axisSelect[1] = 5;
    }
    // Z 轴
    int indexZ = servoPointSafeArea.at(index).axisSelect[2]->currentIndex();
    if (indexZ == 0){
        m_SaveArea[index].axisSelect[2] = 0;
    }
    else if (indexZ == 1){
        m_SaveArea[index].axisSelect[2] = 3;
    }
    else if (indexZ == 2){
        m_SaveArea[index].axisSelect[2] = 6;
    }
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
        break;
    case 1:
        g_Usart->ExtendSendParDeal(CMD_MAIN_MAC,CMD_SUN_MAC_STRUCT);
        break;
    case 2:
        for(i=0;i<AXIS_TOTAL_NUM;i++)
        {
            g_Usart->ExtendSendParDeal(CMD_MAIN_MAC,CMD_SUN_MAC_AXIS,i+1);
            QThread::msleep(10);
        }
        g_Usart->ExtendSendParDeal(CMD_MAIN_MAC,CMD_SUN_MAC_ORIGIN);
        break;
    case 3:
    {
        uint16_t storageGroup=ui->editStorageGroup->text().toUInt();
        if (storageGroup >=0 && storageGroup <100)
        {
            m_ServoCommPar[storageGroup].axis=ui->coboxAxisNum->currentIndex()+1;
            m_ServoCommPar[storageGroup].index=ui->editParaIndex->text().toUInt();
            m_ServoCommPar[storageGroup].sunIndex=ui->editParaSunIndex->text().toUInt();
            m_ServoCommPar[storageGroup].parLen=ui->coboxParaLen->currentIndex()+1;
            m_ServoCommPar[storageGroup].parValue=ui->editParaValue->text().toInt();
            m_ServoCommPar[storageGroup].saveFlag=ui->coboxIsSave->currentIndex();
            m_ServoCommPar[storageGroup].powerOnSet=ui->coboxIsPowerSet->currentIndex();
            g_Usart->ExtendSendParDeal(CMD_MAIN_MAC,CMD_SUN_MAC_SERVO,storageGroup);
        }
        setServoCommPar(m_ServoCommPar);
    }
        break;
    case 4:
        g_Usart->ExtendSendParDeal(CMD_MAIN_MAC,CMD_SUN_MAC_STRUCT);
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
        break;
    default:
        break;
    }

//    int i;
//    switch(index)
//    {
//    case 0:
//        for(i=0;i<AXIS_TOTAL_NUM;i++)
//        {
//            g_Usart->ExtendSendParDeal(CMD_MAIN_SP,CMD_SUN_SP_AXIS_LIMIT,i+1);
//            QThread::msleep(10);
//        }
//        for(i=0;i<AXIS_TOTAL_NUM;i++)
//        {
//            g_Usart->ExtendSendParDeal(CMD_MAIN_MAC,CMD_SUN_MAC_AXIS,i+1);
//            QThread::msleep(10);
//        }


//        break;
//    case 1:
//        for(i=0;i<AXIS_TOTAL_NUM;i++)
//        {
//            g_Usart->ExtendSendParDeal(CMD_MAIN_SERVO,CMD_SUN_SERVO_ACC_DEC,i+1);
//            QThread::msleep(10);
//        }
//        g_Usart->ExtendSendParDeal(CMD_MAIN_SERVO,CMD_SUN_SERVO_MAX_SPEED);
//        break;
//    case 2:
//        g_Usart->ExtendSendParDeal(CMD_MAIN_MAC,CMD_SUN_MAC_LIMIT_SWT);
//        QThread::msleep(10);
//        for(i=0;i<AXIS_TOTAL_NUM;i++)
//        {
//            g_Usart->ExtendSendParDeal(CMD_MAIN_SP,CMD_SUN_SP_AXIS_LIMIT,i+1);
//            QThread::msleep(10);
//        }
//        break;
//    case 3:
//        g_Usart->ExtendSendParDeal(CMD_MAIN_MAC,CMD_SUN_MAC_STRUCT);
//        break;
//    case 4:
//        for(i=0;i<AXIS_TOTAL_NUM;i++)
//        {
//            g_Usart->ExtendSendParDeal(CMD_MAIN_MAC,CMD_SUN_MAC_AXIS,i+1);
//            QThread::msleep(10);
//        }
//        g_Usart->ExtendSendParDeal(CMD_MAIN_MAC,CMD_SUN_MAC_ORIGIN);
//        break;
//    default:
//        break;

//    }

}

//端口自定义显示
void Setting::showPortDefine()
{
//    ui->tableWgtPortDef->setColumnWidth(0,20);
//    ui->tableWgtPortDef->setColumnWidth(1,200);
//    ui->tableWgtPortDef->setColumnWidth(2,200);
//    ui->tableWgtPortDef->setColumnWidth(3,20);
    for(int i=0;i<INPUT_TOTAL_NUM;i++)
    {
        QTableWidgetItem* item1=new QTableWidgetItem(m_Port_X[i].definePort);
        ui->tableWgtPortDef->setItem(i,0,item1);
        QTableWidgetItem* item2=new QTableWidgetItem(m_Port_X[i].defineName);
        ui->tableWgtPortDef->setItem(i,1,item2);
        QTableWidgetItem* item3=new QTableWidgetItem(m_Port_X[i].modifyName);
        ui->tableWgtPortDef->setItem(i,2,item3);
        QTableWidgetItem* item4=new QTableWidgetItem(m_Port_X[i].modifyPort);
        ui->tableWgtPortDef->setItem(i,3,item4);
    }
    for(int i=0;i<OUTPUT_TOTAL_NUM;i++)
    {

        QTableWidgetItem* item1=new QTableWidgetItem(m_Port_Y[i].definePort);
        ui->tableWgtPortDef->setItem(INPUT_TOTAL_NUM+i,0,item1);
        QTableWidgetItem* item2=new QTableWidgetItem(m_Port_Y[i].defineName);
        ui->tableWgtPortDef->setItem(INPUT_TOTAL_NUM+i,1,item2);
        QTableWidgetItem* item3=new QTableWidgetItem(m_Port_Y[i].modifyName);
        ui->tableWgtPortDef->setItem(INPUT_TOTAL_NUM+i,2,item3);
        QTableWidgetItem* item4=new QTableWidgetItem(m_Port_Y[i].modifyPort);
        ui->tableWgtPortDef->setItem(INPUT_TOTAL_NUM+i,3,item4);
    }
}

void Setting::savePortDefine()
{
    for(int i=0;i<INPUT_TOTAL_NUM;i++)
    {
        m_InportFuncDefine[i]=m_Port_X[i].actualPortNum;
    }
    for(int i=0;i<OUTPUT_TOTAL_NUM;i++)
    {
        m_OutportFuncDefine[i]=m_Port_Y[i].actualPortNum;
    }

    g_Usart->ExtendSendParDeal(CMD_MAIN_SIGNAL,CMD_SUN_SIGNAL_IN_FUNC_DEF);
    QThread::msleep(5);
    g_Usart->ExtendSendParDeal(CMD_MAIN_SIGNAL,CMD_SUN_SIGNAL_OUT_FUNC_DEF);
}

void Setting::saveNameDefine()
{

}

void Setting::modifyPort(int row, int column)
{
    if(column == 2)
    {
        auto item = ui->tableWgtPortDef->item(row, column);
        QString originText = item->text();
        FullKeyboard* keyboard = FullKeyboard::instance();
        keyboard->setText(originText);
        keyboard->setCurrentEditObj(nullptr);
        keyboard->exec();
        item->setText(keyboard->getInputText());
    }
    else if(column == 3)
    {
        if(row < INPUT_TOTAL_NUM)
        {
            IOPortDialog dialog(this, IOPortMode::IN);
            if (dialog.exec() == QDialog::Accepted)
            {
                QString text = dialog.getIOOnlineIn();
                m_Port_X[row].actualPortNum = static_cast<uint8_t>(dialog.getInputPort());
                ui->tableWgtPortDef->item(row, column)->setText(text);
            }
        }
        else
        {
            IOPortDialog dialog(this, IOPortMode::OUT);
            if (dialog.exec() == QDialog::Accepted)
            {
                QString text = dialog.getIOOnlineOut();
                m_Port_Y[row-INPUT_TOTAL_NUM].actualPortNum = static_cast<uint8_t>(dialog.getOutputPort());
                ui->tableWgtPortDef->item(row, column)->setText(text);
            }
        }
    }

}

//替换logo处理
void Setting::on_btnLogoUpdate_clicked()
{
    if(Is_Udisk_In())
    {
        QProcess_execute("mount",QStringList() <<"/dev/mmcblk0p1"<< "/mnt/");
        QProcess_execute("cp",QStringList() << "/run/media/sda1/bootlogo.bmp" <<"/mnt/bootlogo.bmp");
        QProcess_execute("cp",QStringList() << "/run/media/sda1/stop.jpg" <<"/root/stop.jpg");
        QProcess_execute("sync",QStringList());
        QProcess_execute("umount",QStringList() <<"/dev/mmcblk0p1");
        emit LOGO_Refresh();
    }
    else
    {//提示u盘未插入
//        MainWindow::pMainWindow->showErrorTip("未插入U盘！");
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
    if(Is_Udisk_In())
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
                MainWindow::pMainWindow->showErrorTip("手持器升级完成,正在重启中...",TipMode::ONLY_OK);
                break;
            }
            case MAINBOARD:
            {
                MainWindow::pMainWindow->showErrorTip("主板升级完成,正在重启中...",TipMode::ONLY_OK);
                break;
            }
            case IOBOARD:
            {
                MainWindow::pMainWindow->showErrorTip("IO板升级完成,正在重启中...",TipMode::ONLY_OK);
                break;
            }
            case SERVO:
            {
                MainWindow::pMainWindow->showErrorTip("伺服升级完成,正在重启中...",TipMode::ONLY_OK);
                break;
            }
            case SYSTEM_DATA_COPY:
            {
                MainWindow::pMainWindow->showErrorTip("数据备份成功!",TipMode::ONLY_OK);
                break;
            }
            case COPY_DATA_REST:
            {
                MainWindow::pMainWindow->showErrorTip("数据还原成功!",TipMode::ONLY_OK);
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
                ui->updata_label->setText("提示 : 手持器升级中,切勿断电,请等待自动重启!");
                break;
            }
            case MAINBOARD:
            {
                ui->updata_label->setText("提示 : 主板升级中,切勿断电,请等待自动重启!");
                break;
            }
            case IOBOARD:
            {
                ui->updata_label->setText("提示 : IO板升级中,切勿断电,请等待自动重启!");
                break;
            }
            case SERVO:
            {
                ui->updata_label->setText("提示 : 伺服升级中,切勿断电,请等待自动重启!");
                break;
            }
            case SYSTEM_DATA_COPY:
            {
                ui->updata_label->setText("提示 : 数据备份中,切勿断电!");
                break;
            }
            case COPY_DATA_REST:
            {
                ui->updata_label->setText("提示 : 数据还原中,切勿断电!");
                break;
            }
            default:
                break;
            }
            ui->updata_widget->show();
            this->update();
            QCoreApplication::processEvents();//调用该函数处理事件循环,立即显示
        });
        connect(ui->Progress_bar,SIGNAL(valueChanged(int)),ui->Progress_num,SLOT(setValue(int)));
        connect(UpgradeDialog,&upgradedialog::UpProgressRefreshSignal,this,[=](){

            ui->Progress_bar->setValue(UpgradeDialog->Upgrade_Progress);
            this->update();
            QCoreApplication::processEvents();
        });
    }
    else
    {
        MainWindow::pMainWindow->showErrorTip("未插入U盘！");
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
    }
    return QWidget::eventFilter(watched, event);
}

