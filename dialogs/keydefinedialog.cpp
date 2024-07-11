#include "keydefinedialog.h"
#include "ui_keydefinedialog.h"

KeyDefineDialog::KeyDefineDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::KeyDefineDialog)
{
    ui->setupUi(this);
}

KeyDefineDialog::~KeyDefineDialog()
{
    delete ui;
}
