#include "clearvardialog.h"
#include "ui_clearvardialog.h"

ClearVarDialog::ClearVarDialog(QWidget *parent) :
    BaseWindow(parent),
    ui(new Ui::ClearVarDialog)
{
    ui->setupUi(this);
    setModal(true);
}

ClearVarDialog::~ClearVarDialog()
{
    delete ui;
}
