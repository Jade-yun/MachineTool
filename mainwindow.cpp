#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QTimer>
#include <QDateTime>
#include <QFile>

#include <QMessageBox>
#include "stackedit.h"
#include "keydefinedialog.h"
#include "sigdefinedialog.h"
#include "referpointdialog.h"
#include "Orderjoint.h"
#include "program_save.h"
#include "port_setting.h"
#include <iostream>

int flag = 0;
int teachFlag = 0;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    FullKeyboard::instance(this)->hide();
//    NumKeyboard::instance(this);

    dlgErrorTip = new ErrorTipDialog(this);

    connect(ui->labProgramName, &ClickableLabel::toggled, this, [=](bool checked){
            qDebug() << "Label toggled, checked:" << checked;
    });

//    setWindowTitle(QString());
//    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);
//    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);

//    for (auto dlg : findChildren<QDialog*>())
//    {
//        dlg->setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
//    }

    connect(ui->btnAdmin, &QPushButton::clicked, [=](){

        QDialog *dialog = new QDialog(this);

        dialog->setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
//        for (auto dlg : findChildren<QDialog*>())
//        {
//            dlg->setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint);
//        }

        auto p = qobject_cast<QWidget*>(stack[1]->parent());
        QPointer<StackEdit> tempStack = stack[1];
        tempStack->setParent(dialog);
        emit tempStack->showOKandCancelButton(true);
        dialog->exec();
        // fixed me???
        // why is this essential??? in case that stack1 to be a dangling pointer
        if (p)
        {
            stack[1]->setParent(p);
            emit tempStack->showOKandCancelButton(false);
            stack[1]->show();
        }
        else
            stack[1]->setParent(nullptr);

    });

    setWidget = new Setting(this);
    ui->stkWidget->addWidget(setWidget);
    teachWidget = new Teach(this);
    ui->stkWidget->addWidget(teachWidget);

	teachManageWidget = new TeachManage(this);
    ui->stkWidget->addWidget(teachManageWidget);

    autoWidget = new AutoForm(this);
    ui->stkWidget->addWidget(autoWidget);
    alarmWidget = new AlarmForm(this);
    ui->stkWidget->addWidget(alarmWidget);
    monitorWidget = new MonitorForm(this);
    ui->stkWidget->addWidget(monitorWidget);
    manualWidget = new ManualForm(this);
    ui->stkWidget->addWidget(manualWidget);

    connectAllSignalsAndSlots();

    // set the style for whole app
    // fix me ???
    // This does not seem to be working, perhaps the styles are conflicting???
    // try to implement this method in every windows, use this->setStyleSheet() instead
//    this->setStyleFromFile(":/styleSheets/style.qss");

//    ui->labDateTime->keyPressEvent();

    QList<QLineEdit*> lineEdits = findChildren<QLineEdit*>();
//    qDebug() << "number of QLineEdit" << lineEdits.size();
    for (auto edit : lineEdits)
    {
        if (edit->objectName() == "editSystemName" || edit->objectName() == "lineEdit_NowFileName")
            edit->installEventFilter(this);
    }

    
    handWheel = new HandWheelDialog(this);
    connect(ui->btnHandWheel, &QPushButton::clicked, handWheel, [=](){
        handWheel->exec();
    });

//    QTabWidget *sourceTabWidget = manualWidget->findChild<QTabWidget*>("tabWidgetManualPage");
//    QWidget *referPointPage = nullptr;
//    if (sourceTabWidget) {
//        referPointPage = sourceTabWidget->widget(4);
//    }
//    autoWidget->setReferPointPage(manualWidget->getReferPointPage());

    connect(ui->btnAlarm, &QPushButton::clicked, this, [=](){

        IfProgramDialog dlg(this);
        dlg.exec();
//        qDebug() << "str transform = " << safeStringToDouble(str);
//        ReferPointDialog dlg(this);
//        dlg.updateUI(manualWidget->getRerferPoints());
//        dlg.exec();
        qDebug() << "************** ";
    });

    for (NumberEdit* edit : findChildren<NumberEdit*>())
    {
        connect(edit, &NumberEdit::showRangeError, this, &MainWindow::showErrorTip);
    }
    //开机读取配置文件信息，暂时做测试用，后续整理
    getOrderjoinIni();
    getCmdIni1();
    getCmdIni();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete dlgErrorTip;
}


void MainWindow::on_Btn_ManualHome_clicked()
{
    if (flag == 0)
    {
        ui->Btn_ManualHome->setText(tr("停止页面"));
        flag = 1;
    }
    // set to manual mode
    else if (flag == 1)
    {
        ui->Btn_ManualHome->setText(tr("手动页面"));
        ui->labRotateSpeed->setNum(10);
        ui->stkWidget->setCurrentWidget(manualWidget);
        flag = 2;
    }
    // set to auto mode
    else if (flag == 2)
    {
        flag = 0;
        ui->Btn_ManualHome->setText(tr("自动页面"));
        ui->stkWidget->setCurrentWidget(autoWidget);
    }

}

void MainWindow::on_Btn_SetHome_clicked()
{
    emit sigSettingHome();
//    qDebug()<< "emit sigSettingHome";
    ui->stkWidget->setCurrentWidget(setWidget);
}

