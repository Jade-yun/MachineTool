#include "sigdefinedialog.h"
#include "ui_sigdefinedialog.h"

#include <QListView>

SigDefineDialog::SigDefineDialog(QWidget *parent) :
    BaseWindow(parent),
    ui(new Ui::SigDefineDialog)
{
    ui->setupUi(this);
    this->update();

    setModal(true);
    connect(ui->btnOK, &QPushButton::clicked, this, &SigDefineDialog::accept);
    connect(ui->btnCancel, &QPushButton::clicked, this, &SigDefineDialog::reject);

    for (auto cobox : findChildren<QComboBox*>())
    {
        auto listView = new QListView(cobox);
        cobox->setView(listView);
        cobox->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    }
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
    if (ui->chboxMainBoardOutExtend2->isChecked()) return ui->coboxMainBoardOutExtend2->currentText();
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

std::pair<uint8_t, uint8_t> SigDefineDialog::getSigTypeAndPortDefine() const
{
    if (ui->tabWidget->currentIndex() == 0)
    {
        uint8_t type = 1;
        uint8_t port = getInputPort();
        return {type, port};
    }
    else if (ui->tabWidget->currentIndex() == 1)
    {
        uint8_t type = 0;
        uint8_t port = getOutputPort();
        return {type, port};
    }
    return {0,0};
}

int SigDefineDialog::getInputPort() const
{
    if (ui->chboxMainBoardIn0->isChecked())
    {
        int index = ui->coboxMainBoardIn0->currentIndex();
        return index + 1;
    }
    if (ui->chboxMainBoardIn1->isChecked())
    {
       int index = ui->coboxMainBoardIn1->currentIndex();
       return index + 12 * 1 + 1;
    }
    if (ui->chboxMainBoardIn2->isChecked())
    {
        int index =  ui->coboxMainBoardIn2->currentIndex();
        return index + 12 * 2 + 1;
    }
    if (ui->chboxMainBoardInExtend0->isChecked())
    {
        int index  = ui->coboxMainBoardInExtend0->currentIndex();
        return index + 12 * 3 + 1;
    }
    if (ui->chboxMainBoardInExtend1->isChecked())
    {
        int index  = ui->coboxMainBoardInExtend1->currentIndex();
        return index + 12 * 4 + 1;
    }
    return 0;
}

int SigDefineDialog::getOutputPort() const
{
    if (ui->chboxMainBoardOut0->isChecked()) {
        int index  = ui->coboxMainBoardOut0->currentIndex();
        return index + 1;
    }
    if (ui->chboxMainBoardOut1->isChecked()) {
        int index = ui->coboxMainBoardOut1->currentIndex();
        return index + 8*1 + 1;
    }
    if (ui->chboxMainBoardOut2->isChecked()) {

        int index = ui->coboxMainBoardOut2->currentIndex();
        return index + 8*2 + 1;
    }
    if (ui->chboxMainBoardOutExtend0->isChecked()) {
        int index = ui->coboxMainBoardOutExtend0->currentIndex();
        return index + 8*3 + 1;
    }
    if (ui->chboxMainBoardOutExtend1->isChecked()) {
        int index = ui->coboxMainBoardOutExtend1->currentIndex();
        return index + 8*4 + 1;
    }
    if (ui->chboxMainBoardOutExtend2->isChecked()) {
        int index = ui->coboxMainBoardOutExtend2->currentIndex();
        return index + 8*5 + 1;
    }
    return 0;
}


SigLEDEdit::SigLEDEdit(QWidget *parent)
    : QLineEdit(parent),
      sigType(0), port(0)
{

}

void SigLEDEdit::mouseReleaseEvent(QMouseEvent *event)
{
    QLineEdit::mouseReleaseEvent(event);
    SigDefineDialog dialog;
    if (dialog.exec() == QDialog::Accepted)
    {
        QString text = dialog.getSigDefine();
        auto type_port = dialog.getSigTypeAndPortDefine();
        this->setSigTypeAndPort(type_port.first, type_port.second);
        this->setText(text);
        emit returnPressed();
    }
}

uint8_t SigLEDEdit::getSigType() const
{
    return sigType;
}

uint8_t SigLEDEdit::getSigPort() const
{
    return port;
}

void SigLEDEdit::setSigTypeAndPort(uint8_t type, uint8_t portIndex)
{
    sigType = type;
    port = portIndex;
}

void SigLEDEdit::setSigType(uint8_t type)
{
    sigType = type;
}

void SigLEDEdit::setSigPort(uint8_t portIndex)
{
    port = portIndex;
}

