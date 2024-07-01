#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QTimer>
#include <QDateTime>
#include <QFile>

#pragma execution_character_set("utf-8")


int flag = 0;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle(QString());
    //this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);

    setAllStyleSheet();

    setWidget = new Setting(this);
    ui->stkWidget->insertWidget(0, setWidget);
    teachWidget = new Teach(this);
    ui->stkWidget->insertWidget(1, teachWidget);
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
    this->setStyleFromFile(":/styleSheets/style.qss");
    
}

MainWindow::~MainWindow()
{
    delete ui;
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
    ui->stkWidget->setCurrentWidget(teachWidget);
}

void MainWindow::on_Btn_AlarmHome_clicked()
{

    // jump to alarm page
    ui->stkWidget->setCurrentWidget(alarmWidget);
}

void MainWindow::setAllStyleSheet()
{
//    this->setStyleSheet("QLabel {\
//                        font-size: 25px; \
//                    }\
//                    QPushButton{ \
//                        font: 25px;\
//                    }\
//                    QTabBar::tab{\
//                        font:25px;\
//                    }");

	this->setStyleSheet("QMainWindow { background-color:qlineargradient(x1 : 0, y1 : 0, x2 : 0, y2 : 1, stop : 0 #00befa, stop: 1 #0160ea)}");
	ui->labDateTime->setStyleSheet("color: yellow;");

	ui->labDateTime_2->setStyleSheet("color: yellow;");

	ui->labSpeed->setStyleSheet("color: yellow;");

	ui->labRotateSpeed->setStyleSheet("color: yellow;");
}

void MainWindow::connectAllSignalsAndSlots()
{
    connect(this, &MainWindow::sigSettingHome, setWidget, &Setting::slotSettingHome);

	//ui->Btn_SetHome->setStyleSheet("QPushButton { background-color:#E7E8EB;border-style:solid;border-width:1px;border-color:#0A7C25;}");

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

void MainWindow::paintEvent(QPaintEvent *event)
{


}
