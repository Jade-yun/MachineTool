#include "stacksetdialog.h"
#include "ui_stacksetdialog.h"

StackSetDialog::StackSetDialog(QWidget *parent) :
    BaseWindow(parent),
    ui(new Ui::StackSetDialog)
{
    ui->setupUi(this);

    // all dialogs are invisible defualt
    this->hide();
    setAcceptDrops(true);
    setModal(true);
    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);

    changeEnable();
    for (int i = 1; i <= 11; i++)
    {
        QCheckBox* chbox = findChild<QCheckBox*>(QString("chbox%1").arg(i));
        connect(chbox, &QCheckBox::stateChanged, this, &StackSetDialog::changeEnable);
    }
}

StackSetDialog::~StackSetDialog()
{
    delete ui;
}

void StackSetDialog::changeEnable()
{
    for (int i = 1; i <= 8; ++i)
    {
        editXName = QString("editX%1").arg(i);
        editYName = QString("editY%1").arg(i);
        editZName = QString("editZ%1").arg(i);
        chboxName = QString("chbox%1").arg(i);

        NumberEdit* editX = findChild<NumberEdit*>(editXName);
        NumberEdit* editY = findChild<NumberEdit*>(editYName);
        NumberEdit* editZ = findChild<NumberEdit*>(editZName);
        QCheckBox* chbox = findChild<QCheckBox*>(chboxName);
        editX->setDecimalPlaces(0);
        editY->setDecimalPlaces(0);
        editZ->setDecimalPlaces(0);
        editX->setInputRange(0, m_StackInfo[i-1].stackPointNum[STACK_X_AXIS]);
        editY->setInputRange(0, m_StackInfo[i-1].stackPointNum[STACK_Y_AXIS]);
        editZ->setInputRange(0, m_StackInfo[i-1].stackPointNum[STACK_Z_AXIS]);
        if (editX && editY && editZ && chbox)
        {
            editX->setEnabled(chbox->isChecked());
            editY->setEnabled(chbox->isChecked());
            editZ->setEnabled(chbox->isChecked());
        }
    }
    for (int i = 9; i <= 11; i++)
    {
        QLineEdit* edit = findChild<QLineEdit*>(QString("edit%1").arg(i));
        QCheckBox* chbox = findChild<QCheckBox*>(QString("chbox%1").arg(i));
        if (edit && chbox)
        {
            edit->setEnabled(chbox->isChecked());
        }
    }
}

void StackSetDialog::on_btnOK_clicked()
{
    for(int i=1;i<=8;i++)
    {
        editXName = QString("editX%1").arg(i);
        editYName = QString("editY%1").arg(i);
        editZName = QString("editZ%1").arg(i);
        chboxName = QString("chbox%1").arg(i);
        QCheckBox* chbox = findChild<QCheckBox*>(chboxName);
        if(chbox->isChecked())
        {
            NumberEdit* editX = findChild<NumberEdit*>(editXName);
            NumberEdit* editY = findChild<NumberEdit*>(editYName);
            NumberEdit* editZ = findChild<NumberEdit*>(editZName);
            m_StackAxisCnt[i-1][0] = (uint16_t)editX->text().toUInt();
            m_StackAxisCnt[i-1][1] = (uint16_t)editY->text().toUInt();
            m_StackAxisCnt[i-1][2] = (uint16_t)editZ->text().toUInt();
            g_Usart->ExtendSendParDeal(CMD_MAIN_STA,CMD_SUN_STA_STACK_NUM_SET,i,0);
        }
    }
    this->accept();
}

void StackSetDialog::on_btnCancel_clicked()
{
    this->reject();
}

void StackSetDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    for(int i=1;i<=8;i++)
    {
        editXName = QString("editX%1").arg(i);
        editYName = QString("editY%1").arg(i);
        editZName = QString("editZ%1").arg(i);
        chboxName = QString("chbox%1").arg(i);
        NumberEdit* editX = findChild<NumberEdit*>(editXName);
        NumberEdit* editY = findChild<NumberEdit*>(editYName);
        NumberEdit* editZ = findChild<NumberEdit*>(editZName);
        editX->setText("0");
        editY->setText("0");
        editZ->setText("0");
    }
}
