#ifndef TEACH_H
#define TEACH_H
#include <QListWidgetItem>
#include <QWidget>
#include <QPushButton>
#include <QCheckBox>
#include "stackedit.h"
#include "vartypedialog.h"
#include "cmd.h"
#include <QMouseEvent>
#include <QDialog>
namespace Ui {
class Teach;
}

class Teach : public QWidget
{
    Q_OBJECT

public:
    explicit Teach(QWidget *parent = nullptr);
    ~Teach();

    void Switch_Pro_ReadOrder(uint8_t ProNum);
private:
    Ui::Teach *ui;
    uint16_t listWgtJumptoLabelIndex = 0;//标签列表中有跳转标签序号 0-未选中，
public slots:
    void OrderColorShow_Handle();
    void Edit_ifOrder_Save_handle(P_LogicIfStruct* LogicIf,uint8_t IfIndex);
    void Edit_ifOrder_handle(P_LogicIfStruct* LogicIf,uint8_t IfIndex);
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

    void OrderEdit_Handle();

    void LogicVar_handle();

    void LogicAxis_handle();

    void LogicStack_handle();

    void LogicProt_handle();

    void chboxCondition1If2If_handle();

    void Stack_Edit_Handle(uint8_t AxisIndex);

    void EditOperatorVarPreOp_handle();

    void WidgetNameRefresh();
private slots:
    void SufferOperUpdata_Handel();

    void Save_Speed_Educat();

    void Search_Init(void);

    void SunPro_Init(void);

    void Wait_Signal_Init(void);

    void Pro_SpeedTeachInit(void);

    void Pro_ReserveOutStructInit(void);

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

    void on_btn_Material_clamp_1_clicked();

    void on_btn_Claw_roll_1_clicked();

    void on_btn_finish_product_1_clicked();


    void on_tabWidget_Reserve_currentChanged(int index);

    void on_Main_Axis_Rotate_button_clicked();

    void on_Main_Axis_locat_button_clicked();

    void on_Auto_Door_button_clicked();

    void on_Chuck_button_clicked();

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

    void on_Reserve_Pass_Button_clicked();

    void on_Reserve_Pass_Box_clicked();

    void on_Reserve_OutTime_Box_clicked();

    void on_Reserve_Overturn_Box_clicked();

    void on_editSpeedTeachX1_editingFinished();

    void on_editSpeedTeachY1_editingFinished();

    void on_editSpeedTeachZ1_editingFinished();

    void on_editSpeedTeachY2_editingFinished();

    void on_editSpeedTeachZ2_editingFinished();

    void on_editSpeedTeachC_editingFinished();

    void on_editSpeedTeachA_editingFinished();

    void on_editSpeedTeachB_editingFinished();

    void on_lineEdit_Reserve_num_editingFinished();

    void on_chboxReturnLabel_clicked();

    void on_chboxReturnStepNum_clicked();

    void on_editReturnStepNum_editingFinished();

    void on_btnAlarmLamp_button_clicked();

    void on_btnAlarmSound_button_clicked();

    void on_Start_Up_Mach_button_clicked();

    void on_Blow_Air_button_clicked();

    void on_lineEdit_General_Position_C_editingFinished();

    void on_lineEdit_General_Position_B_editingFinished();

    void on_lineEdit_General_Position_A_editingFinished();

    void on_Subroutine1_button_clicked();

    void on_Subroutine2_button_clicked();

    void on_Subroutine3_button_clicked();

    void on_Subroutine4_button_clicked();

    void on_Subroutine5_button_clicked();

    void on_Subroutine6_button_clicked();

    void on_Subroutine7_button_clicked();

    void on_Subroutine8_button_clicked();

    void on_tabWidget_Teach_tabBarClicked(int index);

    void on_cb_Standard_clicked();

    void on_cb_Automatic_clicked();

    void on_cb_ParallelOnce_clicked();

    void on_cb_WaitCompletion_clicked();

    void on_combo_SubroutineAll_currentIndexChanged(int index);

    void on_combo_ServoStop_AxisAll_currentIndexChanged(int index);

    void on_combo_LowSpeedTorque_AxisAll_currentIndexChanged(int index);

    void on_lineEdit_LowSpeedTorque_delay_editingFinished();

    void on_combo_Shift_AxisAll_currentIndexChanged(int index);

    void on_lineEdit_Shift_Position_editingFinished();

    void on_lineEdit_Shift_Speed_editingFinished();

    void on_editSearchPosition_editingFinished();

    void on_editSearchSpeed_editingFinished();

    void on_tabWidget_logic_tabBarClicked(int index);

    void on_btnModify_clicked();

    void on_cb_Edit_AxisMove_early_Slow_speed_clicked();

    void on_cb_Edit_AxisMove_early_acc_speed_clicked();

    void Edit_AxisMove_Save_handle(void);

    void Edit_ClawAction_handle();

    uint16_t GetXYPortIndex(P_ProOrderStruct OIS);

    void on_btnPilot_clicked();

    void on_chboxComment_clicked();

    void onCheckBoxToggled(QCheckBox *checkbox, bool checked);

    void Refresh_listWgtJumpto();

    void on_Stack_Edit_btnAxisX1_clicked();

    void on_Stack_Edit_btnAxisZ1_clicked();

    void on_Stack_Edit_btnAxisY1_clicked();

    void on_btn_General_Refresh_clicked();

    void on_btn_Refresh_clicked();

    void on_tableWgtTeach_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);

    void on_AxisEditRefresh_clicked();

    void on_Edit_AxisMove_Refresh_clicked();

private:
    void pageInit();
private:
	QPushButton* generalBtn[4];
	QString generalStr[4];
	QString generalIconPath[4];
	QStringList generalStrList;			//使用哪几个
	QStringList generalIconPathList;
    StackEdit* stack_Temp;
    BaseWindow *stack_Dialog;
    QTimer *Teach_timer;
    QColor randomColor;
    bool mouse;
    QPoint dragPosition;
signals:
    void StackEditBaseInit_signal();
private:
    QTableWidget *table;
    std::array<NumberEdit*, 3> nums; // 个数
    std::array<QTableWidgetItem*, 3> objPos; // 目标位置
private:
	void setGeneralBtnEvent(QString btnName);

};


#endif // TEACH_H
