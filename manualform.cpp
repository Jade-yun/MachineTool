#include "manualform.h"
#include "ui_manualform.h"

#include <QPushButton>
#include <QToolButton>
#include <QHeaderView>
#include "QTabBar"

#include <QString>
#include <QDebug>


#define Test 0
ManualForm::ManualForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ManualForm)
{
    ui->setupUi(this);

    initVar();

    // set tab to be invisible
//    ui->tabAxisAction->setVisible(false);
//    ui->tabWidgetManualPage->removeTab(1);

    tableReference = new QTableWidget(5, 5, ui->frameRerencePoint);

//    tableReference->resize(0, 0);
    // Set table to be uneditable
    tableReference->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // Set table to be unselectable
//    tableReference->setSelectionMode(QAbstractItemView::NoSelection);
    // Initialize table to be invisible
    tableReference->setVisible(referencePoints.size());

    tableReference->setMaximumHeight(41 * 3);
    tableReference->setMinimumWidth(140 * 4);
    tableReference->setMinimumWidth(760 );

    tableReference->setColumnCount(8); // 4 pairs of index and button name
    tableReference->verticalHeader()->setVisible(false); // Hide vertical header
    tableReference->horizontalHeader()->setVisible(false); // Hide horizontal header


    tableReference->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);// Hide scroll bar
    tableReference->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Disable automatic column resizing
//    tableReference->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    // must set minimum size for horizontalHeader, otherwise setColumnWidth can not work
    tableReference->horizontalHeader()->setMinimumSectionSize(30);
    for (int i = 0; i < tableReference->columnCount(); ++i) {
        tableReference->setColumnWidth(i, (i % 2 == 0) ? 30 : 120); // Alternate between 20 and 120 width
    }
    tableReferenceSigAndSlot();

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
#if Test
    if (referenceBtns.size() < 40)
    {
        DraggableButton* btn = new DraggableButton(ui->frameRerencePoint);
        // must initialize these property here, otherwise the new button is not draggable
                btn->setDraggable(draggable[1]);
                btn->setCheckable(true);
                btn->setAutoExclusive(true);

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
            selectedButton[1]->setChecked(true);

            // to press the corresponding item in referenceTable
            int index = referenceBtns.indexOf(btn);
            int row = index / 4;
            int col = (index % 4) * 2;
            // Select the corresponding item in the table
            tableReference->setCurrentCell(row, col + 1);
        });
        addPointToTable(btn);
    }
#else

    if (referencePoints.size() < 40)
    {

        DraggableButton* btn = new DraggableButton(ui->frameRerencePoint);
        // must initialize these property here, otherwise the new button is not draggable
        btn->setDraggable(draggable[1]);
        btn->setCheckable(true);
        btn->setAutoExclusive(true);
        btn->setFixedSize(40, 40);
        // set stylesheet
        btn->setStyleSheet("QPushButton{ border-style:solid; border-width:1px; border-radius:20px;}");

        ReferencePoint point;
        point.index = referencePoints.size() + 1;
        point.name = QString(tr("参考点%1").arg(point.index));
        point.button = btn;

        if (selectedButton[1])
        {
            referencePoints.insert(currentIndex, point);
//            for (int i = currentIndex + 1; i < referencePoints.size() + 1; i++)
//            {
//                referencePoints.at(i).index++;
//            }
        }
        else
        {
            referencePoints.append(point);
        }

        // the position set of buttons here, this part need to be optimized.
        QRect btnPos = QRect(QPoint(20 + 40 * ((referencePoints.size()-1) / 7 ), 150 + 45 * ((referencePoints.size()-1) % 7)), btn->size());
        btn->setGeometry(btnPos);
        const QString btnName = tr((QString::number(referencePoints.size())).toUtf8().constData());
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
            selectedButton[1]->setChecked(true);

            // how to get responding index of seletedButton in referencePoints
            // need to update currentIndex in tableselected slot
            for (int i = 0; i < referencePoints.size(); ++i) {
                if (referencePoints[i].button == btn) {
                    currentIndex = i;
                    break;
                }
            }
            if (currentIndex != -1) {
                qDebug() << "Button index in referencePoints:" << currentIndex;
            } else {
                qDebug() << "Button not found in referencePoints";
            }


            // to press the corresponding item in referenceTable
            int row = currentIndex / 4;
            int col = (currentIndex % 4) * 2;
            // Select the corresponding item in the table
            tableReference->setCurrentCell(row, col + 1);
        });

        addPointsToTable();
        tableReference->setVisible(referencePoints.size());
    }

