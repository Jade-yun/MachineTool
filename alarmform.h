#ifndef ALARMFORM_H
#define ALARMFORM_H

#include <QWidget>

#include "maintaininfomodel.h"

namespace Ui {
class AlarmForm;
}

class AlarmForm : public QWidget
{
    Q_OBJECT

public:
    explicit AlarmForm(QWidget *parent = nullptr);

public:
    void handleAlarm(uint16_t alarmNum);
public slots:
    void SaveParLogHandle(uint16_t MainIndex,uint16_t SunIndex,int32_t oldData,int32_t newData);
private:
    void setupAlarmInfo();
    void addNewAlarmInfo();
    void saveAlarmQueueToConfig();
    void loadAlarmQueueFromConfig();

    void setupMaintainInfo();

    void retranslate();
private:
    Ui::AlarmForm *ui;

private:
    MaintainInfoModel *m_maintainModel;

protected:
    void changeEvent(QEvent *e) override;
};

#endif // ALARMFORM_H
