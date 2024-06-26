#include "autoform.h"
#include "ui_autoform.h"

#pragma execution_character_set("utf-8")


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
