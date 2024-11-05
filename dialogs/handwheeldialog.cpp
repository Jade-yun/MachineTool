#include "handwheeldialog.h"
#include "ui_handwheeldialog.h"
#include <QButtonGroup>

HandWheelDialog::HandWheelDialog(QWidget *parent) :
    BaseWindow(parent),
    ui(new Ui::HandWheelDialog)
{
    ui->setupUi(this);
    initCheckBoxGrop();
    initControls();
    setModal(true);
    connect(ui->btnOK, &QPushButton::clicked, [=](){
       for(int i=0;i<handWheelScale.size();i++)
       {
           if(handWheelScale[i]->isChecked())
           {
               m_manualAxis.multiply=i;
           }
       }
       for(int i=0;i<handWheelAxis.size();i++)
       {
           if(handWheelAxis[i]->isChecked())
           {
               m_manualAxis.handwheelAxis = i+1;
           }
       }
       m_manualAxis.handwheelMode=ui->chboxMode->isChecked();
       setManualAxis(m_manualAxis);
       this->close();
    });
}

HandWheelDialog::~HandWheelDialog()
{
    delete ui;
}

void HandWheelDialog::initControls()
{
    ui->chboxMode->setChecked(m_manualAxis.handwheelMode);
    handWheelScale[m_manualAxis.multiply]->setChecked(1);
    if(m_manualAxis.handwheelAxis == 0)
    {
        handWheelAxis[0]->setChecked(1);
    }
    else
    {
        handWheelAxis[m_manualAxis.handwheelAxis-1]->setChecked(1);
    }
}

void HandWheelDialog::initCheckBoxGrop()
{
    handWheelScale = {ui->chbox0_01,ui->chbox0_1,ui->chbox0_5,ui->chbox1,ui->chbox3};
    handWheelAxis = {ui->chboxX1,ui->chboxZ1,ui->chboxY1,ui->chboxC1,ui->chboxZ2,ui->chboxY2,ui->chboxB,ui->chboxA};
    QButtonGroup* pButtonGroup1 = new QButtonGroup(this);
    QButtonGroup* pButtonGroup2 = new QButtonGroup(this);
    for(int i=0;i<handWheelScale.size();i++)
    {
        pButtonGroup1->addButton(handWheelScale[i],i+1);
    }
    for(int i=0;i<handWheelAxis.size();i++)
    {
        pButtonGroup2->addButton(handWheelAxis[i],i+1);
    }
}


