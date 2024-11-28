#include "manualform.h"
#include "ui_manualform.h"

#include <QPushButton>
#include <QToolButton>
#include <QHeaderView>
#include <QTabBar>
#include <QString>
#include <QDebug>

#include "mainwindow.h"

QString REFERPOINT_PIC = ":/images/referPoint.png";

ManualForm::ManualForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ManualForm)
{
    ui->setupUi(this);

    initVar();

    // set tab to be invisible
//    ui->tabAxisAction->setVisible(false);
//    ui->tabWidgetManualPage->removeTab(1);
    initControls();

    tableReferenceInit();
    pageInit();
    setupReserveWidgets();

    ui->tabGuide->installEventFilter(this);
    ui->tabReference->installEventFilter(this);
//    ui->tabReference->removeEventFilter(this);
    ui->tabWidgetManualPage->installEventFilter(this);

    for (auto cobox : findChildren<QComboBox*>())
    {
        cobox->setFocusPolicy(Qt::NoFocus);
    }

    ui->editAxisActionSpeed->setDecimalPlaces(1);
    ui->editPositionAdd->setDecimalPlaces(2);
    ui->editPositionSub->setDecimalPlaces(2);
    connect(ui->editAxisActionSpeed,&NumberEdit::textChanged,[=](const QString &){
        m_manualAxis.axis=ui->cb_axisActionAxis->currentIndex();
        m_manualAxis.speed=(uint16_t)ui->editAxisActionSpeed->text().toDouble() * 10;
        m_manualAxis.pos_pos=(uint32_t)ui->editPositionAdd->text().toDouble() * 100;
        m_manualAxis.sub_pos=(uint32_t)ui->editPositionSub->text().toDouble() * 100;
        m_manualAxis.ZDrop=ui->chbZAxisDesend->isChecked();
        setManualAxis(m_manualAxis);
    });
    connect(ui->editPositionAdd,&NumberEdit::textChanged,[=](const QString &){
        m_manualAxis.axis=ui->cb_axisActionAxis->currentIndex();
        m_manualAxis.speed=(uint16_t)ui->editAxisActionSpeed->text().toDouble() * 10;
        m_manualAxis.pos_pos=(uint32_t)ui->editPositionAdd->text().toDouble() * 100;
        m_manualAxis.sub_pos=(uint32_t)ui->editPositionSub->text().toDouble() * 100;
        m_manualAxis.ZDrop=ui->chbZAxisDesend->isChecked();
        setManualAxis(m_manualAxis);
    });
    connect(ui->editPositionSub,&NumberEdit::textChanged,[=](const QString &){
        m_manualAxis.axis=ui->cb_axisActionAxis->currentIndex();
        m_manualAxis.speed=(uint16_t)ui->editAxisActionSpeed->text().toDouble() * 10;
        m_manualAxis.pos_pos=(uint32_t)ui->editPositionAdd->text().toDouble() * 100;
        m_manualAxis.sub_pos=(uint32_t)ui->editPositionSub->text().toDouble() * 100;
        m_manualAxis.ZDrop=ui->chbZAxisDesend->isChecked();
    });
    connect(ui->chbZAxisDesend, QOverload<int>::of(&QCheckBox::stateChanged), [=](int){
        m_manualAxis.axis=ui->cb_axisActionAxis->currentIndex();
        m_manualAxis.speed=(uint16_t)ui->editAxisActionSpeed->text().toDouble() * 10;
        m_manualAxis.pos_pos=(uint32_t)ui->editPositionAdd->text().toDouble() * 100;
        m_manualAxis.sub_pos=(uint32_t)ui->editPositionSub->text().toDouble() * 100;
        m_manualAxis.ZDrop=ui->chbZAxisDesend->isChecked();
    });

    connect(ui->handPosAdd,&QPushButton::clicked,this,[=](){
        g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_INCREMENT,m_manualAxis.axis,2);
    });
    connect(ui->HandPosDown,&QPushButton::clicked,this,[=](){
        g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_INCREMENT,m_manualAxis.axis,1);
    });
    connect(ui->btnIntoStack, &QPushButton::clicked, this , [=](){
        emit sigShowStackPage();
    });
    /******按钮控制输出下发槽函数******/
    connect(ui->btnRawMaterial1Loosen,&QPushButton::clicked,this,[=](){//原料1松开/夹紧
        if(ui->btnRawMaterial1Loosen->text() == tr("原料1松开"))
        {//原料1松开
            setbuttonIcon(ui->btnRawMaterial1Loosen,tr("原料1夹紧"),1);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[CLAW_METERIAL_1_CLAMP].actualPortNum,1);
        }
        else
        {//原料1夹紧
            setbuttonIcon(ui->btnRawMaterial1Loosen,tr("原料1松开"),0);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[CLAW_METERIAL_1_CLAMP].actualPortNum,0);
        }
    });
    connect(ui->btnFinishedProduct1Loosen,&QPushButton::clicked,this,[=](){//成品1松开/夹紧
        if(ui->btnFinishedProduct1Loosen->text() == tr("成品1松开"))
        {
            setbuttonIcon(ui->btnFinishedProduct1Loosen,tr("成品1夹紧"),1);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[CLAW_PRODUCT_1_CLAMP].actualPortNum,1);
        }
        else
        {
            setbuttonIcon(ui->btnFinishedProduct1Loosen,tr("成品1松开"),0);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[CLAW_PRODUCT_1_CLAMP].actualPortNum,0);
        }
    });
    connect(ui->btnClaw1Reverse,&QPushButton::clicked,this,[=](){//卡爪1反转/正转
        if(ui->btnClaw1Reverse->text() == tr("卡爪1反转"))
        {
            setbuttonIcon(ui->btnClaw1Reverse,tr("卡爪1正转"),1);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[CLAW_CLAW_1_CLAMP].actualPortNum,1);
        }
        else
        {
            setbuttonIcon(ui->btnClaw1Reverse,tr("卡爪1反转"),0);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[CLAW_CLAW_1_CLAMP].actualPortNum,0);
        }

    });
    connect(ui->btnBlow1Loosen,&QPushButton::clicked,this,[=](){//吹气1断/通
        if(ui->btnBlow1Loosen->text() == tr("吹气1断"))
        {
            setbuttonIcon(ui->btnBlow1Loosen,tr("吹气1通"),1);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[MACHINE_GASS_1_LOOSENED].actualPortNum,1);
        }
        else
        {
            setbuttonIcon(ui->btnBlow1Loosen,tr("吹气1断"),0);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[MACHINE_GASS_1_LOOSENED].actualPortNum,0);
        }

    });
    connect(ui->btnChuck1Loosen,&QPushButton::clicked,this,[=](){//卡盘1松开/夹紧
        if(ui->btnChuck1Loosen->text() == tr("卡盘1松开"))
        {
            setbuttonIcon(ui->btnChuck1Loosen,tr("卡盘1夹紧"),1);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[MACHINE_CHUCK_1_CLAMP].actualPortNum,1);
        }
        else
        {
            setbuttonIcon(ui->btnChuck1Loosen,tr("卡盘1松开"),0);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[MACHINE_CHUCK_1_CLAMP].actualPortNum,0);
        }

    });
    connect(ui->btnAutoGate1Close,&QPushButton::clicked,this,[=](){//自动门关/开
        if(ui->btnAutoGate1Close->text() == tr("自动门1关"))
        {
            setbuttonIcon(ui->btnAutoGate1Close,tr("自动门1开"),1);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[MACHINE_AUTO_DOOR_1_OPEN].actualPortNum,1);
        }
        else
        {
            setbuttonIcon(ui->btnAutoGate1Close,tr("自动门1关"),0);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[MACHINE_AUTO_DOOR_1_OPEN].actualPortNum,0);
        }

    });
    connect(ui->btnStartProcess1Break,&QPushButton::clicked,this,[=](){//启动加工1断/通
        if(ui->btnStartProcess1Break->text() == tr("启动加工1断"))
        {
            setbuttonIcon(ui->btnStartProcess1Break,tr("启动加工1通"),1);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[MACHINE_START_PROCESS_1].actualPortNum,1);
        }
        else
        {
            setbuttonIcon(ui->btnStartProcess1Break,tr("启动加工1断"),0);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[MACHINE_START_PROCESS_1].actualPortNum,0);
        }
    });
    connect(ui->btnMainAxisLocate1Break,&QPushButton::clicked,this,[=](){//主轴定位1断/通
        if(ui->btnMainAxisLocate1Break->text() == tr("主轴定位1断"))
        {
            setbuttonIcon(ui->btnMainAxisLocate1Break,tr("主轴定位1通"),1);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[MACHINE_SPINDLE_FIXED_POS_1].actualPortNum,1);
        }
        else
        {
            setbuttonIcon(ui->btnMainAxisLocate1Break,tr("主轴定位1断"),0);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[MACHINE_SPINDLE_FIXED_POS_1].actualPortNum,0);
        }
    });
    connect(ui->btnControlRotate1Break,&QPushButton::clicked,this,[=](){//主轴旋转1断/通
        if(ui->btnControlRotate1Break->text() == tr("主轴旋转1断"))
        {
            setbuttonIcon(ui->btnControlRotate1Break,tr("主轴旋转1通"),1);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[MACHINE_SPINDLE_ROTATE_1].actualPortNum,1);
        }
        else
        {
            setbuttonIcon(ui->btnControlRotate1Break,tr("主轴旋转1断"),0);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[MACHINE_SPINDLE_ROTATE_1].actualPortNum,0);
        }
    });

