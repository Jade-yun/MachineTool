#include "stacksetdialog.h"
#include "ui_stacksetdialog.h"

StackSetDialog::StackSetDialog(QWidget *parent) :
    QDialog(parent),
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
        QString editXName = QString("editX%1").arg(i);
        QString editYName = QString("editY%1").arg(i);
        QString editZName = QString("editZ%1").arg(i);
        QString chboxName = QString("chbox%1").arg(i);

        QLineEdit* editX = findChild<QLineEdit*>(editXName);
        QLineEdit* editY = findChild<QLineEdit*>(editYName);
        QLineEdit* editZ = findChild<QLineEdit*>(editZName);
        QCheckBox* chbox = findChild<QCheckBox*>(chboxName);

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
    this->accept();
    // to save the parameters...
}

void StackSetDialog::on_btnCancel_clicked()
{
    this->reject();
}
