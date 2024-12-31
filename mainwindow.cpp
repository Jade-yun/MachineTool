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
#include <QEasingCurve>

#include "beeper.h"
#include "ledcontroller.h"

#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>

const QString alarmInfoMappingPath = "/Settings/AlarmInfoMapping.ini";

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
    ui->Progress_bar->setWindowModality(Qt::WindowModality::WindowModal);

    ui->Init_page->setStyleSheet("QWidget { background-image: url(/root/stop.jpg); }");

    MainWindow_SetControl_Stake(false);//先禁用按钮，待初始化成功后才能点击

    ui->wgtHelp->setVisible(false);
    //开机读取配置文件信息，暂时做测试用，后续整理
    getOrderjoinIni();
    readIniProgramStruct();
    readSigSetPara();
    readIniPara();

    // set fonts for whole app
    const std::vector<QString> fontType = {
        "WenQuanYi Zen Hei",            // 默认字体
        "SimSun",                       // 宋体
        "KaiTi",                        // 楷体
        "DejaVu Sans"
    };
    QFont font("SimSun");
    const std::vector<int> fontSize = {
        20, // default
        15, 16, 17, 18, 19, 20, 21, 22
    };
    font.setPixelSize(fontSize[m_SystemSet.wordSize]);
    qApp->setFont(font);

    // after reading all init parameters, set the style for whole app.
    const std::vector<QString> styles = {
        ":/styleSheets/style.qss",
        "/Settings/style/style_orange_color.qss",
        "/Settings/style/style_yellow_color.qss",
        "/Settings/style/style_green_color.qss",
        "/Settings/style/style_brown_color.qss"
    };

    if (m_SystemSet.sysColor < styles.size())
    {
        setStyleFromFile(styles[m_SystemSet.sysColor]);
    }
    else
    {
        setStyleFromFile(styles[0]);
    }


