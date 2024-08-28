#include<QProcess>
#include <QDir>
#include <QThread>
#include "prompt_yes_no_dialog_page.h"
#include "ui_prompt_yes_no_dialog_page.h"
#include "./Prompt_Dialog_page/prompt_com.h"
#include "./method/cmd.h"


prompt_yes_no_Dialog_page::prompt_yes_no_Dialog_page(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::prompt_yes_no_Dialog_page)
{
    ui->setupUi(this);
    this->setModal(true);//设置为模态窗体
}

prompt_yes_no_Dialog_page::~prompt_yes_no_Dialog_page()
{
    delete ui;
}

void prompt_yes_no_Dialog_page::on_Key_yes_no_page_yes_clicked()
{
//    switch(LCD_DoubleButton_Num)
//    {
//        case DOUBLE_BUTTON_01: //确认删除这条指令
//        {
//            if(g_ProOrderOperate(CurPro_Num,m_OperateProOrderListNum,1,0) == 0)
//            {
//                emit DeleteOrder_Return();//发射删除成功信号
//                this->close();
//            }
//            else
//            {
//                ui->yes_no_content_label->setText("删除失败");//不能删除
//            }
//            break;
//        }
//        default:
//            break;
//    }
}

void prompt_yes_no_Dialog_page::on_Key_yes_no_page_no_clicked()
{
    this->close();
}
