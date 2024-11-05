#include "keydefinedialog.h"
#include "ui_keydefinedialog.h"

#include <QListView>
#include <QDebug>

KeyDefineDialog::KeyDefineDialog(QWidget *parent) :
    BaseWindow(parent),
    ui(new Ui::KeyDefineDialog),
    m_KeyFunc{0, 0, 0}
{
    ui->setupUi(this);
    this->update();

    setModal(true);
    for (auto cobox : findChildren<QComboBox*>())
    {
        auto listView = new QListView(cobox);
        cobox->setView(listView);

        cobox->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    }

    connect(ui->btnOK, &QPushButton::clicked, this, &KeyDefineDialog::accept);
    connect(ui->btnCancel, &QPushButton::clicked, this, &KeyDefineDialog::reject);
}

KeyDefineDialog::~KeyDefineDialog()
{
    delete ui;
}

QString KeyDefineDialog::getValveOutStatusStr() const
{
    if (ui->chboxValveOutStatus0->isChecked()) return ui->chboxValveOutStatus0->text();
    if (ui->chboxValveOutStatus1->isChecked()) return ui->chboxValveOutStatus1->text();
    if (ui->chboxValveOutStatus2->isChecked()) return ui->chboxValveOutStatus2->text();
    return QString();
}

QString KeyDefineDialog::getValveOutputStr() const
{
    if (ui->chboxValveAction0->isChecked())
        return ui->coboxValveAction0->currentText() + QString(" ") + getValveOutStatusStr();
    if (ui->chboxValveAction1->isChecked())
        return ui->coboxValveAction1->currentText() + QString(" ") + getValveOutStatusStr();
    if (ui->chboxValveAction2->isChecked())
        return ui->coboxValveAction2->currentText() + QString(" ") + getValveOutStatusStr();
    return QString("0");
}

QString KeyDefineDialog::getReserveOutStatusStr() const
{
    if (ui->chboxReserveOutStatus0->isChecked()) return ui->chboxReserveOutStatus0->text();
    if (ui->chboxReserveOutStatus1->isChecked()) return ui->chboxReserveOutStatus1->text();
    if (ui->chboxReserveOutStatus2->isChecked()) return ui->chboxReserveOutStatus2->text();
    return QString();
}

QString KeyDefineDialog::getMainBoardOutStr() const
{
    if (ui->chboxMainBoardOut0->isChecked()) {
        return ui->coboxMainBoardOut0->currentText() + QString(" ") + getReserveOutStatusStr();
    }
    if (ui->chboxMainBoardOut1->isChecked()) {
        return ui->coboxMainBoardOut1->currentText() + QString(" ") + getReserveOutStatusStr();
    }
    if (ui->chboxMainBoardOut2->isChecked()) {
        return ui->coboxMainBoardOut2->currentText() + QString(" ") + getReserveOutStatusStr();
    }
    if (ui->chboxMainBoardOutExtend0->isChecked()) {
        return ui->coboxMainBoardOutExtend0->currentText() + QString(" ") + getReserveOutStatusStr();
    }
    if (ui->chboxMainBoardOutExtend1->isChecked()) {
        return ui->coboxMainBoardOutExtend1->currentText() + QString(" ") + getReserveOutStatusStr();
    }
    return QString("0");
}

QString KeyDefineDialog::getAxisMoveDirectStr() const
{
    QString text = ui->coboxAxisSelect->currentText();
    if (ui->chboxAxisDirect0->isChecked()) return text + QString(" ") + ui->chboxAxisDirect0->text();
    if (ui->chboxAxisDirect1->isChecked()) return text + QString(" ") + ui->chboxAxisDirect1->text();;
    return text;
}

int KeyDefineDialog::getValveOutputStatus() const
{
    if (ui->chboxValveOutStatus0->isChecked()) return 0;
    if (ui->chboxValveOutStatus1->isChecked()) return 1;
    if (ui->chboxValveOutStatus2->isChecked()) return 2;
    return 0;
}

