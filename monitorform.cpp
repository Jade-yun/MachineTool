#include "monitorform.h"
#include "ui_monitorform.h"
#include "cmd.h"

MonitorForm::MonitorForm(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::MonitorForm)
{
	ui->setupUi(this);

    initVariable();

	ui->stackedWidget_main->setCurrentIndex(0);

    ui->left_tabWidget->setCurrentIndex(0);
    ui->left_InWidget->setCurrentIndex(0);
    ui->left_OutWidget->setCurrentIndex(0);
    ui->right_tabWidget->setCurrentIndex(0);
    ui->right_InWidget->setCurrentIndex(0);
    ui->right_OutWidget->setCurrentIndex(0);

    ui->single_tabWidget->setCurrentIndex(0);


	//所有LED的label设置为指示灯
	QList<QLabel*> labels = this->findChildren<QLabel*>();
	for (auto label : labels)
	{
		if (label->text() == "LED")
		{
            setLED(label, 0, 30);
		}
	}
    InitAllLedName();//设置所有led灯的名称
    SetHandControlName();
    RefreshLedTime = new QTimer;
    RefreshLedTime->setInterval(200);//100毫秒刷新一次
    connect(RefreshLedTime,&QTimer::timeout,this,&MonitorForm::RefreshLedState);
    RefreshLedTime->start();
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
    case 0:
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
    int curIndex = ui->left_InWidget->currentIndex();
    if (curIndex == 0)
    {
        ui->left_InWidget->setCurrentIndex(3);
        ui->lab_page1->setText("4/4");
    }
    else
    {
        ui->left_InWidget->setCurrentIndex(curIndex - 1);
        ui->lab_page1->setText(QString("%1/4").arg(curIndex));
    }

}

void MonitorForm::on_btn1_add_clicked()
{
    int curIndex = ui->left_InWidget->currentIndex();
    if (curIndex == 3)
    {
        ui->left_InWidget->setCurrentIndex(0);
        ui->lab_page1->setText("1/4");
    }
    else
    {
        ui->left_InWidget->setCurrentIndex(curIndex + 1);
        ui->lab_page1->setText(QString("%1/4").arg(curIndex + 2));
    }
}

void MonitorForm::on_btn2_cut_clicked()
{
    int curIndex = ui->left_OutWidget->currentIndex();
    if (curIndex == 0)
    {
        ui->left_OutWidget->setCurrentIndex(2);
        ui->lab_page2->setText("3/3");
    }
    else
    {
        ui->left_OutWidget->setCurrentIndex(curIndex - 1);
        ui->lab_page2->setText(QString("%1/3").arg(curIndex));
    }
}

void MonitorForm::on_btn2_add_clicked()
{
    int curIndex = ui->left_OutWidget->currentIndex();
    if (curIndex == 2)
    {
        ui->left_OutWidget->setCurrentIndex(0);
        ui->lab_page2->setText("1/3");
    }
    else
    {
        ui->left_OutWidget->setCurrentIndex(curIndex + 1);
        ui->lab_page2->setText(QString("%1/3").arg(curIndex + 2));
    }
}

void MonitorForm::on_btn1_cut_2_clicked()
{
    int curIndex = ui->right_InWidget->currentIndex();
    if (curIndex == 0)
    {
        ui->right_InWidget->setCurrentIndex(3);
        ui->lab_page1_2->setText("4/4");
    }
    else
    {
        ui->right_InWidget->setCurrentIndex(curIndex - 1);
        ui->lab_page1_2->setText(QString("%1/4").arg(curIndex));
    }
}

void MonitorForm::on_btn1_add_2_clicked()
{
    int curIndex = ui->right_InWidget->currentIndex();
    if (curIndex == 3)
    {
        ui->right_InWidget->setCurrentIndex(0);
        ui->lab_page1_2->setText("1/4");
    }
    else
    {
        ui->right_InWidget->setCurrentIndex(curIndex + 1);
        ui->lab_page1_2->setText(QString("%1/4").arg(curIndex + 2));
    }
}

void MonitorForm::on_btn2_cut_2_clicked()
{
    int curIndex = ui->right_OutWidget->currentIndex();
    if (curIndex == 0)
    {
        ui->right_OutWidget->setCurrentIndex(2);
        ui->lab_page2_2->setText("3/3");
    }
    else
    {
        ui->right_OutWidget->setCurrentIndex(curIndex - 1);
        ui->lab_page2_2->setText(QString("%1/3").arg(curIndex));
    }
}

void MonitorForm::on_btn2_add_2_clicked()
{
    int curIndex = ui->right_OutWidget->currentIndex();
    if (curIndex == 2)
    {
        ui->right_OutWidget->setCurrentIndex(0);
        ui->lab_page2_2->setText("1/3");
    }
    else
    {
        ui->right_OutWidget->setCurrentIndex(curIndex + 1);
        ui->lab_page2_2->setText(QString("%1/3").arg(curIndex + 2));
    }
}

void MonitorForm::on_btnInWIdget_cut_clicked()
{
    int curIndex = ui->InWidget->currentIndex();
    if (curIndex == 0)
    {
        ui->InWidget->setCurrentIndex(1);
        ui->lab_InWIdget_index->setText("2/2");
    }
    else
    {
        ui->InWidget->setCurrentIndex(curIndex - 1);
        ui->lab_InWIdget_index->setText(QString("%1/2").arg(curIndex));
    }
}

void MonitorForm::on_btnInWIdget_add_clicked()
{
    int curIndex = ui->InWidget->currentIndex();
    if (curIndex == 1)
    {
        ui->InWidget->setCurrentIndex(0);
        ui->lab_InWIdget_index->setText("1/2");
    }
    else
    {
        ui->InWidget->setCurrentIndex(curIndex + 1);
        ui->lab_InWIdget_index->setText(QString("%1/2").arg(curIndex + 2));
    }
}

void MonitorForm::on_btnOutWidget_cut_clicked()
{
    int curIndex = ui->OutWidgetWidget->currentIndex();
    if (curIndex == 0)
    {
        ui->OutWidgetWidget->setCurrentIndex(1);
        ui->lab_OutWidget_Index->setText("2/2");
    }
    else
    {
        ui->OutWidgetWidget->setCurrentIndex(curIndex - 1);
        ui->lab_OutWidget_Index->setText(QString("%1/2").arg(curIndex));
    }
}

