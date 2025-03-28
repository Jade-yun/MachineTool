#include "stackedit.h"
#include "ui_stackedit.h"
#include <QMessageBox>
#include <QCloseEvent>
#include <QListView>
#include <QDebug>
#include <QThread>
#include "mainwindow.h"
QPointer<StackEdit> stack[8] = {nullptr};

void StackEdit::initWidgets()
{
    QTableWidget* tableWidget = ui->tableStack;
    /******************创建表格********************************/
    tableWidget->setRowCount(9);
    tableWidget->setColumnCount(3);
    ui->tableStack->setCornerButtonEnabled(true);
    ui->tableStack->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableStack->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    for (int i = 0; i < 3; ++i)
    {
        axisSelect[i] = new QComboBox(this);
        switch (i) {
            case 0:
                axisSelect[i]->addItems({ QObject::tr("X1轴")/*, QObject::tr("X2轴")*/ });
                break;
            case 1:
                axisSelect[i]->addItems({ QObject::tr("Y1轴"), QObject::tr("Y2轴") });
                break;
            case 2:
                axisSelect[i]->addItems({ QObject::tr("Z1轴"), QObject::tr("Z2轴") });
                break;
            default:
                break;
        }

        speed[i] = new NumberEdit(this);
        pointNum[i] = new NumberEdit(this);
        startPosA[i] = new NumberEdit(this);
        endPosB_X[i] = new NumberEdit(this);
        endPosC_Y[i] = new NumberEdit(this);
        posD[i] = new NumberEdit(this);
        speedDischage[i] = new NumberEdit(this);
        startPosDischage[i] = new NumberEdit(this);

        tableWidget->setCellWidget(0, i, axisSelect[i]);
        tableWidget->setCellWidget(1, i, speed[i]);
        tableWidget->setCellWidget(2, i, pointNum[i]);
        tableWidget->setCellWidget(3, i, startPosA[i]);
        tableWidget->setCellWidget(4, i, endPosB_X[i]);
        tableWidget->setCellWidget(5, i, endPosC_Y[i]);
        tableWidget->setCellWidget(6, i, posD[i]);
        tableWidget->setCellWidget(7, i, speedDischage[i]);
        tableWidget->setCellWidget(8, i, startPosDischage[i]);
    }

    axisSelectNormal[0] = ui->coboxAxisSelect0;
    axisSelectNormal[1] = ui->coboxAxisSelect1;
    axisSelectNormal[2] = ui->coboxAxisSelect2;

    startPosANormal[0] = ui->editStart0;
    startPosANormal[1] = ui->editStart1;
    startPosANormal[2] = ui->editStart2;

    speedNormal[0] = ui->editStackSpeed0;
    speedNormal[1] = ui->editStackSpeed1;
    speedNormal[2] = ui->editStackSpeed2;
    pointNumNormal[0] = ui->editStackNum0;
    pointNumNormal[1] = ui->editStackNum1;
    pointNumNormal[2] = ui->editStackNum2;

    stackDirect[0] = ui->coboxStackDirect0;
    stackDirect[1] = ui->coboxStackDirect1;
    stackDirect[2] = ui->coboxStackDirect2;
    intervalDis[0] = ui->editIntervalDis0;
    intervalDis[1] = ui->editIntervalDis1;
    intervalDis[2] = ui->editIntervalDis2;
    offset[0] = ui->editOffset0;
    offset[1] = ui->editOffset1;
    offset[2] = ui->editOffset2;


    QStringList verticalHeaderLabels = {
        QObject::tr("轴选择"),
        QObject::tr("速度"),
        QObject::tr("点数"),
        QObject::tr("A-起点"),
        QObject::tr("B-X方向终点"),
        QObject::tr("C-Y方向终点"),
        QObject::tr("D点坐标"),
        QObject::tr("放料速度"),
        QObject::tr("放料起始点")
    };
    tableWidget->setVerticalHeaderLabels(verticalHeaderLabels);

    QStringList horizontalHeaderLabels = {
        QObject::tr("X1轴"),
        QObject::tr("Y1轴"),
        QObject::tr("Z1轴")
    };
    tableWidget->setHorizontalHeaderLabels(horizontalHeaderLabels);


    /*****************************************************************/
    // 设置编辑输入框的精度和范围
    for (int i = 0; i < 3; i++)
    {
        speed[i]->setDecimalPlaces(0);
        pointNum[i]->setDecimalPlaces(0);
        startPosA[i]->setDecimalPlaces(2);
        endPosB_X[i]->setDecimalPlaces(2);
        endPosC_Y[i]->setDecimalPlaces(2);
        posD[i]->setDecimalPlaces(2);
        speedDischage[i]->setDecimalPlaces(0);
        startPosDischage[i]->setDecimalPlaces(2);

        speed[i]->setInputRange(1, 100);
        pointNum[i]->setInputRange(0, 100);
        speedDischage[i]->setInputRange(1, 100);

        startPosA[i]->setInputRange(-10000.00, 10000.00);
        endPosB_X[i]->setInputRange(-10000.00, 10000.00);
        endPosC_Y[i]->setInputRange(-10000.00, 10000.00);
        posD[i]->setInputRange(-10000.00, 10000.00);
        startPosDischage[i]->setInputRange(-10000.00, 10000.00);

        startPosANormal[i]->setDecimalPlaces(2);
        intervalDis[i]->setDecimalPlaces(2);
        speedNormal[i]->setDecimalPlaces(0);
        pointNumNormal[i]->setDecimalPlaces(0);
        offset[i]->setDecimalPlaces(2);

        startPosANormal[i]->setInputRange(-10000.00, 10000.00);
        intervalDis[i]->setInputRange(-10000.00, 10000.00);
        speedNormal[i]->setInputRange(1, 100);
        pointNumNormal[i]->setInputRange(0, 100);
        offset[i]->setInputRange(-10000.00, 10000.00);
    }
    // 三点/四点式 Z轴间距
    ui->editIntervalDis->setDecimalPlaces(2);
    ui->editIntervalDis->setInputRange(-10000.00, 10000.00);

    // 旋转料仓编辑输入框
    ui->editStartRotateBin->setInputRange(-10000.00, 10000.00);
    ui->editIntervalDisRotateBin->setDecimalPlaces(2);
    ui->editIntervalDisRotateBin->setInputRange(-10000.00, 10000.00);
    ui->editStackSpeedRotateBin->setDecimalPlaces(0);
    ui->editStackSpeedRotateBin->setInputRange(1, 100);
    ui->editStackNumRotateBin->setDecimalPlaces(0);
    ui->editStackNumRotateBin->setInputRange(0, 100);
    ui->editOffsetRotateBin->setDecimalPlaces(2);
    ui->editOffsetRotateBin->setInputRange(-10000.00, 10000.00);

    /****************************************************************/

    for (auto cobox : findChildren<QComboBox*>())
    {
//        cobox->setView(new QListView(cobox));
        cobox->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    }
    ui->btnFresh->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    ui->btnFresh0->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    ui->btnFresh1->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    ui->btnFresh2->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    ui->btnFreshRotateBin->setFocusPolicy(Qt::FocusPolicy::NoFocus);

    // 初始化时 ui 界面
    ui->btnOK->setVisible(false);
    ui->btnCancel->setVisible(false);
    ui->coboxStackOrder->setCurrentIndex(1);
    ui->coboxStackOrder->setCurrentIndex(0);
    ui->stkWgtStackWay->setCurrentIndex(0);
}

