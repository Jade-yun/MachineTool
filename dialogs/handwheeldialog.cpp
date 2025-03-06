#include "handwheeldialog.h"
#include "ui_handwheeldialog.h"
#include <QButtonGroup>
#include <QDebug>

#include "cmd.h"
#include "iniconfig.h"
#include "QtUi/framelesswidget2.h"

HandWheelDialog::HandWheelDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HandWheelDialog)
{
    ui->setupUi(this);
    setModal(true);

    FramelessWidget2* frameBase = new FramelessWidget2(this);

    frameBase->setWidget(this);
    frameBase->setTitle(tr("手轮"));

    initWidgets();
    connect(ui->btnOK, &QPushButton::clicked, [=](){
//       for(int i = 0; i < handWheelScale.size(); i++)
//       {
//           if(handWheelScale[i]->isChecked())
//           {
//               m_manualAxis.multiply = i;
//           }
//       }
//       for(int i=0; i < handWheelAxis.size(); i++)
//       {
//           if(handWheelAxis[i]->isChecked())
//           {
//               m_manualAxis.handwheelAxis = i + 1;
//           }
//       }

//       m_manualAxis.handwheelMode = ui->chboxHandWhellMode->isChecked();
//       setManualAxis(m_manualAxis);
       this->close();
    });
    for(int i = 0; i < handWheelScale.size(); i++)
    {
        connect(handWheelScale[i],&QCheckBox::clicked,this,[=](){
            if(handWheelScale[i]->isChecked())
            {
                m_manualAxis.multiply = i;
                setManualAxis(m_manualAxis);
            }
        });
    }
    for(int i = 0;i < handWheelAxis.size(); i++)
    {
        connect(handWheelAxis[i],&QCheckBox::clicked,this,[=](){
           if(handWheelAxis[i]->isChecked())
           {
               m_manualAxis.handwheelAxis = i+1;
               setManualAxis(m_manualAxis);
           }
        });
    }
    connect(ui->chboxHandWhellMode,&QCheckBox::clicked,this,[=](bool state){
        m_manualAxis.handwheelMode = state;
        setManualAxis(m_manualAxis);
        emit HandWheelModeChange_Signal(state);
    });
    RefreshHandWheel_handle();
}

HandWheelDialog::~HandWheelDialog()
{
    delete ui;
}

void HandWheelDialog::initWidgets()
{
    handWheelScale = {ui->chbox0_01, ui->chbox0_1, ui->chbox1, ui->chbox3};
    handWheelAxis = {ui->chboxX1, ui->chboxY1, ui->chboxZ1, ui->chboxC, ui->chboxA, ui->chboxB};

    QButtonGroup* pButtonGroup1 = new QButtonGroup(this);
    QButtonGroup* pButtonGroup2 = new QButtonGroup(this);
    for(int i=0; i<handWheelScale.size(); i++)
    {
        pButtonGroup1->addButton(handWheelScale[i], i + 1);
    }
    for(int i=0; i<handWheelAxis.size(); i++)
    {
        pButtonGroup2->addButton(handWheelAxis[i], i + 1);
    }
}

void HandWheelDialog::showEvent(QShowEvent *event)
{
    ui->chboxHandWhellMode->setChecked(m_manualAxis.handwheelMode);

    if (m_manualAxis.multiply < handWheelAxis.size())
    {
        handWheelScale[m_manualAxis.multiply]->setChecked(true);
    }
    else
    {
        qDebug() << "Error: m_manualAxis.multiply is out of bounds";
    }
    if(m_manualAxis.handwheelAxis == 0)
    {
        for (auto chbox : handWheelAxis)
        {
            chbox->setChecked(false);
        }
    }
    else
    {
        if (m_manualAxis.handwheelAxis < handWheelAxis.size() + 1)
        {
            handWheelAxis[m_manualAxis.handwheelAxis - 1]->setChecked(true);
        }
        else
        {
            qDebug() << "Error: m_manualAxis.handwheelAxis is out of bounds";
        }
    }

    QWidget::showEvent(event);
}

void HandWheelDialog::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
    }
    QWidget::changeEvent(e);
}

void HandWheelDialog::HandWheelButtonClickHandel()
{
    ui->chboxHandWhellMode->setChecked(m_manualAxis.handwheelMode);
    emit HandWheelModeChange_Signal(m_manualAxis.handwheelMode);
}

//刷新界面参数处理函数
void HandWheelDialog::RefreshHandWheel_handle()
{
    ui->chboxHandWhellMode->setChecked(m_manualAxis.handwheelMode);
    for(int i = 0; i < handWheelScale.size(); i++)
    {
        if(m_manualAxis.multiply == i)
        {
            handWheelScale[i]->setChecked(true);
        }
    }
    for(int i=0;i<AXIS_TOTAL_NUM;i++)
    {
        if(m_AxisPar[i].axisType == 1)
        {//如果轴是伺服轴，显示选择框，否则隐藏
            handWheelAxis[i]->setEnabled(true);
            handWheelAxis[i]->setText(m_NameDefine[1].axisName[i]);
        }
        else
        {
            handWheelAxis[i]->setEnabled(false);
            handWheelAxis[i]->setText(m_NameDefine[1].axisName[i]);
        }
    }
    if(m_manualAxis.handwheelAxis>0)
    {
        if(handWheelAxis[m_manualAxis.handwheelAxis-1]->isEnabled() == true)
        {
            handWheelAxis[m_manualAxis.handwheelAxis-1]->setChecked(true);
        }
    }

}
