#include "cleardialog.h"
#include "ui_cleardialog.h"

ClearDialog::ClearDialog(QWidget *parent) :
    BaseWindow(parent),
    ui(new Ui::ClearDialog)
{
    ui->setupUi(this);
    setModal(true);
}

ClearDialog::~ClearDialog()
{
    delete ui;
}
