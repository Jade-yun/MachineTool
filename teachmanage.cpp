#include "teachmanage.h"
#include "ui_teachmanage.h"
//#include "KeyboardGlobal.h"

TeachManage::TeachManage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TeachManage)
{
    ui->setupUi(this);
//    GlobalInit();
//    ui->lineEdit_OriginalFileName->installEventFilter(this);

    ui->textEdit->installEventFilter(this);

    ui->btn_USB->setCheckable(true);
    isBtn_USB_Checked = false;
    ui->stackedWidget_TeachManage->setCurrentIndex(0);
}

TeachManage::~TeachManage()
{
    delete ui;
}

void TeachManage::on_btn_USB_clicked()
{
    if (isBtn_USB_Checked)
    {
        //ui->btn_USB->setChecked(false);
        isBtn_USB_Checked = false;
        ui->stackedWidget_TeachManage->setCurrentIndex(0);
    }
    else
    {
        //ui->btn_USB->setChecked(true); 
        isBtn_USB_Checked = true;
        ui->stackedWidget_TeachManage->setCurrentIndex(1);
    }
}

void TeachManage::on_btn_New_clicked()
{

}

void TeachManage::on_btn_Copy_clicked()
{

}

void TeachManage::on_btn_Load_clicked()
{

}

void TeachManage::on_btn_Preview_clicked()
{

}

void TeachManage::on_btn_Del_clicked()
{

}

void TeachManage::on_btn_SelectAll_clicked()
{

}

void TeachManage::on_btn_SelectNotAll_clicked()
{

}

void TeachManage::on_btn_Export_clicked()
{

}

void TeachManage::on_btn_Import_clicked()
{

}

//bool TeachManage::eventFilter(QObject *watched, QEvent *event)
//{
//    if (event->type()==QEvent::FocusIn)
//    {
//        PlatformInputContextBase->FocusIn(watched);
//    }
//    else if (event->type()==QEvent::FocusOut)
//    {
//        PlatformInputContextBase->FocusOut(watched);
//    }

//    return QWidget::eventFilter(watched,event);
//}
