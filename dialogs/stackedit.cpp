#include "stackedit.h"
#include "ui_stackedit.h"
#include <QMessageBox>
#include <QCloseEvent>
#include <QListView>
#include <QDebug>
#include <QThread>
#include "mainwindow.h"
QPointer<StackEdit> stack[8] = {nullptr};

void StackEdit::initStackPara(QTableWidget* tableWidget)
{
    /******************创建表格********************************/
    tableWidget->setRowCount(9);
    tableWidget->setColumnCount(3);

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

        speed[i]->setDecimalPlaces(0);
        pointNum[i]->setDecimalPlaces(0);
        startPosA[i]->setDecimalPlaces(2);
        endPosB_X[i]->setDecimalPlaces(2);
        endPosC_Y[i]->setDecimalPlaces(2);
        posD[i]->setDecimalPlaces(2);
        speedDischage[i]->setDecimalPlaces(0);
        startPosDischage[i]->setDecimalPlaces(2);

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

    auto setEditPrecision_0 = [&](const QVector<NumberEdit*>& edits){
        for (NumberEdit* edit : edits)
        {
            edit->setDecimalPlaces(0);
        }
    };
    auto setEditPrecision_2 = [&](const QVector<NumberEdit*>& edits){
        for (NumberEdit* edit : edits)
        {
            edit->setDecimalPlaces(2);
        }
    };

    setEditPrecision_0({ui->editStackNum0, ui->editStackNum1, ui->editStackNum2,
                        ui->editStackSpeed0, ui->editStackSpeed1, ui->editStackSpeed2,
                        ui->editStackSpeedRotateBin, ui->editStackNumRotateBin});
    setEditPrecision_2({ui->editStart0, ui->editStart1, ui->editStart2,
                        ui->editIntervalDis0, ui->editIntervalDis1, ui->editIntervalDis2, ui->editIntervalDis,
                        ui->editOffset0, ui->editOffset1, ui->editOffset2});
}

void StackEdit::switchStackWay(StackMode mode)
{
    QWidget* widgets [] = {
        ui->labOrder, ui->coboxStackOrder, ui->checkBoxTriAxisUnion
    };

    if (mode == 0 || mode == 1)
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
    else if (mode == 2)
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
}

void StackEdit::showOKandCancelButton(bool isVisible)
{
    ui->btnOK->setVisible(isVisible);
    ui->btnCancel->setVisible(isVisible);
}

void StackEdit::saveStackInfo1()
{
    m_StackInfo[groupIndex].stackOrder = ui->coboxStackOrder->currentIndex();
    m_StackInfo[groupIndex].countMode = ui->coboxCountWay->currentIndex();
    m_StackInfo[groupIndex].stackFinStopFlag = ui->coboxStopAfterStack->currentIndex();
    m_StackInfo[groupIndex].groupLeaveBlank = ui->coboxLeaveBlank->currentIndex();
    m_StackInfo[groupIndex].stackMoveMode = ui->checkBoxTriAxisUnion->isChecked() ? 1 : 0;

    for (int i = 0; i < STACK_AXIS_NUM; i++)
    {
        int indexAsix=axisSelect[i]->currentIndex();
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
        m_StackInfo[groupIndex].axisSpeed[i] = speed[i]->text().toInt();
        m_StackInfo[groupIndex].stackPointNum[i] = pointNum[i]->text().toInt();
        m_StackInfo[groupIndex].stackDir[i] = stackDirect[i]->currentIndex();
        m_StackInfo[groupIndex].dischangeSpeed[i] = (uint8_t)(speedDischage[i]->text().toUInt());
    }
    g_Usart->ExtendSendParDeal(CMD_MAIN_STACK,CMD_SUN_STACK_PAR,groupIndex+1);
    QThread::msleep(10);
    g_Usart->ExtendSendParDeal(CMD_MAIN_STACK,CMD_SUN_STACK_SET);
    setStackInfo(m_StackInfo[groupIndex],groupIndex);
}
void StackEdit::saveStackInfo(int index)
{
    m_StackInfo[groupIndex].stackOrder = ui->coboxStackOrder->currentIndex();
    m_StackInfo[groupIndex].countMode = ui->coboxCountWay->currentIndex();
    m_StackInfo[groupIndex].stackFinStopFlag = ui->coboxStopAfterStack->currentIndex();
    m_StackInfo[groupIndex].groupLeaveBlank = ui->coboxLeaveBlank->currentIndex();
    m_StackInfo[groupIndex].stackMoveMode = ui->checkBoxTriAxisUnion->isChecked() ? 1 : 0;

    for (int i = 0; i < STACK_AXIS_NUM; i++)
    {
        int indexAsix=axisSelect[i]->currentIndex();
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
//        m_StackInfo[groupIndex].axisSelect[i] = axisSelect[i]->currentIndex();
        m_StackInfo[groupIndex].axisSpeed[i] = speed[i]->text().toInt();
        m_StackInfo[groupIndex].stackPointNum[i] = pointNum[i]->text().toInt();
        m_StackInfo[groupIndex].stackDir[i] = stackDirect[i]->currentIndex();
        m_StackInfo[groupIndex].dischangeSpeed[i] = speedDischage[i]->text().toInt();
        m_StackInfo[groupIndex].stackStartPos[i] = startPosA[i]->text().toInt();
        m_StackInfo[groupIndex].stack_X_EndPos[i] = endPosB_X[i]->text().toDouble()*100;
        m_StackInfo[groupIndex].stack_Y_EndPos[i] = endPosC_Y[i]->text().toDouble()*100;
        m_StackInfo[groupIndex].stackDiagonalPos[i] = posD[i]->text().toDouble()*100;
        m_StackInfo[groupIndex].dischangeStartPos[i] = startPosDischage[i]->text().toDouble()*100;
        m_StackInfo[groupIndex].intevalDis[i] = intervalDis[i]->text().toDouble()*100;
        m_StackInfo[groupIndex].offsetDis[i] = offset[i]->text().toDouble()*100;
    }

//    g_Usart->ExtendSendParDeal(CMD_MAIN_STACK,CMD_SUN_STACK_PAR,groupIndex+1);
//    QThread::msleep(10);
    g_Usart->ExtendSendParDeal(CMD_MAIN_STACK,CMD_SUN_STACK_POINT,groupIndex+1,index+1);
    setStackInfo(m_StackInfo[groupIndex],groupIndex);
}

