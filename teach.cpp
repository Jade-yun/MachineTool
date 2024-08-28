#include "teach.h"
#include "ui_teach.h"
#include "./method/program_save.h"
#include "./method/cmd.h"
#include "./method/Orderjoint.h"
#include "./Prompt_Dialog_page/prompt_com.h"
#include <QString>
#include <QDebug>
#include "stackedit.h"
#include <QButtonGroup>

uint16_t Pro_CurOpera_Line = 0;								//当前选中行

Teach::Teach(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::Teach)
{
	ui->setupUi(this);

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

    connect(prompt_yes_no_Dialog_ui,SIGNAL(DeleteOrder_Return()),this,SLOT(Teach_File_List_Refresh()));//发射删除成功信号

	ui->stackedWidget_Senior->setCurrentIndex(0);
	ui->stackedWidget_General->setCurrentIndex(0);

    for (NumberEdit* edit : ui->verticalLayout_5->findChildren<NumberEdit*>())
    {
        edit->setDecimalPlaces(0);
    }

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



    ui->tableWgtTeach->setSelectionBehavior(QAbstractItemView::SelectRows); //设置选择行为时每次选择一行
    ui->tableWgtTeach->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置不可编辑
    ui->tableWgtTeach->setSelectionMode(QAbstractItemView::SingleSelection);//设置为单选
    ui->tableWgtTeach->setShowGrid(true); //是否显示网格
    ui->tableWgtTeach->setAlternatingRowColors(true);//开启隔行自动变色
    ui->tableWgtTeach->setPalette(QPalette(QColor(237, 212, 0)));//隔行颜色
    
    
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

    pageInit();
    connect(ui->btnModify, &QPushButton::pressed, this, [=](){
        // 根据当前选中的程序切换到相应的编辑页面
        if (ui->btnModify->isChecked())
        {
            return;
        }
        ui->tabWidget_Teach->setCurrentIndex(0);
//        ui->stkWgtProgram->setCurrentIndex(3);
    });
}

