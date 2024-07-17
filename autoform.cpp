#include "autoform.h"
#include "ui_autoform.h"
#include <QMenu>
#include <QAction>
#include <QDebug>
#include <QTableWidget>

AutoForm::AutoForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AutoForm)
{
    ui->setupUi(this);

    // init some members;

    stackSet = new StackSetDialog(this);

    keyboard = new NumberKeyboardWindow(this);


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



//    QString buttonName = QString("第%1个按钮").arg(buttonCount + 1);
//    QPushButton *newButton = new QPushButton(buttonName, this);
    QComboBox* coboxStkGroupNum = new QComboBox(this);
    QLineEdit* editX1Num = new QLineEdit(this);
    QLineEdit* editY1Num = new QLineEdit(this);
    QLineEdit* editZ1Num = new QLineEdit(this);
    QComboBox* coboxStkFinishStop = new QComboBox(this);

    table->setCellWidget(0, 0, coboxStkGroupNum);
    table->setCellWidget(0, 1, editX1Num);
    table->setCellWidget(0, 2, editY1Num);
    table->setCellWidget(0, 3, editZ1Num);
    table->setCellWidget(0, 4, coboxStkFinishStop);

    // 创建菜单并添加菜单项
    QMenu* menu = new QMenu(this);

    QAction* action1 = new QAction(tr("堆叠设定"), this);
    QAction* action2 = new QAction(tr("从此运行"), this);
    QAction* action3 = new QAction(tr("断点设置"), this);
    QAction* action4 = new QAction(tr("参考点编辑"), this);
    QAction* action5 = new QAction(tr("自动修正位置"), this);

    menu->addAction(action1);
    menu->addAction(action2);
    menu->addAction(action3);
    menu->addAction(action4);
    menu->addAction(action5);

    ui->btnOperate->setMenu(menu);

/**********************************************************************/
    // 连接菜单项的点击事件
    connect(action1, &QAction::triggered, this, [=]() {
//        qDebug() << "堆叠设定 ";
        stackSet->show();

    });

    connect(action2, &QAction::triggered, this, []() {
        qDebug() << "从此运行 ";
    });

    connect(action3, &QAction::triggered, this, []() {
        qDebug() << "断点设置 ";
    });

    connect(action4, &QAction::triggered, this, []() {
        qDebug() << "参考点编辑 ";
    });

    connect(action5, &QAction::triggered, this, []() {
        qDebug() << "自动修正位置 ";
    });

    connect(ui->btnOperate, &QPushButton::clicked, [=]() {
        QPoint pos = ui->btnOperate->mapToGlobal(QPoint(0, 0));
        menu->exec(QPoint(pos.x(), pos.y() - menu->sizeHint().height()));
    });

/**********************************************************************/
    // these slots are used to adjust global speed
    connect(ui->btnSpeedAdd, &QPushButton::clicked, [=](){
        int value = ui->proBarGlobalSpeed->value();
        if (value < 100 && value > 0 && ui->btnAdjustSpeed->isChecked())
            ui->proBarGlobalSpeed->setValue(++value);
    });
    connect(ui->btnSpeedSub, &QPushButton::clicked, [=](){
        int value = ui->proBarGlobalSpeed->value();
        if (value < 100 && value > 0 && ui->btnAdjustSpeed->isChecked())
            ui->proBarGlobalSpeed->setValue(--value);
    });
    connect(ui->btn10percent, &QPushButton::clicked, [=](){
        if (ui->btnAdjustSpeed->isChecked())
            ui->proBarGlobalSpeed->setValue(10);
    });
    connect(ui->btn20percent, &QPushButton::clicked, [=](){
        if (ui->btnAdjustSpeed->isChecked())
            ui->proBarGlobalSpeed->setValue(20);
    });
//    connect(ui->btn20percent, &QPushButton::toggled, [=](bool checked){
//        if (ui->btnAdjustSpeed->isChecked())
//            ui->proBarGlobalSpeed->setValue(20);
//    });
    connect(ui->btn40percent, &QPushButton::clicked, [=](){
        if (ui->btnAdjustSpeed->isChecked())
            ui->proBarGlobalSpeed->setValue(40);
    });
    connect(ui->btn60percent, &QPushButton::clicked, [=](){
        if (ui->btnAdjustSpeed->isChecked())
            ui->proBarGlobalSpeed->setValue(60);
    });
    connect(ui->btn80percent, &QPushButton::clicked, [=](){
        if (ui->btnAdjustSpeed->isChecked())
            ui->proBarGlobalSpeed->setValue(80);
    });
    connect(ui->btn100percent, &QPushButton::clicked, [=](){
        if (ui->btnAdjustSpeed->isChecked())
            ui->proBarGlobalSpeed->setValue(100);
    });
/**********************************************************************/

    connect(ui->btnMergeExpand, &QPushButton::toggled, this, [=](bool checked){
        if (checked)
        {
            ui->frameTable->show();
            ui->frameGlobalSpeed->show();
    //        qDebug() << " show ...";
        }
        else
        {
            ui->frameTable->show();
            ui->frameGlobalSpeed->close();
        }
    });

    connect(ui->btnNext, &QPushButton::clicked, this, [=](){
    int index = (ui->stkWidgetAuto->currentIndex() + 1 + ui->stkWidgetAuto->count()) % ui->stkWidgetAuto->count();
    ui->stkWidgetAuto->setCurrentIndex(index);
    updateLabelState(index);
    });

    connect(ui->btnLast, &QPushButton::clicked, this, [=](){
        int index = (ui->stkWidgetAuto->currentIndex() - 1 + ui->stkWidgetAuto->count()) % ui->stkWidgetAuto->count();
        ui->stkWidgetAuto->setCurrentIndex(index);
        updateLabelState(index);

    });

}

AutoForm::~AutoForm()
{
    delete ui;
}



void AutoForm::updateLabelState(int index)
{
    const QList<QString> states = {
        tr("1自动状态"),
        tr("2变量状态"),
        tr("3堆叠状态"),
        tr("4当前步号")
    };

    if (index >= 0 && index < states.size())
    {
        ui->labelState->setText(states.at(index));
    }
}

void AutoForm::callNumKeyBoard(QObject* obj)
{
    if (obj)
    {
        keyboard->clearText();
        keyboard->setCurrentEditObj(obj);
        keyboard->show();
        keyboard->raise();
        keyboard->activateWindow();
    }

}


void AutoForm::on_btnEdit_pressed()
{
    if (ui->btnEdit->isChecked())
    {

//        if (isEdit)
            ui->stkWgtOuter->setCurrentIndex(1);
    }
    else
    {
        ui->stkWgtOuter->setCurrentIndex(0);
    }

}


void AutoForm::on_btnAimedProduct_clicked()
{
    // call keyboard for labAimedProd
    callNumKeyBoard(qobject_cast<QLabel*>(ui->labAimedProd));

}
