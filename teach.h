#ifndef TEACH_H
#define TEACH_H

#include <QWidget>
#include <QPushButton>
#include "./Prompt_Dialog_page/prompt_yes_no_dialog_page.h"
#include "ui_prompt_yes_no_dialog_page.h"

#include "vartypedialog.h"

extern uint16_t Pro_CurOpera_Line;								//当前选中行

namespace Ui {
class Teach;
}

class Teach : public QWidget
{
    Q_OBJECT

public:
    explicit Teach(QWidget *parent = nullptr);
    ~Teach();

private:
    Ui::Teach *ui;
    VarTypeDialog* varTypeDialog; // 变量类型弹窗

public slots:
    void on_btn_Tags_clicked();
    void on_btn_Logic_clicked();
    void on_btn_Search_clicked();
    void on_btn_Shift_clicked();
	void on_btn_TorqueProtection_clicked();
	void on_btn_ServoStop_clicked();
	void on_btn_Subroutine_clicked();

	void on_btn_General_AxisAction_clicked();
	void on_btn_General_ClawAction_clicked();
	void on_btn_General_SignalDetection_clicked();
	void on_btn_General_MachineTool_clicked();
	void on_btn_General_Stack_clicked();
	void on_btn_General_ReserveOutput_clicked();
	void on_btn_General_SpeedTeach_clicked();
	void on_btn_General_SignalWait_clicked();
	void on_btn_General_Other_clicked();

	void on_btn_General_1_clicked();
	void on_btn_General_2_clicked();
	void on_btn_General_3_clicked();
	void on_btn_General_4_clicked();

	void on_btn_General_PageTurn_clicked();

    void Teach_File_List_Refresh(void);



private slots:
    void on_chboxReturnStepNum_stateChanged(int arg1);   
    
    void on_btnAlarmLight_toggled(bool checked);

    void Pro_StackResetZeroStructInit(void);

    void Pro_MachineOutStructInit(void);

    void Pro_ClawActionStructInit(void);

    void Pro_ClawCheckStructInit(void);

    void Pro_ReserveCheckStructInit(void);

    void Pro_AxisActOrderInit(void);

    void on_btnDelete_clicked();

    void on_btnDevide_clicked();

    void on_btnCombine_clicked();

    void on_btnMoveUp_clicked();

    void on_btnMoveDown_clicked();

    void on_btnShield_clicked();

    void on_btnInset_clicked();

    void on_btnSave_clicked();

    void on_cb_General_X1_clicked(bool checked);

    void on_cb_General_Y1_clicked(bool checked);

    void on_cb_General_Z1_clicked(bool checked);

    void on_cb_General_Y2_clicked(bool checked);

    void on_cb_General_Z2_clicked(bool checked);

    void on_cb_General_C_clicked(bool checked);

    void on_cb_General_A_clicked(bool checked);

    void on_cb_General_B_clicked(bool checked);

    void on_lineEdit_General_Speed_X1_editingFinished();

    void on_lineEdit_General_Speed_Y1_editingFinished();

    void on_lineEdit_General_Speed_Z1_editingFinished();

    void on_lineEdit_General_Speed_Y2_editingFinished();

    void on_lineEdit_General_Speed_Z2_editingFinished();

    void on_lineEdit_General_Speed_B_editingFinished();

    void on_lineEdit_General_Speed_A_editingFinished();

    void on_lineEdit_General_Speed_C_editingFinished();

    void on_lineEdit_General_Position_X1_editingFinished();

    void on_lineEdit_General_Position_Y1_editingFinished();

    void on_lineEdit_General_Position_Z1_editingFinished();

    void on_lineEdit_General_Position_Y2_editingFinished();

    void on_lineEdit_General_Position_Z2_editingFinished();

    void on_btn_General_Refresh_2_clicked();

    void on_btn_General_Refresh_4_clicked();

    void on_btn_General_Refresh_3_clicked();


    void on_tabWidget_Reserve_currentChanged(int index);

    void on_btn_General_Refresh_5_clicked();

    void on_btn_General_Refresh_6_clicked();

    void on_btn_General_Refresh_7_clicked();

    void on_btn_General_Refresh_9_clicked();

    void on_btn_General_Refresh_8_clicked();

    void on_btn_General_Refresh_10_clicked();

    void on_Stack_group_1_clicked();

    void on_Stack_group_2_clicked();

    void on_Stack_group_3_clicked();

    void on_Stack_group_4_clicked();

    void on_Stack_group_5_clicked();

    void on_Stack_group_6_clicked();

    void on_Stack_group_7_clicked();

    void on_Stack_group_8_clicked();

    void on_Stack_group_9_clicked();

    void on_Stack_group_10_clicked();

    void on_Stack_group_11_clicked();

    void on_Follow_Stack_group_1_clicked();

    void on_Follow_Stack_group_2_clicked();

    void on_Follow_Stack_group_3_clicked();

    void on_Follow_Stack_group_4_clicked();

    void on_Stack_reset_clicked();

    void on_btnModify_clicked();

private:
    void init();
    void pageInit();

private:
	QPushButton* generalBtn[4];
	QString generalStr[4];
	QString generalIconPath[4];
	QStringList generalStrList;			//使用哪几个
	QStringList generalIconPathList;
    prompt_yes_no_Dialog_page *prompt_yes_no_Dialog_ui = new prompt_yes_no_Dialog_page;
private:
	void setGeneralBtnEvent(QString btnName);

    // QObject interface
public:
    bool eventFilter(QObject *watched, QEvent *event) override;
};

#endif // TEACH_H
