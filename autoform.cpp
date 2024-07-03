#include "autoform.h"
#include "ui_autoform.h"
#include <QMenu>
#include <QAction>
#include <QDebug>

AutoForm::AutoForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AutoForm)
{
    ui->setupUi(this);

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

    // 连接菜单项的点击事件
    connect(action1, &QAction::triggered, this, []() {
        qDebug() << "堆叠设定 ";
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


    // set btn slot
    connect(ui->btnSpeedAdd, &QPushButton::clicked, [=](){
        int value = ui->progressBarGlobalSpeed->value();
        if (value < 100 && value > 0)
            ui->progressBarGlobalSpeed->setValue(++value);
    });
    connect(ui->btnSpeedSub, &QPushButton::clicked, [=](){
        int value = ui->progressBarGlobalSpeed->value();
        if (value < 100 && value > 0)
            ui->progressBarGlobalSpeed->setValue(--value);
    });

}

AutoForm::~AutoForm()
{
    delete ui;
}

void AutoForm::on_btnNext_clicked()
{
    int index = (ui->stkWidgetAuto->currentIndex() + 1 + ui->stkWidgetAuto->count()) % ui->stkWidgetAuto->count();
    ui->stkWidgetAuto->setCurrentIndex(index);
    updateLabelState(index);
}

void AutoForm::on_btnLast_clicked()
{
    int index = (ui->stkWidgetAuto->currentIndex() - 1 + ui->stkWidgetAuto->count()) % ui->stkWidgetAuto->count();
    ui->stkWidgetAuto->setCurrentIndex(index);
    updateLabelState(index);
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

