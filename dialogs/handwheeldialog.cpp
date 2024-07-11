#include "handwheeldialog.h"
#include "ui_handwheeldialog.h"

HandWheelDialog::HandWheelDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HandWheelDialog)
{
    ui->setupUi(this);
}

HandWheelDialog::~HandWheelDialog()
{
    delete ui;
}
