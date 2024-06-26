#include "manualform.h"
#include "ui_manualform.h"
#include <QPushButton>
#include <QToolButton>
#include "QTabBar"

#pragma execution_character_set("utf-8")

ManualForm::ManualForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ManualForm)
{
    ui->setupUi(this);
	//ui->tabWidgetSig->setStyleSheet("QTabWidget::tab-bar {border-top: 10px solid black;border-left: 2px solid black;border-right: 2px solid black;}");

    QToolButton* button = new QToolButton(ui->tabWidgetSig);
	button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	button->setIcon(QIcon("./images/enter.png"));
	button->setText("进入堆叠");
	button->setStyleSheet("QToolButton { width: 100px; height: 45px; font-size: 16pt;font-weight:bold;font-family: 'SimSun';background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 #fafafa, stop: 0.4 #f4f4f4,stop: 0.5 #e7e7e7, stop: 1.0 #fafafa)}");
	ui->tabWidgetSig->setCornerWidget(button, Qt::TopRightCorner);

	/*QPushButton* b2 = new QPushButton("设置");
	b2->setObjectName("settingButton");
	ui->tabWidgetSig->setTabEnabled(5, false);
	ui->tabWidgetSig->tabBar()->setTabButton(6, QTabBar::RightSide, b2);*/
}

ManualForm::~ManualForm()
{
    delete ui;
}
