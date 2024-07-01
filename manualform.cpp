#include "manualform.h"
#include "ui_manualform.h"
#include <QPushButton>
#include <QToolButton>
#include "QTabBar"

#include <QString>
#include <QDebug>

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
        // must initialize these property here, otherwise the new button is not draggable
//        btn->setDraggable(draggableMode);
//        btn->setCheckable(draggableMode);
//        btn->setAutoExclusive(true);

        btns.append(btn);

        // the position set of buttons here, this part need to be optimized.
        btn->setFixedSize(120, 40);
        QRect btnPos = QRect(QPoint(20 + 120 * (btns.size() / 8 ), 20 + 50 * (btns.size() % 8)), btn->size());
        btn->setGeometry(btnPos);
        const QString btnName = tr((QString("按钮") + QString::number(btns.size())).toUtf8().constData());
        btn->setText(btnName);
        btn->show();

        // to set the slot function for every button
        // use reference capture here to insure selectedButton is modified.
        connect(btn, &DraggableButton::clicked, this, [=](){
            if (btn == nullptr) {
                qDebug() << "btn is nullptr";
                return;
            }
            selectedButton = btn;
            if (selectedButton == nullptr) {
                qDebug() << "selectedButton is nullptr";
                return;
            }
            selectedButton->setChecked(draggableMode);
        });
    }

}

void ManualForm::on_btnDeleteButton_clicked()
{

    // examine if any button is clicked, delete the choosed one
    if (selectedButton && btns.contains(selectedButton))
    {
        btns.removeOne(selectedButton);
        delete selectedButton;
        selectedButton = nullptr;
    }
    // if no button is selected, delete the last in btns
    else if (!btns.isEmpty())
    {
        DraggableButton* btn = btns.takeLast();
        delete btn;
    }
//    if (btns.isEmpty() == false)
//    {
//        DraggableButton* btn = btns.last();
//        selectedButton = btn;

//        selectedButton->setChecked(true);
//    }
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
    draggableMode = arg1;

    for (auto btn : btns)
    {
        btn->setDraggable(draggableMode);
        // when the button is draaggable, it's also be checkable.
//        btn->setCheckable(draggableMode);
    }

}

void ManualForm::on_btnImportPicture_clicked()
{

    QString imagePath = "/opt/MachineTool/bin/backgroud.png";
//    QString imagePath = "./backgroud.png";
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
    selectedButton = nullptr;

//    // Initialize buttons and hide buttons 1-8
//    for (int i = 0; i < 20; ++i)
//    {
//        buttons[i] = findChild<DraggableButton*>(QString("btn%d").arg(i + 1));
//        if (buttons[i])
//        {
////            buttons[i]->setVisible(i >= 8);
//            buttons[i]->setVisible(false);
//        }
//    }
}
