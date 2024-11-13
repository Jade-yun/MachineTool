#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QTimer>
#include <QDateTime>
#include <QFile>
#include <QTextEdit>
#include <QPixmap>
#include <QPalette>
#include <QMessageBox>
#include "stackedit.h"
#include "keydefinedialog.h"
#include "sigdefinedialog.h"
#include "referpointdialog.h"
#include "Orderjoint.h"
#include "program_save.h"
#include "port_setting.h"

#include "ifprogramdialog.h"
#include <QStyleFactory>

#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>

MainWindow* MainWindow::pMainWindow = nullptr;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , curMode(TriMode::STOP)
{
    timer.start();
    timer1.start();
    ui->setupUi(this);
    pMainWindow = this; // it's better to initialize pMainWindow here
    connect(ui->Progress_bar,SIGNAL(valueChanged(int)),ui->Progress_num,SLOT(setValue(int)));//进度条当前值与显示数字关联
    ui->stkWidget->setCurrentWidget(ui->Init_page);

    ui->Init_page->setStyleSheet("QWidget { background-image: url(/root/stop.jpg); }");

    MainWindow_SetControl_Stake(false);//先禁用按钮，待初始化成功后才能点击

    ui->wgtHelp->setVisible(false);
    //开机读取配置文件信息，暂时做测试用，后续整理
    getOrderjoinIni();
    readIniProgramStruct();
    readIniPortStruct();
    readIniPara();

//    setWindowTitle(QString());
//    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);
//    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);


    login = new LoginDialog(this);
    connect(ui->btnAdmin, &QPushButton::clicked, [=](){
        if (login->exec() == QDialog::Accepted)
        {
            int mode = login->getLoginMode();
            uint loginPasswd = login->getInputPasswd();
        }
        else
        {

        }
    });

    // set the style for whole app
    // fix me ???
    // This does not seem to be working, perhaps the styles are conflicting???
    // try to implement this method in every windows, use this->setStyleSheet() instead
//    auto t1 = timer.elapsed();
//    this->setStyleFromFile(":/styleSheets/style.qss");
//    qDebug() << "setStyleFromFile :" << timer.elapsed() - t1;
//    ui->labDateTime->keyPressEvent();

    QList<QLineEdit*> lineEdits = findChildren<QLineEdit*>();
//    qDebug() << "number of QLineEdit" << lineEdits.size();
    for (auto edit : lineEdits)
    {
        edit->installEventFilter(this);
    }
    //串口通讯线程开启和解析
    CircularBuffer_Init();
    g_Usart = new Usart();
    if (g_Usart->openSerialPort())
    {
        qDebug() << "can not open serialport ";
    }
    startAllThread();
	Refresh_Progress_bar(10);

    // 输入事件扫描 放在最后 app彻底启动完成
    const QVector<QString> devices = {
        "/dev/input/event4",
        "/dev/input/event8",
        "/dev/input/event1",
        "/dev/input/event2",
        "/dev/input/event9"
    };
    scanner = new EventScanner(devices);

    connect(scanner, &EventScanner::eventRotaryEncoder, [](uint16_t code, int32_t value) {
        qDebug() << "Rotary Encoder event:" << code << value;
        // 顺时针旋转
        if (code == 0 && value == 1)
        {
            //to do...
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_AXIS,2);

        }
        // 逆时针旋转
        else if (code == 0 && value == -1)
        {
            // to do...
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_AXIS,1);
        }
    });
    connect(scanner, &EventScanner::eventTrimodeSwitch, [this](uint16_t code, int32_t value) {
        qDebug() << "Trimode Switch event:" << code << value;
        if (code == 142 && value == 1)
        {
            // stop mode
            curMode = TriMode::STOP;
            ui->Btn_TeachHome->setText(tr("教导管理"));
            TrimodeSwitchCommandSend(code,value);
        }
        else if (code == 143 && value == 1)
        {
            // switch to manual mode
            curMode = TriMode::MANUAL;
            ui->Btn_TeachHome->setText(tr("教导"));
            TrimodeSwitchCommandSend(code,value);
        }
        else if (code == 144 && value == 1)
        {
            // switch to automatic mode
            curMode = TriMode::AUTO;
            TrimodeSwitchCommandSend(code,value);
        }
        emit ui->Btn_ManualHome->clicked();
        ui->Btn_ManualHome->setChecked(true);
    });
    connect(scanner, &EventScanner::eventLeftKey, this,[this](uint16_t code, int32_t value) {
        qDebug() << "Left Key event:" << code << value;
        Beeper::instance()->beep();
        keyFunctCommandSend(code,value);
    });
    connect(scanner, &EventScanner::eventRightKey, this,[this](uint16_t code, int32_t value) {
        qDebug() << "Right Key event:" << code << value;
        Beeper::instance()->beep();
        keyAxisCommandSend(code,value);
    });
