#include "setting.h"
#include "ui_setting.h"

#include <QMovie>
#include <QtDebug>

#define TEST 1
//#define TESTKEYBOARD 1

Setting::Setting(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Setting)
{
    initVariables();
    ui->setupUi(this);

    setAllStyleSheet();

//    ui->checkBoxPoiskie->setDisabled(true);
//    ui->checkBoxRussian->setDisabled(true);
//    ui->checkBoxTradChinese->setDisabled(true);
//    ui->checkBoxSpain->setDisabled(true);
//    ui->checkBoxSpecial->setDisabled(true);
//    ui->checkBoxVietnam->setDisabled(true);

    pageSwitchInit();

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
    delete movie;
    movie = nullptr;
}

void Setting::slotSettingHome()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->stackedWidget->setCurrentWidget(ui->pageSettingHome);
    //    qDebug() << "slotSettingHome";
}

void Setting::loadAndPlayGif(const QString& path)
{
    if (movie)
    {
        movie->stop();
    }
    //    movie = new QMovie(path, this);
    //    ui->gifLabel->resize(width, height);
    movie->setFileName(path);
    movie->setSpeed(50);
    ui->gifLabel->setMovie(movie);
    movie->start();
}


// to initial all variables in this function
void Setting::initVariables()
{
    movie = new QMovie(QString(":/images/gif/test.gif"));
}



void Setting::on_comboBox_96_currentIndexChanged(int index)
{
    if (index == 0)
        this->loadAndPlayGif(":/images/gif/test.gif");
    if (index == 1)
        this->loadAndPlayGif(":/images/gif/test1.gif");
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
//    if (index == 0)
//    {
//        ui->stkWidgetIPSet->setCurrentIndex(0);
//    }
//    else if (index == 1)
//    {
//        ui->stkWidgetIPSet->setCurrentIndex(1);
//    }
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



