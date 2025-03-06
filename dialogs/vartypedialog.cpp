#include "vartypedialog.h"
#include "ui_vartypedialog.h"

#include <QListView>

VarTypeDialog::VarTypeDialog(QWidget *parent) :
    BaseWindow(parent),
    ui(new Ui::VarTypeDialog)
{
    ui->setupUi(this);
    this->setModal(true);
    connect(ui->btnOK, &QPushButton::clicked, this, &VarTypeDialog::saveVariableType);
    connect(ui->btnCancel, &QPushButton::clicked, this, &VarTypeDialog::reject);

//    for (int i = 0; i < VAR_TOTAL_NUM; i++)
//    {
//        QString objName = QString("coboxVarType%1").arg(i+1);
//        coboxVarType[i] = qobject_cast<QComboBox*>(this->findChild<QComboBox*>(objName));
//    }

#if VAR_TOTAL_NUM == 8
    coboxVarType = {ui->coboxVarType1, ui->coboxVarType2, ui->coboxVarType3, ui->coboxVarType4,
                    ui->coboxVarType5, ui->coboxVarType6, ui->coboxVarType7, ui->coboxVarType8};
#elif VAR_TOTAL_NUM == 16
    coboxVarType = {ui->coboxVarType1, ui->coboxVarType2, ui->coboxVarType3, ui->coboxVarType4,
                    ui->coboxVarType5, ui->coboxVarType6, ui->coboxVarType7, ui->coboxVarType8,
                    ui->coboxVarType9, ui->coboxVarType10, ui->coboxVarType11, ui->coboxVarType12,
                    ui->coboxVarType13, ui->coboxVarType14, ui->coboxVarType15, ui->coboxVarType16};

#endif

#if VAR_TOTAL_NUM != 16
    // 隐藏变量9-16，后续可能使用
    for (int i = 0; i < ui->gridLayout2->count(); ++i)
    {
        QWidget* widget = ui->gridLayout2->itemAt(i)->widget();
        if (widget)
        {
            widget->setVisible(false);
        }
    }
#endif

    for (int i = 0; i < VAR_TOTAL_NUM; i++)
    {
        coboxVarType[i]->clear();
        QStringList items= {tr("整数"), tr("一位小数"), tr("两位小数")};
        coboxVarType[i]->addItems(items);
        coboxVarType[i]->setView(new QListView(coboxVarType[i]));
        coboxVarType[i]->setFocusPolicy(Qt::NoFocus);
    }
    setVariabText();
}

VarTypeDialog::~VarTypeDialog()
{
    delete ui;
}

void VarTypeDialog::saveVariableType()
{

    for (int i = 0; i < VAR_TOTAL_NUM; i++)
    {
        m_VariableTypeLod[i] = m_VariableType[i];
        m_VariableType[i] = coboxVarType[i]->currentIndex();
    }
    this->accept();
}

void VarTypeDialog::Refresh_VarTypeDialog()
{
    for(uint8_t i=0;i<VAR_TOTAL_NUM;i++)
    {
        coboxVarType[i]->setCurrentIndex(m_VariableType[i]);
    }
}
//设置变量名称显示
void VarTypeDialog::setVariabText()
{
    #if VAR_TOTAL_NUM == 8
    ui->label_var1->setText(m_NameDefine[1].varName[0]);
    ui->label_var2->setText(m_NameDefine[1].varName[1]);
    ui->label_var3->setText(m_NameDefine[1].varName[2]);
    ui->label_var4->setText(m_NameDefine[1].varName[3]);
    ui->label_var5->setText(m_NameDefine[1].varName[4]);
    ui->label_var6->setText(m_NameDefine[1].varName[5]);
    ui->label_var7->setText(m_NameDefine[1].varName[6]);
    ui->label_var8->setText(m_NameDefine[1].varName[7]);
    #elif VAR_TOTAL_NUM == 16
    ui->label_var1->setText(m_NameDefine[1].varName[0]);
    ui->label_var2->setText(m_NameDefine[1].varName[1]);
    ui->label_var3->setText(m_NameDefine[1].varName[2]);
    ui->label_var4->setText(m_NameDefine[1].varName[3]);
    ui->label_var5->setText(m_NameDefine[1].varName[4]);
    ui->label_var6->setText(m_NameDefine[1].varName[5]);
    ui->label_var7->setText(m_NameDefine[1].varName[6]);
    ui->label_var8->setText(m_NameDefine[1].varName[7]);
    ui->label_var9->setText(m_NameDefine[1].varName[8]);
    ui->label_var10->setText(m_NameDefine[1].varName[9]);
    ui->label_var11->setText(m_NameDefine[1].varName[10]);
    ui->label_var12->setText(m_NameDefine[1].varName[11]);
    ui->label_var13->setText(m_NameDefine[1].varName[12]);
    ui->label_var14->setText(m_NameDefine[1].varName[13]);
    ui->label_var15->setText(m_NameDefine[1].varName[14]);
    ui->label_var16->setText(m_NameDefine[1].varName[15]);
#endif
}

void VarTypeDialog::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange)
    {
        this->retranslate();
    }
    QWidget::changeEvent(e);
}

void VarTypeDialog::retranslate()
{
    QStringList items= {tr("整数"), tr("一位小数"), tr("两位小数")};
    for (int i = 0; i < VAR_TOTAL_NUM; i++)
    {
        for (int j = 0; j < items.size(); j++)
        {
            coboxVarType[i]->setItemText(j, items[j]);
        }

    }
}
