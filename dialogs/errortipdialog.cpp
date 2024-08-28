#include "errortipdialog.h"
#include "ui_errortipdialog.h"

ErrorTipDialog::ErrorTipDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ErrorTipDialog)
{
    ui->setupUi(this);

    ui->setupUi(this);

//    QPixmap icon(":/images/alarm.png");
//    ui->labIcon->setPixmap(icon);
    this->setWindowTitle(tr("提示"));
    setModal(false);
    connect(ui->btnOk, &QPushButton::clicked, this, &ErrorTipDialog::close);
}

ErrorTipDialog::~ErrorTipDialog()
{
    delete ui;
}

void ErrorTipDialog::setMessage(const QString &message)
{
    ui->labMessage->setText(message);
}
