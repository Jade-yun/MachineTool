#include "alarmform.h"
#include "ui_alarmform.h"
#include "RefreshKernelBuffer.h"
#include <QSettings>
#include <QDate>

#include "cmd.h"
#include "alarminfodialog.h"

#include "customkeyboard.h"
#include "errortipdialog.h"

AlarmForm::AlarmForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AlarmForm)
{
    ui->setupUi(this);

    setupAlarmInfo();
    setupMaintainInfo();

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
    g_SafeFileHandler->rotateBackups(alarmInfoDataPath);
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
    settings.sync();
    REFRESH_KERNEL_BUFFER(alarmInfoDataPath.toLocal8Bit().data());
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

void AlarmForm::setupAlarmInfo()
{
    loadAlarmQueueFromConfig();

    // style set, which is supposed to encapsulate
    ui->tableAlarmInfo->setColumnCount(3);
    ui->tableAlarmInfo->setRowCount(alarmInfoQueue.size());

    ui->tableAlarmInfo->horizontalHeader()->setVisible(true);
    ui->tableAlarmInfo->verticalHeader()->setVisible(true);
    ui->tableAlarmInfo->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableAlarmInfo->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableAlarmInfo->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableAlarmInfo->setColumnWidth(0, 100); // 第一列固定宽度
    ui->tableAlarmInfo->setColumnWidth(2, 300); // 第三列固定宽度
    ui->tableAlarmInfo->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

    ui->tableAlarmInfo->setHorizontalHeaderLabels({tr("报警编号"), tr("报警内容"), tr("报警时间")});

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

    connect(ui->btnClearAlarm, &QPushButton::clicked, [=](){
        if (alarmInfoQueue.isEmpty()) return ;

        ErrorTipDialog tip(tr("确定清除报警？"), nullptr);
        int reply = tip.exec();
        if (reply == QDialog::Accepted)
        {
            alarmInfoQueue.clear();
            QSettings settings(alarmInfoDataPath, QSettings::IniFormat);
            settings.clear();
            settings.sync();

            // fresh display
            ui->tableAlarmInfo->clear();
            ui->tableAlarmInfo->setRowCount(alarmInfoQueue.size());
        }
    });
}

void AlarmForm::setupMaintainInfo()
{
    m_maintainModel = new MaintainInfoModel(this);
    ui->tableMaintainInfo->setModel(m_maintainModel);
    m_maintainModel->loadFromConfigFile(maintainInfoDataPath);
    m_maintainModel->startPeriodicUpdate();

    QStringList headers = {tr("维护内容"), tr("维护周期"), tr("剩余天数"), tr("下次保养时间")};
    for (int i = 0; i < headers.size(); ++i) {
//        m_maintainModel->setHeaderData(i, Qt::Horizontal, headers[i], Qt::DisplayRole);
        m_maintainModel->setHorizontalHeader(i, headers[i]);
    }

    ui->tableMaintainInfo->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableMaintainInfo->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
//    ui->tableMaintainInfo->setSelectionBehavior(QAbstractItemView::SelectRows);
//    ui->tableMaintainInfo->setSelectionMode(QAbstractItemView::SingleSelection);

    ui->tableMaintainInfo->setColumnWidth(1, 100);
    ui->tableMaintainInfo->setColumnWidth(2, 100);
    ui->tableMaintainInfo->setColumnWidth(3, 300);
    ui->tableMaintainInfo->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);

    connect(ui->btnNewMaintainInfo, &QPushButton::clicked, this, [this]() {
        int rowCount = m_maintainModel->rowCount();
        m_maintainModel->addMaintainInfo(QString("untitled%1").arg(rowCount + 1), 30);
        m_maintainModel->saveToConfigFile(maintainInfoDataPath);
    });

    connect(ui->btnDeleteMaintainInfo, &QPushButton::clicked, this, [this]() {
        int selectedRow = ui->tableMaintainInfo->currentIndex().row();
        m_maintainModel->removeMaintainInfo(selectedRow);
        m_maintainModel->saveToConfigFile(maintainInfoDataPath);
    });

    connect(ui->btnEditMaintainInfo, &QPushButton::clicked, this, [this]() {
        auto index = ui->tableMaintainInfo->currentIndex();
//        int curRow = ui->tableMaintainInfo->currentIndex().row();
        int curCol = ui->tableMaintainInfo->currentIndex().column();
        if (curCol == 0)
        {
            FullKeyboard* keyboard = FullKeyboard::instance();
            keyboard->setText("");
            keyboard->setCurrentEditObj(nullptr);
            keyboard->exec();

            QString content = keyboard->getInputText();

            if (content.isNull()) return;

//            m_maintainModel->reviseMaintainContent(curRow, content);
            m_maintainModel->setData(index, content);

            m_maintainModel->saveToConfigFile(maintainInfoDataPath);
        }
        else if (curCol == 1)
        {
            NumKeyboard temp;
            auto keyboard = &temp;
            QLabel lab;
            keyboard->setCurrentEditObj(&lab);
            keyboard->exec();

            int maintainCycle = lab.text().toInt();
            if (maintainCycle == 0) return;

//            m_maintainModel->reviseMaintainCycle(curRow, maintainCycle);
            m_maintainModel->setData(index, maintainCycle);

            m_maintainModel->saveToConfigFile(maintainInfoDataPath);
        }


    });

    connect(ui->btnRetimingMaintainInfo, &QPushButton::clicked, this, [this]() {
        int selectedRow = ui->tableMaintainInfo->currentIndex().row();
        m_maintainModel->resetCountdown(selectedRow);

        m_maintainModel->saveToConfigFile(maintainInfoDataPath);
    });
}

void AlarmForm::retranslate()
{
    QStringList headers = {tr("维护内容"), tr("维护周期"), tr("剩余天数"), tr("下次保养时间")};
    for (int i = 0; i < headers.size(); ++i) {
//        m_maintainModel->setHeaderData(i, Qt::Horizontal, headers[i], Qt::DisplayRole);
        m_maintainModel->setHorizontalHeader(i, headers[i]);
    }
    ui->tableAlarmInfo->setHorizontalHeaderLabels({tr("报警编号"), tr("报警内容"), tr("报警时间")});
}

void AlarmForm::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
        this->retranslate();
    }
    QWidget::changeEvent(e);
}



