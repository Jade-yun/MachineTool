#include "handwheeldialog.h"
#include "ui_handwheeldialog.h"

HandWheelDialog::HandWheelDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HandWheelDialog)
{
    ui->setupUi(this);

    connect(ui->btnOK, &QPushButton::clicked, [=](){
       this->close();
    });
}

HandWheelDialog::~HandWheelDialog()
{
    delete ui;
}