//    connect(scanner, &EventScanner::errorOccurred, [](const QString &errorString) {
//        qWarning() << "Error:" << errorString;
//        Beeper::instance()->beep();
//    });

//    scanner->start();
//    connect(ui->btnAlarm, &QPushButton::clicked, this, [=](){
//        scanner->start();
//    });
}

MainWindow::~MainWindow()
{
    delete ui;
    delete dlgErrorTip;
    delete g_Usart;
}

void MainWindow::Refresh_Progress_bar(uint8_t data)
{
    QCoreApplication::processEvents();
    ui->Progress_bar->setValue(data);
    ui->Progress_bar->update();
    this->update();
}
//设置主界面控件状态控制
void MainWindow::MainWindow_SetControl_Stake(bool state)
{
    ui->Btn_SetHome->setEnabled(state);
    ui->Btn_TeachHome->setEnabled(state);
    ui->Btn_AlarmHome->setEnabled(state);
    ui->Btn_MonitorHome->setEnabled(state);
    ui->Btn_ManualHome->setEnabled(state);

    ui->btnAdmin->setEnabled(state);
    ui->btnAlarm->setEnabled(state);
    ui->btnHandWheel->setEnabled(state);
    ui->btnHelp->setEnabled(state);

}
void MainWindow::PowerOnStateHandle()
{
    //开机不管三档开关位于那个位置,默认进入停止界面
    ui->Btn_ManualHome->click();
    g_Usart->ExtendSendProDeal(CMD_MAIN_PRO,CMD_SUN_PRO_MODE,2,0,0);
}
//初始化子界面函数
void MainWindow::slotShowSubWindow()
{
    qDebug() << "Setting—stact：" << timer.elapsed() << "毫秒";

    timer.restart();
    Refresh_Progress_bar(20);
    setWidget = new Setting(this);
    qDebug() << "Setting：" << timer.elapsed() << "毫秒";
    timer.restart();
    Refresh_Progress_bar(25);
    ui->stkWidget->addWidget(setWidget);
    qDebug() << "addWidget(setWidget)：" << timer.elapsed() << "毫秒";
    timer.restart();
    Refresh_Progress_bar(30);
    teachWidget = new Teach(this);
    qDebug() << "new Teach(this)" << timer.elapsed() << "毫秒";
    timer.restart();
    ui->stkWidget->addWidget(teachWidget);
    Refresh_Progress_bar(35);
    qDebug() << "addWidget(teachWidget)：" << timer.elapsed() << "毫秒";
    timer.restart();
    teachManageWidget = new TeachManage(this);
    ui->stkWidget->addWidget(teachManageWidget);
    Refresh_Progress_bar(40);
    qDebug() << "TeachManage：" << timer.elapsed() << "毫秒";
    timer.restart();
    autoWidget = new AutoForm(this);
    ui->stkWidget->addWidget(autoWidget);
    Refresh_Progress_bar(60);
    qDebug() << "AutoForm：" << timer.elapsed() << "毫秒";
    timer.restart();
    alarmWidget = new AlarmForm(this);
    ui->stkWidget->addWidget(alarmWidget);
    Refresh_Progress_bar(70);
    qDebug() << "AlarmForm：" << timer.elapsed() << "毫秒";
    timer.restart();
    monitorWidget = new MonitorForm(this);
    ui->stkWidget->addWidget(monitorWidget);
    Refresh_Progress_bar(80);
    qDebug() << "MonitorForm：" << timer.elapsed() << "毫秒";
    timer.restart();
    manualWidget = new ManualForm(this);
    ui->stkWidget->addWidget(manualWidget);
    Refresh_Progress_bar(90);
    qDebug() << "ManualForm：" << timer.elapsed() << "毫秒";

    timer.restart();
    connectAllSignalsAndSlots();
    ui->label_plan->hide();
    ui->Progress_bar->hide();
    ui->Progress_num->hide();
    FullKeyboard::instance(this)->hide();
    qDebug() << "FullKeyboard：" << timer.elapsed() << "毫秒";
    timer.restart();
    qDebug() << "APP启动耗时：" << timer1.elapsed() << "毫秒";
    dlgErrorTip = new ErrorTipDialog(this);

    handWheel = new HandWheelDialog(this);
    handWheel->hide();
    connect(ui->btnHandWheel, &QPushButton::clicked, handWheel, [=](){
        handWheel->exec();
    });

    QList<QLineEdit*> lineEdits = findChildren<QLineEdit*>();

    for (auto edit : lineEdits)
    {
        edit->installEventFilter(this);
    }

    QList<QTextEdit*> textEdits = findChildren<QTextEdit*>();
    for (auto textEdit : textEdits)
    {
        textEdit->installEventFilter(this);
    }

    for (auto cobox : findChildren<QComboBox*>())
    {
        cobox->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    }

    Refresh_Progress_bar(100);
    MainWindow_SetControl_Stake(true);
    scanner->start();
    PowerOnStateHandle();//开机默认进入停止界面
}

