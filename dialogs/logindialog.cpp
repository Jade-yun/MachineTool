#include "logindialog.h"
#include "ui_logindialog.h"

LoginDialog::LoginDialog(QWidget *parent) :
    BaseWindow(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    setModal(true);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}
