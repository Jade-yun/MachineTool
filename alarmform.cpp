#include "alarmform.h"
#include "ui_alarmform.h"

AlarmForm::AlarmForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AlarmForm)
{
    ui->setupUi(this);

    // style set, which is supposed to encapsulate
    ui->tableAlarmInfo->setColumnWidth(0, 700);
    ui->tableAlarmInfo->setColumnWidth(1, 200);

}

AlarmForm::~AlarmForm()
{
    delete ui;
}
