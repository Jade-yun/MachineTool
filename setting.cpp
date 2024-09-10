#include "setting.h"
#include "ui_setting.h"

#include <QMovie>
#include <QPair>
#include <QPropertyAnimation>
#include <QDebug>
#include <QSignalMapper>
#include <QThread>

#include "stackedit.h"

Setting::Setting(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Setting)
{
    ui->setupUi(this);
    for (int i = 0; i < 8; i++)
    {
        stack[i] = new StackEdit(ui->tabWidgetStack->widget(i));
    }

    init();
    controlInitialization();


    pageSwitchInit();
    machineParaLogic();

    connect(ui->coboxStackWay, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index){
        auto mode = (StackMode)(index);
        for (int i = 0; i < 8; i++)
        {
            stack[i]->switchStackWay(mode);
        }
    });
    connect(ui->tabWidgetStack, &QTabWidget::currentChanged, this, [=](int index){
        if (index >=0 && index < 8)
        {
            stack[index]->updateGroupIndex(index);
        }
    });


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

    // 伺服安全点所有编辑框精度0.01
    for (auto edit : ui->tabWgtServoSafePoint->findChildren<NumberEdit*>())
    {
        edit->setDecimalPlaces(2);
    }
    // 旋转料仓工位数字 1-255
    ui->editRotateSiloPlaceNum->setDecimalPlaces(0);
    ui->editRotateSiloPlaceNum->setInputRange(1, 255);

#if 0
    for (QPushButton* btn : ui->pageSettingHome->findChildren<QPushButton*>())
    {
        QPropertyAnimation *animation = new QPropertyAnimation(btn, "geometry", this);
        animation->setDuration(50);

        QRect originalGeometry = btn->geometry();

        int newWidth = originalGeometry.width() * 0.8;
        int newHeight = originalGeometry.height() * 0.8;

        int deltaX = (originalGeometry.width() - newWidth) / 2;
        int deltaY = (originalGeometry.height() - newHeight) / 2;

        QRect smallerGeometry(originalGeometry.x() + deltaX,
                              originalGeometry.y() + deltaY,
                              newWidth,
                              newHeight);

        QObject::connect(btn, &QPushButton::pressed, [btn, animation, originalGeometry, smallerGeometry]() {
            if (animation->state() == QPropertyAnimation::Running) {
//                animation->stop();
                return;
            }

            animation->setStartValue(originalGeometry);
            animation->setEndValue(smallerGeometry);
            animation->start();
        });

        QObject::connect(btn, &QPushButton::leaveEvent, [animation, originalGeometry]() {
            animation->stop();
            animation->setStartValue(animation->targetObject()->property("geometry").toRect());
            animation->setEndValue(originalGeometry);
            animation->start();
        });
    }
