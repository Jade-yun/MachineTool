#include "tippasswddialog.h"
#include "ui_tippasswddialog.h"

TipPasswdDialog::TipPasswdDialog(const QString message, QWidget *parent) :
    BaseWindow(parent),
    ui(new Ui::TipPasswdDialog)
{
    ui->setupUi(this);

    //    QPixmap icon(":/images/alarm.png");
    //    ui->labIcon->setPixmap(icon);
    this->setWindowTitle(tr("提示"));

    ui->labMessage->setText(message);

    ui->editPasswd->setDecimalPlaces(0);
    ui->editPasswd->setText("");
    ui->editPasswd->setInputRange(0, 99999);

    connect(ui->btnOk, &QPushButton::clicked, this, &TipPasswdDialog::accept);
    connect(ui->btnCancel, &QPushButton::clicked, this, &TipPasswdDialog::reject);

}

TipPasswdDialog::~TipPasswdDialog()
{
    delete ui;
}

uint TipPasswdDialog::getPasswd() const
{
    uint passwd = ui->editPasswd->text().toUInt();
    return passwd;
}
