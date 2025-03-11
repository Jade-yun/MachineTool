#include "teach.h"
#include "ui_teach.h"
#include "./method/program_save.h"
#include "./method/cmd.h"
#include "./method/Orderjoint.h"
#include <QString>
#include <QDebug>
#include "stackedit.h"
#include <QButtonGroup>
#include "port_setting.h"
#include <QTableWidgetItem>
#include <QTableWidget>
#include <QObject>
#include <QScrollBar>
#include <QTimer>
#include <mainwindow.h>
#include <QRandomGenerator>
#include <QMouseEvent>
#include "file_preview.h"
Teach::Teach(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::Teach)
{
    ui->setupUi(this);

    /*保存按钮图标闪烁控制*/
    Teach_timer =new QTimer(this);
    Teach_timer->setInterval(500);
    connect(Teach_timer,&QTimer::timeout,this,[=](){
        static bool SaveIconShow = true;
        if(OrderNeedSaveFlag)
        {
            if(SaveIconShow){
                ui->btnSave->setIcon(QIcon(":/images/save.png"));
            }
            else{
                ui->btnSave->setIcon(QIcon());//设置为空图标
            }
            SaveIconShow = !SaveIconShow;
        }
        else
        {
            ui->btnSave->setIcon(QIcon(":/images/save.png"));
            Teach_timer->stop();
        }
    });

    /*堆叠编辑弹窗*/
    stack_Dialog = new BaseWindow();
    stack_Temp = new StackEdit(0, this);
    stack_Dialog->setStackEdit(stack_Temp);
    stack_Dialog->setModal(true);
    connect(this,&Teach::StackEditBaseInit_signal,stack_Dialog,&BaseWindow::StackEditBaseInit);
    connect(stack_Temp, &StackEdit::closeStackEditDialogSignal, stack_Dialog, &QDialog::close);// 连接StackEdit的关闭信号到QDialog的close槽
    connect(stack_Temp,&StackEdit::stackParRefreshSignal,this,[=](){Stack_Edit_Handle(STACK_X_AXIS);});//刷新堆叠指令参数
    connect(ui->coboxVarSelectVarPreOp,QOverload<int>::of(&QComboBox::currentIndexChanged),this,&Teach::EditOperatorVarPreOp_handle);

    ui->listWgtJumpto->setSelectionMode(QAbstractItemView::NoSelection);//禁用选择
    ui->listWidget_LabelEdit->setSelectionMode(QAbstractItemView::NoSelection);//禁用选择

    QTabBar* bar = ui->tabWidget_Teach->tabBar();
    connect(bar, &QTabBar::tabBarClicked, [&](int index) {
        if (index == 1)
        {
            ui->stackedWidget_Senior->setCurrentIndex(0);
        }
        else if (index == 2)
        {
            ui->stackedWidget_General->setCurrentIndex(0);
        }
        });
    connect(ui->chboxCondition1If,SIGNAL(clicked()),this,SLOT(chboxCondition1If2If_handle()));
    connect(ui->chboxCondition2If,SIGNAL(clicked()),this,SLOT(chboxCondition1If2If_handle()));
    connect(ui->Edit_Var_const,SIGNAL(clicked()),this,SLOT(LogicVar_handle()));
    connect(ui->Edit_Var_var,SIGNAL(clicked()),this,SLOT(LogicVar_handle()));
    connect(ui->Edit_Var_Axis,SIGNAL(clicked()),this,SLOT(LogicVar_handle()));
    connect(ui->Edit_Var_Prod,SIGNAL(clicked()),this,SLOT(LogicVar_handle()));

    connect(ui->Edit_Axis_const,SIGNAL(clicked()),this,SLOT(LogicAxis_handle()));
    connect(ui->Edit_Axis_var,SIGNAL(clicked()),this,SLOT(LogicAxis_handle()));
    connect(ui->Edit_Stack_const,SIGNAL(clicked()),this,SLOT(LogicStack_handle()));
    connect(ui->Edit_Stack_num,SIGNAL(clicked()),this,SLOT(LogicStack_handle()));
    connect(ui->Edit_Prod_const,SIGNAL(clicked()),this,SLOT(LogicProt_handle()));
    connect(ui->Edit_Prod_var,SIGNAL(clicked()),this,SLOT(LogicProt_handle()));

    ui->stackedWidget_Senior->setCurrentIndex(0);
    ui->stackedWidget_General->setCurrentIndex(0);
    /*输入编辑框精度设置*/
    for (NumberEdit* edit : ui->Speed_Educat_page->findChildren<NumberEdit*>())
    {
        edit->setDecimalPlaces(0); // 精度0
    }
    ui->lineEdit_General_Speed_X1->setDecimalPlaces(0);
    ui->lineEdit_General_Speed_Y1->setDecimalPlaces(0);
    ui->lineEdit_General_Speed_Z1->setDecimalPlaces(0);
    ui->lineEdit_General_Speed_Y2->setDecimalPlaces(0);
    ui->lineEdit_General_Speed_Z2->setDecimalPlaces(0);
    ui->lineEdit_General_Speed_C->setDecimalPlaces(0);
    ui->lineEdit_Reserve_time->setDecimalPlaces(2);
    ui->lineEdit_Reserve_num->setDecimalPlaces(0);
    ui->editReturnStepNum->setDecimalPlaces(0);
    ui->lineEdit_LowSpeedTorque_delay->setDecimalPlaces(0);
    ui->lineEdit_Shift_Speed->setDecimalPlaces(0);
    ui->editLoopNum->setDecimalPlaces(0);
    ui->lineEdit_Edit_AxisMove_pos->setDecimalPlaces(2);
    ui->lineEdit_Edit_AxisMove_speed->setDecimalPlaces(0);
    ui->lineEdit_Edit_AxisMove_delay->setDecimalPlaces(2);
    ui->lineEdit_Edit_AxisMove_early_pos->setDecimalPlaces(2);
    ui->lineEdit_Edit_AxisMove_early_speed->setDecimalPlaces(0);
    ui->lineEdit_Edit_ReserveOut_Num->setDecimalPlaces(0);
    ui->lineEdit_Edit_ReserveOut_time->setDecimalPlaces(2);
    ui->lineEdit_edit_fault_num->setDecimalPlaces(0);
    ui->lineEdit_edit_tips_num->setDecimalPlaces(0);
    ui->lineEdit_Edit_torque_value->setDecimalPlaces(0);
    ui->lineEdit_Edit_offsetpos->setDecimalPlaces(2);
    ui->lineEdit_edit_offsetspeed->setDecimalPlaces(0);
    ui->lineEdit_Edit_offsetDelay->setDecimalPlaces(2);
    ui->lineEdit_Edit_offset_advenddis->setDecimalPlaces(2);
    ui->lineEdit_Edit_offset_advCspeed->setDecimalPlaces(0);
    ui->editOther->setDecimalPlaces(0);
    ui->editAxisPos->setDecimalPlaces(2);
    ui->lineEdit_Loop_num->setDecimalPlaces(0);
    ui->lineEdit_Edit_Wait_step->setDecimalPlaces(0);
    ui->lineEdit_Edit_Wait_step_2->setDecimalPlaces(0);
    ui->lineEdit_Search_runspeed->setDecimalPlaces(0);
    ui->lineEdit_Search_Adv_speed->setDecimalPlaces(0);
    ui->editRealProcIf->setDecimalPlaces(0);
    ui->editTimernum->setDecimalPlaces(0);
    ui->Stack_Edit_Speed->setDecimalPlaces(0);
    ui->Stack_Edit_AdvSpeed->setDecimalPlaces(0);
    ui->Stack_Edit_Speed_2->setDecimalPlaces(0);
    ui->Stack_Edit_AdvSpeed_2->setDecimalPlaces(0);
    ui->Stack_Edit_Speed_3->setDecimalPlaces(0);
    ui->Stack_Edit_AdvSpeed_3->setDecimalPlaces(0);

    generalBtn[0] = ui->btn_General_1;
    generalBtn[1] = ui->btn_General_2;
    generalBtn[2] = ui->btn_General_3;
    generalBtn[3] = ui->btn_General_4;

    generalStr[0] = QStringLiteral("轴");
    generalStr[1] = QStringLiteral("卡爪");
    generalStr[2] = QStringLiteral("检测");
    generalStr[3] = QStringLiteral("机床");

    generalIconPath[0] = ":/images/teach_img/axis.png";
    generalIconPath[1] = ":/images/teach_img/claw.png";
    generalIconPath[2] = ":/images/teach_img/signalCheck.png";
    generalIconPath[3] = ":/images/teach_img/machine.png";

    SubroutineBtn[0] = ui->Subroutine1_button;
    SubroutineBtn[1] = ui->Subroutine2_button;
    SubroutineBtn[2] = ui->Subroutine3_button;
    SubroutineBtn[3] = ui->Subroutine4_button;
    SubroutineBtn[4] = ui->Subroutine5_button;
    SubroutineBtn[5] = ui->Subroutine6_button;
    SubroutineBtn[6] = ui->Subroutine7_button;
    SubroutineBtn[7] = ui->Subroutine8_button;

    for(auto i=0;i<8;i++)
    {
        connect(SubroutineBtn[i],&QPushButton::clicked,this,[=](){
            if(OrderNeedSaveFlag == true)
            {
                int reply =  MainWindow::pMainWindow->showErrorTip(tr("教导参数有修改，是否需要保存？"));
                if (reply == QDialog::Accepted)
                {
                    widgetSwitchOrderSaveHandel(true);
                }
                else
                {
                    widgetSwitchOrderSaveHandel(false);
                }
            }
            Switch_Pro_ReadOrder(i+1);
            g_Usart->ExtendSendProDeal(CMD_MAIN_PRO,CMD_SUN_PRO_INFO);
            g_Usart->ExtendSendProDeal(CMD_MAIN_PRO,CMD_SUN_PRO_SAVE,m_OperateProNum,2);
        });
    }

    ui->tableWgtTeach->setSelectionBehavior(QAbstractItemView::SelectRows); //设置选择行为时每次选择一行
    ui->tableWgtTeach->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置不可编辑
    ui->tableWgtTeach->setSelectionMode(QAbstractItemView::SingleSelection);//设置为单选
    ui->tableWgtTeach->setShowGrid(true); //是否显示网格
    ui->tableWgtTeach->setAlternatingRowColors(true);//开启隔行自动变色
    ui->tableWgtTeach->setPalette(QPalette(QColor(237, 212, 0)));//隔行颜色
    ui->tableWgtTeach->setColumnWidth(0,80);
    ui->tableWgtTeach->setColumnWidth(1,10);

    connect(ui->tabWidget_Teach, &QTabWidget::tabBarClicked, this, [=](int index){
        if (index == 0)
        {
            ui->stkWgtProgram->setCurrentWidget(ui->pageSubProgram);
        }
        else if (index == 1)
        {
            ui->stackedWidget_Senior->setCurrentIndex(0);
        }
        else if (index == 3)
        {
            ui->stackedWidget_General->setCurrentIndex(0);
        }
    });


    // 逻辑&变量 代表每一条指令的checkbox 插入时遍历以下checkbox
    QButtonGroup* chboxsLogicVar = new QButtonGroup(this);
    chboxsLogicVar->addButton(ui->chboxIfCondition1, 0);
    chboxsLogicVar->addButton(ui->chboxElseCondition1, 1);
    chboxsLogicVar->addButton(ui->chboxEndIf, 2);
    chboxsLogicVar->addButton(ui->chboxLoopStart, 3);
    chboxsLogicVar->addButton(ui->chboxLoopEnd, 4);
    chboxsLogicVar->addButton(ui->chboxVarSelectVarPreOp, 5);
    chboxsLogicVar->addButton(ui->chboxAxisSelectVarPreOp, 6);
    chboxsLogicVar->addButton(ui->chboxStackSelectVarPreOp, 7);
    chboxsLogicVar->addButton(ui->chboxRealProdNumPreOp, 8);
    chboxsLogicVar->addButton(ui->chboxTimerStart, 8);
    chboxsLogicVar->addButton(ui->chboxTimerStop, 9);
    chboxsLogicVar->addButton(ui->chboxTimerClear, 10);
    chboxsLogicVar->setExclusive(true);

    // 初始未选中变量任何chbox时，隐藏堆叠计数
    ui->chboxConstantVarOp->show();
    ui->chboxVariableVarOp->show();
    ui->chboxAxisVarOp->show();
    ui->chboxProdNumVarOp->show();
    ui->chboxStackCntVarOp->hide();

    ui->chboxConstantVarOp->setChecked(true);
    EditOperatorVarPreOp_handle();
    if(ui->coboxVarSelectVarPreOp->currentIndex()<=VAR_TOTAL_NUM)
    {
        ui->editConstantVarPostOp->setDecimalPlaces(m_VariableType[ui->coboxVarSelectVarPreOp->currentIndex()]);
    }
    else
    {
        ui->coboxVarSelectVarPreOp->setCurrentIndex(0);
        ui->editConstantVarPostOp->setDecimalPlaces(m_VariableType[ui->coboxVarSelectVarPreOp->currentIndex()]);
    }
    pageInit();
    connect(ui->btnSave,&QPushButton::clicked,this,&Teach::OrderSaveHandel);//保存按钮
}

Teach::~Teach()
{
    delete ui;
}
/*************************************************************************
**  函数名：  EditOperatorVarPreOp_handle()
**	输入参数：
**	输出参数：
**	函数功能：变量小数类型处理显示
**  作者：    wukui
**  开发日期：2024/10/9
**************************************************************************/
void Teach::EditOperatorVarPreOp_handle()
{
    if(ui->coboxVarSelectVarPreOp->currentIndex()<=VAR_TOTAL_NUM)
    {
        uint8_t VarType = m_VariableType[ui->coboxVarSelectVarPreOp->currentIndex()];
        if(VarType == 0)
        {//整数
            ui->EditOperatorVarPreOp->setText(tr("整数"));
        }
        else if(VarType == 1)
        {
            ui->EditOperatorVarPreOp->setText(tr("一位小数"));
        }
        else if(VarType == 2)
        {
            ui->EditOperatorVarPreOp->setText(tr("两位小数"));
        }

        if(ui->chboxVarSelectVarPreOp->isChecked())
        {//修改小数位数
            ui->editConstantVarPostOp->setDecimalPlaces(VarType);
        }
        for(int i=0;i<VAR_TOTAL_NUM;i++)
        {
            if(m_VariableTypeLod[i] != m_VariableType[i])
            {
                if(OrderNeedSaveFlag == false)
                {
                    Teach_timer->start();
                    OrderNeedSaveFlag = true;
                    SufferOperNeedRefreash = true;
                }
            }
        }
    }
}
/*************************************************************************
**  函数名：  Pro_AxisActOrderInit()
**	输入参数：
**	输出参数：
**	函数功能：轴动作指令参数初始化
**  作者：    wukui
**  开发日期：2024/8/6
**************************************************************************/
void Teach::Pro_AxisActOrderInit(void)
{
    uint8_t i=0;
    for(i=0;i<AXIS_TOTAL_NUM;i++)
    {
        memset(&Temp_AxisMoveOrder[i],0,sizeof(P_AxisMoveStruct));
        Temp_AxisMoveOrder[i].axis = 6;  //6-无效
        Temp_AxisMoveOrder[i].pos = 0;
        Temp_AxisMoveOrder[i].speed = 50;
        Temp_AxisMoveOrder[i].advEndDis = 0;
        Temp_AxisMoveOrder[i].advCSpeedDis = 0;
        Temp_AxisMoveOrder[i].advEndFlag = 0;
        Temp_AxisMoveOrder[i].advCSpeedFlag = 0;
        Temp_AxisMoveOrder[i].advCSpeedSpeed = 0;
        Temp_AxisMoveOrder[i].referPointNum = 0;
    }

    ui->cb_General_X1->setChecked(false);
    ui->cb_General_Y1->setChecked(false);
    ui->cb_General_Z1->setChecked(false);
    ui->cb_General_Y2->setChecked(false);
    ui->cb_General_Z2->setChecked(false);
    ui->cb_General_C->setChecked(false);

    ui->lineEdit_General_Position_X1->setText("0.00");
    ui->lineEdit_General_Speed_X1->setText("50");

    ui->lineEdit_General_Position_Y1->setText("0.00");
    ui->lineEdit_General_Speed_Y1->setText("50");

    ui->lineEdit_General_Position_Z1->setText("0.00");
    ui->lineEdit_General_Speed_Z1->setText("50");

    ui->lineEdit_General_Position_Y2->setText("0.00");
    ui->lineEdit_General_Speed_Y2->setText("50");

    ui->lineEdit_General_Position_Z2->setText("0.00");
    ui->lineEdit_General_Speed_Z2->setText("50");

    ui->lineEdit_General_Position_C->setText("0.00");
    ui->lineEdit_General_Speed_C->setText("50");

}
//机器参数中轴类型改变后，教导界面相关控件处理
void Teach::AxisTypeChangeTeachHandle()
{
    //轴运动指令界面和速度教导指令界面
    if(m_AxisPar[X1_AXIS].axisType == 1)
    {
        ui->cb_General_X1->show();
        ui->lineEdit_General_Position_X1->show();
        ui->lineEdit_General_Speed_X1->show();
        ui->chboxSpeedTeachX1->show();
        ui->editSpeedTeachX1->show();
    }
    else
    {
        ui->cb_General_X1->hide();
        ui->lineEdit_General_Position_X1->hide();
        ui->lineEdit_General_Speed_X1->hide();
        ui->chboxSpeedTeachX1->hide();
        ui->editSpeedTeachX1->hide();
    }
    if(m_AxisPar[Y1_AXIS].axisType == 1)
    {
        ui->cb_General_Y1->show();
        ui->lineEdit_General_Position_Y1->show();
        ui->lineEdit_General_Speed_Y1->show();
        ui->chboxSpeedTeachY1->show();
        ui->editSpeedTeachY1->show();
    }
    else
    {
        ui->cb_General_Y1->hide();
        ui->lineEdit_General_Position_Y1->hide();
        ui->lineEdit_General_Speed_Y1->hide();
        ui->chboxSpeedTeachY1->hide();
        ui->editSpeedTeachY1->hide();
    }
    if(m_AxisPar[Z1_AXIS].axisType == 1)
    {
        ui->cb_General_Z1->show();
        ui->lineEdit_General_Position_Z1->show();
        ui->lineEdit_General_Speed_Z1->show();
        ui->chboxSpeedTeachZ1->show();
        ui->editSpeedTeachZ1->show();
    }
    else
    {
        ui->cb_General_Z1->hide();
        ui->lineEdit_General_Position_Z1->hide();
        ui->lineEdit_General_Speed_Z1->hide();
        ui->chboxSpeedTeachZ1->hide();
        ui->editSpeedTeachZ1->hide();
    }
    if(m_AxisPar[C_AXIS].axisType == 1)
    {
        ui->cb_General_C->show();
        ui->lineEdit_General_Position_C->show();
        ui->lineEdit_General_Speed_C->show();
        ui->chboxSpeedTeachC->show();
        ui->editSpeedTeachC->show();
    }
    else
    {
        ui->cb_General_C->hide();
        ui->lineEdit_General_Position_C->hide();
        ui->lineEdit_General_Speed_C->hide();
        ui->chboxSpeedTeachC->hide();
        ui->editSpeedTeachC->hide();
    }
    if(m_AxisPar[Y2_AXIS].axisType == 1)
    {
        ui->cb_General_Y2->show();
        ui->lineEdit_General_Position_Y2->show();
        ui->lineEdit_General_Speed_Y2->show();
        ui->chboxSpeedTeachY2->show();
        ui->editSpeedTeachY2->show();
    }
    else
    {
        ui->cb_General_Y2->hide();
        ui->lineEdit_General_Position_Y2->hide();
        ui->lineEdit_General_Speed_Y2->hide();
        ui->chboxSpeedTeachY2->hide();
        ui->editSpeedTeachY2->hide();
    }
    if(m_AxisPar[Z2_AXIS].axisType == 1)
    {
        ui->cb_General_Z2->show();
        ui->lineEdit_General_Position_Z2->show();
        ui->lineEdit_General_Speed_Z2->show();
        ui->chboxSpeedTeachZ2->show();
        ui->editSpeedTeachZ2->show();
    }
    else
    {
        ui->cb_General_Z2->hide();
        ui->lineEdit_General_Position_Z2->hide();
        ui->lineEdit_General_Speed_Z2->hide();
        ui->chboxSpeedTeachZ2->hide();
        ui->editSpeedTeachZ2->hide();
    }

    ui->coboxSearchAxisSelect->clear();
    ui->combo_Shift_AxisAll->clear();
    ui->combo_LowSpeedTorque_AxisAll->clear();
    ui->combo_ServoStop_AxisAll->clear();
    ui->coboxAxisSelectVarPreOp->clear();
    ui->coboxAxisSelectVarPostOp->clear();
    ui->comboBox_Edit_torque_axis->clear();
    ui->comboBox_Edit_Var_Axis->clear();
    ui->comboBox_Edit_Axis->clear();
    for(int i=0;i<AXIS_TOTAL_NUM;i++)
    {
        if(m_AxisPar[i].axisType == 1)
        {
            ui->coboxSearchAxisSelect->addItem(m_NameDefine[1].axisName[i]);//轴搜索指令界面-轴选择复选框
            ui->combo_Shift_AxisAll->addItem(m_NameDefine[1].axisName[i]);//轴偏移指令界面-轴选择复选框
            ui->combo_LowSpeedTorque_AxisAll->addItem(m_NameDefine[1].axisName[i]);//低速转矩指令界面-轴选择复选框
            ui->combo_ServoStop_AxisAll->addItem(m_NameDefine[1].axisName[i]);//伺服停止指令界面-轴选择复选框
            ui->coboxAxisSelectVarPreOp->addItem(m_NameDefine[1].axisName[i]);//变量指令界面-轴选择复选框
            ui->coboxAxisSelectVarPostOp->addItem(m_NameDefine[1].axisName[i]);//变量指令界面操作数-轴选择复选框
            ui->comboBox_Edit_torque_axis->addItem(m_NameDefine[1].axisName[i]);//低速转矩编辑-轴选择复选框
            ui->comboBox_Edit_Var_Axis->addItem(m_NameDefine[1].axisName[i]);//变量指令编辑界面-轴选择复选框
            ui->comboBox_Edit_Axis->addItem(m_NameDefine[1].axisName[i]);//变量指令编辑界面-轴选择复选框
        }
    }
}
//卡爪动作指令参数初始化
void Teach::Pro_ClawActionStructInit(void)
{
    ui->cb_finish_product_1->setChecked(false);
    ui->cb_Claw_roll_1->setChecked(false);
    ui->cb_Material_clamp_1->setChecked(false);
    WidgetNameRefresh();
}
//卡爪检测界面参数初始化
void Teach::Pro_ClawCheckStructInit(void)
{
    ui->cb_General_PassStart->setChecked(false);
    ui->cb_General_PassStop->setChecked(false);
    ui->cb_General_PineEnd->setChecked(false);
    ui->cb_General_PineStart->setChecked(false);
    ui->combo_General_PassStart->setCurrentIndex(0);
    ui->combo_General_PassStop->setCurrentIndex(0);
    ui->combo_General_PineStart->setCurrentIndex(0);
    ui->combo_General_PineEnd->setCurrentIndex(0);
    WidgetNameRefresh();
}
//预留检测界面参数初始化
void Teach::Pro_ReserveCheckStructInit(void)
{
    ui->lineEdit_ReserveOn->setText(" ");
    ui->lineEdit_ReserveOff->setText(" ");
    ui->lineEdit_ReserveEnd->setText(" ");
    ui->cb_General_ReserveOn->setChecked(false);
    ui->cb_General_ReserveOff->setChecked(false);
    ui->cb_General_ReserveEnd->setChecked(false);
}
//机床动作界面参数初始化
void Teach::Pro_MachineOutStructInit(void)
{
    ui->Start_Up_Mach_chbox->setChecked(false);
    ui->Main_Axis_Rotate_chbox->setChecked(false);
    ui->Main_Axis_locat_chbox->setChecked(false);
    ui->Auto_Door_chbox->setChecked(false);
    ui->Blow_Air_chbox->setChecked(false);
    ui->Chuck_chbox->setChecked(false);
    ui->lineEdit_Start_Up_Mach->setText("0.00");
    ui->lineEdit_Main_Axis_Rotate->setText("0.00");
    ui->lineEdit_Main_Axis_locat->setText("0.00");
    ui->lineEdit_Auto_Door->setText("0.00");
    ui->lineEdit_Blow_Air->setText("0.00");
    ui->lineEdit_Chuck->setText("0.00");
    ui->Start_Up_Mach_button->setState(true);
    ui->Main_Axis_Rotate_button->setState(true);
    ui->Auto_Door_button->setState(true);
    ui->Main_Axis_locat_button->setState(true);
    ui->Blow_Air_button->setState(true);
    ui->Chuck_button->setState(true);
    ui->Start_Up_Mach_button->setText(m_Port_Y[MACHINE_START_PROCESS_1].modifyName+tr("通"));//机床——启动加工1（输出）
    ui->Main_Axis_Rotate_button->setText(m_Port_Y[MACHINE_SPINDLE_ROTATE_1].modifyName+tr("通"));//机床——主轴旋转1（输出）
    ui->Auto_Door_button->setText(m_Port_Y[MACHINE_AUTO_DOOR_1_OPEN].modifyName);//自动门1开
    ui->Main_Axis_locat_button->setText(m_Port_Y[MACHINE_SPINDLE_FIXED_POS_1].modifyName+tr("通")); //机床——主轴定位1（输出）
    ui->Blow_Air_button->setText(m_Port_Y[MACHINE_GASS_1_LOOSENED].modifyName+tr("通"));//机床——吹气1（输出）
    ui->Chuck_button->setText(m_Port_Y[MACHINE_CHUCK_1_CLAMP].modifyName);//卡盘1夹紧
    Temp_MachineOutStruct[0].type = 1;
    Temp_MachineOutStruct[1].type = 1;
    Temp_MachineOutStruct[2].type = 1;
    Temp_MachineOutStruct[3].type = 1;
    Temp_MachineOutStruct[4].type = 1;
    Temp_MachineOutStruct[5].type = 1;
    Temp_MachineOutStruct[0].outportNum = m_Port_Y[MACHINE_START_PROCESS_1].portNum;
    Temp_MachineOutStruct[1].outportNum = m_Port_Y[MACHINE_SPINDLE_ROTATE_1].portNum;
    Temp_MachineOutStruct[2].outportNum = m_Port_Y[MACHINE_SPINDLE_FIXED_POS_1].portNum;
    Temp_MachineOutStruct[3].outportNum = m_Port_Y[MACHINE_AUTO_DOOR_1_OPEN].portNum;
    Temp_MachineOutStruct[4].outportNum = m_Port_Y[MACHINE_GASS_1_LOOSENED].portNum;
    Temp_MachineOutStruct[5].outportNum = m_Port_Y[MACHINE_CHUCK_1_CLAMP].portNum;
    WidgetNameRefresh();
}
//堆叠界面参数初始化
void Teach::Pro_StackResetZeroStructInit(void)
{
    ui->Stack_reset->setChecked(false);
    ui->Stack_group_1->setChecked(false);
    ui->Stack_group_2->setChecked(false);
    ui->Stack_group_3->setChecked(false);
    ui->Stack_group_4->setChecked(false);
    ui->Stack_group_5->setChecked(false);
    ui->Stack_group_6->setChecked(false);
    ui->Stack_group_7->setChecked(false);
    ui->Stack_group_8->setChecked(false);
    ui->Stack_group_9->setChecked(false);
    ui->Stack_group_10->setChecked(false);
    ui->Stack_group_11->setChecked(false);
    ui->Follow_Stack_group_1->setChecked(false);
    ui->Follow_Stack_group_2->setChecked(false);
    ui->Follow_Stack_group_3->setChecked(false);
    ui->Follow_Stack_group_4->setChecked(false);
    ui->Stack_reset_select_box->setCurrentIndex(false);
}

//预留输出界面参数初始化
void Teach::Pro_ReserveOutStructInit(void)
{
    ui->Reserve_Pass_Box->setChecked(true);
    ui->Reserve_OutTime_Box->setChecked(false);
    ui->Reserve_Overturn_Box->setChecked(false);
    ui->lineEdit_Reserve_Pass_Button->setText("");
    ui->lineEdit_Reserve_OutTime->setText("");
    ui->lineEdit_Reserve_Overturn->setText("");
    ui->lineEdit_Reserve_time->setText("0.00");
    ui->lineEdit_Reserve_num->setText("0");
    ui->Reserve_Pass_Button->setText(tr("预留通"));
    Temp_ReserveOutStruct.type=1;
    Temp_ReserveOutStruct.function = 0;
    Temp_ReserveOutStruct.interval = 0;
    Temp_ReserveOutStruct.outportNum = 0;
    memset(Temp_ReserveOutStruct.ret,0,sizeof(Temp_ReserveOutStruct.ret));
}
//预留输出界面参数初始化
void Teach::Pro_SpeedTeachInit(void)
{
    ui->editSpeedTeachX1->setText("50");
    ui->editSpeedTeachY1->setText("50");
    ui->editSpeedTeachZ1->setText("50");
    ui->editSpeedTeachY2->setText("50");
    ui->editSpeedTeachZ2->setText("50");
    ui->editSpeedTeachC->setText("50");
    for(uint8_t i=0;i<AXIS_TOTAL_NUM;i++)
    {
        Temp_AxisMoveOrder[i].speed = 50;
    }
}
//信号等待界面参数初始化
void Teach::Wait_Signal_Init(void)
{
    if(ui->tabWgtSigWait->currentIndex() == 0){//等待机床界面参数初始化
        ui->Wait_machine_signal_box->setChecked(false);
        ui->Wait_machine_signal_port->setCurrentIndex(0);
        memset(&Temp_WaitInMachineStruct,0,sizeof(Temp_WaitInMachineStruct));
    }
    else if(ui->tabWgtSigWait->currentIndex() == 1){//等待卡爪界面参数初始化
        ui->Wait_clamp_on_chbox->setChecked(false);
        ui->Wait_clamp_off_chbox->setChecked(false);
        ui->Wait_release_on_chbox->setChecked(false);
        ui->Wait_release_off_chbox->setChecked(false);
        ui->Wait_pos_cor_chbox->setChecked(false);
        ui->Wait_clamp_on_comboBox->setCurrentIndex(0);
        ui->Wait_clamp_off_comboBox->setCurrentIndex(0);
        ui->Wait_release_on_comboBox->setCurrentIndex(0);
        ui->Wait_release_off_comboBox->setCurrentIndex(0);
        ui->Wait_pos_cor_comboBox->setCurrentIndex(0);
        QStringList labelText;
        ui->coboxReturnLabe_box->clear();
        ui->coboxReturnLabe_box->addItem("无标签");
        for(int i=0;i<CurrentLableNameList.count();i++)
        {
            labelText = CurrentLableNameList[i].split(signalSpace);
            if(labelText.length()>1)
            {
                ui->coboxReturnLabe_box->addItem(labelText[0]);
            }
        }
        memset(&Temp_WaitInClawStruct,0,sizeof(Temp_WaitInClawStruct));
    }
    else if(ui->tabWgtSigWait->currentIndex() == 2)
    {
        ui->Wait_Reserve_on_chbox->setChecked(false);
        ui->Wait_Reserve_off_chbox->setChecked(false);
        ui->Wait_Reserve_up_chbox->setChecked(false);
        ui->Wait_Reserve_down_chbox->setChecked(false);
        ui->Wait_Reserve_Jump_chbox->setChecked(false);
        ui->lineEdit_Wait_ReserveOn->setText("");
        ui->lineEdit_Wait_ReserveOFF->setText("");
        ui->lineEdit_Wait_ReserveUp->setText("");
        ui->lineEdit_Wait_ReserveDown->setText("");
        ui->lineEdit_Wait_ReserveJump->setText("");
        QStringList labelText;
        ui->coboxReturnLabe_box->clear();
        ui->coboxReturnLabe_box->addItem("无标签");
        for(int i=0;i<CurrentLableNameList.count();i++)
        {
            labelText = CurrentLableNameList[i].split(signalSpace);
            if(labelText.length()>1)
            {
                ui->coboxReturnLabe_box->addItem(labelText[0]);
            }
        }
        memset(&Temp_WaitInReserveStruct,0,sizeof(Temp_WaitInReserveStruct));
    }
    ui->chboxReturnLabel->setChecked(true);
    ui->chboxReturnStepNum->setChecked(true);
    ui->editReturnStepNum->setText("6");
    ui->coboxReturnLabe_box->setCurrentIndex(0);
    Temp_WaitInMachineStruct.type = 0;//跳转类型 返回步号
    Temp_WaitInClawStruct.type = 0;//跳转类型 返回步号
    Temp_WaitInReserveStruct.type = 0;//跳转类型 返回步号
    Temp_WaitInMachineStruct.backListNum = 0;//步号
    Temp_WaitInClawStruct.backListNum = 0;//步号
    Temp_WaitInReserveStruct.backListNum = 0;//步号
    Temp_WaitInMachineStruct.label = 0;//标签号
    Temp_WaitInClawStruct.label = 0;//标签号
    Temp_WaitInReserveStruct.label = 0;//标签号
    WidgetNameRefresh();
}
//子程序指令界面参数初始化
void Teach::SunPro_Init(void)
{
    Temp_SunProStruct.oprMode = 0;
    Temp_SunProStruct.sunProNum = 1;
    ui->cb_Standard->setChecked(true);
    ui->cb_Automatic->setChecked(false);
    ui->cb_ParallelOnce->setChecked(false);
    ui->cb_WaitCompletion->setChecked(false);
    ui->combo_SubroutineAll->setCurrentIndex(0);
}
//搜索界面参数初始化
void Teach::Search_Init(void)
{
    Temp_SearchAxisMoveStruct.axis = 0;
    Temp_SearchAxisMoveStruct.offsetDis = 0;
    Temp_SearchAxisMoveStruct.maxPos = 0;
    Temp_SearchAxisMoveStruct.advCDis = 0;
    Temp_SearchAxisMoveStruct.posStoreFlag = 0;
    Temp_SearchAxisMoveStruct.reachPosAlarmFlag = 0;
    Temp_SearchAxisMoveStruct.runSpeed = 50;
    Temp_SearchAxisMoveStruct.advCSpeed = 50;
    memset(Temp_SearchAxisMoveStruct.inportNum,1,sizeof(Temp_SearchAxisMoveStruct.inportNum));
    memset(Temp_SearchAxisMoveStruct.inporttype,1,sizeof(Temp_SearchAxisMoveStruct.inporttype));
    ui->chboxSearchAxisSelect->setChecked(false);
    ui->chboxResponseStop->setChecked(false);
    ui->coboxSearchAxisSelect->setCurrentIndex(0);
    ui->coboxSearchNum->setCurrentIndex(0);
    ui->editSigStop->setText("");
}
/*************************************************************************
**  函数名：  Switch_Pro_ReadOrder()
**	输入参数：ProNum:程序编号
**	输出参数：
**	函数功能：切换子程序与主程序之间互相切换处理函数
**  作者：    wukui
**  开发日期：2024/9/2
**************************************************************************/
void Teach::Switch_Pro_ReadOrder(uint8_t ProNum)
{
    m_OperateProNum = ProNum;
    m_CurrentSelectProOrderList = 0;//当前操作行号
    m_OperateProOrderListNum = m_ProInfo.proNum[m_OperateProNum];
    g_TotalProCopy(m_OperateProOrder,m_ProOrder[m_OperateProNum]);//将读取的程序赋给当前操作程序
    readLableOrderName(m_CurrentProgramNameAndPath);//每次切换都获取一次标签指令名称
    Teach_File_List_Refresh();//刷新列表
}
/*************************************************************************
**  函数名：  Teach_File_List_Refresh()
**	输入参数：
**	输出参数：
**	函数功能：教导-刷新程序列表
**  作者：    wukui
**  开发日期：2024/8/2
**************************************************************************/
void Teach::Teach_File_List_Refresh(void)
{
    File_Preview_Label = false;
    uint16_t i=0;
    //先清空表格并重新设置行数和列数
    uint16_t Old_CurrentSelectList = m_CurrentSelectProOrderList;
    if(ui && ui->tableWgtTeach)
    {
        ui->tableWgtTeach->clearContents();
        ui->tableWgtTeach->setRowCount(0);
        ui->tableWgtTeach->setColumnCount(3);
        for(i=0;i<m_OperateProOrderListNum;i++)
        {
            QTableWidgetItem *Teach_File_List_NumItem = new QTableWidgetItem();
            QTableWidgetItem *Teach_File_List_OrderItem = new QTableWidgetItem();
            QTableWidgetItem *Teach_File_List_OrderColor = new QTableWidgetItem();
            Teach_File_List_NumItem->setText(JointRunOrderNum(m_OperateProOrder[i]));
            Teach_File_List_NumItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);//设置执行行号内容居中显示
            Teach_File_List_OrderItem->setText(JointStrDeal(m_OperateProOrder[i]));//拼接显示内容
            Teach_File_List_OrderItem->setData(Qt::TextAlignmentRole, Qt::AlignLeft);//内容左右对齐靠左显示
            Teach_File_List_OrderItem->setData(Qt::TextAlignmentRole, Qt::AlignVCenter);//内容上下对齐居中对齐显示
            Teach_File_List_OrderItem->setData(Qt::TextWordWrap,1);//设置内容自动换行显示
            ui->tableWgtTeach->insertRow(i);
            ui->tableWgtTeach->setItem(i,0,Teach_File_List_NumItem);   //显示命令执行序号
            ui->tableWgtTeach->setItem(i,2,Teach_File_List_OrderItem);                 //显示命令内容
            ui->tableWgtTeach->setItem(i,1,Teach_File_List_OrderColor);
            if(m_OperateProOrder[i].noteFlag == 1)
            {//屏蔽指令时行背景显示灰色
                ui->tableWgtTeach->item(i,0)->setBackground(QColor(192, 191, 188));
                ui->tableWgtTeach->item(i,1)->setBackground(QColor(192, 191, 188));
                ui->tableWgtTeach->item(i,2)->setBackground(QColor(192, 191, 188));
            }
        }
        m_CurrentSelectProOrderList = Old_CurrentSelectList;
        if(m_CurrentSelectProOrderList<=ui->tableWgtTeach->rowCount())
        {
            ui->tableWgtTeach->selectRow(m_CurrentSelectProOrderList);
        }
        else
        {
            qDebug()<<"m_CurrentSelectProOrderList > tableWgtTeach.count";
        }
    }
    else
    {
        qDebug() << "Error: ui or tableWgtTeach is nullptr!";
    }

}
//指令行第二列颜色处理
void Teach::OrderColorShow_Handle()
{

}
//标签/注释
void Teach::on_btn_Tags_clicked()
{
    ui->stackedWidget_Senior->setCurrentIndex(1);
    Refresh_listWgtJumpto();
    listWgtJumptoLabelIndex = 0;
}
//刷新listWgtJumpto列表
void Teach::Refresh_listWgtJumpto()
{
    if(ui->tabWidget_Teach->currentWidget() == ui->tabAdvance && ui->stackedWidget_Senior->currentWidget() == ui->label_page)
    {
        if(m_OperateProNum==0)
        {
            ui->chboxComment->setText(tr("标签")+QString::number(getLabelOrderIndex()));
        }
        else if(m_OperateProNum>0&&m_OperateProNum<PRO_NUM)
        {
            ui->chboxComment->setText("子"+QString::number(m_OperateProNum)+"标签"+QString::number(getLabelOrderIndex()));
        }
        ui->listWgtJumpto->clear();
        for(int i=0; i<CurrentLableNameList.count();i++)
        {
            QListWidgetItem *item = new QListWidgetItem();
            QCheckBox *checkbox = new QCheckBox();
            checkbox->setChecked(false);
            checkbox->setAutoExclusive(true);//check为互斥
            checkbox->setText(CurrentLableNameList[i]);
            checkbox->setFixedSize(ui->listWgtJumpto->width(),37);
            ui->listWgtJumpto->addItem(item);
            ui->listWgtJumpto->setItemWidget(item,checkbox);
            connect(checkbox, &QCheckBox::toggled, this, [this, checkbox](bool checked){
                onCheckBoxToggled(checkbox, checked);
                });
        }
    }

    if(ui->tabWidget_Teach->currentWidget() == ui->tabMainPro && ui->stkWgtProgram->currentWidget() == ui->pageLabelEdit)
    {
        ui->listWidget_LabelEdit->clear();
        for(int i=0; i<CurrentLableNameList.count();i++)
        {
            QListWidgetItem *item = new QListWidgetItem();
            QCheckBox *checkbox = new QCheckBox();
            checkbox->setChecked(false);
            checkbox->setAutoExclusive(true);//check为互斥
            checkbox->setText(CurrentLableNameList[i]);
            checkbox->setFixedSize(ui->listWidget_LabelEdit->width(),37);
            ui->listWidget_LabelEdit->addItem(item);
            ui->listWidget_LabelEdit->setItemWidget(item,checkbox);
            connect(checkbox, &QCheckBox::toggled, this, [this, checkbox](bool checked){
                onCheckBoxToggled(checkbox, checked);
                });
        }
        if(ui->btnModify->isChecked())
        {//如果编辑按钮按下
            if(m_OperateProOrderListNum == 0)
            {//如果未载入程序，直接返回
                return;
            }
            OrderEdit_Handle();
        }
    }

}
void Teach::on_btn_Logic_clicked()
{
    ui->stackedWidget_Senior->setCurrentIndex(2);
}