void StackEdit::switchStackWay(StackMode mode)
{
    QWidget* widgets [] = {
        ui->labOrder, ui->coboxStackOrder, ui->checkBoxTriAxisUnion
    };

    if (mode == StackMode::THREE_POINT || mode == StackMode::FOUR_POINT)
    {
        ui->stkWgtStackWay->setCurrentIndex(0);
        ui->grboxAxisZPara->show();
        ui->btnFresh->show();
        ui->btnMoveToStack->show();
        ui->btnMoveToFollow->show();

        ui->labGif->show();

        for (QWidget* widget : widgets)
        {
            widget->setEnabled(true);
        }

        bool enabled = mode == StackMode::FOUR_POINT;
        for (int i = 0; i < 3; i++)
        {
            posD[i]->setEnabled(enabled);
        }
    }
    else if (mode == StackMode::NORMAL)
    {
        ui->stkWgtStackWay->setCurrentIndex(1);
        ui->grboxAxisZPara->hide();
        ui->btnFresh->hide();
        ui->btnMoveToStack->hide();
        ui->btnMoveToFollow->hide();

        ui->labGif->show();

        for (QWidget* widget : widgets)
        {
            widget->setEnabled(true);
        }
    }
    else if (mode == StackMode::ROTATE_BIN)
    {
        ui->stkWgtStackWay->setCurrentIndex(2);
        ui->grboxAxisZPara->hide();
        ui->btnFresh->hide();
        ui->btnMoveToStack->hide();
        ui->btnMoveToFollow->hide();

        ui->labGif->hide();

        for (QWidget* widget : widgets)
        {
            widget->setEnabled(false);
        }
    }
    else if (mode == StackMode::TEACH_THREE_POINT || mode == StackMode::TEACH_FOUR_POINT)
    {
        ui->stkWgtStackWay->setCurrentIndex(0);
        ui->grboxAxisZPara->show();
        ui->btnFresh->show();
        ui->btnMoveToStack->hide();
        ui->btnMoveToFollow->hide();
        ui->btnOK->show();
        ui->btnCancel->show();

        ui->labGif->show();

        for (QWidget* widget : widgets)
        {
            widget->setEnabled(true);
        }

        bool enabled = mode == StackMode::TEACH_FOUR_POINT;
        for (int i = 0; i < 3; i++)
        {
            posD[i]->setEnabled(enabled);
        }
    }
    else if (mode == StackMode::TEACH_NOMAL)
    {
        ui->stkWgtStackWay->setCurrentIndex(1);
        ui->grboxAxisZPara->hide();
        ui->btnFresh->hide();
        ui->btnMoveToStack->hide();
        ui->btnMoveToFollow->hide();
        ui->btnOK->show();
        ui->btnCancel->show();

        ui->labGif->show();

        for (QWidget* widget : widgets)
        {
            widget->setEnabled(true);
        }
    }
    else if (mode == StackMode::TEACH_ROTATE_BIN)
    {
        ui->stkWgtStackWay->setCurrentIndex(2);
        ui->grboxAxisZPara->hide();
        ui->btnFresh->hide();
        ui->btnMoveToStack->hide();
        ui->btnMoveToFollow->hide();
        ui->btnOK->show();
        ui->btnCancel->show();

        ui->labGif->hide();

        for (QWidget* widget : widgets)
        {
            widget->setEnabled(false);
        }
    }
}

void StackEdit::showOKandCancelButton(bool isVisible)
{
    ui->btnOK->setVisible(isVisible);
    ui->btnCancel->setVisible(isVisible);
}

