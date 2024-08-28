#include "autocorrectposdialog.h"
#include "ui_autocorrectposdialog.h"

AutoCorrectPosDialog::AutoCorrectPosDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AutoCorrectPosDialog)
{
    ui->setupUi(this);
}

AutoCorrectPosDialog::~AutoCorrectPosDialog()
{
    delete ui;
}
