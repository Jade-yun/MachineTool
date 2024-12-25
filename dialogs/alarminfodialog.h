#ifndef ALARMINFODIALOG_H
#define ALARMINFODIALOG_H

#include <QDialog>

namespace Ui {
class AlarmInfoDialog;
}

class AlarmInfoDialog : public QDialog
{
    Q_OBJECT

public:
    static AlarmInfoDialog* instance();

    void showAlarmInfo(int alarmNum, const QString& alarmContent, const QString& alarmReason, const QString& solution);
    void showAlarmInfo(int alarmNum);


private:
    explicit AlarmInfoDialog(QWidget *parent = nullptr);
    ~AlarmInfoDialog();
    AlarmInfoDialog& operator=(const AlarmInfoDialog&) = delete;
    AlarmInfoDialog(const AlarmInfoDialog&) = delete;


private:
    Ui::AlarmInfoDialog *ui;


    // QWidget interface
protected:
    void showEvent(QShowEvent *event) override;
};

#endif // ALARMINFODIALOG_H