void MainWindow::on_Btn_MonitorHome_clicked()
{
    // jump to monitor page
    ui->stkWidget->setCurrentWidget(monitorWidget);

}

void MainWindow::on_Btn_TeachHome_clicked()
{
//    ui->stkWidget->setCurrentIndex(1);
    if (teachFlag == 0)
    {
        teachFlag = 1;
        ui->stkWidget->setCurrentWidget(teachWidget);
        emit signal_refresh_TeachList();//发送刷新教导界面列表信号
    }
    else
    {
        ui->stkWidget->setCurrentWidget(teachManageWidget);
        teachFlag = 0;
    }
}

void MainWindow::on_Btn_AlarmHome_clicked()
{

    // jump to alarm page
    ui->stkWidget->setCurrentWidget(alarmWidget);
}

void MainWindow::showErrorTip(const QString &message)
{
    dlgErrorTip->setMessage(message);
    dlgErrorTip->exec();
}

void MainWindow::setSysProgramName(const QString &name)
{
    ui->labProgramName->setText(name);
}

void MainWindow::connectAllSignalsAndSlots()
{


    ui->wgtHelp->hide();
    connect(ui->btnHelp, &QPushButton::clicked, this, [=](){
        ui->wgtHelp->show();
    });
    connect(ui->btnSoftKey, &QPushButton::clicked, this, [=](){
        ui->wgtHelp->hide();
    });

    connect(this, &MainWindow::sigSettingHome, setWidget, &Setting::slotSettingHome);

    connect(manualWidget, &ManualForm::sigShowStackPage, [=](){

        ui->stkWidget->setCurrentWidget(setWidget);
        setWidget->ShowStackPage();
    });
    /*各界面间互动信号槽连接*/
    connect(this,&MainWindow::signal_refresh_TeachList,teachWidget,&Teach::Teach_File_List_Refresh);//刷新教导管理程序显示列表信号

    //显示时间
    QTimer* timer = new QTimer(this);
	connect(timer, &QTimer::timeout, [&]() {
		// 获取当前时间
		QDateTime sysTime = QDateTime::currentDateTime();
		// 转换为字符串
		QString timeStr1 = sysTime.toString("yyyy/MM/dd");
        QString timeStr2 = sysTime.toString("hh:mm:ss");
		// 设置label显示的文本
        ui->labDateTime->setText(timeStr1);
        ui->labDateTime_2->setText(timeStr2);
		});
	// 启动定时器，设置时间间隔为1秒
    timer->start(1000);
}

void MainWindow::setStyleFromFile(const QString &styleSheet)
{
    QFile file(styleSheet);
    file.open(QIODevice::ReadOnly);
    if(file.isOpen())
    {
        QString strFile = QLatin1String(file.readAll());
        qApp->setStyleSheet(strFile);

        file.close();
    }
    else
        qDebug() << "Fail to open stylesheet file: " << styleSheet;

}

// ultimize the event mechanism in Qt to call virtual keyboard
bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
//    QLineEdit* numberEdit = qobject_cast<QLineEdit*>(watched);
    QLineEdit* edit = qobject_cast<QLineEdit*>(watched);

//    KeyEdit* keyEdit = qobject_cast<KeyEdit*>(watched);
//    SigLEDEdit* sigEdit = qobject_cast<SigLEDEdit*>(watched);
//    if (keyEdit || sigEdit)
//    {
//        return QWidget::eventFilter(watched, event);
//    }
    if (edit && edit->isEnabled())
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
    //        qDebug() << "edit MouseButtonPress";
            callFullKeyboard(edit);
            return false;
        }
    }
    // standard event processing
    return QWidget::eventFilter(watched,event);
}

void MainWindow::callFullKeyboard(QObject *watched)
{
    if (QLineEdit* edit = qobject_cast<QLineEdit*>(watched))
    {
        FullKeyboard* keyboard = FullKeyboard::instance(this);
        keyboard->setText(edit->text());
        keyboard->setCurrentEditObj(watched);
        keyboard->show();
        keyboard->raise();
        keyboard->activateWindow();
//        keyboard->exec();
    }
}

void MainWindow::callNumberKeyboard(QObject *watched)
{

}


void MainWindow::on_btnHelp_clicked()
{

//    QMessageBox::information(this, "tips", "this is test",
//                              QMessageBox::Yes | QMessageBox::No, QMessageBox::NoButton);

}

ClickableLabel::ClickableLabel(QWidget *parent)
    : QLabel(parent), checked(false)
{
    // constructor...

}

ClickableLabel::ClickableLabel(const QString &text, QWidget *parent)
    : QLabel(text, parent), checked(false)
{

}

ClickableLabel::~ClickableLabel()
{
    // destructor...
}

bool ClickableLabel::isChecked() const
{
    return checked;
}

void ClickableLabel::setChecked(bool _checked)
{
    checked = _checked;
}

void ClickableLabel::mousePressEvent(QMouseEvent *event)
{

    if (event->button() == Qt::LeftButton)
    {
        checked = !checked;
        emit toggled(checked);
        emit clicked();
    }

}