void StackEdit::saveStackBasicInfo()
{
    m_StackInfo[groupIndex].stackOrder = ui->coboxStackOrder->currentIndex();
    m_StackInfo[groupIndex].countMode = ui->coboxCountWay->currentIndex();
    m_StackInfo[groupIndex].stackFinStopFlag = ui->coboxStopAfterStack->currentIndex();
    m_StackInfo[groupIndex].groupLeaveBlank = ui->coboxLeaveBlank->currentIndex();
    m_StackInfo[groupIndex].stackMoveMode = ui->checkBoxTriAxisUnion->isChecked() ? 1 : 0;

    for (int i = 0; i < STACK_AXIS_NUM; i++)
    {
        int indexAsix=axisSelect[i]->currentIndex();
        if (m_StackFunc.stackType == 0 || m_StackFunc.stackType == 1)
        {
            indexAsix = axisSelect[i]->currentIndex();
        }
        else if (m_StackFunc.stackType == 2)
        {
            indexAsix = axisSelectNormal[i]->currentIndex();
        }
        switch (i)
        {
        case 0:
            if(indexAsix==0)
            {
                m_StackInfo[groupIndex].axisSelect[i]=1;
            }
            break;
        case 1:
            if(indexAsix==0)
            {
                m_StackInfo[groupIndex].axisSelect[i]=2;
            }
            else if(indexAsix==1)
            {
                m_StackInfo[groupIndex].axisSelect[i]=5;
            }
            break;
        case 2:
            if(indexAsix==0)
            {
                m_StackInfo[groupIndex].axisSelect[i]=3;
            }
            else if(indexAsix==1)
            {
                m_StackInfo[groupIndex].axisSelect[i]=6;
            }
            break;
        }

        if (m_StackFunc.stackType == 0 || m_StackFunc.stackType == 1)
        {
            m_StackInfo[groupIndex].axisSpeed[i] = speed[i]->text().toInt();
            m_StackInfo[groupIndex].stackPointNum[i] = pointNum[i]->text().toInt();
        }
        else if (m_StackFunc.stackType == 2)
        {
            m_StackInfo[groupIndex].axisSpeed[i] = speedNormal[i]->text().toInt();
            m_StackInfo[groupIndex].stackPointNum[i] = pointNumNormal[i]->text().toInt();
        }

        m_StackInfo[groupIndex].stackDir[i] = stackDirect[i]->currentIndex();
        m_StackInfo[groupIndex].dischangeSpeed[i] = (uint8_t)(speedDischage[i]->text().toUInt());
    }
    g_Usart->ExtendSendParDeal(CMD_MAIN_STACK,CMD_SUN_STACK_PAR,groupIndex+1);
    QThread::msleep(10);
    g_Usart->ExtendSendParDeal(CMD_MAIN_STACK,CMD_SUN_STACK_SET);
    setStackInfo(m_StackInfo[groupIndex],groupIndex);
}
void StackEdit::saveStackPointPosInfo()
{
    for (int i = 0; i < STACK_AXIS_NUM; i++)
    {
        if (m_StackFunc.stackType == 0 || m_StackFunc.stackType == 1)
        {
            m_StackInfo[groupIndex].stackStartPos[i] = startPosA[i]->text().toDouble()*100;
        }
        else if (m_StackFunc.stackType == 2)
        {
            m_StackInfo[groupIndex].stackStartPos[i] = startPosANormal[i]->text().toDouble()*100;
        }
//        m_StackInfo[groupIndex].stackStartPos[i] = startPosA[i]->text().toDouble()*100;
        m_StackInfo[groupIndex].stack_X_EndPos[i] = endPosB_X[i]->text().toDouble()*100;
        m_StackInfo[groupIndex].stack_Y_EndPos[i] = endPosC_Y[i]->text().toDouble()*100;
        m_StackInfo[groupIndex].stackDiagonalPos[i] = posD[i]->text().toDouble()*100;
        m_StackInfo[groupIndex].dischangeStartPos[i] = startPosDischage[i]->text().toDouble()*100;
        m_StackInfo[groupIndex].intevalDis[i] = intervalDis[i]->text().toDouble()*100;
        m_StackInfo[groupIndex].offsetDis[i] = offset[i]->text().toDouble()*100;
    }
    setStackInfo(m_StackInfo[groupIndex],groupIndex);
}

void StackEdit::saveRotateBinStackInfo()
{
//    m_StackInfo[groupIndex].stackOrder = ui->coboxStackOrder->currentIndex();
    m_StackInfo[groupIndex].countMode = ui->coboxCountWay->currentIndex();
    m_StackInfo[groupIndex].stackFinStopFlag = ui->coboxStopAfterStack->currentIndex();
    m_StackInfo[groupIndex].groupLeaveBlank = ui->coboxLeaveBlank->currentIndex();
//    m_StackInfo[groupIndex].stackMoveMode = ui->checkBoxTriAxisUnion->isChecked() ? 1 : 0;

    for (int i = 0; i < STACK_AXIS_NUM; i++)
    {
        m_StackInfo[groupIndex].axisSelect[i] = ui->coboxAxisSelectRotateBin->currentIndex();
        m_StackInfo[groupIndex].stackStartPos[i] = ui->editStartRotateBin->text().toFloat()*100;
        m_StackInfo[groupIndex].intevalDis[i] = ui->editIntervalDisRotateBin->text().toFloat()*100;
        m_StackInfo[groupIndex].axisSpeed[i] = ui->editStackSpeedRotateBin->text().toUInt();
        m_StackInfo[groupIndex].stackPointNum[i] = ui->editStackNumRotateBin->text().toInt();
        m_StackInfo[groupIndex].offsetDis[i] = ui->editOffsetRotateBin->text().toFloat()*100;;
        m_StackInfo[groupIndex].stackDir[i] = ui->coboxStackDirectRotateBin->currentIndex();
    }
    setStackInfo(m_StackInfo[groupIndex],groupIndex);

    g_Usart->ExtendSendParDeal(CMD_MAIN_STACK, CMD_SUN_STACK_PAR, groupIndex+1);
    QThread::msleep(10);
    g_Usart->ExtendSendParDeal(CMD_MAIN_STACK, CMD_SUN_STACK_SET);
    int axisIndex = ui->coboxAxisSelectRotateBin->currentIndex();
    g_Usart->ExtendSendParDeal(CMD_MAIN_STACK, CMD_SUN_STACK_POINT, groupIndex+1, axisIndex + 3);
}

// 这样写不行，后面写不通。需要换个方式来实现
// MoveStackFollowPoint.Stack_Index 换个方式来实现获取 index
void StackEdit::updateGroupIndex(int index)
{
    if (index > -1 && index < 8)
    {
        groupIndex = index;
//        MoveStackFollowPoint.Stack_Index = groupIndex+1;
    }
}

