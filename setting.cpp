#include "setting.h"
#include "ui_setting.h"

#include <QMovie>
#include <QPair>
#include <QDebug>

#include "stackedit.h"

#define TEST 0
//#define TESTKEYBOARD 1

Setting::Setting(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Setting)
{
    ui->setupUi(this);
    initVariables();

//    ui->checkBoxPoiskie->setDisabled(true);
//    ui->checkBoxRussian->setDisabled(true);
//    ui->checkBoxTradChinese->setDisabled(true);
//    ui->checkBoxSpain->setDisabled(true);
//    ui->checkBoxSpecial->setDisabled(true);
//    ui->checkBoxVietnam->setDisabled(true);

    pageSwitchInit();

    for (int i = 0; i < 8; i++)
    {
        stack[i] = new StackEdit(ui->tabWidgetStack->widget(i));
    }

    connect(ui->coboxStackWay, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index){
        for (int i = 0; i < 8; i++)
        {
            stack[i]->switchStackWay(index);
        }
    });
#if TEST
    /////////////////////////
    for (int i = 0; i < 3; i++)
    {
        connect(tb_1_comboBox[i], QOverload<int>::of(&QComboBox::activated), this, [&, i]() {
            onComboBoxIndexChanged(i); });
    }
    for (int i = 0; i < 3; i++)
    {
        connect(tb_1_btnHead[i], &QPushButton::clicked, this, [=]() {onPushButtonHeadChanged(i); });
    }
    for (int i = 0; i < 9; i++)
    {
        connect(tb_1_btn[i], &QPushButton::clicked, this, [=]() {onPushButtonChanged(i); });
    }
    connect(tb_general_btn, &QPushButton::clicked, this, &Setting::onPushButtonGeneralClicked);
    connect(tb_general_btn, &QPushButton::clicked, this, [](){

    });



#endif


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


