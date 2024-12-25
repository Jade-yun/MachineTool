#include "alarmform.h"
#include "ui_alarmform.h"

#include <QSettings>

#include "cmd.h"
#include "alarminfodialog.h"

const QString alarmInfoMappingPath = "/Settings/AlarmInfoMapping.ini";
const QString alarmInfoDataPath = "/Settings/AlarmInfoData.ini";

AlarmForm::AlarmForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AlarmForm)
{
    ui->setupUi(this);

    loadAlarmQueueFromConfig();

    setupAlarmTable();

}

void AlarmForm::addNewAlarmInfo()
{
    if (alarmInfoQueue.isEmpty())
        return;

    auto newAlarmInfo = alarmInfoQueue.back();
    int newAlarmNum = newAlarmInfo.alarmNum;
    QString newAlarmTime = newAlarmInfo.alarmTime;

    if (ui->tableAlarmInfo->rowCount() > 0)
    {
        QTableWidgetItem* firstItem = ui->tableAlarmInfo->item(0, 0);
        if (firstItem && firstItem->text().toInt() == newAlarmNum)
        {
            return;
        }
    }

    QSettings alarmInfoSettings(alarmInfoMappingPath, QSettings::IniFormat);
    alarmInfoSettings.setIniCodec("utf-8");
    alarmInfoSettings.beginGroup(QString::number(newAlarmNum));
    QString alarmContent = alarmInfoSettings.value("AlarmContent").toString();
    alarmInfoSettings.endGroup();

    ui->tableAlarmInfo->insertRow(0);

    QTableWidgetItem* numItem = new QTableWidgetItem(QString::number(newAlarmNum));
    ui->tableAlarmInfo->setItem(0, 0, numItem);

    QTableWidgetItem* infoItem = new QTableWidgetItem(alarmContent);
    ui->tableAlarmInfo->setItem(0, 1, infoItem);

    QTableWidgetItem* timeItem = new QTableWidgetItem(newAlarmTime);
    ui->tableAlarmInfo->setItem(0, 2, timeItem);

    if (ui->tableAlarmInfo->rowCount() > ALARM_MAX_SIZE)
    {
        ui->tableAlarmInfo->removeRow(ui->tableAlarmInfo->rowCount() - 1);
    }
}

void AlarmForm::handleAlarm(uint16_t alarmNum)
{
    saveAlarmQueueToConfig();
    addNewAlarmInfo();
}

void AlarmForm::saveAlarmQueueToConfig()
{
    QSettings settings(alarmInfoDataPath, QSettings::IniFormat);
    settings.remove("");

    settings.beginWriteArray("AlarmInfoQueue");
    for (int i = 0; i < alarmInfoQueue.size(); ++i)
    {
        settings.setArrayIndex(i);
        settings.setValue("AlarmNum", alarmInfoQueue.at(i).alarmNum);
        settings.setValue("AlarmTime", alarmInfoQueue.at(i).alarmTime);
    }
    settings.endArray();
}

void AlarmForm::loadAlarmQueueFromConfig()
{
    QSettings settings(alarmInfoDataPath, QSettings::IniFormat);

    int size = settings.beginReadArray("AlarmInfoQueue");
    for (int i = 0; i < size; ++i)
    {
        settings.setArrayIndex(i);
        uint16_t alarmNum = settings.value("AlarmNum").toInt();
        QString alarmTime = settings.value("AlarmTime").toString();
        alarmInfoQueue.enqueue({alarmNum, alarmTime});
    }
    settings.endArray();
}

void AlarmForm::setupAlarmTable()
{
    // style set, which is supposed to encapsulate
    ui->tableAlarmInfo->horizontalHeader()->setVisible(true);
    ui->tableAlarmInfo->verticalHeader()->setVisible(true);
    ui->tableAlarmInfo->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableAlarmInfo->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableAlarmInfo->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableAlarmInfo->setColumnWidth(0, 100); // 第一列固定宽度
    ui->tableAlarmInfo->setColumnWidth(2, 300); // 第三列固定宽度
    ui->tableAlarmInfo->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

    ui->tableAlarmInfo->setRowCount(alarmInfoQueue.size());

    if (!alarmInfoQueue.isEmpty())
    {
        QSettings alarmInfoSettings(alarmInfoMappingPath, QSettings::IniFormat);
        alarmInfoSettings.setIniCodec("utf-8");

        // 遍历队列，依次添加到表格
        for (int i = 0; i < alarmInfoQueue.size(); ++i)
        {
            const auto& alarmInfo = alarmInfoQueue[alarmInfoQueue.size() - 1 - i];
            int alarmNum = alarmInfo.alarmNum;
            QString alarmTime = alarmInfo.alarmTime;
            alarmInfoSettings.beginGroup(QString::number(alarmNum));
            QString alarmContent = alarmInfoSettings.value("AlarmContent").toString();
            alarmInfoSettings.endGroup();

            QTableWidgetItem* numItem = new QTableWidgetItem(QString::number(alarmNum));
            ui->tableAlarmInfo->setItem(i, 0, numItem);

            QTableWidgetItem* infoItem = new QTableWidgetItem(alarmContent);
            ui->tableAlarmInfo->setItem(i, 1, infoItem);

            QTableWidgetItem* timeItem = new QTableWidgetItem(alarmTime);
            ui->tableAlarmInfo->setItem(i, 2, timeItem);
        }
    }


    connect(ui->tableAlarmInfo, &QTableWidget::cellClicked, this, [=](int row, int col){

        auto item = ui->tableAlarmInfo->item(row, 0);
        if (!item) return;

        auto alarmNum = item->text().toUInt();

        AlarmInfoDialog::instance()->showAlarmInfo(alarmNum);
    });
}