void StackEdit::StackAxisSelectQcomboboxRefresh()
{
    axisSelect[0]->clear();
    axisSelect[1]->clear();
    axisSelect[2]->clear();
    for(auto i=0;i<AXIS_TOTAL_NUM;i++)
    {
        if(m_AxisPar[i].axisType == 1)
        {
            if(i==X1_AXIS)
            {
                axisSelect[0]->addItem(m_NameDefine[1].axisName[X1_AXIS]);
            }
            if(i==Y1_AXIS)
            {
                axisSelect[1]->addItem(m_NameDefine[1].axisName[Y1_AXIS]);
            }
            if(i==Z1_AXIS)
            {
                axisSelect[2]->addItem(m_NameDefine[1].axisName[Z1_AXIS]);
            }
            if(i==Y2_AXIS)
            {
                axisSelect[1]->addItem(m_NameDefine[1].axisName[Y2_AXIS]);
            }
            if(i==Z2_AXIS)
            {
                axisSelect[2]->addItem(m_NameDefine[1].axisName[Z2_AXIS]);
            }
        }
    }
    if(axisSelect[0]->count()>0)
    {
        axisSelect[0]->setCurrentIndex(0);
    }

    int axisYIndex = m_StackInfo[groupIndex].axisSelect[1];
    int axisZIndex = m_StackInfo[groupIndex].axisSelect[2];
    if (axisYIndex == 2 && m_AxisPar[Y1_AXIS].axisType == 1)
    {
        if(axisSelect[1]->count()>0)
        {
            axisSelect[1]->setCurrentIndex(0);
        }
    }
    else if (axisYIndex == 5 && m_AxisPar[Y2_AXIS].axisType == 1)
    {
        if(axisSelect[1]->count()>1)
        {
            axisSelect[1]->setCurrentIndex(1);
        }
    }
    else
    {
        axisSelect[1]->setCurrentIndex(0);
//        saveStackBasicInfo();
    }

    if (axisZIndex == 3 && m_AxisPar[Z1_AXIS].axisType == 1)
    {
        if(axisSelect[2]->count()>0)
        {
            axisSelect[2]->setCurrentIndex(0);
        }
    }
    else if (axisZIndex == 6 && m_AxisPar[Z2_AXIS].axisType == 1)
    {
        if(axisSelect[2]->count()>1)
        {
            axisSelect[2]->setCurrentIndex(1);
        }
    }
    else
    {
        axisSelect[2]->setCurrentIndex(0);
//        saveStackBasicInfo();
    }

}
void StackEdit::syncParaToUI()
{
    ui->coboxStackOrder->setCurrentIndex(m_StackInfo[groupIndex].stackOrder);
    ui->coboxCountWay->setCurrentIndex(m_StackInfo[groupIndex].countMode);
    ui->coboxStopAfterStack->setCurrentIndex(m_StackInfo[groupIndex].stackFinStopFlag);
    ui->coboxLeaveBlank->setCurrentIndex(m_StackInfo[groupIndex].groupLeaveBlank);
    ui->checkBoxTriAxisUnion->setChecked(m_StackInfo[groupIndex].stackMoveMode == 1);

    axisSelect[0]->setCurrentIndex(0);
    axisSelectNormal[0]->setCurrentIndex(0);
    int axisYIndex = m_StackInfo[groupIndex].axisSelect[1];
    int axisZIndex = m_StackInfo[groupIndex].axisSelect[2];
    if (axisYIndex == 2)
    {
        axisSelect[1]->setCurrentIndex(0);
        axisSelectNormal[1]->setCurrentIndex(0);
    }
    else if (axisYIndex == 5)
    {
        axisSelect[1]->setCurrentIndex(1);
        axisSelectNormal[1]->setCurrentIndex(1);
    }
    if (axisZIndex == 3)
    {
        axisSelect[2]->setCurrentIndex(0);
        axisSelectNormal[2]->setCurrentIndex(0);
    }
    else if (axisZIndex == 6)
    {
        axisSelect[2]->setCurrentIndex(1);
        axisSelectNormal[2]->setCurrentIndex(1);
    }

    for (int i = 0; i < STACK_AXIS_NUM; i++)
    {
//        switch (m_StackInfo[groupIndex].axisSelect[i])
//        {
//        case 1:
//        case 2:
//        case 3:
//            axisSelect[i]->setCurrentIndex(0);
//            break;
//        case 5:
//        case 6:
//            axisSelect[i]->setCurrentIndex(1);
//            break;
//        }

        speed[i]->setText(QString::number(m_StackInfo[groupIndex].axisSpeed[i]));
        speedNormal[i]->setText(QString::number(m_StackInfo[groupIndex].axisSpeed[i]));
        pointNum[i]->setText(QString::number(m_StackInfo[groupIndex].stackPointNum[i]));
        pointNumNormal[i]->setText(QString::number(m_StackInfo[groupIndex].stackPointNum[i]));
        stackDirect[i]->setCurrentIndex(m_StackInfo[groupIndex].stackDir[i]);
        speedDischage[i]->setText(QString::number(m_StackInfo[groupIndex].dischangeSpeed[i]));
        startPosA[i]->setText(QString::number((double)m_StackInfo[groupIndex].stackStartPos[i] / 100, 'f', 2));
        startPosANormal[i]->setText(QString::number((double)m_StackInfo[groupIndex].stackStartPos[i] / 100, 'f', 2));
        endPosB_X[i]->setText(QString::number((double)m_StackInfo[groupIndex].stack_X_EndPos[i] / 100, 'f', 2));
        endPosC_Y[i]->setText(QString::number((double)m_StackInfo[groupIndex].stack_Y_EndPos[i] / 100, 'f', 2));
        posD[i]->setText(QString::number((double)m_StackInfo[groupIndex].stackDiagonalPos[i] / 100, 'f', 2));
        startPosDischage[i]->setText(QString::number((double)m_StackInfo[groupIndex].dischangeStartPos[i] / 100, 'f', 2));
        intervalDis[i]->setText(QString::number((double)m_StackInfo[groupIndex].intevalDis[i] / 100, 'f', 2));
        offset[i]->setText(QString::number((double)m_StackInfo[groupIndex].offsetDis[i] / 100, 'f', 2));
    }
    ui->editIntervalDis->setText(QString::number((double)m_StackInfo[groupIndex].intevalDis[2] / 100, 'f', 2));
    ui->coboxStackDirect->setCurrentIndex(m_StackInfo[groupIndex].stackDir[2]);
}

