#include "autoform.h"
#include "ui_autoform.h"
#include <QMenu>
#include <QAction>
#include <QDebug>
#include <QMessageBox>
#include "Orderjoint.h"
#include "manualform.h"
#include "customedit.h"

AutoForm::AutoForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AutoForm)
{
    ui->setupUi(this);

    //列表初始化设置
    ui->Auto_file_List->setSelectionBehavior(QAbstractItemView::SelectRows); //设置选择行为时每次选择一行
    ui->Auto_file_List->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置不可编辑
    ui->Auto_file_List->setSelectionMode(QAbstractItemView::SingleSelection);//设置为单选
    ui->Auto_file_List->setShowGrid(true); //是否显示网格
    ui->Auto_file_List->setAlternatingRowColors(true);//开启隔行自动变色
    ui->Auto_file_List->setPalette(QPalette(QColor(237, 212, 0)));//隔行颜色
    ui->Auto_file_List->setColumnWidth(0,80);
    ui->Auto_file_List->setColumnWidth(1,10);
    // init some members;

    stackSet = new StackSetDialog(this);

    // 初始化关闭 framGlobalSpeed
    ui->frameGlobalSpeed->close();

    // 代码完善 framGlobalSpeed 弹窗小界面
    QTableWidget* table = new QTableWidget(ui->frameGlobalSpeed);
    table->setGeometry(5, 5, 110 * 5 + 50, 40 *2);
    table->setColumnCount(5);
    table->setRowCount(1);
    for (int i = 0; i < table->columnCount(); i++)
        table->setColumnWidth(i, 110);
    table->setRowHeight(0, 40);
    table->verticalHeader()->setVisible(false); // Hide vertical header
//    table->horizontalHeader()->setVisible(false); // Hide horizontal header
    table->setHorizontalHeaderLabels({tr("堆叠组号"), tr("X1轴个数"), tr("Y1轴个数"), tr("Z1轴个数"),tr("堆叠完停止")});
//    table->verticalScrollBar()->setDisabled(true); // 禁用滚动
    table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//隐藏滚动条
    table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    QTableWidgetItem* item = new QTableWidgetItem("x111");
//    table->setHorizontalHeaderItem(0, item);

  /*************************变量状态******************************************/
    ui->tableWgtVarState->setHorizontalHeaderLabels({tr("变量"), tr("堆叠状态")});
//    ui->tableWgtVarState->horizontalHeader()->setDefaultSectionSize(120);
    int i = 0;
    for (NumberEdit* edit : ui->page2VariableState->findChildren<NumberEdit*>())
    {
        edit->setText("0");
        edit->setDecimalPlaces(0);
        ui->tableWgtVarState->setCellWidget(i++, 0, edit);
    }



//    QString buttonName = QString("第%1个按钮").arg(buttonCount + 1);
//    QPushButton *newButton = new QPushButton(buttonName, this);
    QComboBox* coboxStkGroupNum = new QComboBox(this);
    NumberEdit* editX1Num = new NumberEdit(this);
    NumberEdit* editY1Num = new NumberEdit(this);
    NumberEdit* editZ1Num = new NumberEdit(this);
    QComboBox* coboxStkFinishStop = new QComboBox(this);

    coboxStkGroupNum->addItem("1");
    coboxStkFinishStop->addItems({tr("不使用"), tr("使用")});
    editX1Num->setDecimalPlaces(0);
    editX1Num->setInputRange(0, 2);
    editY1Num->setDecimalPlaces(0);
    editY1Num->setInputRange(0, 2);
    editZ1Num->setDecimalPlaces(0);
    editZ1Num->setInputRange(0, 2);

    table->setCellWidget(0, 0, coboxStkGroupNum);
    table->setCellWidget(0, 1, editX1Num);
    table->setCellWidget(0, 2, editY1Num);
    table->setCellWidget(0, 3, editZ1Num);
    table->setCellWidget(0, 4, coboxStkFinishStop);

    // 创建菜单并添加菜单项
    QMenu* menu = new QMenu(this);

    QAction* action1 = new QAction(tr("堆叠指定"), this);
    QAction* action2 = new QAction(tr("从此运行"), this);
    QAction* action3 = new QAction(tr("断点设置"), this);
    QAction* action4 = new QAction(tr("断点清除"), this);
    QAction* action5 = new QAction(tr("参考点编辑"), this);
    QAction* action6 = new QAction(tr("自动修正位置"), this);

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
/**********************************************************************/
    // 连接菜单项的点击事件
    connect(action1, &QAction::triggered, this, [=]() {
//        qDebug() << "堆叠指定 ";
        stackSet->show();

    });

    connect(action2, &QAction::triggered, this, [=]() {
//        qDebug() << "从此运行 ";
//        if (currentRow = 0)
//            ui->labStartStep0->hide();
//            ui->labStartStep->hide();
        ui->labStartStep0->show();
        ui->labStartStep->show();
    });

    connect(action3, &QAction::triggered, this, [=]() {
//        qDebug() << "断点设置 ";
        ui->labBreakPointPos0->show();
        ui->labBreakPointPos->show();
        int step = 0; // get this
        ui->labBreakPointPos->setText(QString("主程序第%1部分").arg(step));
        // put a breakpoint icon in program table

    });
    connect(action4, &QAction::triggered, this, [=]() {
//        qDebug() << "断点清除 ";
        ui->labBreakPointPos0->hide();
        ui->labBreakPointPos->hide();
    });

    connect(action5, &QAction::triggered, this, &AutoForm::showReferPointDialog);

    connect(action6, &QAction::triggered, this, [=]() {
//        qDebug() << "自动修正位置 ";
        AutoCorrectPosDialog dialog(this) ;
        if (dialog.exec() == QDialog::Accepted)
        {

        }
    });

    connect(ui->btnOperate, &QPushButton::clicked, [=]() {
        QPoint pos = ui->btnOperate->mapToGlobal(QPoint(0, 0));
        menu->exec(QPoint(pos.x(), pos.y() - menu->sizeHint().height()));
    });

/**********************************************************************/
    // these slots are used to adjust global speed
    connect(ui->btnGlobalSpeedAdd, &QPushButton::clicked, [=](){
        int value = ui->proBarGlobalSpeed->value();
        if (value <= 100 && value >= 0 && ui->btnAdjustSpeed->isChecked())
            ui->proBarGlobalSpeed->setValue(++value);
        m_RunPar.globalSpeed = ui->proBarGlobalSpeed->value();
    });
    connect(ui->btnGlobalSpeedSub, &QPushButton::clicked, [=](){
        int value = ui->proBarGlobalSpeed->value();
        if (value <= 100 && value >= 0 && ui->btnAdjustSpeed->isChecked())
            ui->proBarGlobalSpeed->setValue(--value);
        m_RunPar.globalSpeed = ui->proBarGlobalSpeed->value();
    });
    connect(ui->btn10percent, &QPushButton::clicked, [=](){
        if (ui->btnAdjustSpeed->isChecked())
            ui->proBarGlobalSpeed->setValue(10);
        m_RunPar.globalSpeed = ui->proBarGlobalSpeed->value();
    });
    connect(ui->btn20percent, &QPushButton::clicked, [=](){
        if (ui->btnAdjustSpeed->isChecked())
            ui->proBarGlobalSpeed->setValue(20);
        m_RunPar.globalSpeed = ui->proBarGlobalSpeed->value();
    });
//    connect(ui->btn20percent, &QPushButton::toggled, [=](bool checked){
//        if (ui->btnAdjustSpeed->isChecked())
//            ui->proBarGlobalSpeed->setValue(20);
//    });
    connect(ui->btn40percent, &QPushButton::clicked, [=](){
        if (ui->btnAdjustSpeed->isChecked())
            ui->proBarGlobalSpeed->setValue(40);
        m_RunPar.globalSpeed = ui->proBarGlobalSpeed->value();
    });
    connect(ui->btn60percent, &QPushButton::clicked, [=](){
        if (ui->btnAdjustSpeed->isChecked())
            ui->proBarGlobalSpeed->setValue(60);
        m_RunPar.globalSpeed = ui->proBarGlobalSpeed->value();
    });
    connect(ui->btn80percent, &QPushButton::clicked, [=](){
        if (ui->btnAdjustSpeed->isChecked())
            ui->proBarGlobalSpeed->setValue(80);
        m_RunPar.globalSpeed = ui->proBarGlobalSpeed->value();
    });
    connect(ui->btn100percent, &QPushButton::clicked, [=](){
        if (ui->btnAdjustSpeed->isChecked())
            ui->proBarGlobalSpeed->setValue(100);
        m_RunPar.globalSpeed = ui->proBarGlobalSpeed->value();
    });
/**********************************************************************/

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
        g_Usart->ExtendSendProDeal(CMD_MAIN_PRO,CMD_SUN_PRO_DEBUG,m_OperateProNum,m_CurrentSelectProOrderList+1);//运行行号从1开始，为当前选中行序号+1
    });
    connect(ui->coboxProgram, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index) {//自动运行界面程序编号切换
        emit Switch_ProNum_Signal(index);//发送切换程序编号信号，刷新教导界面列表
        g_Usart->ExtendSendProDeal(CMD_MAIN_PRO,CMD_SUN_PRO_INFO);
        g_Usart->ExtendSendProDeal(CMD_MAIN_PRO,CMD_SUN_PRO_SAVE,m_OperateProNum,2);
        Auto_File_List_Refresh(index);//刷新自动界面列表
    });
    connect(ui->btnFollow,&QPushButton::clicked,this,[=](){//自动界面跟随按钮
//        if(ui->btnFollow->isChecked() == true)
//        {
//            if (m_ProRunInfo.proNum[0] < ui->Auto_file_List->rowCount())
//            {
//                QTableWidgetItem *item = ui->Auto_file_List->item(m_ProRunInfo.proNum[0],0);
//                if(item)
//                {
//                    item->setBackground(Qt::blue);
//                }
//            }
//        }
    });
}

