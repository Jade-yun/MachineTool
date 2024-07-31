#include "sigdefinedialog.h"
#include "ui_sigdefinedialog.h"

SigDefineDialog::SigDefineDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SigDefineDialog)
{
    ui->setupUi(this);

    connect(ui->btnOK, &QPushButton::clicked, [=](){
        // save the set to the edit

        this->close();
    });
    connect(ui->btnCancel, &QPushButton::clicked, [=](){
        this->close();
    });
}

SigDefineDialog::~SigDefineDialog()
{
    delete ui;
}

SigLEDEdit::SigLEDEdit(QWidget *parent)
{

}

void SigLEDEdit::mousePressEvent(QMouseEvent *event)
{
    QLineEdit::mousePressEvent(event);
    SigDefineDialog dialog(this);
    dialog.exec();
}