StackEdit::StackEdit(int groupIndex, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StackEdit),
    movie(nullptr),
    groupIndex(groupIndex), pressedRow(-1)
{
    ui->setupUi(this);

    initWidgets();

    connect(ui->btnOK, &QPushButton::clicked, [=](){
        saveStackBasicInfo();//保存参数
        saveStackPointPosInfo();
        for(int i=0;i<STACK_AXIS_NUM;i++)
        {
            g_Usart->ExtendSendParDeal(CMD_MAIN_STACK,CMD_SUN_STACK_POINT,groupIndex+1,i+1);
        }
        this->close();
        emit closeStackEditDialogSignal();
        emit stackParRefreshSignal();
    });
    connect(ui->btnCancel, &QPushButton::clicked, [=](){
        this->close();
        emit closeStackEditDialogSignal();
    });


    moveStack = new StackFollow;
    moveFollow = new StackFollow;

    connect(ui->btnMoveToStack, &QPushButton::clicked, moveStack,[=](){
        MoveStackFollowPoint.Stack_Type = 1;//移至堆叠点
        MoveStackFollowPoint.Stack_Index = groupIndex + 1;
        emit StackFollowRefreshSignal();
        moveStack->exec();
    });
    connect(ui->btnMoveToFollow, &QPushButton::clicked, moveFollow,[=](){
        MoveStackFollowPoint.Stack_Type = 2;//移至跟随点
        MoveStackFollowPoint.Stack_Index = groupIndex + 1;
        emit StackFollowRefreshSignal();
        moveFollow->exec();
    });

    connect(this,&StackEdit::StackFollowRefreshSignal,moveStack,&StackFollow::StackFollowInit);
    connect(this,&StackEdit::StackFollowRefreshSignal,moveFollow,&StackFollow::StackFollowInit);

    logicSigsSlots();
    saveInfoConnections();
    setupRefreshConnections();

}

StackEdit::~StackEdit()
{
    delete ui;
    delete moveStack;
    delete moveFollow;
}

void StackEdit::loadAndPlayGif(const QString& path)
{
    if (movie)
    {
        movie->stop();
    }
    else {
        movie = new QMovie(this);
    }
    //    movie = new QMovie(path, this);
    //    ui->gifLabel->resize(width, height);
    movie->setFileName(path);
    movie->setSpeed(100);
    movie->setScaledSize(ui->labGif->size());
    ui->labGif->setMovie(movie);
    movie->start();
}

void StackEdit::logicSigsSlots()
{
    connect(ui->coboxAxisSelectRotateBin, &QComboBox::currentTextChanged, [=](const QString& text)
    {
        ui->labAxisRotateBin->setText(text);
    });
    /***********普通堆叠模式下参数变化导致构件使能变化和**********************/

    connect(ui->coboxAxisSelect0, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
        ui->labAxis0->setText(QString("X%1").arg(index + 1));
    });

    connect(ui->coboxAxisSelect1, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
        ui->labAxis1->setText(QString("Y%1").arg(index + 1));
    });

    connect(ui->coboxAxisSelect2, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
        ui->labAxis2->setText(QString("Z%1").arg(index + 1));
    });

    connect(ui->editStackNum0, &NumberEdit::textChanged,[=](){

        bool disable = ui->editStackNum0->text().toInt() == 0;

        ui->btnFresh0->setEnabled(!disable);
        ui->coboxAxisSelect0->setEnabled(!disable);
        ui->editStart0->setEnabled(!disable);
        ui->editIntervalDis0->setEnabled(!disable);
        ui->editStackSpeed0->setEnabled(!disable);
        ui->editOffset0->setEnabled(!disable);
        ui->coboxStackDirect0->setEnabled(!disable);
    });

    connect(ui->editStackNum1, &NumberEdit::textChanged, [=]() {
        bool disable = ui->editStackNum1->text().toInt() == 0;

        ui->btnFresh1->setEnabled(!disable);
        ui->coboxAxisSelect1->setEnabled(!disable);
        ui->editStart1->setEnabled(!disable);
        ui->editIntervalDis1->setEnabled(!disable);
        ui->editStackSpeed1->setEnabled(!disable);
        ui->editOffset1->setEnabled(!disable);
        ui->coboxStackDirect1->setEnabled(!disable);
    });

    connect(ui->editStackNum2, &NumberEdit::textChanged, [=]() {
        bool disable = ui->editStackNum2->text().toInt() == 0;

        ui->btnFresh2->setEnabled(!disable);
        ui->coboxAxisSelect2->setEnabled(!disable);
        ui->editStart2->setEnabled(!disable);
        ui->editIntervalDis2->setEnabled(!disable);
        ui->editStackSpeed2->setEnabled(!disable);
        ui->editOffset2->setEnabled(!disable);
        ui->coboxStackDirect2->setEnabled(!disable);
    });

    //
    for (int i = 0; i < 3; i++)
    {
        connect(axisSelect[i], QOverload<int>::of(&QComboBox::currentIndexChanged), [=]() {
            ui->tableStack->horizontalHeaderItem(i)->setText(axisSelect[i]->currentText());
        });
    }

    // 旋转料仓模式下
    connect(ui->editStackNumRotateBin, &NumberEdit::textChanged, [=]() {
        bool disable = ui->editStackNumRotateBin->text() == "0";

        ui->btnFreshRotateBin->setEnabled(!disable);
        ui->coboxAxisSelectRotateBin->setEnabled(!disable);
        ui->editStartRotateBin->setEnabled(!disable);
        ui->editIntervalDisRotateBin->setEnabled(!disable);
        ui->editStackSpeedRotateBin->setEnabled(!disable);
        ui->editOffsetRotateBin->setEnabled(!disable);
        ui->coboxStackDirectRotateBin->setEnabled(!disable);
    });
    connect(ui->coboxAxisSelectRotateBin, QOverload<int>::of(&QComboBox::currentIndexChanged), [=]() {
        ui->labAxisRotateBin->setText(ui->coboxAxisSelectRotateBin->currentText());
    });
}

