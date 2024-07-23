#include "clearvardialog.h"
#include "ui_clearvardialog.h"

ClearVarDialog::ClearVarDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ClearVarDialog)
{
    ui->setupUi(this);
}

ClearVarDialog::~ClearVarDialog()
{
    delete ui;
}