void MainWindow::startAllThread()
{
    t_SendCmdThread=new QThread;
    t_RecvCmdThread=new QThread;
    m_SendCmdThread=new SerialSendCmdThread;
    m_RecvCmdThread=new SerialRecvCmdThread;
    //分配线程
    m_SendCmdThread->moveToThread(t_SendCmdThread);
    m_RecvCmdThread->moveToThread(t_RecvCmdThread);

    connect(t_SendCmdThread,&QThread::started,m_SendCmdThread,&SerialSendCmdThread::serialSendCmd);
    connect(t_RecvCmdThread,&QThread::started,m_RecvCmdThread,&SerialRecvCmdThread::serialRecvCmd);

    t_SendCmdThread->start();
    t_RecvCmdThread->start();
}


void MainWindow::on_Btn_ManualHome_clicked()
{
    if (curMode == TriMode::STOP)
    {
        ui->Btn_ManualHome->setText(tr("停止页面"));
        ui->stkWidget->setCurrentWidget(ui->Init_page);
//        flag = 1;
    }
    // set to manual mode
    else if (curMode == TriMode::MANUAL)
    {
        ui->Btn_ManualHome->setText(tr("手动页面"));
        ui->labRotateSpeed->setNum(10);
        ui->stkWidget->setCurrentWidget(manualWidget);
        emit sigEnterManualPage();
//        flag = 2;
    }
    // set to auto mode
    else if (curMode == TriMode::AUTO)
    {
//        flag = 0;
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
    if (curMode == TriMode::MANUAL)
    {
        ui->stkWidget->setCurrentWidget(teachWidget);
        emit signal_refresh_TeachList();//发送刷新教导界面列表信号
    }
    else
    {
        ui->stkWidget->setCurrentWidget(teachManageWidget);
    }
}

void MainWindow::on_Btn_AlarmHome_clicked()
{

    // jump to alarm page
    ui->stkWidget->setCurrentWidget(alarmWidget);
}

int MainWindow::showErrorTip(const QString &message, TipMode mode)
{

    dlgErrorTip->setMode(mode);
    dlgErrorTip->setMessage(message);
    return dlgErrorTip->exec();
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
//    connect(ui->btnSoftKey, &QPushButton::clicked, this, [=](){
//        ui->wgtHelp->hide();
//    });

    connect(this, &MainWindow::sigSettingHome, setWidget, &Setting::slotSettingHome);

    connect(manualWidget, &ManualForm::sigShowStackPage, [=](){

        ui->stkWidget->setCurrentWidget(setWidget);
        setWidget->ShowStackPage();
    });
    /*各界面间互动信号槽连接*/
    connect(this,&MainWindow::signal_refresh_TeachList,teachWidget,&Teach::Teach_File_List_Refresh);//刷新教导管理程序显示列表信号
    connect(this,&MainWindow::EditOperatorVarPreOp_Refresh,teachWidget,&Teach::EditOperatorVarPreOp_handle);
    connect(setWidget,&Setting::LOGO_Refresh,this,[=](){ui->Init_page->setStyleSheet("QWidget { background-image: url(/root/stop.jpg); }");});
    connect(setWidget,&Setting::monitor_port_refreash,monitorWidget,&MonitorForm::InitAllLedName);//设置里修改端口名称后刷新监视界面端口名称
    //显示时间和刷新实时参数
    QTimer* timer = new QTimer(this);
	connect(timer, &QTimer::timeout, [&]() {
        static int timer_number = 0;
        if(timer_number == 10)
        {//每1s刷新一次
            // 获取当前时间
            QDateTime sysTime = QDateTime::currentDateTime();
            // 转换为字符串
            QString timeStr1 = sysTime.toString("yyyy/MM/dd");
            QString timeStr2 = sysTime.toString("hh:mm:ss");
            // 设置label显示的文本
            ui->labDateTime->setText(timeStr1);
            ui->labDateTime_2->setText(timeStr2);
        }
        ui->labSpeed->setText(QString::number(m_RunPar.globalSpeed)+"%");
        ui->labRotateSpeed->setText(QString::number((double)m_AxisCurSpeed)+"rpm");
        ui->X1_speed->setText(QString::number((double)((m_AxisCurPos[X1_AXIS]/m_AxisPar[X1_AXIS].circlePluseNum)*m_AxisPar[X1_AXIS].circleDis),'f',2)+"mm");
        ui->Y1_speed->setText(QString::number((double)((m_AxisCurPos[Y1_AXIS]/m_AxisPar[Y1_AXIS].circlePluseNum)*m_AxisPar[Y1_AXIS].circleDis),'f',2)+"mm");
        ui->Z1_speed->setText(QString::number((double)((m_AxisCurPos[Z1_AXIS]/m_AxisPar[Z1_AXIS].circlePluseNum)*m_AxisPar[Z1_AXIS].circleDis),'f',2)+"mm");
        ui->C_speed->setText(QString::number((double)((m_AxisCurPos[C_AXIS]/m_AxisPar[C_AXIS].circlePluseNum)*m_AxisPar[C_AXIS].circleDis),'f',2)+"mm");
        ui->Y2_speed->setText(QString::number((double)((m_AxisCurPos[Y2_AXIS]/m_AxisPar[Y2_AXIS].circlePluseNum)*m_AxisPar[Y2_AXIS].circleDis),'f',2)+"mm");
        ui->Z2_speed->setText(QString::number((double)((m_AxisCurPos[Z2_AXIS]/m_AxisPar[Z2_AXIS].circlePluseNum)*m_AxisPar[Z2_AXIS].circleDis),'f',2)+"mm");
		});
    // 启动定时器，设置时间间隔为100毫秒
    timer->start(100);
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
    QSet<QString> editsForKeyboard = {"editSystemName", "lineEdit_NowFileName", "editNoteTitle",
                                      "txt_label","label_Edit"};
    QSet<QString> editsIfCondition = {"editIfVar1Set", "editIfVar2Set"};
    QSet<QString> OperatorVarPreOp = {"EditOperatorVarPreOp"};
    QSet<QString> editReferPoint = {"editReferPoint"};


    // Handle line edits that require the full keyboard
    if (editsForKeyboard.contains(watched->objectName())) {
        QLineEdit* LineEdit = qobject_cast<QLineEdit*>(watched);
         QTextEdit* TextEdit = qobject_cast<QTextEdit*>(watched);
        if (LineEdit && LineEdit->isEnabled() && event->type() == QEvent::MouseButtonRelease) {
            callFullKeyboard(LineEdit);
        }
        if (TextEdit && event->type() == QEvent::MouseButtonRelease) {
            callFullKeyboard(TextEdit);
        }
        return false;
    }

    // Handle line edits for if condition dialogs
    if (editsIfCondition.contains(watched->objectName())) {
        QLineEdit* editIf = qobject_cast<QLineEdit*>(watched);
        if (editIf && editIf->isEnabled() && event->type() == QEvent::MouseButtonRelease) {
            IfProgramDialog dlg(this);
            if (dlg.exec() == QDialog::Accepted) {
                if("editIfVar1Set" == watched->objectName())
                {
                    dlg.SaveLogicIfStruct(0);
                }
                else if("editIfVar2Set" == watched->objectName())
                {
                    dlg.SaveLogicIfStruct(1);
                }
                QString expression = dlg.getConditionExpression();
                editIf->setText(expression);
            }
        }
        return false;
    }

    //变量类型弹窗
    if(OperatorVarPreOp.contains(watched->objectName()))
    {
        QLineEdit* editIf = qobject_cast<QLineEdit*>(watched);
        if (editIf && editIf->isEnabled() && event->type() == QEvent::MouseButtonRelease) {
            VarTypeDialog VarType_Dialog(this);
            VarType_Dialog.Refresh_VarTypeDialog();
            if (VarType_Dialog.exec() == QDialog::Accepted)
            {
                emit EditOperatorVarPreOp_Refresh();
            }
        }
        return false;
    }
    if(editReferPoint.contains(watched->objectName()))
    {
        RefPointEdit* editIf = qobject_cast<RefPointEdit*>(watched);
        if (editIf && editIf->isEnabled() && event->type() == QEvent::MouseButtonRelease) {
            ReferPointDialog dlg(this);
            auto manualWidget = qobject_cast<ManualForm*>(MainWindow::pMainWindow->findChild<ManualForm*>());
                dlg.updateUI(manualWidget->getRerferPoints());
                if (dlg.exec() == QDialog::Accepted)
                {
                    QString refPointName =  dlg.getSelectedRefPoint();
                    editIf->refPointIndex = dlg.getRefPointIndex();
                    editIf->setText(refPointName);

                }
        }
        return false;
    }

    // standard event processing
    return QWidget::eventFilter(watched, event);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (ui->wgtHelp->isVisible() && !ui->wgtHelp->geometry().contains(event->pos()))
    {
        ui->wgtHelp->hide();
    }
}

void MainWindow::callFullKeyboard(QObject *watched)
{
    QString originText = watched->property("text").toString();
    FullKeyboard* keyboard = FullKeyboard::instance();
    keyboard->setText(originText);
    keyboard->setCurrentEditObj(watched);
    //        keyboard->show();
    //        keyboard->raise();
    //        keyboard->activateWindow();
    keyboard->open();
    keyboard->raise();

}


void MainWindow::on_btnHelp_clicked()
{

//    QMessageBox::information(this, "tips", "this is test",
//                              QMessageBox::Yes | QMessageBox::No, QMessageBox::NoButton);

}
//三档开关处理函数
void MainWindow::TrimodeSwitchCommandSend(uint16_t code, int32_t value)
{
    switch (code) {
    case 142://手动
    {
        if(value == 1)
        {
            g_Usart->ExtendSendProDeal(CMD_MAIN_PRO,CMD_SUN_PRO_MODE,1,0,0);
        }
        break;
    }
    case 143://停止
    {
        if(value == 1)
        {
            g_Usart->ExtendSendProDeal(CMD_MAIN_PRO,CMD_SUN_PRO_MODE,2,0,0);
        }
        break;
    }
    case 144://自动
    {
        if(value == 1)
        {
            g_Usart->ExtendSendProDeal(CMD_MAIN_PRO,CMD_SUN_PRO_MODE,3,0,0);
        }
        break;
    }
    default:
        break;
    }
}
//左侧按键功能处理函数
void MainWindow::keyFunctCommandSend(uint16_t code, int32_t value)
{
    switch (code) {
    case 151://启动
    {
        if(value == 1)
        {
            g_Usart->ExtendSendProDeal(CMD_MAIN_PRO,CMD_SUN_PRO_START,1,1,10);
        }

        break;
    }
    case 150://停止
    {
        if(value == 1)
        {
            g_Usart->ExtendSendProDeal(CMD_MAIN_PRO,CMD_SUN_PRO_START,0,0,0);
        }

        break;
    }
    case 153://原点
    {
        if(value == 1)
        {
            g_Usart->ExtendSendProDeal(CMD_MAIN_PRO,CMD_SUN_PRO_START,3,1,50);
            int reply = showErrorTip(tr("回原点中..."),TipMode::NORMAL);
            if (reply == QDialog::Rejected)
            {

            }
        }

        break;
    }
    case 152://复归
    {
        if(value == 1)
        {
            int reply = showErrorTip(tr("是否现在复归?"),TipMode::NORMAL);
            if (reply == QDialog::Accepted)
            {
                g_Usart->ExtendSendProDeal(CMD_MAIN_PRO,CMD_SUN_PRO_START,4,1,50);
            }
        }

        break;
    }
    case 155://ok
    {
        break;
    }
    case 154://EXIT
    {
        break;
    }
    default:
        break;
    }
}
void MainWindow::keyAxisCommandSend(uint16_t code, int32_t value)
{
    uint16_t axisNum = 0;
    uint16_t direction = 0;
    switch (code)
    {
    case 158:
        axisNum=2;
        direction=1;
        break;
    case 170:
        axisNum=2;
        direction=2;
        break;
    case 162:
        axisNum=1;
        direction=1;
        break;
    case 166:
        axisNum=1;
        direction=2;
        break;
    case 157:
        axisNum=3;
        direction=1;
        break;
    case 161:
        axisNum=3;
        direction=2;
        break;
    case 165:
        axisNum=4;
        direction=1;
        break;
    case 169:
        axisNum=4;
        direction=2;
        break;
    case 156:
        axisNum=5;
        direction=1;
        break;
    case 168:
        axisNum=5;
        direction=2;
        break;
    case 160:
        axisNum=6;
        direction=1;
        break;
    case 164:
        axisNum=6;
        direction=2;
        break;
    }
    if(value == 0)
    {
        direction=0;
    }
    g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_AXIS,axisNum,direction);

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

