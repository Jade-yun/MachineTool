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
#include "logindialog.h"
#include "softkeywidget.h"
#include "backgroundprocessform.h"
#include "alarmbar.h"
#include "calculator.h"
#include "alarminfodialog.h"

#include "mythread.h"

#include "eventscanner.h"

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

    void handleAlarm(uint16_t alarmNum);

    void updatelabProgramName();

    void Widget_jump_Order_Need_Save_Handel();

    void m_RobotStateRefreash();

    void labProgramNameRollShow();

    void AxisTypeChange_Handle();

    void SettingParNeedSaveHandle();
public slots:

    void slotShowSubWindow();

    void MainWindow_SetControl_Stake(bool state);

signals:
    void sigSettingHome();
    void signal_refresh_TeachList();//刷新教导界面列表信号
    void EditOperatorVarPreOp_Refresh();//教导界面变量指令-变量类型刷新信号
    void signal_sync_data();       //同步数据到主控板
    void SetAutoRunIcon_Signal();//发送自动界面图标刷新信号
    void alarmOccurred(int alarmNum);
    void Auto_File_List_Refresh_signal(uint8_t ProNum);//刷新自动运行界面列表信号
    void monitor_hand_contril(uint16_t code, int32_t value);//监视界面手控器界面刷新
    void OrderSave_signal(bool SaveFlag);//发送保存程序指令 SaveFlag=false 不保存 SaveFlag = true 保存
    void signal_TeachPageInit();//教导界面初始化信号
    void HandWheelButtonClick_Signal();//手轮按钮按下触发更新手轮界面-手轮模式控件信号
    void RefreshHandWheel_Signal();//刷新手轮界面信号
    void Refresh_globalSpeedShow_Signal();//在自动界面时，通过按键修改速度时，发送刷新自动界面速度显示
    void SwitchPageParSaveSignal(ParSaveIndex Index);//切换界面时设置界面有参数需要保存信号
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

    // dialogs
    ErrorTipDialog* dlgErrorTip;
    LoginDialog* login;
    SoftKeyWidget* softKey;
    BackgroundProcessForm* backgroundProcess;
    Calculator* calculator;
    HandWheelDialog *handWheel;
    AlarmBar* alarmBar;

private:
    TriMode curMode;
    EventScanner* scanner; // customize input event monitoring
    QTimer* checkParaTimer;
    bool RobotResetPromptFlag=0;//复位弹窗标志 0-未弹窗 1-弹窗
private:
    void initUI();
    void handleLoginModeChanged(LoginMode mode);
    void onCheckPara();
    void AdjustSpeedHandel(uint8_t AdjustType);
public:
    static MainWindow* pMainWindow;

public:
    int showErrorTip(const QString& message, TipMode mode = TipMode::NORMAL);
//    void showErrorTip(const QString &message, bool onlyOK);

    void keyAxisCommandSend(uint16_t code, int32_t value);              //按键指令分类发送

    void keyFunctCommandSend(uint16_t code,int32_t value);

    void TrimodeSwitchCommandSend(uint16_t code, int32_t value);
private:
    void connectAllSignalsAndSlots();

    void setStyleFromFile(const QString &styleSheet);

    void PowerOnStateHandle();

    void DataSycStateHandel(uint8_t SysIndex);

    void posflashhandle(AxisCurPos data);
public:
    void callFullKeyboard(QObject *watched);

protected:
    bool eventFilter(QObject *,QEvent *) override;
    void mousePressEvent(QMouseEvent *event) override;
    void changeEvent(QEvent *e) override;

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

class TextTicker : public QLabel
{
    Q_OBJECT


public:
    TextTicker(QWidget *parent = nullptr);
    ~TextTicker();


protected:
    void paintEvent(QPaintEvent *);
    void updateIndex();

private:
    int m_charWidth; //字符串宽度
    int m_curIndex;
    QString m_showText; //需要显示的字符串
    QString calculateVisibleText();
public:

    QString getText()
    {
        return  m_showText;
    }
    // 设置滚动文本的内容
    void setText(const QString &text){
        QLabel::setText(text); // 调用基类的setText方法设置文本
        m_showText = text; // 更新内部存储的文本
        m_charWidth = fontMetrics().averageCharWidth(); // 重新计算字符宽度
        update(); // 触发重绘
    }
};

#endif // MAINWINDOW_H
