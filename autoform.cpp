#include "autoform.h"
#include "ui_autoform.h"

AutoForm::AutoForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AutoForm)
{
    ui->setupUi(this);
}

AutoForm::~AutoForm()
{
    delete ui;
}