#if 0
    connect(ui->btnReserveY7Break,&QPushButton::clicked,this,[=](){
        if(ui->btnReserveY7Break->text() == tr("Y7自动灯断"))
        {
            setbuttonIcon(ui->btnReserveY7Break,tr("Y7自动灯通"),1);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[7-1].actualPortNum,1);
        }
        else
        {
            setbuttonIcon(ui->btnReserveY7Break,tr("Y7自动灯断"),0);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[7-1].actualPortNum,0);
        }
    });
    connect(ui->btnReserveY8Break,&QPushButton::clicked,this,[=](){
        if(ui->btnReserveY8Break->text() == tr("Y8报警灯断"))
        {
            setbuttonIcon(ui->btnReserveY8Break,tr("Y8报警灯通"),1);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[8-1].actualPortNum,1);
        }
        else
        {
            setbuttonIcon(ui->btnReserveY8Break,tr("Y8报警灯断"),0);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[8-1].actualPortNum,0);
        }
    });
    connect(ui->btnReserveY13Break,&QPushButton::clicked,this,[=](){
        if(ui->btnReserveY13Break->text() == tr("Y13急停输出断"))
        {
            setbuttonIcon(ui->btnReserveY13Break,tr("Y13急停输出通"),1);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[13-1].actualPortNum,1);
        }
        else
        {
            setbuttonIcon(ui->btnReserveY13Break,tr("Y13急停输出断"),0);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[13-1].actualPortNum,0);
        }
    });
    connect(ui->btnReserveY14Break,&QPushButton::clicked,this,[=](){
        if(ui->btnReserveY14Break->text() == tr("Y14润滑出断"))
        {
            setbuttonIcon(ui->btnReserveY14Break,tr("Y14润滑出通"),1);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[14-1].actualPortNum,1);
        }
        else
        {
            setbuttonIcon(ui->btnReserveY14Break,tr("Y14润滑出断"),0);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[14-1].actualPortNum,0);
        }
    });
    connect(ui->btnReserveY15Break,&QPushButton::clicked,this,[=](){
        if(ui->btnReserveY15Break->text() == tr("Y15报警灯断"))
        {
            setbuttonIcon(ui->btnReserveY15Break,tr("Y15报警灯通"),1);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[15-1].actualPortNum,1);
        }
        else
        {
            setbuttonIcon(ui->btnReserveY15Break,tr("Y15报警灯断"),0);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[15-1].actualPortNum,0);
        }
    });
    connect(ui->btnReserveY16Break,&QPushButton::clicked,this,[=](){
        if(ui->btnReserveY16Break->text() == tr("Y16暂停灯断"))
        {
            setbuttonIcon(ui->btnReserveY16Break,tr("Y16暂停灯通"),1);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[16-1].actualPortNum,1);
        }
        else
        {
            setbuttonIcon(ui->btnReserveY16Break,tr("Y16暂停灯断"),0);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[16-1].actualPortNum,0);
        }
    });
    connect(ui->btnReserveY19Break,&QPushButton::clicked,this,[=](){
        if(ui->btnReserveY19Break->text() == tr("Y19加工安全1断"))
        {
            setbuttonIcon(ui->btnReserveY19Break,tr("Y19加工安全1通"),1);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[19-1].actualPortNum,1);
        }
        else
        {
            setbuttonIcon(ui->btnReserveY19Break,tr("Y19加工安全1断"),0);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[19-1].actualPortNum,0);
        }
    });
    connect(ui->btnReserveY22Break,&QPushButton::clicked,this,[=](){
        if(ui->btnReserveY22Break->text() == tr("Y22断"))
        {
            setbuttonIcon(ui->btnReserveY22Break,tr("Y22通"),1);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[22-1].actualPortNum,1);
        }
        else
        {
            setbuttonIcon(ui->btnReserveY22Break,tr("Y22断"),0);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[22-1].actualPortNum,0);
        }
    });
    connect(ui->btnReserveY23Break,&QPushButton::clicked,this,[=](){
        if(ui->btnReserveY23Break->text() == tr("Y23断"))
        {
            setbuttonIcon(ui->btnReserveY23Break,tr("Y23通"),1);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[23-1].actualPortNum,1);
        }
        else
        {
            setbuttonIcon(ui->btnReserveY23Break,tr("Y23断"),0);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[23-1].actualPortNum,0);
        }
    });
    connect(ui->btnReserveY24Break,&QPushButton::clicked,this,[=](){
        if(ui->btnReserveY24Break->text() == tr("Y24断"))
        {
            setbuttonIcon(ui->btnReserveY24Break,tr("Y24通"),1);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[24-1].actualPortNum,1);
        }
        else
        {
            setbuttonIcon(ui->btnReserveY24Break,tr("Y24断"),0);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[24-1].actualPortNum,0);
        }
    });
    connect(ui->btnReserveY1Break,&QPushButton::clicked,this,[=](){
        if(ui->btnReserveY1Break->text() == tr("Y1原料1夹紧断"))
        {
            setbuttonIcon(ui->btnReserveY1Break,tr("Y1原料1夹紧通"),1);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[1-1].actualPortNum,1);
        }
        else
        {
            setbuttonIcon(ui->btnReserveY1Break,tr("Y1原料1夹紧断"),0);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[1-1].actualPortNum,0);
        }
    });
    connect(ui->btnReserveY3Break,&QPushButton::clicked,this,[=](){
        if(ui->btnReserveY3Break->text() == tr("Y3成品1夹紧断"))
        {
            setbuttonIcon(ui->btnReserveY3Break,tr("Y3成品1夹紧通"),1);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[3-1].actualPortNum,1);
        }
        else
        {
            setbuttonIcon(ui->btnReserveY3Break,tr("Y3成品1夹紧断"),0);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[3-1].actualPortNum,0);
        }
    });
    connect(ui->btnReserveY5Break,&QPushButton::clicked,this,[=](){
        if(ui->btnReserveY5Break->text() == tr("Y5卡爪1正转断"))
        {
            setbuttonIcon(ui->btnReserveY5Break,tr("Y5卡爪1正转通"),1);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[5-1].actualPortNum,1);
        }
        else
        {
            setbuttonIcon(ui->btnReserveY5Break,tr("Y5卡爪1正转断"),0);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[5-1].actualPortNum,0);
        }
    });
    connect(ui->btnReserveY9Break,&QPushButton::clicked,this,[=](){
        if(ui->btnReserveY9Break->text() == tr("Y9自动门1断"))
        {
            setbuttonIcon(ui->btnReserveY9Break,tr("Y9自动门1通"),1);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[9-1].actualPortNum,1);
        }
        else
        {
            setbuttonIcon(ui->btnReserveY9Break,tr("Y9自动门1断"),0);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[9-1].actualPortNum,0);
        }
    });
    connect(ui->btnReserveY20Break,&QPushButton::clicked,this,[=](){
        if(ui->btnReserveY20Break->text() == tr("Y20卡盘1夹紧断"))
        {
            setbuttonIcon(ui->btnReserveY20Break,tr("Y20卡盘1夹紧通"),1);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[20-1].actualPortNum,1);
        }
        else
        {
            setbuttonIcon(ui->btnReserveY20Break,tr("Y20卡盘1夹紧断"),0);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[20-1].actualPortNum,0);
        }
    });