#endif
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

    ui->labClawSafePic->setPixmap(QPixmap(":/images/settingPageImages/claw_safe.png"));
    ui->labOnlineSafePic->setPixmap(QPixmap(":/images/settingPageImages/online_safe.png"));


    servoPointSafeArea[0] = {
        ui->coboxSafeArea1, ui->coboxProcessFinishNum1,
        ui->coboxAxisXSA1, ui->coboxAxisYSA1, ui->coboxAxisZSA1,
        ui->editMachineA1AxisXSA1, ui->editMachineA1AxisYSA1,
        ui->editMachineA2AxisXSA1, ui->editMachineA2AxisYSA1,
        ui->editStockBinB1AxisXSA1, ui->editStockBinB1AxisYSA1,
        ui->editStockBinB2AxisXSA1, ui->editStockBinB2AxisYSA1,
        ui->editMaxWaitPosSA1, ui->editMachineZMaxFallPosSA1,ui->editMachinInsideHeightSA1,
        ui->editMaxStartPosSA1,ui->editStockBinZMaxFallPosSA1
    };
    servoPointSafeArea[1] = {
        ui->coboxSafeArea1_2, ui->coboxProcessFinishNum1_2,
        ui->coboxAxisXSA1_2, ui->coboxAxisYSA1_2, ui->coboxAxisZSA1_2,
        ui->editMachineA1AxisXSA1_2, ui->editMachineA1AxisYSA1_2,
        ui->editMachineA2AxisXSA1_2, ui->editMachineA2AxisYSA1_2,
        ui->editStockBinB1AxisXSA1_2, ui->editStockBinB1AxisYSA1_2,
        ui->editStockBinB2AxisXSA1_2, ui->editStockBinB2AxisYSA1_2,
        ui->editMaxWaitPosSA1_2, ui->editMachineZMaxFallPosSA1_2,ui->editMachinInsideHeightSA1_2,
        ui->editMaxStartPosSA1_2,ui->editStockBinZMaxFallPosSA1_2
    };
    servoPointSafeArea[2] = {
        ui->coboxSafeArea1_3, ui->coboxProcessFinishNum1_3,
        ui->coboxAxisXSA1_3, ui->coboxAxisYSA1_3, ui->coboxAxisZSA1_3,
        ui->editMachineA1AxisXSA1_3, ui->editMachineA1AxisYSA1_3,
        ui->editMachineA2AxisXSA1_3, ui->editMachineA2AxisYSA1_3,
        ui->editStockBinB1AxisXSA1_3, ui->editStockBinB1AxisYSA1_3,
        ui->editStockBinB2AxisXSA1_3, ui->editStockBinB2AxisYSA1_3,
        ui->editMaxWaitPosSA1_3, ui->editMachineZMaxFallPosSA1_3, ui->editMachinInsideHeightSA1_3,
        ui->editMaxStartPosSA1_3, ui->editStockBinZMaxFallPosSA1_3
    };

    servoPointSafeArea[3] = {
        ui->coboxSafeArea1_4, ui->coboxProcessFinishNum1_4,
        ui->coboxAxisXSA1_4, ui->coboxAxisYSA1_4, ui->coboxAxisZSA1_4,
        ui->editMachineA1AxisXSA1_4, ui->editMachineA1AxisYSA1_4,
        ui->editMachineA2AxisXSA1_4, ui->editMachineA2AxisYSA1_4,
        ui->editStockBinB1AxisXSA1_4, ui->editStockBinB1AxisYSA1_4,
        ui->editStockBinB2AxisXSA1_4, ui->editStockBinB2AxisYSA1_4,
        ui->editMaxWaitPosSA1_4, ui->editMachineZMaxFallPosSA1_4, ui->editMachinInsideHeightSA1_4,
        ui->editMaxStartPosSA1_4, ui->editStockBinZMaxFallPosSA1_4
    };
    /*******************************************************************************/
    clawSafeWidgets[0] = {
        ui->coboxClaw1KeepoutStatus, ui->coboxAxisXClawSafe, ui->coboxAxisZClawSafe,
        ui->editClawKeepoutX1Min, ui->editClawKeepoutX1Max, ui->editClawKeepoutZ1Height,
        ui->editAxisCKeepoutX1Min, ui->editAxisCKeepoutX1Max, ui->editAxisCKeepoutZHeight,
        ui->editAxisCKeepoutForwardMin, ui->editAxisCKeepoutForwardMax,
        ui->editAxisCKeepoutReverseMin, ui->editAxisCKeepoutReverseMax
    };
    clawSafeWidgets[1] = {
        ui->coboxClaw1KeepoutStatus_2, ui->coboxAxisXClawSafe_2, ui->coboxAxisZClawSafe_2,
        ui->editClawKeepoutX1Min_2, ui->editClawKeepoutX1Max_2, ui->editClawKeepoutZ1Height_2,
        ui->editAxisCKeepoutX1Min_2, ui->editAxisCKeepoutX1Max_2, ui->editAxisCKeepoutZHeight_2,
        ui->editAxisCKeepoutForwardMin_2, ui->editAxisCKeepoutForwardMax_2,
        ui->editAxisCKeepoutReverseMin_2, ui->editAxisCKeepoutReverseMax_2
    };
    clawSafeWidgets[2] = {
        ui->coboxClaw1KeepoutStatus_3, ui->coboxAxisXClawSafe_3, ui->coboxAxisZClawSafe_3,
        ui->editClawKeepoutX1Min_3, ui->editClawKeepoutX1Max_3, ui->editClawKeepoutZ1Height_3,
        ui->editAxisCKeepoutX1Min_3, ui->editAxisCKeepoutX1Max_3, ui->editAxisCKeepoutZHeight_3,
        ui->editAxisCKeepoutForwardMin_3, ui->editAxisCKeepoutForwardMax_3,
        ui->editAxisCKeepoutReverseMin_3, ui->editAxisCKeepoutReverseMax_3
    };
    clawSafeWidgets[3] = {
        ui->coboxClaw1KeepoutStatus_4, ui->coboxAxisXClawSafe_4, ui->coboxAxisZClawSafe_4,
        ui->editClawKeepoutX1Min_4, ui->editClawKeepoutX1Max_4, ui->editClawKeepoutZ1Height_4,
        ui->editAxisCKeepoutX1Min_4, ui->editAxisCKeepoutX1Max_4, ui->editAxisCKeepoutZHeight_4,
        ui->editAxisCKeepoutForwardMin_4, ui->editAxisCKeepoutForwardMax_4,
        ui->editAxisCKeepoutReverseMin_4, ui->editAxisCKeepoutReverseMax_4
    };
    /*******************************************************************************/
    onlineSafeWidgets[0] = {
        ui->coboxMasterSlaveMode, ui->editPulseTime, ui->coboxOnlineSelect, ui->coboxOnlineAxis,
        ui->editAreaInput, ui->editAreaOutput, ui->editRequestInput, ui->editRequestOutput,
        ui->editA1A2MinPos, ui->editA1A2MaxPos
    };
    onlineSafeWidgets[1] = {
        ui->coboxMasterSlaveMode_2, ui->editPulseTime_2, ui->coboxOnlineSelect_2, ui->coboxOnlineAxis_2,
        ui->editAreaInput_2, ui->editAreaOutput_2, ui->editRequestInput_2, ui->editRequestOutput_2,
        ui->editA1A2MinPos_2, ui->editA1A2MaxPos_2
    };
    onlineSafeWidgets[2] = {
        ui->coboxMasterSlaveMode_3, ui->editPulseTime_3, ui->coboxOnlineSelect_3, ui->coboxOnlineAxis_3,
        ui->editAreaInput_3, ui->editAreaOutput_3, ui->editRequestInput_3, ui->editRequestOutput_3,
        ui->editA1A2MinPos_3, ui->editA1A2MaxPos_3
    };
    onlineSafeWidgets[3] = {
        ui->coboxMasterSlaveMode_4, ui->editPulseTime_4, ui->coboxOnlineSelect_4, ui->coboxOnlineAxis_4,
        ui->editAreaInput_4, ui->editAreaOutput_4, ui->editRequestInput_4, ui->editRequestOutput_4,
        ui->editA1A2MinPos_4, ui->editA1A2MaxPos_4
    };

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


