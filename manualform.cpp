﻿#include "manualform.h"
#include "ui_manualform.h"

#include <QPushButton>
#include <QToolButton>
#include <QHeaderView>
#include "QTabBar"

#include <QString>
#include <QDebug>

ManualForm::ManualForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ManualForm)
{
    ui->setupUi(this);

    initVar();


    tableReference = new QTableWidget(this);

//    tableReference->resize(0, 0);
    tableReference->setVisible(referenceBtns.size());

    tableReference->setMaximumHeight(41 * 3);
    tableReference->setMinimumWidth(140 * 4);
    tableReference->setMinimumWidth(760 );

    tableReference->setColumnCount(8); // 4 pairs of index and button name
    tableReference->verticalHeader()->setVisible(false); // Hide vertical header
    tableReference->horizontalHeader()->setVisible(false); // Hide horizontal header

//    // Set fixed column widths
//    for (int i = 0; i < tableReference->columnCount(); ++i) {
//        if (i % 2 == 0) {
//            tableReference->setColumnWidth(i, 20); // Index column width
//        } else {
//            tableReference->setColumnWidth(i, 120); // Button name column width
//        }
//    }

    // Disable automatic column resizing
//    tableReference->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    for (int i = 0; i < tableReference->columnCount(); ++i) {
        tableReference->setColumnWidth(i, (i % 2 == 0) ? 2 : 120); // Alternate between 20 and 120 width
    }

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
//        // must initialize these property here, otherwise the new button is not draggable
//        btn->setDraggable(draggable[0]);
//        btn->setCheckable(draggable[0]);
//        btn->setAutoExclusive(true);

        btns.append(btn);

        // the position set of buttons here, this part need to be optimized.
        btn->setFixedSize(120, 40);
        QRect btnPos = QRect(QPoint(20 + 120 * ((btns.size() - 1) / 8 ), 20 + 50 * ((btns.size() - 1) % 8)), btn->size());
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
        DraggableButton* btn = new DraggableButton(ui->frameRerencePoint);
        // must initialize these property here, otherwise the new button is not draggable
        //        btn->setDraggable(draggable[1]);
        //        btn->setCheckable(draggable[1]);
        //        btn->setAutoExclusive(true);

        referenceBtns.append(btn);

        btn->setFixedSize(40, 40);
        // set stylesheet
        btn->setStyleSheet("QPushButton{ border-style:solid; border-width:1px; border-radius:20px;}");
        // the position set of buttons here, this part need to be optimized.
        QRect btnPos = QRect(QPoint(20 + 40 * ((referenceBtns.size()-1) / 7 ), 150 + 45 * ((referenceBtns.size()-1) % 7)), btn->size());
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

        addPointToTable(btn);
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

    int indexToDelete = referenceBtns.size();
    // Update the table if we have deleted an item

//    if (indexToDelete > 0)
//    {
//        int row = indexToDelete / 4;
//        int col = (indexToDelete % 4) * 2;

//        // Remove the specific items
//        delete tableReference->takeItem(row, col);
//        delete tableReference->takeItem(row, col + 1);

//        // Shift remaining items up
//        for (int i = indexToDelete + 1; i < referenceBtns.size() + 1; ++i)
//        {
//            int srcRow = i / 4;
//            int srcCol = (i % 4) * 2;
//            int destRow = (i - 1) / 4;
//            int destCol = ((i - 1) % 4) * 2;

//            tableReference->setItem(destRow, destCol, tableReference->takeItem(srcRow, srcCol));
//            tableReference->setItem(destRow, destCol + 1, tableReference->takeItem(srcRow, srcCol + 1));
//        }

//        // Remove the last row if it's empty
//        int lastRow = (referenceBtns.size()) / 4;
//        if (lastRow < tableReference->rowCount())
//        {
//            bool isRowEmpty = true;
//            for (int i = 0; i < tableReference->columnCount(); ++i)
//            {
//                if (tableReference->item(lastRow, i) != nullptr)
//                {
//                    isRowEmpty = false;
//                    break;
//                }
//            }
//            if (isRowEmpty)
//            {
//                tableReference->removeRow(lastRow);
//            }
//        }
//    }


    if (indexToDelete != -1)
    {
        int row = indexToDelete / 4;
        int col = (indexToDelete % 4) * 2;

        delete tableReference->takeItem(row, col);
        delete tableReference->takeItem(row, col + 1);

        for (int i = indexToDelete + 1; i < referenceBtns.size() + 1; ++i)
        {
            int srcRow = i / 4;
            int srcCol = (i % 4) * 2;
            int destRow = (i - 1) / 4;
            int destCol = ((i - 1) % 4) * 2;

            tableReference->setItem(destRow, destCol, tableReference->takeItem(srcRow, srcCol));
            tableReference->setItem(destRow, destCol + 1, tableReference->takeItem(srcRow, srcCol + 1));
        }

        int lastRow = (referenceBtns.size()) / 4;
        if (lastRow < tableReference->rowCount())
        {
            bool isRowEmpty = true;
            for (int i = 0; i < tableReference->columnCount(); ++i)
            {
                if (tableReference->item(lastRow, i) != nullptr)
                {
                    isRowEmpty = false;
                    break;
                }
            }
            if (isRowEmpty)
            {
                tableReference->removeRow(lastRow);
            }
        }
    }
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

void ManualForm::addPointToTable(const QPushButton *button)
{
    int row = (referenceBtns.size() - 1) / 4;
    int col = ((referenceBtns.size() - 1)% 4) * 2;

    tableReference->setVisible(referenceBtns.size());

    if (row >= tableReference->rowCount()) {
        tableReference->insertRow(row);
        tableReference->setRowHeight(row, 40); // Set the height of the new row to 40
    }

//    QString indexText = QString(tr("%1")).arg(referenceBtns.size());
//    QTableWidgetItem *indexItem = new QTableWidgetItem(indexText);
//    indexItem->setFlags(indexItem->flags() & ~Qt::ItemIsSelectable & ~Qt::ItemIsEditable);
//    tableReference->setItem(row, col, indexItem);
//    tableReference->setColumnWidth(col, 5); // Set the width of the index column to 20

//    QString buttonName = QString(tr("参考点%1")).arg(referenceBtns.size());
//    QTableWidgetItem *buttonItem = new QTableWidgetItem(buttonName);
//    buttonItem->setFlags(buttonItem->flags() & ~Qt::ItemIsEditable);
//    tableReference->setItem(row, col + 1, buttonItem);
//    tableReference->setColumnWidth(col + 1, 120); // Set the width of the button name column to 120

//    tableReference->adjustSize();
    QString indexText = QString(tr("%1")).arg(referenceBtns.size());
    QTableWidgetItem *indexItem = new QTableWidgetItem(indexText);
    indexItem->setFlags(indexItem->flags() & ~Qt::ItemIsSelectable & ~Qt::ItemIsEditable);
    tableReference->setItem(row, col, indexItem);

    QString buttonName = QString(tr("参考点%1")).arg(referenceBtns.size());
    QTableWidgetItem *buttonItem = new QTableWidgetItem(buttonName);
    buttonItem->setFlags(buttonItem->flags() & ~Qt::ItemIsEditable);
    tableReference->setItem(row, col + 1, buttonItem);

//    tableReference->adjustSize();

}

void ManualForm::initVar()
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

