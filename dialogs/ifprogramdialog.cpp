#include "ifprogramdialog.h"
#include "ui_ifprogramdialog.h"

IfProgramDialog::IfProgramDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::IfProgramDialog)
{
    ui->setupUi(this);
    chboxsSigMb = new QButtonGroup(this);
    for (auto chbox : ui->tabMainBoard->findChildren<QCheckBox*>())
    {
        chboxsSigMb->addButton(chbox);
    }
    chboxsSigMb->setExclusive(true);

    QButtonGroup* chboxsVarPreOp = new QButtonGroup(this);
    chboxsVarPreOp->addButton(ui->chbox1Var);
    chboxsVarPreOp->addButton(ui->chbox2Var);
    chboxsVarPreOp->addButton(ui->chbox3Var);
    chboxsVarPreOp->addButton(ui->chbox4Var);
    chboxsVarPreOp->setExclusive(true);

    QButtonGroup* chboxsVarPostOp1 = new QButtonGroup(this);
    chboxsVarPostOp1->addButton(ui->chboxConstantVar);
    chboxsVarPostOp1->addButton(ui->chboxVariableVar);
    chboxsVarPostOp1->addButton(ui->chboxRealProd);
    chboxsVarPostOp1->setExclusive(true);

    QButtonGroup* chboxsVarPostOp2 = new QButtonGroup(this);
    chboxsVarPostOp2->addButton(ui->chboxConstantRealProd);
    chboxsVarPostOp2->addButton(ui->chboxVariableRealProd);
    chboxsVarPostOp2->setExclusive(true);

    QButtonGroup* chboxsVarPostOp3 = new QButtonGroup(this);
    chboxsVarPostOp3->addButton(ui->chboxConstantStack);
    chboxsVarPostOp3->addButton(ui->chboxStack);
    chboxsVarPostOp3->setExclusive(true);

    connect(ui->btnOK, &QPushButton::clicked, this, &IfProgramDialog::accept);
    connect(ui->btnCancel, &QPushButton::clicked, this, &IfProgramDialog::reject);

    ui->chboxConstantVar->clicked(true);
    ui->chboxConstantRealProd->clicked(true);
    ui->chboxConstantStack->clicked(true);

    ui->chboxConstantPos->clicked(true);
    ui->chboxConstantTimer->clicked(true);

    ui->chboxConstantVar->setChecked(true);
    ui->chboxConstantRealProd->setChecked(true);
    ui->chboxConstantStack->setChecked(true);

    ui->chboxConstantPos->setChecked(true);
    ui->chboxConstantTimer->setChecked(true);

    for (NumberEdit* edit : findChildren<NumberEdit*>())
    {
        if (edit->text() == "0")
        {
            edit->setDecimalPlaces(0);
        }
        else if (edit->text() == "0.00")
            edit->setDecimalPlaces(2);
    }

}

IfProgramDialog::~IfProgramDialog()
{
    delete ui;
}

void IfProgramDialog::on_chboxConstantVar_clicked()
{
    ui->editVarPostOp->show();
    ui->coboxVarSelectPostOp1->hide();
    ui->coboxVarRealProductNumPostOp->hide();
}

void IfProgramDialog::on_chboxVariableVar_clicked()
{
    ui->editVarPostOp->hide();
    ui->coboxVarSelectPostOp1->show();
    ui->coboxVarRealProductNumPostOp->hide();
}

void IfProgramDialog::on_chboxRealProd_clicked()
{
    ui->editVarPostOp->hide();
    ui->coboxVarSelectPostOp1->hide();
    ui->coboxVarRealProductNumPostOp->show();
}

void IfProgramDialog::on_chboxConstantRealProd_clicked()
{
    ui->editRealProdNumPostOp->show();
    ui->coboxVarSelectPostOp2->hide();
}

void IfProgramDialog::on_chboxVariableRealProd_clicked()
{
    ui->editRealProdNumPostOp->hide();
    ui->coboxVarSelectPostOp2->show();
}

void IfProgramDialog::on_chboxConstantStack_clicked()
{
    ui->editStackPostOp->show();
    ui->coboxVarStackPostOp->hide();
}

void IfProgramDialog::on_chboxStack_clicked()
{
    ui->editStackPostOp->hide();
    ui->coboxVarStackPostOp->show();
}

void IfProgramDialog::on_chboxConstantTimer_clicked()
{
    ui->editTimerPostOp->show();
    ui->labTimerUnit->show();
    ui->coboxTimerPostOp->hide();

}

void IfProgramDialog::on_chboxTimerTimer_clicked()
{
    ui->editTimerPostOp->hide();
    ui->labTimerUnit->hide();
    ui->coboxTimerPostOp->show();

}

void IfProgramDialog::on_chboxConstantPos_clicked()
{
    ui->editPosPostOp->show();
    ui->labPosUnit->show();
    ui->coboxPosVarSelectPostOp->hide();
    ui->coboxPosPostOpAxisSelect->hide();
}

void IfProgramDialog::on_chboxVariablePos_clicked()
{
    ui->editPosPostOp->hide();
    ui->labPosUnit->hide();
    ui->coboxPosVarSelectPostOp->show();
    ui->coboxPosPostOpAxisSelect->hide();
}

void IfProgramDialog::on_chboxAxis_clicked()
{
    ui->editPosPostOp->hide();
    ui->labPosUnit->hide();
    ui->coboxPosVarSelectPostOp->hide();
    ui->coboxPosPostOpAxisSelect->show();
}
