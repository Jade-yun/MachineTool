#include "cleardialog.h"
#include "ui_cleardialog.h"

ClearDialog::ClearDialog(QWidget *parent) :
    BaseWindow(parent),
    ui(new Ui::ClearDialog)
{
    ui->setupUi(this);
    setModal(true);

//    FramelessWidget2* frameBase = new FramelessWidget2(this);
//    frameBase->setWidget(ui->titleBar);

    ui->stkWidget->setCurrentIndex(0);
    connect(ui->btnOK, &QPushButton::clicked, this, &QDialog::accept);
    ui->EditSilo_Number->setDecimalPlaces(0);
    ui->EditStackGroup1->setDecimalPlaces(0);
    ui->EditStackGroup2->setDecimalPlaces(0);
    connect(ui->chboxClearProd,&QCheckBox::stateChanged,[](int state){
        if(state){
            m_AutoPageParReset.Product_Clear = 1;
        }else{
            m_AutoPageParReset.Product_Clear = 0;
        }
    });
    connect(ui->chboxClearAllVar,&QCheckBox::stateChanged,[](int state){
        if(state){
            m_AutoPageParReset.Var_All_Clear = 1;
        }else{
            m_AutoPageParReset.Var_All_Clear = 0;
        }
    });
    connect(ui->chboxClearAllStack,&QCheckBox::stateChanged,[](int state){
        if(state){
            m_AutoPageParReset.Stack_All_Clear = 1;
        }else{
            m_AutoPageParReset.Stack_All_Clear = 0;
        }
    });
    StackClearGroup = {ui->chboxStack1,ui->chboxStack2,ui->chboxStack3,ui->chboxStack4,ui->chboxStack5,ui->chboxStack6,ui->chboxStack7,ui->chboxStack8};
    for(int i=0;i<StackClearGroup.count();i++)
    {
        connect(StackClearGroup[i],QOverload<int>::of(&QCheckBox::stateChanged), [=](int state){
            if(state){
                m_AutoPageParReset.Stack_Group_Clear[i] = 1;
            }else{
                m_AutoPageParReset.Stack_Group_Clear[i] = 0;
            }
        });
    }
    connect(ui->EditSilo_Number,&NumberEdit::textChanged,[=](const QString text){
        m_AutoPageRotat_Silo_Set.Silo_Number = (uint16_t)text.toUInt();
    });
    connect(ui->EditStackGroup1,&NumberEdit::textChanged,[=](const QString text){
        m_AutoPageRotat_Silo_Set.Stack_Group1 = (uint16_t)text.toUInt();

    });
    connect(ui->EditStackGroup2,&NumberEdit::textChanged,[=](const QString text){
        m_AutoPageRotat_Silo_Set.Stack_Group2 = (uint16_t)text.toUInt();
    });
    connect(ui->chboxSilo_Num,&QCheckBox::stateChanged,[this](int state){
        if(state){
            m_AutoPageRotat_Silo_Set.type = 0;
            ui->EditSilo_Number->setEnabled(true);
            ui->EditStackGroup1->setEnabled(true);
            ui->EditStackGroup2->setEnabled(true);
        }
    });
    connect(ui->chboxSiloInitPos,&QCheckBox::stateChanged,[this](int state){
        if(state){
            m_AutoPageRotat_Silo_Set.type = 1;
            ui->EditSilo_Number->setEnabled(false);
            ui->EditStackGroup1->setEnabled(false);
            ui->EditStackGroup2->setEnabled(false);
        }
    });
    connect(ui->chboxSilo_Stay,&QCheckBox::stateChanged,[this](int state){
        if(state){
            m_AutoPageRotat_Silo_Set.type = 2;
            ui->EditSilo_Number->setEnabled(false);
            ui->EditStackGroup1->setEnabled(false);
            ui->EditStackGroup2->setEnabled(false);
        }
    });

    //变量界面
    VarAutoClear = {ui->chboxVar1,ui->chboxVar2,ui->chboxVar3,ui->chboxVar4,ui->chboxVar5,ui->chboxVar6,ui->chboxVar7,ui->chboxVar8,
                    ui->chboxVar9,ui->chboxVar10,ui->chboxVar11,ui->chboxVar12,ui->chboxVar13,ui->chboxVar14,ui->chboxVar15,ui->chboxVar16};
    VarPowerOffMemory = {ui->chboxVar1_2,ui->chboxVar2_2,ui->chboxVar3_2,ui->chboxVar4_2,ui->chboxVar5_2,ui->chboxVar6_2,ui->chboxVar7_2,ui->chboxVar8_2,
                        ui->chboxVar9_2,ui->chboxVar10_2,ui->chboxVar11_2,ui->chboxVar12_2,ui->chboxVar13_2,ui->chboxVar14_2,ui->chboxVar15_2,ui->chboxVar16_2};
    for(int i=0;i<VAR_TOTAL_NUM;i++)
    {
        connect(VarAutoClear[i],QOverload<int>::of(&QCheckBox::stateChanged),[=](int state){
            if(state){
                m_AutoPageVar.StartReset[i] = 1;
            }else{
                m_AutoPageVar.StartReset[i] = 0;
            }
        });
        connect(VarPowerOffMemory[i],QOverload<int>::of(&QCheckBox::stateChanged),[=](int state){
            if(state){
                m_AutoPageVar.PowerOffMemory[i] = 1;
            }else{
                m_AutoPageVar.PowerOffMemory[i] = 0;
            }
        });
    }
}

