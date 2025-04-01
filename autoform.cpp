#include "autoform.h"
#include "ui_autoform.h"
#include <QMenu>
#include <QAction>
#include <QDebug>
#include <QScrollBar>

#include "Orderjoint.h"
#include "manualform.h"
#include "customedit.h"
#include "program_save.h"
#include "errortipdialog.h"
#include "mainwindow.h"

AutoForm::AutoForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AutoForm)
{
    ui->setupUi(this);

    ui->btnIntelTorque->setEnabled(false);


    ui->AutoEditAxisSpeed->setDecimalPlaces(0);
    ui->AutoEditAxisPos->setDecimalPlaces(2);
    ui->AutoEditAxisDelay->setDecimalPlaces(2);

    //列表初始化设置
    ui->Auto_file_List->setSelectionBehavior(QAbstractItemView::SelectRows); //设置选择行为时每次选择一行
    ui->Auto_file_List->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置不可编辑
    ui->Auto_file_List->setSelectionMode(QAbstractItemView::SingleSelection);//设置为单选
    ui->Auto_file_List->setShowGrid(true); //是否显示网格
    ui->Auto_file_List->setAlternatingRowColors(true);//开启隔行自动变色
    ui->Auto_file_List->setPalette(QPalette(QColor(237, 212, 0)));//隔行颜色
    ui->Auto_file_List->setColumnWidth(0,80);
   //隐藏断点位置和起始步号
    ui->labBreakPointPos->hide();
    ui->labBreakPointPos0->hide();
    ui->labStartStep->hide();
    ui->labStartStep0->hide();

    // init some members;
    stackSet = new StackSetDialog(this);
    clearDialog = new ClearDialog;
    clearDialog->hide();
    productHisDlg = new ProductHistoryDialog(this);

    // 初始化关闭 framGlobalSpeed
    ui->frameGlobalSpeed->close();


    // 代码完善 framGlobalSpeed 弹窗小界面
    StackSpeedFloatPoP= new QTableWidget(ui->frameGlobalSpeed);
    StackSpeedFloatPoP->setGeometry(5, 5, 110 * 5 + 50, 40 *2);
    StackSpeedFloatPoP->setColumnCount(5);
    StackSpeedFloatPoP->setRowCount(1);
    for (int i = 0; i < StackSpeedFloatPoP->columnCount(); i++)
        StackSpeedFloatPoP->setColumnWidth(i, 110);
    StackSpeedFloatPoP->setRowHeight(0, 40);
    StackSpeedFloatPoP->verticalHeader()->setVisible(false); // Hide vertical header
    StackSpeedFloatPoP->setHorizontalHeaderLabels({tr("堆叠组号"), tr("X1轴个数"), tr("Y1轴个数"), tr("Z1轴个数"),tr("堆叠完停止")});
    StackSpeedFloatPoP->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//隐藏滚动条
    StackSpeedFloatPoP->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    StackSpeedFloatPoP->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

  /*************************变量状态******************************************/
    ui->tableWgtVarState->setHorizontalHeaderLabels({tr("变量"), tr("堆叠状态")});
    ui->tableWgtVarState->horizontalHeader()->setVisible(true);
    ui->tableWgtVarState->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    for (int i=0;i<ui->tableWgtVarState->rowCount();i++)
    {
        VarStateEdit[i] = new NumberEdit(this);
        VarStateEdit.at(i)->setDecimalPlaces(m_VariableType[i]);//根据每个变量的小数类型进行设置小数点
        ui->tableWgtVarState->setCellWidget(i,0,VarStateEdit.at(i));
        connect(VarStateEdit.at(i),&NumberEdit::returnPressed,[=](){
            if(m_VariableType[i] == 0)
            {
                m_VariableCurValue[i] = (uint32_t)VarStateEdit.at(i)->text().toUInt();
            }
            else if(m_VariableType[i] == 1)
            {
                m_VariableCurValue[i] = (uint32_t)VarStateEdit.at(i)->text().toDouble()*10;
            }
            else if(m_VariableType[i] == 2)
            {
                m_VariableCurValue[i] = (uint32_t)VarStateEdit.at(i)->text().toDouble()*100;
            }
            g_Usart->ExtendSendParDeal(CMD_MAIN_STA,CMD_SUN_STA_VAR,0,0);//发送变量值
        });
    }
    for(int i=0;i<STACK_TOTAL_NUM;i++)
    {
        AtackStateEdit[i] = new QTableWidgetItem();
        AtackStateEdit[i]->setFlags(AtackStateEdit[i]->flags() & ~Qt::ItemIsSelectable & ~Qt::ItemIsEnabled);
        ui->tableWgtVarState->setItem(i,1,AtackStateEdit[i]);
    }
    //自动状态界面初始化
    AutoStateMode = new QStandardItemModel(0, 4, this);  // 0行4列
    AutoStateMode->setHorizontalHeaderLabels({tr("个数"), "X", "Y", "Z"});
    ui->AutoStatetableView->setModel(AutoStateMode);
    //设置根据最大宽度平分大小
    ui->AutoStatetableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //设置根据最大高度平分大小
    ui->AutoStatetableView->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->AutoStatetableView->verticalHeader()->setVisible(false);//隐藏垂直表头
    ui->AutoStatetableView->setShowGrid(false);//隐藏所有网洛
    //设置不能选择
    ui->AutoStatetableView->setSelectionMode(QAbstractItemView::NoSelection);
    ui->AutoStatetableView->setSelectionBehavior(QAbstractItemView::SelectItems);
    // 禁用编辑功能
    ui->AutoStatetableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // 移除焦点框
    ui->AutoStatetableView->setFocusPolicy(Qt::NoFocus);

    //悬浮小弹窗初始化
    FloatPoP_coboxStkGroupNum = new QComboBox(this);
    FloatPoP_editX1Num = new NumberEdit(this);
    FloatPoP_editY1Num = new NumberEdit(this);
    FloatPoP_editZ1Num = new NumberEdit(this);
    FloatPoP_coboxStkFinishStop = new QComboBox(this);

    FloatPoP_coboxStkGroupNum->clear();
    FloatPoP_coboxStkFinishStop->clear();
    FloatPoP_editX1Num->setDecimalPlaces(0);
    FloatPoP_editY1Num->setDecimalPlaces(0);
    FloatPoP_editZ1Num->setDecimalPlaces(0);

    StackSpeedFloatPoP->setCellWidget(0, 0, FloatPoP_coboxStkGroupNum);
    StackSpeedFloatPoP->setCellWidget(0, 1, FloatPoP_editX1Num);
    StackSpeedFloatPoP->setCellWidget(0, 2, FloatPoP_editY1Num);
    StackSpeedFloatPoP->setCellWidget(0, 3, FloatPoP_editZ1Num);
    StackSpeedFloatPoP->setCellWidget(0, 4, FloatPoP_coboxStkFinishStop);
    connect(FloatPoP_coboxStkGroupNum,&QComboBox::currentTextChanged,[=](){
        uint8_t num = FloatPoP_coboxStkGroupNum->currentText().toUInt();
        if(num>0 && num <= 8)
        {
            FloatPoP_editX1Num->setInputRange(0, m_StackInfo[num-1].stackPointNum[STACK_X_AXIS]);
            FloatPoP_editY1Num->setInputRange(0, m_StackInfo[num-1].stackPointNum[STACK_Y_AXIS]);
            FloatPoP_editZ1Num->setInputRange(0, m_StackInfo[num-1].stackPointNum[STACK_Z_AXIS]);
            if(FloatPoP_coboxStkFinishStop->count() == 2)
            {
                FloatPoP_coboxStkFinishStop->setCurrentIndex(m_StackInfo[num].stackFinStopFlag);
            }
        }
    });
    connect(FloatPoP_coboxStkFinishStop,&QComboBox::currentTextChanged,[=](){
        uint8_t num = FloatPoP_coboxStkGroupNum->currentText().toUInt();
        if(FloatPoP_coboxStkFinishStop->count()==2)
        {
            m_StackInfo[num-1].stackFinStopFlag = FloatPoP_coboxStkFinishStop->currentIndex();
        }
    });
    connect(FloatPoP_coboxStkFinishStop,QOverload<int>::of(&QComboBox::activated),[=](int index){
            if(FloatPoP_coboxStkFinishStop->count()==2)
            {
                m_StackInfo[index].stackFinStopFlag = FloatPoP_coboxStkFinishStop->currentIndex();
                g_Usart->ExtendSendParDeal(CMD_MAIN_STACK,CMD_SUN_STACK_SET);
            }
        });
    connect(FloatPoP_editX1Num,&NumberEdit::returnPressed,[=](){
        if(FloatPoP_coboxStkGroupNum->count()>0)
        {
            uint8_t stackgroupnum = FloatPoP_coboxStkGroupNum->currentText().toUInt();
            if(stackgroupnum>0 && stackgroupnum<=8)
            {
                m_StackAxisCnt[stackgroupnum-1][0] = (uint16_t)FloatPoP_editX1Num->text().toUInt();
                g_Usart->ExtendSendParDeal(CMD_MAIN_STA,CMD_SUN_STA_STACK_NUM_SET,stackgroupnum,0);
            }
        }
    });
    connect(FloatPoP_editY1Num,&NumberEdit::returnPressed,[=](){
        if(FloatPoP_coboxStkGroupNum->count()>0)
        {
            uint8_t stackgroupnum = FloatPoP_coboxStkGroupNum->currentText().toUInt();
            if(stackgroupnum>0 && stackgroupnum<=8)
            {
                m_StackAxisCnt[stackgroupnum-1][1] = (uint16_t)FloatPoP_editY1Num->text().toUInt();
                g_Usart->ExtendSendParDeal(CMD_MAIN_STA,CMD_SUN_STA_STACK_NUM_SET,stackgroupnum,0);
            }
        }
    });
    connect(FloatPoP_editZ1Num,&NumberEdit::returnPressed,[=](){
        if(FloatPoP_coboxStkGroupNum->count()>0)
        {
            uint8_t stackgroupnum = FloatPoP_coboxStkGroupNum->currentText().toUInt();
            if(stackgroupnum>0 && stackgroupnum<=8)
            {
                m_StackAxisCnt[stackgroupnum-1][2] = (uint16_t)FloatPoP_editZ1Num->text().toUInt();
                g_Usart->ExtendSendParDeal(CMD_MAIN_STA,CMD_SUN_STA_STACK_NUM_SET,stackgroupnum,0);
            }
        }
    });
    // 创建菜单并添加菜单项
    QMenu* menu = new QMenu(this);

    QAction* action1 = new QAction(tr("堆叠指定"), this);
    QAction* action2 = new QAction(tr("从此运行"), this);
    QAction* action3 = new QAction(tr("断点设置"), this);
    QAction* action4 = new QAction(tr("断点清除"), this);
    QAction* action5 = new QAction(tr("参考点编辑"), this);
    QAction* action6 = new QAction(tr("自动修正位置"), this);

    operateActions = {
        action1, action2, action3, action4, action5, action6
    };
    actionStrs = {
        "堆叠指定",
        "从此运行",
        "断点设置",
        "断点清除",
        "参考点编辑",
        "自动修正位置"
    };

    menu->addAction(action1);
    menu->addAction(action2);
    menu->addAction(action3);
    menu->addAction(action4);
    menu->addAction(action5);
    menu->addAction(action6);

    const std::vector<QString> iconsPath = {
        ":/images/autoPageImages/stack.png",
        ":/images/autoPageImages/continue.png",
        ":/images/autoPageImages/breakpoint.png",
        ":/images/autoPageImages/clear.png",
        ":/images/autoPageImages/edit.png",
        ":/images/autoPageImages/revise_position.png"
    };

    std::vector<QAction*> actions = {action1, action2, action3, action4, action5, action6};

    const QSize iconSize(25, 25); // Desired size for icons
    for (size_t i = 0; i < actions.size() && i < iconsPath.size(); ++i)
    {
        QIcon icon(iconsPath[i]);
        QPixmap pixmap = icon.pixmap(iconSize);
        actions[i]->setIcon(QIcon(pixmap));
    }

    ui->btnOperate->setMenu(menu);

    referEditDialog = new ReferenceWidget(this);
    referEditDialog->setWindowTitle(tr("参考点编辑"));
    referEditDialog->setWindowModality(Qt::ApplicationModal);

    QWidget* parentWidget = this->parentWidget();
    QRect parentGeometry;
    parentGeometry = parentWidget->geometry();
    int x = parentGeometry.x() + (parentGeometry.width() - referEditDialog->width()) / 2;
    int y = parentGeometry.y() + (parentGeometry.height() - referEditDialog->height()) / 2;
    referEditDialog->move(x, y);


    for (auto cobox : findChildren<QComboBox*>())
    {
        cobox->setFocusPolicy(Qt::NoFocus);
    }

    /***************************************************************/
    // 按钮移动显示
    connect(ui->btnScrollLeft, &QPushButton::clicked, this, [this]() {
        ui->scrollArea->horizontalScrollBar()->setValue(
                    ui->scrollArea->horizontalScrollBar()->value() - 50);
    });

    connect(ui->btnScrollRight, &QPushButton::clicked, this, [this]() {
        ui->scrollArea->horizontalScrollBar()->setValue(
                    ui->scrollArea->horizontalScrollBar()->value() + 50);
    });

/**********************************************************************/
    // 连接菜单项的点击事件
    connect(action1, &QAction::triggered, this, [=]() {
//        qDebug() << "堆叠指定 ";
        stackSet->show();

    });

    connect(action2, &QAction::triggered, this, [=]() {//从改行运行
        if(m_RobotRunSta == MAC_STA_AUTO_INVALID || m_RobotRunSta == MAC_STA_AUTO_STANDBY)
        {//在停止状态下才能设置从此行运行行号
            SetAutoRunParIcon(0);
        }
    });

    connect(action3, &QAction::triggered, this, [=]() {
//        qDebug() << "断点设置 ";
        ui->labBreakPointPos->setText(QString("主程序第%1步").arg(m_CurrentSelectProOrderList+1));
        SetAutoRunParIcon(1);
    });
    connect(action4, &QAction::triggered, this, [=]() {
//        qDebug() << "断点清除 ";
        SetAutoRunParIcon(2);
    });

    // 参考点编辑
    connect(action5, &QAction::triggered, this, &AutoForm::showReferPointDialog);

    connect(action6, &QAction::triggered, this, [=]() {
//        qDebug() << "自动修正位置 ";
//        AutoCorrectPosDialog dialog(this) ;
//        if (dialog.exec() == QDialog::Accepted)
//        {

//        }
    });

/**********************************************************************/
    // these slots are used to adjust global speed
    connect(ui->btnGlobalSpeedAdd, &QPushButton::clicked, [=](){
        int value = ui->proBarGlobalSpeed->value();
        if (value <= 100 && value >= 1 && m_RunPar.allow_globalSpeed == 1)
            ui->proBarGlobalSpeed->setValue(++value);
        m_RunPar.globalSpeed = ui->proBarGlobalSpeed->value();
        g_Usart->ExtendSendParDeal(CMD_MAIN_STA,CMD_SUN_STA_PAR,0,0);
    });
    connect(ui->btnGlobalSpeedSub, &QPushButton::clicked, [=](){
        int value = ui->proBarGlobalSpeed->value();
        if (value <= 100 && value > 1 && m_RunPar.allow_globalSpeed == 1)
            ui->proBarGlobalSpeed->setValue(--value);
        m_RunPar.globalSpeed = ui->proBarGlobalSpeed->value();
        g_Usart->ExtendSendParDeal(CMD_MAIN_STA,CMD_SUN_STA_PAR,0,0);
    });
    connect(ui->btn10percent, &QPushButton::clicked, [=](){
        if (m_RunPar.allow_globalSpeed == 1)
            ui->proBarGlobalSpeed->setValue(10);
        m_RunPar.globalSpeed = ui->proBarGlobalSpeed->value();
        g_Usart->ExtendSendParDeal(CMD_MAIN_STA,CMD_SUN_STA_PAR,0,0);
    });
    connect(ui->btn20percent, &QPushButton::clicked, [=](){
        if (m_RunPar.allow_globalSpeed == 1)
            ui->proBarGlobalSpeed->setValue(20);
        m_RunPar.globalSpeed = ui->proBarGlobalSpeed->value();
        g_Usart->ExtendSendParDeal(CMD_MAIN_STA,CMD_SUN_STA_PAR,0,0);
    });
//    connect(ui->btn20percent, &QPushButton::toggled, [=](bool checked){
//        if (ui->btnAdjustSpeed->isChecked())
//            ui->proBarGlobalSpeed->setValue(20);
//    });
    connect(ui->btn40percent, &QPushButton::clicked, [=](){
        if (m_RunPar.allow_globalSpeed == 1)
            ui->proBarGlobalSpeed->setValue(40);
        m_RunPar.globalSpeed = ui->proBarGlobalSpeed->value();
        g_Usart->ExtendSendParDeal(CMD_MAIN_STA,CMD_SUN_STA_PAR,0,0);
    });
    connect(ui->btn60percent, &QPushButton::clicked, [=](){
        if (m_RunPar.allow_globalSpeed == 1)
            ui->proBarGlobalSpeed->setValue(60);
        m_RunPar.globalSpeed = ui->proBarGlobalSpeed->value();
        g_Usart->ExtendSendParDeal(CMD_MAIN_STA,CMD_SUN_STA_PAR,0,0);
    });
    connect(ui->btn80percent, &QPushButton::clicked, [=](){
        if (m_RunPar.allow_globalSpeed == 1)
            ui->proBarGlobalSpeed->setValue(80);
        m_RunPar.globalSpeed = ui->proBarGlobalSpeed->value();
        g_Usart->ExtendSendParDeal(CMD_MAIN_STA,CMD_SUN_STA_PAR,0,0);
    });
    connect(ui->btn100percent, &QPushButton::clicked, [=](){
        if (m_RunPar.allow_globalSpeed == 1)
            ui->proBarGlobalSpeed->setValue(100);
        m_RunPar.globalSpeed = ui->proBarGlobalSpeed->value();
        g_Usart->ExtendSendParDeal(CMD_MAIN_STA,CMD_SUN_STA_PAR,0,0);
    });
    connect(ui->btnAdjustSpeed,&QPushButton::clicked,this,[=](){
        if(ui->btnAdjustSpeed->isChecked()) {
            m_RunPar.allow_globalSpeed = 1;
        }else {
            m_RunPar.allow_globalSpeed = 0;
        }
    });
/**********************************************************************/
    // 历史信息
    connect(ui->btnHistoryInfo, &QPushButton::clicked, productHisDlg, &ProductHistoryDialog::show);

    // 清零
    connect(ui->btnClear, &QPushButton::clicked, [=](){
        int index = ui->stkWgtState->currentIndex();

        if (index == 0)
        {
            clearDialog->setMode(ClearDialog::AutoState);
            if(clearDialog->exec() == QDialog::Accepted)
            {
                g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_PAR_RESET,0,0);
                if(m_StackFunc.siloType == 1)
                {//如果是旋转式料仓
                    g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_ROTAT_SILO_SET,0,0);
                }
                setAutoPagePar();
            }
        }
        if (index == 1)
        {
            clearDialog->setMode(ClearDialog::VarState);
            if(clearDialog->exec() == QDialog::Accepted)
            {
                g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_VAR_AUTO_RESET,0,0);
                g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_VAR_POWER_OFF_MEMORY,0,0);
            }
            setAutoPagePar();
        }
    });

    connect(ui->btnMergeExpand, &QPushButton::toggled, this, [=](bool checked){
        if (checked)
        {
//            ui->frameTable->show();
            ui->frameGlobalSpeed->show();
    //        qDebug() << " show ...";
        }
        else
        {
//            ui->frameTable->show();
            ui->frameGlobalSpeed->close();
        }
    });

    connect(ui->btnNext, &QPushButton::clicked, this, [=](){
        int index = (ui->stkWgtState->currentIndex() + 1 + ui->stkWgtState->count()) % ui->stkWgtState->count();
        ui->stkWgtState->setCurrentIndex(index);
        updateLabelState(index);
    });

    connect(ui->btnLast, &QPushButton::clicked, this, [=](){
        int index = (ui->stkWgtState->currentIndex() - 1 + ui->stkWgtState->count()) % ui->stkWgtState->count();
        ui->stkWgtState->setCurrentIndex(index);
        updateLabelState(index);

    });
    connect(ui->btnTrial,&QPushButton::clicked,this,[=](){//自动运行界面试行按钮
        if(m_OperateProOrderListNum == 0)
        {//如果未载入程序，直接返回
            return;
        }
        m_CurrentSelectProOrderList = ui->Auto_file_List->currentIndex().row();
        g_Usart->ExtendSendProDeal(CMD_MAIN_PRO,CMD_SUN_PRO_DEBUG,m_OperateProNum,m_CurrentSelectProOrderList+1,m_RunPar.globalSpeed);//运行行号从1开始，为当前选中行序号+1
    });
    connect(ui->coboxProgram, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index) {//自动运行界面程序编号切换
        emit Switch_ProNum_Signal(index);//发送切换程序编号信号，刷新教导界面列表
        g_Usart->ExtendSendProDeal(CMD_MAIN_PRO,CMD_SUN_PRO_INFO);
        g_Usart->ExtendSendProDeal(CMD_MAIN_PRO,CMD_SUN_PRO_SAVE,m_OperateProNum,2);
        Auto_File_List_Refresh(index);//刷新自动界面列表
    });
    connect(ui->AutoEditSavebtn, &QPushButton::clicked,this,&AutoForm::OrderEditSaveHandel);//指令编辑保存

}

