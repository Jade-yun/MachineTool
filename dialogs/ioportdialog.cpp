#include "ioportdialog.h"
#include "ui_ioportdialog.h"

#include <QListView>

IOPortDialog::IOPortDialog(QWidget *parent, IOPortMode mode) :
    BaseWindow(parent),
    ui(new Ui::IOPortDialog)
{
    ui->setupUi(this);
    this->update();

	setModal(true);
    if (mode == IOPortMode::IN)
    {
        ui->stkWgt->setCurrentIndex(0);
    }
    else if (mode == IOPortMode::OUT)
    {
        ui->stkWgt->setCurrentIndex(1);
    }

    for (auto cobox : findChildren<QComboBox*>())
    {
        auto listView = new QListView(cobox);
        cobox->setView(listView);

        cobox->setFocusPolicy(Qt::FocusPolicy::NoFocus);
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

unsigned int IOPortDialog::getInputPort() const
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

unsigned int IOPortDialog::getOutputPort() const
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

QString IOPortDialog::getMainBoardIn() const
{
    if (ui->chboxMainBoardIn0->isChecked()) return ui->coboxMainBoardIn0->currentText();
    if (ui->chboxMainBoardIn1->isChecked()) return ui->coboxMainBoardIn1->currentText();
    if (ui->chboxMainBoardIn2->isChecked()) return ui->coboxMainBoardIn2->currentText();
    if (ui->chboxMainBoardInExtend0->isChecked()) return ui->coboxMainBoardInExtend0->currentText();
    if (ui->chboxMainBoardInExtend1->isChecked()) return ui->coboxMainBoardInExtend1->currentText();
    return QString("null");
}

QString IOPortDialog::getMainBoardOut() const
{
    if (ui->chboxMainBoardOut0->isChecked()) return ui->coboxMainBoardOut0->currentText();
    if (ui->chboxMainBoardOut1->isChecked()) return ui->coboxMainBoardOut1->currentText();
    if (ui->chboxMainBoardOut2->isChecked()) return ui->coboxMainBoardOut2->currentText();
    if (ui->chboxMainBoardOutExtend0->isChecked()) return ui->coboxMainBoardOutExtend0->currentText();
    if (ui->chboxMainBoardOutExtend1->isChecked()) return ui->coboxMainBoardOutExtend1->currentText();
    if (ui->chboxMainBoardOutExtend2->isChecked()) return ui->coboxMainBoardOutExtend2->currentText();
    return QString("null");
}


IOOnlineInEdit::IOOnlineInEdit(QWidget *parent)
    : QLineEdit(parent),
      port(0)
{

}

uint8_t IOOnlineInEdit::getCurrentPort() const
{
    return port;

}

void IOOnlineInEdit::setCurrentPort(uint8_t portIndex)
{
    port = portIndex;
}

void IOOnlineInEdit::mouseReleaseEvent(QMouseEvent *event)
{
    QLineEdit::mouseReleaseEvent(event);
    IOPortDialog dialog(nullptr, IOPortMode::IN);
    if (dialog.exec() == QDialog::Accepted)
    {
        QString text = dialog.getIOOnlineIn();
        port = static_cast<uint8_t>(dialog.getInputPort());
        this->setText(text);
    }
}

IOOnlineOutEdit::IOOnlineOutEdit(QWidget *parent)
    : QLineEdit(parent),
      port(0)
{

}

uint8_t IOOnlineOutEdit::getCurrentPort() const
{
    return port;
}

void IOOnlineOutEdit::setCurrentPort(uint8_t portIndex)
{
    port = portIndex;
}

void IOOnlineOutEdit::mouseReleaseEvent(QMouseEvent *event)
{
    QLineEdit::mouseReleaseEvent(event);
    IOPortDialog dialog(nullptr, IOPortMode::OUT);
    if (dialog.exec() == QDialog::Accepted)
    {
        QString text = dialog.getIOOnlineOut();
        port = static_cast<uint8_t>(dialog.getOutputPort());
        this->setText(text);
    }
}