//    setWindowTitle(QString());
//    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);
//    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);


    softKey = new SoftKeyWidget(this);
    connect(ui->btnSoftKey, &QPushButton::clicked, [=](){
        ui->wgtHelp->hide();
        softKey->show();
    });

    calculator = new Calculator(this);
    calculator->resize(800, 480);
    calculator->close();
    calculator->SetAnimationCurve(QEasingCurve::OutQuart);

    connect(ui->btnCaculator, &QPushButton::clicked, this, [=](){
        ui->wgtHelp->hide();
//        calculator->show();
        QPoint center = this->geometry().center();
        QPoint endPos(center.x() - calculator->width() / 2,
                     center.y() - calculator->height() / 2);
        calculator->StartAnimation(QPoint(endPos.x(), -this->height()), endPos, 1000, true);
        calculator->raise();
    });
    connect(calculator, &Calculator::signalBackHome, this, [=](){
//        qDebug() << "close...";
        calculator->hide();
//        calculator->StartAnimation(calculator->pos(), QPoint(0, -this->height()), 100, false);
    });

    backgroundProcess = new BackgroundProcessForm(this);
    backgroundProcess->hide();
    connect(ui->btnBackgroundProgram, &QPushButton::clicked, [=](){
        ui->wgtHelp->hide();
        backgroundProcess->show();
    });

    login = new LoginDialog(this);
    connect(ui->btnAdmin, &QPushButton::clicked, [=](){
        login->exec();
    });

    connect(login, &LoginDialog::loginModeChanged, this, [=](LoginMode mode){
        qDebug() << "switch to " << (int)mode ;
        handleLoginModeChanged(mode);
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
            if (m_manualAxis.handwheelMode)
            {
                g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_AXIS,2);
            }

        }
        // 逆时针旋转
        else if (code == 0 && value == -1)
        {
            // to do...
            if (m_manualAxis.handwheelMode)
            {
                g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_AXIS,1);
            }
        }
    });
    connect(scanner, &EventScanner::eventTrimodeSwitch, [this](uint16_t code, int32_t value) {
        qDebug() << "Trimode Switch event:" << code << value;
        if (code == 142 && value == 1)
        {
            // stop mode
            curMode = TriMode::STOP;
            bool enabled = LoginDialog::getLoginMode() != LoginMode::Operator;
            ui->Btn_TeachHome->setEnabled(enabled);
            ui->Btn_TeachHome->setText(tr("教导管理"));
            ui->btnHandWheel->setAttribute(Qt::WA_TransparentForMouseEvents, true);
            TrimodeSwitchCommandSend(code,value);
        }
        else if (code == 143 && value == 1)
        {
            // switch to manual mode
            curMode = TriMode::MANUAL;
            ui->Btn_TeachHome->setText(tr("教导"));
            ui->btnHandWheel->setAttribute(Qt::WA_TransparentForMouseEvents, false);
            TrimodeSwitchCommandSend(code,value);
        }
        else if (code == 144 && value == 1)
        {
            // switch to automatic mode
            curMode = TriMode::AUTO;
            ui->Btn_TeachHome->setEnabled(false);
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

    checkParaTimer = new QTimer(this);
    checkParaTimer->setInterval(100);
    connect(checkParaTimer, &QTimer::timeout, this, &MainWindow::onCheckPara);
    connect(this, &MainWindow::alarmOccurred, this, &MainWindow::handleAlarm);

    alarmBar = new AlarmBar(this);
//    alarmBar->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
//    alarmBar->setAttribute(Qt::WA_ShowWithoutActivating);
    alarmBar->hide();
    connect(ui->btnAlarm, &QPushButton::clicked, this, [=](){
        alarmBar->setGeometry(5, 640, 1014, 60);

        int newAlarmNum = alarmInfoQueue.back().alarmNum;

        QSettings alarmInfoSettings(alarmInfoMappingPath, QSettings::IniFormat);
        alarmInfoSettings.setIniCodec("utf-8");
        alarmInfoSettings.beginGroup(QString::number(newAlarmNum));
        QString alarmContent = alarmInfoSettings.value("AlarmContent").toString();
        alarmInfoSettings.endGroup();

        alarmBar->showAlarm(newAlarmNum, alarmContent);
    });

}

MainWindow::~MainWindow()
{
    delete ui;
    delete dlgErrorTip;
//    delete backgroundProcess;
    delete g_Usart;

    delete scanner;
}

void MainWindow::Refresh_Progress_bar(uint8_t data)
{
    QCoreApplication::processEvents();
    ui->Progress_bar->setValue(data);
    ui->Progress_bar->update();
}

void MainWindow::handleAlarm(uint16_t alarmNum)
{
    if (alarmInfoQueue.size() >= ALARM_MAX_SIZE)
    {
        alarmInfoQueue.dequeue();
    }
    QString alarmTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

    alarmInfoQueue.enqueue({alarmNum, alarmTime});

    alarmWidget->handleAlarm(alarmNum);

}
//设置主界面控件状态控制
void MainWindow::MainWindow_SetControl_Stake(bool state)
{
    ui->Btn_SetHome->setAttribute(Qt::WA_TransparentForMouseEvents, !state);
    ui->Btn_TeachHome->setAttribute(Qt::WA_TransparentForMouseEvents, !state);
    ui->Btn_AlarmHome->setAttribute(Qt::WA_TransparentForMouseEvents, !state);
    ui->Btn_MonitorHome->setAttribute(Qt::WA_TransparentForMouseEvents, !state);
    ui->Btn_ManualHome->setAttribute(Qt::WA_TransparentForMouseEvents, !state);

    ui->btnAdmin->setAttribute(Qt::WA_TransparentForMouseEvents, !state);
    ui->btnAlarm->setAttribute(Qt::WA_TransparentForMouseEvents, !state);
    ui->btnHandWheel->setAttribute(Qt::WA_TransparentForMouseEvents, !state);
    ui->btnHelp->setAttribute(Qt::WA_TransparentForMouseEvents, !state);
}

void MainWindow::initUI()
{
    emit login->loginModeChanged(LoginMode::Operator);

    ui->Btn_TeachHome->setText(tr("教导管理"));
    ui->Btn_ManualHome->setText(tr("停止页面"));
}

void MainWindow::handleLoginModeChanged(LoginMode mode)
{
    bool enabled = mode != LoginMode::Operator;

    ui->Btn_TeachHome->setEnabled(enabled && (curMode != TriMode::AUTO));

//    setWidget->handleLoginModeChanged(mode);
    manualWidget->handleLoginModeChanged(mode);
}

void MainWindow::onCheckPara()
{
    static int lastAlarmNum = 0;

    if (lastAlarmNum != m_AlarmNum)
    {
        lastAlarmNum = m_AlarmNum;
        // new alarm occurs
        if (lastAlarmNum)
        {
            emit alarmOccurred(lastAlarmNum);
            ui->btnAlarm->setAttribute(Qt::WA_TransparentForMouseEvents, false);
        }
        // alarm release
        else
        {
            ui->btnAlarm->setAttribute(Qt::WA_TransparentForMouseEvents, true);
        }
    }

    // LED0~5 的使能状态码分别分为0～8位对应位置1
    // 如 LED0 使能 0x01, LED3 使能 0x04
    // 最终控制多个 LED 需要多个状态码进行 |（按位或）操作
    uint8_t ledStatus = 0;
    for (int i = 0; i < OPR_LED_NUM; i++)
    {
        if (m_LedFunc[i].funcNum)
        {
            auto temp = m_LedFunc[i].ledType ? m_InPortSta[m_LedFunc[i].funcNum - 1] : m_OutPortSta[m_LedFunc[i].funcNum - 1];
            ledStatus |= (temp & 1) << i;
        }
    }

    LEDController::instance()->updateLEDStatus(&ledStatus);

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
    scanner->start();
    PowerOnStateHandle();//开机默认进入停止界面
    ui->label_plan->setText("参数同步中:");
    Refresh_Progress_bar(0);
    emit signal_sync_data();

    checkParaTimer->start();

    initUI();
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
//        flag = 2;
    }
    // set to auto mode
    else if (curMode == TriMode::AUTO)
    {
//        flag = 0;
        ui->Btn_ManualHome->setText(tr("自动页面"));
        ui->stkWidget->setCurrentWidget(autoWidget);
        emit Auto_File_List_Refresh_signal(0);//切换到自动界面时默认显示主程序
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
        ui->wgtHelp->raise();
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
    connect(setWidget,&Setting::RefreshPortDefineSignals,setWidget,&Setting::RefreshPortDefine);
    connect(g_Usart,&Usart::DataSycStateSignal,this,&MainWindow::DataSycStateHandel);//开机参数同步失败处理
    connect(this,&MainWindow::signal_sync_data,g_Usart,&Usart::sync_data_handle);//同步参数下发信号
    connect(autoWidget,&AutoForm::Send_planProductNum_Signal,this,[=](){//计划产品个数下发
        g_Usart->ExtendSendProDeal(CMD_MAIN_STA,CMD_SUN_STA_PAR,0,0,0);
    });
    connect(setWidget, &Setting::refreshManualReserve, manualWidget, &ManualForm::updateReserveButtonState);
    connect(setWidget, &Setting::sysNameChanged, this, [=](const QString& sysName){
        const QString& programName = m_CurrentProgramNameAndPath.fileName;

        QString name = sysName + "  " + programName;
        ui->labProgramName->setText(name);
    });
    connect(g_Usart,&Usart::posflashsignal,this,&MainWindow::posflashhandle);//当前坐标实时刷新
    connect(this,&MainWindow::Auto_File_List_Refresh_signal,autoWidget,&AutoForm::Auto_File_List_Refresh);//自动运行界面列表刷新
    connect(autoWidget,&AutoForm::Switch_ProNum_Signal,teachWidget,&Teach::Switch_Pro_ReadOrder);//自动界面切换程序编号
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

        /*自动界面内容刷新*/
        autoWidget->AutoForm_Refresh();
    });
    // 启动定时器，设置时间间隔为100毫秒
    timer->start(100);
}
//刷新实时坐标处理函数，通过信号传值的方式，避免发生线程访问安全
void MainWindow::posflashhandle(AxisCurPos data)
{
    ui->X1_speed->setText(QString::number((double)(((double)data.Pos_x/(double)m_AxisPar[X1_AXIS].circlePluseNum)*(double)m_AxisPar[X1_AXIS].circleDis/100),'f',2)+"mm");
    ui->Y1_speed->setText(QString::number((double)(((double)data.Pos_y/(double)m_AxisPar[Y1_AXIS].circlePluseNum)*(double)m_AxisPar[Y1_AXIS].circleDis/100),'f',2)+"mm");
    ui->Z1_speed->setText(QString::number((double)(((double)data.Pos_z/(double)m_AxisPar[Z1_AXIS].circlePluseNum)*(double)m_AxisPar[Z1_AXIS].circleDis/100),'f',2)+"mm");
    ui->C_speed->setText(QString::number((double)(((double)data.Pos_c/(double)m_AxisPar[C_AXIS].circlePluseNum)*(double)m_AxisPar[C_AXIS].circleDis/100),'f',2)+"mm");
    ui->Y2_speed->setText(QString::number((double)(((double)data.Pos_y2/(double)m_AxisPar[Y2_AXIS].circlePluseNum)*(double)m_AxisPar[Y2_AXIS].circleDis/100),'f',2)+"mm");
    ui->Z2_speed->setText(QString::number((double)(((double)data.Pos_z2/(double)m_AxisPar[Z2_AXIS].circlePluseNum)*(double)m_AxisPar[Z2_AXIS].circleDis/100),'f',2)+"mm");
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
            g_Usart->ExtendSendProDeal(CMD_MAIN_PRO,CMD_SUN_PRO_START,1,1,m_RunPar.globalSpeed);
        }

        break;
    }
    case 150://停止
    {
        if(value == 1)
        {
            g_Usart->ExtendSendProDeal(CMD_MAIN_PRO,CMD_SUN_PRO_START,0,1,0);
        }

        break;
    }
    case 153://原点
    {
        if(value == 1)
        {
            g_Usart->ExtendSendProDeal(CMD_MAIN_PRO,CMD_SUN_PRO_START,3,1,m_RunPar.globalSpeed);
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
                g_Usart->ExtendSendProDeal(CMD_MAIN_PRO,CMD_SUN_PRO_START,4,1,m_RunPar.globalSpeed);
            }
        }

        break;
    }
    case 155://ok
    {
//        showErrorTip(tr("参数同步中...."),TipMode::NORMAL);
//        emit signal_sync_data();
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
//开机同步参数处理函数，SysIndex:同步到那一步
void MainWindow::DataSycStateHandel(uint8_t SysIndex)
{
    if(SysIndex!=SysSendIndex::CMD_FINISH && SysIndex!=SysSendIndex::CMD_SENDERROR)
    {
//        QString SysText = tr("正在同步参数，参数同步编号：")+QString::number(SysIndex);
//        dlgErrorTip->setMessage(SysText);
//        if(!dlgErrorTip->isVisible())
//        {
//            showErrorTip(SysText,TipMode::NORMAL);
//        }
        ui->Progress_bar->setValue((100/30)*SysIndex);
    }
    else if(SysIndex==SysSendIndex::CMD_SENDERROR)
    {
        QString errorText = tr("参数同步失败，同步失败编号：")+QString::number(MySync_Data.SendDataStep);
        dlgErrorTip->setMessage(errorText);
        if(!dlgErrorTip->isVisible())
        {
            showErrorTip(errorText,TipMode::NORMAL);
        }
        MainWindow_SetControl_Stake(true);
        ui->label_plan->hide();
        ui->Progress_bar->hide();
        ui->Progress_num->hide();
        Load_Program_Handle(readPowerOnReadOneProInfo().fileName);//加载上次程序信息
    }
    else
    {
        Refresh_Progress_bar(100);
        MainWindow_SetControl_Stake(true);
        ui->label_plan->hide();
        ui->Progress_bar->hide();
        ui->Progress_num->hide();
        if(dlgErrorTip->isVisible())
        {
            dlgErrorTip->reject();
        }
        Load_Program_Handle(readPowerOnReadOneProInfo().fileName);//加载上次程序信息
    }

}
