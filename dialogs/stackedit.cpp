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
                axisSelect[i]->addItems({ QObject::tr("X1轴"), QObject::tr("X2轴") });
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

        speed[i] = new QLineEdit(this);
        pointNum[i] = new QLineEdit(this);
        startPosA[i] = new QLineEdit(this);
        endPosB_X[i] = new QLineEdit(this);
        endPosC_Y[i] = new QLineEdit(this);
        posD[i] = new QLineEdit(this);
        speedDischage[i] = new QLineEdit(this);
        startPosDischage[i] = new QLineEdit(this);

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
