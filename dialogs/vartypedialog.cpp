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

}

VarTypeDialog::~VarTypeDialog()
{
    delete ui;
}

void VarTypeDialog::saveVariableType()
{

    for (int i = 0; i < VAR_TOTAL_NUM; i++)
    {
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
