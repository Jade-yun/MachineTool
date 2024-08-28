#include "ioportdialog.h"
#include "ui_ioportdialog.h"

IOPortDialog::IOPortDialog(QWidget *parent, IOPortMode mode) :
    QDialog(parent),
    ui(new Ui::IOPortDialog)
{
    ui->setupUi(this);
    if (mode == IOPortMode::IN)
    {
        ui->stkWgt->setCurrentIndex(0);
    }
    else if (mode == IOPortMode::OUT)
    {
        ui->stkWgt->setCurrentIndex(1);
    }

    connect(ui->btnOK, &QPushButton::clicked, this, &IOPortDialog::accept);
    connect(ui->btnCancel, &QPushButton::clicked, this, &IOPortDialog::reject);
}

IOPortDialog::~IOPortDialog()
{
    delete ui;
}

QString IOPortDialog::getIOOnlineIn() const
{
    if (ui->stkWgt->currentIndex() == 0 && ui->tabWgtMainBoardIn->currentIndex() == 0)
    {
        return getMainBoardIn();
    }
    return QString();
}

QString IOPortDialog::getIOOnlineOut() const
{
    if (ui->stkWgt->currentIndex() == 1 && ui->tabWgtMainBoardOut->currentIndex() == 0)
    {
        return getMainBoardOut();
    }
    return QString();
}

QString IOPortDialog::getMainBoardIn() const
{
    if (ui->chboxMainBoardIn0->isChecked()) return ui->coboxMainBoardIn0->currentText();
    if (ui->chboxMainBoardIn1->isChecked()) return ui->coboxMainBoardIn1->currentText();
    if (ui->chboxMainBoardIn2->isChecked()) return ui->coboxMainBoardIn2->currentText();
    if (ui->chboxMainBoardInExtend0->isChecked()) return ui->coboxMainBoardInExtend0->currentText();
    if (ui->chboxMainBoardInExtend1->isChecked()) return ui->coboxMainBoardInExtend1->currentText();
    return QString("NULL");
}

QString IOPortDialog::getMainBoardOut() const
{
    if (ui->chboxMainBoardOut0->isChecked()) return ui->coboxMainBoardOut0->currentText();
    if (ui->chboxMainBoardOut1->isChecked()) return ui->coboxMainBoardOut1->currentText();
    if (ui->chboxMainBoardOut2->isChecked()) return ui->coboxMainBoardOut2->currentText();
    if (ui->chboxMainBoardOutExtend0->isChecked()) return ui->coboxMainBoardOutExtend0->currentText();
    if (ui->chboxMainBoardOutExtend1->isChecked()) return ui->coboxMainBoardOutExtend1->currentText();
    return QString("NULL");
}


IOOnlineInEdit::IOOnlineInEdit(QWidget *parent)
    : QLineEdit(parent)
{

}

void IOOnlineInEdit::mousePressEvent(QMouseEvent *event)
{
    QLineEdit::mousePressEvent(event);
    IOPortDialog dialog(this, IOPortMode::IN);
    if (dialog.exec() == QDialog::Accepted)
    {
        QString text = dialog.getIOOnlineIn();
        this->setText(text);
    }
}

IOOnlineOutEdit::IOOnlineOutEdit(QWidget *parent)
    : QLineEdit(parent)
{

}

void IOOnlineOutEdit::mousePressEvent(QMouseEvent *event)
{
    QLineEdit::mousePressEvent(event);
    IOPortDialog dialog(this, IOPortMode::OUT);
    if (dialog.exec() == QDialog::Accepted)
    {
        QString text = dialog.getIOOnlineOut();
        this->setText(text);
    }
}