#endif
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

# if Test
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

    removePointFromTable();
#else

    for (int i = 0; i < referencePoints.size(); ++i) {
        if (referencePoints[i].button == selectedButton[1]) {
            currentIndex = i;
            break;
        }
    }
    if (currentIndex != -1) {
        qDebug() << "Button index in referencePoints:" << currentIndex;
    } else {
        qDebug() << "Button not found in referencePoints";
    }

    // examine if any button is clicked, delete the choosed one
    if (selectedButton[1] && currentIndex > 0) // currentIndex > 0 condition is essiensial
    {
//        referencePoints.removeOne(selectedButton[1]);
       referencePoints.removeAt(currentIndex);
        delete selectedButton[1];
    }
    // if no button is selected, delete the last in btns
    else if (!referencePoints.isEmpty())
    {
        ReferencePoint point = referencePoints.takeLast();
        delete (point.button);
    }

    if (!referencePoints.isEmpty())
    {
        selectedButton[1] = referencePoints.last().button;
        selectedButton[1]->setChecked(true);

        tableReference->clear();
        addPointsToTable();
        tableReference->setVisible(referencePoints.size());
    }
#endif


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

//    for (auto btn : referenceBtns)
//    {
//        btn->setDraggable(draggable[1]);
//    }
    for(auto point : referencePoints)
    {
        point.button->setDraggable(draggable[1]);
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

void ManualForm::addPointsToTable()
{
#if Test

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
#else
    for (auto point : referencePoints)
    {
        int row = (point.index - 1) / 4;
        int col = ((point.index - 1) % 4) * 2;
        QTableWidgetItem *indexItem = new QTableWidgetItem(QString::number(point.index));
        indexItem->setFlags(indexItem->flags() & ~Qt::ItemIsEnabled);
//        indexItem->setCheckState(Qt::Unchecked);
        tableReference->setItem(row, col, indexItem);
        QTableWidgetItem * nameItem = new QTableWidgetItem(point.name);
        nameItem->setFlags(nameItem->flags() | Qt::ItemIsSelectable | Qt::ItemIsEditable);
        tableReference->setItem(row, col + 1, nameItem);
    }
#endif
}

#if 0
void ManualForm::removePointFromTable()
{
    QTableWidgetItem* item = tableReference->currentItem();
    if (!item)
         return;

    int row = item->row();
    int col = item->column();
    tableReference->removeCellWidget(row, col);
//    tableReference->removeCellWidget(row, col - 1);

    delete item;
    delete tableReference->takeItem(row, col - 1);

    int indexToDelete = referenceBtns.size();
    // Update the table if we have deleted an item

//    if (indexToDelete > 0)
//    {
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


//    if (indexToDelete != -1)
//    {
//        int row = indexToDelete / 4;
//        int col = (indexToDelete % 4) * 2;

//        delete tableReference->takeItem(row, col);
//        delete tableReference->takeItem(row, col + 1);

//        for (int i = indexToDelete + 1; i < referenceBtns.size() + 1; ++i)
//        {
//            int srcRow = i / 4;
//            int srcCol = (i % 4) * 2;
//            int destRow = (i - 1) / 4;
//            int destCol = ((i - 1) % 4) * 2;

//            tableReference->setItem(destRow, destCol, tableReference->takeItem(srcRow, srcCol));
//            tableReference->setItem(destRow, destCol + 1, tableReference->takeItem(srcRow, srcCol + 1));
//        }

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

}

#endif
void ManualForm::tableReferenceSigAndSlot()
{
    connect(tableReference, &QTableWidget::itemPressed, this, [=](QTableWidgetItem *item){
            int row = item->row();
            int col = item->column();
//            qDebug() << "Item pressed at row" << row << "column" << col;
            int index = 4 * row + col / 2;
            if ( 0 < index < referencePoints.size())
            {
//                qDebug() << "index = " << index;
                selectedButton[1] = referencePoints.at(index).button;
                selectedButton[1]->setChecked(true);
//                qDebug() << "seleted button is checked:" << selectedButton[1]->isChecked();
            }
        });
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

