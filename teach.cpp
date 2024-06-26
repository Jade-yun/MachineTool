#include "teach.h"
#include "ui_teach.h"

Teach::Teach(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Teach)
{
    ui->setupUi(this);
}

Teach::~Teach()
{
    delete ui;
}