#endif
    connect(ui->chbMachineControl,&QCheckBox::clicked,this,[=](){//机床控制使能复选框
        if(!ui->chbMachineControl->isChecked())
        {
            ui->btnAutoGate1Close->setEnabled(true);
            ui->btnStartProcess1Break->setEnabled(false);
            ui->btnMainAxisLocate1Break->setEnabled(false);
            ui->btnControlRotate1Break->setEnabled(false);
        }
        else
        {
            ui->btnAutoGate1Close->setEnabled(true);
            ui->btnStartProcess1Break->setEnabled(true);
            ui->btnMainAxisLocate1Break->setEnabled(true);
            ui->btnControlRotate1Break->setEnabled(true);
        }
    });

    StateButtonInit();//初始化相关状态按钮状态

    /*********************************操作指引逻辑********************************************/
    connect(ui->editGuideKeyDef, &KeyEdit::saveKeyDef, this, &ManualForm::onSaveKeyDef);

    connect(ui->btnHideGuide, &QPushButton::toggled, this, [=](bool checked){

        ui->checkBoxEditPosGuide->setVisible(checked);
        ui->btnImportPictureGuide->setVisible(checked);
        ui->btnNewButton->setVisible(checked);
        ui->btnDeleteButton->setVisible(checked);
    });
    /*********************************参考点逻辑********************************************/
    connect(ui->btnHideReference, &QPushButton::toggled, this, [=](bool checked){

        ui->checkBoxEditPosReference->setVisible(checked);
        ui->btnImportPictureReference->setVisible(checked);
        ui->btnNewButtonReference->setVisible(checked);
        ui->btnDeleteButtonReference->setVisible(checked);
    });

}

ManualForm::~ManualForm()
{
    delete ui;
    qDeleteAll(guidePoints.keys());
}

void ManualForm::initControls()
{
    ui->cb_axisActionAxis->setCurrentIndex(m_manualAxis.axis);
    ui->editAxisActionSpeed->setText(QString::number(m_manualAxis.speed/10.0));
    ui->editPositionAdd->setText(QString::number(m_manualAxis.pos_pos/100.0));
    ui->editPositionSub->setText(QString::number(m_manualAxis.sub_pos/100.0));
    ui->chbZAxisDesend->setChecked(m_manualAxis.ZDrop);
}