void MonitorForm::on_btnOutWidget_add_clicked()
{
    int curIndex = ui->OutWidgetWidget->currentIndex();
    if (curIndex == 1)
    {
        ui->OutWidgetWidget->setCurrentIndex(0);
    ui->lab_OutWidget_Index->setText("1/2");
    }
    else
    {
        ui->OutWidgetWidget->setCurrentIndex(curIndex + 1);
        ui->lab_OutWidget_Index->setText(QString("%1/2").arg(curIndex + 2));
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
/*************************************************************************
**  函数名：  RefreshLedState()
**	输入参数：WidgetIndex:分屏显示还是单屏显示 0:双屏,1:单屏 interface:刷新那个界面控件0:双屏模式下左侧显示框控件,1:双屏模式下右侧显示框控件,  单屏模式该参数无效
**	输出参数：
**	函数功能：属新led状态
**  作者：    wukui
**  开发日期：2024/11/6
**************************************************************************/
void MonitorForm::RefreshLedState()
{
    switch (ui->stackedWidget_main->currentIndex()) {
    case 0://双屏模式
    {
        //左侧
        if(ui->left_tabWidget->currentWidget() == ui->tabDoubleValve)
        {
            if(ui->left_InWidget->currentIndex()==0)
            {
                setLedState(ui->X1_LED,1,0);
                setLedState(ui->X2_LED,2,0);
                setLedState(ui->X3_LED,3,0);
                setLedState(ui->X4_LED,4,0);
                setLedState(ui->X5_LED,5,0);
                setLedState(ui->X6_LED,6,0);
                setLedState(ui->X7_LED,7,0);
                setLedState(ui->X8_LED,8,0);
                setLedState(ui->X9_LED,9,0);
                setLedState(ui->X10_LED,10,0);
                setLedState(ui->X11_LED,11,0);
                setLedState(ui->X12_LED,12,0);
                setLedState(ui->X13_LED,13,0);
                setLedState(ui->X14_LED,14,0);
                setLedState(ui->X15_LED,15,0);
                setLedState(ui->X16_LED,16,0);
            }
            else if(ui->left_InWidget->currentIndex()==1)
            {
                setLedState(ui->X17_LED,17,0);
                setLedState(ui->X18_LED,18,0);
                setLedState(ui->X19_LED,19,0);
                setLedState(ui->X20_LED,20,0);
                setLedState(ui->X21_LED,21,0);
                setLedState(ui->X22_LED,22,0);
                setLedState(ui->X23_LED,23,0);
                setLedState(ui->X24_LED,24,0);
                setLedState(ui->X25_LED,25,0);
                setLedState(ui->X26_LED,26,0);
                setLedState(ui->X27_LED,27,0);
                setLedState(ui->X28_LED,28,0);
                setLedState(ui->X29_LED,29,0);
                setLedState(ui->X30_LED,30,0);
                setLedState(ui->X31_LED,31,0);
                setLedState(ui->X32_LED,32,0);
            }
            else if(ui->left_InWidget->currentIndex()==2)
            {
                setLedState(ui->X33_LED,33,0);
                setLedState(ui->X34_LED,34,0);
                setLedState(ui->X35_LED,35,0);
                setLedState(ui->X36_LED,36,0);
                setLedState(ui->EX1_LED,37,0);
                setLedState(ui->EX2_LED,38,0);
                setLedState(ui->EX3_LED,39,0);
                setLedState(ui->EX4_LED,40,0);
                setLedState(ui->EX5_LED,41,0);
                setLedState(ui->EX6_LED,42,0);
                setLedState(ui->EX7_LED,43,0);
                setLedState(ui->EX8_LED,44,0);
                setLedState(ui->EX9_LED,45,0);
                setLedState(ui->EX10_LED,46,0);
                setLedState(ui->EX11_LED,47,0);
                setLedState(ui->EX12_LED,48,0);

            }
            else if(ui->left_InWidget->currentIndex()==3)
            {
                setLedState(ui->EX13_LED,49,0);
                setLedState(ui->EX14_LED,50,0);
                setLedState(ui->EX15_LED,51,0);
                setLedState(ui->EX16_LED,52,0);
                setLedState(ui->EX17_LED,53,0);
                setLedState(ui->EX18_LED,54,0);
                setLedState(ui->EX19_LED,55,0);
                setLedState(ui->EX20_LED,56,0);
                setLedState(ui->EX21_LED,57,0);
                setLedState(ui->EX22_LED,58,0);
                setLedState(ui->EX23_LED,59,0);
                setLedState(ui->EX24_LED,60,0);
            }

        }
        else if(ui->left_tabWidget->currentWidget() == ui->tabOutputType)
        {
            if(ui->left_OutWidget->currentIndex()==0)
            {
                setLedState(ui->Y1_LED,1,1);
                setLedState(ui->Y2_LED,2,1);
                setLedState(ui->Y3_LED,3,1);
                setLedState(ui->Y4_LED,4,1);
                setLedState(ui->Y5_LED,5,1);
                setLedState(ui->Y6_LED,6,1);
                setLedState(ui->Y7_LED,7,1);
                setLedState(ui->Y8_LED,8,1);
                setLedState(ui->Y9_LED,9,1);
                setLedState(ui->Y10_LED,10,1);
                setLedState(ui->Y11_LED,11,1);
                setLedState(ui->Y12_LED,12,1);
                setLedState(ui->Y13_LED,13,1);
                setLedState(ui->Y14_LED,14,1);
                setLedState(ui->Y15_LED,15,1);
                setLedState(ui->Y16_LED,16,1);
            }
            else if(ui->left_OutWidget->currentIndex()==1)
            {
                setLedState(ui->Y17_LED,17,1);
                setLedState(ui->Y18_LED,18,1);
                setLedState(ui->Y19_LED,19,1);
                setLedState(ui->Y20_LED,20,1);
                setLedState(ui->Y21_LED,21,1);
                setLedState(ui->Y22_LED,22,1);
                setLedState(ui->Y23_LED,23,1);
                setLedState(ui->Y24_LED,24,1);
                setLedState(ui->EY1_LED,25,1);
                setLedState(ui->EY2_LED,26,1);
                setLedState(ui->EY3_LED,27,1);
                setLedState(ui->EY4_LED,28,1);
                setLedState(ui->EY5_LED,29,1);
                setLedState(ui->EY6_LED,30,1);
                setLedState(ui->EY7_LED,31,1);
                setLedState(ui->EY8_LED,32,1);
            }
            else if(ui->left_OutWidget->currentIndex()==2)
            {
                setLedState(ui->EY9_LED,33,1);
                setLedState(ui->EY10_LED,34,1);
                setLedState(ui->EY11_LED,35,1);
                setLedState(ui->EY12_LED,36,1);
                setLedState(ui->EY13_LED,37,1);
                setLedState(ui->EY14_LED,38,1);
                setLedState(ui->EY15_LED,39,1);
                setLedState(ui->EY16_LED,40,1);
                setLedState(ui->EY17_LED,41,1);
                setLedState(ui->EY18_LED,42,1);
                setLedState(ui->EY19_LED,43,1);
                setLedState(ui->EY20_LED,44,1);

            }
        }
        else if(ui->left_tabWidget->currentWidget() == ui->tabPortCustomize)
        {

        }
        //右侧
        if(ui->right_tabWidget->currentWidget() == ui->tabDoubleValve_2)
        {
            if(ui->right_InWidget->currentIndex()==0)
            {
                setLedState(ui->X1_LED_2,1,0);
                setLedState(ui->X2_LED_2,2,0);
                setLedState(ui->X3_LED_2,3,0);
                setLedState(ui->X4_LED_2,4,0);
                setLedState(ui->X5_LED_2,5,0);
                setLedState(ui->X6_LED_2,6,0);
                setLedState(ui->X7_LED_2,7,0);
                setLedState(ui->X8_LED_2,8,0);
                setLedState(ui->X9_LED_2,9,0);
                setLedState(ui->X10_LED_2,10,0);
                setLedState(ui->X11_LED_2,11,0);
                setLedState(ui->X12_LED_2,12,0);
                setLedState(ui->X13_LED_2,13,0);
                setLedState(ui->X14_LED_2,14,0);
                setLedState(ui->X15_LED_2,15,0);
                setLedState(ui->X16_LED_2,16,0);
            }
            else if(ui->right_InWidget->currentIndex()==1)
            {
                setLedState(ui->X17_LED_2,17,0);
                setLedState(ui->X18_LED_2,18,0);
                setLedState(ui->X19_LED_2,19,0);
                setLedState(ui->X20_LED_2,20,0);
                setLedState(ui->X21_LED_2,21,0);
                setLedState(ui->X22_LED_2,22,0);
                setLedState(ui->X23_LED_2,23,0);
                setLedState(ui->X24_LED_2,24,0);
                setLedState(ui->X25_LED_2,25,0);
                setLedState(ui->X26_LED_2,26,0);
                setLedState(ui->X27_LED_2,27,0);
                setLedState(ui->X28_LED_2,28,0);
                setLedState(ui->X29_LED_2,29,0);
                setLedState(ui->X30_LED_2,30,0);
                setLedState(ui->X31_LED_2,31,0);
                setLedState(ui->X32_LED_2,32,0);
            }
            else if(ui->right_InWidget->currentIndex()==2)
            {
                setLedState(ui->X33_LED_2,33,0);
                setLedState(ui->X34_LED_2,34,0);
                setLedState(ui->X35_LED_2,35,0);
                setLedState(ui->X36_LED_2,36,0);
                setLedState(ui->EX1_LED_2,37,0);
                setLedState(ui->EX2_LED_2,38,0);
                setLedState(ui->EX3_LED_2,39,0);
                setLedState(ui->EX4_LED_2,40,0);
                setLedState(ui->EX5_LED_2,41,0);
                setLedState(ui->EX6_LED_2,42,0);
                setLedState(ui->EX7_LED_2,43,0);
                setLedState(ui->EX8_LED_2,44,0);
                setLedState(ui->EX9_LED_2,45,0);
                setLedState(ui->EX10_LED_2,46,0);
                setLedState(ui->EX11_LED_2,47,0);
                setLedState(ui->EX12_LED_2,48,0);

            }
            else if(ui->right_InWidget->currentIndex()==3)
            {
                setLedState(ui->EX13_LED_2,49,0);
                setLedState(ui->EX14_LED_2,50,0);
                setLedState(ui->EX15_LED_2,51,0);
                setLedState(ui->EX16_LED_2,52,0);
                setLedState(ui->EX17_LED_2,53,0);
                setLedState(ui->EX18_LED_2,54,0);
                setLedState(ui->EX19_LED_2,55,0);
                setLedState(ui->EX20_LED_2,56,0);
                setLedState(ui->EX21_LED_2,57,0);
                setLedState(ui->EX22_LED_2,58,0);
                setLedState(ui->EX23_LED_2,59,0);
                setLedState(ui->EX24_LED_2,60,0);
            }

        }
        else if(ui->right_tabWidget->currentWidget() == ui->tabOutputType_2)
        {
            if(ui->right_OutWidget->currentIndex()==0)
            {
                setLedState(ui->Y1_LED_2,1,1);
                setLedState(ui->Y2_LED_2,2,1);
                setLedState(ui->Y3_LED_2,3,1);
                setLedState(ui->Y4_LED_2,4,1);
                setLedState(ui->Y5_LED_2,5,1);
                setLedState(ui->Y6_LED_2,6,1);
                setLedState(ui->Y7_LED_2,7,1);
                setLedState(ui->Y8_LED_2,8,1);
                setLedState(ui->Y9_LED_2,9,1);
                setLedState(ui->Y10_LED_2,10,1);
                setLedState(ui->Y11_LED_2,11,1);
                setLedState(ui->Y12_LED_2,12,1);
                setLedState(ui->Y13_LED_2,13,1);
                setLedState(ui->Y14_LED_2,14,1);
                setLedState(ui->Y15_LED_2,15,1);
                setLedState(ui->Y16_LED_2,16,1);
            }
            else if(ui->right_OutWidget->currentIndex()==1)
            {
                setLedState(ui->Y17_LED_2,17,1);
                setLedState(ui->Y18_LED_2,18,1);
                setLedState(ui->Y19_LED_2,19,1);
                setLedState(ui->Y20_LED_2,20,1);
                setLedState(ui->Y21_LED_2,21,1);
                setLedState(ui->Y22_LED_2,22,1);
                setLedState(ui->Y23_LED_2,23,1);
                setLedState(ui->Y24_LED_2,24,1);
                setLedState(ui->EY1_LED_2,25,1);
                setLedState(ui->EY2_LED_2,26,1);
                setLedState(ui->EY3_LED_2,27,1);
                setLedState(ui->EY4_LED_2,28,1);
                setLedState(ui->EY5_LED_2,29,1);
                setLedState(ui->EY6_LED_2,30,1);
                setLedState(ui->EY7_LED_2,31,1);
                setLedState(ui->EY8_LED_2,32,1);
            }
            else if(ui->right_OutWidget->currentIndex()==2)
            {
                setLedState(ui->EY9_LED_2,33,1);
                setLedState(ui->EY10_LED_2,34,1);
                setLedState(ui->EY11_LED_2,35,1);
                setLedState(ui->EY12_LED_2,36,1);
                setLedState(ui->EY13_LED_2,37,1);
                setLedState(ui->EY14_LED_2,38,1);
                setLedState(ui->EY15_LED_2,39,1);
                setLedState(ui->EY16_LED_2,40,1);
                setLedState(ui->EY17_LED_2,41,1);
                setLedState(ui->EY18_LED_2,42,1);
                setLedState(ui->EY19_LED_2,43,1);
                setLedState(ui->EY20_LED_2,44,1);

            }
        }
        else if(ui->right_tabWidget->currentWidget() == ui->tabPortCustomize_2)
        {

        }
        break;
    }
    case 1://单屏模式
    {
        if(ui->single_tabWidget->currentWidget() == ui->tabDoubleValve_3)
        {
            if(ui->InWidget->currentIndex()==0)
            {
                setLedState(ui->X1_LED_3,1,0);
                setLedState(ui->X2_LED_3,2,0);
                setLedState(ui->X3_LED_3,3,0);
                setLedState(ui->X4_LED_3,4,0);
                setLedState(ui->X5_LED_3,5,0);
                setLedState(ui->X6_LED_3,6,0);
                setLedState(ui->X7_LED_3,7,0);
                setLedState(ui->X8_LED_3,8,0);
                setLedState(ui->X9_LED_3,9,0);
                setLedState(ui->X10_LED_3,10,0);
                setLedState(ui->X11_LED_3,11,0);
                setLedState(ui->X12_LED_3,12,0);
                setLedState(ui->X13_LED_3,13,0);
                setLedState(ui->X14_LED_3,14,0);
                setLedState(ui->X15_LED_3,15,0);
                setLedState(ui->X16_LED_3,16,0);
                setLedState(ui->X17_LED_3,17,0);
                setLedState(ui->X18_LED_3,18,0);
                setLedState(ui->X19_LED_3,19,0);
                setLedState(ui->X20_LED_3,20,0);
                setLedState(ui->X21_LED_3,21,0);
                setLedState(ui->X22_LED_3,22,0);
                setLedState(ui->X23_LED_3,23,0);
                setLedState(ui->X24_LED_3,24,0);
                setLedState(ui->X25_LED_3,25,0);
                setLedState(ui->X26_LED_3,26,0);
                setLedState(ui->X27_LED_3,27,0);
                setLedState(ui->X28_LED_3,28,0);
                setLedState(ui->X29_LED_3,29,0);
                setLedState(ui->X30_LED_3,30,0);
                setLedState(ui->X31_LED_3,31,0);
                setLedState(ui->X32_LED_3,32,0);
            }
            else if(ui->InWidget->currentIndex()==1)
            {
                setLedState(ui->X33_LED_3,33,0);
                setLedState(ui->X34_LED_3,34,0);
                setLedState(ui->X35_LED_3,35,0);
                setLedState(ui->X36_LED_3,36,0);
                setLedState(ui->EX1_LED_3,37,0);
                setLedState(ui->EX2_LED_3,38,0);
                setLedState(ui->EX3_LED_3,39,0);
                setLedState(ui->EX4_LED_3,40,0);
                setLedState(ui->EX5_LED_3,41,0);
                setLedState(ui->EX6_LED_3,42,0);
                setLedState(ui->EX7_LED_3,43,0);
                setLedState(ui->EX8_LED_3,44,0);
                setLedState(ui->EX9_LED_3,45,0);
                setLedState(ui->EX10_LED_3,46,0);
                setLedState(ui->EX11_LED_3,47,0);
                setLedState(ui->EX12_LED_3,48,0);
                setLedState(ui->EX13_LED_3,49,0);
                setLedState(ui->EX14_LED_3,50,0);
                setLedState(ui->EX15_LED_3,51,0);
                setLedState(ui->EX16_LED_3,52,0);
                setLedState(ui->EX17_LED_3,53,0);
                setLedState(ui->EX18_LED_3,54,0);
                setLedState(ui->EX19_LED_3,55,0);
                setLedState(ui->EX20_LED_3,56,0);
                setLedState(ui->EX21_LED_3,57,0);
                setLedState(ui->EX22_LED_3,58,0);
                setLedState(ui->EX23_LED_3,59,0);
                setLedState(ui->EX24_LED_3,60,0);
            }
        }
        else if(ui->single_tabWidget->currentWidget() == ui->tabOutputType_3)
        {
            if(ui->OutWidgetWidget->currentIndex()==0)
            {
                setLedState(ui->Y1_LED_3,1,1);
                setLedState(ui->Y2_LED_3,2,1);
                setLedState(ui->Y3_LED_3,3,1);
                setLedState(ui->Y4_LED_3,4,1);
                setLedState(ui->Y5_LED_3,5,1);
                setLedState(ui->Y6_LED_3,6,1);
                setLedState(ui->Y7_LED_3,7,1);
                setLedState(ui->Y8_LED_3,8,1);
                setLedState(ui->Y9_LED_2,9,1);
                setLedState(ui->Y10_LED_3,10,1);
                setLedState(ui->Y11_LED_3,11,1);
                setLedState(ui->Y12_LED_3,12,1);
                setLedState(ui->Y13_LED_3,13,1);
                setLedState(ui->Y14_LED_3,14,1);
                setLedState(ui->Y15_LED_3,15,1);
                setLedState(ui->Y16_LED_3,16,1);
                setLedState(ui->Y17_LED_3,17,1);
                setLedState(ui->Y18_LED_3,18,1);
                setLedState(ui->Y19_LED_3,19,1);
                setLedState(ui->Y20_LED_3,20,1);
                setLedState(ui->Y21_LED_3,21,1);
                setLedState(ui->Y22_LED_3,22,1);
                setLedState(ui->Y23_LED_3,23,1);
                setLedState(ui->Y24_LED_3,24,1);
                setLedState(ui->EY1_LED_3,25,1);
                setLedState(ui->EY2_LED_3,26,1);
                setLedState(ui->EY3_LED_3,27,1);
                setLedState(ui->EY4_LED_3,28,1);
                setLedState(ui->EY5_LED_3,29,1);
                setLedState(ui->EY6_LED_3,30,1);
                setLedState(ui->EY7_LED_3,31,1);
                setLedState(ui->EY8_LED_3,32,1);
            }
            else if(ui->OutWidgetWidget->currentIndex()==1)
            {
                setLedState(ui->EY9_LED_3,33,1);
                setLedState(ui->EY10_LED_3,34,1);
                setLedState(ui->EY11_LED_3,35,1);
                setLedState(ui->EY12_LED_3,36,1);
                setLedState(ui->EY13_LED_3,37,1);
                setLedState(ui->EY14_LED_3,38,1);
                setLedState(ui->EY15_LED_3,39,1);
                setLedState(ui->EY16_LED_3,40,1);
                setLedState(ui->EY17_LED_3,41,1);
                setLedState(ui->EY18_LED_3,42,1);
                setLedState(ui->EY19_LED_3,43,1);
                setLedState(ui->EY20_LED_3,44,1);
            }
        }
        else if(ui->single_tabWidget->currentWidget() == ui->tabPortCustomize_3)
        {

        }
        else if(ui->single_tabWidget->currentWidget() == ui->tabhandControl)
        {

        }
        break;
    }
    default:
        break;
    }
}
/*************************************************************************
**  函数名：  monitor_hand_contril_handle()
**	输入参数：code:按键编号,value 按键值，0-松开 1-按下
**	输出参数：
**	函数功能：根据获取到的led灯状态设置LED灯状态
**  作者：    wukui
**  开发日期：2025/1/2
**************************************************************************/
void MonitorForm::monitor_hand_contril_handle(uint16_t code, int32_t value)
{
    if(ui->stackedWidget_main->currentIndex() && ui->single_tabWidget->currentWidget() == ui->tabhandControl)
    {//手控器界面
        switch(code)
        {
        case HandControlKeyCode::START:
        {
            if(value == 1)
            {
                setLED(ui->label_StartUp,2,30);//绿色
            }
            else
            {
                setLED(ui->label_StartUp,0,30);//灰色
            }
            break;
        }
        case HandControlKeyCode::STOP:
        {
            if(value == 1)
            {
                setLED(ui->label_stop,2,30);//绿色
            }
            else
            {
                setLED(ui->label_stop,0,30);//灰色
            }
            break;
        }
        case HandControlKeyCode::ORIGIN:
        {
            if(value == 1)
            {
                setLED(ui->label_origin,2,30);//绿色
            }
            else
            {
                setLED(ui->label_origin,0,30);//灰色
            }
            break;
        }
        case HandControlKeyCode::RETURN:
        {
            if(value == 1)
            {
                setLED(ui->label_return,2,30);//绿色
            }
            else
            {
                setLED(ui->label_return,0,30);//灰色
            }
            break;
        }
        case HandControlKeyCode::UP:
        {
            if(value == 1)
            {
                setLED(ui->label_up,2,30);//绿色
            }
            else
            {
                setLED(ui->label_up,0,30);//灰色
            }
            break;
        }
        case HandControlKeyCode::DOWN:
        {
            if(value == 1)
            {
                setLED(ui->label_down,2,30);//绿色
            }
            else
            {
                setLED(ui->label_down,0,30);//灰色
            }
            break;
        }
        case HandControlKeyCode::RIGHT_KEY1:
        {
            if(value == 1)
            {
                setLED(ui->label_btn2,2,30);//绿色
            }
            else
            {
                setLED(ui->label_btn2,0,30);//灰色
            }
            break;
        }
        case HandControlKeyCode::RIGHT_KEY2:
        {
            if(value == 1)
            {
                setLED(ui->label_btn9,2,30);//绿色
            }
            else
            {
                setLED(ui->label_btn9,0,30);//灰色
            }
            break;
        }
        case HandControlKeyCode::RIGHT_KEY3:
        {
            if(value == 1)
            {
                setLED(ui->label_btn1,2,30);//绿色
            }
            else
            {
                setLED(ui->label_btn1,0,30);//灰色
            }
            break;
        }
        case HandControlKeyCode::RIGHT_KEY4:
        {
            if(value == 1)
            {
                setLED(ui->label_btn8,2,30);//绿色
            }
            else
            {
                setLED(ui->label_btn8,0,30);//灰色
            }
            break;
        }
        case HandControlKeyCode::RIGHT_KEY5:
        {
            if(value == 1)
            {
                setLED(ui->label_btn3,2,30);//绿色
            }
            else
            {
                setLED(ui->label_btn3,0,30);//灰色
            }
            break;
        }
        case HandControlKeyCode::RIGHT_KEY6:
        {
            if(value == 1)
            {
                setLED(ui->label_btn10,2,30);//绿色
            }
            else
            {
                setLED(ui->label_btn10,0,30);//灰色
            }
            break;
        }
        case HandControlKeyCode::RIGHT_KEY7:
        {
            if(value == 1)
            {
                setLED(ui->label_btn4,2,30);//绿色
            }
            else
            {
                setLED(ui->label_btn4,0,30);//灰色
            }
            break;
        }
        case HandControlKeyCode::RIGHT_KEY8:
        {
            if(value == 1)
            {
                setLED(ui->label_btn11,2,30);//绿色
            }
            else
            {
                setLED(ui->label_btn11,0,30);//灰色
            }
            break;
        }
        case HandControlKeyCode::RIGHT_KEY9:
        {
            if(value == 1)
            {
                setLED(ui->label_btn5,2,30);//绿色
            }
            else
            {
                setLED(ui->label_btn5,0,30);//灰色
            }
            break;
        }
        case HandControlKeyCode::RIGHT_KEY10:
        {
            if(value == 1)
            {
                setLED(ui->label_btn12,2,30);//绿色
            }
            else
            {
                setLED(ui->label_btn12,0,30);//灰色
            }
            break;
        }
        case HandControlKeyCode::RIGHT_KEY11:
        {
            if(value == 1)
            {
                setLED(ui->label_btn6,2,30);//绿色
            }
            else
            {
                setLED(ui->label_btn6,0,30);//灰色
            }
            break;
        }
        case HandControlKeyCode::RIGHT_KEY12:
        {
            if(value == 1)
            {
                setLED(ui->label_btn13,2,30);//绿色
            }
            else
            {
                setLED(ui->label_btn13,0,30);//灰色
            }
            break;
        }
        default:
            break;
        }
    }
}
/*************************************************************************
**  函数名：  setLedState()
**	输入参数：port:需要设置状态的端口,type 0-输入端口 1-输出端口
**	输出参数：
**	函数功能：根据获取到的led灯状态设置LED灯状态
**  作者：    wukui
**  开发日期：2024/11/6
**************************************************************************/
void MonitorForm::setLedState(QLabel* label, uint8_t port,uint8_t type)
{
    if(type == 0)
    {
        setLED(label,m_InPortSta[port-1],30);
    }
    else if(type == 1)
    {
        setLED(label,m_OutPortSta[port-1],30);
    }

}

/*************************************************************************
**  函数名：  setLedName()
**	输入参数：port:需要设置状态的端口,type 0-输入端口 1-输出端口
**	输出参数：
**	函数功能：设置led灯名称
**  作者：    wukui
**  开发日期：2024/11/6
**************************************************************************/
void MonitorForm::setLedName(QLabel* label, uint8_t port,uint8_t type)
{
    if(type == 0 && port<=INPUT_TOTAL_NUM)
    {
        label->setText(m_Port_X[port-1].definePort+" "+m_Port_X[port-1].modifyName);
    }
    else if(type == 1 && port<=OUTPUT_TOTAL_NUM)
    {
        label->setText(m_Port_Y[port-1].definePort+" "+m_Port_Y[port-1].modifyName);
    }
}

void MonitorForm::initVariable()
{
    labsInputPort = {{
        {
            ui->x1, ui->x2, ui->x3, ui->x4, ui->x5, ui->x6, ui->x7, ui->x8,
            ui->x9, ui->x10, ui->x11, ui->x12, ui->x13, ui->x14, ui->x15, ui->x16,
            ui->x17, ui->x18, ui->x19, ui->x20, ui->x21, ui->x22, ui->x23, ui->x24,
            ui->x25, ui->x26, ui->x27, ui->x28, ui->x29, ui->x30, ui->x31, ui->x32,
            ui->x33, ui->x34, ui->x35, ui->x36,
            ui->ex1, ui->ex2, ui->ex3, ui->ex4, ui->ex5, ui->ex6, ui->ex7, ui->ex8,
            ui->ex9, ui->ex10, ui->ex11, ui->ex12, ui->ex13, ui->ex14, ui->ex15, ui->ex16,
            ui->ex17, ui->ex18, ui->ex19, ui->ex20, ui->ex21, ui->ex22, ui->ex23, ui->ex24
        },
        {
            ui->x1_2, ui->x2_2, ui->x3_2, ui->x4_2, ui->x5_2, ui->x6_2, ui->x7_2, ui->x8_2,
            ui->x9_2, ui->x10_2, ui->x11_2, ui->x12_2, ui->x13_2, ui->x14_2, ui->x15_2, ui->x16_2,
            ui->x17_2, ui->x18_2, ui->x19_2, ui->x20_2, ui->x21_2, ui->x22_2, ui->x23_2, ui->x24_2,
            ui->x25_2, ui->x26_2, ui->x27_2, ui->x28_2, ui->x29_2, ui->x30_2, ui->x31_2, ui->x32_2,
            ui->x33_2, ui->x34_2, ui->x35_2, ui->x36_2,
            ui->ex1_2, ui->ex2_2, ui->ex3_2, ui->ex4_2, ui->ex5_2, ui->ex6_2, ui->ex7_2, ui->ex8_2,
            ui->ex9_2, ui->ex10_2, ui->ex11_2, ui->ex12_2, ui->ex13_2, ui->ex14_2, ui->ex15_2, ui->ex16_2,
            ui->ex17_2, ui->ex18_2, ui->ex19_2, ui->ex20_2, ui->ex21_2, ui->ex22_2, ui->ex23_2, ui->ex24_2
        },
        {
            ui->x1_3, ui->x2_3, ui->x3_3, ui->x4_3, ui->x5_3, ui->x6_3, ui->x7_3, ui->x8_3,
            ui->x9_3, ui->x10_3, ui->x11_3, ui->x12_3, ui->x13_3, ui->x14_3, ui->x15_3, ui->x16_3,
            ui->x17_3, ui->x18_3, ui->x19_3, ui->x20_3, ui->x21_3, ui->x22_3, ui->x23_3, ui->x24_3,
            ui->x25_3, ui->x26_3, ui->x27_3, ui->x28_3, ui->x29_3, ui->x30_3, ui->x31_3, ui->x32_3,
            ui->x33_3, ui->x34_3, ui->x35_3, ui->x36_3,
            ui->ex1_3, ui->ex2_3, ui->ex3_3, ui->ex4_3, ui->ex5_3, ui->ex6_3, ui->ex7_3, ui->ex8_3,
            ui->ex9_3, ui->ex10_3, ui->ex11_3, ui->ex12_3, ui->ex13_3, ui->ex14_3, ui->ex15_3, ui->ex16_3,
            ui->ex17_3, ui->ex18_3, ui->ex19_3, ui->ex20_3, ui->ex21_3, ui->ex22_3, ui->ex23_3, ui->ex24_3
        }
    }};

    labsOutputPort = {{
        {
            ui->Y1, ui->Y2, ui->Y3, ui->Y4, ui->Y5, ui->Y6, ui->Y7, ui->Y8,
            ui->Y9, ui->Y10, ui->Y11, ui->Y12, ui->Y13, ui->Y14, ui->Y15, ui->Y16,
            ui->Y17, ui->Y18, ui->Y19, ui->Y20, ui->Y21, ui->Y22, ui->Y23, ui->Y24,
            ui->EY1, ui->EY2, ui->EY3, ui->EY4, ui->EY5, ui->EY6, ui->EY7, ui->EY8,
            ui->EY9, ui->EY10, ui->EY11, ui->EY12, ui->EY13, ui->EY14, ui->EY15, ui->EY16,
            ui->EY17, ui->EY18, ui->EY19, ui->EY20
        },
        {
            ui->Y1_2, ui->Y2_2, ui->Y3_2, ui->Y4_2, ui->Y5_2, ui->Y6_2, ui->Y7_2, ui->Y8_2,
            ui->Y9_2, ui->Y10_2, ui->Y11_2, ui->Y12_2, ui->Y13_2, ui->Y14_2, ui->Y15_2, ui->Y16_2,
            ui->Y17_2, ui->Y18_2, ui->Y19_2, ui->Y20_2, ui->Y21_2, ui->Y22_2, ui->Y23_2, ui->Y24_2,
            ui->EY1_2, ui->EY2_2, ui->EY3_2, ui->EY4_2, ui->EY5_2, ui->EY6_2, ui->EY7_2, ui->EY8_2,
            ui->EY9_2, ui->EY10_2, ui->EY11_2, ui->EY12_2, ui->EY13_2, ui->EY14_2, ui->EY15_2, ui->EY16_2,
            ui->EY17_2, ui->EY18_2, ui->EY19_2, ui->EY20_2
        },
        {
            ui->Y1_3, ui->Y2_3, ui->Y3_3, ui->Y4_3, ui->Y5_3, ui->Y6_3, ui->Y7_3, ui->Y8_3,
            ui->Y9_3, ui->Y10_3, ui->Y11_3, ui->Y12_3, ui->Y13_3, ui->Y14_3, ui->Y15_3, ui->Y16_3,
            ui->Y17_3, ui->Y18_3, ui->Y19_3, ui->Y20_3, ui->Y21_3, ui->Y22_3, ui->Y23_3, ui->Y24_3,
            ui->EY1_3, ui->EY2_3, ui->EY3_3, ui->EY4_3, ui->EY5_3, ui->EY6_3, ui->EY7_3, ui->EY8_3,
            ui->EY9_3, ui->EY10_3, ui->EY11_3, ui->EY12_3, ui->EY13_3, ui->EY14_3, ui->EY15_3, ui->EY16_3,
            ui->EY17_3, ui->EY18_3, ui->EY19_3, ui->EY20_3
        }
    }};

}

void MonitorForm::refeshPortLableName()
{
    for (int i = 0; i < INPUT_TOTAL_NUM; i++)
    {
        const auto& port = m_Port_X[i];

        QString portName = port.functionSet ? port.modifyName : port.ResModifyName;
        for (int j = 0; j < 3; j++)
        {
            labsInputPort.at(j).at(i)->setText(portName);
        }
    }
    for (int i = 0; i < OUTPUT_TOTAL_NUM; i++)
    {
        const auto& port = m_Port_Y[i];

        QString portName = port.functionSet ? port.modifyName : port.ResModifyName;

        for (size_t j = 0; j < labsOutputPort.size(); j++)
        {
            labsOutputPort.at(j).at(i)->setText(portName);
        }
    }
}

/*************************************************************************
**  函数名：  SetHandControlName()
**	输入参数：
**	输出参数：
**	函数功能：设置手控器界面按钮内容
**  作者：    wukui
**  开发日期：2025/1/3
**************************************************************************/
void MonitorForm::SetHandControlName()
{
    //KEY
    ui->label_btn1text->setText(keyFunDesription[0]);
    ui->label_btn2text->setText(keyFunDesription[2]);
    ui->label_btn3text->setText(keyFunDesription[4]);
    ui->label_btn4text->setText(keyFunDesription[6]);
    ui->label_btn5text->setText(keyFunDesription[8]);
    ui->label_btn6text->setText(keyFunDesription[10]);
    ui->label_btn7text->setText(keyFunDesription[12]);
    ui->label_btn8text->setText(keyFunDesription[1]);
    ui->label_btn9text->setText(keyFunDesription[3]);
    ui->label_btn10text->setText(keyFunDesription[5]);
    ui->label_btn11text->setText(keyFunDesription[7]);
    ui->label_btn12text->setText(keyFunDesription[9]);
    ui->label_btn13text->setText(keyFunDesription[11]);
    ui->label_btn14text->setText(keyFunDesription[13]);
    //LED
    if(sigSetDesription[0]!="0")
    {
        ui->label_btn15text->setText(tr("工作:")+sigSetDesription[0]);
    }
    else
    {
        ui->label_btn15text->setText(tr("工作"));
    }
    if(sigSetDesription[1]!="0")
    {
        ui->label_btn16text->setText(tr("加工完成:")+sigSetDesription[1]);
    }
    else
    {
        ui->label_btn16text->setText(tr("加工完成"));
    }
    if(sigSetDesription[2]!="0")
    {
        ui->label_btn17text->setText(tr("启动加工:")+sigSetDesription[2]);
    }
    else
    {
        ui->label_btn17text->setText(tr("启动加工"));
    }
    if(sigSetDesription[3]!="0")
    {
        ui->label_btn18text->setText(tr("信号:")+sigSetDesription[3]);
    }
    else
    {
        ui->label_btn18text->setText(tr("信号"));
    }
    if(sigSetDesription[4]!="0")
    {
        ui->label_btn19text->setText(tr("手轮按键:")+sigSetDesription[4]);
    }
    else
    {
        ui->label_btn19text->setText(tr("手轮按键"));
    }
}

void MonitorForm::showSignalLEDStatus(uint8_t status)
{
    if(!this->isVisible() || !ui->tabhandControl->isVisible()
            || ui->single_tabWidget->currentWidget() != ui->tabhandControl)
    {
        return;
    }
    static const std::array<QLabel*, 4> labLED = {
            ui->label_btn15, ui->label_btn16, ui->label_btn17, ui->label_btn18
        };

        int numLEDs = labLED.size();

        for (int i = 0; i < numLEDs; i++)
        {
            // 检查特定位是否为1
            bool isOn = (status >> i) & 1;

            int color = isOn ? 2 : 0;
            setLED(labLED[i], color, 30);
        }

}

void MonitorForm::showEvent(QShowEvent *event)
{
    RefreshLedTime->start();

    refeshPortLableName();

    QWidget::showEvent(event);
}

void MonitorForm::hideEvent(QHideEvent *event)
{
    if (RefreshLedTime->isActive())
        RefreshLedTime->stop();

    QWidget::hideEvent(event);
}

void MonitorForm::InitAllLedName()
{
    setLedName(ui->x1,1,0);
    setLedName(ui->x2,2,0);
    setLedName(ui->x3,3,0);
    setLedName(ui->x4,4,0);
    setLedName(ui->x5,5,0);
    setLedName(ui->x6,6,0);
    setLedName(ui->x7,7,0);
    setLedName(ui->x8,8,0);
    setLedName(ui->x9,9,0);
    setLedName(ui->x10,10,0);
    setLedName(ui->x11,11,0);
    setLedName(ui->x12,12,0);
    setLedName(ui->x13,13,0);
    setLedName(ui->x14,14,0);
    setLedName(ui->x15,15,0);
    setLedName(ui->x16,16,0);
    setLedName(ui->x17,17,0);
    setLedName(ui->x18,18,0);
    setLedName(ui->x19,19,0);
    setLedName(ui->x20,20,0);
    setLedName(ui->x21,21,0);
    setLedName(ui->x22,22,0);
    setLedName(ui->x23,23,0);
    setLedName(ui->x24,24,0);
    setLedName(ui->x25,25,0);
    setLedName(ui->x26,26,0);
    setLedName(ui->x27,27,0);
    setLedName(ui->x28,28,0);
    setLedName(ui->x29,29,0);
    setLedName(ui->x30,30,0);
    setLedName(ui->x31,31,0);
    setLedName(ui->x32,32,0);
    setLedName(ui->x33,33,0);
    setLedName(ui->x34,34,0);
    setLedName(ui->x35,35,0);
    setLedName(ui->x36,36,0);

    setLedName(ui->ex1, 37,0);
    setLedName(ui->ex2, 38,0);
    setLedName(ui->ex3, 39,0);
    setLedName(ui->ex4, 40,0);
    setLedName(ui->ex5, 41,0);
    setLedName(ui->ex6, 42,0);
    setLedName(ui->ex7, 43,0);
    setLedName(ui->ex8, 44,0);
    setLedName(ui->ex9, 45,0);
    setLedName(ui->ex10,46,0);
    setLedName(ui->ex11,47,0);
    setLedName(ui->ex12,48,0);
    setLedName(ui->ex13,49,0);
    setLedName(ui->ex14,50,0);
    setLedName(ui->ex15,51,0);
    setLedName(ui->ex16,52,0);
    setLedName(ui->ex17,53,0);
    setLedName(ui->ex18,54,0);
    setLedName(ui->ex19,55,0);
    setLedName(ui->ex20,56,0);
    setLedName(ui->ex21,57,0);
    setLedName(ui->ex22,58,0);
    setLedName(ui->ex23,59,0);
    setLedName(ui->ex24,60,0);

    setLedName(ui->Y1,1,1);
    setLedName(ui->Y2,2,1);
    setLedName(ui->Y3,3,1);
    setLedName(ui->Y4,4,1);
    setLedName(ui->Y5,5,1);
    setLedName(ui->Y6,6,1);
    setLedName(ui->Y7,7,1);
    setLedName(ui->Y8,8,1);
    setLedName(ui->Y9,9,1);
    setLedName(ui->Y10,10,1);
    setLedName(ui->Y11,11,1);
    setLedName(ui->Y12,12,1);
    setLedName(ui->Y13,13,1);
    setLedName(ui->Y14,14,1);
    setLedName(ui->Y15,15,1);
    setLedName(ui->Y16,16,1);
    setLedName(ui->Y17,17,1);
    setLedName(ui->Y18,18,1);
    setLedName(ui->Y19,19,1);
    setLedName(ui->Y20,20,1);
    setLedName(ui->Y21,21,1);
    setLedName(ui->Y22,22,1);
    setLedName(ui->Y23,23,1);
    setLedName(ui->Y24,24,1);

    setLedName(ui->EY1,1,1);
    setLedName(ui->EY2,2,1);
    setLedName(ui->EY3,3,1);
    setLedName(ui->EY4,4,1);
    setLedName(ui->EY5,5,1);
    setLedName(ui->EY6,6,1);
    setLedName(ui->EY7,7,1);
    setLedName(ui->EY8,8,1);
    setLedName(ui->EY9,9,1);
    setLedName(ui->EY10,10,1);
    setLedName(ui->EY11,11,1);
    setLedName(ui->EY12,12,1);
    setLedName(ui->EY13,13,1);
    setLedName(ui->EY14,14,1);
    setLedName(ui->EY15,15,1);
    setLedName(ui->EY16,16,1);
    setLedName(ui->EY17,17,1);
    setLedName(ui->EY18,18,1);
    setLedName(ui->EY19,19,1);
    setLedName(ui->EY20,20,1);
    //右侧
    setLedName(ui->x1_2,1,0);
    setLedName(ui->x2_2,2,0);
    setLedName(ui->x3_2,3,0);
    setLedName(ui->x4_2,4,0);
    setLedName(ui->x5_2,5,0);
    setLedName(ui->x6_2,6,0);
    setLedName(ui->x7_2,7,0);
    setLedName(ui->x8_2,8,0);
    setLedName(ui->x9_2,9,0);
    setLedName(ui->x10_2,10,0);
    setLedName(ui->x11_2,11,0);
    setLedName(ui->x12_2,12,0);
    setLedName(ui->x13_2,13,0);
    setLedName(ui->x14_2,14,0);
    setLedName(ui->x15_2,15,0);
    setLedName(ui->x16_2,16,0);
    setLedName(ui->x17_2,17,0);
    setLedName(ui->x18_2,18,0);
    setLedName(ui->x19_2,19,0);
    setLedName(ui->x20_2,20,0);
    setLedName(ui->x21_2,21,0);
    setLedName(ui->x22_2,22,0);
    setLedName(ui->x23_2,23,0);
    setLedName(ui->x24_2,24,0);
    setLedName(ui->x25_2,25,0);
    setLedName(ui->x26_2,26,0);
    setLedName(ui->x27_2,27,0);
    setLedName(ui->x28_2,28,0);
    setLedName(ui->x29_2,29,0);
    setLedName(ui->x30_2,30,0);
    setLedName(ui->x31_2,31,0);
    setLedName(ui->x32_2,32,0);
    setLedName(ui->x33_2,33,0);
    setLedName(ui->x34_2,34,0);
    setLedName(ui->x35_2,35,0);
    setLedName(ui->x36_2,36,0);

    setLedName(ui->ex1_2, 37,0);
    setLedName(ui->ex2_2, 38,0);
    setLedName(ui->ex3_2, 39,0);
    setLedName(ui->ex4_2, 40,0);
    setLedName(ui->ex5_2, 41 ,0);
    setLedName(ui->ex6_2, 42,0);
    setLedName(ui->ex7_2, 43 ,0);
    setLedName(ui->ex8_2, 44,0);
    setLedName(ui->ex9_2, 45,0);
    setLedName(ui->ex10_2,46,0);
    setLedName(ui->ex11_2,47,0);
    setLedName(ui->ex12_2,48,0);
    setLedName(ui->ex13_2,49,0);
    setLedName(ui->ex14_2,50,0);
    setLedName(ui->ex15_2,51,0);
    setLedName(ui->ex16_2,52,0);
    setLedName(ui->ex17_2,53,0);
    setLedName(ui->ex18_2,54,0);
    setLedName(ui->ex19_2,55,0);
    setLedName(ui->ex20_2,56,0);
    setLedName(ui->ex21_2,57,0);
    setLedName(ui->ex22_2,58,0);
    setLedName(ui->ex23_2,59,0);
    setLedName(ui->ex24_2,60,0);

    setLedName(ui->Y1_2,1,1);
    setLedName(ui->Y2_2,2,1);
    setLedName(ui->Y3_2,3,1);
    setLedName(ui->Y4_2,4,1);
    setLedName(ui->Y5_2,5,1);
    setLedName(ui->Y6_2,6,1);
    setLedName(ui->Y7_2,7,1);
    setLedName(ui->Y8_2,8,1);
    setLedName(ui->Y9_2,9,1);
    setLedName(ui->Y10_2,10,1);
    setLedName(ui->Y11_2,11,1);
    setLedName(ui->Y12_2,12,1);
    setLedName(ui->Y13_2,13,1);
    setLedName(ui->Y14_2,14,1);
    setLedName(ui->Y15_2,15,1);
    setLedName(ui->Y16_2,16,1);
    setLedName(ui->Y17_2,17,1);
    setLedName(ui->Y18_2,18,1);
    setLedName(ui->Y19_2,19,1);
    setLedName(ui->Y20_2,20,1);
    setLedName(ui->Y21_2,21,1);
    setLedName(ui->Y22_2,22,1);
    setLedName(ui->Y23_2,23,1);
    setLedName(ui->Y24_2,24,1);

    setLedName(ui->EY1_2, 25,1);
    setLedName(ui->EY2_2, 26,1);
    setLedName(ui->EY3_2, 27,1);
    setLedName(ui->EY4_2, 28,1);
    setLedName(ui->EY5_2, 29,1);
    setLedName(ui->EY6_2, 30,1);
    setLedName(ui->EY7_2, 31,1);
    setLedName(ui->EY8_2, 32,1);
    setLedName(ui->EY9_2, 33,1);
    setLedName(ui->EY10_2,34,1);
    setLedName(ui->EY11_2,35,1);
    setLedName(ui->EY12_2,36,1);
    setLedName(ui->EY13_2,37,1);
    setLedName(ui->EY14_2,38,1);
    setLedName(ui->EY15_2,39,1);
    setLedName(ui->EY16_2,40,1);
    setLedName(ui->EY17_2,41,1);
    setLedName(ui->EY18_2,42,1);
    setLedName(ui->EY19_2,43,1);
    setLedName(ui->EY20_2,44,1);
    //单屏显示
    //右侧
    setLedName(ui->x1_3,1,0);
    setLedName(ui->x2_3,2,0);
    setLedName(ui->x3_3,3,0);
    setLedName(ui->x4_3,4,0);
    setLedName(ui->x5_3,5,0);
    setLedName(ui->x6_3,6,0);
    setLedName(ui->x7_3,7,0);
    setLedName(ui->x8_3,8,0);
    setLedName(ui->x9_3,9,0);
    setLedName(ui->x10_3,10,0);
    setLedName(ui->x11_3,11,0);
    setLedName(ui->x12_3,12,0);
    setLedName(ui->x13_3,13,0);
    setLedName(ui->x14_3,14,0);
    setLedName(ui->x15_3,15,0);
    setLedName(ui->x16_3,16,0);
    setLedName(ui->x17_3,17,0);
    setLedName(ui->x18_3,18,0);
    setLedName(ui->x19_3,19,0);
    setLedName(ui->x20_3,20,0);
    setLedName(ui->x21_3,21,0);
    setLedName(ui->x22_3,22,0);
    setLedName(ui->x23_3,23,0);
    setLedName(ui->x24_3,24,0);
    setLedName(ui->x25_3,25,0);
    setLedName(ui->x26_3,26,0);
    setLedName(ui->x27_3,27,0);
    setLedName(ui->x28_3,28,0);
    setLedName(ui->x29_3,29,0);
    setLedName(ui->x30_3,30,0);
    setLedName(ui->x31_3,31,0);
    setLedName(ui->x32_3,32,0);
    setLedName(ui->x33_3,33,0);
    setLedName(ui->x34_3,34,0);
    setLedName(ui->x35_3,35,0);
    setLedName(ui->x36_3,36,0);

    setLedName(ui->ex1_3, 37,0);
    setLedName(ui->ex2_3, 38,0);
    setLedName(ui->ex3_3, 39,0);
    setLedName(ui->ex4_3, 40,0);
    setLedName(ui->ex5_3, 41,0);
    setLedName(ui->ex6_3, 42,0);
    setLedName(ui->ex7_3, 43,0);
    setLedName(ui->ex8_3, 44,0);
    setLedName(ui->ex9_3, 45,0);
    setLedName(ui->ex10_3,46,0);
    setLedName(ui->ex11_3,47,0);
    setLedName(ui->ex12_3,48,0);
    setLedName(ui->ex13_3,49,0);
    setLedName(ui->ex14_3,50,0);
    setLedName(ui->ex15_3,51,0);
    setLedName(ui->ex16_3,52,0);
    setLedName(ui->ex17_3,53,0);
    setLedName(ui->ex18_3,54,0);
    setLedName(ui->ex19_3,55,0);
    setLedName(ui->ex20_3,56,0);
    setLedName(ui->ex21_3,57,0);
    setLedName(ui->ex22_3,58,0);
    setLedName(ui->ex23_3,59,0);
    setLedName(ui->ex24_3,60,0);

    setLedName(ui->Y1_3,1,1);
    setLedName(ui->Y2_3,2,1);
    setLedName(ui->Y3_3,3,1);
    setLedName(ui->Y4_3,4,1);
    setLedName(ui->Y5_3,5,1);
    setLedName(ui->Y6_3,6,1);
    setLedName(ui->Y7_3,7,1);
    setLedName(ui->Y8_3,8,1);
    setLedName(ui->Y9_3,9,1);
    setLedName(ui->Y10_3,10,1);
    setLedName(ui->Y11_3,11,1);
    setLedName(ui->Y12_3,12,1);
    setLedName(ui->Y13_3,13,1);
    setLedName(ui->Y14_3,14,1);
    setLedName(ui->Y15_3,15,1);
    setLedName(ui->Y16_3,16,1);
    setLedName(ui->Y17_3,17,1);
    setLedName(ui->Y18_3,18,1);
    setLedName(ui->Y19_3,19,1);
    setLedName(ui->Y20_3,20,1);
    setLedName(ui->Y21_3,21,1);
    setLedName(ui->Y22_3,22,1);
    setLedName(ui->Y23_3,23,1);
    setLedName(ui->Y24_3,24,1);

    setLedName(ui->EY1_3, 25,1);
    setLedName(ui->EY2_3, 26,1);
    setLedName(ui->EY3_3, 27,1);
    setLedName(ui->EY4_3, 28,1);
    setLedName(ui->EY5_3, 29,1);
    setLedName(ui->EY6_3, 30,1);
    setLedName(ui->EY7_3, 31,1);
    setLedName(ui->EY8_3, 32,1);
    setLedName(ui->EY9_3, 33,1);
    setLedName(ui->EY10_3,34,1);
    setLedName(ui->EY11_3,35,1);
    setLedName(ui->EY12_3,36,1);
    setLedName(ui->EY13_3,37,1);
    setLedName(ui->EY14_3,38,1);
    setLedName(ui->EY15_3,39,1);
    setLedName(ui->EY16_3,40,1);
    setLedName(ui->EY17_3,41,1);
    setLedName(ui->EY18_3,42,1);
    setLedName(ui->EY19_3,43,1);
    setLedName(ui->EY20_3,44,1);
}