ClearDialog::ClearDialog(ClearDialog::ClearMode mode, QWidget *parent) :
    BaseWindow(parent),
    ui(new Ui::ClearDialog)
{
    ui->setupUi(this);
    setModal(true);

//    FramelessWidget2* frameBase = new FramelessWidget2(this);
//    frameBase->setWidget(this);

    ui->stkWidget->setCurrentIndex(mode);
    connect(ui->btnOK, &QPushButton::clicked, this, &QDialog::accept);
}

ClearDialog::~ClearDialog()
{
    delete ui;
}

void ClearDialog::setMode(ClearDialog::ClearMode mode)
{
    ui->stkWidget->setCurrentIndex(mode);
    UpdateShow(mode);
}

void ClearDialog::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
    }
    QWidget::changeEvent(e);
}
//更新界面显示
void ClearDialog::UpdateShow(ClearDialog::ClearMode mode)
{
    if(mode == ClearMode::AutoState)
    {//清零按钮弹窗
        ui->chboxClearProd->setChecked(false);
        ui->chboxClearAllVar->setChecked(false);
        ui->chboxClearAllStack->setChecked(false);
        for(int i=0;i<StackClearGroup.count();i++)
        {
            StackClearGroup[i]->setChecked(false);
        }
        ui->EditSilo_Number->setText(QString::number(m_AutoPageRotat_Silo_Set.Silo_Number));
        ui->EditStackGroup1->setText(QString::number(m_AutoPageRotat_Silo_Set.Stack_Group1));
        ui->EditStackGroup2->setText(QString::number(m_AutoPageRotat_Silo_Set.Stack_Group2));

        if(m_AutoPageRotat_Silo_Set.type == 0)
        {
            ui->chboxSilo_Num->setChecked(true);
            ui->EditSilo_Number->setEnabled(true);
            ui->EditStackGroup1->setEnabled(true);
            ui->EditStackGroup2->setEnabled(true);
        }
        else if(m_AutoPageRotat_Silo_Set.type == 1)
        {
            ui->chboxSiloInitPos->setChecked(true);
            ui->EditSilo_Number->setEnabled(false);
            ui->EditStackGroup1->setEnabled(false);
            ui->EditStackGroup2->setEnabled(false);
        }
        else if(m_AutoPageRotat_Silo_Set.type == 2)
        {
            ui->chboxSilo_Stay->setChecked(true);
            ui->EditSilo_Number->setEnabled(false);
            ui->EditStackGroup1->setEnabled(false);
            ui->EditStackGroup2->setEnabled(false);
        }
        if(m_StackFunc.siloType == 1)
        {
            ui->Silo_GroupBox->show();
        }
        else
        {
            ui->Silo_GroupBox->hide();
        }
    }
    else if(mode == ClearMode::VarState)
    {//变量弹窗
        for(int i=0;i<VAR_TOTAL_NUM;i++)
        {
            VarAutoClear[i]->setChecked(m_AutoPageVar.StartReset[i]);
            VarPowerOffMemory[i]->setChecked(m_AutoPageVar.PowerOffMemory[i]);
        }
    }
}
