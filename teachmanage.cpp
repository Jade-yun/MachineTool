#include "teachmanage.h"
#include "ui_teachmanage.h"
//#include "KeyboardGlobal.h"


TeachManage::TeachManage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TeachManage)
{
    ui->setupUi(this);
//    GlobalInit();

#if TESTKEYBOARD
    keyboard = new FullKeyboard(this);
    QList<QLineEdit*> lineEdits = findChildren<QLineEdit*>();
    for (auto edit : lineEdits)
    {
        edit->installEventFilter(this);
    }
#endif

//    ui->lineEdit_OriginalFileName->installEventFilter(this);
    ui->lineEdit_NowFileName->setFocusPolicy(Qt::ClickFocus);

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


#if TESTKEYBOARD
// ultimize the event mechanism in Qt to call virtual keyboard
bool TeachManage::eventFilter(QObject *watched, QEvent *event)
{
//    if (watched is in QEdit)
//    if (event->type()==QEvent::FocusIn)
//    if(event->type() == QFocusEvent::FocusIn)
//    {
//        // to call

//        qDebug() << "edit was clicked";
//        return false;
//    }
//    else if (event->type()== QEvent::FocusOut)
//    {
//        // to close
////        PlatformInputContextBase->FocusOut(watched);

//    }
    if (event->type() == QEvent::MouseButtonPress)
    {
        qDebug() << "edit MouseButtonPress";
//        callKeyboard(watched);
//        return false;
        if (QLineEdit *edit = qobject_cast<QLineEdit*>(watched))
        {
            callKeyboard(watched);
            return true;
        }
    }
    // standard event processing
//    return QObject::eventFilter(watched, event);
    return QWidget::eventFilter(watched,event);


//    if (obj == textEdit) {
//        if (event->type() == QEvent::KeyPress) {
//            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
//            qDebug() << "Ate key press" << keyEvent->key();
//            return true;
//        } else {
//            return false;
//        }
//    } else {
//        // pass the event on to the parent class
//        return QMainWindow::eventFilter(obj, event);
    //    }
}

void TeachManage::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
    {
        // 保存键盘的内容到触发键盘控件
        qDebug() << "TeachManage::keyPressEvent(QKeyEvent *event)";

    }
}

void TeachManage::callKeyboard(QObject *watched)
{
    if (QLineEdit* edit = qobject_cast<QLineEdit*>(watched))
    {
        keyboard->setCurrentEdit(edit);
        keyboard->show();
        keyboard->raise();
        keyboard->activateWindow();
    }
}

#endif
