#include "cleardialog.h"
#include "ui_cleardialog.h"

ClearDialog::ClearDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ClearDialog)
{
    ui->setupUi(this);
}

ClearDialog::~ClearDialog()
{
    delete ui;
}