void Setting::pageSwitchInit()
{
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
    }


    /***************************************System setting part**********************************************************/
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
    // 旋转料仓类型
    connect(ui->coboxSiloType, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index){
        ui->labRotateSiloFinHint->setVisible(index);
        ui->coboxRotateSiloFinHint->setVisible(index);
        ui->labRotateSiloPlaceNum->setVisible(index);
        ui->editRotateSiloPlaceNum->setVisible(index);
    });

}

void Setting::machineParaLogic()
{
    ui->tableWgtAxisPara->setColumnCount(5);
    ui->tableWgtAxisPara->setRowCount(6);
//    ui->tableWgtAxisPara->horizontalHeader()->setDefaultSectionSize(140);
//    ui->tableWgtAxisPara->verticalHeader()->setDefaultSectionSize(41);
    ui->tableWgtAxisPara->setHorizontalHeaderLabels(
    {tr("最小坐标"), tr("最大坐标"), tr("单圈脉冲数"), tr("单圈距离"),tr("坐标方向")});
    ui->tableWgtAxisPara->setVerticalHeaderLabels(
    {tr("X1轴"), tr("Y1轴"), tr("Z1轴"), tr("C轴"), tr("Y2轴"), tr("Z2轴")});
    auto addToTable = [&](QTableWidget* tableWgt, int row, const QList<QWidget*> &widgets) {
        int i = 0;
        for (QWidget* widget : widgets)
        {
            tableWgt->setCellWidget(row, i++, widget);

        }
    };

    addToTable(ui->tableWgtAxisPara, 0, {
                   ui->editAxisMinPosX1, ui->editAxisMaxPosX1,ui->editCirclePulseNumX1,
                   ui->editCircleDistanceX1,ui->coboxCoordDirectX1});
    addToTable(ui->tableWgtAxisPara, 1, {
                   ui->editAxisMinPosY1, ui->editAxisMaxPosY1,ui->editCirclePulseNumY1,
                   ui->editCircleDistanceY1,ui->coboxCoordDirectY1});
    addToTable(ui->tableWgtAxisPara, 2, {
                   ui->editAxisMinPosZ1, ui->editAxisMaxPosZ1,ui->editCirclePulseNumZ1,
                   ui->editCircleDistanceZ1,ui->coboxCoordDirectZ1});
    addToTable(ui->tableWgtAxisPara, 3, {
                   ui->editAxisMinPosC, ui->editAxisMaxPosC,ui->editCirclePulseNumC,
                   ui->editCircleDistanceC,ui->coboxCoordDirectC});
    addToTable(ui->tableWgtAxisPara, 4, {
                   ui->editAxisMinPosY2, ui->editAxisMaxPosY2,ui->editCirclePulseNumY2,
                   ui->editCircleDistanceY2,ui->coboxCoordDirectY2});
    addToTable(ui->tableWgtAxisPara, 5, {
                   ui->editAxisMinPosZ2, ui->editAxisMaxPosZ2,ui->editCirclePulseNumZ2,
                   ui->editCircleDistanceZ2,ui->coboxCoordDirectZ2});

    connect(ui->coboxAxisTypeX1, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int state) {
        bool visible = (state != 1);
        ui->tableWgtAxisPara->setRowHidden(0, visible);

    });

    ui->tableWgtAxisSpeed->setColumnCount(5);
    ui->tableWgtAxisSpeed->setRowCount(6);
    ui->tableWgtAxisSpeed->setHorizontalHeaderLabels(
    {tr("加速时间(s)"), tr("减速时间(s)"), tr("加加速"), tr("减减速"),tr("最大速度")});
    ui->tableWgtAxisSpeed->setVerticalHeaderLabels(
    {tr("X1轴"), tr("Y1轴"), tr("Z1轴"), tr("C轴"), tr("Y2轴"), tr("Z2轴")});

//    addToTable(ui->tableWgtAxisSpeed, 0, {


