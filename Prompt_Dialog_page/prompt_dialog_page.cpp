#include "prompt_dialog_page.h"
#include "ui_prompt_dialog_page.h"
#include <QTimer>

Prompt_Dialog_page::Prompt_Dialog_page(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Prompt_Dialog_page)
{
    ui->setupUi(this);
    this->setModal(true); // 设为模态窗口
}

Prompt_Dialog_page::~Prompt_Dialog_page()
{
    delete ui;
}

void Prompt_Dialog_page::on_Key_Prompt_dialog_yes_clicked()
{
    this->close();
}

