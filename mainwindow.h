#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "setting.h"
#include "teach.h"
#include "autoform.h"
#include "alarmform.h"
#include "monitorform.h"
#include "manualform.h"
#include "teachmanage.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_Btn_ManualHome_clicked();
    void on_Btn_SetHome_clicked();

    void on_Btn_MonitorHome_clicked();

    void on_Btn_TeachHome_clicked();

    void on_Btn_AlarmHome_clicked();

signals:
    void sigSettingHome();

private:
    Setting *setWidget;
    Teach *teachWidget;
    AutoForm *autoWidget;
    AlarmForm *alarmWidget;
    MonitorForm *monitorWidget;
    ManualForm *manualWidget;
    TeachManage* teachManageWidget;

private:
    Ui::MainWindow *ui;

private:
    void connectAllSignalsAndSlots();

    void setStyleFromFile(const QString &styleSheet);
private slots:


    // ultimize this interface to realize multi-language
protected:
    void paintEvent(QPaintEvent *event);
};
#endif // MAINWINDOW_H
