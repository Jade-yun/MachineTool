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
//        btn->setDraggable(draggable[0]);
//        btn->setCheckable(draggable[0]);
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
            selectedButton[0] = btn;
            if (selectedButton[0] == nullptr) {
                qDebug() << "selectedButton[0] is nullptr";
                return;
            }
            selectedButton[0]->setChecked(draggable[0]);
        });
    }

}

void ManualForm::on_btnNewButtonReference_clicked()
{
    if (referenceBtns.size() < 40)
    {
        DraggableButton* btn = new DraggableButton(ui->tabReference);
        // must initialize these property here, otherwise the new button is not draggable
//        btn->setDraggable(draggable[1]);
//        btn->setCheckable(draggable[1]);
//        btn->setAutoExclusive(true);

        referenceBtns.append(btn);

        btn->setFixedSize(40, 40);
        // set stylesheet
        btn->setStyleSheet("QPushButton{ border-style:solid; border-width:1px; border-radius:20px;}");
        // the position set of buttons here, this part need to be optimized.
        QRect btnPos = QRect(QPoint(20 + 45 * (referenceBtns.size() / 8 ), 20 + 50 * (referenceBtns.size() % 8)), btn->size());
        btn->setGeometry(btnPos);
        const QString btnName = tr((QString::number(referenceBtns.size())).toUtf8().constData());
        btn->setText(btnName);
        btn->show();

        // to set the slot function for every button
        // use reference capture here to insure selectedButton is modified.
        connect(btn, &DraggableButton::clicked, this, [=](){
            if (btn == nullptr) {
                qDebug() << "btn is nullptr";
                return;
            }
            selectedButton[1] = btn;
            if (selectedButton[1] == nullptr) {
                qDebug() << "selectedButton[1] is nullptr";
                return;
            }
            selectedButton[1]->setChecked(draggable[1]);
        });
    }
}

void ManualForm::on_btnDeleteButton_clicked()
{

    // examine if any button is clicked, delete the choosed one
    if (selectedButton[0] && btns.contains(selectedButton[0]))
    {
        btns.removeOne(selectedButton[0]);
        delete selectedButton[0];
        selectedButton[0] = nullptr;
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
//        selectedButton[0] = btn;

//        selectedButton[0]->setChecked(true);
//    }
}

void ManualForm::on_btnDeleteButtonReference_clicked()
{

    // examine if any button is clicked, delete the choosed one
    if (selectedButton[1] && referenceBtns.contains(selectedButton[1]))
    {
        referenceBtns.removeOne(selectedButton[1]);
        delete selectedButton[1];
        selectedButton[1] = nullptr;
    }
    // if no button is selected, delete the last in btns
    else if (!referenceBtns.isEmpty())
    {
        DraggableButton* btn = referenceBtns.takeLast();
        delete btn;
    }
//    if (referenceBtns.isEmpty() == false)
//    {
//        DraggableButton* btn = referenceBtns.last();
//        selectedButton[1] = btn;

//        selectedButton[1]->setChecked(true);
//    }
}


void ManualForm::on_checkBoxEditPos_stateChanged(int arg1)
{
//    for (int i = 0; i < 20; ++i)
//    {
//        if (buttons[i])
//        {
//            buttons[i]->setDraggable(arg1);
//        }
//    }
    draggable[0] = arg1;

    for (auto btn : btns)
    {
        btn->setDraggable(draggable[0]);
        // when the button is draaggable, it's also be checkable.
//        btn->setCheckable(draggable[0]);
    }

}
void ManualForm::on_checkBoxEditPosReference_stateChanged(int arg1)
{
    draggable[1] = arg1;

    for (auto btn : referenceBtns)
    {
        btn->setDraggable(draggable[1]);
        // when the button is draaggable, it's also be checkable.
//        btn->setCheckable(draggable[1]);
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

void ManualForm::on_btnImportPictureReference_clicked()
{

}

void ManualForm::initButtons()
{
    draggable[0] = false;
    draggable[1] = false;
    selectedButton[0] = nullptr;
    selectedButton[1] = nullptr;

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

