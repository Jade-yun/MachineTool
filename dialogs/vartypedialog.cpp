#include "vartypedialog.h"
#include "ui_vartypedialog.h"

VarTypeDialog::VarTypeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VarTypeDialog)
{
    ui->setupUi(this);
    connect(ui->btnOK, &QPushButton::clicked, this, &VarTypeDialog::saveVariableType);
    connect(ui->btnCancel, &QPushButton::clicked, this, &VarTypeDialog::reject);

    for (int i = 0; i < VAR_TOTAL_NUM; i++)
    {
        QString objName = QString("coboxVarType%1").arg(i+1);
        coboxVarType[i] = qobject_cast<QComboBox*>(this->findChild<QComboBox*>(objName));
    }
//    coboxVarType = {ui->coboxVarType1, ui->coboxVarType2, ui->coboxVarType3, ui->coboxVarType4,
//                   ui->coboxVarType5, ui->coboxVarType6, ui->coboxVarType7, ui->coboxVarType8};

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
    accept();
}
