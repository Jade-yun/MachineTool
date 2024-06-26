#include "monitorform.h"
#include "ui_monitorform.h"

MonitorForm::MonitorForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MonitorForm)
{
    ui->setupUi(this);

	v_lab.clear();
	v_lab.push_back(ui->label_176);
	v_lab.push_back(ui->label_181);
	v_lab.push_back(ui->label_184);
	v_lab.push_back(ui->label_175);
	v_lab.push_back(ui->label_185);
	v_lab.push_back(ui->label_187);
	v_lab.push_back(ui->label_186);
	v_lab.push_back(ui->label_189);

	v_lab.push_back(ui->label_192);
	v_lab.push_back(ui->label_197);
	v_lab.push_back(ui->label_200);
	v_lab.push_back(ui->label_191);
	v_lab.push_back(ui->label_201);
	v_lab.push_back(ui->label_203);
	v_lab.push_back(ui->label_202);
	v_lab.push_back(ui->label_205);

	v_lab.push_back(ui->label_273);
	v_lab.push_back(ui->label_278);
	v_lab.push_back(ui->label_281);
	v_lab.push_back(ui->label_272);
	v_lab.push_back(ui->label_282);
	v_lab.push_back(ui->label_284);
	v_lab.push_back(ui->label_283);
	v_lab.push_back(ui->label_286);

	v_lab.push_back(ui->label_289);
	v_lab.push_back(ui->label_294);
	v_lab.push_back(ui->label_297);
	v_lab.push_back(ui->label_288);
	v_lab.push_back(ui->label_298);
	v_lab.push_back(ui->label_300);
	v_lab.push_back(ui->label_299);
	v_lab.push_back(ui->label_302);


//    for (int i = 0; i < v_lab.size(); i++)
//    {

//        setLED(v_lab[i], 3, 20);
//    }
    for (auto lab : v_lab)
    {
        setLED(lab, 3, 20);
    }

//    foreach (QLabel* var in v_lab)
//	{
//        setLED(var, 3, 20);
//	}
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
