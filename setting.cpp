#include "setting.h"
#include "ui_setting.h"

#include <QMovie>
#include <QtDebug>

Setting::Setting(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Setting)
{
    ui->setupUi(this);
    setAllStyleSheet();

    ui->checkBoxPoiskie->setDisabled(true);
    ui->checkBoxRussian->setDisabled(true);
    ui->checkBoxTradChinese->setDisabled(true);
    ui->checkBoxSpain->setDisabled(true);
    ui->checkBoxSpecial->setDisabled(true);
    ui->checkBoxVietnam->setDisabled(true);
}

Setting::~Setting()
{
    delete ui;
}

void Setting::on_btnSigSet_clicked()
{
    // jump to signal set page
    ui->stackedWidget->setCurrentIndex(1);
    ui->stackedWidget->setCurrentWidget(ui->pageSignal);
    //    qDebug() << "jump to signal set page";
}

void Setting::slotSettingHome()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->stackedWidget->setCurrentWidget(ui->pageSettingHome);
    //    qDebug() << "slotSettingHome";
}

void Setting::loadAndPlayGif(const QString& path)
{

      movie = new QMovie(path, this);
      ui->gifLabel->setMovie(movie);
      movie->start();
}

void Setting::setAllStyleSheet()
{
}

void Setting::on_btnSafetySet_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->pageSafety);
}

void Setting::on_btnProductSet_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->pageProduct);
}

void Setting::on_btnSystemSet_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->pageSystem);
}

void Setting::on_btnServoSpeed_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->pageServoSpeed);
}

void Setting::on_btnServoSafePoint_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->pageServoSafePoint);
}

void Setting::on_btnMachinePara_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->pageMachinePara);

}

void Setting::on_btnStackSet_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->pageStack);
}

void Setting::on_comboBoxIotSelection_currentIndexChanged(int index)
{
    qDebug() << "index = " << index;
    if (index == 2)
    {
        ui->stkWidgetIPSet->setCurrentIndex(0);

        // disable these object
        ui->stkWidgetIPSet->setDisabled(true);
        ui->widgetIotDebug->setDisabled(true);
    }
    else
    {
        ui->stkWidgetIPSet->setEnabled(true);
        ui->widgetIotDebug->setEnabled(true);
    }
    if (index == 0)
    {
        ui->stkWidgetIPSet->setCurrentIndex(0);
    }
    else if (index == 1)
    {
        ui->stkWidgetIPSet->setCurrentIndex(1);
    }
}

void Setting::on_btnLastPage_clicked()
{
    ui->stkWidgetOriginSet->setCurrentWidget(ui->page0);
}

void Setting::on_btnNextPage_clicked()
{
    ui->stkWidgetOriginSet->setCurrentWidget(ui->page1);
}

void Setting::on_comboBox_96_currentIndexChanged(int index)
{
    this->loadAndPlayGif(":/images/gif/test.gif");
}