void ManualForm::on_btnNewButton_clicked()
{
    if (guidePoints.size() < GUIDE_TOTAL_NUM)
    {
        DraggableButton* btn = new DraggableButton(ui->tabGuide);
        btn->setDraggable(draggable[0]);
        btn->setCheckable(draggable[0]);
        btn->setAutoExclusive(true);

        btn->setFixedSize(120, 40);
        QPoint btnPos = QPoint(20 + 120 * (guidePoints.size() / 8), 20 + 50 * (guidePoints.size() % 8));
        btn->setGeometry(QRect(btnPos, btn->size()));
        QString btnName = tr((QString("按钮") + QString::number(guidePoints.size() + 1)).toUtf8().constData());
        btn->setText(btnName);
        btn->show();

        // Create GuidePara for the new button
        GuidePara newGuidePara = { btnName, "0", btnPos, 0, 0 ,0};

        // Insert into QHash
        guidePoints.insert(btn, newGuidePara);

        // Connect button pressed signal
        connect(btn, &DraggableButton::pressed, this, [=]() {
            selectedButton[0] = btn;
            selectedButton[0]->setChecked(draggable[0]);

            // Set the guide info in the UI as needed
            if (guidePoints.contains(selectedButton[0])) {
                GuidePara para = guidePoints.value(selectedButton[0]);
                // update correspongding keyDef name in UI，
                 ui->editGuideKeyDef->setText(para.keyDefStr);
            }
        });
        connect(btn, &DraggableButton::clicked, this,[=](){
            selectedButton[0] = btn;
            if (guidePoints.contains(selectedButton[0])) {
                GuidePara para = guidePoints.value(selectedButton[0]);
                if(para.keyType == 0)
                {//0阀输出 1输出 2轴
                    g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_INCREMENT,m_manualAxis.axis,1);
                }
                else if(para.keyType == 1)
                {

                }
                else if(para.keyType == 2)
                {

                }

            }
//            qDebug() << "按键%s被按下!"<<btn->text();
        });

        connect(btn, &DraggableButton::positionChanged, [=]() {
            auto it = guidePoints.find(selectedButton[0]);
            if (it != guidePoints.end()) {
                it.value().btnPos = btnPos;
                ui->btnSaveGuide->setParaChangedFlag(true);
            }
        });
        ui->btnSaveGuide->setParaChangedFlag(true);;
    }
}

void ManualForm::on_btnDeleteButton_clicked()
{
    if (selectedButton[0] && guidePoints.contains(selectedButton[0]))
    {
        QString guideName = guidePoints.value(selectedButton[0]).guideName;
        int reply =  MainWindow::pMainWindow->showErrorTip(tr("确认删除：%1").arg(guideName));
        if (reply == QDialog::Accepted)
        {
           // Remove the button and corresponding GuidePara
           guidePoints.remove(selectedButton[0]);
           delete selectedButton[0];

           ui->btnSaveGuide->setParaChangedFlag(true);
       }
    }

    if (guidePoints.isEmpty())
    {
        selectedButton[0] = nullptr;
    }
    else
    {
        DraggableButton* btn = guidePoints.keys().first();
        btn->setChecked(true);

        selectedButton[0] = btn;
    }
}

void ManualForm::on_checkBoxEditPosGuide_stateChanged(int arg1)
{
    // Update draggable state
    draggable[0] = arg1;

    // Iterate over all buttons in guidePoints
    for (auto btn : guidePoints.keys())
    {
        btn->setDraggable(draggable[0]);
        // Optionally, set the buttons to be checkable as well
        btn->setCheckable(draggable[0]);
    }

    selectedButton[0]->setChecked(draggable[0]);
}

void ManualForm::on_btnNewButtonReference_clicked()
{
    if (referencePoints.size() < REFERENCE_TOTAL_NUM)
    {
        addReferencePoint();
        updateReferPointsTable();
        ui->btnSaveReference->setParaChangedFlag(true);
    }
}

void ManualForm::on_btnDeleteButtonReference_clicked()
{
    if (!referencePoints.isEmpty())
    {
        if (selectedButton[1])
        {
            auto it = std::find_if(referencePoints.begin(), referencePoints.end(), [=](const ReferPointPara& point) {
                return point.button == selectedButton[1];
            });
            if (it != referencePoints.end())
            {
                QString referName = it->name;
                int reply = MainWindow::pMainWindow->showErrorTip(tr("确定删除：%1").arg(referName));
                if (reply == QDialog::Accepted)
                {
                    removeReferencePoint();
                    updateReferPointsTable();
                    ui->btnSaveReference->setParaChangedFlag(true);
                }
            }
        }
    }
}

void ManualForm::updateReferPointsTable()
{
    tableReference->clearContents();
    tableReference->setRowCount(referencePoints.count() / 4 + 1);

    QList<ReferPointPara> sortedPoints = referencePoints;
    std::sort(sortedPoints.begin(), sortedPoints.end(), [](const ReferPointPara &a, const ReferPointPara &b) {
        return a.index < b.index;
    });

    int num = 0;
    for (const auto &point : sortedPoints)
    {
        int row = num / 4;
        int col = (num % 4) * 2;
        num++;

        QTableWidgetItem *indexItem = new QTableWidgetItem(QString::number(point.index));
        indexItem->setFlags(indexItem->flags() & ~Qt::ItemIsEnabled);
        indexItem->setForeground(QBrush(Qt::black));
        tableReference->setItem(row, col, indexItem);

        QTableWidgetItem *nameItem = new QTableWidgetItem(point.name);
        tableReference->setItem(row, col + 1, nameItem);
    }

    for (int row = 0; row < tableReference->rowCount(); ++row)
    {
        for (int col = 0; col < tableReference->columnCount(); ++col)
        {
            if (!tableReference->item(row, col))
            {
                QTableWidgetItem *emptyItem = new QTableWidgetItem("");
                emptyItem->setFlags(emptyItem->flags() & ~(Qt::ItemIsSelectable | Qt::ItemIsEnabled));
                tableReference->setItem(row, col, emptyItem);
            }
        }
    }

    tableReference->setVisible(!referencePoints.isEmpty());
    // 选中当前checked button 对应的item
    if (selectedButton[1] == nullptr) return;

    int index = selectedButton[1]->text().toInt();
    for (int row = 0; row < tableReference->rowCount(); ++row)
    {
        for (int col = 0; col < tableReference->columnCount(); ++col)
        {
            QTableWidgetItem *indexItem = tableReference->item(row, col);
            if (indexItem && indexItem->data(Qt::DisplayRole).toInt() == index)
            {
                tableReference->setCurrentCell(row, col + 1);
                break;
            }
        }
    }
}

void ManualForm::on_checkBoxEditPosReference_stateChanged(int arg1)
{
    draggable[1] = arg1;

    for (auto point : referencePoints)
    {
        point.button->setDraggable(draggable[1]);
    }
}


void ManualForm::tableReferenceInit()
{
    tableReference = new QTableWidget(ui->frameRerencePoint);

    // Set table to be uneditable
    tableReference->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // Set table to be unselectable
//    tableReference->setSelectionMode(QAbstractItemView::NoSelection);
    // Initialize table to be invisible
    tableReference->setVisible(referencePoints.size());
    tableReference->setFrameShape(QFrame::NoFrame);
    tableReference->setFrameShadow(QFrame::Raised);

    tableReference->setMaximumHeight(40 * 3);
    tableReference->setFixedWidth(760);

    tableReference->setColumnCount(8); // 4 pairs of index and button name
    tableReference->verticalHeader()->setVisible(false); // Hide vertical header
    tableReference->horizontalHeader()->setVisible(false); // Hide horizontal header


    tableReference->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);// Hide scroll bar
    tableReference->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Disable automatic column resizing
