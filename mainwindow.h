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

#include "customkeyboard.h"
#include "handwheeldialog.h"
#include "errortipdialog.h"
#include "ifprogramdialog.h"

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
    void signal_refresh_TeachList();//刷新教导界面列表信号

private:
    Setting *setWidget;
    Teach *teachWidget;
    AutoForm *autoWidget;
    AlarmForm *alarmWidget;
    MonitorForm *monitorWidget;
    ManualForm *manualWidget;
    TeachManage* teachManageWidget;

    ErrorTipDialog* dlgErrorTip;
public:
    static MainWindow* pMainWindow;
// dialogs
public:
    HandWheelDialog *handWheel;
    void showErrorTip(const QString& message);

    void setSysProgramName(const QString& name);
private:
    Ui::MainWindow *ui;

private:
    void connectAllSignalsAndSlots();

    void setStyleFromFile(const QString &styleSheet);
public:
    void callFullKeyboard(QObject *watched);
private slots:
    void on_btnHelp_clicked();
public slots:
    bool eventFilter(QObject *,QEvent *) override;
};

class ClickableLabel :public QLabel
{
    Q_OBJECT
public:
    ClickableLabel(QWidget *parent = nullptr);
    ClickableLabel(const QString& text, QWidget *parent = nullptr);
    ~ClickableLabel();


public:
    bool isChecked() const;
    void setChecked(bool _checked);

signals:
    void toggled(bool checked);
    void clicked();

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    bool checked;


};

#endif // MAINWINDOW_H
