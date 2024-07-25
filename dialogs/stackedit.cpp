#include "stackedit.h"
#include "ui_stackedit.h"
#include <QMessageBox>
#include <QCloseEvent>
#include <QDebug>

QPointer<StackEdit> stack[8] = {nullptr};

StackEdit::StackEdit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StackEdit)
{
    ui->setupUi(this);

    ui->tableStack->setCornerButtonEnabled(true);
    StackPara stackPara(ui->tableStack);
    for (int i = 0; i < 3; i++)
    {
        connect(stackPara.axisSelect[i], QOverload<int>::of(&QComboBox::currentIndexChanged), [=]() {
//            qDebug() << "axis " << i << ": " << stackPara.axisSelect[i]->currentText();
            ui->tableStack->horizontalHeaderItem(i)->setText(stackPara.axisSelect[i]->currentText());
        });
    }

    ui->btnOK->setVisible(false);
    ui->btnCancel->setVisible(false);
    connect(this, &StackEdit::moveButtonPos, [=](bool isVisible){
        ui->btnOK->setVisible(isVisible);
        ui->btnCancel->setVisible(isVisible);
    });
}

StackEdit::~StackEdit()
{
    delete ui;
}

void StackEdit::closeEvent(QCloseEvent *event)
{
//    if (event)
//    this->setParent(lastParent);
//    lastParent->show();
//    event->accept();//默认情况下接收关闭信号，关闭窗体
//    QMessageBox::StandardButton button = QMessageBox::information(this,"询问","确定关闭吗?",
//                                                                  QMessageBox::StandardButton::Yes,
//                                                                  QMessageBox::StandardButton::No);
//    if(QMessageBox::StandardButton::Yes == button)
//    {
//        event->accept();//默认情况下接收关闭信号，关闭窗体
//    }
//    else
//    {
//        event->ignore();//忽略关闭信号，阻止窗体关闭
//    }
}