//               });

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
//                                 ui->labX1AxisPara,
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
//                                 ui->labY1AxisPara,
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
//                                 ui->labZ1AxisPara,
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
//                                 ui->labCAxisPara,
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
//                                 ui->labY2AxisPara,
                                 ui->editAxisMinPosY2,
                                 ui->editAxisMaxPosY2,
                                 ui->editCirclePulseNumY2,
                                 ui->editCircleDistanceY2,
                                 ui->coboxCoordDirectY2,

//                                 ui->labY2AxisSpeed,
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
//                                 ui->labZ2AxisPara,
                                 ui->editAxisMinPosZ2,
                                 ui->editAxisMaxPosZ2,
                                 ui->editCirclePulseNumZ2,
                                 ui->editCircleDistanceZ2,
                                 ui->coboxCoordDirectZ2,

//                                 ui->labZ2AxisSpeed,
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


//PXC_240826    通信
void Setting::controlInitialization()
{
    //输出类型
    outputTypeList.push_back(ui->coboxOutPortType0);
    outputTypeList.push_back(ui->coboxOutPortType1);
    outputTypeList.push_back(ui->coboxOutPortType2);
    outputTypeList.push_back(ui->coboxOutPortType3);
    outputTypeList.push_back(ui->coboxOutPortType4);
    outputTypeList.push_back(ui->coboxOutPortType5);
    outputTypeList.push_back(ui->coboxOutPortType6);
    outputTypeList.push_back(ui->coboxOutPortType7);
    outputTypeList.push_back(ui->coboxOutPortType8);
    outputTypeList.push_back(ui->coboxOutPortType9);
    outputTypeList.push_back(ui->coboxOutPortType10);
    outputTypeList.push_back(ui->coboxOutPortType11);
    outputTypeList.push_back(ui->coboxOutPortType12);
    outputTypeList.push_back(ui->coboxOutPortType13);

    //互锁设置
    outportInterlockList.push_back(ui->chbox0InterlockGroup0);
    outportInterlockList.push_back(ui->chbox2InterlockGroup0);
    outportInterlockList.push_back(ui->chbox1InterlockGroup0);
    outportInterlockList.push_back(ui->chbox3InterlockGroup0);

    outportInterlockList.push_back(ui->chbox0InterlockGroup1);
    outportInterlockList.push_back(ui->chbox2InterlockGroup1);
    outportInterlockList.push_back(ui->chbox1InterlockGroup1);
    outportInterlockList.push_back(ui->chbox3InterlockGroup1);

    outportInterlockList.push_back(ui->chbox0InterlockGroup2);
    outportInterlockList.push_back(ui->chbox2InterlockGroup2);
    outportInterlockList.push_back(ui->chbox1InterlockGroup2);
    outportInterlockList.push_back(ui->chbox3InterlockGroup2);

    outportInterlockList.push_back(ui->chbox0InterlockGroup3);
    outportInterlockList.push_back(ui->chbox2InterlockGroup3);
    outportInterlockList.push_back(ui->chbox1InterlockGroup3);
    outportInterlockList.push_back(ui->chbox3InterlockGroup3);

    outportInterlockList.push_back(ui->chbox0InterlockGroup4);
    outportInterlockList.push_back(ui->chbox2InterlockGroup4);
    outportInterlockList.push_back(ui->chbox1InterlockGroup4);
    outportInterlockList.push_back(ui->chbox3InterlockGroup4);

    outportInterlockList.push_back(ui->chbox0InterlockGroup5);
    outportInterlockList.push_back(ui->chbox2InterlockGroup5);
    outportInterlockList.push_back(ui->chbox1InterlockGroup5);
    outportInterlockList.push_back(ui->chbox3InterlockGroup5);

    outportInterlockList.push_back(ui->chbox0InterlockGroup6);
    outportInterlockList.push_back(ui->chbox2InterlockGroup6);
    outportInterlockList.push_back(ui->chbox1InterlockGroup6);
    outportInterlockList.push_back(ui->chbox3InterlockGroup6);

    outportInterlockList.push_back(ui->chbox0InterlockGroup7);
    outportInterlockList.push_back(ui->chbox2InterlockGroup7);
    outportInterlockList.push_back(ui->chbox1InterlockGroup7);
    outportInterlockList.push_back(ui->chbox3InterlockGroup7);

    outportInterlockList.push_back(ui->chbox0InterlockGroup8);
    outportInterlockList.push_back(ui->chbox2InterlockGroup8);
    outportInterlockList.push_back(ui->chbox1InterlockGroup8);
    outportInterlockList.push_back(ui->chbox3InterlockGroup8);

    outportInterlockList.push_back(ui->chbox0InterlockGroup9);
    outportInterlockList.push_back(ui->chbox2InterlockGroup9);
    outportInterlockList.push_back(ui->chbox1InterlockGroup9);
    outportInterlockList.push_back(ui->chbox3InterlockGroup9);

    outportInterlockList.push_back(ui->chbox0InterlockGroup10);
    outportInterlockList.push_back(ui->chbox2InterlockGroup10);
    outportInterlockList.push_back(ui->chbox1InterlockGroup10);
    outportInterlockList.push_back(ui->chbox3InterlockGroup10);

    outportInterlockList.push_back(ui->chbox0InterlockGroup11);
    outportInterlockList.push_back(ui->chbox2InterlockGroup11);
    outportInterlockList.push_back(ui->chbox1InterlockGroup11);
    outportInterlockList.push_back(ui->chbox3InterlockGroup11);

    //预留关联
    outportRelevancyList.push_back(ui->coboxOutPortRelevancy0);
    outportRelevancyList.push_back(ui->coboxOutPortRelevancy1);
    outportRelevancyList.push_back(ui->coboxOutPortRelevancy2);
    outportRelevancyList.push_back(ui->coboxOutPortRelevancy3);
    outportRelevancyList.push_back(ui->coboxOutPortRelevancy4);
    outportRelevancyList.push_back(ui->coboxOutPortRelevancy5);
    outportRelevancyList.push_back(ui->coboxOutPortRelevancy6);
    outportRelevancyList.push_back(ui->coboxOutPortRelevancy7);
    outportRelevancyList.push_back(ui->coboxOutPortRelevancy8);
    outportRelevancyList.push_back(ui->coboxOutPortRelevancy9);
    outportRelevancyList.push_back(ui->coboxOutPortRelevancy10);
    outportRelevancyList.push_back(ui->coboxOutPortRelevancy11);
    outportRelevancyList.push_back(ui->coboxOutPortRelevancy12);
    outportRelevancyList.push_back(ui->coboxOutPortRelevancy13);

    //预留关联出
    outportReleteOutList.push_back(ui->coboxOutPortRelateOut0);
    outportReleteOutList.push_back(ui->coboxOutPortRelateOut1);
    outportReleteOutList.push_back(ui->coboxOutPortRelateOut2);
    outportReleteOutList.push_back(ui->coboxOutPortRelateOut3);
    outportReleteOutList.push_back(ui->coboxOutPortRelateOut4);
    outportReleteOutList.push_back(ui->coboxOutPortRelateOut5);
    outportReleteOutList.push_back(ui->coboxOutPortRelateOut6);
    outportReleteOutList.push_back(ui->coboxOutPortRelateOut7);
    outportReleteOutList.push_back(ui->coboxOutPortRelateOut8);
    outportReleteOutList.push_back(ui->coboxOutPortRelateOut9);
    outportReleteOutList.push_back(ui->coboxOutPortRelateOut10);
    outportReleteOutList.push_back(ui->coboxOutPortRelateOut11);
    outportReleteOutList.push_back(ui->coboxOutPortRelateOut12);
    outportReleteOutList.push_back(ui->coboxOutPortRelateOut13);

    //高级
    seniorFuncList.push_back(ui->coboxKnifeOrigin1Check);
    seniorFuncList.push_back(ui->coboxKnifeOrigin2Check);
    seniorFuncList.push_back(ui->coboxlabChuckOriginCheck);
    seniorFuncList.push_back(ui->coboxStockSaveIn1Check);
    seniorFuncList.push_back(ui->coboxStockSaveIn2Check);
    seniorFuncList.push_back(ui->coboxStockSaveInOutCheck);
    seniorFuncList.push_back(ui->coboxAlarmIn1Check);
    seniorFuncList.push_back(ui->coboxAlarmIn2Check);
    seniorFuncList.push_back(ui->coboxlabEmergencyStopCheck);
    seniorFuncList.push_back(ui->coboxPauseStopCheck);
    seniorFuncList.push_back(ui->coboxPressureCheck);
    seniorFuncList.push_back(ui->coboxRemoteAuto);
    seniorFuncList.push_back(ui->coboxRemoteStop);
    seniorFuncList.push_back(ui->coboxBottomOilLimit);
    seniorFuncList.push_back(ui->coboxOilerSwt);
    seniorFuncList.push_back(ui->coboxLubricationPump);
    seniorFuncList.push_back(ui->coboxProcessSafe1);
    seniorFuncList.push_back(ui->coboxProcessSafe2);
    seniorFuncList.push_back(ui->coboxEmergencySnapMotorEnable);
    seniorFuncList.push_back(ui->coboxEmergenceStopOutput);
    seniorFuncList.push_back(ui->coboxAutoLigth);
    seniorFuncList.push_back(ui->coboxAlarmLight);
    seniorFuncList.push_back(ui->coboxAlarmBuzzer);
    seniorFuncList.push_back(ui->coboxPauseLight);

    seniorFuncList.push_back(ui->coboxProcessFinish1);
    seniorFuncList.push_back(ui->coboxProcessFinish2);
    seniorFuncList.push_back(ui->coboxLocateFinish1);
    seniorFuncList.push_back(ui->coboxLocateFinish2);
    seniorFuncList.push_back(ui->coboxStartProcess1);
    seniorFuncList.push_back(ui->coboxStartProcess2);
    seniorFuncList.push_back(ui->coboxReserveAdvance1);
    seniorFuncList.push_back(ui->coboxReserveAdvance2);
    seniorFuncList.push_back(ui->coboxMainAxisRotate1);
    seniorFuncList.push_back(ui->coboxMainAxisRotate2);
    seniorFuncList.push_back(ui->coboxMainAxisLocate1);
    seniorFuncList.push_back(ui->coboxMainAxisLocate2);
    seniorFuncList.push_back(ui->coboxBlow1);
    seniorFuncList.push_back(ui->coboxBlow2);
    seniorFuncList.push_back(ui->coboxReserveAdvance3);
    seniorFuncList.push_back(ui->coboxReserveAdvance4);
    seniorFuncList.push_back(ui->coboxManualChuckIn1);
    seniorFuncList.push_back(ui->coboxManualChuckIn2);
    seniorFuncList.push_back(ui->coboxAutoDoorCtl1);
    seniorFuncList.push_back(ui->coboxAutoDoorCtl2);
    seniorFuncList.push_back(ui->coboxReserveAdvance5);
    seniorFuncList.push_back(ui->coboxReserveAdvance6);
    seniorFuncList.push_back(ui->coboxReserveAdvance7);
    seniorFuncList.push_back(ui->coboxReserveAdvance8);

    for (int i=0;i<outputTypeList.count();i++)
    {
        connect(outputTypeList[i], QOverload<int>::of(&QComboBox::activated), this,&Setting::outputTypeSlots);
    }

    for (int i=0;i<outportInterlockList.count();i++)
    {
        connect(outportInterlockList[i], QOverload<int>::of(&QCheckBox::stateChanged), this,&Setting::outportInterlockSlots);
    }

    for (int i=0;i<outportRelevancyList.count();i++)
    {
        connect(outportRelevancyList[i], QOverload<int>::of(&QComboBox::activated), this,&Setting::outportRelevancySlots);
    }

    for (int i=0;i<outportReleteOutList.count();i++)
    {
        connect(outportReleteOutList[i], QOverload<int>::of(&QComboBox::activated), this,&Setting::outportReleteOutSlots);
    }

    for (int i=0;i<seniorFuncList.count();i++)
    {
        connect(seniorFuncList[i], QOverload<int>::of(&QComboBox::activated), this,&Setting::seniorFuncSlots);
    }

//    QSignalMapper *outputTypeMapper = new QSignalMapper(this);
//    for (int i=0;i<outputTypeList.count();i++)
//    {
//        connect(outputTypeList[i], QOverload<int>::of(&QComboBox::activated), outputTypeMapper,QOverload<>::of(&QSignalMapper::map));
//        outputTypeMapper->setMapping(outputTypeList[i],i);
//    }
//    connect(outputTypeMapper,QOverload<int>::of(&QSignalMapper::mapped),this,&Setting::outputTypeSlots);



}