void StackEdit::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
        retranslate();
        // 需要重新同步参数
        this->syncParaToUI();
    }
    QWidget::changeEvent(e);
}

void StackEdit::retranslate()
{
    QStringList verticalHeaderLabels = {
        QObject::tr("轴选择"),
        QObject::tr("速度"),
        QObject::tr("点数"),
        QObject::tr("A-起点"),
        QObject::tr("B-X方向终点"),
        QObject::tr("C-Y方向终点"),
        QObject::tr("D点坐标"),
        QObject::tr("放料速度"),
        QObject::tr("放料起始点")
    };
    ui->tableStack->setVerticalHeaderLabels(verticalHeaderLabels);

    QStringList horizontalHeaderLabels = {
        QObject::tr("X1轴"),
        QObject::tr("Y1轴"),
        QObject::tr("Z1轴")
    };
    ui->tableStack->setHorizontalHeaderLabels(horizontalHeaderLabels);

    axisSelect[0]->setItemText(0, QObject::tr("X1轴"));
    axisSelect[1]->setItemText(0, QObject::tr("Y1轴"));
    axisSelect[1]->setItemText(1, QObject::tr("Y2轴"));
    axisSelect[2]->setItemText(0, QObject::tr("Z1轴"));
    axisSelect[2]->setItemText(1, QObject::tr("Z2轴"));

}

void StackEdit::showEvent(QShowEvent *event)
{
    stack[groupIndex]->syncParaToUI();

    QWidget::showEvent(event);
}

void StackEdit::saveInfoConnections()
{
    connect(ui->coboxStackOrder, QOverload<int>::of(&QComboBox::activated), this, [=](int ){
        saveStackBasicInfo();
    });
    connect(ui->coboxCountWay, QOverload<int>::of(&QComboBox::activated), this, [=](int ){
        saveStackBasicInfo();
    });
    connect(ui->coboxStopAfterStack, QOverload<int>::of(&QComboBox::activated), this, [=](int ){
        saveStackBasicInfo();
    });
    connect(ui->coboxLeaveBlank, QOverload<int>::of(&QComboBox::activated), this, [=](int ){
        saveStackBasicInfo();
    });
    connect(ui->checkBoxTriAxisUnion, &QCheckBox::clicked, this, &StackEdit::saveStackBasicInfo);

//#begin
    /*********************普通模式和三点/四点式下共有的参数**********************************/
    connect(ui->coboxStackDirect, QOverload<int>::of(&QComboBox::activated), this, [=](int index){
        m_StackInfo[groupIndex].stackDir[2] = index;

        g_Usart->ExtendSendParDeal(CMD_MAIN_STACK,CMD_SUN_STACK_PAR,groupIndex+1);
        QThread::msleep(10);
        g_Usart->ExtendSendParDeal(CMD_MAIN_STACK,CMD_SUN_STACK_SET);
        setStackInfo(m_StackInfo[groupIndex],groupIndex);
    });
    connect(ui->editIntervalDis, &NumberEdit::returnPressed,[=](){
        auto value = ui->editIntervalDis->text().toFloat();
        m_StackInfo[groupIndex].intevalDis[2] = static_cast<int32_t>(value * 100);

        g_Usart->ExtendSendParDeal(CMD_MAIN_STACK,CMD_SUN_STACK_POINT,groupIndex+1,3);
        setStackInfo(m_StackInfo[groupIndex],groupIndex);
    });

    for (int i = 0; i < STACK_AXIS_NUM; i++)
    {
        connect(axisSelect[i], QOverload<int>::of(&QComboBox::activated), this, [=](int index){
            if (index != -1)
            {
                saveStackBasicInfo();
            }
        });
        connect(axisSelectNormal[i], QOverload<int>::of(&QComboBox::activated), this, [=](int index){
            if (index != -1)
            {
                saveStackBasicInfo();
            }
        });
        connect(startPosA[i],&NumberEdit::returnPressed, [=](){
            saveStackPointPosInfo();
            g_Usart->ExtendSendParDeal(CMD_MAIN_STACK,CMD_SUN_STACK_POINT,groupIndex+1,i+1);
        });
        connect(startPosANormal[i],&NumberEdit::returnPressed, [=](){
            saveStackPointPosInfo();
            g_Usart->ExtendSendParDeal(CMD_MAIN_STACK,CMD_SUN_STACK_POINT,groupIndex+1,i+1);
        });
        connect(pointNum[i],&NumberEdit::returnPressed,[=](){
            saveStackBasicInfo();
        });
        connect(pointNumNormal[i],&NumberEdit::returnPressed,[=](){
            saveStackBasicInfo();
        });
        connect(speed[i],&NumberEdit::returnPressed,[=](){
            saveStackBasicInfo();
        });
        connect(speedNormal[i],&NumberEdit::returnPressed,[=](){
            saveStackBasicInfo();
        });
        /*********************普通模式和三点/四点式下共有的参数**********************************/
//#end

        connect(stackDirect[i], QOverload<int>::of(&QComboBox::activated), this, [=](int index){
            if (index != -1)
            {
                saveStackBasicInfo();
            }
        });
        connect(speedDischage[i],&NumberEdit::returnPressed,[=](){
            saveStackBasicInfo();
        });
        connect(endPosB_X[i],&NumberEdit::returnPressed,[=](){
            saveStackPointPosInfo();
            g_Usart->ExtendSendParDeal(CMD_MAIN_STACK,CMD_SUN_STACK_POINT,groupIndex+1,i+1);
        });
        connect(endPosC_Y[i],&NumberEdit::returnPressed,[=](){
            saveStackPointPosInfo();
            g_Usart->ExtendSendParDeal(CMD_MAIN_STACK,CMD_SUN_STACK_POINT,groupIndex+1,i+1);
        });
        connect(posD[i],&NumberEdit::returnPressed,[=](){
            saveStackPointPosInfo();
            g_Usart->ExtendSendParDeal(CMD_MAIN_STACK,CMD_SUN_STACK_POINT,groupIndex+1,i+1);
        });
        connect(startPosDischage[i],&NumberEdit::returnPressed,[=](){
            saveStackPointPosInfo();
            g_Usart->ExtendSendParDeal(CMD_MAIN_STACK,CMD_SUN_STACK_POINT,groupIndex+1,i+1);
        });
        connect(intervalDis[i],&NumberEdit::returnPressed,[=](){
            saveStackPointPosInfo();
            g_Usart->ExtendSendParDeal(CMD_MAIN_STACK,CMD_SUN_STACK_POINT,groupIndex+1,i+1);
        });
        connect(offset[i],&NumberEdit::returnPressed,[=](){
            saveStackPointPosInfo();
            g_Usart->ExtendSendParDeal(CMD_MAIN_STACK,CMD_SUN_STACK_POINT,groupIndex+1,i+1);
        });
    }
}