int KeyDefineDialog::getValveOutputPort() const
{
    if (ui->chboxValveAction0->isChecked())
    {
        int index = ui->coboxValveAction0->currentIndex();
        switch (index) {
        case 0: // 原料1夹紧
            return 0 + 1;
        case 1: // 成品1夹紧
            return 1 + 1;
        case 2: // 原料2夹紧
            return 24 + 1;
        case 3: // 成品2夹紧
            return 26 + 1;
        case 4: // 卡爪1正转
            return 4 + 1;
        case 5: // 卡爪2正转
            return 28 + 1;
        case 6: // 翻转台1正转
            return 36 + 1;
        case 7: // 翻转台1夹紧
            return 38 + 1;
        default:
            break;
        }

    }
    if (ui->chboxValveAction1->isChecked())
    {
        int index = ui->coboxValveAction1->currentIndex();
        if (index == 0) // 8=自动门1开
        {
            return 8 + 1;
        }
        else if (index == 1) // 32=自动门1开
        {
            return 32 + 1;
        }
    }
    if (ui->chboxValveAction2->isChecked())
    {
        int index = ui->coboxValveAction2->currentIndex();
        if (index == 0) //19=卡盘1夹紧
        {
            return 19 + 1;
        }
        else if (index == 1) // 34=卡盘2夹紧
        {
            return 34 + 1;
        }
        else if (index == 2) // 10=吹气1
        {
            return 10 + 1;
        }
        else if (index == 3) //31=吹气2
        {
            return 31 + 1;
        }

    }
    return 0;
}

int KeyDefineDialog::getReserveOutStatus() const
{
    if (ui->chboxReserveOutStatus0->isChecked()) return 0;
    if (ui->chboxReserveOutStatus1->isChecked()) return 1;
    if (ui->chboxReserveOutStatus2->isChecked()) return 2;
    return 0;
}

int KeyDefineDialog::getMainBoardOutPort() const
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

QString KeyDefineDialog::getKeyDefineStr() const
{
    QString text;
    switch (ui->tabWidget->currentIndex()) {
    case 0:
        text = getValveOutputStr();
        return text;
    case 1:
        text = getMainBoardOutStr();
        return text;
    case 2:
        text = getAxisMoveDirectStr();
        return text;
    default:
        break;
    }
    return text;
}

KeyDefineDialog::KeyFunc KeyDefineDialog::getKeyFuncDefine()
{
    switch (ui->tabWidget->currentIndex()) {
    case 0:
        m_KeyFunc.keyType = 0;
        m_KeyFunc.portNum = getValveOutputPort();
        m_KeyFunc.funcStatus = getValveOutputStatus();
        break;
    case 1:
        m_KeyFunc.keyType = 0;
        m_KeyFunc.portNum = getMainBoardOutPort();
        m_KeyFunc.funcStatus = getReserveOutStatus();
        break;
    case 2:
        m_KeyFunc.keyType = 2;
        m_KeyFunc.portNum = ui->coboxAxisSelect->currentIndex() + 1;
        m_KeyFunc.funcStatus = ui->chboxAxisDirect0->isChecked() ? 0 : 1;
        break;
    default:
        break;
    }
    return {m_KeyFunc.keyType, m_KeyFunc.portNum, m_KeyFunc.funcStatus};
}


KeyEdit::KeyEdit(QWidget *parent)
    : QLineEdit(parent),
      keyType(0), portNum(0), funcStatus(0)
{

}

void KeyEdit::mouseReleaseEvent(QMouseEvent *event)
{
    QLineEdit::mouseReleaseEvent(event);
//    qDebug() << "KeyEdit is clicked ";
    KeyDefineDialog dialog;
    if (dialog.exec() == QDialog::Accepted)
    {

        QString expression = dialog.getKeyDefineStr();
        auto keyFunc = dialog.getKeyFuncDefine();
        this->setKeyFunc(keyFunc.keyType, keyFunc.portNum, keyFunc.funcStatus);
        this->setText(expression);
        emit saveKeyDef();
    }
}

uint8_t KeyEdit::getKeyType() const
{
    return keyType;
}

uint8_t KeyEdit::getPortNum() const
{
    return portNum;
}

uint8_t KeyEdit::getKeyFuncStatus() const
{
    return funcStatus;
}

void KeyEdit::setKeyFunc(uint8_t keyType, uint8_t portNum, uint8_t funcStatus)
{
    this->keyType = keyType;
    this->portNum = portNum;
    this->funcStatus = funcStatus;
}
