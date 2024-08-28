#include "setting.h"
#include "ui_setting.h"

#include <QMovie>
#include <QPair>
#include <QDebug>

#include "stackedit.h"

Setting::Setting(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Setting)
{
    ui->setupUi(this);
    initVariables();


    pageSwitchInit();
    machineParaLogic();

    for (int i = 0; i < 8; i++)
    {
        stack[i] = new StackEdit(ui->tabWidgetStack->widget(i));
    }

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

    for (NumberEdit* edit : ui->tabWidgetProduct->findChildren<NumberEdit*>())
    {
        edit->setDecimalPlaces(1); // 精度0.1
    }
    // 用户设置
    for (NumberEdit* edit : ui->tabUserSet->findChildren<NumberEdit*>())
    {
        edit->setDecimalPlaces(0);
    }

    // 伺服安全点所有编辑框精度0.01
    for (int i = 0; i < ui->tabWgtServoSafePoint->count(); i++)
    {

        auto edits = ui->tabWgtServoSafePoint->widget(i)->findChildren<NumberEdit*>();
//        qDebug() << "number of NumberEdit: " << edits->
        for (NumberEdit* edit : edits)
        {
            edit->setDecimalPlaces(2);
        }
    }

}

Setting::~Setting()
{
    delete ui;
}

void Setting::slotSettingHome()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->stackedWidget->setCurrentWidget(ui->pageSettingHome);
    //    qDebug() << "slotSettingHome";
}

void Setting::ShowStackPage()
{
    ui->stackedWidget->setCurrentWidget(ui->pageStack);
}


// to initial all variables in this function
void Setting::initVariables()
{
    //    movie = new QMovie(QString(":/images/gif/test.gif"));
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
    ui->labCLimitPos->setVisible(false);
    ui->coboxLimitPosC->setVisible(false);
    ui->coboxLimitNegC->setVisible(false);

//    ui->labALimitPos->setVisible(false);
//    ui->coboxLimitPosA->setVisible(false);
//    ui->coboxLimitNegA->setVisible(false);

//    ui->labBLimitPos->setVisible(false);
//    ui->coboxLimitPosB->setVisible(false);
//    ui->coboxLimitNegB->setVisible(false);


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

//    setupAxisTypeConnections(ui->coboxAxisTypeA, {
//                                 ui->labAAxisPara,
//                                 ui->editAxisMinPosA,
//                                 ui->editAxisMaxPosA,
//                                 ui->editCirclePulseNumA,
//                                 ui->editCircleDistanceA,
//                                 ui->coboxCoordDirectA,

//                                 ui->labAAxisSpeed,
//                                 ui->editAccTimeA,
//                                 ui->editDecTimeA,
//                                 ui->editAccAccA,
//                                 ui->editDecDecA,
//                                 ui->editMaxSpeedA,

//                                 ui->labAOrigin,
//                                 ui->coboxOriginTypeA,
//                                 ui->editFindOriginSpeedA,
//                                 ui->editLeaveOriginSpeedA,
//                                 ui->editOriginOffsetA,
//                                 ui->coboxBackOriginDirectA,
//                                 ui->coboxBackOriginOrderA
//                             });

//    setupAxisTypeConnections(ui->coboxAxisTypeB, {
//                                 ui->labBAxisPara,
//                                 ui->editAxisMinPosB,
//                                 ui->editAxisMaxPosB,
//                                 ui->editCirclePulseNumB,
//                                 ui->editCircleDistanceB,
//                                 ui->coboxCoordDirectB,

//                                 ui->labBAxisSpeed,
//                                 ui->editAccTimeB,
//                                 ui->editDecTimeB,
//                                 ui->editAccAccB,
//                                 ui->editDecDecB,
//                                 ui->editMaxSpeedB,

//                                 ui->labBOrigin,
//                                 ui->coboxOriginTypeB,
//                                 ui->editFindOriginSpeedB,
//                                 ui->editLeaveOriginSpeedB,
//                                 ui->editOriginOffsetB,
//                                 ui->coboxBackOriginDirectB,
//                                 ui->coboxBackOriginOrderB
//                             });

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





