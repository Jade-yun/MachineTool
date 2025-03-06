#include "alarmbar.h"
#include "ui_alarmbar.h"

#include "alarmbar.h"
#include "alarminfodialog.h"
#include "cmd.h"

#include <QHBoxLayout>
#include <QMessageBox>
#include <QMouseEvent>
#include <QApplication>
#include <QScreen>
#include <QElapsedTimer>
#include <QButtonGroup>
#include <QSettings>

extern const QString alarmInfoMappingPath;

AlarmBar::AlarmBar(QWidget* parent)
    : QDialog(parent),
      ui(new Ui::AlarmBar)
{
    ui->setupUi(this);

    QButtonGroup* group = new QButtonGroup(this);
    group->setExclusive(false);
    group->addButton(ui->btnAlarmHelp);
    group->addButton(ui->btnClose);

    barHeight = this->height();
    isDragging = false;
    // 动画设置
    animation = new QPropertyAnimation(this, "geometry");
    animation->setDuration(100);

    connect(ui->btnClose, &QPushButton::clicked, this, [=](){
        this->hide();
    });
    connect(ui->btnAlarmHelp, &QPushButton::clicked, this, [=](){
        this->hide();

        if (alarmInfoQueue.isEmpty()) return;

        int alarmNum = alarmInfoQueue.back().alarmNum;
        AlarmInfoDialog::instance()->showAlarmInfo(alarmNum);
    });
}

AlarmBar::~AlarmBar()
{
    delete ui;
}

void AlarmBar::showAlarm(int alarmNum, const QString& alarmContent)
{
    // 设置报警信息
    ui->labAlarm->setText(tr("报警编号：") + QString::number(alarmNum) + "—" + alarmContent);
    this->show();

//    QRect startGeometry = geometry();
//    startGeometry.moveTop(parentWidget()->height());
//    setGeometry(startGeometry);

//    QRect endGeometry = geometry();
//    endGeometry.moveTop(parentWidget()->height() - barHeight);

//    animation->setStartValue(startGeometry);
//    animation->setEndValue(endGeometry);
    //    animation->start();
}

void AlarmBar::showAlarm(int alarmNum)
{
    this->setGeometry(5, 640, 1014, 60);

    QSettings alarmInfoSettings(alarmInfoMappingPath, QSettings::IniFormat);
    alarmInfoSettings.setIniCodec("utf-8");
    alarmInfoSettings.beginGroup(QString::number(alarmNum));
    QString alarmContent = alarmInfoSettings.value("AlarmContent").toString();
    alarmInfoSettings.endGroup();

    this->showAlarm(alarmNum, alarmContent);
}

void AlarmBar::hideAlarm()
{
    // 动画从显示位置到隐藏位置
    QRect startGeometry = geometry();
    QRect endGeometry = geometry();
    endGeometry.moveTop(parentWidget()->height());

    animation->setStartValue(startGeometry);
    animation->setEndValue(endGeometry);
    animation->start();
}

void AlarmBar::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && ui->labAlarm->rect().contains(event->pos()))
    {
        dragStartPos = event->globalPos() - frameGeometry().topLeft();
        isDragging = true;
    }
}

void AlarmBar::mouseMoveEvent(QMouseEvent* event)
{
    static QElapsedTimer timer;
    if (!timer.isValid()) timer.start();

    if (timer.elapsed() < 10) // 40ms -> 25FPS
    {
        return;
    }
    timer.restart();

    if (isDragging && event->buttons() & Qt::LeftButton)
    {
        // 计算新位置
        QPoint newPos = event->globalPos() - dragStartPos;
        int maxHeight = QApplication::primaryScreen()->availableGeometry().height() - barHeight; // 限制最大高度
        int minHeight = 0;

        // 限制在上下边界内
        newPos.setY(qBound(minHeight, newPos.y(), maxHeight));
        newPos.setX(geometry().x()); // 保持 X 坐标不变
        move(newPos);
    }
}

void AlarmBar::mouseReleaseEvent(QMouseEvent* event)
{
    Q_UNUSED(event)
    isDragging = false; // 停止拖动
}

void AlarmBar::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
    }
    QWidget::changeEvent(e);
}