void StackEdit::setupRefreshConnections()
{
    connect(ui->btnFresh, &QPushButton::clicked, this, &StackEdit::onRefreshClicked);
    connect(ui->tableStack->verticalHeader(), &QHeaderView::sectionPressed, this, [this](int logicalIndex){
//        qDebug() << "Pressed row: " << logicalIndex;
        pressedRow = logicalIndex;

        QWidget *focusedWidget = ui->tableStack->focusWidget();
        if (focusedWidget)
        {
            focusedWidget->clearFocus();
        }
    });

    std::vector<QPushButton*> refreshBtns = {
        ui->btnFresh0, ui->btnFresh1, ui->btnFresh2
    };
    for (int i = 0; i < 3; i++)
    {
        connect(refreshBtns[i], &QPushButton::clicked, this, [this, i](){
            onRefreshNormalStackClicked(i);
        });
    }
}

void StackEdit::on_coboxStackOrder_currentIndexChanged(int index)
{
    switch (index) {
    case 0:

    loadAndPlayGif(":/images/gif/YXZ.gif");
        break;
    case 1:

    loadAndPlayGif(":/images/gif/XYZ.gif");
        break;
    case 2:

    loadAndPlayGif(":/images/gif/ZYX.gif");
        break;
    case 3:

    loadAndPlayGif(":/images/gif/ZXY.gif");
        break;
    default:
        break;
    }
}

void StackEdit::onRefreshClicked()
{
    const QSet<int> positionRows = {
       3, 4, 5, 6, 8
    };

    auto updateCellEdit = [this](int row, int col){
        auto curWidget = ui->tableStack->cellWidget(row, col);
            if (!curWidget) return;

            auto edit = qobject_cast<NumberEdit *>(curWidget);
            if (!edit) return;

            const std::vector<int> axisPos = {
                m_AxisCurPos.Pos_x, m_AxisCurPos.Pos_y, m_AxisCurPos.Pos_z,
                m_AxisCurPos.Pos_c, m_AxisCurPos.Pos_y2, m_AxisCurPos.Pos_z2
            };

            int axisIndex = m_StackInfo[groupIndex].axisSelect[col] - 1;
            if (axisIndex < 0 || axisIndex > 5) {
                qDebug() << "onRefreshClicked, Invalid axisIndex: " << axisIndex;
                return;
            }

            QString newValue = QString::number(axisPos.at(axisIndex) / 100.0, 'f', 2);
            QString oldValue = edit->text();

            if (newValue == oldValue) return;

            edit->setText(newValue);

            // 触发对应保存参数的槽
            emit edit->returnPressed();
    };


    int curRow = ui->tableStack->currentRow();
    int curCol = ui->tableStack->currentColumn();

    if (curRow < 0 || curCol < 0) return;

    // 选中的不是和位置相关的控件所在行
    if (!positionRows.contains(curRow)) return;

    // 按下表头选中整行
    QWidget *focusedWidget = QApplication::focusWidget();
    auto curWidget = ui->tableStack->cellWidget(curRow, curCol);
    if (pressedRow != -1 && pressedRow == curRow && focusedWidget != curWidget)
    {
        const int columnCount = 3;
        for (int col = 0; col < columnCount; ++col)
        {
            updateCellEdit(curRow, col);
        }
    }
    else
    {
        updateCellEdit(curRow, curCol);
    }


//    QWidget *focusedWidget = QApplication::focusWidget();

//    if (focusedWidget) {
//        auto *focusedEdit = qobject_cast<NumberEdit *>(focusedWidget);
//        if (focusedEdit) {

////            focusedEdit->setFocus();
//        }
    //    }
}

void StackEdit::onRefreshNormalStackClicked(int colIndex)
{
    if (colIndex < 0 || colIndex > 2)
    {
        qDebug() << "onRefreshNormalStackClicked, Invalid index: " << colIndex;
        return;
    }

    int axisIndex = m_StackInfo[groupIndex].axisSelect[colIndex] - 1;
    if (axisIndex < 0 || axisIndex > 5)
    {
        qDebug() << "onRefreshNormalStackClicked, Invalid axisIndex: " << axisIndex;
        return;
    }
    const std::vector<int> axisPos = {
        m_AxisCurPos.Pos_x, m_AxisCurPos.Pos_y, m_AxisCurPos.Pos_z,
        m_AxisCurPos.Pos_c, m_AxisCurPos.Pos_y2, m_AxisCurPos.Pos_z2
    };

    QString newText = QString::number(axisPos.at(axisIndex) / 100.0, 'f', 2);
    QString oldText = startPosANormal[colIndex]->text();
    if (oldText != newText)
    {
        startPosANormal[colIndex]->setText(newText);

        emit startPosANormal[colIndex]->returnPressed();
    }
}

