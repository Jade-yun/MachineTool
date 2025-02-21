#include "alarminfodialog.h"
#include "ui_alarminfodialog.h"

#include "framelesswidget2.h"

#include <QSettings>

const QString alarmInfoMappingPath = "/Settings/AlarmInfoMapping.ini";

AlarmInfoDialog::AlarmInfoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AlarmInfoDialog)
{
    ui->setupUi(this);

    FramelessWidget2 *framless = new FramelessWidget2(this);
    framless->setWidget(this);
    framless->setResizeEnable(false);

    this->setModal(true);

    ui->labAlarmContent->setWordWrap(true);
    ui->labAlarmContent->setAlignment(Qt::AlignVCenter);
    ui->labAlarmReason->setWordWrap(true);
    ui->labAlarmReason->setAlignment(Qt::AlignVCenter);
    ui->labSolution->setWordWrap(true);
    ui->labSolution->setAlignment(Qt::AlignVCenter);

    connect(ui->btnCancel, &QPushButton::clicked, this, &QDialog::reject);
}

AlarmInfoDialog::~AlarmInfoDialog()
{
    delete ui;
}

void AlarmInfoDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
//    showAlarmInfo(1000, "无", "五",
//                   "1.这是方案1。\n2.this is solution 2.\n3.this is solution 3.\n4.this is solution 4\n");
}

AlarmInfoDialog *AlarmInfoDialog::instance()
{
    static AlarmInfoDialog aInstance;
    return &aInstance;
}

void AlarmInfoDialog::showAlarmInfo(int alarmNum, const QString &alarmContent, const QString &alarmReason, const QString &solution)
{
    ui->labAlarmNum->setNum(alarmNum);
    ui->labAlarmContent->setText(alarmContent);
    ui->labAlarmReason->setText(alarmReason);
    ui->labSolution->setText(solution);

    this->show();
}

void AlarmInfoDialog::showAlarmInfo(int alarmNum)
{
    if (ui->labAlarmNum->text().toInt() != alarmNum)
    {

        QSettings alarmInfoSettings(alarmInfoMappingPath, QSettings::IniFormat);
        alarmInfoSettings.setIniCodec("utf-8");
        alarmInfoSettings.beginGroup(QString::number(alarmNum));
        QString alarmContent = alarmInfoSettings.value("AlarmContent").toString();
        QString alarmReason = alarmInfoSettings.value("AlarmReason").toString();
        QString solution = alarmInfoSettings.value("Solution").toString();
        alarmInfoSettings.endGroup();

        ui->labAlarmNum->setNum(alarmNum);
        ui->labAlarmContent->setText(alarmContent);
        ui->labAlarmReason->setText(alarmReason);
        ui->labSolution->setText(solution);
    }

    this->show();
}