void Teach::on_btn_Search_clicked()
{
    ui->stackedWidget_Senior->setCurrentIndex(3);
    Search_Init();
}

void Teach::on_btn_Shift_clicked()
{
    ui->stackedWidget_Senior->setCurrentIndex(4);
    ui->combo_Shift_AxisAll->setCurrentIndex(0);
    ui->lineEdit_Shift_Position->setText("0.00");
    ui->lineEdit_Shift_Speed->setText("50");
    memset(&Temp_OffsetAxisStruct,0,sizeof(Temp_OffsetAxisStruct));
    Temp_OffsetAxisStruct.axis = 0;
    Temp_OffsetAxisStruct.offsetPos = 0;
    Temp_OffsetAxisStruct.speed = 50;
}

void Teach::on_btn_TorqueProtection_clicked()
{
    ui->stackedWidget_Senior->setCurrentIndex(5);
    ui->combo_LowSpeedTorque_AxisAll->setCurrentIndex(0);
    ui->lineEdit_LowSpeedTorque_delay->setText("1");
    ui->cb_LowSpeedTorque_AxisAll->setChecked(false);
    Temp_TorqueGardStruct.axis = 0;
    Temp_TorqueGardStruct.torqueValue = 1;
}

void Teach::on_btn_ServoStop_clicked()
{
    ui->stackedWidget_Senior->setCurrentIndex(6);
    ui->cb_ServoStop_AxisAll->setChecked(false);
    ui->combo_ServoStop_AxisAll->setCurrentIndex(0);
    ui->lineEdit_ServoStop_delay->setText("0.00");
    Temp_AxisStopStruct.axis = 0;
}

void Teach::on_btn_Subroutine_clicked()
{
    ui->stackedWidget_Senior->setCurrentIndex(7);
    SunPro_Init();
}
//轴动作按钮处理函数
void Teach::on_btn_General_AxisAction_clicked()
{
    ui->stackedWidget_General->setCurrentIndex(1);
    ui->stackedWidget_Four->setCurrentIndex(0);
    generalBtn[3]->setVisible(false);
    generalStrList.clear();
    generalIconPathList.clear();
    for (int i = 1; i < 4; i++)
    {
        generalStrList.append(generalStr[i]);
        generalIconPathList.append(generalIconPath[i]);
    }
    for (int i = 0; i < 3; i++)
    {
        generalBtn[i]->setText(generalStrList[i]);
        generalBtn[i]->setIcon(QIcon(generalIconPathList[i]));
    }
    Pro_AxisActOrderInit();//轴运动界面参数和控件初始化
}

void Teach::on_btn_General_ClawAction_clicked()
{
    ui->stackedWidget_General->setCurrentIndex(1);
    ui->stackedWidget_Four->setCurrentIndex(1);
    generalBtn[3]->setVisible(false);
    generalStrList.clear();
    generalIconPathList.clear();
    for (int i = 2; i < 4; i++)
    {
        generalStrList.append(generalStr[i]);
        generalIconPathList.append(generalIconPath[i]);
    }
    generalStrList.append(generalStr[0]);
    generalIconPathList.append(generalIconPath[0]);
    for (int i = 0; i < 3; i++)
    {
        generalBtn[i]->setText(generalStrList[i]);
        generalBtn[i]->setIcon(QIcon(generalIconPathList[i]));
    }
    Pro_ClawActionStructInit();//卡爪动作指令界面初始化
}

void Teach::on_btn_General_SignalDetection_clicked()
{
    ui->stackedWidget_General->setCurrentIndex(1);
    ui->stackedWidget_Four->setCurrentIndex(2);
    ui->tabWidget_Reserve->setCurrentIndex(0);
    generalBtn[3]->setVisible(false);
    generalStrList.clear();
    generalIconPathList.clear();
    generalStrList.append(generalStr[3]);
    generalIconPathList.append(generalIconPath[3]);
    for (int i = 0; i < 2; i++)
    {
        generalStrList.append(generalStr[i]);
        generalIconPathList.append(generalIconPath[i]);
    }
    for (int i = 0; i < 3; i++)
    {
        generalBtn[i]->setText(generalStrList[i]);
        generalBtn[i]->setIcon(QIcon(generalIconPathList[i]));
    }
    Pro_ClawCheckStructInit();
    Pro_ReserveCheckStructInit();
}

void Teach::on_btn_General_MachineTool_clicked()
{
    ui->stackedWidget_General->setCurrentIndex(1);
    ui->stackedWidget_Four->setCurrentIndex(3);
    generalBtn[3]->setVisible(false);
    generalStrList.clear();
    generalIconPathList.clear();
    for (int i = 0; i < 3; i++)
    {
        generalStrList.append(generalStr[i]);
        generalIconPathList.append(generalIconPath[i]);
    }
    for (int i = 0; i < 3; i++)
    {
        generalBtn[i]->setText(generalStrList[i]);
        generalBtn[i]->setIcon(QIcon(generalIconPathList[i]));
    }
    Pro_MachineOutStructInit();
}

void Teach::on_btn_General_Stack_clicked()
{
    ui->stackedWidget_General->setCurrentIndex(2);
    Pro_StackResetZeroStructInit();//堆叠界面参数初始化
}

void Teach::on_btn_General_ReserveOutput_clicked()
{
    ui->stackedWidget_General->setCurrentIndex(3);
    Pro_ReserveOutStructInit();
}

void Teach::on_btn_General_SpeedTeach_clicked()
{
    ui->stackedWidget_General->setCurrentIndex(4);
    Pro_SpeedTeachInit();
}

void Teach::on_btn_General_SignalWait_clicked()
{
    ui->stackedWidget_General->setCurrentIndex(5);
    Wait_Signal_Init();
}

void Teach::on_btn_General_Other_clicked()
{
    ui->stackedWidget_General->setCurrentIndex(1);
    ui->stackedWidget_Four->setCurrentIndex(4);
    generalBtn[3]->setVisible(true);
    for (int i = 0; i < 4; i++)
    {
        generalBtn[i]->setText(generalStr[i]);
        generalBtn[i]->setIcon(QIcon(generalIconPath[i]));
    }
    //界面参数初始化
    Temp_OtherAlarmCustStruct.alarmNum = 1450;
    Temp_OtherAlarmCustStruct.type = 4;
    Temp_OtherAlarmLampStruct.type = 1;
    Temp_OtherAlarmSoundStruct.type = 1;
    Temp_OtherCycStopCustStruct.cycleNum = 0;
    ui->btnAlarmLamp_button->setState(true);
    ui->btnAlarmSound_button->setState(true);
    ui->btnAlarmLamp_button->setText(m_Port_Y[OTHER_ALARM_LAMP].modifyName+tr("开"));
    ui->btnAlarmSound_button->setText(m_Port_Y[OTHER_ALARM_SOUND].modifyName+tr("开"));

    ui->AlarmLamp_chbox->setChecked(false);
    ui->AlarmSound_chbox->setChecked(false);

    ui->AlarmDelay_chbox->setChecked(false);
    ui->AlarmCust_chbox->setChecked(false);
    ui->CycStop_chbox->setChecked(false);
    WidgetNameRefresh();
}

void Teach::on_btn_General_1_clicked()
{
    setGeneralBtnEvent(ui->btn_General_1->text());
}

void Teach::on_btn_General_2_clicked()
{
    setGeneralBtnEvent(ui->btn_General_2->text());
}

void Teach::on_btn_General_3_clicked()
{
    setGeneralBtnEvent(ui->btn_General_3->text());
}

void Teach::on_btn_General_4_clicked()
{
    setGeneralBtnEvent(ui->btn_General_4->text());
}

void Teach::on_btn_General_PageTurn_clicked()
{
    int index = ui->stackedWidget_Axis->currentIndex();
    if (index == 0)
    {
        ui->stackedWidget_Axis->setCurrentIndex(1);
        ui->btn_General_PageTurn->setIcon(QIcon(":/images/switch_left.png"));
    }
    else
    {
        ui->stackedWidget_Axis->setCurrentIndex(0);
        ui->btn_General_PageTurn->setIcon(QIcon(":/images/switch_right.png"));
    }
}

void Teach::setGeneralBtnEvent(QString btnName)
{
    int index = 0;
    for (int i = 0; i < 4; i++)
    {
        if (btnName == generalStr[i])
        {
            index = i;
            break;
        }
    }
    switch (index)
    {
    case 0:
        on_btn_General_AxisAction_clicked();
        break;
    case 1:
        on_btn_General_ClawAction_clicked();
        break;
    case 2:
        on_btn_General_SignalDetection_clicked();
        break;
    case 3:
        on_btn_General_MachineTool_clicked();
        break;
    }
}

void Teach::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
    }
    QWidget::changeEvent(e);
}