void StackEdit::updateGroupIndex(int index)
{
    if (index > -1 && index < 8)
    {
        groupIndex = index;
        MoveStackFollowPoint.Stack_Index = groupIndex+1;
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
    int axisYIndex = m_StackInfo[groupIndex].axisSelect[1];
    int axisZIndex = m_StackInfo[groupIndex].axisSelect[2];
    if (axisYIndex == 2)
    {
        axisSelect[1]->setCurrentIndex(0);
    }
    else if (axisYIndex == 5)
    {
        axisSelect[1]->setCurrentIndex(1);
    }
    if (axisZIndex == 3)
    {
        axisSelect[2]->setCurrentIndex(0);
    }
    else if (axisZIndex == 6)
    {
        axisSelect[2]->setCurrentIndex(1);
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
        pointNum[i]->setText(QString::number(m_StackInfo[groupIndex].stackPointNum[i]));
        stackDirect[i]->setCurrentIndex(m_StackInfo[groupIndex].stackDir[i]);
        speedDischage[i]->setText(QString::number(m_StackInfo[groupIndex].dischangeSpeed[i]));
        startPosA[i]->setText(QString::number((double)m_StackInfo[groupIndex].stackStartPos[i] / 100, 'f', 2));
        endPosB_X[i]->setText(QString::number((double)m_StackInfo[groupIndex].stack_X_EndPos[i] / 100, 'f', 2));
        endPosC_Y[i]->setText(QString::number((double)m_StackInfo[groupIndex].stack_Y_EndPos[i] / 100, 'f', 2));
        posD[i]->setText(QString::number((double)m_StackInfo[groupIndex].stackDiagonalPos[i] / 100, 'f', 2));
        startPosDischage[i]->setText(QString::number((double)m_StackInfo[groupIndex].dischangeStartPos[i] / 100, 'f', 2));
        intervalDis[i]->setText(QString::number((double)m_StackInfo[groupIndex].intevalDis[i] / 100, 'f', 2));
        offset[i]->setText(QString::number((double)m_StackInfo[groupIndex].offsetDis[i] / 100, 'f', 2));
    }
}

StackEdit::StackEdit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StackEdit),movie(nullptr),
    groupIndex(0)
{
    ui->setupUi(this);

    ui->tableStack->setCornerButtonEnabled(true);
    ui->tableStack->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableStack->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    initStackPara(ui->tableStack);

    for (int i = 0; i < 3; i++)
    {
        connect(axisSelect[i], QOverload<int>::of(&QComboBox::currentIndexChanged), [=]() {
            ui->tableStack->horizontalHeaderItem(i)->setText(axisSelect[i]->currentText());
        });
    }

    ui->btnOK->setVisible(false);
    ui->btnCancel->setVisible(false);
    ui->coboxStackOrder->setCurrentIndex(1);
    ui->coboxStackOrder->setCurrentIndex(0);

    ui->stkWgtStackWay->setCurrentIndex(0);

    connect(ui->btnOK, &QPushButton::clicked, [=](){
        saveStackInfo1();//保存参数
        saveStackInfo(groupIndex);
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

//    connect(ui->btnMoveToStack, &QPushButton::clicked, moveStack, &StackFollow::exec);
//    connect(ui->btnMoveToFollow, &QPushButton::clicked, moveFollow, &StackFollow::exec);

    connect(ui->btnMoveToStack, &QPushButton::clicked, moveStack,[=](){
        MoveStackFollowPoint.Stack_Type = 1;//移至堆叠点
        emit StackFollowRefreshSignal();
        moveStack->exec();
    });
    connect(ui->btnMoveToFollow, &QPushButton::clicked, moveFollow,[=](){
        MoveStackFollowPoint.Stack_Type = 2;//移至跟随点
        emit StackFollowRefreshSignal();
        moveFollow->exec();
    });
    connect(this,&StackEdit::emit StackFollowRefreshSignal,moveStack,&StackFollow::StackFollowInit);
    connect(this,&StackEdit::emit StackFollowRefreshSignal,moveFollow,&StackFollow::StackFollowInit);
    logicSigsSlots();
//    saveInfoConnections();

    for (auto cobox : findChildren<QComboBox*>())
    {
//        cobox->setView(new QListView(cobox));
        cobox->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    }
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


/*****************此处为不同堆叠方式下两个界面的共用参数数据同步*******************************************/
    // 0. 轴选择  共用
    // 1. startPosA 起始  共用
    // 2. intervalDis 共用（只有z轴在 三点式和四点式的模式下才有)
    // 3. speed  共用
    // 4. pointNum 个数  共用
    // 5. offset 偏移
    // 6. stackDirect 方向 共用（只有z轴在 三点式和四点式的模式下才有)
    connect(axisSelect[0], QOverload<int>::of(&QComboBox::currentIndexChanged),
            ui->coboxAxisSelect0, &QComboBox::setCurrentIndex);
    connect(ui->coboxAxisSelect0, QOverload<int>::of(&QComboBox::currentIndexChanged),
            axisSelect[0], &QComboBox::setCurrentIndex);
    connect(axisSelect[1], QOverload<int>::of(&QComboBox::currentIndexChanged),
            ui->coboxAxisSelect1, &QComboBox::setCurrentIndex);
    connect(ui->coboxAxisSelect1, QOverload<int>::of(&QComboBox::currentIndexChanged),
            axisSelect[1], &QComboBox::setCurrentIndex);
    connect(axisSelect[2], QOverload<int>::of(&QComboBox::currentIndexChanged),
            ui->coboxAxisSelect2, &QComboBox::setCurrentIndex);
    connect(ui->coboxAxisSelect2, QOverload<int>::of(&QComboBox::currentIndexChanged),
            axisSelect[2], &QComboBox::setCurrentIndex);

    connect(startPosA[0], &NumberEdit::textChanged, ui->editStart0, &NumberEdit::setText);
    connect(ui->editStart0, &NumberEdit::textChanged, startPosA[0], &NumberEdit::setText);
    connect(startPosA[1], &NumberEdit::textChanged, ui->editStart1, &NumberEdit::setText);
    connect(ui->editStart1, &NumberEdit::textChanged, startPosA[1], &NumberEdit::setText);
    connect(startPosA[2], &NumberEdit::textChanged, ui->editStart2, &NumberEdit::setText);
    connect(ui->editStart2, &NumberEdit::textChanged, startPosA[2], &NumberEdit::setText);

    connect(speed[0], &NumberEdit::textChanged, ui->editStackSpeed0, &NumberEdit::setText);
    connect(ui->editStackSpeed0, &NumberEdit::textChanged, speed[0], &NumberEdit::setText);
    connect(speed[1], &NumberEdit::textChanged, ui->editStackSpeed1, &NumberEdit::setText);
    connect(ui->editStackSpeed1, &NumberEdit::textChanged, speed[1], &NumberEdit::setText);
    connect(speed[2], &NumberEdit::textChanged, ui->editStackSpeed2, &NumberEdit::setText);
    connect(ui->editStackSpeed2, &NumberEdit::textChanged, speed[2], &NumberEdit::setText);

    connect(pointNum[0], &NumberEdit::textChanged, ui->editStackNum0, &NumberEdit::setText);
    connect(ui->editStackNum0, &NumberEdit::textChanged, pointNum[0], &NumberEdit::setText);
    connect(pointNum[1], &NumberEdit::textChanged, ui->editStackNum1, &NumberEdit::setText);
    connect(ui->editStackNum1, &NumberEdit::textChanged, pointNum[1], &NumberEdit::setText);
    connect(pointNum[2], &NumberEdit::textChanged, ui->editStackNum2, &NumberEdit::setText);
    connect(ui->editStackNum2, &NumberEdit::textChanged, pointNum[2], &NumberEdit::setText);

    // intervalDis 共用（只有z轴在 三点式和四点式的模式下才有)
    connect(ui->editIntervalDis, &NumberEdit::textChanged, ui->editIntervalDis2, &NumberEdit::setText);
//    connect(ui->editStackNum2, &NumberEdit::textChanged, ui->editIntervalDis2, &NumberEdit::setText);

    connect(ui->coboxStackDirect, QOverload<int>::of(&QComboBox::currentIndexChanged),
            ui->coboxStackDirect2, &QComboBox::setCurrentIndex);
    connect(ui->coboxStackDirect2, QOverload<int>::of(&QComboBox::currentIndexChanged),
            ui->coboxStackDirect, &QComboBox::setCurrentIndex);


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
}

void StackEdit::saveInfoConnections()
{
    connect(ui->coboxStackOrder, QOverload<int>::of(&QComboBox::activated), this, [=](int ){
        saveStackInfo1();
    });
    connect(ui->coboxCountWay, QOverload<int>::of(&QComboBox::activated), this, [=](int ){
        saveStackInfo1();
    });
    connect(ui->coboxStopAfterStack, QOverload<int>::of(&QComboBox::activated), this, [=](int ){
        saveStackInfo1();
    });
    connect(ui->coboxLeaveBlank, QOverload<int>::of(&QComboBox::activated), this, [=](int ){
        saveStackInfo1();
    });
    connect(ui->checkBoxTriAxisUnion, &QCheckBox::clicked, this, &StackEdit::saveStackInfo1);
    connect(ui->coboxStackDirect, QOverload<int>::of(&QComboBox::activated), this, [=](int ){
        saveStackInfo1();
    });

    for (int i = 0; i < STACK_AXIS_NUM; i++)
    {
        connect(axisSelect[i], QOverload<int>::of(&QComboBox::activated), this, [=](int index){
            if (index != -1)
            {
                saveStackInfo1();
            }
        });
        connect(stackDirect[i], QOverload<int>::of(&QComboBox::activated), this, [=](int index){
            if (index != -1)
            {
                saveStackInfo1();
            }
        });

        connect(speed[i],&NumberEdit::textChanged,[=](const QString &){
            saveStackInfo1();
        });
        connect(pointNum[i],&NumberEdit::textChanged,[=](const QString &){
            saveStackInfo1();
        });
        connect(speedDischage[i],&NumberEdit::textChanged,[=](const QString &){
            saveStackInfo1();
        });
        connect(startPosA[i],&NumberEdit::textChanged,[=](const QString &){
            saveStackInfo(i);
        });
        connect(endPosB_X[i],&NumberEdit::textChanged,[=](const QString &){
            saveStackInfo(i);
        });
        connect(endPosC_Y[i],&NumberEdit::textChanged,[=](const QString &){
            saveStackInfo(i);
        });
        connect(posD[i],&NumberEdit::textChanged,[=](const QString &){
            saveStackInfo(i);
        });
        connect(startPosDischage[i],&NumberEdit::textChanged,[=](const QString &){
            saveStackInfo(i);
        });
        connect(intervalDis[i],&NumberEdit::textChanged,[=](const QString &){
            saveStackInfo(i);
        });
        connect(offset[i],&NumberEdit::textChanged,[=](const QString &){
            saveStackInfo(i);
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
        objPos[i] = new QTableWidgetItem("");
        table->setItem(i, 1, objPos[i]);
        connect(nums[i],&NumberEdit::finishedInput, this,[=](){
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
                }
            }
        });
    }
    StackFollowInit();//初始化参数
    moveButton = new QPushButton(tr("移动"), this);
    moveButton->setFixedHeight(45);
    moveButton->setFixedWidth(110);
    connect(moveButton,&QPushButton::clicked,this,[=](){//点击移动按钮时，下发移动至堆叠点数据
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
                g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_STACK);
            }
        }

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
void StackFollow::updateVerticalHeader(int index, const QString &text)
{
    QStringList headers = table->verticalHeader()->model()->headerData(index, Qt::Vertical).toStringList();
    headers[index] = text;
    table->setVerticalHeaderLabels(headers);
}