AutoForm::~AutoForm()
{
    delete ui;
}

/*************************************************************************
**  函数名：  Teach_File_List_Refresh()
**	输入参数：
**	输出参数：
**	函数功能：教导-刷新程序列表
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
    ui->Auto_file_List->setColumnCount(3);
    for(i=0;i<m_OperateProOrderListNum;i++)
    {
        QTableWidgetItem *Auto_File_List_NumItem = new QTableWidgetItem();
        QTableWidgetItem *Auto_File_List_OrderItem = new QTableWidgetItem();
        QTableWidgetItem *Auto_File_List_OrderColor = new QTableWidgetItem();
        Auto_File_List_NumItem->setText(JointRunOrderNum(m_OperateProOrder[i]));
        Auto_File_List_NumItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);//设置执行行号内容居中显示
        Auto_File_List_OrderItem->setText(JointStrDeal(m_OperateProOrder[i]));//拼接显示内容
        Auto_File_List_OrderItem->setData(Qt::TextAlignmentRole, Qt::AlignLeft);//内容左右对齐靠左显示
        Auto_File_List_OrderItem->setData(Qt::TextAlignmentRole, Qt::AlignVCenter);//内容上下对齐居中对齐显示
        Auto_File_List_OrderItem->setData(Qt::TextWordWrap,1);//设置内容自动换行显示
        ui->Auto_file_List->insertRow(i);
        ui->Auto_file_List->setItem(i,0,Auto_File_List_NumItem);   //显示命令执行序号
        ui->Auto_file_List->setItem(i,2,Auto_File_List_OrderItem);                 //显示命令内容
        ui->Auto_file_List->setItem(i,1,Auto_File_List_OrderColor);
        if(m_OperateProOrder[i].noteFlag == 1)
        {//屏蔽指令时行背景显示灰色
            ui->Auto_file_List->item(i,0)->setBackground(QColor(192, 191, 188));
            ui->Auto_file_List->item(i,1)->setBackground(QColor(192, 191, 188));
            ui->Auto_file_List->item(i,2)->setBackground(QColor(192, 191, 188));
        }

        m_CurrentSelectProOrderList = Old_CurrentSelectList;
        ui->Auto_file_List->selectRow(m_CurrentSelectProOrderList);
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


void AutoForm::on_btnEdit_pressed()
{

}


void AutoForm::on_btnAimedProduct_clicked()
{
    // call keyboard for labAimedProd
    callNumKeyBoard(qobject_cast<QLabel*>(ui->labAimedProd));

}

void AutoForm::on_btnEdit_toggled(bool checked)
{
    if (checked)
    {
            ui->stkWgtOuter->setCurrentWidget(ui->pageEdit);
    }
    else
    {
        ui->stkWgtOuter->setCurrentWidget(ui->pageState);
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