AutoForm::~AutoForm()
{
    delete ui;

    delete clearDialog;
}

//更新速度堆叠弹窗界面显示
void AutoForm::UpdateFloatPoPShow()
{
    if(ui->frameGlobalSpeed->isVisible())
    {
        if(FloatPoPStackOrderNumList != processInstructions(0))
        {
            FloatPoP_coboxStkGroupNum->clear();
            FloatPoP_coboxStkFinishStop->clear();
            FloatPoPStackOrderNumList = processInstructions(0);
            FloatPoP_coboxStkGroupNum->clear();
            for(int i=0;i<FloatPoPStackOrderNumList.count();i++)
            {
                FloatPoP_coboxStkGroupNum->addItem(FloatPoPStackOrderNumList.at(i));
            }
            FloatPoP_coboxStkFinishStop->addItems({tr("不使用"), tr("使用")});
        }
        if(FloatPoP_coboxStkGroupNum->count()>0)
        {
            uint8_t stackIndex = FloatPoP_coboxStkGroupNum->currentIndex();
            if(ui->stkWgtState->currentWidget() != ui->page3StackState)
            {
                g_Usart->ExtendSendReadPar(CMD_MAIN_STA,CMD_SUN_STA_ALL_STACK_NUM,1,0);//发送读取所有堆叠位置点数指令
            }
            FloatPoP_editX1Num->setValue(m_StackAxisCnt[stackIndex][0]);
            FloatPoP_editY1Num->setValue(m_StackAxisCnt[stackIndex][1]);
            FloatPoP_editZ1Num->setValue(m_StackAxisCnt[stackIndex][2]);
            FloatPoP_editX1Num->setText(QString::number(m_StackAxisCnt[stackIndex][0]));
            FloatPoP_editY1Num->setText(QString::number(m_StackAxisCnt[stackIndex][1]));
            FloatPoP_editZ1Num->setText(QString::number(m_StackAxisCnt[stackIndex][2]));
            if(FloatPoP_coboxStkFinishStop->count() == 2)
            {
                FloatPoP_coboxStkFinishStop->setCurrentIndex(m_StackInfo[stackIndex].stackFinStopFlag);
            }
        }
        else
        {
            FloatPoP_coboxStkFinishStop->clear();
        }
        if(m_RobotRunSta == MAC_STA_RUN)
        {
            if(StackSpeedFloatPoP->isEnabled() == true)
            {
                StackSpeedFloatPoP->setEnabled(false);
            }
            uint16_t tempProNum = m_ProRunInfo.proNum[m_OperateProNum];
            if(m_OperateProOrder[tempProNum].cmd == C_STACK_MOVE)
            {
                P_StackMoveStruct* StackMove = (P_StackMoveStruct*)m_OperateProOrder[tempProNum].pData;
                uint8_t stackIndex = StackMove->stackNum-1;
                if(stackIndex>=0)
                {
                    for (int i = 0; i < FloatPoP_coboxStkGroupNum->count(); i++) {
                        QString itemText = FloatPoP_coboxStkGroupNum->itemText(i);  // 获取第i项的文本
                        if (itemText == StackMove->stackNum) {
                            FloatPoP_coboxStkGroupNum->setCurrentIndex(i);  // 匹配则选中该项
                            break;  // 找到后退出循环
                        }
                    }
                    FloatPoP_coboxStkFinishStop->setCurrentIndex(m_StackInfo[stackIndex].stackFinStopFlag);
                    FloatPoP_editX1Num->setValue(m_StackAxisCnt[stackIndex][0]);
                    FloatPoP_editY1Num->setValue(m_StackAxisCnt[stackIndex][1]);
                    FloatPoP_editZ1Num->setValue(m_StackAxisCnt[stackIndex][2]);
                    FloatPoP_editX1Num->setText(QString::number(m_StackAxisCnt[stackIndex][0]));
                    FloatPoP_editY1Num->setText(QString::number(m_StackAxisCnt[stackIndex][1]));
                    FloatPoP_editZ1Num->setText(QString::number(m_StackAxisCnt[stackIndex][2]));
                }
            }
        }
        else
        {
            if(StackSpeedFloatPoP->isEnabled() == false)
            {
                StackSpeedFloatPoP->setEnabled(true);
            }
        }
    }
}
//刷新速度显示相关控件
void AutoForm::Refresh_globalSpeedShowHandel()
{
    ui->proBarGlobalSpeed->setValue(m_RunPar.globalSpeed);
}
//自动界面程序列表滑动或点击时处理函数
void AutoForm::on_Auto_file_List_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
    if(currentRow>=0 && currentRow <= m_OperateProOrderListNum)
    {
        if(m_OperateProOrderListNum == 0)
        {//如果未载入程序，直接返回
            return;
        }
        if(ui->Auto_file_List->rowCount() && m_AutoInforRefresh.Program_Follow_Flog == 0)
        {
            m_CurrentSelectProOrderList = currentRow;
            if(ui->btnEdit->isChecked())
            {//如果编辑按钮按下
                OrderEditHandel();
            }
        }
    }
    else
    {
        qDebug()<<"currentRow error:"<<currentRow;
    }

}
//设置自动运行界面操作中的自动运行，断点设置，断点清除图标处理函数
void AutoForm::SetAutoRunParIcon(uint8_t type)
{
    switch (type) {
    case 0://设置从此运行行号图标
    {
        if(m_OperateProOrder[m_CurrentSelectProOrderList].cmd != C_PRO_END)
        {
            m_RunPar.startRunLineFlag = true;
            m_RunPar.startRunLineNum = m_OperateProOrder[m_CurrentSelectProOrderList].list;
            m_RunPar.startRunLineProNum = m_OperateProNum;
            if(m_RunPar.breakPointFlag == true && m_RunPar.startRunLineNum == m_RunPar.breakPointList && m_RunPar.startRunLineProNum == m_RunPar.breakPointProNum)
            {
                m_RunPar.breakPointFlag = 0;
                m_RunPar.breakPointList = 1;
                m_RunPar.breakPointProNum = 0;
            }
            g_Usart->ExtendSendParDeal(CMD_MAIN_STA,CMD_SUN_STA_PAR,0,0);
            //为目标行设置新的图标和文本
            SetAutoRunIcon();
        }
        break;
    }
    case 1://设置断点图标
    {
        if(m_OperateProOrder[m_CurrentSelectProOrderList].cmd != C_PRO_END)
        {
            if(m_OperateProOrder[m_CurrentSelectProOrderList].list  < m_ProInfo.proNum[m_OperateProNum]-1)
            {
                //为目标行设置新的图标和文本
                m_RunPar.breakPointFlag = true;
                m_RunPar.breakPointList = m_OperateProOrder[m_CurrentSelectProOrderList].list;
                m_RunPar.breakPointProNum = m_OperateProNum;
                if(m_RunPar.startRunLineFlag == true && m_RunPar.startRunLineNum == m_RunPar.breakPointList && m_RunPar.startRunLineProNum == m_RunPar.breakPointProNum)
                {
                    m_RunPar.startRunLineFlag = 0;
                    m_RunPar.startRunLineNum = 1;
                    m_RunPar.startRunLineProNum = 0;
                }
                g_Usart->ExtendSendParDeal(CMD_MAIN_STA,CMD_SUN_STA_PAR,0,0);
                SetAutoRunIcon();
            }
            else
            {
                MainWindow::pMainWindow->showErrorTip(tr("最后一步不允许设为断点！"),TipMode::ONLY_OK);
            }
        }
        break;
    }
    case 2://断点清除
    {
        m_RunPar.breakPointFlag = false;
        m_RunPar.breakPointList = 1;
        m_RunPar.breakPointProNum = 0;
        g_Usart->ExtendSendParDeal(CMD_MAIN_STA,CMD_SUN_STA_PAR,0,0);
        SetAutoRunIcon();
        break;
    }
    default:
        break;
    }
}
void AutoForm::SetAutoRunIcon()
{
    for(int row=0;row<ui->Auto_file_List->rowCount();row++)
    {
        QTableWidgetItem *item = ui->Auto_file_List->item(row,0);
        if(item)
        {
            item->setIcon(QIcon());//先清除图标
            item->setTextAlignment(Qt::AlignCenter);//设置内容居中显示
        }
    }
    if(m_RunPar.breakPointFlag == true)
    {
        //为目标行设置新的图标和文本
        if(m_RunPar.breakPointList>0)
        {
            QTableWidgetItem *item = new QTableWidgetItem(QString::number(m_OperateProOrder[m_RunPar.breakPointList-1].runOrderNum));
            QIcon icon(":/images/autoPageImages/breakpoint.png");
            ui->Auto_file_List->setItem(m_RunPar.breakPointList-1,0,item);
            item->setIcon(icon);
            item->setTextAlignment(Qt::AlignCenter);//设置内容居中显示
        }

        ui->labBreakPointPos0->show();
        ui->labBreakPointPos->show();
        ui->labBreakPointPos->setText(QString("主程序第%1步").arg(m_RunPar.breakPointList));
    }
    else
    {
        ui->labBreakPointPos0->hide();
        ui->labBreakPointPos->hide();
    }

    if(m_RunPar.startRunLineFlag == true)
    {
        //为目标行设置新的图标和文本
        if(m_RunPar.startRunLineNum>0)
        {
            QTableWidgetItem *item = new QTableWidgetItem(QString::number(m_OperateProOrder[m_RunPar.startRunLineNum-1].runOrderNum));
            QIcon icon(":/images/autoPageImages/continue.png");
            ui->Auto_file_List->setItem(m_RunPar.startRunLineNum-1,0,item);
            item->setIcon(icon);
            item->setTextAlignment(Qt::AlignCenter);//设置内容居中显示
        }


        ui->labStartStep->show();
        ui->labStartStep0->show();
        ui->labStartStep->setText(QString::number(m_RunPar.startRunLineNum));
    }
    else
    {
        ui->labStartStep->hide();
        ui->labStartStep0->hide();
    }
}
/*************************************************************************
**  函数名：  OrderEditSaveHandel()
**	输入参数：
**	输出参数：
**	函数功能：自动运行界面指令编辑保存处理函数
**  作者：    wukui
**  开发日期：2024/12/27
**************************************************************************/
void AutoForm::OrderEditSaveHandel()
{
    if(ui->stkWgtEdit->currentWidget()==ui->AutoPageAxisEditpage)
    {//轴运动，轴搜索指令编辑保存
        if(m_OperateProOrder[m_CurrentSelectProOrderList].cmd == C_AXIS_MOVE)
        {
            P_AxisMoveStruct* AxisMove = (P_AxisMoveStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
            AxisMove->pos = AxisMove->pos+ui->AutoEditAxisPos->text().toDouble()*100;
            AxisMove->speed = ui->AutoEditAxisSpeed->text().toUInt();
            m_OperateProOrder[m_CurrentSelectProOrderList].delay = ui->AutoEditAxisDelay->text().toDouble()*100;
        }
        else if(m_OperateProOrder[m_CurrentSelectProOrderList].cmd == C_SEARCH_AXIS_MOVE)
        {
            P_SearchAxisMoveStruct* SearchAxisMove = (P_SearchAxisMoveStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
            SearchAxisMove->maxPos = SearchAxisMove->maxPos+ui->AutoEditAxisPos->text().toDouble()*100;
            SearchAxisMove->runSpeed = ui->AutoEditAxisSpeed->text().toUInt();
            m_OperateProOrder[m_CurrentSelectProOrderList].delay = ui->AutoEditAxisDelay->text().toDouble()*100;
        }
        else if(m_OperateProOrder[m_CurrentSelectProOrderList].cmd == C_OFFSET_AXIS)
        {
            P_OffsetAxisStruct* OffsetAxis = (P_OffsetAxisStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
            OffsetAxis->offsetPos = OffsetAxis->offsetPos+ui->AutoEditAxisPos->text().toDouble()*100;
            OffsetAxis->speed = ui->AutoEditAxisSpeed->text().toUInt();
            m_OperateProOrder[m_CurrentSelectProOrderList].delay = ui->AutoEditAxisDelay->text().toDouble()*100;
        }
    }
    else if(ui->stkWgtEdit->currentWidget()==ui->AutoEditDelaypage)
    {
        switch(m_OperateProOrder[m_CurrentSelectProOrderList].cmd)
        {
        case C_CLAW_ACTION:
        case C_RESERVE_OUT:
        case C_WAIT_IN_MACHINE:
        case C_WAIT_IN_CLAW:
        case C_WAIT_IN_RESERVE:
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
            m_OperateProOrder[m_CurrentSelectProOrderList].delay = ui->AutoEditdelay->text().toDouble()*100;
            break;
        }
        default:
            break;
        }
    }
    else if(ui->stkWgtEdit->currentWidget()==ui->AutoEditStack)
    {
        switch(m_OperateProOrder[m_CurrentSelectProOrderList].cmd)
        {
        case C_STACK_MOVE:
        case C_STACK_FOLLOW:
        {
            m_OperateProOrder[m_CurrentSelectProOrderList].delay = ui->AutoEditStackdelay->text().toDouble()*100;
            break;
        }
        default:
            break;
        }
    }
    else if(ui->stkWgtEdit->currentWidget()==ui->AutoEditIfpage)
    {
        if(ui->AutoEditIfSunPage->currentWidget() == ui->AutoEditLoopnumpage && m_OperateProOrder[m_CurrentSelectProOrderList].cmd ==C_LOGIC_WHILE_START)
        {
            P_LogicWhileStartStruct* LogicWhileStart =(P_LogicWhileStartStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
            LogicWhileStart->cycNum =  ui->Autoedloopnum->text().toUInt();
        }
        else if(ui->AutoEditIfSunPage->currentWidget() == ui->AutoEditIfOperandpage)
        {
            if(m_OperateProOrder[m_CurrentSelectProOrderList].cmd == C_LOGIC_VAR)
            {
                P_LogicVarStruct* LogicVar =(P_LogicVarStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
                if(m_VariableType[LogicVar->varNum] == 0)
                {
                    LogicVar->sufferOperValue = ui->AutoeditIfOperand->text().toUInt();
                }
                else if(m_VariableType[LogicVar->varNum] == 1)
                {
                    LogicVar->sufferOperValue = ui->AutoeditIfOperand->text().toDouble()*10;
                }
                else if(m_VariableType[LogicVar->varNum] == 2)
                {
                    LogicVar->sufferOperValue = ui->AutoeditIfOperand->text().toDouble()*100;
                }
            }
            else if(m_OperateProOrder[m_CurrentSelectProOrderList].cmd == C_LOGIC_AXIS)
            {
                P_LogicAxisStruct* LogicAxis =(P_LogicAxisStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
                LogicAxis->sufferOperValue = ui->AutoeditIfOperand->text().toDouble()*100;
            }
            else if(m_OperateProOrder[m_CurrentSelectProOrderList].cmd == C_LOGIC_STACK)
            {
                P_LogicStackStruct* LogicStack =(P_LogicStackStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
                LogicStack->sufferOperValue = ui->AutoeditIfOperand->text().toUInt();
            }
            else if(m_OperateProOrder[m_CurrentSelectProOrderList].cmd == C_LOGIC_PRODUCT)
            {
                P_LogicCurProductNumStruct* LogicCurProductNum =(P_LogicCurProductNumStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
                LogicCurProductNum->sufferOperValue = ui->AutoeditIfOperand->text().toUInt();
            }
        }
    }
    else if(ui->stkWgtEdit->currentWidget()==ui->AutoEditTorquepage)
    {
        if(m_OperateProOrder[m_CurrentSelectProOrderList].cmd == C_TORQUE_GARD)
        {
            P_TorqueGardStruct* TorqueGard =(P_TorqueGardStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
            TorqueGard->torqueValue = ui->AutoEditTorque->text().toUInt();
        }
    }
    else if(ui->stkWgtEdit->currentWidget()==ui->AutoEditTimeLimitpage)
    {
        if(m_OperateProOrder[m_CurrentSelectProOrderList].cmd == C_SUN_PRO)
        {
            m_OperateProOrder[m_CurrentSelectProOrderList].delay = ui->TimeLimitdelay->text().toDouble()*100;
        }
    }

    if(ui->AutoEditchboxIfCondition1->isChecked() == true)
    {
        AutoFromIfOrderEditSaveHandle(0);
    }
    else if(ui->AutoEditchboxIfCondition2->isChecked() == true)
    {
        AutoFromIfOrderEditSaveHandle(1);
    }

    g_TotalProCopy(m_ProOrder[m_OperateProNum],m_OperateProOrder);
    saveProgram(m_CurrentProgramNameAndPath);
    g_Usart->ExtendSendProDeal(CMD_MAIN_PRO,CMD_SUN_PRO_INSERT,m_OperateProNum,m_CurrentSelectProOrderList,1);
    g_Usart->ExtendSendProDeal(CMD_MAIN_PRO,CMD_SUN_PRO_INFO);
    g_Usart->ExtendSendProDeal(CMD_MAIN_PRO,CMD_SUN_PRO_SAVE,m_OperateProNum,1);
    Auto_File_List_Refresh(m_OperateProNum);//刷新程序列表
}
/*************************************************************************
**  函数名：  OrderEditHandel()
**	输入参数：
**	输出参数：
**	函数功能：自动运行界面指令编辑处理函数
**  作者：    wukui
**  开发日期：2024/12/26
**************************************************************************/
void AutoForm::OrderEditHandel()
{
    if(m_CurrentSelectProOrderList<=m_OperateProOrderListNum)
    {
        switch(m_OperateProOrder[m_CurrentSelectProOrderList].cmd)
        {
        case C_AXIS_MOVE:
        {
            ui->stkWgtEdit->setCurrentWidget(ui->AutoPageAxisEditpage);
            ui->AutoEditSavebtn->show();
            P_AxisMoveStruct* AxisMove = (P_AxisMoveStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
            double AxisPos = 0.00;
            double AxisMove_delay = m_OperateProOrder[m_CurrentSelectProOrderList].delay;
            ui->AutoEditAxisPos->setText(QString::number(AxisPos));
            ui->AutoEditAxisSpeed->setText(QString::number(AxisMove->speed));
            ui->AutoEditAxisDelay->setText(QString::number(AxisMove_delay/100,'f',2));
            break;
        }
        case C_SEARCH_AXIS_MOVE:
        {
            ui->stkWgtEdit->setCurrentWidget(ui->AutoPageAxisEditpage);
            ui->AutoEditSavebtn->show();
            P_SearchAxisMoveStruct* SearchAxisMove = (P_SearchAxisMoveStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
            double AxisPos = 0.00;
            double AxisMove_delay = m_OperateProOrder[m_CurrentSelectProOrderList].delay;
            ui->AutoEditAxisPos->setText(QString::number(AxisPos));
            ui->AutoEditAxisSpeed->setText(QString::number(SearchAxisMove->runSpeed));
            ui->AutoEditAxisDelay->setText(QString::number(AxisMove_delay/100,'f',2));
            break;
        }
        case C_OFFSET_AXIS:
        {
            ui->stkWgtEdit->setCurrentWidget(ui->AutoPageAxisEditpage);
            ui->AutoEditSavebtn->show();
            P_OffsetAxisStruct* OffsetAxis =(P_OffsetAxisStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
            double AxisPos = 0.00;
            double AxisMove_delay = m_OperateProOrder[m_CurrentSelectProOrderList].delay;
            ui->AutoEditAxisPos->setText(QString::number(AxisPos));
            ui->AutoEditAxisSpeed->setText(QString::number(OffsetAxis->speed));
            ui->AutoEditAxisDelay->setText(QString::number(AxisMove_delay/100,'f',2));
            break;
        }
        case C_CLAW_ACTION:
        case C_RESERVE_OUT:
        case C_WAIT_IN_MACHINE:
        case C_WAIT_IN_CLAW:
        case C_WAIT_IN_RESERVE:
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
            ui->stkWgtEdit->setCurrentWidget(ui->AutoEditDelaypage);
            ui->AutoEditSavebtn->show();
            double ClawAction_delay = m_OperateProOrder[m_CurrentSelectProOrderList].delay;
            ui->AutoEditdelay->setText(QString::number(ClawAction_delay/100,'f',2));
            break;
        }
        case C_STACK_MOVE:
        case C_STACK_FOLLOW:
        {
            ui->stkWgtEdit->setCurrentWidget(ui->AutoEditStack);
            ui->AutoEditSavebtn->show();
            ui->AutoEditStackX1btn->setChecked(true);
            double ClawAction_delay = m_OperateProOrder[m_CurrentSelectProOrderList].delay;
            ui->AutoEditStackdelay->setText(QString::number(ClawAction_delay/100,'f',2));
            break;
        }
        case C_OTHER_ALARM_CUST:
        case C_OTHER_CYC_STOP:
        case C_LABEL:
        case C_PRO_END:
        case C_LOGIC_WHILE_END:
        case C_LOGIC_TIME:
        {
            ui->stkWgtEdit->setCurrentWidget(ui->NoParModpage);
            break;
        }
        case C_LOGIC_IF:
        {
            ui->stkWgtEdit->setCurrentWidget(ui->AutoEditIfpage);
            ui->AutoEditSavebtn->show();
            ui->AutoEditchboxIfCondition1->setChecked(true);
            AutoFromIfOrderEditHandle(0);
            break;
        }
        case C_LOGIC_ELSE:
        case C_LOGIC_END:
            ui->stkWgtEdit->setCurrentWidget(ui->AutoEditIfpage);
            ui->AutoEditIfSunPage->setCurrentWidget(ui->AutoEditIfNoModifypage);
            ui->AutoEditSavebtn->hide();//隐藏保存按钮
            ui->AutoEditchboxIfCondition1->setChecked(true);
            ui->AutoEditchboxIfCondition2->setEnabled(false);
            break;
        case C_LOGIC_WHILE_START:
        {
            ui->AutoEditSavebtn->show();//保存按钮
            P_LogicWhileStartStruct* LogicWhileStart =(P_LogicWhileStartStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
            ui->AutoEditchboxIfCondition1->setEnabled(true);
            ui->AutoEditchboxIfCondition2->setEnabled(false);
            if(ui->AutoEditchboxIfCondition1->isChecked()==false)
            {
                ui->AutoEditchboxIfCondition1->setChecked(true);
            }
            ui->Autoedloopnum->setDecimalPlaces(0);
            ui->Autoedloopnum->setText(QString::number(LogicWhileStart->cycNum,'f',0));
            ui->stkWgtEdit->setCurrentWidget(ui->AutoEditIfpage);
            ui->AutoEditIfSunPage->setCurrentWidget(ui->AutoEditLoopnumpage);
            break;
        }
        case C_LOGIC_VAR:
        {
            ui->stkWgtEdit->setCurrentWidget(ui->AutoEditIfpage);
            ui->AutoEditchboxIfCondition1->setEnabled(true);
            ui->AutoEditchboxIfCondition2->setEnabled(false);
            if(ui->AutoEditchboxIfCondition1->isChecked()==false)
            {
                ui->AutoEditchboxIfCondition1->setChecked(true);
            }
            ui->AutoEditOperandSecond->hide();//隐藏单位
            ui->AutoEditSavebtn->show();//保存按钮
            P_LogicVarStruct* LogicVar =(P_LogicVarStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
            if(LogicVar->sufferOperType == 0)
            {
                ui->AutoEditIfSunPage->setCurrentWidget(ui->AutoEditIfOperandpage);
                if(m_VariableType[LogicVar->varNum] == 0)
                {//整数
                    ui->AutoeditIfOperand->setDecimalPlaces(0);//根据变量小数位数进行设置
                    ui->AutoeditIfOperand->setText(QString::number(LogicVar->sufferOperValue,'f',0));
                }
                else if(m_VariableType[LogicVar->varNum] == 1)
                {//一位小数
                    ui->AutoeditIfOperand->setDecimalPlaces(1);//根据变量小数位数进行设置
                    ui->AutoeditIfOperand->setText(QString::number(((double)LogicVar->sufferOperValue)/10,'f',1));
                }
                else if(m_VariableType[LogicVar->varNum] == 2)
                {//两位小数
                    ui->AutoeditIfOperand->setDecimalPlaces(2);//根据变量小数位数进行设置
                    ui->AutoeditIfOperand->setText(QString::number(((double)LogicVar->sufferOperValue)/100,'f',2));
                }
            }
            else
            {
                ui->AutoEditIfSunPage->setCurrentWidget(ui->AutoEditIfNoModifypage);
                ui->AutoEditSavebtn->hide();//隐藏保存按钮
            }
            break;
        }
        case C_LOGIC_AXIS:
        {
            ui->stkWgtEdit->setCurrentWidget(ui->AutoEditIfpage);
            ui->AutoEditchboxIfCondition1->setEnabled(true);
            ui->AutoEditchboxIfCondition2->setEnabled(false);
            if(ui->AutoEditchboxIfCondition1->isChecked()==false)
            {
                ui->AutoEditchboxIfCondition1->setChecked(true);
            }
            ui->AutoEditOperandSecond->hide();//隐藏单位
            ui->AutoEditSavebtn->show();//保存按钮
            P_LogicAxisStruct* LogicAxis =(P_LogicAxisStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
            if(LogicAxis->sufferOperType == 0)
            {
                ui->AutoEditIfSunPage->setCurrentWidget(ui->AutoEditIfOperandpage);
                //两位小数
                ui->AutoeditIfOperand->setDecimalPlaces(2);//根据变量小数位数进行设置
                ui->AutoeditIfOperand->setText(QString::number(((double)LogicAxis->sufferOperValue)/100,'f',2));
            }
            else
            {
                ui->AutoEditIfSunPage->setCurrentWidget(ui->AutoEditIfNoModifypage);
                ui->AutoEditSavebtn->hide();//隐藏保存按钮
            }
            break;
        }
        case C_LOGIC_STACK:
        {
            ui->stkWgtEdit->setCurrentWidget(ui->AutoEditIfpage);
            ui->AutoEditchboxIfCondition1->setEnabled(true);
            ui->AutoEditchboxIfCondition2->setEnabled(false);
            if(ui->AutoEditchboxIfCondition1->isChecked()==false)
            {
                ui->AutoEditchboxIfCondition1->setChecked(true);
            }
            ui->AutoEditOperandSecond->hide();//隐藏单位
            ui->AutoEditSavebtn->show();//保存按钮
            P_LogicStackStruct* LogicStack =(P_LogicStackStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
            if(LogicStack->sufferOperType == 0)
            {
                ui->AutoEditIfSunPage->setCurrentWidget(ui->AutoEditIfOperandpage);
                ui->AutoeditIfOperand->setDecimalPlaces(0);
                ui->AutoeditIfOperand->setText(QString::number(LogicStack->sufferOperValue,'f',0));
            }
            else
            {
                ui->AutoEditIfSunPage->setCurrentWidget(ui->AutoEditIfNoModifypage);
                ui->AutoEditSavebtn->hide();//隐藏保存按钮
            }
            break;
        }
        case C_LOGIC_PRODUCT:
        {
            ui->stkWgtEdit->setCurrentWidget(ui->AutoEditIfpage);
            ui->AutoEditchboxIfCondition1->setEnabled(true);
            ui->AutoEditchboxIfCondition2->setEnabled(false);
            if(ui->AutoEditchboxIfCondition1->isChecked()==false)
            {
                ui->AutoEditchboxIfCondition1->setChecked(true);
            }
            ui->AutoEditOperandSecond->hide();//隐藏单位
            ui->AutoEditSavebtn->show();//保存按钮
            P_LogicCurProductNumStruct* LogicCurProductNum =(P_LogicCurProductNumStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
            if(LogicCurProductNum->sufferOperType == 0)
            {
                ui->AutoEditIfSunPage->setCurrentWidget(ui->AutoEditIfOperandpage);
                ui->AutoeditIfOperand->setDecimalPlaces(0);
                ui->AutoeditIfOperand->setText(QString::number(LogicCurProductNum->sufferOperValue,'f',0));
            }
            else
            {
                ui->AutoEditIfSunPage->setCurrentWidget(ui->AutoEditIfNoModifypage);
                ui->AutoEditSavebtn->hide();//隐藏保存按钮
            }
            break;
        }
        case C_TORQUE_GARD:
        {
            ui->AutoEditchboxIfCondition1->setEnabled(true);
            ui->AutoEditchboxIfCondition2->setEnabled(false);
            if(ui->AutoEditchboxIfCondition1->isChecked()==false)
            {
                ui->AutoEditchboxIfCondition1->setChecked(true);
            }
            ui->AutoEditTorque->setDecimalPlaces(0);
            ui->stkWgtEdit->setCurrentWidget(ui->AutoEditTorquepage);
            P_TorqueGardStruct* TorqueGard =(P_TorqueGardStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
            ui->AutoEditTorque->setText(QString::number(TorqueGard->torqueValue));
            ui->AutoEditSavebtn->show();//保存按钮
            break;
        }
        case C_SUN_PRO:
        {
            P_SunProStruct* SunPro =(P_SunProStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
            if(SunPro->oprMode == 3)
            {
                ui->stkWgtEdit->setCurrentWidget(ui->AutoEditTimeLimitpage);
                ui->TimeLimitdelay->setDecimalPlaces(2);
                double OtherDelay = m_OperateProOrder[m_CurrentSelectProOrderList].delay;
                ui->TimeLimitdelay->setText(QString::number(OtherDelay/100,'f',2));
                ui->AutoEditSavebtn->show();//保存按钮
            }
            else
            {
                ui->stkWgtEdit->setCurrentWidget(ui->AutoEditIfNoModifypage);
                ui->AutoEditSavebtn->hide();//隐藏保存按钮
            }
            break;
        }
        default:
            break;
        }
    }
}
//自动界面IF指令编辑处理
void AutoForm::AutoFromIfOrderEditHandle(uint8_t IfIndex)
{
    P_LogicIfStruct* LogicIf =(P_LogicIfStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
    if(LogicIf->reqSelectFlag[0] == 0)
    {
        ui->AutoEditchboxIfCondition1->setEnabled(true);
        ui->AutoEditchboxIfCondition2->setEnabled(false);
    }
    else
    {
        ui->AutoEditchboxIfCondition1->setEnabled(true);
        ui->AutoEditchboxIfCondition2->setEnabled(true);
    }
    if(LogicIf->cmpType[IfIndex]==0 || LogicIf->cmpType[IfIndex]==103)
    {//如果比较类型为输入和输出
        ui->AutoEditIfSunPage->setCurrentWidget(ui->AutoEditIfNoModifypage);
        ui->AutoEditSavebtn->hide();//隐藏保存按钮
    }
    else if(LogicIf->cmpType[IfIndex]>=1 && LogicIf->cmpType[IfIndex]<=20)
    {//如果比较类型为变量类型
        if(LogicIf->sufferCmpType[IfIndex]==0)
        {//被比较类型为常量
            ui->AutoEditSavebtn->show();//显示保存按钮
            ui->AutoEditIfSunPage->setCurrentWidget(ui->AutoEditIfOperandpage);
            if(m_VariableType[LogicIf->sufferCmpType[IfIndex]] == 0)
            {//整数
                ui->AutoeditIfOperand->setDecimalPlaces(0);//根据变量小数位数进行设置
                ui->AutoeditIfOperand->setText(QString::number(LogicIf->sufferCmpValue[IfIndex]));
            }
            else if(m_VariableType[LogicIf->sufferCmpType[IfIndex]] == 1)
            {//一位小数
                ui->AutoeditIfOperand->setDecimalPlaces(1);//根据变量小数位数进行设置
                ui->AutoeditIfOperand->setText(QString::number(((double)LogicIf->sufferCmpValue[IfIndex])/10,'f',1));
            }
            else if(m_VariableType[LogicIf->sufferCmpType[IfIndex]] == 2)
            {//两位小数
                ui->AutoeditIfOperand->setDecimalPlaces(2);//根据变量小数位数进行设置
                ui->AutoeditIfOperand->setText(QString::number(((double)LogicIf->sufferCmpValue[IfIndex])/100,'f',2));
            }
            ui->AutoEditOperandSecond->hide();//隐藏单位
        }
        else if((LogicIf->sufferCmpType[IfIndex]>=1 && LogicIf->sufferCmpType[IfIndex]<=20) || LogicIf->sufferCmpType[IfIndex]==102)
        {//被比较类型是变量或被比较类型是实际产量
            ui->AutoEditIfSunPage->setCurrentWidget(ui->AutoEditIfNoModifypage);
            ui->AutoEditSavebtn->hide();//隐藏保存按钮
        }
    }
    else if(LogicIf->cmpType[IfIndex]>=21 && LogicIf->cmpType[IfIndex]<=60)
    {//堆叠组
        if(LogicIf->sufferCmpType[IfIndex]==0)
        {//常量
            ui->AutoEditOperandSecond->hide();//隐藏单位
            ui->AutoEditIfSunPage->setCurrentWidget(ui->AutoEditIfOperandpage);
            ui->AutoeditIfOperand->setDecimalPlaces(0);//根据变量小数位数进行设置
            ui->AutoeditIfOperand->setText(QString::number(LogicIf->sufferCmpValue[IfIndex]));
            ui->AutoEditSavebtn->show();//显示保存按钮
        }
        else
        {
            ui->AutoEditIfSunPage->setCurrentWidget(ui->AutoEditIfNoModifypage);
            ui->AutoEditSavebtn->hide();//隐藏保存按钮
        }
    }
    else if(LogicIf->cmpType[IfIndex]>=61 && LogicIf->cmpType[IfIndex]<=80)
    {//轴
        if(LogicIf->sufferCmpType[IfIndex]==0)
        {//常量
            ui->AutoEditOperandSecond->hide();//隐藏单位
            ui->coboxSearchAxisSelect->setEnabled(false);
            ui->coboxSearchSymbolSelect->setCurrentText(QString::number(LogicIf->cmpMode[IfIndex]));
            ui->AutoEditIfSunPage->setCurrentWidget(ui->AutoEditIfAxisPospage);
            ui->AutoeditIfOperand->setDecimalPlaces(2);//根据变量小数位数进行设置
            ui->editSearchPosition->setText(QString::number(((double)LogicIf->sufferCmpValue[IfIndex])/100,'f',2));
            ui->AutoEditSavebtn->show();//显示保存按钮
        }
        else
        {
            ui->AutoEditIfSunPage->setCurrentWidget(ui->AutoEditIfNoModifypage);
            ui->AutoEditSavebtn->hide();//隐藏保存按钮
        }
    }
    else if(LogicIf->cmpType[IfIndex]>=81 && LogicIf->cmpType[IfIndex]<=100)
    {//定时器
         if(LogicIf->sufferCmpType[IfIndex]==0)
         {
             ui->AutoEditOperandSecond->show();//显示单位
             ui->AutoEditIfSunPage->setCurrentWidget(ui->AutoEditIfOperandpage);
             ui->AutoeditIfOperand->setDecimalPlaces(2);//根据变量小数位数进行设置
             ui->AutoeditIfOperand->setText(QString::number(((double)LogicIf->sufferCmpValue[IfIndex]/100),'f',2));
             ui->AutoEditSavebtn->show();//显示保存按钮
         }
         else if(LogicIf->sufferCmpType[IfIndex]==81 && LogicIf->sufferCmpType[IfIndex]==101)
         {
             ui->AutoEditIfSunPage->setCurrentWidget(ui->AutoEditIfNoModifypage);
             ui->AutoEditSavebtn->hide();//隐藏保存按钮
         }
    }
    else if(LogicIf->cmpType[IfIndex] == 102)
    {//实际产量
        if(LogicIf->sufferCmpType[IfIndex]==0)
        {//被比较类型为常量
            ui->AutoEditSavebtn->show();//显示保存按钮
            ui->AutoEditIfSunPage->setCurrentWidget(ui->AutoEditIfOperandpage);
            ui->AutoeditIfOperand->setDecimalPlaces(0);//根据变量小数位数进行设置
            ui->AutoeditIfOperand->setText(QString::number(LogicIf->sufferCmpValue[IfIndex]));
            ui->AutoEditOperandSecond->hide();//隐藏单位
        }
        else if((LogicIf->sufferCmpType[IfIndex]>=1 && LogicIf->sufferCmpType[IfIndex]<=20))
        {//被比较类型是变量
            ui->AutoEditIfSunPage->setCurrentWidget(ui->AutoEditIfNoModifypage);
            ui->AutoEditSavebtn->hide();//隐藏保存按钮
        }
    }
    else if(LogicIf->cmpType[IfIndex] == 103)
    {//位置
        if(LogicIf->sufferCmpType[IfIndex]==0)
        {//被比较类型为位置（常量）
            ui->AutoEditSavebtn->show();//显示保存按钮
            ui->AutoEditIfSunPage->setCurrentWidget(ui->AutoEditIfAxisPospage);
            ui->AutoeditIfOperand->setDecimalPlaces(2);//根据变量小数位数进行设置
            ui->AutoeditIfOperand->setText(QString::number(((double)LogicIf->sufferCmpValue[IfIndex])/100,'f',2));
            ui->AutoEditOperandSecond->hide();//隐藏单位
        }
        else if((LogicIf->sufferCmpType[IfIndex]>=1 && LogicIf->sufferCmpType[IfIndex]<=20) || LogicIf->sufferCmpType[IfIndex]==103)
        {//被比较类型是变量或被比较类型是轴
            ui->AutoEditIfSunPage->setCurrentWidget(ui->AutoEditIfNoModifypage);
            ui->AutoEditSavebtn->hide();//隐藏保存按钮
        }
    }
}
//自动界面IF指令编辑保存处理
void AutoForm::AutoFromIfOrderEditSaveHandle(uint8_t IfIndex)
{
    P_LogicIfStruct* LogicIf =(P_LogicIfStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;

    if(ui->stkWgtEdit->currentWidget() == ui->AutoEditIfpage && (ui->AutoEditIfSunPage->currentWidget() == ui->AutoEditIfOperandpage || ui->AutoEditIfSunPage->currentWidget() == ui->AutoEditIfAxisPospage))
    {
        if(LogicIf->cmpType[IfIndex]>=1 && LogicIf->cmpType[IfIndex]<=20)
        {
            if(LogicIf->sufferCmpType[IfIndex]==0)
            {
                if(m_VariableType[LogicIf->sufferCmpType[IfIndex]] == 0)
                {//整数
                    LogicIf->sufferCmpValue[IfIndex] =  ui->AutoeditIfOperand->text().toUInt();
                }
                else if(m_VariableType[LogicIf->sufferCmpType[IfIndex]] == 1)
                {//一位小数
                    LogicIf->sufferCmpValue[IfIndex] =  ui->AutoeditIfOperand->text().toDouble()*10;
                }
                else if(m_VariableType[LogicIf->sufferCmpType[IfIndex]] == 2)
                {//两位小数
                    LogicIf->sufferCmpValue[IfIndex] =  ui->AutoeditIfOperand->text().toDouble()*100;
                }
            }
        }
        else if(LogicIf->cmpType[IfIndex]>=61 && LogicIf->cmpType[IfIndex]<=80)
        {//轴
            if(LogicIf->sufferCmpType[IfIndex]==0)
            {//常量
                LogicIf->cmpMode[IfIndex] = ui->coboxSearchSymbolSelect->currentIndex();
                LogicIf->sufferCmpValue[IfIndex] = ui->editSearchPosition->text().toDouble()*100;
            }
        }
        else if(LogicIf->cmpType[IfIndex]>=81 && LogicIf->cmpType[IfIndex]<=100)
        {//定时器
            if(LogicIf->sufferCmpType[IfIndex]==0)
            {//常量
                LogicIf->sufferCmpValue[IfIndex] = ui->AutoeditIfOperand->text().toUInt();
            }
        }
        else if(LogicIf->cmpType[IfIndex] == 102)
        {//实际产量
            if(LogicIf->sufferCmpType[IfIndex]==0)
            {//常量
                LogicIf->sufferCmpValue[IfIndex] = ui->AutoeditIfOperand->text().toUInt();
            }
        }
        else if(LogicIf->cmpType[IfIndex] == 103)
        {//位置
            if(LogicIf->sufferCmpType[IfIndex]==0)
            {//常量
                LogicIf->sufferCmpValue[IfIndex] = ui->AutoeditIfOperand->text().toUInt();
            }
        }
    }

}
/*************************************************************************
**  函数名：  AutoForm_Refresh()
**	输入参数：
**	输出参数：
**	函数功能：自动运行界面相关数据刷新函数
**  作者：    wukui
**  开发日期：2024/12/20
**************************************************************************/
void AutoForm::AutoForm_Refresh()
{
    Program_Follow_Refresh();//跟随刷新
    Auto_State_Refresh();
    Auto_CurStep_Refresh();
    Var_State_Refresh();//变量状态刷新
    Stack_State_Refresh();//堆叠状态刷新
    UpdateFloatPoPShow();//堆叠/速度悬浮弹窗刷新显示
}
/*************************************************************************
**  函数名：  Auto_State_Refresh()
**	输入参数：
**	输出参数：
**	函数功能：自动运行界面自动状态参数刷新
**  作者：    wukui
**  开发日期：2024/12/20
**************************************************************************/
void AutoForm::Auto_State_Refresh()
{
    ui->labAutoOperateTime->setText(QString::number(((double)m_RunInfo.runTime)/1000/60/60,'f',1)+"h"); //自动运行时间
    ui->labPrebeatCycle->setText(QString::number(((double)m_RunInfo.preShootCyc),'f',1)+"s");//前拍周期
    ui->labFormingCycle->setText(QString::number(((double)m_RunInfo.takeShapeCyc),'f',1)+"s");//成型周期
    ui->labFetchTime->setText(QString::number(((double)m_RunInfo.fetchTime),'f',2)+"s"); //取物时间
    ui->labActualProd->setText(QString::number(m_RunInfo.actualProductNum));             //实际产品
    ui->labCurrentStepNumber->setText(QString::number(m_ProRunInfo.proNum[m_OperateProNum]+1));//当前步号
}
/*************************************************************************
**  函数名：  Var_State_Refresh()
**	输入参数：
**	输出参数：
**	函数功能：自动运行界面变量状态参数刷新
**  作者：    wukui
**  开发日期：2025/3/21
**************************************************************************/
void AutoForm::Var_State_Refresh()
{
    if(ui->stkWgtState->currentWidget() == ui->page2VariableState)
    {
        if(m_RobotRunSta == MAC_STA_RUN)
        {
            if(ui->tableWgtVarState->isEnabled() == true)
            {
                ui->tableWgtVarState->setEnabled(false);
            }
        }
        else
        {
            if(ui->tableWgtVarState->isEnabled() == false)
            {
                ui->tableWgtVarState->setEnabled(true);
            }
        }
        for(int i=0;i<VAR_TOTAL_NUM;i++)
        {
            VarStateEdit.at(i)->setDecimalPlaces(m_VariableType[i]);
            if(m_VariableType[i] == 0)
            {
                VarStateEdit.at(i)->setText(QString::number(m_VariableCurValue[i],'f',0));
            }
            else if(m_VariableType[i] == 1)
            {
                VarStateEdit.at(i)->setText(QString::number((double)(m_VariableCurValue[i]/10),'f',1));
            }
            else if(m_VariableType[i] == 2)
            {
                VarStateEdit.at(i)->setText(QString::number((double)(m_VariableCurValue[i]/100),'f',2));
            }
            VarStateEdit.at(i)->setText(VarStateEdit.at(i)->text());
        }
        for(int i=0;i<STACK_TOTAL_NUM;i++)
        {
            AtackStateEdit.at(i)->setText(QString::number(m_StackCurPileCnt[i],'f',0));
        }
        g_Usart->ExtendSendReadPar(CMD_MAIN_STA,CMD_SUN_STA_STACK,1,0);//发送读取堆叠实时参数指令
        g_Usart->ExtendSendReadPar(CMD_MAIN_STA,CMD_SUN_STA_VAR,0,0);//发送读取变量值
    }
}

/*************************************************************************
**  函数名：  Stack_State_Refresh()
**	输入参数：
**	输出参数：
**	函数功能：自动运行界面堆叠状态参数刷新
**  作者：    wukui
**  开发日期：2025/3/22
**************************************************************************/
void AutoForm::Stack_State_Refresh()
{
    if(ui->stkWgtState->currentWidget() == ui->page3StackState)
    {
        if(StackOrderNumList != processInstructions(1))
        {
            StackOrderNumList = processInstructions(1);
            AutoStateMode->removeRows(0,AutoStateMode->rowCount());//删除所有行并释放内存
            for(int i=0;i<StackOrderNumList.count();i++)
            {
                AutoStateMode->insertRow(AutoStateMode->rowCount());
                // 获取当前行索引
                int row = AutoStateMode->rowCount() - 1;

                // 设置第一列数据
                QStandardItem *item = new QStandardItem(QString(tr("堆叠%1组:")).arg(StackOrderNumList.at(i)));
                AutoStateMode->setItem(row, 0, item);
                for(int j=0;j<STACK_AXIS_NUM;j++)
                {
                    QStandardItem *item = new QStandardItem(QString("%1").arg(m_StackAxisCnt[i][j]));
                    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);//水平+垂直居中
                    AutoStateMode->setItem(row, j+1, item);
                }
            }
        }
        if(StackOrderNumList.count()>0)
        {
            for (int i=0;i<StackOrderNumList.count();i++) {
                for(int j=0;j<STACK_AXIS_NUM;j++)
                {
                    QModelIndex index = AutoStateMode->index(i, j+1);
                    AutoStateMode->setData(index,m_StackAxisCnt[i][j],Qt::DisplayRole);
                }
            }
            g_Usart->ExtendSendReadPar(CMD_MAIN_STA,CMD_SUN_STA_ALL_STACK_NUM,1,0);//发送读取所有堆叠位置点数指令
        }
    }
}

//用来获取所有堆叠指令或跟随堆叠指令的堆叠组号 type=0：不需要获取跟随堆叠点组号，type=1:都需要获取
QStringList AutoForm::processInstructions(uint8_t type)
{
    QSet<int> uniqueStackNum;//用与去重的临时集合
    QStringList resultList;//结果列表
    for(int i=0;i<m_OperateProOrderListNum;i++)
    {
        if(m_OperateProOrder[i].cmd == C_STACK_MOVE)
        {//堆叠指令
            P_StackMoveStruct* StackMove = (P_StackMoveStruct*)m_OperateProOrder[i].pData;
            if(!uniqueStackNum.contains(StackMove->stackNum))
            {
                uniqueStackNum.insert(StackMove->stackNum);
                resultList.append(QString::number(StackMove->stackNum));
            }
        }
        if(type == 1)
        {
            if(m_OperateProOrder[i].cmd == C_STACK_FOLLOW)
            {//跟随堆叠指令
                P_StackFollowStruct* StackFollow = (P_StackFollowStruct*)m_OperateProOrder[i].pData;
                if(!uniqueStackNum.contains(StackFollow->stackNum))
                {
                    uniqueStackNum.insert(StackFollow->stackNum);
                    resultList.append(QString::number(StackFollow->stackNum));
                }
            }
        }

    }
    //排序
    std::sort(resultList.begin(), resultList.end(),
            [](const QString& a, const QString& b) {
                return a.toInt() < b.toInt();
            });
    return resultList;
}
/*************************************************************************
**  函数名：  Auto_CurStep_Refresh()
**	输入参数：
**	输出参数：
**	函数功能：自动运行界面当前步号刷新
**  作者：    wukui
**  开发日期：2025/1/2
**************************************************************************/
void AutoForm::Auto_CurStep_Refresh()
{
    if(ui->stkWgtState->currentWidget() == ui->page4CurrentStep)
    {
        ui->labMainProgStepNum->setText(QString::number(m_ProRunInfo.proNum[0]));
        ui->labSubProg1StepNum->setText(QString::number(m_ProRunInfo.proNum[1]));
        ui->labSubProg2StepNum->setText(QString::number(m_ProRunInfo.proNum[2]));
        ui->labSubProg3StepNum->setText(QString::number(m_ProRunInfo.proNum[3]));
        ui->labSubProg5StepNum->setText(QString::number(m_ProRunInfo.proNum[4]));
        ui->labSubProg6StepNum->setText(QString::number(m_ProRunInfo.proNum[5]));
        ui->labSubProg7StepNum->setText(QString::number(m_ProRunInfo.proNum[6]));
        ui->labSubProg8StepNum->setText(QString::number(m_ProRunInfo.proNum[7]));
    }
    if(m_ProRunInfo.proNum[0]+1 >= m_RunPar.startRunLineNum && m_RunPar.startRunLineNum != 1)
    {
        m_RunPar.startRunLineNum = 1;
        SetAutoRunIcon();
    }
}
/*************************************************************************
**  函数名：  Program_Follow_Refresh()
**	输入参数：
**	输出参数：
**	函数功能：自动运行界面跟随刷新处理函数  背景颜色rgb(33, 97, 137)
**  作者：    wukui
**  开发日期：2024/12/20
**************************************************************************/
void AutoForm::Program_Follow_Refresh()
{ 
    if(ui->btnFollow->isChecked() == true)
    {//如果开启了跟随功能
        if(m_AutoInforRefresh.Program_Follow_Flog == 0)
        {
            ui->Auto_file_List->clearSelection();//清空行选中状态
            ui->Auto_file_List->setSelectionMode(QAbstractItemView::NoSelection);//禁用选择功能
            m_AutoInforRefresh.Program_Follow_Flog = 1;
        }

        if (m_ProRunInfo.proNum[m_OperateProNum] <= ui->Auto_file_List->rowCount() )//&& m_AutoInforRefresh.Old_Follow_ProNum != m_OperateProOrder[m_ProRunInfo.proNum[m_OperateProNum]].runOrderNum)
        {
            qDebug()<<"当前运行序号："<<m_OperateProOrder[m_ProRunInfo.proNum[m_OperateProNum]].runOrderNum;
            for(int row = 0;row<ui->Auto_file_List->rowCount();row++)
            {
                if (ui->Auto_file_List->item(row, 0))
                {
                    if(ui->Auto_file_List->item(row, 0)->text().toInt() == m_OperateProOrder[m_ProRunInfo.proNum[m_OperateProNum]].runOrderNum)
                    {
                         ui->Auto_file_List->scrollToItem(ui->Auto_file_List->item(row, 0));//若列表项在可视区域外，视图会自动滚动以确保该项可见。
                         for (int column = 0; column < ui->Auto_file_List->columnCount(); column++)
                         {
                             if (ui->Auto_file_List->item(row, column)) {
                                 ui->Auto_file_List->item(row, column)->setBackground(QBrush(QColor(33, 122, 178)));
                             }
                         }
                    }
                    else
                    {
                        for (int column = 0; column < ui->Auto_file_List->columnCount(); column++)
                        {
                            if (ui->Auto_file_List->item(row, column)) {
                                ui->Auto_file_List->item(row, column)->setBackground(QBrush(Qt::NoBrush));
                            }
                        }
                    }
                }
            }
            //m_AutoInforRefresh.Old_Follow_ProNum = m_OperateProOrder[m_ProRunInfo.proNum[m_OperateProNum]].runOrderNum;//上一次运行行号
        }
    }
    else
    {
        if(m_AutoInforRefresh.Program_Follow_Flog == 1)
        {
            ui->Auto_file_List->setSelectionMode(QAbstractItemView::SingleSelection);//单行选择
            if(ui->Auto_file_List->rowCount()==m_OperateProOrderListNum)
            {
               ui->Auto_file_List->selectRow(m_CurrentSelectProOrderList);
               ui->Auto_file_List->scrollToItem(ui->Auto_file_List->item(m_CurrentSelectProOrderList, 0));
            }
            else
            {
               qDebug() << "当前打开程序异常";
            }
            m_AutoInforRefresh.Program_Follow_Flog = 0;
            m_AutoInforRefresh.Old_Follow_ProNum = 0;
            for(int row = 0;row<ui->Auto_file_List->rowCount();row++)
            {//清空行背景色
                if (ui->Auto_file_List->item(row, 0))
                {
                    for (int column = 0; column < ui->Auto_file_List->columnCount(); column++)
                    {
                        if (ui->Auto_file_List->item(row, column)) {
                            ui->Auto_file_List->item(row, column)->setBackground(QBrush(Qt::NoBrush));
                        }
                    }
                }
            }
        }
    }
}
/*************************************************************************
**  函数名：  Auto_File_List_Refresh()
**	输入参数：
**	输出参数：
**	函数功能：自动-程序列表刷新
**  作者：    wukui
**  开发日期：2024/8/2
**************************************************************************/
void AutoForm::Auto_File_List_Refresh(uint8_t ProNum)
{
    uint16_t i=0;
    if(ProNum != m_OperateProNum)
    {
        emit Switch_ProNum_Signal(ProNum);//发送切换程序编号信号，刷新教导界面列表
        g_Usart->ExtendSendProDeal(CMD_MAIN_PRO,CMD_SUN_PRO_INFO);
        g_Usart->ExtendSendProDeal(CMD_MAIN_PRO,CMD_SUN_PRO_SAVE,m_OperateProNum,2);
    }
    //先清空表格并重新设置行数和列数
    uint16_t Old_CurrentSelectList = m_CurrentSelectProOrderList;
    ui->Auto_file_List->clearContents();
    ui->Auto_file_List->setRowCount(0);
    ui->Auto_file_List->setColumnCount(2);
    for(i=0;i<m_OperateProOrderListNum;i++)
    {
        QTableWidgetItem *Auto_File_List_NumItem = new QTableWidgetItem();
        QTableWidgetItem *Auto_File_List_OrderItem = new QTableWidgetItem();
        Auto_File_List_NumItem->setText(JointRunOrderNum(m_OperateProOrder[i]));
        Auto_File_List_NumItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);//设置执行行号内容居中显示
        Auto_File_List_OrderItem->setText(JointStrDeal(m_OperateProOrder[i]));//拼接显示内容
        Auto_File_List_OrderItem->setData(Qt::TextAlignmentRole, Qt::AlignLeft);//内容左右对齐靠左显示
        Auto_File_List_OrderItem->setData(Qt::TextAlignmentRole, Qt::AlignVCenter);//内容上下对齐居中对齐显示
        Auto_File_List_OrderItem->setData(Qt::TextWordWrap,1);//设置内容自动换行显示
        ui->Auto_file_List->insertRow(i);
        ui->Auto_file_List->setItem(i,0,Auto_File_List_NumItem);   //显示命令执行序号
        ui->Auto_file_List->setItem(i,1,Auto_File_List_OrderItem);                 //显示命令内容
        if(m_OperateProOrder[i].noteFlag == 1)
        {//屏蔽指令时行背景显示灰色
            ui->Auto_file_List->item(i,0)->setBackground(QColor(192, 191, 188));
            ui->Auto_file_List->item(i,1)->setBackground(QColor(192, 191, 188));
        }

        m_CurrentSelectProOrderList = Old_CurrentSelectList;
        ui->Auto_file_List->selectRow(m_CurrentSelectProOrderList);
    }
    //界面初始化

    ui->stkWgtOuter->setCurrentWidget(ui->pageState);//界面切到自动状态界面
    ui->stkWgtState->setCurrentIndex(0);
    updateLabelState(0);
    ui->btnEdit->setChecked(false);//编辑按钮默认关闭
    if(ProNum == 0)
    {//如果在主程序，刷新从此运行行号标志
        g_Usart->ExtendSendParDeal(CMD_MAIN_STA,CMD_SUN_STA_PAR,0,0);
        SetAutoRunIcon();
    }
}

void AutoForm::updateLabelState(int index)
{
    const QList<QString> states = {
        tr("1自动状态"),
        tr("2变量状态"),
        tr("3堆叠状态"),
        tr("4当前步号")
    };
    const QList<QString> btnTexts = {
        tr("清零"),
        tr("变量"),
        tr("堆叠"),
        tr("步号"),
    };

    if (index >= 0 && index < states.size())
    {
        ui->labelState->setText(states.at(index));
        ui->btnClear->setText(btnTexts.at(index));
        if(index >1)
        {
            if(ui->btnClear->isEnabled() == true)
                ui->btnClear->setEnabled(false);
        }
        else
        {
            if(ui->btnClear->isEnabled() == false)
                ui->btnClear->setEnabled(true);
        }
    }
}

void AutoForm::callNumKeyBoard(QObject* obj)
{
    if (obj)
    {
        NumKeyboard temp(this);
        NumKeyboard* keyboard = &temp;

        keyboard->clearText();
        keyboard->setCurrentEditObj(obj);
        keyboard->exec();
    }
}

void AutoForm::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange)
    {
        retranslate();
    }
    QWidget::changeEvent(e);
}

void AutoForm::retranslate()
{
    ui->retranslateUi(this);

    for (int i = 0; i < operateActions.size(); i++)
    {
        const char* sourceText = actionStrs.at(i).c_str();
        operateActions.at(i)->setText(QCoreApplication::translate("AutoForm", sourceText, nullptr));
    }
    ui->tableWgtVarState->setHorizontalHeaderLabels({tr("变量"), tr("堆叠状态")});

}


void AutoForm::on_btnAimedProduct_clicked()
{
    // call keyboard for labAimedProd
    callNumKeyBoard(qobject_cast<QLabel*>(ui->labAimedProd));
    m_RunPar.planProductNum = ui->labAimedProd->text().toUInt();//计划产品数
    emit Send_planProductNum_Signal();
}

void AutoForm::on_btnEdit_toggled(bool checked)
{
    if (checked)
    {
        if(m_OperateProOrderListNum == 0)
        {//如果未载入程序，直接返回
            return;
        }
        ui->stkWgtOuter->setCurrentWidget(ui->pageEdit);
        OrderEditHandel();
    }
    else
    {
        ui->stkWgtOuter->setCurrentWidget(ui->pageState);
        ui->stkWgtState->setCurrentWidget(ui->page1AutoState);
    }

}

void AutoForm::showReferPointDialog()
{
#if RERFERENCE_DIALOG_TEST
    ManualForm* keyboardParent = this->parent()->findChild<ManualForm*>();
    if (!keyboardParent)
        qDebug() << "参考点编辑 ";
    QWidget* referPointPage = nullptr;
    if (!tabReferPoint) {
        tabReferPoint = parent()->findChild<QTabWidget*>("tabWidgetManualPage");
    }

    if (!tabReferPoint) {
        qWarning() << "TabWidget not found!";
        return;
    }

    int originalIndex = 4;
    referPointPage = tabReferPoint->widget(originalIndex);

    connect(dialogReferPoint, &QDialog::finished, this, [=](int result) {
        if (referPointPage && tabReferPoint)
        {
            tabReferPoint->insertTab(originalIndex, referPointPage, tr("参考点"));
        }

    }, Qt::UniqueConnection);

    if (referPointPage)
    {
        tabReferPoint->removeTab(originalIndex);
        dialogReferPoint->layout()->addWidget(referPointPage);
        referPointPage->show();
    }

//    dialogReferPoint->setWindowModality(Qt::WindowModal);

    okButton->move(dialogReferPoint->width() - okButton->width() - 20, dialogReferPoint->height() - okButton->height() - 15);
    cancelButton->move(okButton->x() - cancelButton->width() - 10, okButton->y());
    okButton->raise();
    cancelButton->raise();

#endif

    referEditDialog->show();

}

//自动界面轴编辑位置输入
void AutoForm::on_AutoEditAxisPos_textChanged(const QString &arg1)
{
    if(m_OperateProOrder[m_CurrentSelectProOrderList].cmd == C_AXIS_MOVE)
    {
        P_AxisMoveStruct* AxisMove = (P_AxisMoveStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
        if(AxisMove->pos+ui->AutoEditAxisPos->text().toDouble()*100 > AxisMove->pos+500 || AxisMove->pos+ui->AutoEditAxisPos->text().toDouble()*100 < AxisMove->pos-500)
        {
            MainWindow::pMainWindow->showErrorTip(tr("输入值的范围不能超过-5.00～5.00,请重新输入。"),TipMode::ONLY_OK);
            ui->AutoEditAxisPos->setText(QString::number(0.00));
        }
    }
    else if(m_OperateProOrder[m_CurrentSelectProOrderList].cmd == C_SEARCH_AXIS_MOVE)
    {
        P_SearchAxisMoveStruct* SearchAxisMove = (P_SearchAxisMoveStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
        if(SearchAxisMove->maxPos+ui->AutoEditAxisPos->text().toDouble()*100 > SearchAxisMove->maxPos+500 || SearchAxisMove->maxPos+ui->AutoEditAxisPos->text().toDouble()*100 < SearchAxisMove->maxPos-500)
        {
            MainWindow::pMainWindow->showErrorTip(tr("输入值的范围不能超过-5.00～5.00,请重新输入。"),TipMode::ONLY_OK);
            ui->AutoEditAxisPos->setText(QString::number(0.00));
        }
    }
    else if(m_OperateProOrder[m_CurrentSelectProOrderList].cmd == C_OFFSET_AXIS)
    {
        P_OffsetAxisStruct* OffsetAxis = (P_OffsetAxisStruct*)m_OperateProOrder[m_CurrentSelectProOrderList].pData;
        if(OffsetAxis->offsetPos+ui->AutoEditAxisPos->text().toDouble()*100 > OffsetAxis->offsetPos+500 || OffsetAxis->offsetPos+ui->AutoEditAxisPos->text().toDouble()*100 < OffsetAxis->offsetPos-500)
        {
            MainWindow::pMainWindow->showErrorTip(tr("输入值的范围不能超过-5.00～5.00,请重新输入。"),TipMode::ONLY_OK);
            ui->AutoEditAxisPos->setText(QString::number(0.00));
        }
    }
}

void AutoForm::on_AutoEditchboxIfCondition1_clicked(bool checked)
{
    if(checked == true && m_OperateProOrder[m_CurrentSelectProOrderList].cmd == C_LOGIC_IF)
    {
        AutoFromIfOrderEditHandle(0);//条件0复选框选中
    }
}

void AutoForm::on_AutoEditchboxIfCondition2_clicked(bool checked)
{
    if(checked == true && m_OperateProOrder[m_CurrentSelectProOrderList].cmd == C_LOGIC_IF)
    {
        AutoFromIfOrderEditHandle(1);//条件1复选框选中
    }
}