// to initial all variables in this function
void Setting::initVariables()
{
//    movie = new QMovie(QString(":/images/gif/test.gif"));
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

        ui->stackedWidget->setCurrentWidget(ui->pageServoSpeed);
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
    connect(ui->btnNextPageOrigin, &QPushButton::clicked, this, [=](){
        ui->stkWidgetOriginSet->setCurrentIndex((ui->stkWidgetOriginSet->currentIndex() - 1 + ui->stkWidgetOriginSet->count())
                                                 % ui->stkWidgetOriginSet->count());
    });
    connect(ui->btnNextPageOrigin, &QPushButton::clicked, this, [=](){
        ui->stkWidgetOriginSet->setCurrentIndex((ui->stkWidgetOriginSet->currentIndex() + 1 + ui->stkWidgetOriginSet->count())
                                                 % ui->stkWidgetOriginSet->count());
    });
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
    connect(ui->btnLastPageServoPara, &QPushButton::clicked, this, [=](){
        ui->stkWidgetServoPara->setCurrentIndex((ui->stkWidgetServoPara->currentIndex() - 1 + ui->stkWidgetServoPara->count())
                                                 % ui->stkWidgetServoPara->count());
        ui->labelPageNum->setText(QString("%1/2").arg(ui->stkWidgetServoPara->currentIndex() + 1));
//        ui->grboxAxisSelect->show();
    });
    connect(ui->btnNextPageServoPara, &QPushButton::clicked, this, [=](){
        ui->stkWidgetServoPara->setCurrentIndex((ui->stkWidgetServoPara->currentIndex() + 1 + ui->stkWidgetServoPara->count())
                                                 % ui->stkWidgetServoPara->count());
        ui->labelPageNum->setText(QString("%1/2").arg(ui->stkWidgetServoPara->currentIndex() + 1));
//        ui->grboxAxisSelect->show();
    });


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

#if TEST
///////////////////////////////////////////////////////////////////////
void Setting::onComboBoxIndexChanged(int index)
{
    QString selectedText = tb_1_comboBox[index]->currentText();
    tb_1_btnHead[index]->setText(selectedText);
}

void Setting::onPushButtonChanged(int index)
{
    for (int i = 0; i < 9; i++)
    {
        if (i == index)
        {
            tb_1_btn[i]->setChecked(true);
        }
        else
        {
            tb_1_btn[i]->setChecked(false);
        }
    }
    for (int i = 0; i < 3; i++)
    {
        tb_1_btnHead[i]->setChecked(true);
    }
}

void Setting::onPushButtonHeadChanged(int index)
{
    for (int i = 0; i < 3; i++)
    {
        if (i == index)
        {
            tb_1_btnHead[i]->setChecked(true);
        }
        else
        {
            tb_1_btnHead[i]->setChecked(false);
        }
    }
    for (int i = 0; i < 9; i++)
    {
        tb_1_btn[i]->setChecked(true);
    }
}

void Setting::onPushButtonGeneralClicked(int index)
{
    for (int i = 0; i < 9; i++)
    {
        tb_1_btn[i]->setChecked(true);
    }
    for (int i = 0; i < 3; i++)
    {
        tb_1_btnHead[i]->setChecked(true);
    }
}

#if 0
void Setting::setAllStyleSheet()
{
    tb_general_btn = new QPushButton();
    tb_general_btn->setStyleSheet("QPushButton{background-color: qlineargradient(x1: 0, y1: 1, x2: 0, y2: 0,stop: 0 #DCDCDC, stop: 0.7 #F8F8FF,stop : 1 #F5F5F5); }");
    ui->tableWidget_stack->setCellWidget(0, 0, tb_general_btn);
    for (int i = 1; i < 4; i++)
    {
        tb_1_btnHead[i - 1] = new QPushButton();
        tb_1_btnHead[i - 1]->setCheckable(true);
        QString strStyleSheet = QString("QPushButton{text-align : center;background-color: qlineargradient(x1: 0, y1: 1, x2: 0, y2: 0,stop: 0 #DCDCDC, stop: 0.7 #F8F8FF,stop : 1 #F5F5F5); }QPushButton::checked{ background-color: #1E90FF;font: bold; }");
        //
        tb_1_btnHead[i - 1]->setStyleSheet(strStyleSheet);
        ui->tableWidget_stack->setCellWidget(0, i, tb_1_btnHead[i - 1]);
    }
    QStringList labelStr = { "轴选择","速度","点数","A-起点","B-X方向终点","C-Y方向终点","D点坐标","放料速度","放料起始点" };
    for (int i = 1; i < 10; i++)
    {
        tb_1_btn[i - 1] = new QPushButton(labelStr[i - 1]);
        tb_1_btn[i - 1]->setCheckable(true);
        QString strStyleSheet = QString("QPushButton{text-align : left;background-color: qlineargradient(x1: 0, y1: 1, x2: 0, y2: 0,stop: 0 #DCDCDC, stop: 0.7 #F8F8FF,stop : 1 #F5F5F5); }QPushButton::checked{ background-color: #1E90FF;font: bold; }");
        //
        tb_1_btn[i - 1]->setStyleSheet(strStyleSheet);
        ui->tableWidget_stack->setCellWidget(i, 0, tb_1_btn[i - 1]);
    }

    for (int i = 1; i < 4; i++)
    {
        tb_1_comboBox[i - 1] = new QComboBox();
        QString axis;
        if (i == 1)
            axis = "X";
        else if (i == 2)
            axis = "Y";
        else
            axis = "Z";
        tb_1_comboBox[i - 1]->addItem(QString("%1").arg(axis) + "1轴");
        tb_1_comboBox[i - 1]->addItem(QString("%1").arg(axis) + "2轴");
        ui->tableWidget_stack->setCellWidget(1, i, tb_1_comboBox[i - 1]);
        tb_1_btnHead[i - 1]->setText(tb_1_comboBox[i - 1]->currentText());
        tb_1_comboBox[i - 1]->setStyleSheet("QComboBox::!editable{border: 2px solid #808A87;}QComboBox::drop-down{subcontrol-origin: padding-right;subcontrol-position: top right;width: 20px;border-left-width: 2px;border-left-color: gray; border-left-style: solid; }QComboBox::down-arrow{image: url(:/images/arrow_down_normal.png); }");
    }

    for (int i = 1; i < 4; i++)
    {
        for (int j = 2; j < 10; j++)
        {
            tb_1_lineEdit[i - 1][j - 2] = new QLineEdit();
            ui->tableWidget_stack->setCellWidget(j, i, tb_1_lineEdit[i - 1][j - 2]);

            tb_1_lineEdit[i - 1][j - 2]->setStyleSheet("border-radius: 5px;border: 1px solid #000000;");
        }
    }



}

#endif


#endif