//    tableReference->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    // must set minimum section size for horizontalHeader, otherwise setColumnWidth can not work
    tableReference->horizontalHeader()->setMinimumSectionSize(30);
    tableReference->verticalHeader()->setDefaultSectionSize(40);
    for (int i = 0; i < tableReference->columnCount(); ++i) {
        tableReference->setColumnWidth(i, (i % 2 == 0) ? 40 : 140); // Alternate between 40 and 140 width
    }

    connect(tableReference, &QTableWidget::itemPressed, this, [=](QTableWidgetItem *item) {
        int row = item->row();
        int col = item->column() - 1;
        QTableWidgetItem *indexItem = tableReference->item(row, col);
        int index = indexItem->data(Qt::DisplayRole).toInt();
//        if (indexItem && indexItem->data(Qt::DisplayRole).toInt() == index)

        auto it = std::find_if(referencePoints.begin(), referencePoints.end(), [=](const ReferPointPara& point){
            return point.index == index;
        });
        if (it != referencePoints.end())
        {
            DraggableButton* btn =  it->button;
            btn->setChecked(true);
            selectedButton[1] = btn;
            QString name = it->name;
            ui->textReferPointName->setText(name);
        }
    });

}

void ManualForm::pageInit()
{
    ui->checkBoxEditPosGuide->setVisible(false);
    ui->btnImportPictureGuide->setVisible(false);
    ui->btnNewButton->setVisible(false);
    ui->btnDeleteButton->setVisible(false);

    ui->checkBoxEditPosReference->setVisible(false);
    ui->btnImportPictureReference->setVisible(false);
    ui->btnNewButtonReference->setVisible(false);
    ui->btnDeleteButtonReference->setVisible(false);

    ui->tableWgtAxisPos->setHorizontalHeaderLabels({tr("轴位置")});

    ui->labelGuideBackgroud->setPixmap(QPixmap(""));
    QPixmap picReferPoint(REFERPOINT_PIC);
    picReferPoint.scaled(ui->labReferPointBackGround->size());
    ui->labReferPointBackGround->setPixmap(picReferPoint);

    ui->labelGuideBackgroud->setPixmap(QPixmap(":/images/guide.png"));

    ui->btnAutoGate1Close->setEnabled(true);
    ui->btnStartProcess1Break->setEnabled(false);
    ui->btnMainAxisLocate1Break->setEnabled(false);
    ui->btnControlRotate1Break->setEnabled(false);

}

int ManualForm::getNextAvailableIndex()
{
    QSet<int> usedIndices;
    for (const auto &point : referencePoints)
    {
        usedIndices.insert(point.index);
    }
    for (int i = 1; i <= REFERENCE_TOTAL_NUM; ++i)
    {
        if (!usedIndices.contains(i))
        {
            return i;
        }
    }
    return 0;
}

void ManualForm::updateGuidePoints()
{

    for (auto it = guidePoints.begin(); it != guidePoints.end(); ++it)
    {
        DraggableButton* btn = it.key();
        if (btn)
        {
            delete btn;
            btn = nullptr;
        }
    }
    guidePoints.clear();

    for (int i = 0; i < GUIDE_TOTAL_NUM; i++)
    {
        if (m_Guide[i].guideFlag == false) break; // 操作点无效

        QString guideName = m_Guide[i].guideName;
        QString keyDefStr = m_Guide[i].keyDefStr;
        QPoint btnPos =  m_Guide[i].btnPos;
        uint8_t keyType = m_Guide[i].keyType;
        uint8_t portNum = m_Guide[i].portNum;
        uint8_t status = m_Guide[i].status;
        GuidePara guide { guideName, keyDefStr, btnPos, keyType, portNum, status };

        DraggableButton* btn = new DraggableButton(ui->tabGuide);
        guidePoints.insert(btn, guide);

        connect(btn, &DraggableButton::pressed, this, [=]() {
            selectedButton[0] = btn;
            selectedButton[0]->setChecked(draggable[0]);

            // Set the guide info in the UI as needed
            if (guidePoints.contains(selectedButton[0])) {
                GuidePara para = guidePoints.value(selectedButton[0]);
                // update correspongding keyDef name in UI，
                 ui->editGuideKeyDef->setText(para.keyDefStr);
            }
        });
        connect(btn, &DraggableButton::positionChanged, [=]() {
            QPoint btnPos = btn->pos();
            auto it = guidePoints.find(selectedButton[0]);
            if (it != guidePoints.end()) {
                it.value().btnPos = btnPos;
                ui->btnSaveGuide->setParaChangedFlag(true);;
            }
        });
    }

    for (auto it = guidePoints.begin(); it != guidePoints.end(); ++it)
    {
        DraggableButton* btn = it.key();
        GuidePara guide = it.value();

        if (btn) {
//            bool state = ui->checkBoxEditPosGuide->isChecked();
            btn->setDraggable(draggable[0]);
            btn->setText(guide.guideName);
            btn->setGeometry(QRect(guide.btnPos, QSize(120, 45)));
        }
    }

}

void ManualForm::updateReferPointsList()
{
    for (auto& point : referencePoints)
    {
        delete point.button;
        point.button = nullptr;
    }
    referencePoints.clear();

    for (int i = 0; i < REFERENCE_TOTAL_NUM; i++)
    {
        if (true == m_RefPoint[i].refFlag)
        {
            QString referName = m_RefPoint[i].refName;
            int refIndex = i + 1;
            DraggableButton* btn = new DraggableButton(ui->frameRerencePoint);
            QPoint btnPos(m_RefPoint[i].xPos, m_RefPoint[i].yPos);
            ReferPointPara point = {refIndex, referName, btn, btnPos};
            referencePoints.push_back(point);

            btn->setCheckable(true);
            btn->setAutoExclusive(true);
            btn->setDraggable(draggable[1]);
            btn->setFixedSize(40, 40);
            btn->setStyleSheet("QPushButton{ border-style:solid; outline:none; border-width:1px; border-radius:20px;}");

            btn->move(point.pointPos);
            btn->setText(QString::number(point.index));
            btn->show();
            connect(btn, &DraggableButton::positionChanged, this, [=](){
                auto it = std::find_if(referencePoints.begin(), referencePoints.end(), [=](const ReferPointPara& point) {
                    return point.button == btn;
                });
                if (it != referencePoints.end())
                {
                    it->pointPos = btn->pos();
                    ui->btnSaveReference->setParaChangedFlag(true);;
                }
            });

            connect(btn, &DraggableButton::pressed, this, [=]() {
                selectedButton[1] = btn;

                int index = 0;
                auto it = std::find_if(referencePoints.begin(), referencePoints.end(), [=](const ReferPointPara& p) {
                    return p.button == selectedButton[1];
                });
                if (it != referencePoints.end()) {
                    ui->textReferPointName->setText(it->name);
                    index = it->index;
                }
                for (int row = 0; row < tableReference->rowCount(); ++row) {
                    for (int col = 0; col < tableReference->columnCount(); ++col) {
                        QTableWidgetItem *indexItem = tableReference->item(row, col);
                        if (indexItem && indexItem->data(Qt::DisplayRole).toInt() == index) {
                            tableReference->setCurrentCell(row, col + 1);
                            return;
                        }
                    }
                }
            }, Qt::UniqueConnection);
        }
    }
    if (referencePoints.isEmpty())
    {
        selectedButton[1] = nullptr;
    }
    else
    {
        selectedButton[1] = referencePoints.at(0).button;
        selectedButton[1]->setChecked(true);
        emit selectedButton[1]->pressed();
    }

    updateReferPointsTable();

}

