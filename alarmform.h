#ifndef ALARMFORM_H
#define ALARMFORM_H

#include <QWidget>

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

private:
    void addNewAlarmInfo();
    void saveAlarmQueueToConfig();
    void loadAlarmQueueFromConfig();

    void setupAlarmTable();

//    void getAlarmInfo(int alarmNum, QString& alarmContent, )

private:
    Ui::AlarmForm *ui;
};

#endif // ALARMFORM_H