Teach::~Teach()
{
	delete ui;
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
        Temp_AxisMoveOrder[i].speed = 0;
        Temp_AxisMoveOrder[i].advEndDis = 0;
        Temp_AxisMoveOrder[i].advCSpeedDis = 0;
        Temp_AxisMoveOrder[i].advEndFlag = 0;
        Temp_AxisMoveOrder[i].advCSpeedFlag = 0;
        Temp_AxisMoveOrder[i].advCSpeedSpeed = 0;
        Temp_AxisMoveOrder[i].referPointNum = 0;

        m_AxisPar[i].axisMaxPos = 1000000;//测试用，后续删除

    }

    ui->cb_General_X1->setChecked(false);
    ui->cb_General_Y1->setChecked(false);
    ui->cb_General_Z1->setChecked(false);
    ui->cb_General_Y2->setChecked(false);
    ui->cb_General_Z2->setChecked(false);

    ui->lineEdit_General_Position_X1->setText("0.00");
    ui->lineEdit_General_Speed_X1->setText("0");

    ui->lineEdit_General_Position_Y1->setText("0.00");
    ui->lineEdit_General_Speed_Y1->setText("0");

    ui->lineEdit_General_Position_Z1->setText("0.00");
    ui->lineEdit_General_Speed_Z1->setText("0");

    ui->lineEdit_General_Position_Y2->setText("0.00");
    ui->lineEdit_General_Speed_Y2->setText("0");

    ui->lineEdit_General_Position_Z2->setText("0.00");
    ui->lineEdit_General_Speed_Z2->setText("0");

    ui->lineEdit_General_Position_C->setText("0.00");
    ui->lineEdit_General_Speed_C->setText("0");

    ui->lineEdit_General_Position_A->setText("0.00");
    ui->lineEdit_General_Speed_A->setText("0");

    ui->lineEdit_General_Position_B->setText("0.00");
    ui->lineEdit_General_Speed_B->setText("0");
}
//卡爪动作指令参数初始化
void Teach::Pro_ClawActionStructInit(void)
{
    memset(Temp_ClawActionStruct,0,sizeof(Temp_ClawActionStruct));
    ui->btn_General_Refresh_2->setText("原料1夹紧");
    ui->btn_General_Refresh_3->setText("成品1夹紧");
    ui->btn_General_Refresh_4->setText("夹抓1正传");
    ui->cb_General_X1_3->setChecked(false);
    ui->cb_General_X1_4->setChecked(false);
    ui->cb_General_X1_5->setChecked(false);
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
    ui->cb_General_X1_6->setChecked(false);
    ui->cb_General_X1_7->setChecked(false);
    ui->cb_General_X1_8->setChecked(false);
    ui->cb_General_X1_9->setChecked(false);
    ui->cb_General_X1_10->setChecked(false);
    ui->cb_General_X1_11->setChecked(false);
    ui->lineEdit_ReserveOn_2->setText("0.00");
    ui->lineEdit_ReserveOn_3->setText("0.00");
    ui->lineEdit_ReserveOn_4->setText("0.00");
    ui->lineEdit_ReserveOn_5->setText("0.00");
    ui->lineEdit_ReserveOn_6->setText("0.00");
    ui->lineEdit_ReserveOn_7->setText("0.00");
    ui->btn_General_Refresh_5->setText("启动加工1通");
    ui->btn_General_Refresh_6->setText("主轴旋转1通");
    ui->btn_General_Refresh_7->setText("自动门1开");
    ui->btn_General_Refresh_8->setText("主轴定位1通");
    ui->btn_General_Refresh_9->setText("吹气1通");
    ui->btn_General_Refresh_10->setText("卡盘1夹紧");
    memset(Temp_MachineOutStruct,0,sizeof(Temp_MachineOutStruct));
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
    uint16_t i=0;
    //先清空表格并重新设置行数和列数
    ui->tableWgtTeach->clearContents();
    ui->tableWgtTeach->setRowCount(0);
    ui->tableWgtTeach->setColumnCount(2);
    for(i=0;i<m_ProInfo.proNum[m_OperateProNum];i++)
    {
        QTableWidgetItem *Teach_File_List_NumItem = new QTableWidgetItem();
        QTableWidgetItem *Teach_File_List_OrderItem = new QTableWidgetItem();
        Teach_File_List_NumItem->setText(JointRunOrderNum(m_OperateProOrder[i]));
        Teach_File_List_NumItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);//设置执行行号内容居中显示
        Teach_File_List_OrderItem->setText(JointStrDeal(m_OperateProOrder[i]));//拼接显示内容

        ui->tableWgtTeach->insertRow(i);
        ui->tableWgtTeach->setItem(i,0,Teach_File_List_NumItem);   //显示命令执行序号
        ui->tableWgtTeach->setItem(i,1,Teach_File_List_OrderItem);                 //显示命令内容
    }
    ui->tableWgtTeach->selectRow(Pro_CurOpera_Line);
}

void Teach::on_btn_Tags_clicked()
{
	ui->stackedWidget_Senior->setCurrentIndex(1);
}

void Teach::on_btn_Logic_clicked()
{
	ui->stackedWidget_Senior->setCurrentIndex(2);
}

void Teach::on_btn_Search_clicked()
{
	ui->stackedWidget_Senior->setCurrentIndex(3);
}

void Teach::on_btn_Shift_clicked()
{
	ui->stackedWidget_Senior->setCurrentIndex(4);
}

void Teach::on_btn_TorqueProtection_clicked()
{
	ui->stackedWidget_Senior->setCurrentIndex(5);
}

void Teach::on_btn_ServoStop_clicked()
{
	ui->stackedWidget_Senior->setCurrentIndex(6);
}

void Teach::on_btn_Subroutine_clicked()
{
	ui->stackedWidget_Senior->setCurrentIndex(7);
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
    Pro_StackResetZeroStructInit();//堆叠解厄面参数初始化
}

void Teach::on_btn_General_ReserveOutput_clicked()
{
	ui->stackedWidget_General->setCurrentIndex(3);
}

void Teach::on_btn_General_SpeedTeach_clicked()
{
	ui->stackedWidget_General->setCurrentIndex(4);
}

