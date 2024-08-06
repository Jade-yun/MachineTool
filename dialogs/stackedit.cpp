#include "stackedit.h"
#include "ui_stackedit.h"
#include <QMessageBox>
#include <QCloseEvent>
#include <QDebug>

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
}

void StackEdit::initNormalStackUI(QWidget *normalStack)
{

    //    normalStack->show();
}

void StackEdit::switchStackWay(int mode)
{
    if (mode == 0 || mode == 1)
    {
        ui->stkWgtStackWay->setCurrentIndex(0);
        ui->grboxAxisZPara->show();
        ui->btnFresh->show();
        ui->btnMoveToStack->show();
        ui->btnMoveToFollow->show();
    }
    else if (mode == 2)
    {
        ui->stkWgtStackWay->setCurrentIndex(1);
        ui->grboxAxisZPara->hide();
        ui->btnFresh->hide();
        ui->btnMoveToStack->hide();
        ui->btnMoveToFollow->hide();
    }
}

void StackEdit::showOKandCancelButton(bool isVisible)
{
    ui->btnOK->setVisible(isVisible);
    ui->btnCancel->setVisible(isVisible);
}

StackEdit::StackEdit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StackEdit),
    movie(nullptr)
{
    ui->setupUi(this);

    ui->tableStack->setCornerButtonEnabled(true);
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

    connect(ui->btnOK, &QPushButton::clicked, [=](){
       QDialog* dialog = qobject_cast<QDialog*>(this->parent());
       if (dialog)
       {
//           qDebug() << "close parent dialog";
           dialog->close();
       }
//       else
//            qDebug() << "parent widget is not a dialog";
    });
    connect(ui->btnCancel, &QPushButton::clicked, [=](){
        QDialog* dialog = qobject_cast<QDialog*>(this->parent());
        if (dialog)
        {
            dialog->close();
        }
    });


    moveStack = new StackFollow(this);
    moveFollow = new StackFollow(this);

//    connect(moveStack->moveButton, &QPushButton::clicked, this, [=](){
//        qDebug() << "moveStack->moveButton clicked" ;
//    });

    connect(ui->btnMoveToStack, &QPushButton::clicked, moveStack, &StackFollow::exec);
    connect(ui->btnMoveToFollow, &QPushButton::clicked, moveFollow, &StackFollow::exec);

    logicSigsSlots();

}

StackEdit::~StackEdit()
{
    delete ui;
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
    movie->setSpeed(50);
    ui->labGif->setMovie(movie);
    movie->start();
}

void StackEdit::logicSigsSlots()
{
/*****************此处连接不同堆叠方式下两个界面的共用参数数据同步信号和槽的连接*******************************************/
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
    connect(ui->editIntervalDis, &NumberEdit::textChanged, ui->editIntervalDis, &NumberEdit::setText);
    connect(ui->editStackNum2, &NumberEdit::textChanged, ui->editIntervalDis2, &NumberEdit::setText);

    connect(ui->coboxStackDirect, QOverload<int>::of(&QComboBox::currentIndexChanged),
            ui->coboxStackDirect0, &QComboBox::setCurrentIndex);
    connect(ui->coboxStackDirect0, QOverload<int>::of(&QComboBox::currentIndexChanged),
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

void StackEdit::on_coboxStackOrder_currentIndexChanged(int index)
{
    switch (index) {
    case 0:

    loadAndPlayGif(":/images/gif/test.gif");
        break;
    case 1:

    loadAndPlayGif(":/images/gif/test1.gif");
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
//        num->setInputRange();

        table->setCellWidget(i, 0, nums[i]);

        objPos[i] = new QTableWidgetItem("");
        table->setItem(i, 1, objPos[i]);
    }

    moveButton = new QPushButton(tr("移动"), this);
    moveButton->setFixedHeight(40);
    moveButton->setFixedWidth(110);

    closeButton = new QPushButton(tr("关闭"), this);
    closeButton->setFixedHeight(40);
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

void StackFollow::updateVerticalHeader(int index, const QString &text)
{
    QStringList headers = table->verticalHeader()->model()->headerData(index, Qt::Vertical).toStringList();
    headers[index] = text;
    table->setVerticalHeaderLabels(headers);
}
