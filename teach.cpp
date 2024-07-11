#include "teach.h"
#include "ui_teach.h"

Teach::Teach(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::Teach)
{
	ui->setupUi(this);

	QTabBar* bar = ui->tabWidget_Teach->tabBar();
	connect(bar, &QTabBar::tabBarClicked, [&](int index) {
		if (index == 1)
		{
			ui->stackedWidget_Senior->setCurrentIndex(0);
		}
		else if (index == 2)
		{
			ui->stackedWidget_General->setCurrentIndex(0);
		}
		});

	ui->stackedWidget_Senior->setCurrentIndex(0);
	ui->stackedWidget_General->setCurrentIndex(0);

	generalBtn[0] = ui->btn_General_1;
	generalBtn[1] = ui->btn_General_2;
	generalBtn[2] = ui->btn_General_3;
	generalBtn[3] = ui->btn_General_4;

    generalStr[0] = QStringLiteral("轴");
    generalStr[1] = QStringLiteral("卡爪");
    generalStr[2] = QStringLiteral("检测");
    generalStr[3] = QStringLiteral("机床");

	generalIconPath[0] = ":/images/teach_img/axis.png";
	generalIconPath[1] = ":/images/teach_img/claw.png";
	generalIconPath[2] = ":/images/teach_img/signalCheck.png";
	generalIconPath[3] = ":/images/teach_img/machine.png";

}

Teach::~Teach()
{
	delete ui;
}

void Teach::on_btn_Tags_clicked()
{
	ui->stackedWidget_Senior->setCurrentIndex(1);
}

void Teach::on_btn_Logic_clicked()
{
	ui->stackedWidget_Senior->setCurrentIndex(2);
}

void Teach::on_btn_Search_clicked()
{
	ui->stackedWidget_Senior->setCurrentIndex(3);
}

void Teach::on_btn_Shift_clicked()
{
	ui->stackedWidget_Senior->setCurrentIndex(4);
}

void Teach::on_btn_TorqueProtection_clicked()
{
	ui->stackedWidget_Senior->setCurrentIndex(5);
}

void Teach::on_btn_ServoStop_clicked()
{
	ui->stackedWidget_Senior->setCurrentIndex(6);
}

void Teach::on_btn_Subroutine_clicked()
{
	ui->stackedWidget_Senior->setCurrentIndex(7);
}

void Teach::on_btn_General_AxisAction_clicked()
{
	ui->stackedWidget_General->setCurrentIndex(1);
	ui->stackedWidget_Four->setCurrentIndex(0);
	generalBtn[3]->setVisible(false);
	generalStrList.clear();
	generalIconPathList.clear();
	for (int i = 1; i < 4; i++)
	{
		generalStrList.append(generalStr[i]);
		generalIconPathList.append(generalIconPath[i]);
	}
	for (int i = 0; i < 3; i++)
	{
		generalBtn[i]->setText(generalStrList[i]);
		generalBtn[i]->setIcon(QIcon(generalIconPathList[i]));
	}
}

void Teach::on_btn_General_ClawAction_clicked()
{
	ui->stackedWidget_General->setCurrentIndex(1);
	ui->stackedWidget_Four->setCurrentIndex(1);
	generalBtn[3]->setVisible(false);
	generalStrList.clear();
	generalIconPathList.clear();
	for (int i = 2; i < 4; i++)
	{
		generalStrList.append(generalStr[i]);
		generalIconPathList.append(generalIconPath[i]);
	}
	generalStrList.append(generalStr[0]);
	generalIconPathList.append(generalIconPath[0]);
	for (int i = 0; i < 3; i++)
	{
		generalBtn[i]->setText(generalStrList[i]);
		generalBtn[i]->setIcon(QIcon(generalIconPathList[i]));
	}
}

void Teach::on_btn_General_SignalDetection_clicked()
{
	ui->stackedWidget_General->setCurrentIndex(1);
	ui->stackedWidget_Four->setCurrentIndex(2);
	generalBtn[3]->setVisible(false);
	generalStrList.clear();
	generalIconPathList.clear();
	generalStrList.append(generalStr[3]);
	generalIconPathList.append(generalIconPath[3]);
	for (int i = 0; i < 2; i++)
	{
		generalStrList.append(generalStr[i]);
		generalIconPathList.append(generalIconPath[i]);
	}
	for (int i = 0; i < 3; i++)
	{
		generalBtn[i]->setText(generalStrList[i]);
		generalBtn[i]->setIcon(QIcon(generalIconPathList[i]));
	}
}

void Teach::on_btn_General_MachineTool_clicked()
{
	ui->stackedWidget_General->setCurrentIndex(1);
	ui->stackedWidget_Four->setCurrentIndex(3);
	generalBtn[3]->setVisible(false);
	generalStrList.clear();
	generalIconPathList.clear();
	for (int i = 0; i < 3; i++)
	{
		generalStrList.append(generalStr[i]);
		generalIconPathList.append(generalIconPath[i]);
	}
	for (int i = 0; i < 3; i++)
	{
		generalBtn[i]->setText(generalStrList[i]);
		generalBtn[i]->setIcon(QIcon(generalIconPathList[i]));
	}
}

void Teach::on_btn_General_Stack_clicked()
{
	ui->stackedWidget_General->setCurrentIndex(2);
}

void Teach::on_btn_General_ReserveOutput_clicked()
{
	ui->stackedWidget_General->setCurrentIndex(3);
}

void Teach::on_btn_General_SpeedTeach_clicked()
{
	ui->stackedWidget_General->setCurrentIndex(4);
}

void Teach::on_btn_General_SignalWait_clicked()
{
	ui->stackedWidget_General->setCurrentIndex(5);
}

void Teach::on_btn_General_Other_clicked()
{
	ui->stackedWidget_General->setCurrentIndex(1);
	ui->stackedWidget_Four->setCurrentIndex(4);
	generalBtn[3]->setVisible(true);
	for (int i = 0; i < 4; i++)
	{
		generalBtn[i]->setText(generalStr[i]);
		generalBtn[i]->setIcon(QIcon(generalIconPath[i]));
	}
}

void Teach::on_btn_General_1_clicked()
{
	setGeneralBtnEvent(ui->btn_General_1->text());
}

void Teach::on_btn_General_2_clicked()
{
	setGeneralBtnEvent(ui->btn_General_2->text());
}

void Teach::on_btn_General_3_clicked()
{
	setGeneralBtnEvent(ui->btn_General_3->text());
}

void Teach::on_btn_General_4_clicked()
{
	setGeneralBtnEvent(ui->btn_General_4->text());
}

void Teach::on_btn_General_PageTurn_clicked()
{
	int index = ui->stackedWidget_Axis->currentIndex();
	if (index == 0)
	{
		ui->stackedWidget_Axis->setCurrentIndex(1);
		ui->btn_General_PageTurn->setIcon(QIcon(":/images/switch_left.png"));
	}
	else
	{
		ui->stackedWidget_Axis->setCurrentIndex(0);
		ui->btn_General_PageTurn->setIcon(QIcon(":/images/switch_right.png"));
	}
}

void Teach::setGeneralBtnEvent(QString btnName)
{
	int index = 0;
	for (int i = 0; i < 4; i++)
	{
		if (btnName == generalStr[i])
		{
			index = i;
			break;
		}
	}
	switch (index)
	{
	case 0:
		on_btn_General_AxisAction_clicked();
		break;
	case 1:
		on_btn_General_ClawAction_clicked();
		break;
	case 2:
		on_btn_General_SignalDetection_clicked();
		break;
	case 3:
		on_btn_General_MachineTool_clicked();
		break;
	}
}
