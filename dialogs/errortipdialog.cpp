#include "errortipdialog.h"
#include "ui_errortipdialog.h"

ErrorTipDialog::ErrorTipDialog(QWidget *parent) :
    BaseWindow(parent),
    ui(new Ui::ErrorTipDialog)
{
    ui->setupUi(this);

//    QPixmap icon(":/images/alarm.png");
//    ui->labIcon->setPixmap(icon);
    this->setWindowTitle(tr("提示"));
    setModal(true);
    setMode(TipMode::NORMAL);

    connect(ui->btnOk, &QPushButton::clicked, this, &ErrorTipDialog::accept);
    connect(ui->btnCancel, &QPushButton::clicked, this, &ErrorTipDialog::reject);
}

ErrorTipDialog::ErrorTipDialog(const QString &message, TipMode mode, QWidget *parent) :
    BaseWindow(parent),
    ui(new Ui::ErrorTipDialog)
{
    ui->setupUi(this);

    this->setWindowTitle(tr("提示"));
    setModal(true);
    setMode(mode);
    setMessage(message);

    connect(ui->btnOk, &QPushButton::clicked, this, &ErrorTipDialog::accept);
    connect(ui->btnCancel, &QPushButton::clicked, this, &ErrorTipDialog::reject);
}

ErrorTipDialog::ErrorTipDialog(const QString &message, QWidget *parent) :
    BaseWindow(parent),
    ui(new Ui::ErrorTipDialog)
{
    ui->setupUi(this);

    this->setWindowTitle(tr("提示"));
    setModal(true);
    setMode(TipMode::NORMAL);
    setMessage(message);

    connect(ui->btnOk, &QPushButton::clicked, this, &ErrorTipDialog::accept);
    connect(ui->btnCancel, &QPushButton::clicked, this, &ErrorTipDialog::reject);
}

ErrorTipDialog::~ErrorTipDialog()
{
    delete ui;
}

void ErrorTipDialog::setMessage(const QString &message)
{
    ui->labMessage->setText(message);
}

void ErrorTipDialog::setMode(TipMode mode)
{
    QSize btnSize = ui->btnOk->size();
    if (mode == TipMode::NORMAL)
    {
        ui->btnCancel->show();
        ui->btnOk->show();
        ui->btnOk->setGeometry(100, 280, btnSize.width(), btnSize.height());
    }
    else if (mode == TipMode::ONLY_OK)
    {
        ui->btnCancel->hide();
        ui->btnOk->show();
        ui->btnOk->setGeometry(200, 280, btnSize.width(), btnSize.height());
    }
    else if(mode == TipMode::NULL_BUTTON)
    {
        ui->btnOk->hide();
        ui->btnCancel->hide();
    }
}

void ErrorTipDialog::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
    }
    QWidget::changeEvent(e);
}
