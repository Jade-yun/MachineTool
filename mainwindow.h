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

// dialogs
public:
    HandWheelDialog *handWheel;
private:
    Ui::MainWindow *ui;

//    FullKeyboard* keyboard;

private:
    void connectAllSignalsAndSlots();

    void setStyleFromFile(const QString &styleSheet);

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