//教导界面-删除按钮处理函数
void Teach::on_btnDelete_clicked()
{
    if(m_OperateProOrderListNum == 0)
    {//如果未载入程序，直接返回
        return;
    }
    m_CurrentSelectProOrderList = ui->tableWgtTeach->currentIndex().row();//获取当前选中行号，从0开始
    if(m_CurrentSelectProOrderList<m_OperateProOrderListNum)
    {
        int reply =  MainWindow::pMainWindow->showErrorTip(tr("确认删除所选动作？"));
        if (reply == QDialog::Accepted)
        {
            if(m_OperateProOrder[m_CurrentSelectProOrderList].cmd == C_LABEL)
            {
                P_LabelStruct* LabelStruct = (P_LabelStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
                if(LabelStruct->labelNum>0)
                {
                    uint16_t temp_labelNum = LabelStruct->labelNum;
                    if(g_ProOrderOperate(m_OperateProNum,m_CurrentSelectProOrderList,1,0) == 0)
                    {
                        temp_labelNum = ReturnLableListIndex(temp_labelNum);
                        if(temp_labelNum>0)
                        {
                            CurrentLableNameList.removeAt(temp_labelNum);
                        }
                        else
                        {
                            MainWindow::pMainWindow->showErrorTip(tr("删除失败"),TipMode::ONLY_OK);
                            return;
                        }
                    }
                    else
                    {
                        MainWindow::pMainWindow->showErrorTip(tr("删除失败"),TipMode::ONLY_OK);
                        return;
                    }
                }
            }
            else
            {
                if(g_ProOrderOperate(m_OperateProNum,m_CurrentSelectProOrderList,1,0) != 0)
                {
                    MainWindow::pMainWindow->showErrorTip(tr("删除失败"),TipMode::ONLY_OK);
                }
            }
            g_Usart->ExtendSendProDeal(CMD_MAIN_PRO,CMD_SUN_PRO_DELET,m_OperateProNum,1,0);
            Teach_File_List_Refresh();//刷新程序列表
            OrderNeedSaveFlag = true;
            Teach_timer->start();
        }
    }
}
//教导界面-分解按钮处理函数
void Teach::on_btnDevide_clicked()
{
    if(m_OperateProOrderListNum == 0)
    {//如果未载入程序，直接返回
        return;
    }
    m_CurrentSelectProOrderList = ui->tableWgtTeach->currentIndex().row();//获取当前选中行号，从0开始
    if(m_CurrentSelectProOrderList<m_OperateProOrderListNum)
    {
        g_ProOrderOperate(m_OperateProNum,m_CurrentSelectProOrderList+1,3,0);//分解处理函数
        g_Usart->ExtendSendProDeal(CMD_MAIN_PRO,CMD_SUN_PRO_DELET,m_OperateProNum,3,0);
        Teach_File_List_Refresh();//刷新程序列表
        OrderNeedSaveFlag = true;
        Teach_timer->start();
    }
}
//教导界面-组合按钮处理函数
void Teach::on_btnCombine_clicked()
{
    if(m_OperateProOrderListNum == 0)
    {//如果未载入程序，直接返回
        return;
    }
    m_CurrentSelectProOrderList = ui->tableWgtTeach->currentIndex().row();//获取当前选中行号，从0开始
    if(m_CurrentSelectProOrderList<m_OperateProOrderListNum)
    {
        g_ProOrderOperate(m_OperateProNum,m_CurrentSelectProOrderList+1,2,1);//date:0-向下组合 1-向上组合
        g_Usart->ExtendSendProDeal(CMD_MAIN_PRO,CMD_SUN_PRO_DELET,m_OperateProNum,2,1);
        Teach_File_List_Refresh();//刷新程序列表
        OrderNeedSaveFlag = true;
        Teach_timer->start();
    }
}
//教导界面-上移按钮处理函数
void Teach::on_btnMoveUp_clicked()
{
    if(m_OperateProOrderListNum == 0)
    {//如果未载入程序，直接返回
        return;
    }
    m_CurrentSelectProOrderList = ui->tableWgtTeach->currentIndex().row();//获取当前选中行号，从0开始
    if(m_CurrentSelectProOrderList<m_OperateProOrderListNum)
    {
        g_ProOrderOperate(m_OperateProNum,m_CurrentSelectProOrderList+1,4,1);//每次上移一行
        g_Usart->ExtendSendProDeal(CMD_MAIN_PRO,CMD_SUN_PRO_DELET,m_OperateProNum,4,1);
        Teach_File_List_Refresh();//刷新程序列表
        OrderNeedSaveFlag = true;
        Teach_timer->start();
    }
}
//教导界面-下移按钮处理函数
void Teach::on_btnMoveDown_clicked()
{
    if(m_OperateProOrderListNum == 0)
    {//如果未载入程序，直接返回
        return;
    }
    m_CurrentSelectProOrderList = ui->tableWgtTeach->currentIndex().row();//获取当前选中行号，从0开始
    if(m_CurrentSelectProOrderList<m_OperateProOrderListNum)
    {
        g_ProOrderOperate(m_OperateProNum,m_CurrentSelectProOrderList+1,5,1);//每次下移一行
        g_Usart->ExtendSendProDeal(CMD_MAIN_PRO,CMD_SUN_PRO_DELET,m_OperateProNum,5,1);
        Teach_File_List_Refresh();//刷新程序列表
        OrderNeedSaveFlag = true;
        Teach_timer->start();
    }
}
//教导界面-屏蔽按钮处理函数
void Teach::on_btnShield_clicked()
{
    if(m_OperateProOrderListNum == 0)
    {//如果未载入程序，直接返回
        return;
    }
    m_CurrentSelectProOrderList = ui->tableWgtTeach->currentIndex().row();//获取当前选中行号，从0开始
    if(m_CurrentSelectProOrderList<m_OperateProOrderListNum)
    {
        g_ProOrderOperate(m_OperateProNum,m_CurrentSelectProOrderList+1,6,!m_OperateProOrder[m_CurrentSelectProOrderList].noteFlag);
        g_Usart->ExtendSendProDeal(CMD_MAIN_PRO,CMD_SUN_PRO_DELET,m_OperateProNum,6,m_OperateProOrder[m_CurrentSelectProOrderList].noteFlag);
        Teach_File_List_Refresh();//刷新程序列表
        OrderNeedSaveFlag = true;
        Teach_timer->start();
    }
}
//教导界面-插入按钮处理函数
void Teach::on_btnInset_clicked()
{
    if(m_OperateProOrderListNum == 0)
    {//如果未载入程序，直接返回
        return;
    }
    m_CurrentSelectProOrderList=ui->tableWgtTeach->currentIndex().row();
    if(m_CurrentSelectProOrderList<m_OperateProOrderListNum)
    {
        uint8_t data_Temp[64] = {0};
        memset(data_Temp,0,sizeof(data_Temp));
        if(ui->tabWidget_Teach->currentWidget() == ui->tabUniversal){//通用指令界面
            if(ui->stackedWidget_General->currentWidget() == ui->Axis_Claw_Test_Mach_page)
            {
                if(ui->stackedWidget_Four->currentWidget() == ui->Axis_Move_main_page)
                {//如果当前界面在轴动作指令界面
                    for(uint8_t i=AXIS_TOTAL_NUM; i>0; i--)
                    {
                        if(Temp_AxisMoveOrder[i-1].axis < AXIS_TOTAL_NUM)
                        {//选中轴
                            data_Temp[0] = m_OperateProNum;  //程序编号
                            data_Temp[1] = m_OperateProOrder[m_CurrentSelectProOrderList].list;
                            data_Temp[2] = m_OperateProOrder[m_CurrentSelectProOrderList].list>>8;
                            data_Temp[3] = m_OperateProOrder[m_CurrentSelectProOrderList].runOrderNum;
                            data_Temp[4] = m_OperateProOrder[m_CurrentSelectProOrderList].runOrderNum>>8;
                            data_Temp[5] = C_AXIS_MOVE;
                            data_Temp[6] = 0;
                            data_Temp[9] = Temp_AxisMoveOrder[i-1].pos;
                            data_Temp[10] = Temp_AxisMoveOrder[i-1].pos>>8;
                            data_Temp[11] = Temp_AxisMoveOrder[i-1].pos>>16;
                            data_Temp[12] = Temp_AxisMoveOrder[i-1].pos>>24;
                            data_Temp[13] = Temp_AxisMoveOrder[i-1].advEndDis;
                            data_Temp[14] = Temp_AxisMoveOrder[i-1].advEndDis>>8;
                            data_Temp[15] = Temp_AxisMoveOrder[i-1].advEndDis>>16;
                            data_Temp[16] = Temp_AxisMoveOrder[i-1].advEndDis>>24;
                            data_Temp[17] = Temp_AxisMoveOrder[i-1].advCSpeedDis;
                            data_Temp[18] = Temp_AxisMoveOrder[i-1].advCSpeedDis>>8;
                            data_Temp[19] = Temp_AxisMoveOrder[i-1].advCSpeedDis>>16;
                            data_Temp[20] = Temp_AxisMoveOrder[i-1].advCSpeedDis>>24;
                            data_Temp[21] = Temp_AxisMoveOrder[i-1].axis;
                            data_Temp[22] = Temp_AxisMoveOrder[i-1].speed;
                            data_Temp[23] = Temp_AxisMoveOrder[i-1].advEndFlag;
                            data_Temp[24] = Temp_AxisMoveOrder[i-1].advCSpeedFlag;
                            data_Temp[25] = Temp_AxisMoveOrder[i-1].advCSpeedSpeed;
                            data_Temp[26] = Temp_AxisMoveOrder[i-1].referPointNum;
                            g_InsertProOrder(data_Temp);
                            Teach_File_List_Refresh();//刷新程序列表
                        }
                    }
                }
                else if(ui->stackedWidget_Four->currentWidget() == ui->Claw_page){//卡爪界面
                    data_Temp[0] = m_OperateProNum;  //程序编号
                    data_Temp[1] = m_OperateProOrder[m_CurrentSelectProOrderList].list;
                    data_Temp[2] = m_OperateProOrder[m_CurrentSelectProOrderList].list>>8;
                    data_Temp[3] = m_OperateProOrder[m_CurrentSelectProOrderList].runOrderNum;
                    data_Temp[4] = m_OperateProOrder[m_CurrentSelectProOrderList].runOrderNum>>8;
                    data_Temp[5] = C_CLAW_ACTION;
                    data_Temp[6] = 0;
                    if(ui->cb_finish_product_1->isChecked())
                    {//成品1夹紧
                        data_Temp[9] = m_Port_Y[CLAW_PRODUCT_1_CLAMP].portNum;//输出端口号
                        data_Temp[10] = ui->btn_finish_product_1->getState();//控制类型
                        g_InsertProOrder(data_Temp);
                    }
                    else if(ui->cb_Claw_roll_1->isChecked())
                    {//卡爪1正转
                        data_Temp[9] = m_Port_Y[CLAW_CLAW_1_CLAMP].portNum;//输出端口号
                        data_Temp[10] = ui->btn_Claw_roll_1->getState();//控制类型
                        g_InsertProOrder(data_Temp);
                    }
                    else if(ui->cb_Material_clamp_1->isChecked())
                    {//原料1夹紧
                        data_Temp[9] = m_Port_Y[CLAW_METERIAL_1_CLAMP].portNum;//输出端口号
                        data_Temp[10] = ui->btn_Material_clamp_1->getState();//控制类型
                        g_InsertProOrder(data_Temp);
                    }
                    Teach_File_List_Refresh();//刷新程序列表
                }
                else if(ui->stackedWidget_Four->currentWidget() == ui->Signal_detect_page){//信号检测界面
                    data_Temp[0] = m_OperateProNum;  //程序编号
                    data_Temp[1] = m_OperateProOrder[m_CurrentSelectProOrderList].list;
                    data_Temp[2] = m_OperateProOrder[m_CurrentSelectProOrderList].list>>8;
                    data_Temp[3] = m_OperateProOrder[m_CurrentSelectProOrderList].runOrderNum;
                    data_Temp[4] = m_OperateProOrder[m_CurrentSelectProOrderList].runOrderNum>>8;
                    data_Temp[6] = 0;
                    if(ui->tabWidget_Reserve->currentWidget() == ui->tabDoubleValve)
                    {//卡爪检测界面
                        data_Temp[5] = C_CLAW_CHECK;
                        if(ui->cb_General_PassStart->isChecked())
                        {//夹紧检测开始
                            if(ui->combo_General_PassStart->currentText() == m_Port_X[SIGNAL_DETECTION_METERIAL_1_CLAMP_LIMIT].modifyName)
                            {
                                data_Temp[9] = m_Port_X[SIGNAL_DETECTION_METERIAL_1_CLAMP_LIMIT].portNum;
                            }
                            else if(ui->combo_General_PassStart->currentText() == m_Port_X[SIGNAL_DETECTION_PRODUCT_1_CLAMP_LIMIT].modifyName)
                            {
                                data_Temp[9] = m_Port_X[SIGNAL_DETECTION_PRODUCT_1_CLAMP_LIMIT].portNum;
                            }
                            else if(ui->combo_General_PassStart->currentText() == m_Port_X[SIGNAL_DETECTION_CHUCK_1_CLAMP_LIMIT].modifyName)
                            {
                                data_Temp[9] = m_Port_X[SIGNAL_DETECTION_CHUCK_1_CLAMP_LIMIT].portNum;
                            }
                            data_Temp[10] = 1;
                            g_InsertProOrder(data_Temp);
                        }
                        else if(ui->cb_General_PassStop->isChecked())
                        {//夹紧检测结束
                            if(ui->combo_General_PassStop->currentText() == m_Port_X[SIGNAL_DETECTION_METERIAL_1_CLAMP_LIMIT].modifyName)
                            {
                                data_Temp[9] = m_Port_X[SIGNAL_DETECTION_METERIAL_1_CLAMP_LIMIT].portNum;
                            }
                            else if(ui->combo_General_PassStop->currentText() == m_Port_X[SIGNAL_DETECTION_PRODUCT_1_CLAMP_LIMIT].modifyName)
                            {
                                data_Temp[9] = m_Port_X[SIGNAL_DETECTION_PRODUCT_1_CLAMP_LIMIT].portNum;
                            }
                            else if(ui->combo_General_PassStop->currentText() == m_Port_X[SIGNAL_DETECTION_CHUCK_1_CLAMP_LIMIT].modifyName)
                            {
                                data_Temp[9] = m_Port_X[SIGNAL_DETECTION_CHUCK_1_CLAMP_LIMIT].portNum;
                            }
                            data_Temp[10] = 0;
                            g_InsertProOrder(data_Temp);
                        }
                        else if(ui->cb_General_PineStart->isChecked())
                        {//松开检测开始
                            if(ui->combo_General_PineStart->currentText() == m_Port_X[SIGNAL_DETECTION_METERIAL_1_LOOSENED_LIMIT].modifyName)
                            {
                                data_Temp[9] = m_Port_X[SIGNAL_DETECTION_METERIAL_1_LOOSENED_LIMIT].portNum;
                            }
                            else if(ui->combo_General_PineStart->currentText() == m_Port_X[SIGNAL_DETECTION_PRODUCT_1_LOOSENED_LIMIT].modifyName)
                            {
                                data_Temp[9] = m_Port_X[SIGNAL_DETECTION_PRODUCT_1_LOOSENED_LIMIT].portNum;
                            }
                            else if(ui->combo_General_PineStart->currentText() == m_Port_X[SIGNAL_DETECTION_CHUCK_1_LOOSENED_LIMIT].modifyName)
                            {
                                data_Temp[9] = m_Port_X[SIGNAL_DETECTION_CHUCK_1_LOOSENED_LIMIT].portNum;
                            }
                            data_Temp[10] = 2;
                            g_InsertProOrder(data_Temp);
                        }
                        else if(ui->cb_General_PineEnd->isChecked())
                        {//松开检测结束
                            if(ui->combo_General_PineEnd->currentText() == m_Port_X[SIGNAL_DETECTION_METERIAL_1_LOOSENED_LIMIT].modifyName)
                            {
                                data_Temp[9] = m_Port_X[SIGNAL_DETECTION_METERIAL_1_LOOSENED_LIMIT].portNum;
                            }
                            else if(ui->combo_General_PineEnd->currentText() == m_Port_X[SIGNAL_DETECTION_PRODUCT_1_LOOSENED_LIMIT].modifyName)
                            {
                                data_Temp[9] = m_Port_X[SIGNAL_DETECTION_PRODUCT_1_LOOSENED_LIMIT].portNum;
                            }
                            else if(ui->combo_General_PineEnd->currentText() == m_Port_X[SIGNAL_DETECTION_CHUCK_1_LOOSENED_LIMIT].modifyName)
                            {
                                data_Temp[9] = m_Port_X[SIGNAL_DETECTION_CHUCK_1_LOOSENED_LIMIT].portNum;
                            }
                            data_Temp[10] = 0;
                            g_InsertProOrder(data_Temp);
                        }
                    }
                    else if(ui->tabWidget_Reserve->currentWidget() == ui->tabOutputType)
                    {//预留检测界面
                        data_Temp[5] = C_RESERVE_CHECK;
                        if(ui->cb_General_ReserveOn->isChecked())
                        {//预留检测ON
                            data_Temp[9] = ui->lineEdit_ReserveOn->getCurrentPort();
                            data_Temp[10] = 1;
                            g_InsertProOrder(data_Temp);
                        }
                        else if(ui->cb_General_ReserveOff->isChecked())
                        {//预留检测OFF
                            data_Temp[9] = ui->lineEdit_ReserveOff->getCurrentPort();
                            data_Temp[10] = 2;
                            g_InsertProOrder(data_Temp);
                        }
                        else if(ui->cb_General_ReserveEnd->isChecked())
                        {//预留检测END
                            data_Temp[9] = ui->lineEdit_ReserveEnd->getCurrentPort();
                            data_Temp[10] = 0;
                            g_InsertProOrder(data_Temp);
                        }
                    }
                    Teach_File_List_Refresh();//刷新程序列表
                }
                else if(ui->stackedWidget_Four->currentWidget() == ui->Machine_action_page){//机床动作界面
                    double MachineDelay = 0;
                    data_Temp[0] = m_OperateProNum;  //程序编号
                    data_Temp[1] = m_OperateProOrder[m_CurrentSelectProOrderList].list;
                    data_Temp[2] = m_OperateProOrder[m_CurrentSelectProOrderList].list>>8;
                    data_Temp[3] = m_OperateProOrder[m_CurrentSelectProOrderList].runOrderNum;
                    data_Temp[4] = m_OperateProOrder[m_CurrentSelectProOrderList].runOrderNum>>8;
                    data_Temp[5] = C_MACHINE_OUT;
                    data_Temp[6] = 0;
                    if(ui->Start_Up_Mach_chbox->isChecked()){//启动加工1通/断-选中
                        MachineDelay = ui->lineEdit_Start_Up_Mach->text().toDouble();
                        data_Temp[7] = (uint16_t)(MachineDelay*100);
                        data_Temp[8] = (uint16_t)(MachineDelay*100)>>8;
                        data_Temp[9] = m_Port_Y[MACHINE_START_PROCESS_1].portNum;
                        data_Temp[10] = Temp_MachineOutStruct[0].type;
                        g_InsertProOrder(data_Temp);
                    }
                    else if(ui->Main_Axis_Rotate_chbox->isChecked()){//主轴旋转1通/断-选中
                        MachineDelay = ui->lineEdit_Main_Axis_Rotate->text().toDouble();
                        data_Temp[7] = (uint16_t)(MachineDelay*100);
                        data_Temp[8] = (uint16_t)(MachineDelay*100)>>8;
                        data_Temp[9] = m_Port_Y[MACHINE_SPINDLE_ROTATE_1].portNum;
                        data_Temp[10] = Temp_MachineOutStruct[1].type;
                        g_InsertProOrder(data_Temp);
                    }
                    else if(ui->Main_Axis_locat_chbox->isChecked()){//主轴定位1通/断-选中
                        MachineDelay = ui->lineEdit_Main_Axis_locat->text().toDouble();
                        data_Temp[7] = (uint16_t)(MachineDelay*100);
                        data_Temp[8] = (uint16_t)(MachineDelay*100)>>8;
                        data_Temp[9] = m_Port_Y[MACHINE_SPINDLE_FIXED_POS_1].portNum;
                        data_Temp[10] = Temp_MachineOutStruct[2].type;
                        g_InsertProOrder(data_Temp);
                    }
                    else if(ui->Auto_Door_chbox->isChecked()){//自动门1开/关-选中
                        MachineDelay = ui->lineEdit_Auto_Door->text().toDouble();
                        data_Temp[7] = (uint16_t)(MachineDelay*100);
                        data_Temp[8] = (uint16_t)(MachineDelay*100)>>8;
                        if(Temp_MachineOutStruct[3].type == 1)
                        {//自动门开
                            data_Temp[9] = m_Port_Y[MACHINE_AUTO_DOOR_1_OPEN].portNum;
                        }
                        else if(Temp_MachineOutStruct[3].type == 0)
                        {//自动门关
                            data_Temp[9] = m_Port_Y[MACHINE_AUTO_DOOR_1_OPEN].portNum;
                        }
                        data_Temp[10] = Temp_MachineOutStruct[3].type;
                        g_InsertProOrder(data_Temp);
                    }
                    else if(ui->Blow_Air_chbox->isChecked()){//吹气1通/断-选中
                        MachineDelay = ui->lineEdit_Blow_Air->text().toDouble();
                        data_Temp[7] = (uint16_t)(MachineDelay*100);
                        data_Temp[8] = (uint16_t)(MachineDelay*100)>>8;
                        data_Temp[9] = m_Port_Y[MACHINE_GASS_1_LOOSENED].portNum;
                        data_Temp[10] = Temp_MachineOutStruct[4].type;
                        g_InsertProOrder(data_Temp);
                    }
                    else if(ui->Chuck_chbox->isChecked()){//卡盘1夹紧/松开-选中
                        MachineDelay = ui->lineEdit_Chuck->text().toDouble();
                        data_Temp[7] = (uint16_t)(MachineDelay*100);
                        data_Temp[8] = (uint16_t)(MachineDelay*100)>>8;
                        if(Temp_MachineOutStruct[5].type == 0)
                        {//卡盘1松开
                            data_Temp[9] = m_Port_Y[MACHINE_CHUCK_1_CLAMP].portNum;
                        }
                        else if(Temp_MachineOutStruct[5].type == 1)
                        {//卡盘1夹紧
                            data_Temp[9] = m_Port_Y[MACHINE_CHUCK_1_CLAMP].portNum;
                        }
                        data_Temp[10] = Temp_MachineOutStruct[5].type;
                        g_InsertProOrder(data_Temp);
                    }
                    Teach_File_List_Refresh();//刷新程序列表
                }
                else if(ui->stackedWidget_Four->currentWidget() == ui->Other_Order_page){//其他指令动作界面
                    data_Temp[0] = m_OperateProNum;  //程序编号
                    data_Temp[1] = m_OperateProOrder[m_CurrentSelectProOrderList].list;
                    data_Temp[2] = m_OperateProOrder[m_CurrentSelectProOrderList].list>>8;
                    data_Temp[3] = m_OperateProOrder[m_CurrentSelectProOrderList].runOrderNum;
                    data_Temp[4] = m_OperateProOrder[m_CurrentSelectProOrderList].runOrderNum>>8;
                    data_Temp[6] = 0;
                    if(ui->AlarmDelay_chbox->isChecked())
                    {//报警延时
                        data_Temp[5] = C_OTHER_DELAY;
                        g_InsertProOrder(data_Temp);
                    }
                    else if(ui->AlarmCust_chbox->isChecked())
                    {//报警自定义
                        data_Temp[5] = C_OTHER_ALARM_CUST;
                        data_Temp[9] = Temp_OtherAlarmCustStruct.alarmNum;
                        data_Temp[10] = Temp_OtherAlarmCustStruct.alarmNum>>8;
                        data_Temp[11] = Temp_OtherAlarmCustStruct.type;
                        g_InsertProOrder(data_Temp);
                    }
                    else if(ui->CycStop_chbox->isChecked())
                    {//周期停止
                        data_Temp[5] = C_OTHER_CYC_STOP;
                        data_Temp[9] = Temp_OtherCycStopCustStruct.cycleNum;
                        data_Temp[10] = Temp_OtherCycStopCustStruct.cycleNum>>8;
                        g_InsertProOrder(data_Temp);
                    }
                    else if(ui->AlarmLamp_chbox->isChecked())
                    {//报警灯
                        data_Temp[5] = C_OTHER_ALARM_LAMP;
                        data_Temp[9] = Temp_OtherAlarmLampStruct.outportNum;
                        data_Temp[10] = Temp_OtherAlarmLampStruct.type;

                        g_InsertProOrder(data_Temp);
                    }
                    else if(ui->AlarmSound_chbox->isChecked())
                    {//报警声
                        data_Temp[5] = C_OTHER_ALARM_SOUND;
                        data_Temp[9] = Temp_OtherAlarmSoundStruct.outportNum;
                        data_Temp[10] = Temp_OtherAlarmSoundStruct.type;
                        g_InsertProOrder(data_Temp);
                    }
                    Teach_File_List_Refresh();//刷新程序列表
                }
            }
            else if(ui->stackedWidget_General->currentWidget() == ui->Stack_page){//堆叠界面
                data_Temp[0] = m_OperateProNum;  //程序编号
                data_Temp[1] = m_OperateProOrder[m_CurrentSelectProOrderList].list;
                data_Temp[2] = m_OperateProOrder[m_CurrentSelectProOrderList].list>>8;
                data_Temp[3] = m_OperateProOrder[m_CurrentSelectProOrderList].runOrderNum;
                data_Temp[4] = m_OperateProOrder[m_CurrentSelectProOrderList].runOrderNum>>8;
                data_Temp[5] = C_STACK_MOVE;
                data_Temp[6] = 0;
                if(ui->Stack_group_1->isChecked())
                {
                    data_Temp[33] = 1;//堆叠组号，1-11
                    g_InsertProOrder(data_Temp);
                }
                else if(ui->Stack_group_2->isChecked())
                {
                    data_Temp[33] = 2;
                    g_InsertProOrder(data_Temp);
                }
                else if(ui->Stack_group_3->isChecked())
                {
                    data_Temp[33] = 3;
                    g_InsertProOrder(data_Temp);
                }
                else if(ui->Stack_group_4->isChecked())
                {
                    data_Temp[33] = 4;
                    g_InsertProOrder(data_Temp);
                }
                else if(ui->Stack_group_5->isChecked())
                {
                    data_Temp[33] = 5;
                    g_InsertProOrder(data_Temp);
                }
                else if(ui->Stack_group_6->isChecked())
                {
                    data_Temp[33] = 6;
                    g_InsertProOrder(data_Temp);
                }
                else if(ui->Stack_group_7->isChecked())
                {
                    data_Temp[33] = 7;
                    g_InsertProOrder(data_Temp);
                }
                else if(ui->Stack_group_8->isChecked())
                {
                    data_Temp[33] = 8;
                    g_InsertProOrder(data_Temp);
                }
                else if(ui->Stack_group_9->isChecked())
                {
                    data_Temp[33] = 9;
                    g_InsertProOrder(data_Temp);
                }
                else if(ui->Stack_group_10->isChecked())
                {
                    data_Temp[33] = 10;
                    g_InsertProOrder(data_Temp);
                }
                else if(ui->Stack_group_11->isChecked())
                {
                    data_Temp[33] = 11;
                    g_InsertProOrder(data_Temp);
                }
                else if(ui->Follow_Stack_group_1->isChecked())
                {
                    data_Temp[5] = C_STACK_FOLLOW;
                    data_Temp[33] = 1;
                    g_InsertProOrder(data_Temp);
                }
                else if(ui->Follow_Stack_group_2->isChecked())
                {
                    data_Temp[5] = C_STACK_FOLLOW;
                    data_Temp[33] = 2;
                    g_InsertProOrder(data_Temp);
                }
                else if(ui->Follow_Stack_group_3->isChecked())
                {
                    data_Temp[5] = C_STACK_FOLLOW;
                    data_Temp[33] = 3;
                    g_InsertProOrder(data_Temp);
                }
                else if(ui->Follow_Stack_group_4->isChecked())
                {
                    data_Temp[5] = C_STACK_FOLLOW;
                    data_Temp[33] = 4;
                    g_InsertProOrder(data_Temp);
                }
                else if(ui->Stack_reset->isChecked())
                {
                    data_Temp[5] = C_STACK_RESET_ZERO;
                    data_Temp[11] = ui->Stack_reset_select_box->currentIndex()+1;
                    g_InsertProOrder(data_Temp);
                }
                Teach_File_List_Refresh();//刷新程序列表
            }
            else if(ui->stackedWidget_General->currentWidget() == ui->Reserve_Out_page){//预留输出

                data_Temp[0] = m_OperateProNum;  //程序编号
                data_Temp[1] = m_OperateProOrder[m_CurrentSelectProOrderList].list;
                data_Temp[2] = m_OperateProOrder[m_CurrentSelectProOrderList].list>>8;
                data_Temp[3] = m_OperateProOrder[m_CurrentSelectProOrderList].runOrderNum;
                data_Temp[4] = m_OperateProOrder[m_CurrentSelectProOrderList].runOrderNum>>8;
                data_Temp[5] = C_RESERVE_OUT;
                data_Temp[6] = 0;
                Temp_ReserveOutStruct.interval = ui->lineEdit_Reserve_num->text().toUInt();
                double Reserve_time = ui->lineEdit_Reserve_time->text().toDouble();
                data_Temp[7] = (uint16_t)(Reserve_time*100);
                data_Temp[8] = (uint16_t)(Reserve_time*100)>>8;
                data_Temp[9] = Temp_ReserveOutStruct.interval;
                data_Temp[10] = Temp_ReserveOutStruct.interval>>8;
                data_Temp[12] = Temp_ReserveOutStruct.function;
                data_Temp[13] = Temp_ReserveOutStruct.type;
                if(Temp_ReserveOutStruct.function == 0)
                {
                    data_Temp[13] = ui->Reserve_Pass_Button->getState();
                    Temp_ReserveOutStruct.outportNum = ui->lineEdit_Reserve_Pass_Button->getCurrentPort();
                    data_Temp[11] = Temp_ReserveOutStruct.outportNum;
                    if(ui->lineEdit_Reserve_Pass_Button->text()!="")
                    {
                        g_InsertProOrder(data_Temp);
                    }
                }
                else if(Temp_ReserveOutStruct.function == 1)
                {
                    Temp_ReserveOutStruct.outportNum = ui->lineEdit_Reserve_OutTime->getCurrentPort();
                    data_Temp[11] = Temp_ReserveOutStruct.outportNum;
                    if(ui->lineEdit_Reserve_OutTime->text()!="")
                    {
                        g_InsertProOrder(data_Temp);
                    }
                }
                else if(Temp_ReserveOutStruct.function == 2)
                {
                    Temp_ReserveOutStruct.outportNum = ui->lineEdit_Reserve_Overturn->getCurrentPort();
                    data_Temp[11] = Temp_ReserveOutStruct.outportNum;
                    if(ui->lineEdit_Reserve_Overturn->text()!="")
                    {
                        g_InsertProOrder(data_Temp);
                    }
                }
                Teach_File_List_Refresh();//刷新程序列表
            }
            else if(ui->stackedWidget_General->currentWidget()  == ui->Signal_wait_page){//信号等待界面
                data_Temp[0] = m_OperateProNum;  //程序编号
                data_Temp[1] = m_OperateProOrder[m_CurrentSelectProOrderList].list;
                data_Temp[2] = m_OperateProOrder[m_CurrentSelectProOrderList].list>>8;
                data_Temp[3] = m_OperateProOrder[m_CurrentSelectProOrderList].runOrderNum;
                data_Temp[4] = m_OperateProOrder[m_CurrentSelectProOrderList].runOrderNum>>8;
                data_Temp[6] = 0;
                switch(ui->tabWgtSigWait->currentIndex())
                {
                case 0://等待机床界面
                    if(ui->Wait_machine_signal_box->isChecked())
                    {
                        Temp_WaitInMachineStruct.inportSta = 1;//默认有信号
                        if(ui->Wait_machine_signal_port->currentText() == m_Port_X[SIGNAL_WAITING_PROCESS_COMPLETE_1].modifyName)
                        {
                            Temp_WaitInMachineStruct.inportNum = m_Port_X[SIGNAL_WAITING_PROCESS_COMPLETE_1].portNum;
                        }
                        else if(ui->Wait_machine_signal_port->currentText() == m_Port_X[SIGNAL_WAITING_FIXED_POS_COMPLETE_1].modifyName)
                        {
                            Temp_WaitInMachineStruct.inportNum = m_Port_X[SIGNAL_WAITING_FIXED_POS_COMPLETE_1].portNum;
                        }
                        else if(ui->Wait_machine_signal_port->currentText() == m_Port_X[SIGNAL_WAITING_KNIFE_SEAT_ORIGIN_1].modifyName)
                        {
                            Temp_WaitInMachineStruct.inportNum = m_Port_X[SIGNAL_WAITING_KNIFE_SEAT_ORIGIN_1].portNum;
                        }
                        data_Temp[5] = C_WAIT_IN_MACHINE;
                        data_Temp[9] = Temp_WaitInMachineStruct.backListNum;
                        data_Temp[10] = Temp_WaitInMachineStruct.backListNum>>8;
                        data_Temp[11] = Temp_WaitInMachineStruct.inportNum;
                        data_Temp[12] = Temp_WaitInMachineStruct.inportSta;
                        data_Temp[13] = Temp_WaitInMachineStruct.type;
                        data_Temp[14] = Temp_WaitInMachineStruct.label;
                        g_InsertProOrder(data_Temp);
                    }
                    break;
                case 1://等待卡爪界面
                {
                    if(ui->Wait_clamp_on_chbox->isChecked())
                    {
                        if(ui->Wait_clamp_on_comboBox->currentText() == m_Port_X[SIGNAL_WAITING_METERIAL_1_CLAMP_LIMIT].modifyName)
                        {
                            Temp_WaitInClawStruct.inportNum =  m_Port_X[SIGNAL_WAITING_METERIAL_1_CLAMP_LIMIT].portNum;
                        }
                        else if(ui->Wait_clamp_on_comboBox->currentText() == m_Port_X[SIGNAL_WAITING_PRODUCT_1_CLAMP_LIMIT].modifyName)
                        {
                            Temp_WaitInClawStruct.inportNum =  m_Port_X[SIGNAL_WAITING_PRODUCT_1_CLAMP_LIMIT].portNum;
                        }
                        else if(ui->Wait_clamp_on_comboBox->currentText() == m_Port_X[SIGNAL_WAITING_CHUCK_1_CLAMP_LIMIT].modifyName)
                        {
                            Temp_WaitInClawStruct.inportNum =  m_Port_X[SIGNAL_WAITING_CHUCK_1_CLAMP_LIMIT].portNum;
                        }
                    }
                    else if(ui->Wait_release_on_chbox->isChecked())
                    {
                        if(ui->Wait_release_on_comboBox->currentText() == m_Port_X[SIGNAL_WAITING_METERIAL_1_LOOSENED_LIMIT].modifyName)
                        {
                            Temp_WaitInClawStruct.inportNum =  m_Port_X[SIGNAL_WAITING_METERIAL_1_LOOSENED_LIMIT].portNum;
                        }
                        else if(ui->Wait_release_on_comboBox->currentText() == m_Port_X[SIGNAL_WAITING_PRODUCT_1_LOOSENED_LIMIT].modifyName)
                        {
                            Temp_WaitInClawStruct.inportNum =  m_Port_X[SIGNAL_WAITING_PRODUCT_1_LOOSENED_LIMIT].portNum;
                        }
                        else if(ui->Wait_release_on_comboBox->currentText() == m_Port_X[SIGNAL_WAITING_CHUCK_1_LOOSENED_LIMIT].modifyName)
                        {
                            Temp_WaitInClawStruct.inportNum =  m_Port_X[SIGNAL_WAITING_CHUCK_1_LOOSENED_LIMIT].portNum;
                        }
                    }
                    else if(ui->Wait_clamp_off_chbox->isChecked())
                    {
                        if(ui->Wait_clamp_off_comboBox->currentText() == m_Port_X[SIGNAL_WAITING_METERIAL_1_CLAMP_LIMIT].modifyName)
                        {
                            Temp_WaitInClawStruct.inportNum =  m_Port_X[SIGNAL_WAITING_METERIAL_1_CLAMP_LIMIT].portNum;
                        }
                        else if(ui->Wait_clamp_off_comboBox->currentText() == m_Port_X[SIGNAL_WAITING_PRODUCT_1_CLAMP_LIMIT].modifyName)
                        {
                            Temp_WaitInClawStruct.inportNum =  m_Port_X[SIGNAL_WAITING_PRODUCT_1_CLAMP_LIMIT].portNum;
                        }
                        else if(ui->Wait_clamp_off_comboBox->currentText() == m_Port_X[SIGNAL_WAITING_CHUCK_1_CLAMP_LIMIT].modifyName)
                        {
                            Temp_WaitInClawStruct.inportNum =  m_Port_X[SIGNAL_WAITING_CHUCK_1_CLAMP_LIMIT].portNum;
                        }
                    }
                    else if(ui->Wait_release_off_chbox->isChecked())
                    {
                        if(ui->Wait_release_off_comboBox->currentText() == m_Port_X[SIGNAL_WAITING_METERIAL_1_LOOSENED_LIMIT].modifyName)
                        {
                            Temp_WaitInClawStruct.inportNum =  m_Port_X[SIGNAL_WAITING_METERIAL_1_LOOSENED_LIMIT].portNum;
                        }
                        else if(ui->Wait_release_off_comboBox->currentText() == m_Port_X[SIGNAL_WAITING_PRODUCT_1_LOOSENED_LIMIT].modifyName)
                        {
                            Temp_WaitInClawStruct.inportNum =  m_Port_X[SIGNAL_WAITING_PRODUCT_1_LOOSENED_LIMIT].portNum;
                        }
                        else if(ui->Wait_release_off_comboBox->currentText() == m_Port_X[SIGNAL_WAITING_CHUCK_1_LOOSENED_LIMIT].modifyName)
                        {
                            Temp_WaitInClawStruct.inportNum =  m_Port_X[SIGNAL_WAITING_CHUCK_1_LOOSENED_LIMIT].portNum;
                        }
                    }
                    else if(ui->Wait_pos_cor_chbox->isChecked())
                    {
                        if(ui->Wait_pos_cor_comboBox->currentText() == m_Port_X[SIGNAL_WAITING_METERIAL_1_CLAMP].modifyName)
                        {
                            Temp_WaitInClawStruct.inportNum =  m_Port_Y[SIGNAL_WAITING_METERIAL_1_CLAMP].portNum;
                        }
                        else if(ui->Wait_pos_cor_comboBox->currentText() == m_Port_X[SIGNAL_WAITING_PRODUCT_1_CLAMP].modifyName)
                        {
                            Temp_WaitInClawStruct.inportNum =  m_Port_Y[SIGNAL_WAITING_PRODUCT_1_CLAMP].portNum;
                        }
                        else if(ui->Wait_pos_cor_comboBox->currentText() == m_Port_X[SIGNAL_WAITING_CHUCK_1_CLAMP].modifyName)
                        {
                            Temp_WaitInClawStruct.inportNum =  m_Port_Y[SIGNAL_WAITING_CHUCK_1_CLAMP].portNum;
                        }
                    }
                    uint16_t labelnum = ReturnLabelnum(ui->coboxReturnLabe_box->currentText());
                    Temp_WaitInClawStruct.label = labelnum;
                    Temp_WaitInClawStruct.backListNum = ui->editReturnStepNum->text().toInt();
                    data_Temp[5] = C_WAIT_IN_CLAW;
                    data_Temp[9] = Temp_WaitInClawStruct.backListNum;
                    data_Temp[10] = Temp_WaitInClawStruct.backListNum>>8;
                    data_Temp[11] = Temp_WaitInClawStruct.inportNum;
                    data_Temp[13] = Temp_WaitInClawStruct.type;
                    data_Temp[14] = Temp_WaitInClawStruct.label;
                    if(ui->Wait_clamp_on_chbox->isChecked() || ui->Wait_release_on_chbox->isChecked())
                    {
                        Temp_WaitInClawStruct.inportSta = 1;
                        data_Temp[12] = Temp_WaitInClawStruct.inportSta;
                        g_InsertProOrder(data_Temp);
                    }
                    else if(ui->Wait_clamp_off_chbox->isChecked() || ui->Wait_release_off_chbox->isChecked())
                    {
                        Temp_WaitInClawStruct.inportSta = 0;
                        data_Temp[12] = Temp_WaitInClawStruct.inportSta;
                        g_InsertProOrder(data_Temp);
                    }
                    else if(ui->Wait_pos_cor_chbox->isChecked())
                    {
                        Temp_WaitInClawStruct.inportSta = 2;
                        data_Temp[12] = Temp_WaitInClawStruct.inportSta;
                        g_InsertProOrder(data_Temp);
                    }
                    break;
                }
                case 2://等待预留界面
                {
                    uint16_t labelnum = ReturnLabelnum(ui->coboxReturnLabe_box->currentText());
                    Temp_WaitInReserveStruct.label = labelnum;
                    Temp_WaitInReserveStruct.backListNum = ui->editReturnStepNum->text().toInt();
                    data_Temp[5] = C_WAIT_IN_RESERVE;
                    data_Temp[9] = Temp_WaitInReserveStruct.backListNum;
                    data_Temp[10] = Temp_WaitInReserveStruct.backListNum>>8;
                    data_Temp[13] = Temp_WaitInReserveStruct.type;
                    data_Temp[14] = Temp_WaitInReserveStruct.label;
                    if(ui->Wait_Reserve_on_chbox->isChecked())
                    {
                        Temp_WaitInReserveStruct.inportSta = 1;
                        Temp_WaitInReserveStruct.inportNum = ui->lineEdit_Wait_ReserveOn->getCurrentPort();
                        data_Temp[11] = Temp_WaitInReserveStruct.inportNum;
                        data_Temp[12] = Temp_WaitInReserveStruct.inportSta;
                        if(ui->lineEdit_Wait_ReserveOn->text()!="")
                        {
                            g_InsertProOrder(data_Temp);
                        }
                    }
                    else if(ui->Wait_Reserve_off_chbox->isChecked())
                    {
                        Temp_WaitInReserveStruct.inportSta = 0;
                        Temp_WaitInReserveStruct.inportNum = ui->lineEdit_Wait_ReserveOFF->getCurrentPort();
                        data_Temp[11] = Temp_WaitInReserveStruct.inportNum;
                        data_Temp[12] = Temp_WaitInReserveStruct.inportSta;
                        if(ui->lineEdit_Wait_ReserveOFF->text()!="")
                        {
                            g_InsertProOrder(data_Temp);
                        }
                    }
                    else if(ui->Wait_Reserve_up_chbox->isChecked())
                    {
                        Temp_WaitInReserveStruct.inportSta = 2;
                        Temp_WaitInReserveStruct.inportNum = ui->lineEdit_Wait_ReserveUp->getCurrentPort();
                        data_Temp[11] = Temp_WaitInReserveStruct.inportNum;
                        data_Temp[12] = Temp_WaitInReserveStruct.inportSta;
                        if(ui->lineEdit_Wait_ReserveUp->text()!="")
                        {
                            g_InsertProOrder(data_Temp);
                        }

                    }
                    else if(ui->Wait_Reserve_down_chbox->isChecked())
                    {
                        Temp_WaitInReserveStruct.inportSta = 3;
                        Temp_WaitInReserveStruct.inportNum = ui->lineEdit_Wait_ReserveDown->getCurrentPort();
                        data_Temp[11] = Temp_WaitInReserveStruct.inportNum;
                        data_Temp[12] = Temp_WaitInReserveStruct.inportSta;
                        if(ui->lineEdit_Wait_ReserveDown->text()!="")
                        {
                            g_InsertProOrder(data_Temp);
                        }
                    }
                    else if(ui->Wait_Reserve_Jump_chbox->isChecked())
                    {
                        Temp_WaitInReserveStruct.inportSta = 4;
                        Temp_WaitInReserveStruct.inportNum = ui->lineEdit_Wait_ReserveJump->getCurrentPort();
                        data_Temp[11] = Temp_WaitInReserveStruct.inportNum;
                        data_Temp[12] = Temp_WaitInReserveStruct.inportSta;
                        if(ui->lineEdit_Wait_ReserveJump->text()!="")
                        {
                            g_InsertProOrder(data_Temp);
                        }
                    }
                    break;
                }
                default:
                    break;
                }
                Teach_File_List_Refresh();//刷新程序列表
            }
        }
        else if(ui->tabWidget_Teach->currentWidget() == ui->tabAdvance){//高级指令界面
            if(ui->stackedWidget_Senior->currentWidget() == ui->SunPro_page){//子程序指令
                data_Temp[0] = m_OperateProNum;  //程序编号
                data_Temp[1] = m_OperateProOrder[m_CurrentSelectProOrderList].list;
                data_Temp[2] = m_OperateProOrder[m_CurrentSelectProOrderList].list>>8;
                data_Temp[3] = m_OperateProOrder[m_CurrentSelectProOrderList].runOrderNum;
                data_Temp[4] = m_OperateProOrder[m_CurrentSelectProOrderList].runOrderNum>>8;
                data_Temp[5] = C_SUN_PRO;
                data_Temp[6] = 0;
                data_Temp[9] = Temp_SunProStruct.sunProNum;
                data_Temp[10] = Temp_SunProStruct.oprMode;
                g_InsertProOrder(data_Temp);
                Teach_File_List_Refresh();//刷新程序列表
            }
            else if(ui->stackedWidget_Senior->currentWidget() == ui->ServoStop_page){//伺服停止指令
                if(ui->cb_ServoStop_AxisAll->isChecked())
                {
                    double delay = ui->lineEdit_ServoStop_delay->text().toDouble();
                    data_Temp[0] = m_OperateProNum;  //程序编号
                    data_Temp[1] = m_OperateProOrder[m_CurrentSelectProOrderList].list;
                    data_Temp[2] = m_OperateProOrder[m_CurrentSelectProOrderList].list>>8;
                    data_Temp[3] = m_OperateProOrder[m_CurrentSelectProOrderList].runOrderNum;
                    data_Temp[4] = m_OperateProOrder[m_CurrentSelectProOrderList].runOrderNum>>8;
                    data_Temp[5] = C_AXIS_STOP;
                    data_Temp[6] = 0;
                    data_Temp[7] = (uint16_t)(delay*100);
                    data_Temp[8] = (uint16_t)(delay*100)>>8;
                    data_Temp[9] = Temp_AxisStopStruct.axis;
                    g_InsertProOrder(data_Temp);
                    Teach_File_List_Refresh();//刷新程序列表
                }
            }
            else if(ui->stackedWidget_Senior->currentWidget() == ui->LowSpeedTorquw_page){//转矩保护指令
                if(ui->cb_LowSpeedTorque_AxisAll->isChecked())
                {
                    uint16_t delay = ui->lineEdit_LowSpeedTorque_delay->text().toUInt();
                    data_Temp[0] = m_OperateProNum;  //程序编号
                    data_Temp[1] = m_OperateProOrder[m_CurrentSelectProOrderList].list;
                    data_Temp[2] = m_OperateProOrder[m_CurrentSelectProOrderList].list>>8;
                    data_Temp[3] = m_OperateProOrder[m_CurrentSelectProOrderList].runOrderNum;
                    data_Temp[4] = m_OperateProOrder[m_CurrentSelectProOrderList].runOrderNum>>8;
                    data_Temp[5] = C_TORQUE_GARD;
                    data_Temp[6] = 0;
                    data_Temp[7] = 0/*(delay*100)*/;
                    data_Temp[8] = 0/*(delay*100)>>8*/;
                    data_Temp[9] = Temp_TorqueGardStruct.axis;
                    data_Temp[10] = Temp_TorqueGardStruct.torqueValue;
                    g_InsertProOrder(data_Temp);
                    Teach_File_List_Refresh();//刷新程序列表
                }
            }
            else if(ui->stackedWidget_Senior->currentWidget() == ui->OffsetAxis_page){//偏移指令
                if(ui->cb_Shift_AxisAll->isChecked())
                {
                    data_Temp[0] = m_OperateProNum;  //程序编号
                    data_Temp[1] = m_OperateProOrder[m_CurrentSelectProOrderList].list;
                    data_Temp[2] = m_OperateProOrder[m_CurrentSelectProOrderList].list>>8;
                    data_Temp[3] = m_OperateProOrder[m_CurrentSelectProOrderList].runOrderNum;
                    data_Temp[4] = m_OperateProOrder[m_CurrentSelectProOrderList].runOrderNum>>8;
                    data_Temp[5] = C_OFFSET_AXIS;
                    data_Temp[6] = 0;
                    data_Temp[9] = Temp_OffsetAxisStruct.offsetPos;
                    data_Temp[10] = Temp_OffsetAxisStruct.offsetPos>>8;
                    data_Temp[11] = Temp_OffsetAxisStruct.offsetPos>>16;
                    data_Temp[12] = Temp_OffsetAxisStruct.offsetPos>>24;
                    data_Temp[21] = Temp_OffsetAxisStruct.axis;
                    data_Temp[22] = Temp_OffsetAxisStruct.speed;
                    g_InsertProOrder(data_Temp);
                    Teach_File_List_Refresh();//刷新程序列表
                }
            }
            else if(ui->stackedWidget_Senior->currentWidget() == ui->Search_page){//搜索指令界面
                data_Temp[0] = m_OperateProNum;  //程序编号
                data_Temp[1] = m_OperateProOrder[m_CurrentSelectProOrderList].list;
                data_Temp[2] = m_OperateProOrder[m_CurrentSelectProOrderList].list>>8;
                data_Temp[3] = m_OperateProOrder[m_CurrentSelectProOrderList].runOrderNum;
                data_Temp[4] = m_OperateProOrder[m_CurrentSelectProOrderList].runOrderNum>>8;
                data_Temp[6] = 0;
                if(ui->chboxSearchAxisSelect->isChecked())
                {
                    Temp_SearchAxisMoveStruct.searchNum = ui->coboxSearchNum->currentIndex()+1;
                    data_Temp[5] = C_SEARCH_AXIS_MOVE;
                    data_Temp[9] = Temp_SearchAxisMoveStruct.maxPos;
                    data_Temp[10] = Temp_SearchAxisMoveStruct.maxPos>>8;
                    data_Temp[11] = Temp_SearchAxisMoveStruct.maxPos>>16;
                    data_Temp[12] = Temp_SearchAxisMoveStruct.maxPos>>24;
                    data_Temp[13] = Temp_SearchAxisMoveStruct.advCDis;
                    data_Temp[14] = Temp_SearchAxisMoveStruct.advCDis>>8;
                    data_Temp[15] = Temp_SearchAxisMoveStruct.advCDis>>16;
                    data_Temp[16] = Temp_SearchAxisMoveStruct.advCDis>>24;
                    data_Temp[17] = Temp_SearchAxisMoveStruct.offsetDis;
                    data_Temp[18] = Temp_SearchAxisMoveStruct.offsetDis>>8;
                    data_Temp[19] = Temp_SearchAxisMoveStruct.offsetDis>>16;
                    data_Temp[20] = Temp_SearchAxisMoveStruct.offsetDis>>24;
                    data_Temp[21] = Temp_SearchAxisMoveStruct.axis;
                    data_Temp[22] = Temp_SearchAxisMoveStruct.posStoreFlag;
                    data_Temp[23] = Temp_SearchAxisMoveStruct.reachPosAlarmFlag;
                    data_Temp[24] = Temp_SearchAxisMoveStruct.runSpeed;
                    data_Temp[25] = Temp_SearchAxisMoveStruct.advCSpeed;
                    data_Temp[26] = Temp_SearchAxisMoveStruct.searchNum;
                    if(ui->editSigStop->text() != "")
                    {
                        Temp_SearchAxisMoveStruct.inportNum[0] = ui->editSigStop->getCurrentPort();//获取停止通道
                        Temp_SearchAxisMoveStruct.inportNum[1] = ui->editSigStop->getCurrentPort();//获取停止通道
                        Temp_SearchAxisMoveStruct.inporttype[0] = ui->editSigStop->getPortSetState();
                        Temp_SearchAxisMoveStruct.inporttype[1] = ui->editSigStop->getPortSetState();
                        data_Temp[29] = Temp_SearchAxisMoveStruct.inportNum[0];
                        data_Temp[30] = Temp_SearchAxisMoveStruct.inportNum[1];
                        data_Temp[31] = Temp_SearchAxisMoveStruct.inporttype[0];
                        data_Temp[32] = Temp_SearchAxisMoveStruct.inporttype[1];
                        g_InsertProOrder(data_Temp);
                    }
                    Teach_File_List_Refresh();//刷新程序列表
                }
                else if(ui->chboxResponseStop->isChecked())
                {//感应停止
                    data_Temp[5] = C_SEARCH_STOP;
                    g_InsertProOrder(data_Temp);
                    Teach_File_List_Refresh();//刷新程序列表
                }
            }
            else if(ui->stackedWidget_Senior->currentWidget() == ui->Logical_var_page){//逻辑&变量指令
                if(ui->tabWidget_logic->currentWidget() == ui->tab_if){//逻辑&变量指令-如果指令
                    data_Temp[0] = m_OperateProNum;  //程序编号
                    data_Temp[1] = m_OperateProOrder[m_CurrentSelectProOrderList].list;
                    data_Temp[2] = m_OperateProOrder[m_CurrentSelectProOrderList].list>>8;
                    data_Temp[3] = m_OperateProOrder[m_CurrentSelectProOrderList].runOrderNum;
                    data_Temp[4] = m_OperateProOrder[m_CurrentSelectProOrderList].runOrderNum>>8;
                    data_Temp[6] = 0;

                    if(ui->chboxIfCondition1->isChecked())
                    {//选择了 条件1
                        Temp_LogicIfStruct.reqSelectFlag[0] = 0;
                        if(ui->chboxAnd->isChecked())
                        {//选择了 并且
                            Temp_LogicIfStruct.reqSelectFlag[0] = 1;
                        }
                        else if(ui->chboxOr->isChecked())
                        {//选择了 或者
                            Temp_LogicIfStruct.reqSelectFlag[0] = 2;
                        }
                        data_Temp[5] = C_LOGIC_IF;
                        data_Temp[9] = Temp_LogicIfStruct.sufferCmpValue[0];
                        data_Temp[10] = Temp_LogicIfStruct.sufferCmpValue[0]>>8;
                        data_Temp[11] = Temp_LogicIfStruct.sufferCmpValue[0]>>16;
                        data_Temp[12] = Temp_LogicIfStruct.sufferCmpValue[0]>>24;
                        data_Temp[13] = Temp_LogicIfStruct.sufferCmpValue[1];
                        data_Temp[14] = Temp_LogicIfStruct.sufferCmpValue[1]>>8;
                        data_Temp[15] = Temp_LogicIfStruct.sufferCmpValue[1]>>16;
                        data_Temp[16] = Temp_LogicIfStruct.sufferCmpValue[1]>>24;
                        data_Temp[17] = Temp_LogicIfStruct.reqSelectFlag[0];
                        data_Temp[18] = Temp_LogicIfStruct.reqSelectFlag[1];
                        data_Temp[19] = Temp_LogicIfStruct.cmpType[0];
                        data_Temp[20] = Temp_LogicIfStruct.cmpType[1];
                        data_Temp[21] = Temp_LogicIfStruct.inportNum[0];
                        data_Temp[22] = Temp_LogicIfStruct.inportNum[1];
                        data_Temp[23] = Temp_LogicIfStruct.inportType[0];
                        data_Temp[24] = Temp_LogicIfStruct.inportType[1];
                        data_Temp[25] = Temp_LogicIfStruct.cmpMode[0];
                        data_Temp[26] = Temp_LogicIfStruct.cmpMode[1];
                        data_Temp[27] = Temp_LogicIfStruct.sufferCmpType[0];
                        data_Temp[28] = Temp_LogicIfStruct.sufferCmpType[1];
                        if(ui->editIfVar1Set->text()!="" && Temp_LogicIfStruct.reqSelectFlag[0] == 0)
                        {
                            g_InsertProOrder(data_Temp);
                        }
                        else if(ui->editIfVar1Set->text()!="" && ui->editIfVar2Set->text()!="" && Temp_LogicIfStruct.reqSelectFlag[0] != 0)
                        {
                            g_InsertProOrder(data_Temp);
                        }
                    }
                    else if(ui->chboxElseCondition1->isChecked())
                    {//如果选择-否则
                        data_Temp[5] = C_LOGIC_ELSE;
                        data_Temp[9] = 0;
                        data_Temp[10] =0;
                        data_Temp[11] =0;
                        data_Temp[12] =0;
                        g_InsertProOrder(data_Temp);
                    }
                    else if(ui->chboxEndIf->isChecked())
                    {//结束如果
                        data_Temp[5] = C_LOGIC_END;
                        data_Temp[9] = 0;
                        data_Temp[10] =0;
                        data_Temp[11] =0;
                        data_Temp[12] =0;
                        g_InsertProOrder(data_Temp);
                    }
                    Teach_File_List_Refresh();//刷新程序列表
                }
                else if(ui->tabWidget_logic->currentWidget() == ui->tab_Circulate){//逻辑&变量指令-循环指令
                    data_Temp[0] = m_OperateProNum;  //程序编号
                    data_Temp[1] = m_OperateProOrder[m_CurrentSelectProOrderList].list;
                    data_Temp[2] = m_OperateProOrder[m_CurrentSelectProOrderList].list>>8;
                    data_Temp[3] = m_OperateProOrder[m_CurrentSelectProOrderList].runOrderNum;
                    data_Temp[4] = m_OperateProOrder[m_CurrentSelectProOrderList].runOrderNum>>8;
                    data_Temp[6] = 0;
                    if(ui->chboxLoopStart->isChecked())
                    {//选中循环开始
                        Temp_LogicWhileStartStruct.cycNum = ui->editLoopNum->text().toUInt();
                        data_Temp[5] = C_LOGIC_WHILE_START;
                        data_Temp[9] = Temp_LogicWhileStartStruct.cycNum;
                        data_Temp[10] = Temp_LogicWhileStartStruct.cycNum>>8;
                        data_Temp[11] = Temp_LogicWhileStartStruct.cycNum>>16;
                        data_Temp[12] = Temp_LogicWhileStartStruct.cycNum>>24;
                        g_InsertProOrder(data_Temp);
                    }
                    else if(ui->chboxLoopEnd->isChecked())
                    {//循环结束
                        data_Temp[5] = C_LOGIC_WHILE_END;
                        g_InsertProOrder(data_Temp);
                    }
                    Teach_File_List_Refresh();//刷新程序列表
                }
                else if(ui->tabWidget_logic->currentWidget() == ui->tab_variable){//逻辑&变量指令-变量界面
                    data_Temp[0] = m_OperateProNum;  //程序编号
                    data_Temp[1] = m_OperateProOrder[m_CurrentSelectProOrderList].list;
                    data_Temp[2] = m_OperateProOrder[m_CurrentSelectProOrderList].list>>8;
                    data_Temp[3] = m_OperateProOrder[m_CurrentSelectProOrderList].runOrderNum;
                    data_Temp[4] = m_OperateProOrder[m_CurrentSelectProOrderList].runOrderNum>>8;
                    data_Temp[6] = 0;
                    Temp_LogicVarStruct.operMode = ui->coboxVarOperand->currentIndex(); // + - * / ==
                    Temp_LogicAxisStruct.operMode = ui->coboxVarOperand->currentIndex(); // + - * / ==
                    Temp_LogicStackStruct.operMode  = ui->coboxVarOperand->currentIndex(); // + - * / ==
                    Temp_LogicCurProductNumStruct.operMode  = ui->coboxVarOperand->currentIndex(); // + - * / ==
                    if(ui->chboxVarSelectVarPreOp->isChecked())
                    {//选择变量
                        data_Temp[5] = C_LOGIC_VAR;
                        Temp_LogicVarStruct.varNum = ui->coboxVarSelectVarPreOp->currentIndex()+1;//变量编号1-16
                        if(ui->chboxConstantVarOp->isChecked())
                        {//操作数类型-常量
                            Temp_LogicVarStruct.sufferOperType = 0;
                            int32_t sufferOperValue = 0;
                            if(m_VariableType[ui->coboxVarSelectVarPreOp->currentIndex()]==0)
                            {
                                sufferOperValue = ui->editConstantVarPostOp->text().toInt();
                            }
                            else if(m_VariableType[ui->coboxVarSelectVarPreOp->currentIndex()]==1)
                            {
                                sufferOperValue = ui->editConstantVarPostOp->text().toDouble()*10;
                            }
                            else if(m_VariableType[ui->coboxVarSelectVarPreOp->currentIndex()]==2)
                            {
                                sufferOperValue = ui->editConstantVarPostOp->text().toDouble()*100;
                            }
                            Temp_LogicVarStruct.sufferOperValue = sufferOperValue;//ui->editConstantVarPostOp->text().toDouble();
                        }
                        else if(ui->chboxVariableVarOp->isChecked())
                        {//操作数类型-变量
                            Temp_LogicVarStruct.sufferOperType = ui->coboxVarSelectPostOp->currentIndex()+1;
                        }
                        else if(ui->chboxAxisVarOp->isChecked())
                        {//操作数类型-轴
                            int i=0;
                            for(i=0;i<AXIS_TOTAL_NUM;i++)
                            {
                                if(ui->coboxAxisSelectVarPostOp->currentText() == m_NameDefine[1].axisName[i])
                                {
                                    break;
                                }
                            }
                            Temp_LogicVarStruct.sufferOperType = i+21;
                        }
                        else if(ui->chboxProdNumVarOp->isChecked())
                        {//操作数类型-实际产量
                            Temp_LogicVarStruct.sufferOperType = 101;
                        }
                        data_Temp[9] = Temp_LogicVarStruct.sufferOperValue;
                        data_Temp[10] = Temp_LogicVarStruct.sufferOperValue>>8;
                        data_Temp[11] = Temp_LogicVarStruct.sufferOperValue>>16;
                        data_Temp[12] = Temp_LogicVarStruct.sufferOperValue>>24;
                        data_Temp[13] = Temp_LogicVarStruct.varNum;
                        data_Temp[14] = Temp_LogicVarStruct.operMode;
                        data_Temp[15] = Temp_LogicVarStruct.sufferOperType;
                        g_InsertProOrder(data_Temp);
                    }
                    else if(ui->chboxAxisSelectVarPreOp->isChecked())
                    {//选择轴
                        Temp_LogicAxisStruct.axisNum = 0;
                        data_Temp[5] = C_LOGIC_AXIS;
                        for(int i=0;i<AXIS_TOTAL_NUM;i++)
                        {
                            if(ui->coboxAxisSelectVarPreOp->currentText() == m_NameDefine[1].axisName[i])
                            {
                                Temp_LogicAxisStruct.axisNum = i+1;
                            }
                        }
                        if(ui->chboxConstantVarOp->isChecked())
                        {//操作数类型-常量
                            Temp_LogicAxisStruct.sufferOperType = 0;
                            Temp_LogicAxisStruct.sufferOperValue = ui->editConstantVarPostOp->text().toDouble()*100;
                        }
                        else if(ui->chboxVariableVarOp->isChecked())
                        {//操作数类型-变量
                            Temp_LogicAxisStruct.sufferOperType = ui->coboxVarSelectPostOp->currentIndex()+1;
                        }
                        data_Temp[9] = Temp_LogicAxisStruct.sufferOperValue;
                        data_Temp[10] = Temp_LogicAxisStruct.sufferOperValue>>8;
                        data_Temp[11] = Temp_LogicAxisStruct.sufferOperValue>>16;
                        data_Temp[12] = Temp_LogicAxisStruct.sufferOperValue>>24;
                        data_Temp[13] = Temp_LogicAxisStruct.axisNum;
                        data_Temp[14] = Temp_LogicAxisStruct.operMode;
                        data_Temp[15] = Temp_LogicAxisStruct.sufferOperType;
                        g_InsertProOrder(data_Temp);
                    }
                    else if(ui->chboxStackSelectVarPreOp->isChecked())
                    {//选择堆叠
                        data_Temp[5] = C_LOGIC_STACK;
                        Temp_LogicStackStruct.stackNum = ui->coboxStackSelectVarPreOp->currentIndex()+1;//堆叠组号，1-8
                        if(ui->chboxConstantVarOp->isChecked())
                        {//操作数类型-常量
                            Temp_LogicStackStruct.sufferOperType = 0;
                            Temp_LogicStackStruct.sufferOperValue = ui->editConstantVarPostOp->text().toDouble();
                        }
                        else if(ui->chboxStackCntVarOp->isChecked())
                        {//操作数类型-堆叠计数
                            Temp_LogicStackStruct.sufferOperType = ui->coboxStackVarPostOp->currentIndex()+1;//堆叠计数1-20
                        }
                        data_Temp[9] = Temp_LogicStackStruct.sufferOperValue;
                        data_Temp[10] = Temp_LogicStackStruct.sufferOperValue>>8;
                        data_Temp[11] = Temp_LogicStackStruct.sufferOperValue>>16;
                        data_Temp[12] = Temp_LogicStackStruct.sufferOperValue>>24;
                        data_Temp[13] = Temp_LogicStackStruct.stackNum;
                        data_Temp[14] = Temp_LogicStackStruct.operMode;
                        data_Temp[15] = Temp_LogicStackStruct.sufferOperType;
                        g_InsertProOrder(data_Temp);
                    }
                    else if(ui->chboxRealProdNumPreOp->isChecked())
                    {//实际产量
                        data_Temp[5] = C_LOGIC_PRODUCT;
                        Temp_LogicCurProductNumStruct.productNum = 1;//实际产量号
                        if(ui->chboxConstantVarOp->isChecked())
                        {//操作数类型-常量
                            Temp_LogicCurProductNumStruct.sufferOperType = 0;
                            Temp_LogicCurProductNumStruct.sufferOperValue = ui->editConstantVarPostOp->text().toInt();
                        }
                        else if(ui->chboxVariableVarOp->isChecked())
                        {//操作数类型-变量
                            Temp_LogicCurProductNumStruct.sufferOperType = ui->coboxVarSelectPostOp->currentIndex()+1;
                        }
                        data_Temp[9] = Temp_LogicCurProductNumStruct.sufferOperValue;
                        data_Temp[10] = Temp_LogicCurProductNumStruct.sufferOperValue>>8;
                        data_Temp[11] = Temp_LogicCurProductNumStruct.sufferOperValue>>16;
                        data_Temp[12] = Temp_LogicCurProductNumStruct.sufferOperValue>>24;
                        data_Temp[13] = Temp_LogicCurProductNumStruct.productNum;
                        data_Temp[14] = Temp_LogicCurProductNumStruct.operMode;
                        data_Temp[15] = Temp_LogicCurProductNumStruct.sufferOperType;
                        g_InsertProOrder(data_Temp);
                    }
                    Teach_File_List_Refresh();//刷新程序列表
                }
                else if(ui->tabWidget_logic->currentWidget() == ui->tab_timer){//逻辑&变量指令-定时器界面
                    data_Temp[0] = m_OperateProNum;  //程序编号
                    data_Temp[1] = m_OperateProOrder[m_CurrentSelectProOrderList].list;
                    data_Temp[2] = m_OperateProOrder[m_CurrentSelectProOrderList].list>>8;
                    data_Temp[3] = m_OperateProOrder[m_CurrentSelectProOrderList].runOrderNum;
                    data_Temp[4] = m_OperateProOrder[m_CurrentSelectProOrderList].runOrderNum>>8;
                    data_Temp[6] = 0;
                    data_Temp[5] = C_LOGIC_TIME;
                    Temp_LogicTimeStruct.timeNum = ui->coboxTimerSelect->currentIndex()+1;
                    if(ui->chboxTimerStart->isChecked())
                    {//定时开始
                        Temp_LogicTimeStruct.operMode = 0;
                        data_Temp[9] = Temp_LogicTimeStruct.timeNum;
                        data_Temp[10] = Temp_LogicTimeStruct.operMode;
                        g_InsertProOrder(data_Temp);
                    }
                    else if(ui->chboxTimerStop->isChecked())
                    {//定时结束
                        Temp_LogicTimeStruct.operMode = 1;
                        data_Temp[9] = Temp_LogicTimeStruct.timeNum;
                        data_Temp[10] = Temp_LogicTimeStruct.operMode;
                        g_InsertProOrder(data_Temp);
                    }
                    else if(ui->chboxTimerClear->isChecked())
                    {//定时清零
                        Temp_LogicTimeStruct.operMode = 2;
                        data_Temp[9] = Temp_LogicTimeStruct.timeNum;
                        data_Temp[10] = Temp_LogicTimeStruct.operMode;
                        g_InsertProOrder(data_Temp);
                    }
                    Teach_File_List_Refresh();//刷新程序列表
                }
            }
            else if(ui->stackedWidget_Senior->currentWidget() == ui->label_page)
            {//标签界面
                data_Temp[0] = m_OperateProNum;  //程序编号
                data_Temp[1] = m_OperateProOrder[m_CurrentSelectProOrderList].list;
                data_Temp[2] = m_OperateProOrder[m_CurrentSelectProOrderList].list>>8;
                data_Temp[3] = m_OperateProOrder[m_CurrentSelectProOrderList].runOrderNum;
                data_Temp[4] = m_OperateProOrder[m_CurrentSelectProOrderList].runOrderNum>>8;
                data_Temp[6] = 0;
                data_Temp[5] = C_LABEL;
                if(ui->chboxComment->isChecked())
                {
                    uint16_t LableIndex = getLabelOrderIndex();
                    if(LableIndex>0)
                    {
                        QString chboxCommentName = "";//标签插入复选框
                        QString Lable_Name = "";//插入标签名称
                        if(m_OperateProNum == 0)
                        {
                            chboxCommentName = "标签"+QString::number(LableIndex);
                            ui->chboxComment->setText(chboxCommentName);
                        }
                        else
                        {
                            chboxCommentName = "子"+QString::number(m_OperateProNum)+"标签"+QString::number(LableIndex);
                            ui->chboxComment->setText(chboxCommentName);
                        }
                        Lable_Name = chboxCommentName+" "+ui->txt_label->text();
                        CurrentLableNameList.append(QString(Lable_Name));

                        QListWidgetItem *item = new QListWidgetItem();
                        QCheckBox *checkbox = new QCheckBox();
                        checkbox->setChecked(false);
                        checkbox->setAutoExclusive(true);//check为互斥
                        checkbox->setText(Lable_Name);
                        checkbox->setFixedSize(ui->listWgtJumpto->width(),37);
                        ui->listWgtJumpto->addItem(item);
                        ui->listWgtJumpto->setItemWidget(item,checkbox);

                        connect(checkbox, &QCheckBox::toggled, this, [this, checkbox](bool checked){
                                        onCheckBoxToggled(checkbox, checked);
                                    });

                        data_Temp[9] = LableIndex;
                        data_Temp[10] = LableIndex>>8;
                        data_Temp[11] = 0;//插入一个新标签
                        g_InsertProOrder(data_Temp);
                    }
                }
                else if(listWgtJumptoLabelIndex > 0)
                {
                    data_Temp[9] = listWgtJumptoLabelIndex;
                    data_Temp[10] = listWgtJumptoLabelIndex>>8;
                    data_Temp[11] = 1;//插入一个跳转标签指令
                    g_InsertProOrder(data_Temp);
                }
                Teach_File_List_Refresh();//刷新程序列表
            }
        }
        Teach_timer->start();
    }
    else
    {
        m_CurrentSelectProOrderList =0;
        ui->tableWgtTeach->selectRow(m_CurrentSelectProOrderList);
    }

}
//主界面切换指令提示保存处理
void Teach::widgetSwitchOrderSaveHandel(bool SaveFlag)
{
    if(SaveFlag == true)
    {
        OrderSaveHandel();
    }
    else
    {
        m_OperateProOrderListNum = m_ProInfo.proNum[m_OperateProNum];
        g_TotalProCopy(m_OperateProOrder,m_ProOrder[m_OperateProNum]);
        CurrentLableNameList = LableNameList[m_OperateProNum];
        saveProgram(m_CurrentProgramNameAndPath);
        if(SufferOperNeedRefreash == true)
        {//变量类型发生改变时，下发更新变量类型
            std::copy(std::begin(m_VariableTypeLod),std::end(m_VariableTypeLod),std::begin(m_VariableType));//将老的变量类型赋值给新的
            SufferOperNeedRefreash = false;
        }
        g_Usart->ExtendSendProDeal(CMD_MAIN_PRO,CMD_SUN_PRO_INSERT,m_OperateProNum,m_CurrentSelectProOrderList,1);
        g_Usart->ExtendSendProDeal(CMD_MAIN_PRO,CMD_SUN_PRO_INFO);
        g_Usart->ExtendSendProDeal(CMD_MAIN_PRO,CMD_SUN_PRO_SAVE,m_OperateProNum,1);
        Teach_File_List_Refresh();//刷新程序列表
        OrderNeedSaveFlag = false;
    }
}
//指令保存处理函数
void Teach::OrderSaveHandel()
{
    if(m_OperateProOrderListNum == 0)
    {//如果未载入程序，直接返回
        return;
    }
    if(ui->tabWidget_Teach->currentWidget() == ui->tabMainPro)
    {
        if (ui->stkWgtProgram->currentWidget() == ui->pageAxisAction)
        {

            Edit_AxisMove_Save_handle();

        }
        else if(ui->stkWgtProgram->currentWidget() == ui->pageClawAction)
        {
            if(m_OperateProOrder[m_CurrentSelectProOrderList].cmd == C_CLAW_ACTION)
            {
                Edit_ClawAction_handle();
            }
        }
        else if(ui->stkWgtProgram->currentWidget() == ui->pageReserveOut)
        {//预留输出指令编辑保存
            if(m_OperateProOrder[m_CurrentSelectProOrderList].cmd == C_RESERVE_OUT)
            {
                P_ReserveOutStruct* ReserveOut =(P_ReserveOutStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
                m_OperateProOrder[m_CurrentSelectProOrderList].delay = ui->lineEdit_Edit_ReserveOut_time->text().toDouble()*100;
                ReserveOut->interval = ui->lineEdit_Edit_ReserveOut_Num->text().toUInt();
            }
        }
        else if(ui->stkWgtProgram->currentWidget() == ui->pageSigWait)
        {//等待机床信号/等待卡爪指令编辑保存处理
            if(m_OperateProOrder[m_CurrentSelectProOrderList].cmd == C_WAIT_IN_MACHINE)
            {
                P_WaitInMachineStruct* WaitInMachine =(P_WaitInMachineStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
                m_OperateProOrder[m_CurrentSelectProOrderList].delay = ui->lineEdit_Edit_Wait_time->text().toDouble()*100;
                if(ui->checkBox_Edit_Wait_step->isChecked())
                {
                    WaitInMachine->type = 0;
                    WaitInMachine->backListNum = ui->lineEdit_Edit_Wait_step->text().toUInt();
                }
                else if(ui->checkBox_Edit_Wait_label->isChecked())
                {
                    WaitInMachine->type = 1;
                    WaitInMachine->label = ui->comboBox_Edit_Wait_label->currentIndex();
                }
            }
            else if(m_OperateProOrder[m_CurrentSelectProOrderList].cmd == C_WAIT_IN_CLAW)
            {
                P_WaitInClawStruct* WaitInClaw =(P_WaitInClawStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
                m_OperateProOrder[m_CurrentSelectProOrderList].delay = ui->lineEdit_Edit_Wait_time->text().toDouble()*100;
                if(ui->checkBox_Edit_Wait_step->isChecked())
                {
                    WaitInClaw->type = 0;
                    WaitInClaw->backListNum = ui->lineEdit_Edit_Wait_step->text().toUInt();
                }
                else if(ui->checkBox_Edit_Wait_label->isChecked())
                {
                    WaitInClaw->type = 1;
                    WaitInClaw->label = ui->comboBox_Edit_Wait_label->currentIndex();
                }
            }
        }
        else if(ui->stkWgtProgram->currentWidget() == ui->pageSigWait_2)
        {//等待预留指令编辑保存处理
            if(m_OperateProOrder[m_CurrentSelectProOrderList].cmd == C_WAIT_IN_RESERVE)
            {
                P_WaitInReserveStruct* WaitInReserve =(P_WaitInReserveStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
                m_OperateProOrder[m_CurrentSelectProOrderList].delay = ui->lineEdit_Edit_Wait_time_2->text().toDouble()*100;
                if(ui->checkBox_Edit_Wait_step_2->isChecked())
                {
                    WaitInReserve->type = 0;
                    WaitInReserve->backListNum = ui->lineEdit_Edit_Wait_step_2->text().toUInt();
                }
                else if(ui->checkBox_Edit_Wait_label_2->isChecked())
                {
                    WaitInReserve->type = 1;
                    WaitInReserve->label = ui->comboBox_Edit_Wait_label_2->currentIndex();
                }
            }

        }
        else if(ui->stkWgtProgram->currentWidget() == ui->pageAlarmDef)
        {//自定义报警编辑保存处理
            if(m_OperateProOrder[m_CurrentSelectProOrderList].cmd == C_OTHER_ALARM_CUST)
            {
                P_OtherAlarmCustStruct* OtherAlarmCust =(P_OtherAlarmCustStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
                if(ui->checkBox_fault_num->isChecked())
                {
                    OtherAlarmCust->type = 4;
                    OtherAlarmCust->alarmNum = Temp_OtherAlarmCustStruct.alarmNum;
                }
                else if(ui->checkBox_tips_num->isChecked())
                {
                    OtherAlarmCust->type = 5;
                    OtherAlarmCust->alarmNum = Temp_OtherAlarmCustStruct.alarmNum;
                }
            }
        }
        else if(ui->stkWgtProgram->currentWidget() == ui->pageSubProgEdit)
        {//子程序命令编辑保存处理
            if(m_OperateProOrder[m_CurrentSelectProOrderList].cmd == C_SUN_PRO)
            {
                P_SunProStruct* SunPro =(P_SunProStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
                SunPro->sunProNum = ui->coboxSubProgSelectEdit->currentIndex()+1;
            }
        }
        else if(ui->stkWgtProgram->currentWidget() == ui->pageTorqueProtect)
        {//转矩保护指令编辑保存处理
            if(m_OperateProOrder[m_CurrentSelectProOrderList].cmd == C_TORQUE_GARD)
            {
                P_TorqueGardStruct* TorqueGard =(P_TorqueGardStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
                TorqueGard->torqueValue = ui->lineEdit_Edit_torque_value->text().toUInt();
                for(int i=0;i<AXIS_TOTAL_NUM;i++)
                {
                    if(ui->comboBox_Edit_torque_axis->currentText() == m_NameDefine[1].axisName[i] && m_AxisPar[i].axisType == 1)
                    {
                        TorqueGard->axis = i;
                    }
                }
            }
        }
        else if(ui->stkWgtProgram->currentWidget() == ui->pageOffset)
        {//偏移移-轴偏移运动命令编辑保存处理
            if(m_OperateProOrder[m_CurrentSelectProOrderList].cmd == C_OFFSET_AXIS)
            {
                P_OffsetAxisStruct* OffsetAxis =(P_OffsetAxisStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
                OffsetAxis->offsetPos = ui->lineEdit_Edit_offsetpos->text().toDouble()*100;
                OffsetAxis->advEndDis = ui->lineEdit_Edit_offset_advenddis->text().toDouble()*100;
                OffsetAxis->advCSpeedDis = ui->lineEdit_Edit_offset_advenddis->text().toDouble()*100;
                OffsetAxis->speed = ui->lineEdit_edit_offsetspeed->text().toUInt();
                OffsetAxis->advCSpeedSpeed = ui->lineEdit_Edit_offset_advCspeed->text().toUInt();
                m_OperateProOrder[m_CurrentSelectProOrderList].delay = ui->lineEdit_Edit_offsetDelay->text().toDouble()*100;
                if(ui->Edit_offset_advendflag->isChecked()){
                    OffsetAxis->advEndFlag = 1;
                }
                else {
                    OffsetAxis->advEndFlag = 0;
                }
                if(ui->Edit_offset_acc->isChecked()){
                    OffsetAxis->advCSpeedFlag = 1;
                }
                else if(ui->Edit_offset_down_speed->isChecked()){
                    OffsetAxis->advCSpeedFlag = 2;
                }
                else{
                    OffsetAxis->advCSpeedFlag = 0;
                }
            }
        }
        else if(ui->stkWgtProgram->currentWidget() == ui->pageSearch)
        {//轴搜索指令编辑保存处理
            if(m_OperateProOrder[m_CurrentSelectProOrderList].cmd == C_SEARCH_AXIS_MOVE)
            {
                P_SearchAxisMoveStruct* SearchAxisMove =(P_SearchAxisMoveStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
                if(ui->checkBox_PosMemory->isChecked()){
                    SearchAxisMove->posStoreFlag = 1;
                }
                else{
                    SearchAxisMove->posStoreFlag = 0;
                }
                if(ui->checkBox_Edit_Arrive_alarm->isChecked()){
                    SearchAxisMove->reachPosAlarmFlag = 1;
                }
                else{
                    SearchAxisMove->reachPosAlarmFlag = 0;
                }
                m_OperateProOrder[m_CurrentSelectProOrderList].delay = ui->lineEdit_Search_delay->text().toDouble()*100;
                SearchAxisMove->maxPos = (int32_t)ui->lineEdit_Search_Mix_Pos->text().toDouble()*100;
                SearchAxisMove->runSpeed = ui->lineEdit_Search_runspeed->text().toUInt();
                SearchAxisMove->advCDis = (int32_t)ui->lineEdit_Search_AdvPos->text().toDouble()*100;
                SearchAxisMove->advCSpeed = ui->lineEdit_Search_Adv_speed->text().toUInt();
                SearchAxisMove->searchNum = ui->coboxSearchNum->currentIndex()+1;//1-8编号
                SearchAxisMove->offsetDis = (int32_t)ui->lineEdit_Search_offseet_pos->text().toDouble()*100;
                SearchAxisMove->inportNum[0] = ui->lineEdit_Search_stop->getCurrentPort();
            }

        }
        else if(ui->stkWgtProgram->currentWidget() == ui->pageIf)
        {//如果指令编辑保存处理
            if(m_OperateProOrder[m_CurrentSelectProOrderList].cmd == C_LOGIC_IF)
            {
                P_LogicIfStruct* LogicIf =(P_LogicIfStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
                if(ui->chboxCondition1If->isChecked())
                {
                    Edit_ifOrder_Save_handle(LogicIf,0);
                }
                else if(ui->chboxCondition2If->isChecked())
                {
                    Edit_ifOrder_Save_handle(LogicIf,1);
                }
            }

        }
        else if(ui->stkWgtProgram->currentWidget() == ui->pageVarOperation)
        {//变量指令编辑保存处理函数

            if(ui->Var_page->currentWidget() == ui->Var_Var_page){//变量-变量
                if(m_OperateProOrder[m_CurrentSelectProOrderList].cmd == C_LOGIC_VAR)
                {
                    P_LogicVarStruct* LogicVar =(P_LogicVarStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;

                    if(ui->Edit_Var_const->isChecked())
                    {
                        LogicVar->operMode = ui->comboBox_Edit_Var_Oper->currentIndex();
                        LogicVar->sufferOperType = 0;
                        int32_t sufferOperValue = 0;
                        if(m_VariableType[ui->comboBox_Edit_Var->currentIndex()]==0)
                        {
                            sufferOperValue = ui->lineEdit_Edit_Var_const->text().toInt();
                        }
                        else if(m_VariableType[ui->comboBox_Edit_Var->currentIndex()]==1)
                        {
                            sufferOperValue = ui->lineEdit_Edit_Var_const->text().toDouble()*10;
                        }
                        else if(m_VariableType[ui->comboBox_Edit_Var->currentIndex()]==2)
                        {
                            sufferOperValue = ui->lineEdit_Edit_Var_const->text().toDouble()*100;
                        }
                        LogicVar->sufferOperValue = sufferOperValue;
                    }
                    else if(ui->Edit_Var_var->isChecked())
                    {
                        LogicVar->operMode = 4;
                        LogicVar->sufferOperType = ui->comboBox_Edit_Var_Var->currentIndex()+1;
                    }
                    else if(ui->Edit_Var_Axis->isChecked())
                    {
                        LogicVar->operMode = 4;
                        LogicVar->sufferOperType = ui->comboBox_Edit_Var_Var->currentIndex()+21;
                    }
                    else if(ui->Edit_Var_Prod->isChecked())
                    {
                        LogicVar->operMode = 4;
                        LogicVar->sufferOperType = 101;
                    }
                }

            }
            else if(ui->Var_page->currentWidget() == ui->Var_Axis_page){//变量-轴
                if(m_OperateProOrder[m_CurrentSelectProOrderList].cmd == C_LOGIC_AXIS)
                {
                    P_LogicAxisStruct* LogicAxis =(P_LogicAxisStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
                    if(ui->Edit_Axis_const->isChecked())
                    {
                        LogicAxis->operMode = ui->comboBox_Edit_Axis_Oper->currentIndex();
                        LogicAxis->sufferOperType = 0;
                        LogicAxis->sufferOperValue = ui->lineEdit_Edit_Axis_const->text().toDouble()*100;
                    }
                    else if(ui->Edit_Axis_var->isChecked())
                    {
                        LogicAxis->operMode = 4;
                        LogicAxis->sufferOperType = ui->comboBox_Edit_Axis_Var->currentIndex()+1;
                    }
                }

            }
            else if(ui->Var_page->currentWidget() == ui->Var_Stack_page){//变量-堆叠
                if(m_OperateProOrder[m_CurrentSelectProOrderList].cmd == C_LOGIC_STACK)
                {
                    P_LogicStackStruct* LogicStack =(P_LogicStackStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
                    if(ui->Edit_Stack_const->isChecked())
                    {
                        LogicStack->sufferOperType = 0;
                        LogicStack->operMode = ui->comboBox_Edit_Stack_Oper->currentIndex();
                        LogicStack->sufferOperValue = ui->lineEdit_Edit_Stack_const->text().toUInt();
                    }
                    else if(ui->Edit_Stack_num->isChecked())
                    {
                        LogicStack->sufferOperType = ui->comboBox_Edit_Stack_num1->currentIndex()+1;
                        LogicStack->operMode = 4;
                    }
                }

            }
            else if(ui->Var_page->currentWidget() == ui->Var_Prod_page){//变量-实际产量
                if(m_OperateProOrder[m_CurrentSelectProOrderList].cmd == C_LOGIC_PRODUCT)
                {
                    P_LogicCurProductNumStruct* LogicCurProductNum =(P_LogicCurProductNumStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
                    if(ui->Edit_Prod_const->isChecked())
                    {
                        LogicCurProductNum->sufferOperType = 0;
                        LogicCurProductNum->operMode = ui->comboBox_Edit_Prod_Oper->currentIndex();
                        LogicCurProductNum->sufferOperValue = ui->lineEdit_Edit_Prod_const->text().toUInt();
                    }
                    else if(ui->Edit_Prod_var->isChecked())
                    {
                        LogicCurProductNum->sufferOperType = ui->comboBox_Edit_Prod_Var->currentIndex()+1;
                        LogicCurProductNum->operMode = 4;
                    }

                }

            }
        }
        else if(ui->stkWgtProgram->currentWidget() == ui->pageLoop)
        {//循环开始指令编辑保存处理
            if(m_OperateProOrder[m_CurrentSelectProOrderList].cmd == C_LOGIC_WHILE_START)
            {
                P_LogicWhileStartStruct* LogicWhileStart =(P_LogicWhileStartStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
                LogicWhileStart->cycNum = ui->lineEdit_Loop_num->text().toUInt();
            }

        }
        else if(ui->stkWgtProgram->currentWidget() == ui->pageLabelEdit)
        {//标签编辑界面
            if(m_OperateProOrder[m_CurrentSelectProOrderList].cmd == C_LABEL)
            {
                P_LabelStruct* LabelStruct =(P_LabelStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
                if(LabelStruct->type == 0)
                {//替换标签内容
                    // 找到第一个空格的位置
                    uint16_t LabelIndex = ReturnLableListIndex(LabelStruct->labelNum);
                    if(LabelIndex>0)
                    {
                        int spaceIndex = CurrentLableNameList[LabelIndex].indexOf(' ');
                        if (spaceIndex != -1) { // 如果找到了空格
                            // 替换空格之后的内容
                            QString revise_Text = ui->label_Edit->text();
                            QString clearedText = CurrentLableNameList[LabelIndex].left(spaceIndex + 1); // 保留包括空格在内的左侧部分
                            CurrentLableNameList[LabelIndex]=clearedText+revise_Text;
                        }
                    }
                }
                else if(LabelStruct->type == 1)
                {//跳转到指令
                    LabelStruct->labelNum = listWgtJumptoLabelIndex;
                }
            }

        }
        else if(ui->stkWgtProgram->currentWidget() == ui->pageStack)
        {//堆叠指令编辑界面
            if(m_OperateProOrder[m_CurrentSelectProOrderList].cmd == C_STACK_MOVE || m_OperateProOrder[m_CurrentSelectProOrderList].cmd == C_STACK_FOLLOW || m_OperateProOrder[m_CurrentSelectProOrderList].cmd == C_STACK_RESET_ZERO)
            {
                P_StackMoveStruct* StackMove = (P_StackMoveStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
                m_OperateProOrder[m_CurrentSelectProOrderList].delay = ui->Stack_Edit_Delay->text().toDouble()*100;
                //x1轴参数
                m_StackInfo[StackMove->stackNum-1].stackStartPos[0] = ui->Stack_Edit_Pos->text().toDouble()*100;
                m_StackInfo[StackMove->stackNum-1].axisSpeed[0] = ui->Stack_Edit_Speed->text().toUInt();
                StackMove->advEndFlag[0] = ui->cb_Stack_Edit_end->isChecked();
                if(ui->cb_Stack_Edit_acc->isChecked())
                {
                    StackMove->advCSpeedFlag[0] = 1;
                }
                else if(ui->cb_Stack_Edit_DSpeed->isChecked())
                {
                    StackMove->advCSpeedFlag[0] = 2;
                }
                else
                {
                    StackMove->advCSpeedFlag[0] = 0;
                }
                StackMove->advEndDis[0] = ui->Stack_Edit_AdvPos->text().toDouble()*100;
                StackMove->advCSpeedDis[0] = ui->Stack_Edit_AdvPos->text().toDouble()*100;
                StackMove->advCSpeedSpeed[0] = ui->Stack_Edit_AdvSpeed->text().toUInt();
                //z1轴参数
                m_StackInfo[StackMove->stackNum-1].stackStartPos[2] = ui->Stack_Edit_Pos_2->text().toDouble()*100;
                m_StackInfo[StackMove->stackNum-1].axisSpeed[2] = ui->Stack_Edit_Speed_2->text().toUInt();
                StackMove->advEndFlag[2] = ui->cb_Stack_Edit_end_2->isChecked();
                if(ui->cb_Stack_Edit_acc_2->isChecked())
                {
                    StackMove->advCSpeedFlag[2] = 1;
                }
                else if(ui->cb_Stack_Edit_DSpeed_2->isChecked())
                {
                    StackMove->advCSpeedFlag[2] = 2;
                }
                else
                {
                    StackMove->advCSpeedFlag[2] = 0;
                }
                StackMove->advEndDis[2] = ui->Stack_Edit_AdvPos_2->text().toDouble()*100;
                StackMove->advCSpeedDis[2] = ui->Stack_Edit_AdvPos_2->text().toDouble()*100;
                StackMove->advCSpeedSpeed[2] = ui->Stack_Edit_AdvSpeed_2->text().toUInt();
                //Y1轴参数
                m_StackInfo[StackMove->stackNum-1].stackStartPos[1] = ui->Stack_Edit_Pos_3->text().toDouble()*100;
                m_StackInfo[StackMove->stackNum-1].axisSpeed[1] = ui->Stack_Edit_Speed_3->text().toUInt();
                StackMove->advEndFlag[1] = ui->cb_Stack_Edit_end_3->isChecked();
                if(ui->cb_Stack_Edit_acc_3->isChecked())
                {
                    StackMove->advCSpeedFlag[1] = 1;
                }
                else if(ui->cb_Stack_Edit_DSpeed_3->isChecked())
                {
                    StackMove->advCSpeedFlag[1] = 2;
                }
                else
                {
                    StackMove->advCSpeedFlag[1] = 0;
                }
                StackMove->advEndDis[1] = ui->Stack_Edit_AdvPos_3->text().toDouble()*100;
                StackMove->advCSpeedDis[1] = ui->Stack_Edit_AdvPos_3->text().toDouble()*100;
                StackMove->advCSpeedSpeed[1] = ui->Stack_Edit_AdvSpeed_3->text().toUInt();
            }

        }
        else if(ui->stkWgtProgram->currentWidget() == ui->pageDelayTime)
        {//延时指令编辑保存处理
            m_OperateProOrder[m_CurrentSelectProOrderList].delay = ui->lineEdit_OtherDelay->text().toDouble()*100;
        }
    }

    bool isSpeedTeach = true;
    if(ui->tabWidget_Teach->currentWidget() == ui->tabUniversal)
    {//速度教导界面参数保存
        if(ui->stackedWidget_General->currentWidget() == ui->Speed_Educat_page)
        {
            isSpeedTeach = false;
            Save_Speed_Educat();
        }
    }
    if(isSpeedTeach)
    {
        if(SufferOperNeedRefreash == true)
        {//变量类型发生改变时，下发更新变量类型
            SufferOperUpdata_Handel();
            g_Usart->ExtendSendProDeal(CMD_MAIN_PRO,CMD_SUN_STA_VAR_TYPE,1,0);
            SufferOperNeedRefreash = false;
        }
        g_TotalProCopy(m_ProOrder[m_OperateProNum],m_OperateProOrder);
        saveProgram(m_CurrentProgramNameAndPath);
        g_Usart->ExtendSendProDeal(CMD_MAIN_PRO,CMD_SUN_PRO_INSERT,m_OperateProNum,m_CurrentSelectProOrderList,1);
        g_Usart->ExtendSendProDeal(CMD_MAIN_PRO,CMD_SUN_PRO_INFO);
        g_Usart->ExtendSendProDeal(CMD_MAIN_PRO,CMD_SUN_PRO_SAVE,m_OperateProNum,1);

    }
    Teach_File_List_Refresh();//刷新程序列表
    qDebug() << "save Refresh end";
    OrderNeedSaveFlag = false;
}

//变量类型更新函数
void Teach::SufferOperUpdata_Handel()
{
    for(int i=0;i<PRO_NUM;i++)
    {
        if(i!=m_OperateProNum)
        {
            for(int j=0;j<m_ProInfo.proNum[i];j++)
            {
                if(m_ProOrder[i][j].cmd == C_LOGIC_IF)
                {
                    P_LogicIfStruct* LogicIfStruct =(P_LogicIfStruct*)m_ProOrder[i][j].pData;
                    if(LogicIfStruct->reqSelectFlag[0] == 0)
                    {
                        IfOrderSufferOperUpdata(LogicIfStruct,0);
                    }
                    else
                    {
                        IfOrderSufferOperUpdata(LogicIfStruct,0);
                        IfOrderSufferOperUpdata(LogicIfStruct,1);
                    }
                }
                if(m_ProOrder[i][j].cmd == C_LOGIC_VAR)
                {
                    P_LogicVarStruct* LogicVarStruct =(P_LogicVarStruct*)m_ProOrder[i][j].pData;
                    LogicVar_SufferOperUpdata(LogicVarStruct);
                }
            }
        }
        else
        {
            for(int j=0;j<m_ProInfo.proNum[m_OperateProNum];j++)
            {
                if(m_OperateProOrder[j].cmd == C_LOGIC_IF)
                {
                    P_LogicIfStruct* LogicIfStruct =(P_LogicIfStruct*)m_OperateProOrder[j].pData;
                    if(LogicIfStruct->reqSelectFlag[0] == 0)
                    {
                        IfOrderSufferOperUpdata(LogicIfStruct,0);
                    }
                    else
                    {
                        IfOrderSufferOperUpdata(LogicIfStruct,0);
                        IfOrderSufferOperUpdata(LogicIfStruct,1);
                    }
                }
                if(m_OperateProOrder[j].cmd == C_LOGIC_VAR)
                {
                    P_LogicVarStruct* LogicVarStruct =(P_LogicVarStruct*)m_OperateProOrder[j].pData;
                    LogicVar_SufferOperUpdata(LogicVarStruct);
                }
            }
        }
    }
}
//逻辑&变量-变量-变量命令-变量类型发生改变时调用处理函数
void Teach::LogicVar_SufferOperUpdata(P_LogicVarStruct* LogicVarStruct)
{
    uint8_t VariableTypeIndex = LogicVarStruct->varNum-1;
    if(m_VariableTypeLod[VariableTypeIndex] != m_VariableType[VariableTypeIndex])
    {
        if(LogicVarStruct->sufferOperType == 0)
        {
            if(m_VariableType[VariableTypeIndex] == 0)
            {
                if(m_VariableTypeLod[VariableTypeIndex] == 1)
                {
                    LogicVarStruct->sufferOperValue=LogicVarStruct->sufferOperValue/10;
                }
                else if(m_VariableTypeLod[VariableTypeIndex] == 2)
                {
                    LogicVarStruct->sufferOperValue=LogicVarStruct->sufferOperValue/100;
                }
            }
            else if(m_VariableType[VariableTypeIndex] == 1)
            {
                if(m_VariableTypeLod[VariableTypeIndex] == 0)
                {
                    LogicVarStruct->sufferOperValue=LogicVarStruct->sufferOperValue*10;
                }
                else if(m_VariableTypeLod[VariableTypeIndex] == 2)
                {
                    LogicVarStruct->sufferOperValue=LogicVarStruct->sufferOperValue/10;
                }
            }
            else if(m_VariableType[VariableTypeIndex] == 2)
            {
                if(m_VariableTypeLod[VariableTypeIndex] == 0)
                {
                    LogicVarStruct->sufferOperValue=LogicVarStruct->sufferOperValue*100;
                }
                else if(m_VariableTypeLod[VariableTypeIndex] == 1)
                {
                    LogicVarStruct->sufferOperValue=LogicVarStruct->sufferOperValue*10;
                }
            }
        }
    }
}
//if条件指令变量类型修改时调用处理函数
void Teach::IfOrderSufferOperUpdata(P_LogicIfStruct* LogicIfStruct,uint8_t index)
{
    if(LogicIfStruct->cmpType[index]>=1 && LogicIfStruct->cmpType[index]<=20 && LogicIfStruct->sufferCmpType[index]==0)
    {//比较类型为常量
        uint8_t VariableTypeIndex = LogicIfStruct->cmpType[index]-1;
        if(m_VariableTypeLod[VariableTypeIndex] != m_VariableType[VariableTypeIndex])
        {
            if(m_VariableType[VariableTypeIndex] == 0)
            {
                if(m_VariableTypeLod[VariableTypeIndex] == 1)
                {
                    LogicIfStruct->sufferCmpValue[index]=LogicIfStruct->sufferCmpValue[index]/10;
                }
                else if(m_VariableTypeLod[VariableTypeIndex] == 2)
                {
                    LogicIfStruct->sufferCmpValue[index]=LogicIfStruct->sufferCmpValue[index]/100;
                }
            }
            else if(m_VariableType[VariableTypeIndex] == 1)
            {
                if(m_VariableTypeLod[VariableTypeIndex] == 0)
                {
                    LogicIfStruct->sufferCmpValue[index]=LogicIfStruct->sufferCmpValue[index]*10;
                }
                else if(m_VariableTypeLod[VariableTypeIndex] == 2)
                {
                    LogicIfStruct->sufferCmpValue[index]=LogicIfStruct->sufferCmpValue[index]/10;
                }
            }
            else if(m_VariableType[VariableTypeIndex] == 2)
            {
                if(m_VariableTypeLod[VariableTypeIndex] == 0)
                {
                    LogicIfStruct->sufferCmpValue[index]=LogicIfStruct->sufferCmpValue[index]*100;
                }
                else if(m_VariableTypeLod[VariableTypeIndex] == 1)
                {
                    LogicIfStruct->sufferCmpValue[index]=LogicIfStruct->sufferCmpValue[index]*10;
                }
            }
        }
    }
}
//赋值速度教导界面的轴速度
void Teach::Save_Speed_Educat()
{
    QCheckBox* Speed_EducatBoxes[] = {
        ui->chboxSpeedTeachX1,
        ui->chboxSpeedTeachY1,
        ui->chboxSpeedTeachZ1,
        ui->chboxSpeedTeachC,
        ui->chboxSpeedTeachY2,
        ui->chboxSpeedTeachZ2,
    };

    NumberEdit* Speed_EducatEdit[] = {
        ui->editSpeedTeachX1,
        ui->editSpeedTeachY1,
        ui->editSpeedTeachZ1,
        ui->editSpeedTeachC,
        ui->editSpeedTeachY2,
        ui->editSpeedTeachZ2,
    };

    const int count = sizeof(Speed_EducatBoxes) / sizeof(QCheckBox*); // Number of elements in arrays

    for (int i = 0; i < count; ++i)
    {
        if (Speed_EducatBoxes[i]->isChecked())
        {
            for(uint16_t j=0;j<m_OperateProOrderListNum;j++)
            {
                if(m_OperateProOrder[j].cmd == C_AXIS_MOVE)
                {
                    P_AxisMoveStruct* AxisMove = (P_AxisMoveStruct*)m_OperateProOrder[j].pData;
                    if(AxisMove->axis == i && i<=5)
                    {
                        AxisMove->speed = Speed_EducatEdit[i]->text().toUInt();
                    }
                    else if(i>5)
                    {
                        if(AxisMove->axis == 4 && i==6)
                        {
                            AxisMove->speed = Speed_EducatEdit[i]->text().toUInt();
                        }
                        else if(AxisMove->axis == 5 && i==7)
                        {
                            AxisMove->speed = Speed_EducatEdit[i]->text().toUInt();
                        }
                    }
                }
            }
            g_Usart->ExtendSendProDeal(CMD_MAIN_PRO,CMD_SUN_PRO_SPEED,m_OperateProNum,i,Speed_EducatEdit[i]->text().toUInt());
        }
    }
}
/*************************************************************************
**  函数名：  Edit_ifOrder_Save_handle()
**	输入参数：LogicIf:if结构体参数，IfIndex:0-条件1，1-条件2
**	输出参数：
**	函数功能：如果指令保存处理函数
**  作者：    wukui
**  开发日期：2024/9/22
**************************************************************************/
void Teach::Edit_ifOrder_Save_handle(P_LogicIfStruct* LogicIf,uint8_t IfIndex)
{
    if(ui->stkWgtIf->currentWidget() == ui->pageSigIn)
    {//输入信号
        if(ui->chboxMainBoardIn0->isChecked())
        {
            if(ui->btnMainBoardIn0->getState()==0){
                LogicIf->inportType[IfIndex] = 0;
            }
            else if(ui->btnMainBoardIn0->getState()==1){
                LogicIf->inportType[IfIndex] = 1;
            }
            LogicIf->inportNum[IfIndex] = ui->coboxMainBoardIn0->currentIndex()+1;
        }
        else if(ui->chboxMainBoardIn1->isChecked())
        {
            if(ui->btnMainBoardIn1->getState()==0){
                LogicIf->inportType[IfIndex] = 0;
            }
            else if(ui->btnMainBoardIn1->getState()==1){
                LogicIf->inportType[IfIndex] = 1;
            }
            LogicIf->inportNum[IfIndex] = ui->coboxMainBoardIn1->currentIndex()+13;
        }
        else if(ui->chboxMainBoardIn2->isChecked())
        {
            if(ui->btnMainBoardIn2->getState()==0){
                LogicIf->inportType[IfIndex] = 0;
            }
            else if(ui->btnMainBoardIn2->getState()==1){
                LogicIf->inportType[IfIndex] = 1;
            }
            LogicIf->inportNum[IfIndex] = ui->coboxMainBoardIn2->currentIndex()+25;
        }
        else if(ui->chboxMainBoardInExtend0->isChecked())
        {
            if(ui->btnBoardInExtend0->getState()==0){
                LogicIf->inportType[IfIndex] = 0;
            }
            else if(ui->btnBoardInExtend0->getState()==1){
                LogicIf->inportType[IfIndex] = 1;
            }
            LogicIf->inportNum[IfIndex] = ui->coboxMainBoardInExtend0->currentIndex()+37;
        }
        else if(ui->chboxMainBoardInExtend1->isChecked())
        {
            if(ui->btnBoardInExtend1->isChecked()==0){
                LogicIf->inportType[IfIndex] = 0;
            }
            else if(ui->btnBoardInExtend1->isChecked()==1){
                LogicIf->inportType[IfIndex] = 1;
            }
            LogicIf->inportNum[IfIndex] = ui->coboxMainBoardInExtend1->currentIndex()+49;
        }
    }
    else if(ui->stkWgtIf->currentWidget() == ui->pageSigOut)
    {//输出信号
        if(ui->chboxMainBoardOut0->isChecked())
        {
            if(ui->btnMainBoardOut0->getState()==0){
                LogicIf->inportType[IfIndex] = 0;
            }
            else if(ui->btnMainBoardOut0->getState()==1){
                LogicIf->inportType[IfIndex] = 1;
            }
            LogicIf->inportNum[IfIndex] = ui->coboxMainBoardOut0->currentIndex()+1;
        }
        else if(ui->chboxMainBoardOut1->isChecked())
        {
            if(ui->btnMainBoardOut1->getState()==0){
                LogicIf->inportType[IfIndex] = 0;
            }
            else if(ui->btnMainBoardOut1->getState()==1){
                LogicIf->inportType[IfIndex] = 1;
            }
            LogicIf->inportNum[IfIndex] = ui->coboxMainBoardOut1->currentIndex()+9;
        }
        else if(ui->chboxMainBoardOut2->isChecked())
        {
            if(ui->btnMainBoardOut2->getState()==0){
                LogicIf->inportType[IfIndex] = 0;
            }
            else if(ui->btnMainBoardOut2->getState()==1){
                LogicIf->inportType[IfIndex] = 1;
            }
            LogicIf->inportNum[IfIndex] = ui->coboxMainBoardOut2->currentIndex()+17;
        }
        else if(ui->chboxMainBoardOutExtend0->isChecked())
        {
            if(ui->btnBoardOutExtend0->getState()==0){
                LogicIf->inportType[IfIndex] = 0;
            }
            else if(ui->btnBoardOutExtend0->getState()==1){
                LogicIf->inportType[IfIndex] = 1;
            }
            LogicIf->inportNum[IfIndex] = ui->coboxMainBoardOutExtend0->currentIndex()+25;
        }
        else if(ui->chboxMainBoardOutExtend1->isChecked())
        {
            if(ui->btnBoardOutExtend1->getState()==0){
                LogicIf->inportType[IfIndex] = 0;
            }
            else if(ui->btnBoardOutExtend1->getState()==1){
                LogicIf->inportType[IfIndex] = 1;
            }
            LogicIf->inportNum[IfIndex] = ui->coboxMainBoardOutExtend1->currentIndex()+33;
        }
    }
    else if(ui->stkWgtIf->currentWidget() == ui->pageVar)
    {//变量
        LogicIf->cmpMode[IfIndex] = ui->coboxVarOperator->currentIndex();
        if(LogicIf->sufferCmpType[IfIndex] == 0)
        {//被比较对象类型-常量
            int32_t sufferOperValue = 0;
            if(m_VariableType[ui->coboxVarDisplay->currentIndex()]==0)
            {
                sufferOperValue = ui->editVarIf->text().toInt();
            }
            else if(m_VariableType[ui->coboxVarDisplay->currentIndex()]==1)
            {
                sufferOperValue = ui->editVarIf->text().toDouble()*10;
            }
            else if(m_VariableType[ui->coboxVarDisplay->currentIndex()]==2)
            {
                sufferOperValue = ui->editVarIf->text().toDouble()*100;
            }
            LogicIf->sufferCmpValue[IfIndex] = sufferOperValue;
        }
        else if(LogicIf->sufferCmpType[IfIndex] >= 1 && LogicIf->sufferCmpType[IfIndex] <= 20)
        {//被比较对象类型-变量
            LogicIf->sufferCmpType[IfIndex] = ui->coboxVarSelectIf->currentIndex()+1;
        }
        else if(LogicIf->sufferCmpType[IfIndex] >= 102)
        {//被比较对象类型-实际产量
            LogicIf->inportNum[IfIndex] = ui->coboxVarRealProductNum->currentIndex()+102;
        }
    }
    else if(ui->stkWgtIf->currentWidget() == ui->pageOtherIf)
    {//堆叠组方式1/2
        if(LogicIf->sufferCmpType[IfIndex] == 0)
        {
            LogicIf->cmpMode[IfIndex] = ui->coboxOperatorOther->currentIndex();
            LogicIf->sufferCmpValue[IfIndex] = ui->editOther->text().toUInt();
        }
        else if(LogicIf->sufferCmpType[IfIndex]>=21 &&LogicIf->sufferCmpType[IfIndex]<=40)
        {
            LogicIf->cmpMode[IfIndex] = ui->coboxOperatorOther->currentIndex();
            LogicIf->sufferCmpType[IfIndex] = ui->coboxOtherSelect->currentIndex()+21;
        }
        else if(LogicIf->sufferCmpType[IfIndex] == 103)
        {
            LogicIf->cmpMode[IfIndex] = ui->coboxOperatorOther_2->currentIndex()+4;
            LogicIf->sufferCmpType[IfIndex] = 103;
        }
    }
    else if(ui->stkWgtIf->currentWidget() == ui->pageAxisPosIf)
    {//轴
        LogicIf->cmpMode[IfIndex] = ui->coboxOperatorAxisPos->currentIndex();
        if(LogicIf->sufferCmpType[IfIndex] == 0)
        {
            LogicIf->sufferCmpValue[IfIndex] = ui->editAxisPos->text().toDouble()*100;
        }
        else if(LogicIf->sufferCmpType[IfIndex]>=1 && LogicIf->sufferCmpType[IfIndex]<=20)
        {
            LogicIf->sufferCmpType[IfIndex] = ui->coboxVarSelectAxisPos->currentIndex()+1;
        }
        else if(LogicIf->sufferCmpType[IfIndex]>=61 && LogicIf->sufferCmpType[IfIndex]<=80)
        {
            LogicIf->sufferCmpType[IfIndex] = ui->coboxAxisDisplay_2->currentIndex()+61;
        }
    }
    else if(ui->stkWgtIf->currentWidget() == ui->pageTimerIf)
    {//定时器
        LogicIf->cmpMode[IfIndex] = ui->coboxOperatorTimer->currentIndex();
        if(LogicIf->sufferCmpType[IfIndex]==0)
        {
            LogicIf->sufferCmpValue[IfIndex] = ui->editTimernum->text().toUInt();
        }
        else if(LogicIf->sufferCmpType[IfIndex]>=81 && LogicIf->sufferCmpType[IfIndex]<=100)
        {
            LogicIf->sufferCmpType[IfIndex] = ui->coboxTimerDisplayIf_2->currentIndex()+81;
        }
    }
    else if(ui->stkWgtIf->currentWidget() == ui->pageRealProdNum)
    {//实际产量
        LogicIf->cmpMode[IfIndex] = ui->coboxRealProdOperator->currentIndex();
        if(LogicIf->sufferCmpType[IfIndex] == 0)
        {
            LogicIf->sufferCmpValue[IfIndex] = ui->editRealProcIf->text().toInt();
        }
        else if(LogicIf->sufferCmpType[IfIndex] >= 1 && LogicIf->sufferCmpType[IfIndex] <= 20)
        {
            LogicIf->sufferCmpType[IfIndex] = ui->coboxRealProductNum_2->currentIndex()+1;
        }
    }
}

/*************************************************************************
**  函数名：  Edit_AxisMove_Save_handle()
**	输入参数：无
**	输出参数：无
**	函数功能：轴指令编辑参数修改保存处理
**  作者：    wk
**  开发日期：2024/9/6
**************************************************************************/
void Teach::Edit_AxisMove_Save_handle(void)
{
    if(m_OperateProOrder[m_CurrentSelectProOrderList].cmd == C_AXIS_MOVE)
    {
        P_AxisMoveStruct *AxisMove = (P_AxisMoveStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
        AxisMove->pos = ui->lineEdit_Edit_AxisMove_pos->text().toDouble()*100;
        AxisMove->speed = ui->lineEdit_Edit_AxisMove_speed->text().toUInt();
        AxisMove->advEndFlag = ui->cb_Edit_AxisMove_early_End->isChecked();
        AxisMove->advEndDis = ui->lineEdit_Edit_AxisMove_early_pos->text().toDouble()*100;
        AxisMove->advCSpeedDis = ui->lineEdit_Edit_AxisMove_early_pos->text().toDouble()*100;
        AxisMove->advCSpeedSpeed = ui->lineEdit_Edit_AxisMove_early_speed->text().toUInt();
        if(ui->cb_Edit_AxisMove_early_acc_speed->isChecked())
        {
            AxisMove->advCSpeedFlag = 1;
        }
        else if(ui->cb_Edit_AxisMove_early_Slow_speed->isChecked())
        {
            AxisMove->advCSpeedFlag = 2;
        }
        else
        {
            AxisMove->advCSpeedFlag = 0;
        }
        AxisMove->referPointNum = (uint8_t)ui->editReferPoint->getRefPointIndex();
        m_OperateProOrder[m_CurrentSelectProOrderList].delay = ui->lineEdit_Edit_AxisMove_delay->text().toDouble()*100;
    }
}
/*************************************************************************
**  函数名：  Edit_ClawAction_handle()
**	输入参数：无
**	输出参数：无
**	函数功能：卡爪指令编辑参数修改保存处理
**  作者：    wk
**  开发日期：2024/9/6
**************************************************************************/
void Teach::Edit_ClawAction_handle()
{
    P_ClawActionStruct* ClawAction = (P_ClawActionStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
    uint16_t modPortIndex = GetXYPortIndex(m_OperateProOrder[m_CurrentSelectProOrderList]);

    if(ui->ClawClamp->isChecked())
    {
        ClawAction->type = 1;
//        if(modPortIndex == CLAW_METERIAL_1_LOOSENED)
//        {
//            modPortIndex = CLAW_METERIAL_1_CLAMP;
//        }
//        else if(modPortIndex == CLAW_PRODUCT_1_LOOSENED)
//        {
//            modPortIndex = CLAW_PRODUCT_1_CLAMP;
//        }
//        else if(modPortIndex == CLAW_CLAW_1_LOOSENED)
//        {
//            modPortIndex = CLAW_CLAW_1_CLAMP;
//        }
//        ClawAction->outportNum = m_Port_Y[modPortIndex].portNum;//变成夹紧端口
    }
    else if(ui->ClawRelease->isChecked())
    {
        ClawAction->type = 0;
//        if(modPortIndex == CLAW_METERIAL_1_CLAMP)
//        {
//            modPortIndex = CLAW_METERIAL_1_LOOSENED;
//        }
//        else if(modPortIndex == CLAW_PRODUCT_1_CLAMP)
//        {
//            modPortIndex = CLAW_PRODUCT_1_LOOSENED;
//        }
//        else if(modPortIndex == CLAW_CLAW_1_CLAMP)
//        {
//            modPortIndex = CLAW_CLAW_1_LOOSENED;
//        }
//        ClawAction->outportNum = m_Port_Y[modPortIndex].portNum;//变成松开端口
    }
    m_OperateProOrder[m_CurrentSelectProOrderList].delay = ui->ClawDelay->text().toDouble()*100;
}


//获取输入输出端口数组对应索引
uint16_t Teach::GetXYPortIndex(P_ProOrderStruct OIS)
{
    uint16_t Index = 0;
    switch (OIS.cmd) {
    case C_CLAW_ACTION:
    {
        P_ClawActionStruct* ClawAction = (P_ClawActionStruct*) OIS.pData;
        for(uint16_t i=0;i<OUTPUT_TOTAL_NUM;i++)
        {
            if(m_Port_Y[i].portNum == ClawAction->outportNum)
            {
                Index = i;
                break;
            }
        }
        break;
    }
    default:
        break;

    }
    return Index;
}

//轴动作指令-X1选中/不选中处理
void Teach::on_cb_General_X1_clicked(bool checked)
{
    if(checked == true)
    {//如果选中主引拔x1轴
        Temp_AxisMoveOrder[0].axis = X1_AXIS;
    }
    else if(checked == false)
    {
        Temp_AxisMoveOrder[0].axis = 6;
    }
}
//轴动作-Y1轴复选框处理函数
void Teach::on_cb_General_Y1_clicked(bool checked)
{
    if(checked == true)
    {//如果选中主引拔Y1轴
        Temp_AxisMoveOrder[1].axis = Y1_AXIS;
    }
    else if(checked == false)
    {
        Temp_AxisMoveOrder[1].axis = 6;
    }
}
//轴动作-Z1轴复选框处理函数
void Teach::on_cb_General_Z1_clicked(bool checked)
{
    if(checked == true)
    {//如果选中主引拔Z1轴
        Temp_AxisMoveOrder[2].axis = Z1_AXIS;
    }
    else if(checked == false)
    {
        Temp_AxisMoveOrder[2].axis = 6;
    }
}
//轴动作-C轴复选框处理函数
void Teach::on_cb_General_C_clicked(bool checked)
{
    if(checked == true)
    {//如果选中主引拔C轴
        Temp_AxisMoveOrder[3].axis = C_AXIS;
    }
    else if(checked == false)
    {
        Temp_AxisMoveOrder[3].axis = 6;
    }
}
//轴动作-Y2轴复选框处理函数
void Teach::on_cb_General_Y2_clicked(bool checked)
{
    if(checked == true)
    {//如果选中主引拔Z1轴
        Temp_AxisMoveOrder[4].axis = Y2_AXIS;
    }
    else if(checked == false)
    {
        Temp_AxisMoveOrder[4].axis = 6;
    }
}
//轴动作-Z2轴复选框处理函数
void Teach::on_cb_General_Z2_clicked(bool checked)
{
    if(checked == true)
    {//如果选中主引拔Z1轴
        Temp_AxisMoveOrder[5].axis = Z2_AXIS;
    }
    else if(checked == false)
    {
        Temp_AxisMoveOrder[5].axis = 6;
    }
}
//X1-速度
void Teach::on_lineEdit_General_Speed_X1_editingFinished()
{
    Temp_AxisMoveOrder[X1_AXIS].speed = ui->lineEdit_General_Speed_X1->text().toInt();
}
//Y1-速度
void Teach::on_lineEdit_General_Speed_Y1_editingFinished()
{
    Temp_AxisMoveOrder[Y1_AXIS].speed = ui->lineEdit_General_Speed_Y1->text().toInt();
}
//Z1-速度
void Teach::on_lineEdit_General_Speed_Z1_editingFinished()
{
    Temp_AxisMoveOrder[Z1_AXIS].speed = ui->lineEdit_General_Speed_Z1->text().toInt();
}
//Y2-速度
void Teach::on_lineEdit_General_Speed_Y2_editingFinished()
{
    Temp_AxisMoveOrder[Y2_AXIS].speed = ui->lineEdit_General_Speed_Y2->text().toInt();
}
//Z2-速度
void Teach::on_lineEdit_General_Speed_Z2_editingFinished()
{
    Temp_AxisMoveOrder[Z2_AXIS].speed = ui->lineEdit_General_Speed_Z2->text().toInt();
}
//C-速度
void Teach::on_lineEdit_General_Speed_C_editingFinished()
{
    Temp_AxisMoveOrder[C_AXIS].speed = ui->lineEdit_General_Speed_C->text().toInt();
}


void Teach::on_lineEdit_General_Position_X1_editingFinished()
{

    if(ui->lineEdit_General_Position_X1->text().toDouble()*100 <= m_AxisPar[X1_AXIS].axisMaxPos)
    {
        Temp_AxisMoveOrder[X1_AXIS].pos = ui->lineEdit_General_Position_X1->text().toDouble()*100;
    }
    else
    {
        ui->lineEdit_General_Position_X1->setText(QString::number((float)(Temp_AxisMoveOrder[X1_AXIS].pos)/100));
        //可添加提示超出最大范围
        MainWindow::pMainWindow->showErrorTip(tr("X1轴超出最大范围"),TipMode::ONLY_OK);
    }
}

void Teach::on_lineEdit_General_Position_Y1_editingFinished()
{
    if(ui->lineEdit_General_Position_Y1->text().toDouble()*100 <= m_AxisPar[Y1_AXIS].axisMaxPos)
    {
        Temp_AxisMoveOrder[Y1_AXIS].pos = ui->lineEdit_General_Position_Y1->text().toDouble()*100;
    }
    else
    {
        ui->lineEdit_General_Position_Y1->setText(QString::number((float)(Temp_AxisMoveOrder[Y1_AXIS].pos)/100));
        //可添加提示超出最大范围
        MainWindow::pMainWindow->showErrorTip(tr("Y1轴超出最大范围"),TipMode::ONLY_OK);
    }
}

void Teach::on_lineEdit_General_Position_Z1_editingFinished()
{
    if(ui->lineEdit_General_Position_Z1->text().toDouble()*100 <= m_AxisPar[Z1_AXIS].axisMaxPos)
    {
        Temp_AxisMoveOrder[Z1_AXIS].pos = ui->lineEdit_General_Position_Z1->text().toDouble()*100;
    }
    else
    {
        ui->lineEdit_General_Position_Z1->setText(QString::number((float)(Temp_AxisMoveOrder[Z1_AXIS].pos)/100));
        //可添加提示超出最大范围
        MainWindow::pMainWindow->showErrorTip(tr("Z1轴超出最大范围"),TipMode::ONLY_OK);
    }
}

void Teach::on_lineEdit_General_Position_Y2_editingFinished()
{

    if(ui->lineEdit_General_Position_Y2->text().toDouble()*100 <= m_AxisPar[Y2_AXIS].axisMaxPos)
    {
        Temp_AxisMoveOrder[Y2_AXIS].pos = ui->lineEdit_General_Position_Y2->text().toDouble()*100;
    }
    else
    {
        ui->lineEdit_General_Position_Y2->setText(QString::number((float)(Temp_AxisMoveOrder[Y2_AXIS].pos)/100));
        //可添加提示超出最大范围
        MainWindow::pMainWindow->showErrorTip(tr("Y2轴超出最大范围"),TipMode::ONLY_OK);
    }
}

void Teach::on_lineEdit_General_Position_Z2_editingFinished()
{
    if(ui->lineEdit_General_Position_Y2->text().toDouble()*100 <= m_AxisPar[Z2_AXIS].axisMaxPos)
    {
        Temp_AxisMoveOrder[Z2_AXIS].pos = ui->lineEdit_General_Position_Y2->text().toDouble()*100;
    }
    else
    {
        ui->lineEdit_General_Position_Z2->setText(QString::number((float)(Temp_AxisMoveOrder[Z2_AXIS].pos)/100));
        //可添加提示超出最大范围
        MainWindow::pMainWindow->showErrorTip(tr("Z2轴超出最大范围"),TipMode::ONLY_OK);
    }
}

//原料1松开/夹紧
void Teach::on_btn_Material_clamp_1_clicked()
{
    if(ui->btn_Material_clamp_1->getState() == false)
    {
        ui->btn_Material_clamp_1->setText(m_Port_Y[CLAW_METERIAL_1_LOOSENED].modifyName);//原料1松开
    }
    else
    {
        ui->btn_Material_clamp_1->setText(m_Port_Y[CLAW_METERIAL_1_CLAMP].modifyName);//原料1夹紧
    }
}
//成品1松开/夹紧
void Teach::on_btn_finish_product_1_clicked()
{
    if(ui->btn_finish_product_1->getState() == false)
    {
        ui->btn_finish_product_1->setText(m_Port_Y[CLAW_PRODUCT_1_LOOSENED].modifyName);
    }
    else
    {
        ui->btn_finish_product_1->setText(m_Port_Y[CLAW_PRODUCT_1_CLAMP].modifyName);
    }
}
//卡爪正转/反转
void Teach::on_btn_Claw_roll_1_clicked()
{
    if(ui->btn_Claw_roll_1->getState() == false)
    {
        ui->btn_Claw_roll_1->setText(m_Port_Y[CLAW_CLAW_1_LOOSENED].modifyName);//卡爪1正转
    }
    else
    {
        ui->btn_Claw_roll_1->setText(m_Port_Y[CLAW_CLAW_1_CLAMP].modifyName);//卡爪1反转
    }
}
//切换信号检测子界面显示
void Teach::on_tabWidget_Reserve_currentChanged(int index)
{
    if(index == 0)
    {
        Pro_ClawCheckStructInit();//刷新卡爪检测界面参数
    }
    else if(index == 1)
    {
        Pro_ReserveCheckStructInit();//刷新预留检测界面参数
    }
}
//启动加工1通/断按键
void Teach::on_Start_Up_Mach_button_clicked()
{
    Temp_MachineOutStruct[0].type = !Temp_MachineOutStruct[0].type;
    if(Temp_MachineOutStruct[0].type == 1)
    {
        ui->Start_Up_Mach_button->setText(m_Port_Y[MACHINE_START_PROCESS_1].modifyName+"通");//启动加工1通
    }
    else if(Temp_MachineOutStruct[0].type == 0)
    {
        ui->Start_Up_Mach_button->setText(m_Port_Y[MACHINE_START_PROCESS_1].modifyName+"断");//启动加工1断
    }
    Temp_MachineOutStruct[0].outportNum = m_Port_Y[MACHINE_START_PROCESS_1].portNum;
}
//主轴旋转1通/断按键
void Teach::on_Main_Axis_Rotate_button_clicked()
{
    Temp_MachineOutStruct[1].type = !Temp_MachineOutStruct[1].type;
    if(Temp_MachineOutStruct[1].type == 1)
    {
        ui->Main_Axis_Rotate_button->setText(m_Port_Y[MACHINE_SPINDLE_ROTATE_1].modifyName+"通");//主轴旋转1通
    }
    else if(Temp_MachineOutStruct[1].type == 0)
    {
        ui->Main_Axis_Rotate_button->setText(m_Port_Y[MACHINE_SPINDLE_ROTATE_1].modifyName+"断");//主轴旋转1断
    }
    Temp_MachineOutStruct[1].outportNum = m_Port_Y[MACHINE_SPINDLE_ROTATE_1].portNum;
}
//主轴定位1通/断按键
void Teach::on_Main_Axis_locat_button_clicked()
{
    Temp_MachineOutStruct[2].type = !Temp_MachineOutStruct[2].type;
    if(Temp_MachineOutStruct[2].type == 1)
    {
        ui->Main_Axis_locat_button->setText(m_Port_Y[MACHINE_SPINDLE_FIXED_POS_1].modifyName+"通");//主轴定位1通
    }
    else if(Temp_MachineOutStruct[2].type == 0)
    {
        ui->Main_Axis_locat_button->setText(m_Port_Y[MACHINE_SPINDLE_FIXED_POS_1].modifyName+"断");//主轴定位1断
    }
    Temp_MachineOutStruct[2].outportNum = m_Port_Y[MACHINE_SPINDLE_FIXED_POS_1].portNum;
}
//自动门1开/关
void Teach::on_Auto_Door_button_clicked()
{
    Temp_MachineOutStruct[3].type = !Temp_MachineOutStruct[3].type;
    if(Temp_MachineOutStruct[3].type == 1)
    {
        ui->Auto_Door_button->setText(m_Port_Y[MACHINE_AUTO_DOOR_1_OPEN].modifyName);//自动门1开
    }
    else if(Temp_MachineOutStruct[3].type == 0)
    {
        ui->Auto_Door_button->setText(m_Port_Y[MACHINE_AUTO_DOOR_1_CLOSE].modifyName);//自动门1关
    }
}
//吹气1通/断
void Teach::on_Blow_Air_button_clicked()
{
    Temp_MachineOutStruct[4].type = !Temp_MachineOutStruct[4].type;
    if(Temp_MachineOutStruct[4].type == 1)
    {
        ui->Blow_Air_button->setText(m_Port_Y[MACHINE_GASS_1_LOOSENED].modifyName+"通");//吹气1通
    }
    else if(Temp_MachineOutStruct[4].type == 0)
    {
        ui->Blow_Air_button->setText(m_Port_Y[MACHINE_GASS_1_LOOSENED].modifyName+"断");//吹气1断
    }
    Temp_MachineOutStruct[4].outportNum = m_Port_Y[MACHINE_GASS_1_LOOSENED].portNum;
}
//卡盘1夹紧/松开
void Teach::on_Chuck_button_clicked()
{
    Temp_MachineOutStruct[5].type = !Temp_MachineOutStruct[5].type;
    if(Temp_MachineOutStruct[5].type == 0)
    {
        ui->Chuck_button->setText(m_Port_Y[MACHINE_CHUCK_1_LOOSENED].modifyName);//卡盘1松开
    }
    else if(Temp_MachineOutStruct[5].type == 1)
    {
        ui->Chuck_button->setText(m_Port_Y[MACHINE_CHUCK_1_CLAMP].modifyName);//卡盘1夹紧
    }
}

void Teach::on_Stack_group_1_clicked()
{
    if(ui->Stack_group_1->isChecked())
    {
        ui->Stack_reset->setChecked(false);
        ui->Stack_group_2->setChecked(false);
        ui->Stack_group_3->setChecked(false);
        ui->Stack_group_4->setChecked(false);
        ui->Stack_group_5->setChecked(false);
        ui->Stack_group_6->setChecked(false);
        ui->Stack_group_7->setChecked(false);
        ui->Stack_group_8->setChecked(false);
        ui->Stack_group_9->setChecked(false);
        ui->Stack_group_10->setChecked(false);
        ui->Stack_group_11->setChecked(false);
        ui->Follow_Stack_group_1->setChecked(false);
        ui->Follow_Stack_group_2->setChecked(false);
        ui->Follow_Stack_group_3->setChecked(false);
        ui->Follow_Stack_group_4->setChecked(false);
    }
    else
    {
        ui->Stack_group_1->setChecked(true);
    }
}

void Teach::on_Stack_group_2_clicked()
{
    if(ui->Stack_group_2->isChecked())
    {
        ui->Stack_reset->setChecked(false);
        ui->Stack_group_1->setChecked(false);
        ui->Stack_group_3->setChecked(false);
        ui->Stack_group_4->setChecked(false);
        ui->Stack_group_5->setChecked(false);
        ui->Stack_group_6->setChecked(false);
        ui->Stack_group_7->setChecked(false);
        ui->Stack_group_8->setChecked(false);
        ui->Stack_group_9->setChecked(false);
        ui->Stack_group_10->setChecked(false);
        ui->Stack_group_11->setChecked(false);
        ui->Follow_Stack_group_1->setChecked(false);
        ui->Follow_Stack_group_2->setChecked(false);
        ui->Follow_Stack_group_3->setChecked(false);
        ui->Follow_Stack_group_4->setChecked(false);
    }
    else
    {
        ui->Stack_group_2->setChecked(true);
    }
}

void Teach::on_Stack_group_3_clicked()
{
    if(ui->Stack_group_3->isChecked())
    {
        ui->Stack_reset->setChecked(false);
        ui->Stack_group_1->setChecked(false);
        ui->Stack_group_2->setChecked(false);
        ui->Stack_group_4->setChecked(false);
        ui->Stack_group_5->setChecked(false);
        ui->Stack_group_6->setChecked(false);
        ui->Stack_group_7->setChecked(false);
        ui->Stack_group_8->setChecked(false);
        ui->Stack_group_9->setChecked(false);
        ui->Stack_group_10->setChecked(false);
        ui->Stack_group_11->setChecked(false);
        ui->Follow_Stack_group_1->setChecked(false);
        ui->Follow_Stack_group_2->setChecked(false);
        ui->Follow_Stack_group_3->setChecked(false);
        ui->Follow_Stack_group_4->setChecked(false);
    }
    else
    {
        ui->Stack_group_3->setChecked(true);
    }
}

void Teach::on_Stack_group_4_clicked()
{
    if(ui->Stack_group_4->isChecked())
    {
        ui->Stack_reset->setChecked(false);
        ui->Stack_group_1->setChecked(false);
        ui->Stack_group_2->setChecked(false);
        ui->Stack_group_3->setChecked(false);
        ui->Stack_group_5->setChecked(false);
        ui->Stack_group_6->setChecked(false);
        ui->Stack_group_7->setChecked(false);
        ui->Stack_group_8->setChecked(false);
        ui->Stack_group_9->setChecked(false);
        ui->Stack_group_10->setChecked(false);
        ui->Stack_group_11->setChecked(false);
        ui->Follow_Stack_group_1->setChecked(false);
        ui->Follow_Stack_group_2->setChecked(false);
        ui->Follow_Stack_group_3->setChecked(false);
        ui->Follow_Stack_group_4->setChecked(false);
    }
    else
    {
        ui->Stack_group_4->setChecked(true);
    }
}

void Teach::on_Stack_group_5_clicked()
{
    if(ui->Stack_group_5->isChecked())
    {
        ui->Stack_reset->setChecked(false);
        ui->Stack_group_1->setChecked(false);
        ui->Stack_group_2->setChecked(false);
        ui->Stack_group_3->setChecked(false);
        ui->Stack_group_4->setChecked(false);
        ui->Stack_group_6->setChecked(false);
        ui->Stack_group_7->setChecked(false);
        ui->Stack_group_8->setChecked(false);
        ui->Stack_group_9->setChecked(false);
        ui->Stack_group_10->setChecked(false);
        ui->Stack_group_11->setChecked(false);
        ui->Follow_Stack_group_1->setChecked(false);
        ui->Follow_Stack_group_2->setChecked(false);
        ui->Follow_Stack_group_3->setChecked(false);
        ui->Follow_Stack_group_4->setChecked(false);
    }
    else
    {
        ui->Stack_group_5->setChecked(true);
    }
}

void Teach::on_Stack_group_6_clicked()
{
    if(ui->Stack_group_6->isChecked())
    {
        ui->Stack_reset->setChecked(false);
        ui->Stack_group_1->setChecked(false);
        ui->Stack_group_2->setChecked(false);
        ui->Stack_group_3->setChecked(false);
        ui->Stack_group_4->setChecked(false);
        ui->Stack_group_5->setChecked(false);
        ui->Stack_group_7->setChecked(false);
        ui->Stack_group_8->setChecked(false);
        ui->Stack_group_9->setChecked(false);
        ui->Stack_group_10->setChecked(false);
        ui->Stack_group_11->setChecked(false);
        ui->Follow_Stack_group_1->setChecked(false);
        ui->Follow_Stack_group_2->setChecked(false);
        ui->Follow_Stack_group_3->setChecked(false);
        ui->Follow_Stack_group_4->setChecked(false);
    }
    else
    {
        ui->Stack_group_6->setChecked(true);
    }
}

void Teach::on_Stack_group_7_clicked()
{
    if(ui->Stack_group_7->isChecked())
    {
        ui->Stack_reset->setChecked(false);
        ui->Stack_group_1->setChecked(false);
        ui->Stack_group_2->setChecked(false);
        ui->Stack_group_3->setChecked(false);
        ui->Stack_group_4->setChecked(false);
        ui->Stack_group_5->setChecked(false);
        ui->Stack_group_6->setChecked(false);
        ui->Stack_group_8->setChecked(false);
        ui->Stack_group_9->setChecked(false);
        ui->Stack_group_10->setChecked(false);
        ui->Stack_group_11->setChecked(false);
        ui->Follow_Stack_group_1->setChecked(false);
        ui->Follow_Stack_group_2->setChecked(false);
        ui->Follow_Stack_group_3->setChecked(false);
        ui->Follow_Stack_group_4->setChecked(false);
    }
    else
    {
        ui->Stack_group_7->setChecked(true);
    }
}

void Teach::on_Stack_group_8_clicked()
{
    if(ui->Stack_group_8->isChecked())
    {
        ui->Stack_reset->setChecked(false);
        ui->Stack_group_1->setChecked(false);
        ui->Stack_group_2->setChecked(false);
        ui->Stack_group_3->setChecked(false);
        ui->Stack_group_4->setChecked(false);
        ui->Stack_group_5->setChecked(false);
        ui->Stack_group_6->setChecked(false);
        ui->Stack_group_7->setChecked(false);
        ui->Stack_group_9->setChecked(false);
        ui->Stack_group_10->setChecked(false);
        ui->Stack_group_11->setChecked(false);
        ui->Follow_Stack_group_1->setChecked(false);
        ui->Follow_Stack_group_2->setChecked(false);
        ui->Follow_Stack_group_3->setChecked(false);
        ui->Follow_Stack_group_4->setChecked(false);
    }
    else
    {
        ui->Stack_group_8->setChecked(true);
    }
}

void Teach::on_Stack_group_9_clicked()
{
    if(ui->Stack_group_9->isChecked())
    {
        ui->Stack_reset->setChecked(false);
        ui->Stack_group_1->setChecked(false);
        ui->Stack_group_2->setChecked(false);
        ui->Stack_group_3->setChecked(false);
        ui->Stack_group_4->setChecked(false);
        ui->Stack_group_5->setChecked(false);
        ui->Stack_group_7->setChecked(false);
        ui->Stack_group_8->setChecked(false);
        ui->Stack_group_6->setChecked(false);
        ui->Stack_group_10->setChecked(false);
        ui->Stack_group_11->setChecked(false);
        ui->Follow_Stack_group_1->setChecked(false);
        ui->Follow_Stack_group_2->setChecked(false);
        ui->Follow_Stack_group_3->setChecked(false);
        ui->Follow_Stack_group_4->setChecked(false);
    }
    else
    {
        ui->Stack_group_9->setChecked(true);
    }
}

void Teach::on_Stack_group_10_clicked()
{
    if(ui->Stack_group_10->isChecked())
    {
        ui->Stack_reset->setChecked(false);
        ui->Stack_group_1->setChecked(false);
        ui->Stack_group_2->setChecked(false);
        ui->Stack_group_3->setChecked(false);
        ui->Stack_group_4->setChecked(false);
        ui->Stack_group_5->setChecked(false);
        ui->Stack_group_7->setChecked(false);
        ui->Stack_group_8->setChecked(false);
        ui->Stack_group_9->setChecked(false);
        ui->Stack_group_6->setChecked(false);
        ui->Stack_group_11->setChecked(false);
        ui->Follow_Stack_group_1->setChecked(false);
        ui->Follow_Stack_group_2->setChecked(false);
        ui->Follow_Stack_group_3->setChecked(false);
        ui->Follow_Stack_group_4->setChecked(false);
    }
    else
    {
        ui->Stack_group_10->setChecked(true);
    }
}

void Teach::on_Stack_group_11_clicked()
{
    if(ui->Stack_group_11->isChecked())
    {
        ui->Stack_reset->setChecked(false);
        ui->Stack_group_1->setChecked(false);
        ui->Stack_group_2->setChecked(false);
        ui->Stack_group_3->setChecked(false);
        ui->Stack_group_4->setChecked(false);
        ui->Stack_group_5->setChecked(false);
        ui->Stack_group_7->setChecked(false);
        ui->Stack_group_8->setChecked(false);
        ui->Stack_group_9->setChecked(false);
        ui->Stack_group_10->setChecked(false);
        ui->Stack_group_6->setChecked(false);
        ui->Follow_Stack_group_1->setChecked(false);
        ui->Follow_Stack_group_2->setChecked(false);
        ui->Follow_Stack_group_3->setChecked(false);
        ui->Follow_Stack_group_4->setChecked(false);
    }
    else
    {
        ui->Stack_group_11->setChecked(true);
    }
}

void Teach::on_Follow_Stack_group_1_clicked()
{
    if(ui->Follow_Stack_group_1->isChecked())
    {
        ui->Stack_reset->setChecked(false);
        ui->Stack_group_1->setChecked(false);
        ui->Stack_group_2->setChecked(false);
        ui->Stack_group_3->setChecked(false);
        ui->Stack_group_4->setChecked(false);
        ui->Stack_group_5->setChecked(false);
        ui->Stack_group_7->setChecked(false);
        ui->Stack_group_8->setChecked(false);
        ui->Stack_group_9->setChecked(false);
        ui->Stack_group_10->setChecked(false);
        ui->Stack_group_11->setChecked(false);
        ui->Stack_group_6->setChecked(false);
        ui->Follow_Stack_group_2->setChecked(false);
        ui->Follow_Stack_group_3->setChecked(false);
        ui->Follow_Stack_group_4->setChecked(false);
    }
    else
    {
        ui->Follow_Stack_group_1->setChecked(true);
    }
}

void Teach::on_Follow_Stack_group_2_clicked()
{
    if(ui->Follow_Stack_group_2->isChecked())
    {
        ui->Stack_reset->setChecked(false);
        ui->Stack_group_1->setChecked(false);
        ui->Stack_group_2->setChecked(false);
        ui->Stack_group_3->setChecked(false);
        ui->Stack_group_4->setChecked(false);
        ui->Stack_group_5->setChecked(false);
        ui->Stack_group_7->setChecked(false);
        ui->Stack_group_8->setChecked(false);
        ui->Stack_group_9->setChecked(false);
        ui->Stack_group_10->setChecked(false);
        ui->Stack_group_11->setChecked(false);
        ui->Stack_group_6->setChecked(false);
        ui->Follow_Stack_group_1->setChecked(false);
        ui->Follow_Stack_group_3->setChecked(false);
        ui->Follow_Stack_group_4->setChecked(false);
    }
    else
    {
        ui->Follow_Stack_group_2->setChecked(true);
    }
}

void Teach::on_Follow_Stack_group_3_clicked()
{
    if(ui->Follow_Stack_group_3->isChecked())
    {
        ui->Stack_reset->setChecked(false);
        ui->Stack_group_1->setChecked(false);
        ui->Stack_group_2->setChecked(false);
        ui->Stack_group_3->setChecked(false);
        ui->Stack_group_4->setChecked(false);
        ui->Stack_group_5->setChecked(false);
        ui->Stack_group_7->setChecked(false);
        ui->Stack_group_8->setChecked(false);
        ui->Stack_group_9->setChecked(false);
        ui->Stack_group_10->setChecked(false);
        ui->Stack_group_11->setChecked(false);
        ui->Stack_group_6->setChecked(false);
        ui->Follow_Stack_group_1->setChecked(false);
        ui->Follow_Stack_group_2->setChecked(false);
        ui->Follow_Stack_group_4->setChecked(false);
    }
    else
    {
        ui->Follow_Stack_group_3->setChecked(true);
    }
}

void Teach::on_Follow_Stack_group_4_clicked()
{
    if(ui->Follow_Stack_group_4->isChecked())
    {
        ui->Stack_reset->setChecked(false);
        ui->Stack_group_1->setChecked(false);
        ui->Stack_group_2->setChecked(false);
        ui->Stack_group_3->setChecked(false);
        ui->Stack_group_4->setChecked(false);
        ui->Stack_group_5->setChecked(false);
        ui->Stack_group_7->setChecked(false);
        ui->Stack_group_8->setChecked(false);
        ui->Stack_group_9->setChecked(false);
        ui->Stack_group_10->setChecked(false);
        ui->Stack_group_11->setChecked(false);
        ui->Stack_group_6->setChecked(false);
        ui->Follow_Stack_group_1->setChecked(false);
        ui->Follow_Stack_group_2->setChecked(false);
        ui->Follow_Stack_group_3->setChecked(false);
    }
    else
    {
        ui->Follow_Stack_group_4->setChecked(true);
    }
}

void Teach::on_Stack_reset_clicked()
{
    if(ui->Stack_reset->isChecked())
    {
        ui->Stack_group_1->setChecked(false);
        ui->Stack_group_2->setChecked(false);
        ui->Stack_group_3->setChecked(false);
        ui->Stack_group_4->setChecked(false);
        ui->Stack_group_5->setChecked(false);
        ui->Stack_group_7->setChecked(false);
        ui->Stack_group_8->setChecked(false);
        ui->Stack_group_9->setChecked(false);
        ui->Stack_group_10->setChecked(false);
        ui->Stack_group_11->setChecked(false);
        ui->Follow_Stack_group_1->setChecked(false);
        ui->Follow_Stack_group_2->setChecked(false);
        ui->Follow_Stack_group_3->setChecked(false);
        ui->Follow_Stack_group_4->setChecked(false);
    }
    else
    {
        ui->Stack_reset->setChecked(true);
    }
}


void Teach::pageInit()
{
    /**************************逻辑&变量部分槽****************************************/
    connect(ui->chboxAnd, &QCheckBox::clicked, [=](){
        if (ui->chboxAnd->isChecked())
            ui->chboxOr->setChecked(false);
    });
    connect(ui->chboxOr, &QCheckBox::clicked, [=](){
        if (ui->chboxOr->isChecked())
            ui->chboxAnd->setChecked(false);
    });
    connect(ui->chboxVarSelectVarPreOp, &QCheckBox::clicked, this, [=](){
        if (ui->chboxVarSelectVarPreOp->isChecked())
        {
            ui->chboxConstantVarOp->show();
            ui->chboxVariableVarOp->show();
            ui->chboxAxisVarOp->show();
            ui->chboxProdNumVarOp->show();
            ui->chboxStackCntVarOp->hide();

            ui->chboxConstantVarOp->setChecked(true);
            //修改小数位数
            if(ui->coboxVarSelectVarPreOp->currentIndex()<=VAR_TOTAL_NUM)
            {
                ui->editConstantVarPostOp->setDecimalPlaces(m_VariableType[ui->coboxVarSelectVarPreOp->currentIndex()]);
            }
            else
            {
                ui->coboxVarSelectVarPreOp->setCurrentIndex(0);
                ui->editConstantVarPostOp->setDecimalPlaces(m_VariableType[ui->coboxVarSelectVarPreOp->currentIndex()]);
            }
        }
    });

    connect(ui->chboxAxisSelectVarPreOp, &QCheckBox::clicked, this, [=](){
        if (ui->chboxAxisSelectVarPreOp->isChecked())
        {
            ui->chboxConstantVarOp->show();
            ui->chboxVariableVarOp->show();
            ui->chboxAxisVarOp->hide();
            ui->chboxProdNumVarOp->hide();
            ui->chboxStackCntVarOp->hide();

            ui->chboxConstantVarOp->setChecked(true);
            ui->editConstantVarPostOp->setDecimalPlaces(2);
        }
    });
    connect(ui->chboxStackSelectVarPreOp, &QCheckBox::clicked, this, [=](){
        if (ui->chboxStackSelectVarPreOp->isChecked())
        {
            ui->chboxConstantVarOp->show();
            ui->chboxVariableVarOp->hide();
            ui->chboxAxisVarOp->hide();
            ui->chboxProdNumVarOp->hide();
            ui->chboxStackCntVarOp->show();

            ui->chboxConstantVarOp->setChecked(true);
            ui->editConstantVarPostOp->setDecimalPlaces(0);
        }
    });
    connect(ui->chboxRealProdNumPreOp, &QCheckBox::clicked, this, [=](){
        if (ui->chboxRealProdNumPreOp->isChecked())
        {
            ui->chboxConstantVarOp->show();
            ui->chboxVariableVarOp->show();
            ui->chboxAxisVarOp->hide();
            ui->chboxProdNumVarOp->hide();
            ui->chboxStackCntVarOp->hide();

            ui->chboxConstantVarOp->setChecked(true);
            ui->editConstantVarPostOp->setDecimalPlaces(0);
        }
    });
    connect(ui->chboxConstantVarOp, &QCheckBox::stateChanged, this, [=](int state){
        if (state == Qt::Checked)
        {
            ui->stkWgtVarOp->setCurrentIndex(0);
            ui->coboxVarOperand->setCurrentIndex(0);
            ui->coboxVarOperand->setEnabled(true);
        }
        else {
            ui->coboxVarOperand->setCurrentIndex(4);
            ui->coboxVarOperand->setDisabled(true);
        }
    });
    connect(ui->chboxVariableVarOp, &QCheckBox::stateChanged, this, [=](int state){
        if (state == Qt::Checked)
            ui->stkWgtVarOp->setCurrentIndex(1);
    });
    connect(ui->chboxAxisVarOp, &QCheckBox::stateChanged, this, [=](int state){
        if (state == Qt::Checked)
            ui->stkWgtVarOp->setCurrentIndex(2);
    });
    connect(ui->chboxProdNumVarOp, &QCheckBox::stateChanged, this, [=](int state){
        if (state == Qt::Checked)
            ui->stkWgtVarOp->setCurrentIndex(3);
    });
    connect(ui->chboxStackCntVarOp, &QCheckBox::stateChanged, this, [=](int state){
        if (state == Qt::Checked)
            ui->stkWgtVarOp->setCurrentIndex(4);
    });

    /****************************搜索*****************************************/
    connect(ui->chboxSearchAxisSelect, &QCheckBox::clicked, this, [=](){
        if (ui->chboxSearchAxisSelect->isChecked())
        {
            ui->chboxResponseStop->setChecked(false);
        }
    });
    connect(ui->chboxResponseStop, &QCheckBox::clicked, this, [=](){
        if (ui->chboxResponseStop->isChecked())
        {
            ui->chboxSearchAxisSelect->setChecked(false);
        }
    });
    /****************************信号等待*****************************************/
    connect(ui->tabWgtSigWait, &QTabWidget::currentChanged, this, [=](int tabIndex){
        bool visible = tabIndex;
        ui->grboxReturn->setVisible(visible);
    });
    connect(ui->chboxReturnLabel, &QCheckBox::stateChanged, [=](int state){
        if (state == Qt::Checked)
        {
            ui->coboxReturnLabe_box->show();
            ui->editReturnStepNum->hide();
        }
    });
    connect(ui->chboxReturnStepNum, &QCheckBox::stateChanged, [=](int state){
        if (state == Qt::Checked)
        {
            ui->coboxReturnLabe_box->hide();
            ui->editReturnStepNum->show();
        }
    });
    coboxVarSelectVarPreOpItemSet();
    AxisTypeChangeTeachHandle();
}

//预留通按钮
void Teach::on_Reserve_Pass_Button_clicked()
{
    if(ui->Reserve_Pass_Button->getState() == 1)
    {
        ui->Reserve_Pass_Button->setText(tr("预留通"));
    }
    else
    {
        ui->Reserve_Pass_Button->setText(tr("预留断"));
    }
}

//预留通断复选框
void Teach::on_Reserve_Pass_Box_clicked()
{
    if(ui->Reserve_Pass_Box->isChecked())
    {
        ui->Reserve_OutTime_Box->setChecked(false);
        ui->Reserve_Overturn_Box->setChecked(false);
    }
    else
    {
        ui->Reserve_Pass_Box->setChecked(true);
    }
    Temp_ReserveOutStruct.function = 0;//输出功能-普通输出
}
//预留通时复选框
void Teach::on_Reserve_OutTime_Box_clicked()
{
    if(ui->Reserve_OutTime_Box->isChecked())
    {
        ui->Reserve_Pass_Box->setChecked(false);
        ui->Reserve_Overturn_Box->setChecked(false);
    }
    else
    {
        ui->Reserve_OutTime_Box->setChecked(true);
    }
    Temp_ReserveOutStruct.function = 1;//输出功能-预留同时
    Temp_ReserveOutStruct.type = 1;//达到预留通设定的时间后自动断开
}
//预留翻转复选框
void Teach::on_Reserve_Overturn_Box_clicked()
{
    if(ui->Reserve_Overturn_Box->isChecked())
    {
        ui->Reserve_Pass_Box->setChecked(false);
        ui->Reserve_OutTime_Box->setChecked(false);
    }
    else
    {
        ui->Reserve_Overturn_Box->setChecked(true);
    }
    Temp_ReserveOutStruct.function = 2;//输出功能-预留翻转
    Temp_ReserveOutStruct.type = !Temp_ReserveOutStruct.type;
}
//预留输出间隔
void Teach::on_lineEdit_Reserve_num_editingFinished()
{
    Temp_ReserveOutStruct.outportNum = ui->lineEdit_Reserve_num->text().toUInt();
}
//速度教导-X1速度输入
void Teach::on_editSpeedTeachX1_editingFinished()
{
    Temp_AxisMoveOrder[X1_AXIS].speed = ui->editSpeedTeachX1->text().toUInt();
}
//速度教导-Y1速度输入
void Teach::on_editSpeedTeachY1_editingFinished()
{
     Temp_AxisMoveOrder[Y1_AXIS].speed = ui->editSpeedTeachY1->text().toUInt();
}
//速度教导-Z1速度输入
void Teach::on_editSpeedTeachZ1_editingFinished()
{
    Temp_AxisMoveOrder[Z1_AXIS].speed = ui->editSpeedTeachZ1->text().toUInt();
}
//速度教导-Y2速度输入
void Teach::on_editSpeedTeachY2_editingFinished()
{
     Temp_AxisMoveOrder[Y2_AXIS].speed = ui->editSpeedTeachY2->text().toUInt();
}
//速度教导-Z2速度输入
void Teach::on_editSpeedTeachZ2_editingFinished()
{
    Temp_AxisMoveOrder[Z2_AXIS].speed = ui->editSpeedTeachZ2->text().toUInt();
}
//速度教导-C速度输入
void Teach::on_editSpeedTeachC_editingFinished()
{
     Temp_AxisMoveOrder[C_AXIS].speed = ui->editSpeedTeachC->text().toUInt();
}

void Teach::on_chboxReturnLabel_clicked()
{
    if(ui->chboxReturnLabel->isChecked())
    {
        Temp_WaitInMachineStruct.type = 1;//返回标签
        Temp_WaitInClawStruct.type = 1;//返回标签
        Temp_WaitInReserveStruct.type = 1;//返回标签
    }
}

void Teach::on_chboxReturnStepNum_clicked()
{
    if(ui->chboxReturnStepNum->isChecked())
    {
        Temp_WaitInMachineStruct.type = 0;//返回步号
        Temp_WaitInClawStruct.type = 0;//返回步号
        Temp_WaitInReserveStruct.type = 0;//返回步号
    }
}
//步号输入
void Teach::on_editReturnStepNum_editingFinished()
{
    int16_t backlistnum = ui->editReturnStepNum->text().toInt();
    Temp_WaitInMachineStruct.backListNum = backlistnum;//步号
    Temp_WaitInClawStruct.backListNum = backlistnum;//步号
    Temp_WaitInReserveStruct.backListNum = backlistnum;//步号
}

void Teach::on_btnAlarmLamp_button_clicked()
{
    Temp_OtherAlarmLampStruct.type = !Temp_OtherAlarmLampStruct.type;
    if(Temp_OtherAlarmLampStruct.type == 0)
    {
        ui->btnAlarmLamp_button->setText(m_Port_Y[OTHER_ALARM_LAMP].modifyName+"关");
    }
    else
    {
        ui->btnAlarmLamp_button->setText(m_Port_Y[OTHER_ALARM_LAMP].modifyName+"开");
    }
    Temp_OtherAlarmLampStruct.outportNum = m_Port_Y[OTHER_ALARM_LAMP].portNum;//实际端口
}

void Teach::on_btnAlarmSound_button_clicked()
{
    Temp_OtherAlarmSoundStruct.type = !Temp_OtherAlarmSoundStruct.type;
   if(Temp_OtherAlarmSoundStruct.type == 0)
   {
       ui->btnAlarmSound_button->setText(m_Port_Y[OTHER_ALARM_SOUND].modifyName+"关");
   }
   else
   {
       ui->btnAlarmSound_button->setText(m_Port_Y[OTHER_ALARM_SOUND].modifyName+"开");
   }
   Temp_OtherAlarmLampStruct.outportNum = m_Port_Y[OTHER_ALARM_SOUND].portNum;//实际端口
}

void Teach::on_lineEdit_General_Position_C_editingFinished()
{
    if(ui->lineEdit_General_Position_C->text().toDouble()*100 <= m_AxisPar[C_AXIS].axisMaxPos)
    {
        Temp_AxisMoveOrder[C_AXIS].pos = ui->lineEdit_General_Position_C->text().toDouble()*100;
    }
    else
    {
        ui->lineEdit_General_Position_C->setText(QString::number((float)(Temp_AxisMoveOrder[C_AXIS].pos)/100));
        //可添加提示超出最大范围
    }
}

void Teach::on_tabWidget_Teach_tabBarClicked(int index)
{
    if(index == 0)
    {
        ui->Subroutine1_button->setText(m_NameDefine[1].subProgName[0]);
        ui->Subroutine2_button->setText(m_NameDefine[1].subProgName[1]);
        ui->Subroutine3_button->setText(m_NameDefine[1].subProgName[2]);
        ui->Subroutine4_button->setText(m_NameDefine[1].subProgName[3]);
        ui->Subroutine5_button->setText(m_NameDefine[1].subProgName[4]);
        ui->Subroutine6_button->setText(m_NameDefine[1].subProgName[5]);
        ui->Subroutine7_button->setText(m_NameDefine[1].subProgName[6]);
        ui->Subroutine8_button->setText(m_NameDefine[1].subProgName[7]);
        if(OrderNeedSaveFlag == true)
        {
            int reply =  MainWindow::pMainWindow->showErrorTip(tr("教导参数有修改，是否需要保存？"));
            if (reply == QDialog::Accepted)
            {
                widgetSwitchOrderSaveHandel(true);
            }
            else
            {
                widgetSwitchOrderSaveHandel(false);
            }
        }
        Switch_Pro_ReadOrder(0);
        g_Usart->ExtendSendProDeal(CMD_MAIN_PRO,CMD_SUN_PRO_INFO);
        g_Usart->ExtendSendProDeal(CMD_MAIN_PRO,CMD_SUN_PRO_SAVE,0,2);
        Teach_File_List_Refresh();//刷新列表
    }
    ui->btnModify->setChecked(false);//切换界面时关闭编辑状态
}
//高级-子程序-标准
void Teach::on_cb_Standard_clicked()
{
    Temp_SunProStruct.oprMode = 0;
}
//高级-子程序-自动
void Teach::on_cb_Automatic_clicked()
{
    Temp_SunProStruct.oprMode = 1;
}
//高级-子程序-并行一次
void Teach::on_cb_ParallelOnce_clicked()
{
    Temp_SunProStruct.oprMode = 2;
}
//高级-子程序-等待子程序完成
void Teach::on_cb_WaitCompletion_clicked()
{
    Temp_SunProStruct.oprMode = 3;
}
//高级-子程序-子程序编号选择框
void Teach::on_combo_SubroutineAll_currentIndexChanged(int index)
{
    Temp_SunProStruct.sunProNum = index+1;//1-8
}
//高级-伺服停止-轴编号选择框
void Teach::on_combo_ServoStop_AxisAll_currentIndexChanged(int index)
{
    if(ui->combo_ServoStop_AxisAll->currentText() == m_NameDefine[1].axisName[index])
    {
        Temp_AxisStopStruct.axis = index;
    }
}
//高级-转矩保护-轴编号选择框
void Teach::on_combo_LowSpeedTorque_AxisAll_currentIndexChanged(int index)
{
    if(ui->combo_LowSpeedTorque_AxisAll->currentText() == m_NameDefine[1].axisName[index])
    {
        Temp_TorqueGardStruct.axis = index;
    }
}
//高级-转矩保护-转矩百分比输入
void Teach::on_lineEdit_LowSpeedTorque_delay_editingFinished()
{
    Temp_TorqueGardStruct.torqueValue = ui->lineEdit_LowSpeedTorque_delay->text().toUInt();
}
//高级-偏移-轴编号选择框
void Teach::on_combo_Shift_AxisAll_currentIndexChanged(int index)
{
    if(ui->combo_Shift_AxisAll->currentText() == m_NameDefine[1].axisName[index])
    {
        Temp_OffsetAxisStruct.axis = index;
    }
}

void Teach::on_lineEdit_Shift_Position_editingFinished()
{
    Temp_OffsetAxisStruct.offsetPos = ui->lineEdit_Shift_Position->text().toDouble()*100;
}

void Teach::on_lineEdit_Shift_Speed_editingFinished()
{
    Temp_OffsetAxisStruct.speed = ui->lineEdit_Shift_Speed->text().toUInt();
}

void Teach::on_editSearchPosition_editingFinished()
{
    Temp_SearchAxisMoveStruct.maxPos = ui->editSearchPosition->text().toDouble()*100;
}

void Teach::on_editSearchSpeed_editingFinished()
{
    Temp_SearchAxisMoveStruct.runSpeed = ui->editSearchSpeed->text().toUInt();
}

void Teach::on_tabWidget_logic_tabBarClicked(int index)
{
    switch (index) {
    case 0://如果page
        Temp_LogicIfStruct.sufferCmpValue[0]=0;
        Temp_LogicIfStruct.sufferCmpType[0]=0;
        Temp_LogicIfStruct.cmpType[0]=0;
        Temp_LogicIfStruct.reqSelectFlag[0] = 0;
        Temp_LogicIfStruct.inportNum[0]=0;
        Temp_LogicIfStruct.inportType[0]=0;
        Temp_LogicIfStruct.cmpMode[0]=0;

        Temp_LogicIfStruct.sufferCmpValue[1]=0;
        Temp_LogicIfStruct.sufferCmpType[1]=0;
        Temp_LogicIfStruct.cmpType[1]=0;
        Temp_LogicIfStruct.reqSelectFlag[1] = 0;
        Temp_LogicIfStruct.inportNum[1]=0;
        Temp_LogicIfStruct.inportType[1]=0;
        Temp_LogicIfStruct.cmpMode[1]=0;
        break;
    case 1://循环page
        Temp_LogicWhileStartStruct.cycNum = 1;
        ui->editLoopNum->setText("1");
        break;
    case 2://变量page
    {
        Temp_LogicVarStruct.sufferOperType=0;
        Temp_LogicVarStruct.sufferOperValue=0;
        Temp_LogicVarStruct.varNum=1;
        Temp_LogicVarStruct.operMode=0;

        Temp_LogicAxisStruct.sufferOperType=0;
        Temp_LogicAxisStruct.sufferOperValue=0;
        Temp_LogicAxisStruct.axisNum=1;
        Temp_LogicAxisStruct.operMode=0;

        Temp_LogicStackStruct.sufferOperType=0;
        Temp_LogicStackStruct.sufferOperValue=0;
        Temp_LogicStackStruct.stackNum=1;
        Temp_LogicStackStruct.operMode=0;

        Temp_LogicCurProductNumStruct.sufferOperType=0;
        Temp_LogicCurProductNumStruct.sufferOperValue=0;
        Temp_LogicCurProductNumStruct.productNum=1;
        Temp_LogicCurProductNumStruct.operMode=0;
        ui->coboxVarSelectVarPreOp->setCurrentIndex(0);
        ui->coboxAxisSelectVarPreOp->setCurrentIndex(0);
        ui->coboxStackSelectVarPreOp->setCurrentIndex(0);
        QString VariableText = "";
        if(m_VariableType[0]==0)
        {
            VariableText = "整数";
        }
        else if(m_VariableType[0] == 1)
        {
            VariableText = "一位小数";
        }
        else if(m_VariableType[0] == 2)
        {
            VariableText = "两位小数";
        }
        ui->EditOperatorVarPreOp->setText(VariableText);
        break;
    }
    case 3://定时器page
        Temp_LogicTimeStruct.timeNum=1;
        Temp_LogicTimeStruct.operMode=0;
        break;
    default:
        break;
    }
}
//编辑按钮
void Teach::on_btnModify_clicked()
{
    if(ui->btnModify->isChecked())
    {
        if(m_OperateProOrderListNum == 0)
        {//如果未载入程序，直接返回
            return;
        }
        ui->tabWidget_Teach->setCurrentIndex(0);
        OrderEdit_Handle();
        if(m_OperateProOrder[m_CurrentSelectProOrderList].cmd == C_STACK_MOVE)
        {
            Stack_Dialog_show();
        }
    }
    else
    {
        ui->tabWidget_Teach->setCurrentIndex(2);//取消编辑时，自动切换到通用主界面
        ui->stackedWidget_General->setCurrentWidget(ui->General_main_page);
    }
}

/*************************************************************************
**  函数名：  Stack_Dialog_show()
**	输入参数：
**	输出参数：
**	函数功能：堆叠指令编辑弹窗处理函数
**  作者：    wukui
**  开发日期：2024/9/30
**************************************************************************/
void Teach::Stack_Dialog_show()
{
    P_StackMoveStruct* StackMove =(P_StackMoveStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
    if(m_StackFunc.siloType == 0)
    {//料仓类型-0平板式料仓
        if(m_StackFunc.stackType == 0)
        {//堆叠方式-3点式
            stack_Temp->switchStackWay(StackMode::TEACH_THREE_POINT);
        }
        else if(m_StackFunc.stackType == 1)
        {
            stack_Temp->switchStackWay(StackMode::TEACH_FOUR_POINT);
        }
        else if(m_StackFunc.stackType == 2)
        {
            stack_Temp->switchStackWay(StackMode::TEACH_NOMAL);
        }
        stack_Temp->updateGroupIndex(StackMove->stackNum-1);
        stack_Temp->syncParaToUI();
        stack_Dialog->setGeometry(5,120,stack_Temp->width(),stack_Temp->height());//设置显示其实位置和大小
        stack_Temp->show();
        stack_Dialog->show();
        emit StackEditBaseInit_signal();
    }
    ui->Stack_Edit_btnAxisX1->setStyleSheet("border-style:outset;\
                                            background-color:qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop:0 #00befa, stop: 1 #0160ea);/*背景色*/\
                                            border-width:1px;\
                                            border-color:#0055ff;\
                                            border-radius:5px;");
}
/*************************************************************************
**  函数名：  Stack_Edit_Handle(uint8_t AxisIndex)
**	输入参数：AxisIndex 轴编号0-x 1-y 2-z
**	输出参数：
**	函数功能：堆叠指令编辑处理函数
**  作者：    wukui
**  开发日期：2024/9/30
**************************************************************************/
void Teach::Stack_Edit_Handle(uint8_t AxisIndex)
{
    ui->stkWgtProgram->setCurrentWidget(ui->pageStack);

    P_StackMoveStruct* StackMove =(P_StackMoveStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
    if(m_StackInfo[StackMove->stackNum-1].stackPointNum[AxisIndex]==0)
    {
        ui->stkWgtStack->setCurrentWidget(ui->Stack_Edit_page2);
    }
    else if(m_StackInfo[StackMove->stackNum-1].stackPointNum[AxisIndex] > 0 && AxisIndex == STACK_X_AXIS)
    {
        ui->stkWgtStack->setCurrentWidget(ui->Stack_Edit_pageX1);
        double stackstartPos = m_StackInfo[StackMove->stackNum-1].stackStartPos[AxisIndex];
        double stackDelay = m_OperateProOrder[m_CurrentSelectProOrderList].delay;
        ui->Stack_Edit_Pos->setText(QString::number(stackstartPos/100,'f',2));
        ui->Stack_Edit_Speed->setText(QString::number(m_StackInfo[StackMove->stackNum-1].axisSpeed[AxisIndex]));
        ui->Stack_Edit_Delay->setText(QString::number(stackDelay/100,'f',2));
        ui->cb_Stack_Edit_end->setChecked(StackMove->advEndFlag[AxisIndex]);
        if(StackMove->advCSpeedFlag[AxisIndex] == 0)
        {
            ui->cb_Stack_Edit_acc->setChecked(false);
            ui->cb_Stack_Edit_DSpeed->setChecked(false);
        }
        else if(StackMove->advCSpeedFlag[AxisIndex] == 1)
        {
            ui->cb_Stack_Edit_acc->setChecked(true);
            ui->cb_Stack_Edit_DSpeed->setChecked(false);
        }
        else if(StackMove->advCSpeedFlag[AxisIndex] == 2)
        {
            ui->cb_Stack_Edit_acc->setChecked(false);
            ui->cb_Stack_Edit_DSpeed->setChecked(true);
        }
        double AdvPos = StackMove->advEndDis[AxisIndex];
        ui->Stack_Edit_AdvPos->setText(QString::number(AdvPos/100,'f',2));
        ui->Stack_Edit_AdvSpeed->setText(QString::number(StackMove->advCSpeedSpeed[AxisIndex]));
    }
    else if(m_StackInfo[StackMove->stackNum-1].stackPointNum[AxisIndex] > 0 && AxisIndex == STACK_Z_AXIS)
    {
        ui->stkWgtStack->setCurrentWidget(ui->Stack_Edit_pageZ1);
        double stackstartPos = m_StackInfo[StackMove->stackNum-1].stackStartPos[AxisIndex];
        double stackDelay = m_OperateProOrder[m_CurrentSelectProOrderList].delay;
        ui->Stack_Edit_Pos_2->setText(QString::number(stackstartPos/100,'f',2));
        ui->Stack_Edit_Speed_2->setText(QString::number(m_StackInfo[StackMove->stackNum-1].axisSpeed[AxisIndex]));
        ui->Stack_Edit_Delay_2->setText(QString::number(stackDelay/100,'f',2));
        ui->cb_Stack_Edit_end_2->setChecked(StackMove->advEndFlag[AxisIndex]);
        if(StackMove->advCSpeedFlag[AxisIndex] == 0)
        {
            ui->cb_Stack_Edit_acc_2->setChecked(false);
            ui->cb_Stack_Edit_DSpeed_2->setChecked(false);
        }
        else if(StackMove->advCSpeedFlag[AxisIndex] == 1)
        {
            ui->cb_Stack_Edit_acc_2->setChecked(true);
            ui->cb_Stack_Edit_DSpeed_2->setChecked(false);
        }
        else if(StackMove->advCSpeedFlag[AxisIndex] == 2)
        {
            ui->cb_Stack_Edit_acc_2->setChecked(false);
            ui->cb_Stack_Edit_DSpeed_2->setChecked(true);
        }
        double AdvPos = StackMove->advEndDis[AxisIndex];
        ui->Stack_Edit_AdvPos_2->setText(QString::number(AdvPos/100,'f',2));
        ui->Stack_Edit_AdvSpeed_2->setText(QString::number(StackMove->advCSpeedSpeed[AxisIndex]));
    }
    else if(m_StackInfo[StackMove->stackNum-1].stackPointNum[AxisIndex] > 0 && AxisIndex == STACK_Y_AXIS)
    {
        ui->stkWgtStack->setCurrentWidget(ui->Stack_Edit_pageY1);
        double stackstartPos = m_StackInfo[StackMove->stackNum-1].stackStartPos[AxisIndex];
        double stackDelay = m_OperateProOrder[m_CurrentSelectProOrderList].delay;
        ui->Stack_Edit_Pos_3->setText(QString::number(stackstartPos/100,'f',2));
        ui->Stack_Edit_Speed_3->setText(QString::number(m_StackInfo[StackMove->stackNum-1].axisSpeed[AxisIndex]));
        ui->Stack_Edit_Delay_3->setText(QString::number(stackDelay/100,'f',2));
        ui->cb_Stack_Edit_end_3->setChecked(StackMove->advEndFlag[AxisIndex]);
        if(StackMove->advCSpeedFlag[AxisIndex] == 0)
        {
            ui->cb_Stack_Edit_acc_3->setChecked(false);
            ui->cb_Stack_Edit_DSpeed_3->setChecked(false);
        }
        else if(StackMove->advCSpeedFlag[AxisIndex] == 1)
        {
            ui->cb_Stack_Edit_acc_3->setChecked(true);
            ui->cb_Stack_Edit_DSpeed_3->setChecked(false);
        }
        else if(StackMove->advCSpeedFlag[AxisIndex] == 2)
        {
            ui->cb_Stack_Edit_acc_3->setChecked(false);
            ui->cb_Stack_Edit_DSpeed_3->setChecked(true);
        }
        double AdvPos = StackMove->advEndDis[AxisIndex];
        ui->Stack_Edit_AdvPos_3->setText(QString::number(AdvPos/100,'f',2));
        ui->Stack_Edit_AdvSpeed_3->setText(QString::number(StackMove->advCSpeedSpeed[AxisIndex]));
    }

}
//指令编辑处理函数
void Teach::OrderEdit_Handle()
{
//    ui->tableWgtTeach->setCurrentCell(m_CurrentSelectProOrderList,0);
    if(m_CurrentSelectProOrderList<=ui->tableWgtTeach->rowCount())
    {
        ui->tableWgtTeach->selectRow(m_CurrentSelectProOrderList);
    }
    switch(m_OperateProOrder[m_CurrentSelectProOrderList].cmd)
    {
    case C_AXIS_MOVE:
    {
        ui->stkWgtProgram->setCurrentWidget(ui->pageAxisAction);
        P_AxisMoveStruct* AxisMove = (P_AxisMoveStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
        double AxisPos = AxisMove->pos;
        double advEndDis = AxisMove->advEndDis;
        double AxisMove_delay = m_OperateProOrder[m_CurrentSelectProOrderList].delay;
        ui->lineEdit_Edit_AxisMove_pos->setText(QString::number(AxisPos/100,'f',2));
        ui->lineEdit_Edit_AxisMove_speed->setText(QString::number(AxisMove->speed,'f',0));
        ui->lineEdit_Edit_AxisMove_delay->setText(QString::number(AxisMove_delay/100,'f',2));
        ui->cb_Edit_AxisMove_early_End->setChecked(AxisMove->advEndFlag);
        if(AxisMove->advCSpeedFlag == 0)
        {
            ui->cb_Edit_AxisMove_early_acc_speed->setChecked(false);
            ui->cb_Edit_AxisMove_early_Slow_speed->setChecked(false);
        }
        else if(AxisMove->advCSpeedFlag == 1)
        {
            ui->cb_Edit_AxisMove_early_acc_speed->setChecked(true);
            ui->cb_Edit_AxisMove_early_Slow_speed->setChecked(false);
        }
        else if(AxisMove->advCSpeedFlag == 2)
        {
            ui->cb_Edit_AxisMove_early_acc_speed->setChecked(false);
            ui->cb_Edit_AxisMove_early_Slow_speed->setChecked(true);
        }
        ui->lineEdit_Edit_AxisMove_early_speed->setText(QString::number(AxisMove->advCSpeedSpeed,'f',0));
        ui->lineEdit_Edit_AxisMove_early_pos->setText(QString::number(advEndDis/100,'f',2));
        if(ui->editReferPoint->getRefPointIndex() == 0)
        {
            ui->editReferPoint->setText("");
        }
        else
        {
            ui->editReferPoint->setText(tr("参考点")+QString::number(AxisMove->referPointNum));
        }
        break;
    }
    case C_CLAW_ACTION:
    {
        ui->stkWgtProgram->setCurrentWidget(ui->pageClawAction);
        P_ClawActionStruct* ClawAction =(P_ClawActionStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
        double ClawAction_delay = m_OperateProOrder[m_CurrentSelectProOrderList].delay;
        if(ClawAction->type == 0)
        {
            ui->ClawRelease->setChecked(true);
        }
        else if(ClawAction->type ==1)
        {
            ui->ClawClamp->setChecked(true);
        }
        ui->ClawDelay->setText(QString::number(ClawAction_delay/100,'f',2));
        break;
    }
    case C_STACK_MOVE:
    {
        Stack_Edit_Handle(STACK_X_AXIS);
        break;
    }
    case C_STACK_FOLLOW:
    {
        ui->stkWgtProgram->setCurrentWidget(ui->pageStack);
        P_StackFollowStruct* StackFollow =(P_StackFollowStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
        break;
    }
    case C_RESERVE_OUT:
    {
        ui->stkWgtProgram->setCurrentWidget(ui->pageReserveOut);
        P_ReserveOutStruct* ReserveOut =(P_ReserveOutStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
        double delay_time = m_OperateProOrder[m_CurrentSelectProOrderList].delay;
        ui->lineEdit_Edit_ReserveOut_Num->setText(QString::number(ReserveOut->interval));
        ui->lineEdit_Edit_ReserveOut_time->setText(QString::number(delay_time/100,'f',2));
        break;
    }
    case C_WAIT_IN_MACHINE:
    {
        ui->stkWgtProgram->setCurrentWidget(ui->pageSigWait);
        P_WaitInMachineStruct* WaitInMachine =(P_WaitInMachineStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
        double wait_time = m_OperateProOrder[m_CurrentSelectProOrderList].delay;
        ui->lineEdit_Edit_Wait_time->setText(QString::number(wait_time/100,'f',2));
        ui->lineEdit_Edit_Wait_step->setText(QString::number(WaitInMachine->backListNum));
        ui->comboBox_Edit_Wait_label->setCurrentIndex(WaitInMachine->label);
        if(WaitInMachine->type == 0)
        {
            ui->checkBox_Edit_Wait_step->setChecked(true);
        }
        else if(WaitInMachine->type == 1)
        {
            ui->checkBox_Edit_Wait_label->setChecked(true);
        }
        break;
    }
    case C_WAIT_IN_CLAW:
    {
        ui->stkWgtProgram->setCurrentWidget(ui->pageSigWait);
        P_WaitInClawStruct* WaitInClaw =(P_WaitInClawStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
        double wait_time = m_OperateProOrder[m_CurrentSelectProOrderList].delay;
        ui->lineEdit_Edit_Wait_time->setText(QString::number(wait_time/100,'f',2));
        ui->lineEdit_Edit_Wait_step->setText(QString::number(WaitInClaw->backListNum));
        ui->comboBox_Edit_Wait_label->setCurrentIndex(0);
        QStringList labelText;
        ui->comboBox_Edit_Wait_label->clear();
        ui->comboBox_Edit_Wait_label->addItem("无标签");
        for(int i=0;i<CurrentLableNameList.count();i++)
        {
            labelText = CurrentLableNameList[i].split(signalSpace);
            if(labelText.length()>1)
            {
                ui->comboBox_Edit_Wait_label->addItem(labelText[0]);
            }
        }
        if(WaitInClaw->type == 0)
        {
            ui->lineEdit_Edit_Wait_step->setText(QString::number(WaitInClaw->backListNum));
            ui->checkBox_Edit_Wait_step->setChecked(true);
        }
        else if(WaitInClaw->type == 1)
        {
            ui->checkBox_Edit_Wait_label->setChecked(true);
            ui->comboBox_Edit_Wait_label->setCurrentIndex(WaitInClaw->label);
        }
        break;
    }
    case C_WAIT_IN_RESERVE:
    {
        ui->stkWgtProgram->setCurrentWidget(ui->pageSigWait_2);
        P_WaitInReserveStruct* WaitInReserve =(P_WaitInReserveStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
        double wait_time = m_OperateProOrder[m_CurrentSelectProOrderList].delay;
        ui->lineEdit_Edit_Wait_time_2->setText(QString::number(wait_time/100,'f',2));
        ui->lineEdit_Edit_Wait_step_2->setText(QString::number(WaitInReserve->backListNum));
        ui->comboBox_Edit_Wait_label_2->setCurrentIndex(0);
        QStringList labelText;
        ui->comboBox_Edit_Wait_label_2->clear();
        ui->comboBox_Edit_Wait_label_2->addItem("无标签");
        for(int i=0;i<CurrentLableNameList.count();i++)
        {
            labelText = CurrentLableNameList[i].split(signalSpace);
            if(labelText.length()>1)
            {
                ui->comboBox_Edit_Wait_label_2->addItem(labelText[0]);
            }
        }
        if(WaitInReserve->type == 0)
        {
            ui->lineEdit_Edit_Wait_step_2->setText(QString::number(WaitInReserve->backListNum));
            ui->checkBox_Edit_Wait_step_2->setChecked(true);
        }
        else if(WaitInReserve->type == 1)
        {
            ui->checkBox_Edit_Wait_label_2->setChecked(true);
            ui->comboBox_Edit_Wait_label_2->setCurrentIndex(WaitInReserve->label);
        }
        ui->checkBox_Edit_Wait_001s->setChecked(true);
        ui->lineEdit_Edit_Wait_time_2->setDecimalPlaces(2);
        break;
    }
    case C_STACK_RESET_ZERO:
    case C_CLAW_CHECK:
    case C_RESERVE_CHECK:
    case C_MACHINE_OUT:
    case C_OTHER_DELAY:
    case C_OTHER_ALARM_LAMP:
    case C_OTHER_ALARM_SOUND:
    case C_AXIS_STOP:
    case C_SEARCH_STOP:
    {
        double OtherDelay = m_OperateProOrder[m_CurrentSelectProOrderList].delay;
        ui->stkWgtProgram->setCurrentWidget(ui->pageDelayTime);
        ui->lineEdit_OtherDelay->setText(QString::number(OtherDelay/100,'f',2));
        break;
    }
    case C_OTHER_ALARM_CUST:
    {
        ui->stkWgtProgram->setCurrentWidget(ui->pageAlarmDef);
        P_OtherAlarmCustStruct* OtherAlarmCust =(P_OtherAlarmCustStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
        ui->checkBox_fault_num->setChecked(false);
        ui->checkBox_tips_num->setChecked(false);
        if(OtherAlarmCust->type==4)
        {
            ui->checkBox_fault_num->setChecked(true);
            ui->lineEdit_edit_fault_num->setText(QString::number(OtherAlarmCust->alarmNum));
        }
        else if(OtherAlarmCust->type == 5)
        {
            ui->checkBox_tips_num->setChecked(true);
            ui->lineEdit_edit_tips_num->setText(QString::number(OtherAlarmCust->alarmNum));
        }
        break;
    }
    case C_OTHER_CYC_STOP:
    {
        ui->stkWgtProgram->setCurrentWidget(ui->pageNone);
        break;
    }
    case C_LABEL:
    {
        ui->stkWgtProgram->setCurrentWidget(ui->pageLabelEdit);
        P_LabelStruct* LabelStruct =(P_LabelStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
        if(LabelStruct->type == 0)
        {//标签指令
            ui->jumpLabel->hide();
            ui->listWidget_LabelEdit->hide();
            ui->labCommentName->show();
            ui->label_Edit->show();
            if(LabelStruct->labelNum>0)
            {
                uint16_t ListIndex = ReturnLableListIndex(LabelStruct->labelNum);
                if(ListIndex>0)
                {
                    QStringList labelText = CurrentLableNameList[ListIndex].split(signalSpace);
                    if(labelText.length()>1)
                    {
                        ui->label_Edit->setText(labelText[1]);
                        ui->labCommentName->setText(labelText[0]);
                    }
                }

            }
        }
        else if(LabelStruct->type == 1)
        {//标签跳转
            ui->jumpLabel->show();
            ui->listWidget_LabelEdit->show();
            ui->labCommentName->hide();
            ui->label_Edit->hide();
//            Refresh_listWgtJumpto();
            ui->listWidget_LabelEdit->clear();
            for(int i=0; i<CurrentLableNameList.count();i++)
            {
                QListWidgetItem *item = new QListWidgetItem();
                QCheckBox *checkbox = new QCheckBox();
                checkbox->setChecked(false);
                checkbox->setAutoExclusive(true);//check为互斥
                checkbox->setText(CurrentLableNameList[i]);
                checkbox->setFixedSize(ui->listWidget_LabelEdit->width(),37);
                ui->listWidget_LabelEdit->addItem(item);
                ui->listWidget_LabelEdit->setItemWidget(item,checkbox);
                connect(checkbox, &QCheckBox::toggled, this, [this, checkbox](bool checked){
                    onCheckBoxToggled(checkbox, checked);
                    });
                if(LabelStruct->labelNum == i+1)
                {
                    checkbox->setChecked(true);
                }
            }


        }
        break;
    }
    case C_LOGIC_IF:
    {
        ui->stkWgtProgram->setCurrentWidget(ui->pageIf);
        P_LogicIfStruct* LogicIf =(P_LogicIfStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
        ui->chboxCondition2If->setChecked(false);
        ui->chboxCondition1If->setChecked(true);
        Edit_ifOrder_handle(LogicIf,0);
        if(LogicIf->reqSelectFlag[0] == 0)
        {
            ui->chboxCondition1If->setEnabled(true);
            ui->chboxCondition2If->setEnabled(false);
        }
        else
        {
            ui->chboxCondition1If->setEnabled(true);
            ui->chboxCondition2If->setEnabled(true);
        }
        break;
    }
    case C_LOGIC_ELSE:
        ui->stkWgtProgram->setCurrentWidget(ui->pageNone);
        break;
    case C_LOGIC_END:
    case C_PRO_END:
        ui->stkWgtProgram->setCurrentWidget(ui->pageNone);
        break;
    case C_LOGIC_WHILE_START:
    {
        ui->stkWgtProgram->setCurrentWidget(ui->pageLoop);
        P_LogicWhileStartStruct* LogicWhileStart =(P_LogicWhileStartStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
        ui->lineEdit_Loop_num->setText(QString::number(LogicWhileStart->cycNum));
        break;
    }
    case C_LOGIC_WHILE_END:
        ui->stkWgtProgram->setCurrentWidget(ui->pageNone);
        break;
    case C_LOGIC_VAR:
    {
        ui->stkWgtProgram->setCurrentWidget(ui->pageVarOperation);
        ui->Var_page->setCurrentWidget(ui->Var_Var_page);
        P_LogicVarStruct* LogicVar =(P_LogicVarStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;

        ui->comboBox_Edit_Var->setCurrentIndex(LogicVar->varNum-1);
        ui->comboBox_Edit_Var->setEnabled(false);
        if(LogicVar->sufferOperType == 0)
        {
            ui->Edit_Var_const->setChecked(true);
            ui->comboBox_Edit_Var_Oper->setCurrentIndex(LogicVar->operMode);
            ui->comboBox_Edit_Var_Oper->setEnabled(true);
            ui->lineEdit_Edit_Var_const->show();
            ui->comboBox_Edit_Var_Var->hide();
            ui->comboBox_Edit_Var_Axis->hide();
            ui->comboBox_Edit_Var_Prod->hide();
            ui->lineEdit_Edit_Var_const->setDecimalPlaces(m_VariableType[ui->comboBox_Edit_Var->currentIndex()]);
            double sufferOperValue = LogicVar->sufferOperValue;
            if(m_VariableType[ui->comboBox_Edit_Var->currentIndex()]==0)
            {
                ui->lineEdit_Edit_Var_const->setText(QString::number(sufferOperValue));
            }
            else if(m_VariableType[ui->comboBox_Edit_Var->currentIndex()]==1)
            {
                ui->lineEdit_Edit_Var_const->setText(QString::number(sufferOperValue/10,'f',1));
            }
            else if(m_VariableType[ui->comboBox_Edit_Var->currentIndex()]==2)
            {
                ui->lineEdit_Edit_Var_const->setText(QString::number(sufferOperValue/100,'f',2));
            }
        }
        else if(LogicVar->sufferOperType>=1 && LogicVar->sufferOperType<=20)
        {
            ui->Edit_Var_var->setChecked(true);
            ui->comboBox_Edit_Var_Oper->setCurrentIndex(4);
            ui->comboBox_Edit_Var_Oper->setEnabled(false);
            ui->lineEdit_Edit_Var_const->hide();
            ui->comboBox_Edit_Var_Var->show();
            ui->comboBox_Edit_Var_Axis->hide();
            ui->comboBox_Edit_Var_Prod->hide();
            ui->comboBox_Edit_Var_Var->setCurrentIndex(LogicVar->sufferOperType-1);
        }
        else if(LogicVar->sufferOperType>=21 && LogicVar->sufferOperType<=40)
        {
            ui->Edit_Var_Axis->setChecked(true);
            ui->comboBox_Edit_Var_Oper->setCurrentIndex(4);
            ui->comboBox_Edit_Var_Oper->setEnabled(false);
            ui->lineEdit_Edit_Var_const->hide();
            ui->comboBox_Edit_Var_Var->hide();
            ui->comboBox_Edit_Var_Axis->show();
            ui->comboBox_Edit_Var_Prod->hide();
            ui->comboBox_Edit_Var_Var->setCurrentIndex(LogicVar->sufferOperType-21);
        }
        else if(LogicVar->sufferOperType==101)
        {
            ui->Edit_Var_Prod->setChecked(true);
            ui->comboBox_Edit_Var_Oper->setCurrentIndex(4);
            ui->comboBox_Edit_Var_Oper->setEnabled(false);
            ui->lineEdit_Edit_Var_const->hide();
            ui->comboBox_Edit_Var_Var->hide();
            ui->comboBox_Edit_Var_Axis->hide();
            ui->comboBox_Edit_Var_Prod->show();
            ui->comboBox_Edit_Var_Var->setCurrentIndex(LogicVar->sufferOperType-101);
        }
        break;
    }
    case C_LOGIC_AXIS:
    {
        ui->stkWgtProgram->setCurrentWidget(ui->pageVarOperation);
        ui->Var_page->setCurrentWidget(ui->Var_Axis_page);
        P_LogicAxisStruct* LogicAxis =(P_LogicAxisStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
        if(LogicAxis->axisNum>0)
        {
            if(m_AxisPar[LogicAxis->axisNum-1].axisType == 1)
            {//m_NameDefine[1].axisName[i] &&
                for(int i=0;i<ui->comboBox_Edit_Axis->count();i++)
                {
                    if(ui->comboBox_Edit_Axis->itemText(i) == m_NameDefine[1].axisName[LogicAxis->axisNum-1])
                    {
                        ui->comboBox_Edit_Axis->setCurrentIndex(i);
                    }
                }
            }
            else
            {
                ui->comboBox_Edit_Axis->setEditText(m_NameDefine[1].axisName[LogicAxis->axisNum-1]);
            }
        }
//        ui->comboBox_Edit_Axis->setCurrentIndex(LogicAxis->axisNum);
        ui->comboBox_Edit_Axis->setEnabled(false);
        if(LogicAxis->sufferOperType == 0)
        {
            ui->Edit_Axis_const->setChecked(true);
            ui->comboBox_Edit_Axis_Oper->setCurrentIndex(LogicAxis->operMode);
            ui->comboBox_Edit_Axis_Oper->setEnabled(true);
            ui->lineEdit_Edit_Axis_const->show();
            ui->comboBox_Edit_Axis_Var->hide();
            ui->lineEdit_Edit_Axis_const->setDecimalPlaces(2);
            double sufferOperValue = LogicAxis->sufferOperValue;
            ui->lineEdit_Edit_Axis_const->setText(QString::number(sufferOperValue/100,'f',2));
        }
        else if(LogicAxis->sufferOperType>=1 && LogicAxis->sufferOperType<=20)
        {
            ui->Edit_Axis_var->setChecked(true);
            ui->comboBox_Edit_Axis_Oper->setCurrentIndex(4);
            ui->comboBox_Edit_Axis_Oper->setEnabled(false);
            ui->lineEdit_Edit_Axis_const->hide();
            ui->comboBox_Edit_Axis_Var->show();
            ui->comboBox_Edit_Axis_Var->setCurrentIndex(LogicAxis->sufferOperType-1);
        }
        break;
    }
    case C_LOGIC_STACK:
    {

        ui->stkWgtProgram->setCurrentWidget(ui->pageVarOperation);
        ui->Var_page->setCurrentWidget(ui->Var_Stack_page);
        P_LogicStackStruct* LogicStack =(P_LogicStackStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
        ui->comboBox_Edit_Stack_num->setCurrentIndex(LogicStack->stackNum);
        ui->comboBox_Edit_Stack_num->setEnabled(false);
        if(LogicStack->sufferOperType == 0)
        {
            ui->Edit_Stack_const->setChecked(true);
            ui->comboBox_Edit_Stack_Oper->setCurrentIndex(LogicStack->operMode);
            ui->comboBox_Edit_Stack_Oper->setEnabled(true);
            ui->lineEdit_Edit_Stack_const->show();
            ui->comboBox_Edit_Stack_num1->hide();
            ui->lineEdit_Edit_Stack_const->setDecimalPlaces(0);
            ui->lineEdit_Edit_Stack_const->setText(QString::number(LogicStack->sufferOperValue));
        }
        else if(LogicStack->sufferOperType>=1 && LogicStack->sufferOperType<=20)
        {
            ui->Edit_Stack_num->setChecked(true);
            ui->comboBox_Edit_Stack_Oper->setCurrentIndex(4);
            ui->comboBox_Edit_Stack_Oper->setEnabled(false);
            ui->lineEdit_Edit_Stack_const->hide();
            ui->comboBox_Edit_Stack_num1->show();
            ui->comboBox_Edit_Stack_num1->setCurrentIndex(LogicStack->sufferOperType+1);
        }
        break;
    }
    case C_LOGIC_PRODUCT:
    {
        ui->stkWgtProgram->setCurrentWidget(ui->pageVarOperation);
        ui->Var_page->setCurrentWidget(ui->Var_Prod_page);
        P_LogicCurProductNumStruct* LogicCurProductNum =(P_LogicCurProductNumStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
        ui->comboBox_Edit_Prod_num->setCurrentIndex(LogicCurProductNum->productNum-1);
        ui->comboBox_Edit_Prod_num->setEnabled(false);
        if(LogicCurProductNum->sufferOperType == 0)
        {
            ui->Edit_Prod_const->setChecked(true);
            ui->comboBox_Edit_Prod_Oper->setCurrentIndex(LogicCurProductNum->operMode);
            ui->comboBox_Edit_Prod_Oper->setEnabled(true);
            ui->lineEdit_Edit_Prod_const->show();
            ui->comboBox_Edit_Prod_Var->hide();
            ui->lineEdit_Edit_Prod_const->setDecimalPlaces(0);
            ui->lineEdit_Edit_Prod_const->setText(QString::number(LogicCurProductNum->sufferOperValue));
        }
        else if(LogicCurProductNum->sufferOperType>=1 && LogicCurProductNum->sufferOperType<=20)
        {
            ui->Edit_Prod_var->setChecked(true);
            ui->comboBox_Edit_Prod_Oper->setCurrentIndex(4);
            ui->comboBox_Edit_Prod_Oper->setEnabled(false);
            ui->lineEdit_Edit_Prod_const->hide();
            ui->comboBox_Edit_Prod_Var->show();
            ui->comboBox_Edit_Prod_Var->setCurrentIndex(LogicCurProductNum->sufferOperType+1);
        }
        break;
    }
    case C_LOGIC_TIME:
        ui->stkWgtProgram->setCurrentWidget(ui->pageNone);
        break;
    case C_SEARCH_AXIS_MOVE:
    {
        ui->stkWgtProgram->setCurrentWidget(ui->pageSearch);
        P_SearchAxisMoveStruct* SearchAxisMove =(P_SearchAxisMoveStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
        double Search_delay = m_OperateProOrder[m_CurrentSelectProOrderList].delay;
        double maxPos = SearchAxisMove->maxPos;
        double advpos = SearchAxisMove->advCDis;
        double offseet_pos = SearchAxisMove->offsetDis;
        ui->lineEdit_Search_delay->setText(QString::number(Search_delay/100,'f',2));
        ui->lineEdit_Search_Mix_Pos->setText(QString::number(maxPos/100,'f',2));
        ui->lineEdit_Search_runspeed->setText(QString::number(SearchAxisMove->runSpeed));
        ui->lineEdit_Search_AdvPos->setText(QString::number(advpos/100,'f',2));
        ui->lineEdit_Search_Adv_speed->setText(QString::number(SearchAxisMove->advCSpeed));
        ui->lineEdit_Search_offseet_pos->setText(QString::number(offseet_pos/100,'f',2));
        QString SearchAxisText =Get_XYPort_Name(m_OperateProOrder[m_CurrentSelectProOrderList]);
        ui->lineEdit_Search_stop->setText(SearchAxisText);
        ui->lineEdit_Search_stop->setCurrentPort(SearchAxisMove->inportNum[0]);
        if(SearchAxisMove->posStoreFlag == 1)
        {
            ui->checkBox_PosMemory->setChecked(true);
        }
        else if(SearchAxisMove->posStoreFlag == 0)
        {
            ui->checkBox_PosMemory->setChecked(false);
        }
        if(SearchAxisMove->reachPosAlarmFlag == 1)
        {
            ui->checkBox_Edit_Arrive_alarm->setChecked(true);
        }
        else if(SearchAxisMove->reachPosAlarmFlag == 0)
        {
            ui->checkBox_Edit_Arrive_alarm->setChecked(false);
        }
        break;
    }
    case C_OFFSET_AXIS:
    {
        ui->stkWgtProgram->setCurrentWidget(ui->pageOffset);
        P_OffsetAxisStruct* OffsetAxis =(P_OffsetAxisStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
        double offsetPos = OffsetAxis->offsetPos;
        double advCSpeedDis = OffsetAxis->advCSpeedDis;
        double delay = m_OperateProOrder[m_CurrentSelectProOrderList].delay;
        ui->lineEdit_Edit_offsetpos->setText(QString::number(offsetPos/100,'f',2));
        ui->lineEdit_edit_offsetspeed->setText(QString::number(OffsetAxis->speed));
        ui->lineEdit_Edit_offsetDelay->setText(QString::number(delay,'f',2));
        ui->lineEdit_Edit_offset_advenddis->setText(QString::number(advCSpeedDis/100,'f',2));
        ui->lineEdit_Edit_offset_advCspeed->setText(QString::number(OffsetAxis->advCSpeedSpeed));
        if(OffsetAxis->advEndFlag == 0)
        {
            ui->Edit_offset_advendflag->setChecked(false);
        }
        else if(OffsetAxis->advEndFlag == 1)
        {
            ui->Edit_offset_advendflag->setChecked(true);
        }
        if(OffsetAxis->advCSpeedFlag == 0)
        {
            ui->Edit_offset_acc->setChecked(false);
            ui->Edit_offset_down_speed->setChecked(false);
        }
        else if(OffsetAxis->advCSpeedFlag == 1)
        {
            ui->Edit_offset_acc->setChecked(true);
            ui->Edit_offset_down_speed->setChecked(false);
        }
        else if(OffsetAxis->advCSpeedFlag == 2)
        {
            ui->Edit_offset_acc->setChecked(false);
            ui->Edit_offset_down_speed->setChecked(true);
        }
        break;
    }
    case C_TORQUE_GARD:
    {
        ui->stkWgtProgram->setCurrentWidget(ui->pageTorqueProtect);
        P_TorqueGardStruct* TorqueGard =(P_TorqueGardStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
        ui->lineEdit_Edit_torque_value->setText(QString::number(TorqueGard->torqueValue));
        if(m_AxisPar[TorqueGard->axis].axisType == 1)
        {
            for(int i=0;i<ui->comboBox_Edit_torque_axis->count();i++)
            {
                if(ui->comboBox_Edit_torque_axis->itemText(i) == m_NameDefine[1].axisName[TorqueGard->axis])
                {
                    ui->comboBox_Edit_torque_axis->setCurrentIndex(i);
                }
            }
        }
        else
        {
            ui->comboBox_Edit_torque_axis->setEditText(m_NameDefine[1].axisName[TorqueGard->axis]);
        }

        break;
    }
    case C_SUN_PRO:
    {

        P_SunProStruct* SunPro =(P_SunProStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
        if(SunPro->oprMode == 3)
        {
            double OtherDelay = m_OperateProOrder[m_CurrentSelectProOrderList].delay;
            ui->stkWgtProgram->setCurrentWidget(ui->pageDelayTime);
            ui->lineEdit_OtherDelay->setText(QString::number(OtherDelay/100,'f',2));
        }
        else
        {
            ui->stkWgtProgram->setCurrentWidget(ui->pageSubProgEdit);
            ui->coboxSubProgSelectEdit->setCurrentIndex(SunPro->sunProNum-1);
        }

        break;
    }
    default:
        break;
    }
}
void Teach::on_cb_Edit_AxisMove_early_Slow_speed_clicked()
{
    if(ui->cb_Edit_AxisMove_early_acc_speed->isChecked())
    {
        ui->cb_Edit_AxisMove_early_acc_speed->setChecked(false);
    }
}


void Teach::on_cb_Edit_AxisMove_early_acc_speed_clicked()
{
    if(ui->cb_Edit_AxisMove_early_Slow_speed->isChecked())
    {
        ui->cb_Edit_AxisMove_early_Slow_speed->setChecked(false);
    }
}

/*************************************************************************
**  函数名：  Edit_ifOrder_handle()
**	输入参数：LogicIf:if结构体参数，IfIndex:0-条件1，1-条件2
**	输出参数：
**	函数功能：如果指令编辑处理函数
**  作者：    wukui
**  开发日期：2024/9/2
**************************************************************************/
void Teach::Edit_ifOrder_handle(P_LogicIfStruct* LogicIf,uint8_t IfIndex)
{
    if(LogicIf->cmpType[IfIndex] == 0)
    {//输入信号
        ui->stkWgtIf->setCurrentWidget(ui->pageSigIn);
        if(LogicIf->inportNum[IfIndex]>=1 && LogicIf->inportNum[IfIndex]<=12)
        {
            ui->chboxMainBoardIn0->setChecked(true);
            ui->btnMainBoardIn0->setState(LogicIf->inportType[IfIndex]);
            ui->coboxMainBoardIn0->setCurrentIndex(LogicIf->inportNum[IfIndex]-1);
        }
        else if(LogicIf->inportNum[IfIndex]>=13 && LogicIf->inportNum[IfIndex]<=24)
        {
            ui->chboxMainBoardIn1->setChecked(true);
            ui->btnMainBoardIn1->setState(LogicIf->inportType[IfIndex]);
            ui->coboxMainBoardIn1->setCurrentIndex(LogicIf->inportNum[IfIndex]-13);
        }
        else if(LogicIf->inportNum[IfIndex]>=25 && LogicIf->inportNum[IfIndex]<=36)
        {
            ui->chboxMainBoardIn2->setChecked(true);
            ui->btnMainBoardIn2->setState(LogicIf->inportType[IfIndex]);
            ui->coboxMainBoardIn2->setCurrentIndex(LogicIf->inportNum[IfIndex]-25);
        }
        else if(LogicIf->inportNum[IfIndex]>=37 && LogicIf->inportNum[IfIndex]<=48)
        {
            ui->chboxMainBoardInExtend0->setChecked(true);
            ui->btnBoardInExtend0->setState(LogicIf->inportType[IfIndex]);
            ui->coboxMainBoardInExtend0->setCurrentIndex(LogicIf->inportNum[IfIndex]-37);
        }
        else if(LogicIf->inportNum[IfIndex]>=49 && LogicIf->inportNum[IfIndex]<=60)
        {
            ui->chboxMainBoardInExtend1->setChecked(true);
            ui->btnBoardInExtend1->setState(LogicIf->inportType[IfIndex]);
            ui->coboxMainBoardInExtend1->setCurrentIndex(LogicIf->inportNum[IfIndex]-49);
        }

    }
    else if(LogicIf->cmpType[IfIndex] == 103)
    {//输出信号
        ui->stkWgtIf->setCurrentWidget(ui->pageSigOut);
        if(LogicIf->inportNum[IfIndex]>=1 && LogicIf->inportNum[IfIndex]<=8)
        {
            ui->chboxMainBoardOut0->setChecked(true);
            ui->btnMainBoardOut0->setState(LogicIf->inportType[IfIndex]);
            ui->coboxMainBoardOut0->setCurrentIndex(LogicIf->inportNum[IfIndex]-1);
        }
        else if(LogicIf->inportNum[IfIndex]>=9 && LogicIf->inportNum[IfIndex]<=16)
        {
            ui->chboxMainBoardOut1->setChecked(true);
            ui->btnMainBoardOut1->setState(LogicIf->inportType[IfIndex]);
            ui->coboxMainBoardOut1->setCurrentIndex(LogicIf->inportNum[IfIndex]-9);
        }
        else if(LogicIf->inportNum[IfIndex]>=17 && LogicIf->inportNum[IfIndex]<=24)
        {
            ui->chboxMainBoardOut2->setChecked(true);
            ui->btnMainBoardOut2->setState(LogicIf->inportType[IfIndex]);
            ui->coboxMainBoardOut2->setCurrentIndex(LogicIf->inportNum[IfIndex]-17);
        }
        else if(LogicIf->inportNum[IfIndex]>=25 && LogicIf->inportNum[IfIndex]<=44)
        {
            ui->chboxMainBoardOutExtend0->setChecked(true);
            ui->btnBoardOutExtend0->setState(LogicIf->inportType[IfIndex]);
            ui->coboxMainBoardOutExtend0->setCurrentIndex(LogicIf->inportNum[IfIndex]-25);
        }
    }
    else if(LogicIf->cmpType[IfIndex] >=1 && LogicIf->cmpType[IfIndex] <=20)
    {//变量
        ui->stkWgtIf->setCurrentWidget(ui->pageVar);
        ui->coboxVarDisplay->setCurrentIndex(LogicIf->cmpType[IfIndex]-1);
        ui->coboxVarDisplay->setEnabled(false);
        ui->coboxVarOperator->setCurrentIndex(LogicIf->cmpMode[IfIndex]);
        if(LogicIf->sufferCmpType[IfIndex] == 0)
        {//被比较对象类型-常量
            ui->editVarIf->show();
            ui->coboxVarSelectIf->hide();
            ui->coboxVarRealProductNum->hide();
            ui->coboxVarStack->hide();
            double CmpValue = LogicIf->sufferCmpValue[IfIndex];
            if(m_VariableType[ui->coboxVarDisplay->currentIndex()]==0)
            {
                ui->editVarIf->setText(QString::number(CmpValue));
            }
            else if(m_VariableType[ui->coboxVarDisplay->currentIndex()]==1)
            {
                ui->editVarIf->setText(QString::number(CmpValue/10,'f',1));
            }
            else if(m_VariableType[ui->coboxVarDisplay->currentIndex()]==2)
            {
                ui->editVarIf->setText(QString::number(CmpValue/100,'f',2));
            }
        }
        else if(LogicIf->sufferCmpType[IfIndex] >= 1 && LogicIf->sufferCmpType[IfIndex] <= 20)
        {//被比较对象类型-变量
            ui->editVarIf->hide();
            ui->coboxVarSelectIf->show();
            ui->coboxVarRealProductNum->hide();
            ui->coboxVarStack->hide();
            ui->coboxVarSelectIf->setCurrentIndex(LogicIf->sufferCmpType[IfIndex]-1);
        }
        else if(LogicIf->sufferCmpType[IfIndex] >= 102)
        {//被比较对象类型-实际产量
            ui->editVarIf->hide();
            ui->coboxVarSelectIf->hide();
            ui->coboxVarRealProductNum->show();
            ui->coboxVarStack->hide();
            ui->coboxVarRealProductNum->setCurrentIndex(LogicIf->sufferCmpType[IfIndex]-102);
        }
        if(ui->coboxVarDisplay->currentIndex()>VAR_TOTAL_NUM)
        {//防止溢出
            ui->coboxVarDisplay->setCurrentIndex(0);
        }
        ui->editVarIf->setDecimalPlaces(m_VariableType[ui->coboxVarDisplay->currentIndex()]);//根据变量类型设置控件小数位数
    }
    else if(LogicIf->cmpType[IfIndex]>=21 && LogicIf->cmpType[IfIndex] <= 60)
    {//堆叠组方式1/2
        ui->stkWgtIf->setCurrentWidget(ui->pageOtherIf);
        if(LogicIf->cmpType[IfIndex]>=41)
        {
            ui->coboxOtherDisplay->setCurrentIndex(LogicIf->cmpType[IfIndex]-41);
        }
        else
        {
            ui->coboxOtherDisplay->setCurrentIndex(LogicIf->cmpType[IfIndex]-21);
        }
        ui->coboxOtherDisplay->setEnabled(false);
        if(LogicIf->sufferCmpType[IfIndex] == 0)
        {
            ui->editOther->show();
            ui->coboxOtherSelect->hide();
            ui->coboxOtherSelect_stackFinish->hide();
            ui->editOther->setText(QString::number(LogicIf->sufferCmpValue[IfIndex]));
            ui->coboxOperatorOther->show();
            ui->coboxOperatorOther_2->hide();
            ui->coboxOperatorOther->setCurrentIndex(LogicIf->cmpMode[IfIndex]);
        }
        else if(LogicIf->sufferCmpType[IfIndex]>=21 &&LogicIf->sufferCmpType[IfIndex]<=40)
        {
            ui->editOther->hide();
            ui->coboxOtherSelect->show();
            ui->coboxOtherSelect_stackFinish->hide();
            ui->coboxOtherSelect->setCurrentIndex(LogicIf->sufferCmpType[IfIndex]-21);
            ui->coboxOperatorOther->show();
            ui->coboxOperatorOther_2->hide();
            ui->coboxOperatorOther->setCurrentIndex(LogicIf->cmpMode[IfIndex]);
        }
        else if(LogicIf->sufferCmpType[IfIndex] == 103)
        {
            ui->editOther->hide();
            ui->coboxOtherSelect->hide();
            ui->coboxOtherSelect_stackFinish->show();
            ui->coboxOtherSelect_stackFinish->setCurrentIndex(LogicIf->sufferCmpType[IfIndex]-103);
            ui->coboxOperatorOther->hide();
            ui->coboxOperatorOther_2->show();
            ui->coboxOperatorOther_2->setCurrentIndex(LogicIf->cmpMode[IfIndex]-4);
        }
    }
    else if(LogicIf->cmpType[IfIndex]>=41 && LogicIf->cmpType[IfIndex] <= 60)
    {//堆叠组方式2
        ui->stkWgtIf->setCurrentWidget(ui->pageOtherIf);
        ui->coboxOtherDisplay->setCurrentIndex(LogicIf->cmpType[IfIndex]-41);
        ui->coboxOtherDisplay->setEnabled(false);

       if(LogicIf->sufferCmpType[IfIndex] == 103)
        {
            ui->editOther->hide();
            ui->coboxOtherSelect->hide();
            ui->coboxOtherSelect_stackFinish->show();
            ui->coboxOtherSelect_stackFinish->setCurrentIndex(LogicIf->sufferCmpType[IfIndex]-103);
            ui->coboxOperatorOther->hide();
            ui->coboxOperatorOther_2->show();
            ui->coboxOperatorOther_2->setCurrentIndex(LogicIf->cmpMode[IfIndex]-4);
        }
    }
    else if(LogicIf->cmpType[IfIndex]>=61 && LogicIf->cmpType[IfIndex] <= 80)
    {//轴
        ui->stkWgtIf->setCurrentWidget(ui->pageAxisPosIf);
        ui->coboxAxisDisplay->setCurrentIndex(LogicIf->cmpType[IfIndex]-61);
        ui->coboxAxisDisplay->setEnabled(false);
        ui->coboxOperatorAxisPos->setCurrentIndex(LogicIf->cmpMode[IfIndex]);
        if(LogicIf->sufferCmpType[IfIndex] == 0)
        {
            ui->editAxisPos->show();
            ui->coboxVarSelectAxisPos->hide();
            ui->coboxAxisDisplay_2->hide();
            ui->editAxisPos->setText(QString::number(LogicIf->sufferCmpValue[IfIndex],'f',2));
        }
        else if(LogicIf->sufferCmpType[IfIndex]>=1 && LogicIf->sufferCmpType[IfIndex]<=20)
        {
            ui->editAxisPos->hide();
            ui->coboxVarSelectAxisPos->show();
            ui->coboxAxisDisplay_2->hide();
            ui->coboxVarSelectAxisPos->setCurrentIndex(LogicIf->sufferCmpType[IfIndex]-1);
        }
        else if(LogicIf->sufferCmpType[IfIndex]>=61 && LogicIf->sufferCmpType[IfIndex]<=80)
        {
            ui->editAxisPos->hide();
            ui->coboxVarSelectAxisPos->hide();
            ui->coboxAxisDisplay_2->show();
            ui->coboxAxisDisplay_2->setCurrentIndex(LogicIf->sufferCmpType[IfIndex]-61);
        }
    }
    else if(LogicIf->cmpType[IfIndex]>=81 && LogicIf->cmpType[IfIndex] <= 100)
    {//定时器
        ui->stkWgtIf->setCurrentWidget(ui->pageTimerIf);
        ui->coboxTimerDisplayIf->setCurrentIndex(LogicIf->cmpType[IfIndex]-81);
        ui->coboxTimerDisplayIf->setEnabled(false);
        ui->coboxOperatorTimer->setCurrentIndex(LogicIf->cmpMode[IfIndex]);
        if(LogicIf->sufferCmpType[IfIndex]==0)
        {
            ui->editTimernum->show();
            ui->coboxTimerDisplayIf_2->hide();
            ui->editTimernum->setText(QString::number(LogicIf->sufferCmpValue[IfIndex]));
        }
        else if(LogicIf->sufferCmpType[IfIndex]>=81 && LogicIf->sufferCmpType[IfIndex]<=100)
        {
            ui->editTimernum->hide();
            ui->coboxTimerDisplayIf_2->show();
            ui->coboxTimerDisplayIf_2->setCurrentIndex(LogicIf->sufferCmpType[IfIndex]-81);
        }
    }
    else if(LogicIf->cmpType[IfIndex] == 102)
    {//实际产量
        ui->stkWgtIf->setCurrentWidget(ui->pageRealProdNum);
        ui->coboxRealProdNum->setEnabled(false);
        ui->coboxRealProdNum->setCurrentIndex(LogicIf->cmpType[IfIndex]-102);
        ui->coboxRealProdOperator->setCurrentIndex(LogicIf->cmpMode[IfIndex]);
        if(LogicIf->sufferCmpType[IfIndex] == 0)
        {
            ui->editRealProcIf->show();
            ui->editRealProcIf->setText(QString::number(LogicIf->sufferCmpValue[IfIndex]));
            ui->coboxRealProductNum_2->hide();
        }
        else if(LogicIf->sufferCmpType[IfIndex] >= 1 && LogicIf->sufferCmpType[IfIndex] <=20)
        {//变量
            ui->editRealProcIf->hide();
            ui->coboxRealProductNum_2->show();
            ui->coboxRealProductNum_2->setCurrentIndex(LogicIf->sufferCmpType[IfIndex]-1);
        }
    }
}

//如果指令条件1条件2按钮处理函数
void Teach::chboxCondition1If2If_handle()
{
    P_LogicIfStruct* LogicIf =(P_LogicIfStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
    if(ui->chboxCondition1If->isChecked())
    {
        Edit_ifOrder_handle(LogicIf,0);
    }
    else if(ui->chboxCondition2If->isChecked())
    {
        Edit_ifOrder_handle(LogicIf,1);
    }
}
//逻辑&变量-变量指令界面操作处理函数
void Teach::LogicVar_handle()
{
    if(ui->Edit_Var_const->isChecked())
    {
        ui->comboBox_Edit_Var_Oper->setEnabled(true);
        ui->lineEdit_Edit_Var_const->show();
        ui->comboBox_Edit_Var_Var->hide();
        ui->comboBox_Edit_Var_Axis->hide();
        ui->comboBox_Edit_Var_Prod->hide();
        if(ui->comboBox_Edit_Var->currentIndex()>VAR_TOTAL_NUM)
        {
            ui->comboBox_Edit_Var->setCurrentIndex(0);
        }
        ui->lineEdit_Edit_Var_const->setDecimalPlaces(m_VariableType[ui->comboBox_Edit_Var->currentIndex()]);
    }
    else if(ui->Edit_Var_var->isChecked())
    {
        ui->comboBox_Edit_Var_Oper->setCurrentIndex(4);
        ui->comboBox_Edit_Var_Oper->setEnabled(false);
        ui->lineEdit_Edit_Var_const->hide();
        ui->comboBox_Edit_Var_Var->show();
        ui->comboBox_Edit_Var_Axis->hide();
        ui->comboBox_Edit_Var_Prod->hide();
    }
    else if(ui->Edit_Var_Axis->isChecked())
    {
        ui->comboBox_Edit_Var_Oper->setCurrentIndex(4);
        ui->comboBox_Edit_Var_Oper->setEnabled(false);
        ui->lineEdit_Edit_Var_const->hide();
        ui->comboBox_Edit_Var_Var->hide();
        ui->comboBox_Edit_Var_Axis->show();
        ui->comboBox_Edit_Var_Prod->hide();
    }
    else if(ui->Edit_Var_Prod->isChecked())
    {
        ui->comboBox_Edit_Var_Oper->setCurrentIndex(4);
        ui->comboBox_Edit_Var_Oper->setEnabled(false);
        ui->lineEdit_Edit_Var_const->hide();
        ui->comboBox_Edit_Var_Var->hide();
        ui->comboBox_Edit_Var_Axis->hide();
        ui->comboBox_Edit_Var_Prod->show();
    }
}
//逻辑&变量-轴指令界面操作处理函数
void Teach::LogicAxis_handle()
{
    if(ui->Edit_Axis_const->isChecked())
    {
        ui->comboBox_Edit_Axis_Oper->setEnabled(true);
        ui->lineEdit_Edit_Axis_const->show();
        ui->comboBox_Edit_Axis_Var->hide();
    }
    else if(ui->Edit_Axis_var->isChecked())
    {
        ui->comboBox_Edit_Axis_Oper->setCurrentIndex(4);
        ui->comboBox_Edit_Axis_Oper->setEnabled(false);
        ui->lineEdit_Edit_Axis_const->hide();
        ui->comboBox_Edit_Axis_Var->show();
    }
}
//逻辑&变量-堆叠指令界面操作处理函数
void Teach::LogicStack_handle()
{
    if(ui->Edit_Stack_const->isChecked())
    {
        ui->comboBox_Edit_Stack_Oper->setEnabled(true);
        ui->lineEdit_Edit_Stack_const->show();
        ui->comboBox_Edit_Stack_num1->hide();
    }
    else if(ui->Edit_Stack_num->isChecked())
    {
        ui->comboBox_Edit_Stack_Oper->setCurrentIndex(4);
        ui->comboBox_Edit_Stack_Oper->setEnabled(false);
        ui->lineEdit_Edit_Stack_const->hide();
        ui->comboBox_Edit_Stack_num1->show();
    }
}

//逻辑&变量-实际产量指令界面操作处理函数
void Teach::LogicProt_handle()
{
    if(ui->Edit_Prod_const->isChecked())
    {
        ui->comboBox_Edit_Prod_Oper->setEnabled(true);
        ui->lineEdit_Edit_Prod_const->show();
        ui->comboBox_Edit_Prod_Var->hide();
        ui->lineEdit_Edit_Prod_const->setDecimalPlaces(0);
    }
    else if(ui->Edit_Prod_var->isChecked())
    {
        ui->comboBox_Edit_Prod_Oper->setCurrentIndex(4);
        ui->comboBox_Edit_Prod_Oper->setEnabled(false);
        ui->lineEdit_Edit_Prod_const->hide();
        ui->comboBox_Edit_Prod_Var->show();
    }
}
//点击或滑动行时编辑指令处理
void Teach::on_tableWgtTeach_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
    if(currentRow>=0 && currentRow <= m_OperateProOrderListNum)
    {
        m_CurrentSelectProOrderList = currentRow;
        if(m_OperateProOrderListNum == 0)
        {//如果未载入程序，直接返回
            return;
        }
        if(ui->btnModify->isChecked())
        {//如果编辑按钮按下
            OrderEdit_Handle();
        }
    }
    else
    {
        qDebug()<<"currentRow error:"<<currentRow;
    }
}
//试行触发操作处理
void Teach::on_btnPilot_clicked()
{
    if(m_OperateProOrderListNum == 0)
    {//如果未载入程序，直接返回
        return;
    }
    m_CurrentSelectProOrderList = ui->tableWgtTeach->currentIndex().row();
    if(m_CurrentSelectProOrderList<m_OperateProOrderListNum)
    {
        g_Usart->ExtendSendProDeal(CMD_MAIN_PRO,CMD_SUN_PRO_DEBUG,m_OperateProNum,m_CurrentSelectProOrderList+1,m_RunPar.globalSpeed);//运行行号从1开始，为当前选中行序号+1
    }
}
//标签指令复选框处理函数
void Teach::on_chboxComment_clicked()
{
    listWgtJumptoLabelIndex = 0;
    Refresh_listWgtJumpto();
}
//标签指令列表中checkbox点击处理
void Teach::onCheckBoxToggled(QCheckBox *checkbox, bool checked)
{
    if(checked)
    {
        ui->chboxComment->setChecked(false);
        QString Itemtext = checkbox->text();
        QRegularExpression re("标签(\\d+)"); // 匹配“标签”后面的数字
        QRegularExpressionMatch match = re.match(Itemtext);
        if (match.hasMatch()) {
            listWgtJumptoLabelIndex = match.captured(1).toUInt(); // 提取匹配到的数字
        } else {
            qDebug() << "No number found.";
        }
    }
}


void Teach::on_Stack_Edit_btnAxisX1_clicked()
{
    Stack_Edit_Handle(STACK_X_AXIS);
    ui->Stack_Edit_btnAxisX1->setStyleSheet("border-style:outset; /*边框风格*/\
                                            background-color:qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop:0 #00befa, stop: 1 #0160ea);/*背景色*/\
                                            border-width:1px;/*边框宽度*/\
                                            border-color:#0055ff; /*边框颜色*/\
                                            border-radius:5px; /*边框倒角*/");
    ui->Stack_Edit_btnAxisY1->setStyleSheet("font-family: 'SimSun';\
                                            font-size: 22px; \
                                            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 #FFFAFA, stop: 0.4 #EEE9E9,stop: 0.5 #EEE9E9, stop: 1.0 #CDC9C9);\
                                            border-style:solid; /*边框风格*/\
                                            border-width:1px;/*边框宽度*/\
                                            border-color:#7A77AC; /*边框颜色*/\
                                            border-radius:5px; /*边框倒角*/");
    ui->Stack_Edit_btnAxisZ1->setStyleSheet("font-family: 'SimSun';\
                                            font-size: 22px; \
                                            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 #FFFAFA, stop: 0.4 #EEE9E9,stop: 0.5 #EEE9E9, stop: 1.0 #CDC9C9);\
                                            border-style:solid; /*边框风格*/\
                                            border-width:1px;/*边框宽度*/\
                                            border-color:#7A77AC; /*边框颜色*/\
                                            border-radius:5px; /*边框倒角*/");
}

void Teach::on_Stack_Edit_btnAxisZ1_clicked()
{
    Stack_Edit_Handle(STACK_Z_AXIS);
    ui->Stack_Edit_btnAxisZ1->setStyleSheet("border-style:outset; /*边框风格*/\
                                            background-color:qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop:0 #00befa, stop: 1 #0160ea);/*背景色*/\
                                            border-width:1px;/*边框宽度*/\
                                            border-color:#0055ff; /*边框颜色*/\
                                            border-radius:5px; /*边框倒角*/");
    ui->Stack_Edit_btnAxisY1->setStyleSheet("font-family: 'SimSun';\
                                            font-size: 22px; \
                                            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 #FFFAFA, stop: 0.4 #EEE9E9,stop: 0.5 #EEE9E9, stop: 1.0 #CDC9C9);\
                                            border-style:solid; /*边框风格*/\
                                            border-width:1px;/*边框宽度*/\
                                            border-color:#7A77AC; /*边框颜色*/\
                                            border-radius:5px; /*边框倒角*/");
    ui->Stack_Edit_btnAxisX1->setStyleSheet("font-family: 'SimSun';\
                                            font-size: 22px; \
                                            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 #FFFAFA, stop: 0.4 #EEE9E9,stop: 0.5 #EEE9E9, stop: 1.0 #CDC9C9);\
                                            border-style:solid; /*边框风格*/\
                                            border-width:1px;/*边框宽度*/\
                                            border-color:#7A77AC; /*边框颜色*/\
                                            border-radius:5px; /*边框倒角*/");
}

void Teach::on_Stack_Edit_btnAxisY1_clicked()
{
    Stack_Edit_Handle(STACK_Y_AXIS);
    ui->Stack_Edit_btnAxisY1->setStyleSheet("border-style:outset;\
                                            background-color:qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop:0 #00befa, stop: 1 #0160ea);/*背景色*/\
                                            border-width:1px;\
                                            border-color:#0055ff;\
                                            border-radius:5px;");
    ui->Stack_Edit_btnAxisX1->setStyleSheet("font-family: 'SimSun';\
                                            font-size: 22px; \
                                            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 #FFFAFA, stop: 0.4 #EEE9E9,stop: 0.5 #EEE9E9, stop: 1.0 #CDC9C9);\
                                            border-style:solid; /*边框风格*/\
                                            border-width:1px;/*边框宽度*/\
                                            border-color:#7A77AC; /*边框颜色*/\
                                            border-radius:5px; /*边框倒角*/");
    ui->Stack_Edit_btnAxisZ1->setStyleSheet("font-family: 'SimSun';\
                                            font-size: 22px; \
                                            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 #FFFAFA, stop: 0.4 #EEE9E9,stop: 0.5 #EEE9E9, stop: 1.0 #CDC9C9);\
                                            border-style:solid; /*边框风格*/\
                                            border-width:1px;/*边框宽度*/\
                                            border-color:#7A77AC; /*边框颜色*/\
                                            border-radius:5px; /*边框倒角*/");
}
//轴插入指令坐标刷新按钮处理
void Teach::on_btn_General_Refresh_clicked()
{
    double AxisCurPos = 0;
    for(uint8_t i=0;i<AXIS_TOTAL_NUM+2;i++)
    {
        switch (Temp_AxisMoveOrder[i].axis) {
        case X1_AXIS:
            AxisCurPos = m_AxisCurPos.Pos_x;//m_AxisCurPos[X1_AXIS];
            Temp_AxisMoveOrder[X1_AXIS].pos = AxisCurPos;
            ui->lineEdit_General_Position_X1->setText(QString::number(AxisCurPos/100,'f',2));
            break;
        case Y1_AXIS:
            AxisCurPos = m_AxisCurPos.Pos_y;//m_AxisCurPos[Y1_AXIS];
            Temp_AxisMoveOrder[Y1_AXIS].pos = AxisCurPos;
            ui->lineEdit_General_Position_Y1->setText(QString::number(AxisCurPos/100,'f',2));
            break;
        case Z1_AXIS:
            AxisCurPos = m_AxisCurPos.Pos_z;//m_AxisCurPos[Z1_AXIS];
            Temp_AxisMoveOrder[Z1_AXIS].pos = AxisCurPos;
            ui->lineEdit_General_Position_Z1->setText(QString::number(AxisCurPos/100,'f',2));
            break;
        case C_AXIS:
            AxisCurPos = m_AxisCurPos.Pos_c;//m_AxisCurPos[C_AXIS];
            Temp_AxisMoveOrder[C_AXIS].pos = AxisCurPos;
            ui->lineEdit_General_Position_C->setText(QString::number(AxisCurPos/100,'f',2));
            break;
        case Y2_AXIS:
            AxisCurPos = m_AxisCurPos.Pos_y2;//m_AxisCurPos[Y2_AXIS];
            Temp_AxisMoveOrder[Y2_AXIS].pos = AxisCurPos;
            if(ui->stackedWidget_Axis->currentWidget() == ui->Axis_Move_page1)
            {
                ui->lineEdit_General_Position_Y2->setText(QString::number(AxisCurPos/100,'f',2));
            }
            break;
        case Z2_AXIS:
            AxisCurPos = m_AxisCurPos.Pos_z2;//m_AxisCurPos[Z2_AXIS];
            Temp_AxisMoveOrder[Z2_AXIS].pos = AxisCurPos;
            if(ui->stackedWidget_Axis->currentWidget() == ui->Axis_Move_page1)
            {
                ui->lineEdit_General_Position_Z2->setText(QString::number(AxisCurPos/100,'f',2));
            }
            break;
        default:
            break;
        }
    }
}
//轴搜索指令界面刷新按钮处理
void Teach::on_btn_Refresh_clicked()
{
    uint8_t Axis_Index = 0;
    double AxisCurPos = 0;
    for(int i=0;i<AXIS_TOTAL_NUM;i++)
    {
        if(ui->coboxSearchAxisSelect->currentText() == m_NameDefine[1].axisName[i])
        {
            Axis_Index = i;
        }
    }
    if(ui->chboxSearchAxisSelect->isChecked())
    {
        switch (Axis_Index) {
        case X1_AXIS:
            AxisCurPos = m_AxisCurPos.Pos_x;//m_AxisCurPos[X1_AXIS];
            break;
        case Y1_AXIS:
            AxisCurPos = m_AxisCurPos.Pos_y;//m_AxisCurPos[Y1_AXIS];
            break;
        case Z1_AXIS:
            AxisCurPos = m_AxisCurPos.Pos_z;//m_AxisCurPos[Z1_AXIS];
            break;
        case C_AXIS:
            AxisCurPos = m_AxisCurPos.Pos_c;//m_AxisCurPos[C_AXIS];
            break;
        case Y2_AXIS:
            AxisCurPos = m_AxisCurPos.Pos_y2;//m_AxisCurPos[Y2_AXIS];
            break;
        case Z2_AXIS:
            AxisCurPos = m_AxisCurPos.Pos_z2;//m_AxisCurPos[Z2_AXIS];
            break;
        default:
            break;
        }
        ui->editSearchPosition->setText(QString::number(AxisCurPos/100,'f',2));
        Temp_SearchAxisMoveStruct.maxPos = ui->editSearchPosition->text().toDouble()*100;
    }
}

//堆叠轴坐标刷新
void Teach::on_AxisEditRefresh_clicked()
{
//    double AxisCurPos = 0;
//    if(m_OperateProOrder[m_CurrentSelectProOrderList].cmd == C_AXIS_MOVE)
//    {
//        P_AxisMoveStruct *AxisMove = (P_AxisMoveStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
//        switch (AxisMove->axis) {
//        case X1_AXIS:
//            AxisCurPos = m_AxisCurPos.Pos_x;//m_AxisCurPos[X1_AXIS];
//            ui->lineEdit_Edit_AxisMove_pos->setText(QString::number(AxisCurPos/100,'f',2));
//            break;
//        case Y1_AXIS:
//            AxisCurPos = m_AxisCurPos.Pos_y;//m_AxisCurPos[Y1_AXIS];
//            ui->lineEdit_Edit_AxisMove_pos->setText(QString::number(AxisCurPos/100,'f',2));
//            break;
//        case Z1_AXIS:
//            AxisCurPos = m_AxisCurPos.Pos_z;//m_AxisCurPos[Z1_AXIS];
//            ui->lineEdit_Edit_AxisMove_pos->setText(QString::number(AxisCurPos/100,'f',2));
//            break;
//        case C_AXIS:
//            AxisCurPos = m_AxisCurPos.Pos_c;//m_AxisCurPos[C_AXIS];
//            ui->lineEdit_Edit_AxisMove_pos->setText(QString::number(AxisCurPos/100,'f',2));
//            break;
//        case Y2_AXIS:
//            AxisCurPos = m_AxisCurPos.Pos_y2;//m_AxisCurPos[Y2_AXIS];
//            if(ui->stackedWidget_Axis->currentWidget() == ui->Axis_Move_page1)
//            {
//                ui->lineEdit_Edit_AxisMove_pos->setText(QString::number(AxisCurPos/100,'f',2));
//            }
//            else if(ui->stackedWidget_Axis->currentWidget() == ui->Axis_Move_page2)
//            {
//                if(ui->cb_General_A->isChecked())
//                {
//                    ui->lineEdit_Edit_AxisMove_pos->setText(QString::number(AxisCurPos/100,'f',2));
//                }
//            }
//            break;
//        case Z2_AXIS:
//            AxisCurPos = m_AxisCurPos.Pos_z2;//m_AxisCurPos[Z2_AXIS];
//            if(ui->stackedWidget_Axis->currentWidget() == ui->Axis_Move_page1)
//            {
//                ui->lineEdit_Edit_AxisMove_pos->setText(QString::number(AxisCurPos/100,'f',2));
//            }
//            else if(ui->stackedWidget_Axis->currentWidget() == ui->Axis_Move_page2)
//            {
//                if(ui->cb_General_B->isChecked())
//                {
//                    ui->lineEdit_Edit_AxisMove_pos->setText(QString::number(AxisCurPos/100,'f',2));
//                }
//            }
//            break;
//        default:
//            break;
//        }
//    }
}
//轴编辑坐标刷新
void Teach::on_Edit_AxisMove_Refresh_clicked()
{
    double AxisCurPos = 0;
    if(m_OperateProOrder[m_CurrentSelectProOrderList].cmd == C_AXIS_MOVE)
    {
        P_AxisMoveStruct *AxisMove = (P_AxisMoveStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
        switch (AxisMove->axis) {
        case X1_AXIS:
            AxisCurPos = m_AxisCurPos.Pos_x;//m_AxisCurPos[X1_AXIS];
            ui->lineEdit_Edit_AxisMove_pos->setText(QString::number(AxisCurPos/100,'f',2));
            break;
        case Y1_AXIS:
            AxisCurPos = m_AxisCurPos.Pos_y;//m_AxisCurPos[Y1_AXIS];
            ui->lineEdit_Edit_AxisMove_pos->setText(QString::number(AxisCurPos/100,'f',2));
            break;
        case Z1_AXIS:
            AxisCurPos = m_AxisCurPos.Pos_z;//m_AxisCurPos[Z1_AXIS];
            ui->lineEdit_Edit_AxisMove_pos->setText(QString::number(AxisCurPos/100,'f',2));
            break;
        case C_AXIS:
            AxisCurPos = m_AxisCurPos.Pos_c;//m_AxisCurPos[C_AXIS];
            ui->lineEdit_Edit_AxisMove_pos->setText(QString::number(AxisCurPos/100,'f',2));
            break;
        case Y2_AXIS:
            AxisCurPos = m_AxisCurPos.Pos_y2;//m_AxisCurPos[Y2_AXIS];
            if(ui->stackedWidget_Axis->currentWidget() == ui->Axis_Move_page1)
            {
                ui->lineEdit_Edit_AxisMove_pos->setText(QString::number(AxisCurPos/100,'f',2));
            }
            break;
        case Z2_AXIS:
            AxisCurPos = m_AxisCurPos.Pos_z2;//m_AxisCurPos[Z2_AXIS];
            if(ui->stackedWidget_Axis->currentWidget() == ui->Axis_Move_page1)
            {
                ui->lineEdit_Edit_AxisMove_pos->setText(QString::number(AxisCurPos/100,'f',2));
            }
            break;
        default:
            break;
        }
    }
}
//教导界面控件名称刷新
void Teach::WidgetNameRefresh()
{
    //原料1松开/夹紧
    if(m_Port_Y[CLAW_METERIAL_1_CLAMP].functionSet == 1)
    {
        ui->cb_Material_clamp_1->setCheckable(true);
        ui->btn_Material_clamp_1->setEnabled(true);
        if(ui->btn_Material_clamp_1->text() != m_Port_Y[CLAW_METERIAL_1_CLAMP].modifyName)
        {
            ui->btn_Material_clamp_1->setText(m_Port_Y[CLAW_METERIAL_1_CLAMP].modifyName);
        }
    }
    else
    {
        ui->cb_Material_clamp_1->setCheckable(false);
        ui->btn_Material_clamp_1->setEnabled(false);
    }
    //成品1松开/夹紧
    if(m_Port_Y[CLAW_PRODUCT_1_CLAMP].functionSet == 1)
    {
        ui->cb_finish_product_1->setCheckable(true);
        ui->btn_finish_product_1->setEnabled(true);
        if(ui->btn_finish_product_1->text() != m_Port_Y[CLAW_PRODUCT_1_CLAMP].modifyName)
        {
            ui->btn_finish_product_1->setText(m_Port_Y[CLAW_PRODUCT_1_CLAMP].modifyName);
        }
    }
    else
    {
        ui->cb_finish_product_1->setCheckable(false);
        ui->btn_finish_product_1->setEnabled(false);

    }
    //卡爪1正转/反转
    if(m_Port_Y[CLAW_CLAW_1_CLAMP].functionSet == 1)
    {
        ui->cb_Claw_roll_1->setCheckable(true);
        ui->btn_Claw_roll_1->setEnabled(true);
        if(ui->btn_Claw_roll_1->text() != m_Port_Y[CLAW_CLAW_1_CLAMP].modifyName)
        {
            ui->btn_Claw_roll_1->setText(m_Port_Y[CLAW_CLAW_1_CLAMP].modifyName);
        }
    }
    else
    {
        ui->cb_Claw_roll_1->setCheckable(false);
        ui->btn_Claw_roll_1->setEnabled(false);
    }
    //报警灯和报警声
    if(m_SeniorFunc.alarmLight == 1)
    {
        ui->btnAlarmLamp_button->setEnabled(true);
        ui->AlarmLamp_chbox->setEnabled(true);
    }
    else
    {
        ui->btnAlarmLamp_button->setEnabled(false);
        ui->AlarmLamp_chbox->setEnabled(false);
    }
    if(m_SeniorFunc.alarmBuzzer == 1)
    {
        ui->btnAlarmSound_button->setEnabled(true);
        ui->AlarmSound_chbox->setEnabled(true);
    }
    else
    {
        ui->btnAlarmSound_button->setEnabled(false);
        ui->AlarmSound_chbox->setEnabled(false);
    }
    //卡爪检测界面
    if(m_Port_Y[CLAW_METERIAL_1_CLAMP].functionSet == 1 || m_Port_Y[CLAW_PRODUCT_1_CLAMP].functionSet == 1 || m_Port_Y[MACHINE_CHUCK_1_CLAMP].functionSet == 1)
    {
        if(m_OutportInterlock[0][1] != 0 || m_OutportInterlock[0][3] != 0    //原料1夹紧-正向/反向检测使用
            ||m_OutportInterlock[1][1] != 0 || m_OutportInterlock[1][3] != 0 //成品1夹紧-正向/反向检测使用
            ||m_OutportInterlock[4][1] != 0 || m_OutportInterlock[4][3] != 0)//卡盘2夹紧-正向/反向检测使用
        {
            ui->cb_General_PassStart->setEnabled(true);
            ui->cb_General_PassStop->setEnabled(true);
            ui->cb_General_PineEnd->setEnabled(true);
            ui->cb_General_PineStart->setEnabled(true);
            ui->combo_General_PassStart->setEnabled(true);
            ui->combo_General_PassStop->setEnabled(true);
            ui->combo_General_PineStart->setEnabled(true);
            ui->combo_General_PineEnd->setEnabled(true);
            ui->combo_General_PassStart->clear();//先清空所有选项
            ui->combo_General_PassStop->clear();
            ui->combo_General_PineStart->clear();
            ui->combo_General_PineEnd->clear();
            if(m_Port_Y[CLAW_METERIAL_1_CLAMP].functionSet == 1 && (m_OutportInterlock[0][1] != 0 || m_OutportInterlock[0][3] != 0))
            {
                ui->combo_General_PassStart->addItem(m_Port_X[SIGNAL_DETECTION_METERIAL_1_CLAMP_LIMIT].modifyName);
                ui->combo_General_PineStart->addItem(m_Port_X[SIGNAL_DETECTION_METERIAL_1_LOOSENED_LIMIT].modifyName);
                ui->combo_General_PassStop->addItem(m_Port_X[SIGNAL_DETECTION_METERIAL_1_CLAMP_LIMIT].modifyName);
                ui->combo_General_PineEnd->addItem(m_Port_X[SIGNAL_DETECTION_METERIAL_1_LOOSENED_LIMIT].modifyName);
            }
            if(m_Port_Y[CLAW_PRODUCT_1_CLAMP].functionSet == 1 && (m_OutportInterlock[1][1] != 0 || m_OutportInterlock[1][3] != 0))
            {
                ui->combo_General_PassStart->addItem(m_Port_X[SIGNAL_DETECTION_PRODUCT_1_CLAMP_LIMIT].modifyName);
                ui->combo_General_PineStart->addItem(m_Port_X[SIGNAL_DETECTION_PRODUCT_1_LOOSENED_LIMIT].modifyName);
                ui->combo_General_PassStop->addItem(m_Port_X[SIGNAL_DETECTION_PRODUCT_1_CLAMP_LIMIT].modifyName);
                ui->combo_General_PineEnd->addItem(m_Port_X[SIGNAL_DETECTION_PRODUCT_1_LOOSENED_LIMIT].modifyName);
            }
            if(m_Port_Y[MACHINE_CHUCK_1_CLAMP].functionSet == 1 && (m_OutportInterlock[4][1] != 0 || m_OutportInterlock[4][3] != 0))
            {
                ui->combo_General_PassStart->addItem(m_Port_X[SIGNAL_DETECTION_CHUCK_1_CLAMP_LIMIT].modifyName);
                ui->combo_General_PineStart->addItem(m_Port_X[SIGNAL_DETECTION_CHUCK_1_LOOSENED_LIMIT].modifyName);
                ui->combo_General_PassStop->addItem(m_Port_X[SIGNAL_DETECTION_CHUCK_1_CLAMP_LIMIT].modifyName);
                ui->combo_General_PineEnd->addItem(m_Port_X[SIGNAL_DETECTION_CHUCK_1_LOOSENED_LIMIT].modifyName);
            }

            if(ui->combo_General_PassStart->count()>0)
            {
                ui->combo_General_PassStart->setCurrentIndex(0);
            }
            if(ui->combo_General_PassStop->count()>0)
            {
                ui->combo_General_PassStop->setCurrentIndex(0);
            }
            if(ui->combo_General_PineStart->count()>0)
            {
                ui->combo_General_PineStart->setCurrentIndex(0);
            }
            if(ui->combo_General_PineEnd->count()>0)
            {
                ui->combo_General_PineEnd->setCurrentIndex(0);
            }
        }
        else
        {
            ui->cb_General_PassStart->setEnabled(false);
            ui->cb_General_PassStop->setEnabled(false);
            ui->cb_General_PineEnd->setEnabled(false);
            ui->cb_General_PineStart->setEnabled(false);
            ui->combo_General_PassStart->setEnabled(false);
            ui->combo_General_PassStop->setEnabled(false);
            ui->combo_General_PineStart->setEnabled(false);
            ui->combo_General_PineEnd->setEnabled(false);
        }
    }
    else
    {
        ui->cb_General_PassStart->setEnabled(false);
        ui->cb_General_PassStop->setEnabled(false);
        ui->cb_General_PineEnd->setEnabled(false);
        ui->cb_General_PineStart->setEnabled(false);
        ui->combo_General_PassStart->setEnabled(false);
        ui->combo_General_PassStop->setEnabled(false);
        ui->combo_General_PineStart->setEnabled(false);
        ui->combo_General_PineEnd->setEnabled(false);
    }
    //机床输出界面
    if(m_SeniorFunc.startProduct1 == 1)
    {//启动加工1使用
        ui->Start_Up_Mach_chbox->setEnabled(true);
        ui->Start_Up_Mach_button->setEnabled(true);
        ui->lineEdit_Start_Up_Mach->setEnabled(true);
    }
    else
    {
        ui->Start_Up_Mach_chbox->setEnabled(false);
        ui->Start_Up_Mach_button->setEnabled(false);
        ui->lineEdit_Start_Up_Mach->setEnabled(false);
    }
    if(m_SeniorFunc.mainAxisRotate1 == 1)
    {//主轴旋转1使用
        ui->Main_Axis_Rotate_chbox->setEnabled(true);
        ui->Main_Axis_Rotate_button->setEnabled(true);
        ui->lineEdit_Main_Axis_Rotate->setEnabled(true);
    }
    else
    {
        ui->Main_Axis_Rotate_chbox->setEnabled(false);
        ui->Main_Axis_Rotate_button->setEnabled(false);
        ui->lineEdit_Main_Axis_Rotate->setEnabled(false);
    }
    if(m_SeniorFunc.mainAxisLocate1 == 1)
    {//主轴定位1使用
        ui->Main_Axis_locat_chbox->setEnabled(true);
        ui->Main_Axis_locat_button->setEnabled(true);
        ui->lineEdit_Main_Axis_locat->setEnabled(true);
    }
    else
    {
        ui->Main_Axis_locat_chbox->setEnabled(false);
        ui->Main_Axis_locat_button->setEnabled(false);
        ui->lineEdit_Main_Axis_locat->setEnabled(false);
    }
    if(m_SeniorFunc.autoDoorCot1 == 1)
    {//自动门1使用
        ui->Auto_Door_chbox->setEnabled(true);
        ui->Auto_Door_button->setEnabled(true);
        ui->lineEdit_Auto_Door->setEnabled(true);
    }
    else
    {
        ui->Auto_Door_chbox->setEnabled(false);
        ui->Auto_Door_button->setEnabled(false);
        ui->lineEdit_Auto_Door->setEnabled(false);
    }
    if(m_SeniorFunc.biowAir1 == 1)
    {//吹气1使用
        ui->Blow_Air_chbox->setEnabled(true);
        ui->lineEdit_Blow_Air->setEnabled(true);
        ui->Blow_Air_button->setEnabled(true);
    }
    else
    {
        ui->Blow_Air_chbox->setEnabled(false);
        ui->lineEdit_Blow_Air->setEnabled(false);
        ui->Blow_Air_button->setEnabled(false);
    }
    if(m_OutportInterlock[4][1] != 0)
    {//互锁设置-卡盘1夹紧使用
        ui->Chuck_chbox->setEnabled(true);
        ui->Chuck_button->setEnabled(true);
        ui->lineEdit_Chuck->setEnabled(true);
    }
    else
    {
        ui->Chuck_chbox->setEnabled(false);
        ui->Chuck_button->setEnabled(false);
        ui->lineEdit_Chuck->setEnabled(false);
    }
    //信号等待界面-等待机床界面
    ui->Wait_machine_signal_port->clear();
    ui->Wait_machine_signal_box->setEnabled(true);
    ui->Wait_machine_signal_port->setEnabled(true);
    if(m_SeniorFunc.knifeOrigin1Check == 1)
    {//刀座原点1检测 0关 1开
        ui->Wait_machine_signal_port->addItem(m_Port_X[33].modifyName);
    }
    if(m_SeniorFunc.processFinish1 == 1)
    {//加工完成1 0关 1开
        ui->Wait_machine_signal_port->addItem(m_Port_X[31].modifyName);
    }
    if(m_SeniorFunc.locateFinish1 == 1)
    {//定位完成1 0关 1开
        ui->Wait_machine_signal_port->addItem(m_Port_X[32].modifyName);
    }
    if(m_SeniorFunc.knifeOrigin1Check == 0 && m_SeniorFunc.processFinish1 == 0 && m_SeniorFunc.locateFinish1 == 0)
    {
        ui->Wait_machine_signal_box->setEnabled(false);
        ui->Wait_machine_signal_port->setEnabled(false);
    }
    //信号等待界面-等待卡爪界面
    if(m_Port_Y[CLAW_METERIAL_1_CLAMP].functionSet == 1 || m_Port_Y[CLAW_PRODUCT_1_CLAMP].functionSet == 1 || m_Port_Y[MACHINE_CHUCK_1_CLAMP].functionSet == 1)
    {
        if(m_OutportInterlock[0][1] != 0 || m_OutportInterlock[0][3] != 0    //原料1夹紧-正向/反向检测使用
            ||m_OutportInterlock[1][1] != 0 || m_OutportInterlock[1][3] != 0 //成品1夹紧-正向/反向检测使用
            ||m_OutportInterlock[4][1] != 0 || m_OutportInterlock[4][3] != 0)//卡盘2夹紧-正向/反向检测使用
        {
            ui->Wait_clamp_on_chbox->setEnabled(true);
            ui->Wait_clamp_off_chbox->setEnabled(true);
            ui->Wait_release_on_chbox->setEnabled(true);
            ui->Wait_release_off_chbox->setEnabled(true);
            ui->Wait_pos_cor_chbox->setEnabled(true);
            ui->Wait_clamp_on_comboBox->setEnabled(true);
            ui->Wait_clamp_off_comboBox->setEnabled(true);
            ui->Wait_release_on_comboBox->setEnabled(true);
            ui->Wait_release_off_comboBox->setEnabled(true);
            ui->Wait_pos_cor_comboBox->setEnabled(true);
            ui->Wait_clamp_on_comboBox->clear();//先清空所有选项
            ui->Wait_clamp_off_comboBox->clear();
            ui->Wait_release_on_comboBox->clear();
            ui->Wait_release_off_comboBox->clear();
            ui->Wait_pos_cor_comboBox->clear();
            if(m_Port_Y[CLAW_METERIAL_1_CLAMP].functionSet == 1 && (m_OutportInterlock[0][1] != 0 || m_OutportInterlock[0][3] != 0))
            {
                ui->Wait_clamp_on_comboBox->addItem(m_Port_X[SIGNAL_DETECTION_METERIAL_1_CLAMP_LIMIT].modifyName);
                ui->Wait_clamp_off_comboBox->addItem(m_Port_X[SIGNAL_DETECTION_METERIAL_1_CLAMP_LIMIT].modifyName);
                ui->Wait_release_on_comboBox->addItem(m_Port_X[SIGNAL_DETECTION_METERIAL_1_LOOSENED_LIMIT].modifyName);
                ui->Wait_release_off_comboBox->addItem(m_Port_X[SIGNAL_DETECTION_METERIAL_1_LOOSENED_LIMIT].modifyName);
                ui->Wait_pos_cor_comboBox->addItem(m_Port_X[SIGNAL_DETECTION_METERIAL_1_CLAMP_LIMIT].modifyName);
            }
            if(m_Port_Y[CLAW_PRODUCT_1_CLAMP].functionSet == 1 && (m_OutportInterlock[1][1] != 0 || m_OutportInterlock[1][3] != 0))
            {
                ui->Wait_clamp_on_comboBox->addItem(m_Port_X[SIGNAL_DETECTION_PRODUCT_1_CLAMP_LIMIT].modifyName);
                ui->Wait_clamp_off_comboBox->addItem(m_Port_X[SIGNAL_DETECTION_PRODUCT_1_CLAMP_LIMIT].modifyName);
                ui->Wait_release_on_comboBox->addItem(m_Port_X[SIGNAL_DETECTION_PRODUCT_1_LOOSENED_LIMIT].modifyName);
                ui->Wait_release_off_comboBox->addItem(m_Port_X[SIGNAL_DETECTION_PRODUCT_1_LOOSENED_LIMIT].modifyName);
                ui->Wait_pos_cor_comboBox->addItem(m_Port_X[SIGNAL_DETECTION_PRODUCT_1_CLAMP_LIMIT].modifyName);
            }
            if(m_Port_Y[MACHINE_CHUCK_1_CLAMP].functionSet == 1 && (m_OutportInterlock[4][1] != 0 || m_OutportInterlock[4][3] != 0))
            {
                ui->Wait_clamp_on_comboBox->addItem(m_Port_X[SIGNAL_DETECTION_CHUCK_1_CLAMP_LIMIT].modifyName);
                ui->Wait_clamp_off_comboBox->addItem(m_Port_X[SIGNAL_DETECTION_CHUCK_1_CLAMP_LIMIT].modifyName);
                ui->Wait_release_on_comboBox->addItem(m_Port_X[SIGNAL_DETECTION_CHUCK_1_LOOSENED_LIMIT].modifyName);
                ui->Wait_release_off_comboBox->addItem(m_Port_X[SIGNAL_DETECTION_CHUCK_1_LOOSENED_LIMIT].modifyName);
                ui->Wait_pos_cor_comboBox->addItem(m_Port_X[SIGNAL_DETECTION_CHUCK_1_CLAMP_LIMIT].modifyName);
            }

            if(ui->Wait_clamp_on_comboBox->count()>0)
            {
                ui->Wait_clamp_on_comboBox->setCurrentIndex(0);
            }
            if(ui->Wait_clamp_off_comboBox->count()>0)
            {
                ui->Wait_clamp_off_comboBox->setCurrentIndex(0);
            }
            if(ui->Wait_release_on_comboBox->count()>0)
            {
                ui->Wait_release_on_comboBox->setCurrentIndex(0);
            }
            if(ui->Wait_release_off_comboBox->count()>0)
            {
                ui->Wait_release_off_comboBox->setCurrentIndex(0);
            }
            if(ui->Wait_pos_cor_comboBox->count()>0)
            {
                ui->Wait_pos_cor_comboBox->setCurrentIndex(0);
            }
        }
        else
        {
            ui->Wait_clamp_on_chbox->setEnabled(false);
            ui->Wait_clamp_off_chbox->setEnabled(false);
            ui->Wait_release_on_chbox->setEnabled(false);
            ui->Wait_release_off_chbox->setEnabled(false);
            ui->Wait_pos_cor_chbox->setEnabled(false);
            ui->Wait_clamp_on_comboBox->setEnabled(false);
            ui->Wait_clamp_off_comboBox->setEnabled(false);
            ui->Wait_release_on_comboBox->setEnabled(false);
            ui->Wait_release_off_comboBox->setEnabled(false);
            ui->Wait_pos_cor_comboBox->setEnabled(false);
        }
    }
    else
    {
        ui->Wait_clamp_on_chbox->setEnabled(false);
        ui->Wait_clamp_off_chbox->setEnabled(false);
        ui->Wait_release_on_chbox->setEnabled(false);
        ui->Wait_release_off_chbox->setEnabled(false);
        ui->Wait_pos_cor_chbox->setEnabled(false);
        ui->Wait_clamp_on_comboBox->setEnabled(false);
        ui->Wait_clamp_off_comboBox->setEnabled(false);
        ui->Wait_release_on_comboBox->setEnabled(false);
        ui->Wait_release_off_comboBox->setEnabled(false);
        ui->Wait_pos_cor_comboBox->setEnabled(false);
    }

    ui->cb_General_X1->setText(m_NameDefine[1].axisName[X1_AXIS]);
    ui->chboxSpeedTeachX1->setText(m_NameDefine[1].axisName[X1_AXIS]);
    ui->cb_General_Y1->setText(m_NameDefine[1].axisName[Y1_AXIS]);
    ui->chboxSpeedTeachY1->setText(m_NameDefine[1].axisName[Y1_AXIS]);
    ui->cb_General_Z1->setText(m_NameDefine[1].axisName[Z1_AXIS]);
    ui->chboxSpeedTeachZ1->setText(m_NameDefine[1].axisName[Z1_AXIS]);
    ui->cb_General_C->setText(m_NameDefine[1].axisName[C_AXIS]);
    ui->chboxSpeedTeachC->setText(m_NameDefine[1].axisName[C_AXIS]);
    ui->cb_General_Y2->setText(m_NameDefine[1].axisName[Y2_AXIS]);
    ui->chboxSpeedTeachY2->setText(m_NameDefine[1].axisName[Y2_AXIS]);
    ui->cb_General_Z2->setText(m_NameDefine[1].axisName[Z2_AXIS]);
    ui->chboxSpeedTeachZ2->setText(m_NameDefine[1].axisName[Z2_AXIS]);

    ui->Stack_Edit_btnAxisX1->setText(m_NameDefine[1].axisName[X1_AXIS]);
    ui->Stack_Edit_btnAxisY1->setText(m_NameDefine[1].axisName[Y1_AXIS]);
    ui->Stack_Edit_btnAxisZ1->setText(m_NameDefine[1].axisName[Z1_AXIS]);
}

//变量类型个数填充下拉列表框初始化
void Teach::coboxVarSelectVarPreOpItemSet()
{
    ui->coboxVarSelectVarPreOp->clear();
    ui->coboxVarSelectPostOp->clear();
    for(int i=0;i<VAR_TOTAL_NUM;i++)
    {
        ui->coboxVarSelectVarPreOp->addItem(m_NameDefine[1].varName[i]);
        ui->coboxVarSelectPostOp->addItem(m_NameDefine[1].varName[i]);
    }
}
//故障编号输入
void Teach::on_lineEdit_edit_fault_num_textChanged(const QString &arg1)
{
    uint16_t alarmnum=arg1.toUInt();
    if(alarmnum>=1450 && alarmnum<=1499)
    {
        Temp_OtherAlarmCustStruct.alarmNum = alarmnum;
    }
    else
    {
        MainWindow::pMainWindow->showErrorTip(tr("输入值的范围不能超过1450～1499,请重新输入。"),TipMode::ONLY_OK);
        ui->lineEdit_edit_fault_num->setText(QString::number(Temp_OtherAlarmCustStruct.alarmNum));
    }
}
//提示编号输入
void Teach::on_lineEdit_edit_tips_num_textChanged(const QString &arg1)
{
    uint16_t alarmnum=arg1.toUInt();
    if(alarmnum>=1950 && alarmnum<=1999)
    {
        Temp_OtherAlarmCustStruct.alarmNum = alarmnum;
    }
    else
    {
        MainWindow::pMainWindow->showErrorTip(tr("输入值的范围不能超过1450～1499,请重新输入。"),TipMode::ONLY_OK);
        ui->lineEdit_edit_tips_num->setText(QString::number(Temp_OtherAlarmCustStruct.alarmNum));
    }
}
//轴搜索指令轴编号选择
void Teach::on_coboxSearchAxisSelect_currentIndexChanged(int index)
{
    if(index<=3)
    {
        Temp_SearchAxisMoveStruct.axis=index;
    }
    if(index==4 || index == 6)
    {
        Temp_SearchAxisMoveStruct.axis=4;
    }
    if(index==5 || index == 7)
    {
        Temp_SearchAxisMoveStruct.axis=4;
    }
}

//从其他界面切换到教导界面时界面初始化
void Teach::SwitchPageInit()
{
    ui->tabWidget_Teach->setCurrentWidget(ui->tabUniversal);
    ui->stackedWidget_General->setCurrentWidget(ui->General_main_page);//切换到通用主界面
    ui->btnModify->setChecked(false);//切换界面时关闭编辑状态
}

//文件操作权限