StackFollow::StackFollow(QWidget *parent)
    : QDialog(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    table = new QTableWidget(3, 2, this);
    table->setHorizontalHeaderLabels({QObject::tr("个数"), QObject::tr("目标位置")});
    table->setVerticalHeaderLabels({tr("X1轴"), tr("Y1轴"), tr("Z1轴")});
    table->verticalHeader()->setDefaultSectionSize(40);
    table->horizontalHeader()->setDefaultSectionSize(130);

    for (int i = 0; i < 3; i++)
    {
        nums[i] = new NumberEdit();
        nums[i]->setFixedHeight(40);
        nums[i]->setDecimalPlaces(0);

        table->setCellWidget(i, 0, nums[i]);
        objPos[i] = new QTableWidgetItem("0.00");
        table->setItem(i, 1, objPos[i]);
        connect(nums[i],&NumberEdit::returnPressed, this,[=](){
            if(MoveStackFollowPoint.Stack_Index>0)
            {
                if(nums[i]->text().toUInt()<1 || nums[i]->text().toUInt()>m_StackInfo[MoveStackFollowPoint.Stack_Index-1].stackPointNum[i])
                {
                    auto tip_text = QString(tr("输入值的范围不能超过%1～%2,请重新输入！")).arg(1).arg(m_StackInfo[MoveStackFollowPoint.Stack_Index-1].stackPointNum[i]);
                    this->hide();
                    int reply = MainWindow::pMainWindow->showErrorTip(tip_text,TipMode::ONLY_OK);
                    if (reply == QDialog::Accepted || reply == QDialog::Rejected)
                    {
                        nums[i]->setText(QString::number(MoveStackFollowPoint.Stack_Point[i]));
                    }
                }
                else
                {
                    MoveStackFollowPoint.Stack_Point[i] = (uint8_t)nums[i]->text().toUInt();
                    if(MoveStackFollowPoint.Stack_Type == 1)
                    {//堆叠坐标读取
                        g_Usart->ExtendSendReadPar(CMD_MAIN_STA,CMD_SUN_STA_STACK_NUM_POS,MoveStackFollowPoint.Stack_Index,0);//发送设置的点数，读取坐标
                    }
                    else if(MoveStackFollowPoint.Stack_Type == 2)
                    {//跟随堆叠坐标读取
                        g_Usart->ExtendSendReadPar(CMD_MAIN_STA,CMD_SUN_STA_STACK_FOLLOW_NUM_POS,MoveStackFollowPoint.Stack_Index,0);//发送设置的点数，读取坐标
                    }
                }
            }
        });
    }
    connect(g_Usart,&Usart::Stack_Axis_Pos_Refresh,this,[=](){
        objPos[0]->setText(QString::number(((double)MoveStackFollowPoint.Stack_Axis_Pos[0])/100,'f',2));
        objPos[1]->setText(QString::number(((double)MoveStackFollowPoint.Stack_Axis_Pos[1])/100,'f',2));
        objPos[2]->setText(QString::number(((double)MoveStackFollowPoint.Stack_Axis_Pos[2])/100,'f',2));
    });
    StackFollowInit();//初始化参数
    moveButton = new QPushButton(tr("移动"), this);
    moveButton->setFixedHeight(45);
    moveButton->setFixedWidth(110);
    connect(moveButton,&QPushButton::pressed,this,[=](){//按下移动按钮时，下发移动至堆叠点数据
        if(MoveStackFollowPoint.Stack_Index>0)
        {
            if(m_StackInfo[MoveStackFollowPoint.Stack_Index-1].stackPointNum[0]>0 && m_AxisPar[X1_AXIS].axisType != 1)
            {
                this->close();
                MainWindow::pMainWindow->showErrorTip(tr("X1轴伺服未开启，请把轴的堆叠点数设为0"),TipMode::ONLY_OK);
            }
            else if(m_StackInfo[MoveStackFollowPoint.Stack_Index-1].stackPointNum[1]>0 && m_AxisPar[Y1_AXIS].axisType != 1)
            {
                this->close();
                MainWindow::pMainWindow->showErrorTip(tr("Y1轴伺服未开启，请把轴的堆叠点数设为0"),TipMode::ONLY_OK);
            }
            else if(m_StackInfo[MoveStackFollowPoint.Stack_Index-1].stackPointNum[2]>0 && m_AxisPar[Z1_AXIS].axisType != 1)
            {
                this->close();
                MainWindow::pMainWindow->showErrorTip(tr("Z1轴伺服未开启，请把轴的堆叠点数设为0"),TipMode::ONLY_OK);
            }
            else
            {
                g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_STACK,0,0);//parNum=0,发送移动到目标位置 parNum=1,停止
            }
        }

    });
    connect(moveButton,&QPushButton::released,this,[=](){
        g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_STACK,1,0);//parNum=0,发送移动到目标位置 parNum=1,停止
    });
    closeButton = new QPushButton(tr("关闭"), this);
    closeButton->setFixedHeight(45);
    closeButton->setFixedWidth(110);
    connect(closeButton, &QPushButton::clicked, this, &StackFollow::close);


    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(moveButton);
    buttonLayout->addWidget(closeButton);

    mainLayout->addWidget(table);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);

    setFixedSize(450, 300);

}
//界面参数初始化
void StackFollow::StackFollowInit()
{
    if(MoveStackFollowPoint.Stack_Index>0)
    {
        for(int index=0;index<3;index++)
        {
            if(m_StackInfo[MoveStackFollowPoint.Stack_Index-1].stackPointNum[index]>0)
            {//根据堆叠点数设置个数卡框是否可以输入
                nums[index]->setEnabled(true);
                MoveStackFollowPoint.Stack_Point[index] = 1;
                nums[index]->setText(QString::number(MoveStackFollowPoint.Stack_Point[index]));
            }
            else
            {
                nums[index]->setEnabled(false);
                MoveStackFollowPoint.Stack_Point[index] = 0;
                nums[index]->setText(QString::number(MoveStackFollowPoint.Stack_Point[index]));
            }
        }
    }

}

void StackFollow::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange)
    {
        table->setHorizontalHeaderLabels({QObject::tr("个数"), QObject::tr("目标位置")});
        table->setVerticalHeaderLabels({tr("X1轴"), tr("Y1轴"), tr("Z1轴")});

        closeButton->setText(tr("关闭"));
        moveButton->setText(tr("移动"));
    }
    QWidget::changeEvent(e);
}
void StackFollow::updateVerticalHeader(int index, const QString &text)
{
    QStringList headers = table->verticalHeader()->model()->headerData(index, Qt::Vertical).toStringList();
    headers[index] = text;
    table->setVerticalHeaderLabels(headers);
}