void Setting::outputTypeSlots(int id)
{
    //先将最新的参数存入结构体中
    for (int i=0;i<OUT_PORT_TYPE_NUM;i++)
    {
        m_OutPortType[i][1]=outputTypeList[i]->currentIndex();
    }
    g_Usart->ExtendSendParDeal(CMD_MAIN_SIGNAL,CMD_SUN_SIGNAL_OUT_TYPE);
}

void Setting::outportInterlockSlots(int id)
{
    //先将最新的参数存入结构体中
    int index=0;
    for (int i=0;i<OUT_INTERLOCK_NUM;i++)
    {
        for (int j=0;j<4;j++)
        {
            if(outportInterlockList[4*i+j]->isChecked())
            {
                switch (j)
                {
                case 0:
                    m_OutportInterlock[i][j]=m_Port_Y[outportInterlockIndex[i][j]].portNum;
                    break;
                case 1:
                    m_OutportInterlock[i][j]=m_Port_X[outportInterlockIndex[i][j]].portNum;
                    break;
                case 2:
                    m_OutportInterlock[i][j]=m_Port_Y[outportInterlockIndex[i][j]].portNum;
                    break;
                case 3:
                    m_OutportInterlock[i][j]=m_Port_X[outportInterlockIndex[i][j]].portNum;
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
}

void Setting::outportRelevancySlots(int id)
{
    //先将最新的参数存入结构体中
    for (int i=0;i<OUT_PORT_RELEVANCY_NUM;i++)
    {
        m_OutportRelevancy[i][2]=outportRelevancyList[i]->currentIndex();
    }
    g_Usart->ExtendSendParDeal(CMD_MAIN_SIGNAL,CMD_SUN_SIGNAL_OUT_RELEVENCY);
}

void Setting::outportReleteOutSlots(int id)
{
    //先将最新的参数存入结构体中
    for (int i=0;i<OUT_PORT_RELEOUT_NUM;i++)
    {
        m_OutportReleteOut[i][1]=outportReleteOutList[i]->currentIndex();
    }
    g_Usart->ExtendSendParDeal(CMD_MAIN_SIGNAL,CMD_SUN_SIGNAL_RELATE_OUT);
}

void Setting::seniorFuncSlots(int id)
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
    m_SeniorFunc.startFinish1 = seniorFuncList[index++]->currentIndex();
    m_SeniorFunc.startFinish2 = seniorFuncList[index++]->currentIndex();
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
}

void Setting::saveKeyFuncDefine()
{
    const std::array<KeyEdit* const, 16> keyEdits = {
        ui->editKeyX1Sub, ui->editKeyX1Add, ui->editKeyY1Sub, ui->editKeyY1Add,
        ui->editKeyZ1Sub, ui->editKeyZ1Add, ui->editKeyCSub, ui->editKeyCAdd,
        ui->editKeyAorY2Sub, ui->editKeyAorY2ADD, ui->editKeyBorZ2Sub, ui->editKeyBorZ2Add,
        ui->editKeyF1, ui->editKeyF2, ui->editKeyF3, ui->editKeyF4
    };
    for (int i = 0; i < keyEdits.size(); i++)
    {
        m_KeyFunc[i].keyType = keyEdits.at(i)->getKeyType();
        m_KeyFunc[i].funcNum = keyEdits.at(i)->getPortNum();
        m_KeyFunc[i].oprMode = keyEdits.at(i)->getKeyFuncStatus();
    }
}

void Setting::saveLEDSignal()
{

    const std::array<const SigLEDEdit* const , 5> sigEdits = {
        ui->editLED0, ui->editLED1, ui->editLED2, ui->editLED3, ui->editLED4
    };

    for (auto i = (size_t)0; i < sigEdits.size(); i++)
    {
        m_LedFunc[i].ledType = sigEdits.at(i)->getSigType();
        m_LedFunc[i].funcNum = sigEdits.at(i)->getSigPort();
    }
}

void Setting::saveMachineSafePara()
{
    const QComboBox* machineSafe[] = {
        ui->coboxArmDownAutoGate1Switch, ui->coboxArmDownAutoGate1SwitchStatus,
        ui->coboxArmDownAutoGate2Switch, ui->coboxArmDownAutoGate2SwitchStatus
    };

    m_MachineSave.armFallAutoDoor1Swt = machineSafe[0]->currentIndex();
    m_MachineSave.armFallAutoDoor1SwtSta = machineSafe[1]->currentIndex();
    m_MachineSave.armFallAutoDoor2Swt = machineSafe[2]->currentIndex();
    m_MachineSave.armFallAutoDoor2SwtSta = machineSafe[3]->currentIndex();

}

void Setting::saveStackBinSafePara()
{
    const QComboBox* stackBinSafe[] = {ui->coboxStockIn1Recovery, ui->coboxStockIn2Recovery};
    m_StackSave.stackSaveIn1Roatore = stackBinSafe[0]->currentIndex();
    m_StackSave.stackSaveIn2Roatore = stackBinSafe[1]->currentIndex();

}

void Setting::saveClawSafePara(int index)
{

            m_ClawSave[index].clawKeepoutSta = clawSafeWidgets.at(index).clawKeepoutSta->currentIndex();
            m_ClawSave[index].axisSelect_X = clawSafeWidgets.at(index).axisSelect_X->currentIndex();
            m_ClawSave[index].axisSelect_Z = clawSafeWidgets.at(index).axisSelect_Z->currentIndex();
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
}

void Setting::saveProductSetPara()
{
    const std::array<NumberEdit*, 7> productSet = {ui->editWaitProcessFinishTime, ui->editPulseOutputTime,
                                                   ui->editValveLimitTime, ui->editAutoCycleTime,
                                                   ui->editAlarmBuzzerTime, ui->editChainReachDelay,
                                                   ui->editClampCheckFilterTime};
    const std::array<NumberEdit*, 4> productSeniorEdit = {ui->editLubricationOpenTime, ui->editLubricationCloseTime,
                                                          ui->editLubricationDelay, ui->editLubricationIntervalCycle};

    const std::array<QComboBox*, 3> productIot = {ui->coboxMainboardComunication, ui->coboxParityCheck,
                                                  ui->coboxBaudRate};

    m_ProductSet.waitProcessFinTime = productSet.at(0)->text().toFloat() * 10;
    m_ProductSet.pluseOutTime = productSet.at(1)->text().toFloat() * 10;
    m_ProductSet.airVlaveLimitTime = productSet.at(2)->text().toFloat() * 10;
    m_ProductSet.autoCycTime = productSet.at(3)->text().toFloat() * 10;
    m_ProductSet.alarmBuzzerTime = productSet.at(4)->text().toFloat() * 10;
    m_ProductSet.chainReachDelay = productSet.at(5)->text().toFloat() * 10;
    m_ProductSet.clampCheckFilterTime = productSet.at(6)->text().toFloat() * 10;

    m_ProductSenior.lubricOpenTime = productSeniorEdit.at(0)->text().toFloat() * 10;
    m_ProductSenior.lubricCloseTime = productSeniorEdit.at(1)->text().toFloat() * 10;
    m_ProductSenior.lubricDelay = productSeniorEdit.at(2)->text().toFloat() * 10;
    m_ProductSenior.lubricGapCyc = productSeniorEdit.at(3)->text().toInt();
    m_ProductSenior.productMemoryFlag = ui->coboxProductMemory->currentIndex();

    m_Internet.netCommType = productIot.at(0)->currentIndex();
    m_Internet.parityMode = productIot.at(1)->currentIndex();
    m_Internet.baudMode = productIot.at(2)->currentIndex();
}

void Setting::saveServoPara()
{
    const std::array<const QComboBox*, 8> servoPara = {
        ui->coboxEncoderType, ui->coboxEncoderBitNum, ui->coboxServoBrand, ui->coboxServoModel,
        ui->coboxExtendIOBoard, ui->coboxRotateAxisUnit, ui->coboxResetSubProgram, ui->coboxTorqueLimitFlag};
    m_ServoPar.encoderType = servoPara.at(0)->currentIndex();
    m_ServoPar.encoderBitNum = servoPara.at(1)->currentIndex();
    m_ServoPar.servoBrand = servoPara.at(2)->currentIndex();
    m_ServoPar.servoModel = servoPara.at(3)->currentIndex();
    m_ServoPar.extendIOBoard = servoPara.at(4)->currentIndex();
    m_ServoPar.rotateAxisUnit = servoPara.at(5)->currentIndex();
    m_ServoPar.resetSunProFlag = servoPara.at(6)->currentIndex();
    m_ServoPar.torqueLimitFlag = servoPara.at(7)->currentIndex();
}

void Setting::saveSafeAreaPara(int index)
{
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
        m_SaveArea[index].machineA1Pos[i] = servoPointSafeArea.at(index).machineA1Pos[i]->text().toFloat()*100;
        m_SaveArea[index].starckB1Pos[i] = servoPointSafeArea.at(index).starckB1Pos[i]->text().toFloat()*100;
        m_SaveArea[index].starckB2Pos[i] = servoPointSafeArea.at(index).starckB2Pos[i]->text().toFloat()*100;
    }

    m_SaveArea[index].machine_Z_WaitMaxPos = servoPointSafeArea.at(index).machine_Z_WaitMaxPos->text().toFloat()*100;
    m_SaveArea[index].machine_Z_FallMaxPos = servoPointSafeArea.at(index).machine_Z_FallMaxPos->text().toFloat()*100;
    m_SaveArea[index].machine_Z_InsideHigh = servoPointSafeArea.at(index).machine_Z_InsideHigh->text().toFloat()*100;
    m_SaveArea[index].stack_Z_StartMaxPos = servoPointSafeArea.at(index).stack_Z_StartMaxPos->text().toFloat()*100;
    m_SaveArea[index].stack_Z_FallMaxPos = servoPointSafeArea.at(index).stack_Z_FallMaxPos->text().toFloat()*100;

}

void Setting::saveDegreePara()
{
    const NumberEdit* servoPointPosLimit[] = {ui->editTolerance, ui->editOriginDegree, ui->editAcitonConfirmError,
                                 ui->editSafeRampageHeightZ1, ui->editSafeRampageHeightZ2, ui->editSafeRotatePosY1,
                                 ui->editSafeRotatePosY2};

    m_DegreeParS.tolerance = servoPointPosLimit[0]->text().toFloat()*100;
    m_DegreeParS.originDegree = servoPointPosLimit[1]->text().toFloat()*100;
    m_DegreeParS.actionFinDegree = servoPointPosLimit[2]->text().toFloat()*100;
    m_DegreeParS.saveRampageHige_Z1 = servoPointPosLimit[3]->text().toFloat()*100;
    m_DegreeParS.saveRampageHige_Z2 = servoPointPosLimit[4]->text().toFloat()*100;
    m_DegreeParS.saveRotatePos_Y1 = servoPointPosLimit[5]->text().toFloat()*100;
    m_DegreeParS.saveRotatePos_Y2 = servoPointPosLimit[6]->text().toFloat()*100;

}

void Setting::saveStackFunc()
{
    const std::array<QComboBox* const, 8> coboxs = {
        ui->coboxStackWay, ui->coboxGroupStack, ui->coboxRowStack, ui->coboxLayerStack,
        ui->coboxDividedStack, ui->coboxStackMemory, ui->coboxSiloType, ui->coboxRotateSiloFinHint
    };
    NumberEdit* const rotateSiloPlaceNum = ui->editRotateSiloPlaceNum;
    m_StackFunc.stackType = coboxs.at(0)->currentIndex();
    m_StackFunc.groupStack =coboxs.at(1)->currentIndex();
    m_StackFunc.rowStack = coboxs.at(2)->currentIndex();
    m_StackFunc.storeyStack =coboxs.at(3)->currentIndex();
    m_StackFunc.divideStack = coboxs.at(4)->currentIndex();
    m_StackFunc.stackMemory =coboxs.at(5)->currentIndex();
    m_StackFunc.siloType = coboxs.at(6)->currentIndex();
    m_StackFunc.rotateSiloFinHint = coboxs.at(7)->currentIndex();
    m_StackFunc.rotateSiloPlaceNum = rotateSiloPlaceNum->text().toInt();
}




