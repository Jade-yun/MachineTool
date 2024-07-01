#include "manualform.h"
#include "ui_manualform.h"
#include <QPushButton>
#include <QToolButton>
#include "QTabBar"

#include <QString>
#include <QDebug>

#pragma execution_character_set("utf-8")

ManualForm::ManualForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ManualForm)
{
    ui->setupUi(this);

    initButtons();

}

ManualForm::~ManualForm()
{
    delete ui;
}

void ManualForm::on_btnNewButton_clicked()
{

    // to new button
    if (btns.size() < 20)
    {
        DraggableButton* btn = new DraggableButton(ui->tabOperate);
        btns.append(btn);

        btn->setFixedSize(120, 40);
        QRect btnPos = QRect(QPoint(20 + 120 * (btns.size() / 8 ), 20 + 50 * (btns.size() % 8)), btn->size());
        btn->setGeometry(btnPos);
        const QString btnName = tr((QString("按钮") + QString::number(btns.size())).toUtf8().constData());
        btn->setText(btnName);
        btn->show();

        // to set the slot function for every button
//        connect(btn, &DraggableButton::clicked, this, [=]() {
        connect(btn, &DraggableButton::clicked, this, [&, btn]() {
            lastClickedButton = btn;

            // fix me ??? this cannot work, the possible reson is event conflict.
            btn->setCheckable(draggableMode);
            btn->setChecked(draggableMode);
            btn->setAutoExclusive(true);
        });
    }

    // precreate buttons in ui, and set it visibility
//    for (int i = 0; i < 20; ++i) {
//        if (buttons[i] && !buttons[i]->isVisible()) {
//            buttons[i]->setVisible(true);
//            break;
//        }
//    }
}

void ManualForm::on_btnDeleteButton_clicked()
{
    // to delete
    // examine if any button is clicked, delete the choosed one
    if (lastClickedButton && btns.contains(lastClickedButton))
    {
        btns.removeOne(lastClickedButton);
        delete lastClickedButton;
        lastClickedButton = nullptr;
    }
    // if no button is choosed, delete the last in btns
    else if (!btns.isEmpty())
    {
        DraggableButton* btn = btns.takeLast();
        delete btn;
    }
}


void ManualForm::on_checkBoxEditPosition_stateChanged(int arg1)
{
//    for (int i = 0; i < 20; ++i)
//    {
//        if (buttons[i])
//        {
//            buttons[i]->setDraggable(arg1);
//        }
//    }

    for (auto btn : btns)
    {
        btn->setDraggable(arg1);

        //
        btn->setCheckable(arg1);
//        btn->setChecked(arg1);
//        qDebug() << "btn is checkable:" << btn->isCheckable();
    }

}

void ManualForm::on_btnImportPicture_clicked()
{
//    QString imagePath = "/opt/MachineTool/bin/backgroud.png";
    QString imagePath = "./backgroud.png";
    QPixmap pixmap(imagePath);

    if (!pixmap.isNull())
    {
        ui->labelBackgroud->setPixmap(pixmap);
        ui->labelBackgroud->setScaledContents(true);
        ui->labelBackgroud->show();
    }
    else
    {
        qDebug() << "Failed to load image: " << imagePath;
    }
}

void ManualForm::initButtons()
{
    draggableMode = false;
    lastClickedButton = nullptr;

//    // Initialize buttons and hide buttons 1-8
//    for (int i = 0; i < 20; ++i)
//    {
//        buttons[i] = findChild<DraggableButton*>(QString("btn%1").arg(i + 1));
//        if (buttons[i])
//        {
////            buttons[i]->setVisible(i >= 8);
//            buttons[i]->setVisible(false);
//        }
//    }
}
