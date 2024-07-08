#ifndef TEACH_H
#define TEACH_H

#include <QWidget>
#include <QPushButton>

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

private:
	QPushButton* generalBtn[4];
	QString generalStr[4];
	QString generalIconPath[4];
	QStringList generalStrList;			//使用哪几个
	QStringList generalIconPathList;

private:
	void setGeneralBtnEvent(QString btnName);

};

#endif // TEACH_H
