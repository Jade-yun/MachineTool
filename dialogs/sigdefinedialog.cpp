#include "sigdefinedialog.h"
#include "ui_sigdefinedialog.h"

SigDefineDialog::SigDefineDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SigDefineDialog)
{
    ui->setupUi(this);

    connect(ui->btnOK, &QPushButton::clicked, this, &SigDefineDialog::accept);
    connect(ui->btnCancel, &QPushButton::clicked, this, &SigDefineDialog::reject);
}

SigDefineDialog::~SigDefineDialog()
{
    delete ui;
}

QString SigDefineDialog::getMainBoardInSig() const
{
    if (ui->chboxMainBoardIn0->isChecked()) return ui->coboxMainBoardIn0->currentText();
    if (ui->chboxMainBoardIn1->isChecked()) return ui->coboxMainBoardIn1->currentText();
    if (ui->chboxMainBoardIn2->isChecked()) return ui->coboxMainBoardIn2->currentText();
    if (ui->chboxMainBoardInExtend0->isChecked()) return ui->coboxMainBoardInExtend0->currentText();
    if (ui->chboxMainBoardInExtend1->isChecked()) return ui->coboxMainBoardInExtend1->currentText();
    return QString("0");
}

QString SigDefineDialog::getMainBoardOutSig() const
{
    if (ui->chboxMainBoardOut0->isChecked()) return ui->coboxMainBoardOut0->currentText();
    if (ui->chboxMainBoardOut1->isChecked()) return ui->coboxMainBoardOut1->currentText();
    if (ui->chboxMainBoardOut2->isChecked()) return ui->coboxMainBoardOut2->currentText();
    if (ui->chboxMainBoardOutExtend0->isChecked()) return ui->coboxMainBoardOutExtend0->currentText();
    if (ui->chboxMainBoardOutExtend1->isChecked()) return ui->coboxMainBoardOutExtend1->currentText();
    return QString("0");
}

QString SigDefineDialog::getSigDefine() const
{
    if (ui->tabWidget->currentIndex() == 0)
    {
         return getMainBoardInSig();
    }
    else if (ui->tabWidget->currentIndex() == 1)
    {
        return getMainBoardOutSig();
    }
    return QString("0");
}

SigLEDEdit::SigLEDEdit(QWidget *parent)
    : QLineEdit(parent)
{

}

void SigLEDEdit::mousePressEvent(QMouseEvent *event)
{
    QLineEdit::mousePressEvent(event);
    SigDefineDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted)
    {
        QString text = dialog.getSigDefine();
        this->setText(text);
    }
}
