#include "cleardialog.h"
#include "ui_cleardialog.h"

#include "framelesswidget2.h"

ClearDialog::ClearDialog(QWidget *parent) :
    BaseWindow(parent),
    ui(new Ui::ClearDialog)
{
    ui->setupUi(this);
    setModal(true);

//    FramelessWidget2* frameBase = new FramelessWidget2(this);
//    frameBase->setWidget(ui->titleBar);

    ui->stkWidget->setCurrentIndex(0);
    connect(ui->btnOK, &QPushButton::clicked, this, &QDialog::accept);

}

ClearDialog::ClearDialog(ClearDialog::ClearMode mode, QWidget *parent) :
    BaseWindow(parent),
    ui(new Ui::ClearDialog)
{
    ui->setupUi(this);
    setModal(true);

//    FramelessWidget2* frameBase = new FramelessWidget2(this);
//    frameBase->setWidget(this);

    ui->stkWidget->setCurrentIndex(mode);
    connect(ui->btnOK, &QPushButton::clicked, this, &QDialog::accept);
}

ClearDialog::~ClearDialog()
{
    delete ui;
}

void ClearDialog::setMode(ClearDialog::ClearMode mode)
{
    ui->stkWidget->setCurrentIndex(mode);
}
