#include "autocorrectposdialog.h"
#include "ui_autocorrectposdialog.h"

AutoCorrectPosDialog::AutoCorrectPosDialog(QWidget *parent) :
    BaseWindow(parent),
    ui(new Ui::AutoCorrectPosDialog)
{
    ui->setupUi(this);
    setModal(true);
}

AutoCorrectPosDialog::~AutoCorrectPosDialog()
{
    delete ui;
}