void ManualForm::setupReserveWidgets()
{
    for (auto vlayout : ui->tabReserve->findChildren<QVBoxLayout*>())
    {
        vlayout->setAlignment(Qt::AlignTop);
    }
    for (auto hlayout : ui->tabReserve->findChildren<QHBoxLayout*>())
    {
        hlayout->setAlignment(Qt::AlignLeft);
    }

//    ui->tabWgtReserve->removeTab(1);
    ui->tabWgtReserve->setTabEnabled(1, false);

    const std::vector<QPushButton*> group1 = {
        ui->btnPortY1, ui->btnPortY2, ui->btnPortY3, ui->btnPortY4,
        ui->btnPortY5, ui->btnPortY6, ui->btnPortY7, ui->btnPortY8};
    const std::vector<QPushButton*> group2 = {
        ui->btnPortY9, ui->btnPortY10, ui->btnPortY11, ui->btnPortY12,
        ui->btnPortY13, ui->btnPortY14, ui->btnPortY15, ui->btnPortY16};
    const std::vector<QPushButton*> group3 = {
        ui->btnPortY17, ui->btnPortY18, ui->btnPortY19, ui->btnPortY20,
        ui->btnPortY21, ui->btnPortY22, ui->btnPortY23, ui->btnPortY24};

    const std::vector<std::pair<std::vector<QPushButton*>, QGroupBox*>> buttonGroups = {
        {group1, ui->grboxReserveMb1},
        {group2, ui->grboxReserveMb2},
        {group3, ui->grboxReserveMb3},
    };

    for (size_t i = 0; i < reserveButtons.size(); i++)
    {
        auto btn = reserveButtons[i];
        setbuttonIcon(btn, btn->text(), 0);
        connect(btn, &QPushButton::clicked, this, [=](){
            const QString& portName = m_Port_Y[i].functionSet == 0 ? m_Port_Y[i].ResModifyName : m_Port_Y[i].modifyName;

            if (m_OutPortSta[i])
            {
                setbuttonIcon(btn, portName + tr("断"), 0);
                g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL, CMD_SUN_MANUAL_OUT, m_Port_Y[i].actualPortNum, 0);
            }
            else
            {
                setbuttonIcon(btn, portName + tr("通"), 1);
                g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL, CMD_SUN_MANUAL_OUT, m_Port_Y[i].actualPortNum, 1);
            }
        });
    }

    for (const auto& group : buttonGroups)
    {
        for (auto btn : group.first)
        {

        }
    }
}

void ManualForm::updateGroupBoxVisibility(const std::vector<std::pair<std::vector<QPushButton *>, QGroupBox *> > &buttonGroups)
{
    for (const auto& group : buttonGroups) {
        bool anyButtonVisible = false;

        for (auto btn : group.first) {
            if (btn->isVisible()) {
                anyButtonVisible = true;
                break;
            }
        }

        group.second->setVisible(anyButtonVisible);
    }
}

const QList<ReferPointPara> &ManualForm::getRerferPoints() const
{
    return referencePoints;
}

void ManualForm::handleLoginModeChanged(LoginMode mode)
{
    if (mode == LoginMode::Operator)
    {
        ui->tabWidgetManualPage->removeTab(4);
        bool canUse = false;

        ui->btnIntoStack->setEnabled(canUse);
        ui->checkBoxEditPosReference->setVisible(canUse);
        ui->btnImportPictureReference->setVisible(canUse);
        ui->btnNewButtonReference->setVisible(canUse);
        ui->btnDeleteButtonReference->setVisible(canUse);
        ui->btnEditGuideName->setVisible(canUse);
        ui->editGuideKeyDef->setVisible(canUse);
        ui->btnSaveGuide->setVisible(canUse);
    }
    else
    {
        bool canUse = true;

        ui->tabWidgetManualPage->addTab(ui->tabReference, tr("参考点"));
        ui->btnIntoStack->setEnabled(canUse);
    }
}

void ManualForm::updateReserveButtonState()
{
    for (size_t i = 0; i < reserveButtons.size(); ++i)
    {
        const auto& port = m_Port_Y[i];
        auto button = reserveButtons[i];

        bool isVisible = (port.functionSet == 0);
        button->setVisible(isVisible);

        const QString& portName = (port.functionSet == 0) ? port.ResDefineName : port.defineName;

        QString statusText = m_OutPortSta[i] ? tr("通") : tr("断");

        button->setText(portName + statusText);
    }

}

void ManualForm::addReferencePoint()
{
    DraggableButton* btn = new DraggableButton(ui->frameRerencePoint);
    btn->setCheckable(true);
    btn->setAutoExclusive(true);
    btn->setDraggable(draggable[1]);
    btn->setFixedSize(40, 40);
    btn->setStyleSheet("QPushButton{ border-style:solid; outline:none; border-width:1px; border-radius:20px;}");

    int newIndex = getNextAvailableIndex();
    if (newIndex == 0)
    {
        return;
    }
    QString referName = QString("参考点%1").arg(newIndex);
    int posIndex = newIndex - 1;
    QPoint btnPos = QPoint(10 + 42 * (posIndex / 7), 120 + 42 * (posIndex % 7));

    ReferPointPara point = {newIndex, referName, btn, btnPos};
    referencePoints.append(point);

    btn->setGeometry(QRect(btnPos, btn->size()));
    btn->setText(QString::number(point.index));
    btn->show();

    // this is used to update point geometry pos in ui
    connect(btn, &DraggableButton::positionChanged, this, [=](){
        auto it = std::find_if(referencePoints.begin(), referencePoints.end(), [=](const ReferPointPara& point) {
            return point.button == btn;
        });
        if (it != referencePoints.end())
        {
            it->pointPos = btn->pos();
            ui->btnSaveReference->setParaChangedFlag(true);
        }
    });

    connect(btn, &DraggableButton::pressed, this, [=]() {
        selectedButton[1] = btn;

        int index = 0;
        auto it = std::find_if(referencePoints.begin(), referencePoints.end(), [=](const ReferPointPara& point) {
            return point.button == selectedButton[1];
        });
        if (it != referencePoints.end())
        {
            ui->textReferPointName->setText(it->name);
            index = it->index;
        }
        for (int row = 0; row < tableReference->rowCount(); ++row)
        {
            for (int col = 0; col < tableReference->columnCount(); ++col)
            {
                QTableWidgetItem *indexItem = tableReference->item(row, col);
                if (indexItem && indexItem->data(Qt::DisplayRole).toInt() == index)
                {
//                        qDebug() << "Found item with armedIndex:" << index
//                                 << " at row:" << row
//                                 << " col:" << col;
                    tableReference->setCurrentCell(row, col + 1);
//                        tableReference->itemPressed(tableReference->itemAt(row, col + 1));
                    return ;
                }
            }
        }
    });

    if (selectedButton[1] == nullptr)
    {
       selectedButton[1] = btn;
       btn->setChecked(true);
       emit btn->pressed();
    }
}