void Teach::on_btn_General_SignalWait_clicked()
{
	ui->stackedWidget_General->setCurrentIndex(5);
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

//教导界面-删除按钮处理函数
void Teach::on_btnDelete_clicked()
{

//    m_OperateProOrderListNum = ui->tableWgtTeach->currentIndex().row();//获取当前选中行号，从0开始
//    prompt_yes_no_Dialog_ui->show();
//    prompt_yes_no_Dialog_ui->ui->yes_no_content_label->setText(Return_DoubleButtonDisplay_string(DOUBLE_BUTTON_01));
//    LCD_DoubleButton_Num = DOUBLE_BUTTON_01;
}
//教导界面-分解按钮处理函数
void Teach::on_btnDevide_clicked()
{
    m_OperateProOrderListNum = ui->tableWgtTeach->currentIndex().row();//获取当前选中行号，从0开始
    g_ProOrderOperate(m_OperateProNum,m_OperateProOrderListNum,3,0);//分解处理函数
    Teach_File_List_Refresh();//刷新程序列表
}
//教导界面-组合按钮处理函数
void Teach::on_btnCombine_clicked()
{
    m_OperateProOrderListNum = ui->tableWgtTeach->currentIndex().row();//获取当前选中行号，从0开始
    g_ProOrderOperate(m_OperateProNum,m_OperateProOrderListNum,2,1);//date:0-向下组合 1-向上组合
    Teach_File_List_Refresh();//刷新程序列表
}
//教导界面-上移按钮处理函数
void Teach::on_btnMoveUp_clicked()
{
    m_OperateProOrderListNum = ui->tableWgtTeach->currentIndex().row();//获取当前选中行号，从0开始
    g_ProOrderOperate(m_OperateProNum,m_OperateProOrderListNum,4,1);//每次上移一行
    Teach_File_List_Refresh();//刷新程序列表
}
//教导界面-下移按钮处理函数
void Teach::on_btnMoveDown_clicked()
{
    m_OperateProOrderListNum = ui->tableWgtTeach->currentIndex().row();//获取当前选中行号，从0开始
    g_ProOrderOperate(m_OperateProNum,m_OperateProOrderListNum,5,1);//每次下移一行
    Teach_File_List_Refresh();//刷新程序列表
}
//教导界面-屏蔽按钮处理函数
void Teach::on_btnShield_clicked()
{

    m_OperateProOrderListNum = ui->tableWgtTeach->currentIndex().row();//获取当前选中行号，从0开始
    g_ProOrderOperate(m_OperateProNum,m_OperateProOrderListNum,6,!(m_OperateProOrder[m_OperateProOrderListNum].noteFlag));
    Teach_File_List_Refresh();//刷新程序列表
}
//教导界面-插入按钮处理函数
void Teach::on_btnInset_clicked()
{
    Pro_CurOpera_Line=ui->tableWgtTeach->currentIndex().row();
    uint8_t data_Temp[64] = {0};
    memset(data_Temp,0,sizeof(data_Temp));

    if(ui->stackedWidget_General->currentWidget() == ui->Axis_Claw_Test_Mach_page)
    {
        if(ui->stackedWidget_Four->currentWidget() == ui->Axis_Move_main_page)
        {//如果当前界面在轴动作指令界面
            for(uint8_t i=0; i<AXIS_TOTAL_NUM; i++)
            {
                if(Temp_AxisMoveOrder[i].axis < AXIS_TOTAL_NUM)
                {//选中轴
                    data_Temp[0] = m_OperateProNum;  //程序编号
                    data_Temp[1] = m_OperateProOrder[Pro_CurOpera_Line].list;
                    data_Temp[2] = m_OperateProOrder[Pro_CurOpera_Line].list>>8;
                    data_Temp[3] = m_OperateProOrder[Pro_CurOpera_Line].runOrderNum;
                    data_Temp[4] = m_OperateProOrder[Pro_CurOpera_Line].runOrderNum>>8;
                    data_Temp[5] = C_AXIS_MOVE;
                    data_Temp[6] = m_OperateProOrder[Pro_CurOpera_Line].noteFlag;
                    data_Temp[7] = m_OperateProOrder[Pro_CurOpera_Line].delay;
                    data_Temp[8] = m_OperateProOrder[Pro_CurOpera_Line].delay>>8;
                    data_Temp[9] = Temp_AxisMoveOrder[i].pos;
                    data_Temp[10] = Temp_AxisMoveOrder[i].pos>>8;
                    data_Temp[11] = Temp_AxisMoveOrder[i].pos>>16;
                    data_Temp[12] = Temp_AxisMoveOrder[i].pos>>24;
                    data_Temp[13] = Temp_AxisMoveOrder[i].advEndDis;
                    data_Temp[14] = Temp_AxisMoveOrder[i].advEndDis>>8;
                    data_Temp[15] = Temp_AxisMoveOrder[i].advEndDis>>16;
                    data_Temp[16] = Temp_AxisMoveOrder[i].advEndDis>>24;
                    data_Temp[17] = Temp_AxisMoveOrder[i].advCSpeedDis;
                    data_Temp[18] = Temp_AxisMoveOrder[i].advCSpeedDis>>8;
                    data_Temp[19] = Temp_AxisMoveOrder[i].advCSpeedDis>>16;
                    data_Temp[20] = Temp_AxisMoveOrder[i].advCSpeedDis>>24;
                    data_Temp[21] = Temp_AxisMoveOrder[i].axis;
                    data_Temp[22] = Temp_AxisMoveOrder[i].speed;
                    data_Temp[23] = Temp_AxisMoveOrder[i].advEndFlag;
                    data_Temp[24] = Temp_AxisMoveOrder[i].advCSpeedFlag;
                    data_Temp[25] = Temp_AxisMoveOrder[i].advCSpeedSpeed;
                    data_Temp[26] = Temp_AxisMoveOrder[i].referPointNum;
                    g_InsertProOrder(data_Temp);
                    Teach_File_List_Refresh();//刷新程序列表
                }
            }
        }
        else if(ui->stackedWidget_Four->currentWidget() == ui->Claw_page){//卡爪界面
            data_Temp[0] = m_OperateProNum;  //程序编号
            data_Temp[1] = m_OperateProOrder[Pro_CurOpera_Line].list;
            data_Temp[2] = m_OperateProOrder[Pro_CurOpera_Line].list>>8;
            data_Temp[3] = m_OperateProOrder[Pro_CurOpera_Line].runOrderNum;
            data_Temp[4] = m_OperateProOrder[Pro_CurOpera_Line].runOrderNum>>8;
            data_Temp[5] = C_CLAW_ACTION;
            data_Temp[6] = m_OperateProOrder[Pro_CurOpera_Line].noteFlag;
            data_Temp[7] = m_OperateProOrder[Pro_CurOpera_Line].delay;
            data_Temp[8] = m_OperateProOrder[Pro_CurOpera_Line].delay>>8;
            if(ui->cb_General_X1_3->isChecked())
            {//原料1夹紧
                data_Temp[9] = Temp_ClawActionStruct[0].outportNum;//输出端口号
                data_Temp[10] = Temp_ClawActionStruct[0].type;
                g_InsertProOrder(data_Temp);
            }
            else if(ui->cb_General_X1_4->isChecked())
            {
                data_Temp[9] = Temp_ClawActionStruct[1].outportNum;//输出端口号
                data_Temp[10] = Temp_ClawActionStruct[1].type;
                g_InsertProOrder(data_Temp);
            }
            else if(ui->cb_General_X1_5->isChecked())
            {
                data_Temp[9] = Temp_ClawActionStruct[2].outportNum;//输出端口号
                data_Temp[10] = Temp_ClawActionStruct[2].type;
                g_InsertProOrder(data_Temp);
            }
            Teach_File_List_Refresh();//刷新程序列表
        }
        else if(ui->stackedWidget_Four->currentWidget() == ui->Signal_detect_page){//信号检测界面
            data_Temp[0] = m_OperateProNum;  //程序编号
            data_Temp[1] = m_OperateProOrder[Pro_CurOpera_Line].list;
            data_Temp[2] = m_OperateProOrder[Pro_CurOpera_Line].list>>8;
            data_Temp[3] = m_OperateProOrder[Pro_CurOpera_Line].runOrderNum;
            data_Temp[4] = m_OperateProOrder[Pro_CurOpera_Line].runOrderNum>>8;
            data_Temp[6] = m_OperateProOrder[Pro_CurOpera_Line].noteFlag;
            data_Temp[7] = m_OperateProOrder[Pro_CurOpera_Line].delay;
            data_Temp[8] = m_OperateProOrder[Pro_CurOpera_Line].delay>>8;
            if(ui->tabWidget_Reserve->currentWidget() == ui->tabDoubleValve)
            {//卡爪检测界面
                data_Temp[5] = C_CLAW_CHECK;
                if(ui->cb_General_PassStart->isChecked())
                {//夹紧检测开始
                    data_Temp[9] = 0;//检测端口号，测试用，后期修改
                    data_Temp[10] = 1;
                    g_InsertProOrder(data_Temp);
                }
                else if(ui->cb_General_PassStop->isChecked())
                {//夹紧检测结束
                    data_Temp[9] = 0;//检测端口号，测试用，后期修改
                    data_Temp[10] = 0;
                    g_InsertProOrder(data_Temp);
                }
                else if(ui->cb_General_PineStart->isChecked())
                {//松开检测开始
                    data_Temp[9] = 0;//检测端口号，测试用，后期修改
                    data_Temp[10] = 2;
                    g_InsertProOrder(data_Temp);
                }
                else if(ui->cb_General_PineEnd->isChecked())
                {//松开检测结束
                    data_Temp[9] = 0;//检测端口号，测试用，后期修改
                    data_Temp[10] = 0;
                    g_InsertProOrder(data_Temp);
                }
            }
            else if(ui->tabWidget_Reserve->currentWidget() == ui->tabOutputType)
            {//预留检测界面
                data_Temp[5] = C_RESERVE_CHECK;
                if(ui->cb_General_ReserveOn->isChecked())
                {//预留检测ON
                    data_Temp[9] = 0;//检测端口号，测试用，后期修改
                    data_Temp[10] = 0;
                    g_InsertProOrder(data_Temp);
                }
                else if(ui->cb_General_ReserveOff->isChecked())
                {//预留检测OFF
                    data_Temp[9] = 0;//检测端口号，测试用，后期修改
                    data_Temp[10] = 1;
                    g_InsertProOrder(data_Temp);
                }
                else if(ui->cb_General_ReserveEnd->isChecked())
                {//预留检测END
                    data_Temp[9] = 0;//检测端口号，测试用，后期修改
                    data_Temp[10] = 2;
                    g_InsertProOrder(data_Temp);
                }
            }
            Teach_File_List_Refresh();//刷新程序列表
        }
        else if(ui->stackedWidget_Four->currentWidget() == ui->Machine_action_page){//机床动作界面
            data_Temp[0] = m_OperateProNum;  //程序编号
            data_Temp[1] = m_OperateProOrder[Pro_CurOpera_Line].list;
            data_Temp[2] = m_OperateProOrder[Pro_CurOpera_Line].list>>8;
            data_Temp[3] = m_OperateProOrder[Pro_CurOpera_Line].runOrderNum;
            data_Temp[4] = m_OperateProOrder[Pro_CurOpera_Line].runOrderNum>>8;
            data_Temp[5] = C_MACHINE_OUT;
            data_Temp[6] = m_OperateProOrder[Pro_CurOpera_Line].noteFlag;
    //        data_Temp[7] = m_OperateProOrder[Pro_CurOpera_Line].delay;
    //        data_Temp[8] = m_OperateProOrder[Pro_CurOpera_Line].delay>>8;
            if(ui->cb_General_X1_6->isChecked()){//启动加工1通/断-选中
                data_Temp[7] = ui->lineEdit_ReserveOn_2->text().toUInt();
                data_Temp[8] = ui->lineEdit_ReserveOn_2->text().toUInt()>>8;
                data_Temp[9] = 0;//输出端口号，后面修改
                data_Temp[10] = Temp_MachineOutStruct[0].type;
                g_InsertProOrder(data_Temp);
            }
            else if(ui->cb_General_X1_7->isChecked()){//主轴旋转1通/断-选中
                data_Temp[7] = ui->lineEdit_ReserveOn_3->text().toUInt();
                data_Temp[8] = ui->lineEdit_ReserveOn_3->text().toUInt()>>8;
                data_Temp[9] = 0;//输出端口号，后面修改
                data_Temp[10] = Temp_MachineOutStruct[1].type;
                g_InsertProOrder(data_Temp);
            }
            else if(ui->cb_General_X1_8->isChecked()){//主轴定位1通/断-选中
                data_Temp[7] = ui->lineEdit_ReserveOn_4->text().toUInt();
                data_Temp[8] = ui->lineEdit_ReserveOn_4->text().toUInt()>>8;
                data_Temp[9] = 0;//输出端口号，后面修改
                data_Temp[10] = Temp_MachineOutStruct[2].type;
                g_InsertProOrder(data_Temp);
            }
            else if(ui->cb_General_X1_9->isChecked()){//自动门1开/关-选中
                data_Temp[7] = ui->lineEdit_ReserveOn_5->text().toUInt();
                data_Temp[8] = ui->lineEdit_ReserveOn_5->text().toUInt()>>8;
                data_Temp[9] = 0;//输出端口号，后面修改
                data_Temp[10] = Temp_MachineOutStruct[3].type;
                g_InsertProOrder(data_Temp);
            }
            else if(ui->cb_General_X1_10->isChecked()){//吹气1通/断-选中
                data_Temp[7] = ui->lineEdit_ReserveOn_6->text().toUInt();
                data_Temp[8] = ui->lineEdit_ReserveOn_6->text().toUInt()>>8;
                data_Temp[9] = 0;//输出端口号，后面修改
                data_Temp[10] = Temp_MachineOutStruct[4].type;
                g_InsertProOrder(data_Temp);
            }
            else if(ui->cb_General_X1_11->isChecked()){//卡盘1夹紧/松开-选中
                data_Temp[7] = ui->lineEdit_ReserveOn_7->text().toUInt();
                data_Temp[8] = ui->lineEdit_ReserveOn_7->text().toUInt()>>8;
                data_Temp[9] = 0;//输出端口号，后面修改
                data_Temp[10] = Temp_MachineOutStruct[5].type;
                g_InsertProOrder(data_Temp);
            }
            Teach_File_List_Refresh();//刷新程序列表
        }
    }
    else if(ui->stackedWidget_General->currentWidget() == ui->Stack_page){//堆叠界面
        data_Temp[0] = m_OperateProNum;  //程序编号
        data_Temp[1] = m_OperateProOrder[Pro_CurOpera_Line].list;
        data_Temp[2] = m_OperateProOrder[Pro_CurOpera_Line].list>>8;
        data_Temp[3] = m_OperateProOrder[Pro_CurOpera_Line].runOrderNum;
        data_Temp[4] = m_OperateProOrder[Pro_CurOpera_Line].runOrderNum>>8;
        data_Temp[5] = C_STACK_MOVE;
        data_Temp[6] = m_OperateProOrder[Pro_CurOpera_Line].noteFlag;
        data_Temp[7] = m_OperateProOrder[Pro_CurOpera_Line].delay;
        data_Temp[8] = m_OperateProOrder[Pro_CurOpera_Line].delay>>8;
        if(ui->Stack_group_1->isChecked())
        {
            data_Temp[33] = 1;//堆叠组号，1-8
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
}
//教导界面-保存按钮处理函数
void Teach::on_btnSave_clicked()
{
    saveProgram(m_CurrentProgramNameAndPath);
    Teach_File_List_Refresh();//刷新程序列表
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

//轴动作-A轴复选框处理函数
void Teach::on_cb_General_A_clicked(bool checked)
{
    if(checked == true)
    {//如果选中主引拔A轴
        Temp_AxisMoveOrder[4].axis = Y2_AXIS;
    }
    else if(checked == false)
    {
        Temp_AxisMoveOrder[4].axis = 6;
    }
}
//轴动作-B轴复选框处理函数
void Teach::on_cb_General_B_clicked(bool checked)
{
    if(checked == true)
    {//如果选中主引拔B轴
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
//B-速度
void Teach::on_lineEdit_General_Speed_B_editingFinished()
{
    Temp_AxisMoveOrder[Z2_AXIS].speed = ui->lineEdit_General_Speed_Z2->text().toInt();
}
//A-速度
void Teach::on_lineEdit_General_Speed_A_editingFinished()
{
    Temp_AxisMoveOrder[Y2_AXIS].speed = ui->lineEdit_General_Speed_Y2->text().toInt();
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
//        ui->lineEdit_General_Position_Y1->setText(QString::number((float)(Temp_AxisMoveOrder[Y1_AXIS].pos)/100));
        //可添加提示超出最大范围
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
//        ui->lineEdit_General_Position_Z1->setText(QString::number((float)(Temp_AxisMoveOrder[Z1_AXIS].pos)/100));
        //可添加提示超出最大范围
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
//        ui->lineEdit_General_Position_Y2->setText(QString::number((float)(Temp_AxisMoveOrder[Y2_AXIS].pos)/100));
//        //可添加提示超出最大范围
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
//        ui->lineEdit_General_Position_Z2->setText(QString::number((float)(Temp_AxisMoveOrder[Z2_AXIS].pos)/100));
        //可添加提示超出最大范围
    }
}

//原料1松开/夹紧
void Teach::on_btn_General_Refresh_2_clicked()
{
    if(Temp_ClawActionStruct[0].type == 0)
    {
        ui->btn_General_Refresh_2->setText("原料1松开");
    }
    else
    {
        ui->btn_General_Refresh_2->setText("原料1夹紧");
    }
    Temp_ClawActionStruct[0].type = !Temp_ClawActionStruct[0].type;
}
//成品1松开/夹紧
void Teach::on_btn_General_Refresh_3_clicked()
{
    if(Temp_ClawActionStruct[1].type == 0)
    {
        ui->btn_General_Refresh_3->setText("成品1松开");
    }
    else
    {
        ui->btn_General_Refresh_3->setText("成品1夹紧");
    }
    Temp_ClawActionStruct[1].type = !Temp_ClawActionStruct[1].type;
}
//卡爪正转/反转
void Teach::on_btn_General_Refresh_4_clicked()
{
    if(Temp_ClawActionStruct[2].type == 0)
    {
        ui->btn_General_Refresh_4->setText("卡爪1正转");
    }
    else
    {
        ui->btn_General_Refresh_4->setText("卡爪1反转");
    }
    Temp_ClawActionStruct[2].type = !Temp_ClawActionStruct[2].type;
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
void Teach::on_btn_General_Refresh_5_clicked()
{
    Temp_MachineOutStruct[0].type = !Temp_MachineOutStruct[0].type;
    if(Temp_MachineOutStruct[0].type == 0)
    {
        ui->btn_General_Refresh_5->setText("启动加工1通");
    }
    else if(Temp_MachineOutStruct[0].type == 1)
    {
        ui->btn_General_Refresh_5->setText("启动加工1断");
    }

}
//主轴旋转1通/断按键
void Teach::on_btn_General_Refresh_6_clicked()
{
    Temp_MachineOutStruct[1].type = !Temp_MachineOutStruct[1].type;
    if(Temp_MachineOutStruct[1].type == 0)
    {
        ui->btn_General_Refresh_6->setText("主轴旋转1通");
    }
    else if(Temp_MachineOutStruct[1].type == 1)
    {
        ui->btn_General_Refresh_6->setText("主轴旋转1断");
    }
}
//主轴定位1通/断按键
void Teach::on_btn_General_Refresh_7_clicked()
{
    Temp_MachineOutStruct[2].type = !Temp_MachineOutStruct[2].type;
    if(Temp_MachineOutStruct[2].type == 0)
    {
        ui->btn_General_Refresh_7->setText("主轴定位1通");
    }
    else if(Temp_MachineOutStruct[2].type == 1)
    {
        ui->btn_General_Refresh_7->setText("主轴定位1断");
    }
}
//自动门1开/关
void Teach::on_btn_General_Refresh_9_clicked()
{
    Temp_MachineOutStruct[3].type = !Temp_MachineOutStruct[3].type;
    if(Temp_MachineOutStruct[3].type == 0)
    {
        ui->btn_General_Refresh_9->setText("自动门1开");
    }
    else if(Temp_MachineOutStruct[3].type == 1)
    {
        ui->btn_General_Refresh_9->setText("自动门1关");
    }
}
//吹气1通/断
void Teach::on_btn_General_Refresh_8_clicked()
{
    Temp_MachineOutStruct[4].type = !Temp_MachineOutStruct[4].type;
    if(Temp_MachineOutStruct[4].type == 0)
    {
        ui->btn_General_Refresh_8->setText("吹气1通");
    }
    else if(Temp_MachineOutStruct[4].type == 1)
    {
        ui->btn_General_Refresh_8->setText("吹气1断");
    }
}
//卡盘1夹紧/松开
void Teach::on_btn_General_Refresh_10_clicked()
{
    Temp_MachineOutStruct[4].type = !Temp_MachineOutStruct[4].type;
    if(Temp_MachineOutStruct[4].type == 0)
    {
        ui->btn_General_Refresh_10->setText("卡盘1夹紧");
    }
    else if(Temp_MachineOutStruct[4].type == 1)
    {
        ui->btn_General_Refresh_10->setText("卡盘1松开");
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
            ui->editConstantVarPostOp->setDecimalPlaces(2);
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
            ui->coboxReturnLabel->show();
            ui->editReturnStepNum->hide();
        }
    });
    connect(ui->chboxReturnStepNum, &QCheckBox::stateChanged, [=](int state){
        if (state == Qt::Checked)
        {
            ui->coboxReturnLabel->hide();
            ui->editReturnStepNum->show();
        }
    });

}

void Teach::on_chboxReturnStepNum_stateChanged(int arg1)
{

}

void Teach::on_btnAlarmLight_toggled(bool checked)
{
    qDebug() << "btnAlarmLigh: " << checked;
}
