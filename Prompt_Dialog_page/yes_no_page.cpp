#include "yes_no_page.h"
#include "ui_yes_no_page.h"

yes_no_page::yes_no_page(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::yes_no_page)
{
    ui->setupUi(this);
}

yes_no_page::~yes_no_page()
{
    delete ui;
}

void yes_no_page::on_Key_yes_no_page_yes_clicked()
{

}

void yes_no_page::on_Key_yes_no_page_no_clicked()
{
    this->close();
}
