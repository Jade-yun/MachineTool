#include "keydefinedialog.h"
#include "ui_keydefinedialog.h"

#include <QDebug>

KeyDefineDialog::KeyDefineDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::KeyDefineDialog)
{
    ui->setupUi(this);

    connect(ui->btnOK, &QPushButton::clicked, this, &KeyDefineDialog::accept);
    connect(ui->btnCancel, &QPushButton::clicked, this, &KeyDefineDialog::reject);
}

KeyDefineDialog::~KeyDefineDialog()
{
    delete ui;
}

QString KeyDefineDialog::getValveOutStatus() const
{
    if (ui->chboxValveOutStatus0->isChecked()) return ui->chboxValveOutStatus0->text();
    if (ui->chboxValveOutStatus1->isChecked()) return ui->chboxValveOutStatus1->text();
    if (ui->chboxValveOutStatus2->isChecked()) return ui->chboxValveOutStatus2->text();
    return QString();
}

QString KeyDefineDialog::getValveOutput() const
{
    if (ui->chboxValveAction0->isChecked())
        return ui->coboxValveAction0->currentText() + QString(" ") + getValveOutStatus();
    if (ui->chboxValveAction1->isChecked())
        return ui->coboxValveAction1->currentText() + QString(" ") + getValveOutStatus();
    if (ui->chboxValveAction2->isChecked())
        return ui->coboxValveAction2->currentText() + QString(" ") + getValveOutStatus();
    return QString("0");
}

QString KeyDefineDialog::getReserveOutStatus() const
{
    if (ui->chboxReserveOutStatus0->isChecked()) return ui->chboxReserveOutStatus0->text();
    if (ui->chboxReserveOutStatus1->isChecked()) return ui->chboxReserveOutStatus1->text();
    if (ui->chboxReserveOutStatus2->isChecked()) return ui->chboxReserveOutStatus2->text();
    return QString();
}

QString KeyDefineDialog::getMainBoardOut() const
{
    if (ui->chboxMainBoardOut0->isChecked())
        return ui->coboxMainBoardOut0->currentText() + QString(" ") + getReserveOutStatus();
    if (ui->chboxMainBoardOut1->isChecked())
        return ui->coboxMainBoardOut1->currentText() + QString(" ") + getReserveOutStatus();
    if (ui->chboxMainBoardOut2->isChecked())
        return ui->coboxMainBoardOut2->currentText() + QString(" ") + getReserveOutStatus();

    if (ui->chboxMainBoardOutExtend0->isChecked())
        return ui->coboxMainBoardOutExtend0->currentText() + QString(" ") + getReserveOutStatus();
    if (ui->chboxMainBoardOutExtend1->isChecked())
        return ui->coboxMainBoardOutExtend1->currentText() + QString(" ") + getReserveOutStatus();
    return QString("0");
}

QString KeyDefineDialog::getAxisMoveDirect() const
{
    QString text = ui->coboxAxisSelect->currentText();
    if (ui->chboxAxisDirect0->isChecked()) return text + QString(" ") + ui->chboxAxisDirect0->text();
    if (ui->chboxAxisDirect0->isChecked()) return text + QString(" ") + ui->chboxAxisDirect1->text();;
    return text;
}

QString KeyDefineDialog::getKeyDefine() const
{
    QString text;
    switch (ui->tabWidget->currentIndex()) {
    case 0:
        text = getValveOutput();
        return text;
    case 1:
        text = getMainBoardOut();
        return text;
    case 2:
        text = getAxisMoveDirect();
        return text;
    default:
        break;
    }
    return text;
}



#if USE_LINEEDIT

KeyEdit::KeyEdit(QWidget *parent)
    : QLineEdit(parent)
{

}

void KeyEdit::mousePressEvent(QMouseEvent *event)
{
    QLineEdit::mousePressEvent(event);
//    qDebug() << "KeyEdit is clicked ";
    KeyDefineDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted)
    {

        QString res = dialog.getKeyDefine();
        this->setText(res);
        emit saveKeyDef();
    }
}

#else
KeyEdit::KeyEdit(QWidget *parent)
    : QLabel(parent)
{
    setStyleSheet(
        "QLabel {"
        "   background-color: white;"
        "   border: 1px solid #A0A0A0;"
        "   border-radius: 2px;"
        "   padding: 2px;"
        "   color: black;"
        "   min-height: 40px;"
        "}"
    );
}

void KeyEdit::mousePressEvent(QMouseEvent *event)
{
//    QLineEdit::mousePressEvent(event);
    qDebug() << "KeyEdit is clicked ";
}
#endif