void ManualForm::removeReferencePoint()
{
    if (selectedButton[1])
    {
        // 找到与 selectedButton[1] 关联的 ReferPointPara
        auto it = std::find_if(referencePoints.begin(), referencePoints.end(), [=](const ReferPointPara& point) {
            return point.button == selectedButton[1];
        });

        if (it != referencePoints.end())
        {
            delete it->button;
            referencePoints.erase(it);
            selectedButton[1] = nullptr;
        }
    }
    if (!referencePoints.isEmpty())
    {
        selectedButton[1] = referencePoints.last().button;
        selectedButton[1]->setChecked(true);
        emit selectedButton[1]->pressed();
    }
    else {
        selectedButton[1] = nullptr;
    }
}

bool ManualForm::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::Hide) {
//        QString objectName = watched->objectName();
//        qDebug() << objectName << "was hidden";

        if (watched == ui->tabGuide) {
            onCheckSavedGuide();
            return true;
        }
        else if (watched == ui->tabReference) {
            onCheckSavedReferPointPara();
            return true;
        }
        else if (watched == ui->tabWidgetManualPage) {
//            qDebug() << "QTabWidget was hidden";
            int index = ui->tabWidgetManualPage->currentIndex();
            if (index == 0)
            {
                onCheckSavedGuide();
                return true;
            }
            else if (index == 4)
            {
                onCheckSavedReferPointPara();
                return true;
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}

void ManualForm::on_btnEditReference_clicked()
{
    if (selectedButton[1])
    {
        auto it = std::find_if(referencePoints.begin(), referencePoints.end(), [=](const ReferPointPara& point) {
            return point.button == selectedButton[1];
        });

        if (it != referencePoints.end())
        {

            FullKeyboard *keyboard = FullKeyboard::instance();
            keyboard->setText(it->name);
            keyboard->setCurrentEditObj(ui->textReferPointName);
            keyboard->exec(); // must use exec here, using open cannot ensure to save the name.

            QString text = keyboard->getInputText();
            if (it->name != text )
            {
                it->name = text;
                ui->btnSaveReference->setParaChangedFlag(true);
            }
        }
    }
}

void ManualForm::initVar()
{
    draggable[0] = false;
    draggable[1] = false;
    selectedButton[0] = nullptr;
    selectedButton[1] = nullptr;

    reserveButtons = {
        ui->btnPortY1, ui->btnPortY2, ui->btnPortY3, ui->btnPortY4,
        ui->btnPortY5, ui->btnPortY6, ui->btnPortY7, ui->btnPortY8,
        ui->btnPortY9, ui->btnPortY10, ui->btnPortY11, ui->btnPortY12,
        ui->btnPortY13, ui->btnPortY14, ui->btnPortY15, ui->btnPortY16,
        ui->btnPortY17, ui->btnPortY18, ui->btnPortY19, ui->btnPortY20,
        ui->btnPortY21, ui->btnPortY22, ui->btnPortY23, ui->btnPortY24
    };
}

void ManualForm::on_btnEditGuideName_clicked()
{
    if (selectedButton[0] == nullptr) return;

    QString guideName = selectedButton[0]->text();

    FullKeyboard *keyboard = FullKeyboard::instance();
    keyboard->setText(guideName);
    keyboard->setCurrentEditObj(selectedButton[0]);
    keyboard->exec();

    QString currentText = selectedButton[0]->text();

    if (guideName != currentText)
    {
        DraggableButton *currentButton = selectedButton[0];

        GuidePara &guidePara = guidePoints[currentButton];
        guidePara.guideName = currentText;

        ui->btnSaveGuide->setParaChangedFlag(true);
    }
}

void ManualForm::onSaveKeyDef()
{
    // save the KeyDef setting in coresponding guidePoints
    if (selectedButton[0] && guidePoints.contains(selectedButton[0]))
    {
        guidePoints[selectedButton[0]].keyDefStr = ui->editGuideKeyDef->text();
        // save the corresponding parameters in struct GuidePara
        guidePoints[selectedButton[0]].keyType = ui->editGuideKeyDef->getKeyType();
        guidePoints[selectedButton[0]].portNum = ui->editGuideKeyDef->getPortNum();
        guidePoints[selectedButton[0]].status = ui->editGuideKeyDef->getKeyFuncStatus();

        ui->btnSaveGuide->setParaChangedFlag(true);
    }
}

void ManualForm::onCheckSavedGuide()
{
    if (ui->btnSaveGuide->isParaChanged() == false) {
        return;
    }
    int reply =  MainWindow::pMainWindow->showErrorTip(tr("操作指引参数有修改，是否需要保存？"));
    if (reply == QDialog::Accepted)
    {
        on_btnSaveGuide_clicked();
    }
    else if (reply == QDialog::Rejected)
    {
        updateGuidePoints();
    }
    ui->btnSaveGuide->setParaChangedFlag(false);;
}

void ManualForm::onCheckSavedReferPointPara()
{
    if (ui->btnSaveReference->isParaChanged() == false) {
        return;
    }
    int reply =  MainWindow::pMainWindow->showErrorTip(tr("参考点参数有修改，是否需要保存？"));
    if (reply == QDialog::Accepted)
    {
        on_btnSaveReference_clicked();
    }
    else if (reply == QDialog::Rejected)
    {
        updateReferPointsList();
    }
    ui->btnSaveReference->setParaChangedFlag(false);
}
void ManualForm::on_btnSaveGuide_clicked()
{
//    for (auto& [k, v] : guidePoints)
//    {

//    }

    P_GuideStruct defaultGuide = {0, "", "0", QPoint(0, 0), 0, 0, 0};

    // 清空 m_Guide 数组
    std::fill(std::begin(m_Guide), std::end(m_Guide), defaultGuide);

    int index = 0;
    for (const auto& guide : guidePoints) {
        if (index >= GUIDE_TOTAL_NUM) break;

        m_Guide[index].guideFlag = true;
        m_Guide[index].guideName = guide.guideName;
        m_Guide[index].keyDefStr = guide.keyDefStr;
        m_Guide[index].btnPos = guide.btnPos;
        m_Guide[index].keyType = guide.keyType;
        m_Guide[index].portNum = guide.portNum;
        m_Guide[index].status = guide.status;
        ++index;
    }
    ui->btnSaveGuide->setParaChangedFlag(false);

}
void ManualForm::on_btnSaveReference_clicked()
{
    if (selectedButton[1])
    {
        auto it = std::find_if(referencePoints.begin(), referencePoints.end(), [=](const ReferPointPara& point) {
            return point.button == selectedButton[1];
        });

        if (it != referencePoints.end())
        {
            it->name = ui->textReferPointName->toPlainText();
            QPoint pointPos = selectedButton[1]->geometry().topLeft();
            it->pointPos = pointPos;

            updateReferPointsTable();
        }
    }


    // 重置结构体信息
    P_RefStruct defaultRefPoint = {0, {0}, {0}, "", 0, 0};
    std::fill(std::begin(m_RefPoint), std::end(m_RefPoint), defaultRefPoint);

   // 更新结构体信息
    for (int i = 0; i < referencePoints.size(); i++)
    {
        int arrayIndex = referencePoints.at(i).index - 1;
        m_RefPoint[arrayIndex].refFlag = true;
        //            m_RefPoint[arrayIndex].index = referencePoints.at(i).index;
        m_RefPoint[arrayIndex].refName = referencePoints.at(i).name;
        m_RefPoint[arrayIndex].xPos = referencePoints.at(i).pointPos.x();
        m_RefPoint[arrayIndex].yPos = referencePoints.at(i).pointPos.y();
    }

    ui->btnSaveReference->setParaChangedFlag(false);
}

void ManualForm::onTabChanged(int index)
{

    static int lastTabIndex = 4;
    if (lastTabIndex == 0)
    {
        onCheckSavedGuide();
    }
    else if (lastTabIndex == 4)
    {
//        qDebug() << "check if referPoint has been saved";
        onCheckSavedReferPointPara();
    }
    lastTabIndex = index;
}

void ManualForm::on_btnImportPictureGuide_clicked()
{
    bool useDefultPic = false;
    QPixmap pic("./guide.png");
    if (pic.isNull())
    {
        int res = MainWindow::pMainWindow->showErrorTip(tr("未找到图片：guide.png，图片像素: 1001×450px。是否使用默认图片？"));
        useDefultPic = res == QDialog::Accepted;

        if (useDefultPic)
        {
            pic = QPixmap(":/images/guide.png");
        }
        else {
            pic = QPixmap("./guide.png");
        }
    }
    ui->labelGuideBackgroud->setPixmap(pic);
}

void ManualForm::on_btnImportPictureReference_clicked()
{
    bool useDefultPic = false;
    QPixmap pic("./referPoint.png");
    if (pic.isNull())
    {
        int res = MainWindow::pMainWindow->showErrorTip(
                    tr("未找到图片: referPoint.png，图片像素: 760×480px。是否使用默认图片？"));
        useDefultPic = res == QDialog::Accepted;

        if (useDefultPic)
        {
            REFERPOINT_PIC = ":/images/referPoint.png";
            pic = QPixmap(":/images/referPoint.png");
        }
        else {
            REFERPOINT_PIC = "./referPoint.png";
            pic = QPixmap("./referPoint.png");
        }
    }
    ui->labReferPointBackGround->setPixmap(pic);
}

void ManualForm::on_cb_axisActionAxis_currentIndexChanged(int index)
{
    m_manualAxis.axis=index;
}
//待状态灯的按钮设置状态灯状态
void ManualForm::setbuttonIcon(QPushButton *button,const QString &ButtonText, uint8_t state)
{
    switch (state) {
    case 1://红灯为夹紧、正转、通或开状态
        button->setIcon(QIcon(":/images/state_red.png"));
        break;
    case 0://熄灭为松开、反转、断或关状态
        button->setIcon(QIcon(":/images/state_grey.png"));
        break;
    default:
        break;
    }
    button->setStyleSheet("text-align: left; padding-left: 10px;");
    button->setText(ButtonText);
    button->setIconSize(QSize(24, 24));
}

//卡爪/机床,预留界面状态灯按钮初始化
void ManualForm::StateButtonInit()
{
    //卡爪/机床
    setbuttonIcon(ui->btnRawMaterial1Loosen,tr("原料1松开"),false);
    setbuttonIcon(ui->btnFinishedProduct1Loosen,tr("成品1松开"),false);
    setbuttonIcon(ui->btnClaw1Reverse,tr("卡爪1反转"),false);
    setbuttonIcon(ui->btnBlow1Loosen,tr("吹气1断"),false);
    setbuttonIcon(ui->btnChuck1Loosen,tr("卡盘1松开"),false);
    setbuttonIcon(ui->btnAutoGate1Close,tr("自动门1关"),false);
    setbuttonIcon(ui->btnStartProcess1Break,tr("启动加工1断"),false);
    setbuttonIcon(ui->btnMainAxisLocate1Break,tr("主轴定位1断"),false);
    setbuttonIcon(ui->btnControlRotate1Break,tr("控制旋转1断"),false);
    //预留
//    setbuttonIcon(ui->btnReserveY7Break,tr("Y7自动灯断"),false);
//    setbuttonIcon(ui->btnReserveY8Break,tr("Y8报警灯断"),false);
//    setbuttonIcon(ui->btnReserveY13Break,tr("Y13急停断"),false);
//    setbuttonIcon(ui->btnReserveY14Break,tr("Y14润滑出断"),false);
//    setbuttonIcon(ui->btnReserveY15Break,tr("Y15报警灯断"),false);
//    setbuttonIcon(ui->btnReserveY16Break,tr("Y16暂停灯断"),false);
//    setbuttonIcon(ui->btnReserveY19Break,tr("Y19加工安全1断"),false);
//    setbuttonIcon(ui->btnReserveY22Break,tr("Y22断"),false);
//    setbuttonIcon(ui->btnReserveY23Break,tr("Y23断"),false);
//    setbuttonIcon(ui->btnReserveY24Break,tr("Y24断"),false);
//    setbuttonIcon(ui->btnReserveY1Break,tr("Y1原料1夹紧断"),false);
//    setbuttonIcon(ui->btnReserveY3Break,tr("Y3成品1夹紧断"),false);
//    setbuttonIcon(ui->btnReserveY5Break,tr("Y5卡爪1正转断"),false);
//    setbuttonIcon(ui->btnReserveY9Break,tr("Y9自动门1开断"),false);
//    setbuttonIcon(ui->btnReserveY20Break,tr("Y20卡盘1夹紧断"),false);
}
