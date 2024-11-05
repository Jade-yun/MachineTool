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

#include "mythread.h"

#include "eventscanner.h"
#include "beeper.h"

#include <QThread>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

enum class TriMode : unsigned char {

    STOP = 0,
    MANUAL,
    AUTO
};

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

    void Refresh_Progress_bar(uint8_t data);
public slots:

    void slotShowSubWindow();

    void MainWindow_SetControl_Stake(bool state);
signals:
    void sigSettingHome();
    void sigEnterManualPage();
    void sigLeaveManualPage();
    void signal_refresh_TeachList();//刷新教导界面列表信号
    void EditOperatorVarPreOp_Refresh();//教导界面变量指令-变量类型刷新信号

private:
    Ui::MainWindow *ui;

    QElapsedTimer timer,timer1;
    Setting *setWidget;
    Teach *teachWidget;
    AutoForm *autoWidget;
    AlarmForm *alarmWidget;
    MonitorForm *monitorWidget;
    ManualForm *manualWidget;
    TeachManage* teachManageWidget;

    ErrorTipDialog* dlgErrorTip;

private:
    TriMode curMode;
    EventScanner* scanner; // customize input event monitoring

public:
    static MainWindow* pMainWindow;
// dialogs
public:
    HandWheelDialog *handWheel;
    int showErrorTip(const QString& message, TipMode mode = TipMode::NORMAL);
//    void showErrorTip(const QString &message, bool onlyOK);

    void setSysProgramName(const QString& name);

    void keyAxisCommandSend(uint16_t code, int32_t value);              //按键指令分类发送

private:
    void connectAllSignalsAndSlots();

    void setStyleFromFile(const QString &styleSheet);


public:
    void callFullKeyboard(QObject *watched);
private slots:
    void on_btnHelp_clicked();
public slots:
    bool eventFilter(QObject *,QEvent *) override;
protected:
    void mousePressEvent(QMouseEvent *event) override;

    //子线程相关
public:
    QThread *t_SendCmdThread;
    QThread *t_RecvCmdThread;

    SerialSendCmdThread *m_SendCmdThread;
    SerialRecvCmdThread *m_RecvCmdThread;
    void startAllThread();

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
