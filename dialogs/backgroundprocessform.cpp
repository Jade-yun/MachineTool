#include "backgroundprocessform.h"
#include "ui_backgroundprocessform.h"

BackgroundProcessForm::BackgroundProcessForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BackgroundProcessForm)
{
    ui->setupUi(this);

    frameBase = new FramelessWidget2(this);

    frameBase->setWidget(this);
    frameBase->setTitle(tr("后台程序"));
}

BackgroundProcessForm::~BackgroundProcessForm()
{
    delete ui;
}

void BackgroundProcessForm::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
    }
    QWidget::changeEvent(e);
}
