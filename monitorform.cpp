#include "monitorform.h"
#include "ui_monitorform.h"

MonitorForm::MonitorForm(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::MonitorForm)
{
	ui->setupUi(this);

	ui->stackedWidget_main->setCurrentIndex(0);

	ui->stackedWidget1->setCurrentIndex(0);
	ui->stackedWidget2->setCurrentIndex(0);
	ui->stackedWidget3->setCurrentIndex(0);
	ui->stackedWidget4->setCurrentIndex(0);
	ui->stackedWidget12->setCurrentIndex(0);

	ui->tabWidgetSig->setCurrentIndex(0);
	ui->tabWidgetSig_2->setCurrentIndex(0);
	ui->tabWidgetSig_3->setCurrentIndex(0);
	

	//所有LED的label设置为指示灯
	QList<QLabel*> labels = this->findChildren<QLabel*>();
	for (auto label : labels)
	{
		if (label->text() == "LED")
		{
			setLED(label, 3, 30);
		}
	}
}

MonitorForm::~MonitorForm()
{
	delete ui;
}

void MonitorForm::setLED(QLabel* label, int color, int size)
{
	label->setText("");
	QString min_width = QString("min-width: %1px;").arg(size);              // 最小宽度：size
	QString min_height = QString("min-height: %1px;").arg(size);            // 最小高度：size
	QString max_width = QString("max-width: %1px;").arg(size);              // 最小宽度：size
	QString max_height = QString("max-height: %1px;").arg(size);            // 最小高度：size
	// 再设置边界形状及边框
	QString border_radius = QString("border-radius: %1px;").arg(size / 2);    // 边框是圆角，半径为size/2
	QString border = QString("border:0px solid black;");                    // 边框为1px黑色
	// 最后设置背景颜色
	QString background = "background-color:";
	switch (color) {
	case 1:
		// 红色
		background += "rgb(255,0,0)";
		break;
	case 2:
		// 绿色
		background += "rgb(0,255,0)";
		break;
	case 3:
	{
		// 渐变色

		background += QString("qlineargradient(x1: 0, y1 : 0, x2 : 0, y2 : 1, stop : 0 black, stop : 1 gray);");
		break;
	}
	default:
		break;
	}

	const QString SheetStyle = min_width + min_height + max_width + max_height + border_radius + border + background;
	label->setStyleSheet(SheetStyle);
}

void MonitorForm::on_btn1_cut_clicked()
{
	int curIndex = ui->stackedWidget1->currentIndex();
	if (curIndex == 0)
	{
		ui->stackedWidget1->setCurrentIndex(2);
		ui->lab_page1->setText("3/3");
	}
	else
	{
		ui->stackedWidget1->setCurrentIndex(curIndex - 1);
		ui->lab_page1->setText(QString("%1/3").arg(curIndex));
	}

}

void MonitorForm::on_btn1_add_clicked()
{
	int curIndex = ui->stackedWidget1->currentIndex();
	if (curIndex == 2)
	{
		ui->stackedWidget1->setCurrentIndex(0);
		ui->lab_page1->setText("1/3");
	}
	else
	{
		ui->stackedWidget1->setCurrentIndex(curIndex + 1);
		ui->lab_page1->setText(QString("%1/3").arg(curIndex + 2));
	}
}

void MonitorForm::on_btn2_cut_clicked()
{
	int curIndex = ui->stackedWidget2->currentIndex();
	if (curIndex == 0)
	{
		ui->stackedWidget2->setCurrentIndex(1);
		ui->lab_page2->setText("2/2");
	}
	else
	{
		ui->stackedWidget2->setCurrentIndex(curIndex - 1);
		ui->lab_page2->setText(QString("%1/2").arg(curIndex));
	}
}

void MonitorForm::on_btn2_add_clicked()
{
	int curIndex = ui->stackedWidget2->currentIndex();
	if (curIndex == 1)
	{
		ui->stackedWidget2->setCurrentIndex(0);
		ui->lab_page2->setText("1/2");
	}
	else
	{
		ui->stackedWidget2->setCurrentIndex(curIndex + 1);
		ui->lab_page2->setText(QString("%1/2").arg(curIndex + 2));
	}
}

void MonitorForm::on_btn3_cut_clicked()
{
	int curIndex = ui->stackedWidget3->currentIndex();
	if (curIndex == 0)
	{
		ui->stackedWidget3->setCurrentIndex(2);
		ui->lab_page3->setText("3/3");
	}
	else
	{
		ui->stackedWidget3->setCurrentIndex(curIndex - 1);
		ui->lab_page3->setText(QString("%1/3").arg(curIndex));
	}
}

void MonitorForm::on_btn3_add_clicked()
{
	int curIndex = ui->stackedWidget3->currentIndex();
	if (curIndex == 2)
	{
		ui->stackedWidget3->setCurrentIndex(0);
		ui->lab_page3->setText("1/3");
	}
	else
	{
		ui->stackedWidget3->setCurrentIndex(curIndex + 1);
		ui->lab_page3->setText(QString("%1/3").arg(curIndex + 2));
	}
}

void MonitorForm::on_btn4_cut_clicked()
{
	int curIndex = ui->stackedWidget4->currentIndex();
	if (curIndex == 0)
	{
		ui->stackedWidget4->setCurrentIndex(1);
		ui->lab_page4->setText("2/2");
	}
	else
	{
		ui->stackedWidget4->setCurrentIndex(curIndex - 1);
		ui->lab_page4->setText(QString("%1/2").arg(curIndex));
	}
}

void MonitorForm::on_btn4_add_clicked()
{
	int curIndex = ui->stackedWidget4->currentIndex();
	if (curIndex == 1)
	{
		ui->stackedWidget4->setCurrentIndex(0);
		ui->lab_page4->setText("1/2");
	}
	else
	{
		ui->stackedWidget4->setCurrentIndex(curIndex + 1);
		ui->lab_page4->setText(QString("%1/2").arg(curIndex + 2));
	}
}

void MonitorForm::on_btn12_cut_clicked()
{
	int curIndex = ui->stackedWidget12->currentIndex();
	if (curIndex == 0)
	{
		ui->stackedWidget12->setCurrentIndex(1);
		ui->lab_page12->setText("2/2");
	}
	else
	{
		ui->stackedWidget12->setCurrentIndex(curIndex - 1);
		ui->lab_page12->setText(QString("%1/2").arg(curIndex));
	}
}

void MonitorForm::on_btn12_add_clicked()
{
	int curIndex = ui->stackedWidget12->currentIndex();
	if (curIndex == 1)
	{
		ui->stackedWidget12->setCurrentIndex(0);
		ui->lab_page12->setText("1/2");
	}
	else
	{
		ui->stackedWidget12->setCurrentIndex(curIndex + 1);
		ui->lab_page12->setText(QString("%1/2").arg(curIndex + 2));
	}
}

void MonitorForm::on_btnMerge_Expand_clicked()
{
	int curIndex = ui->stackedWidget_main->currentIndex();
	if (curIndex == 0)
	{
        ui->btnMerge_Expand->setIcon(QIcon(":/images/monitorImages/monitor_merge.png"));
        ui->btnMerge_Expand->setIconSize(QSize(35, 35));
		ui->stackedWidget_main->setCurrentIndex(1);
	}
	else
	{
        ui->btnMerge_Expand->setIcon(QIcon(":/images/monitorImages/monitor_expand.png"));
        ui->btnMerge_Expand->setIconSize(QSize(35, 35));
		ui->stackedWidget_main->setCurrentIndex(0);
	}
}
