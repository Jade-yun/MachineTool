#include "manualform.h"
#include "ui_manualform.h"

#include <QPushButton>
#include <QToolButton>
#include <QHeaderView>
#include <QTabBar>
#include <QString>
#include <QDebug>

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

    tableReference->setMaximumHeight(30 * 4);
//    tableReference->setMinimumWidth(10 * 4);
    tableReference->setMinimumWidth(760);

    tableReference->setColumnCount(8); // 4 pairs of index and button name
    tableReference->verticalHeader()->setVisible(false); // Hide vertical header
    tableReference->horizontalHeader()->setVisible(false); // Hide horizontal header


    tableReference->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);// Hide scroll bar
    tableReference->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Disable automatic column resizing
//    tableReference->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    // must set minimum section size for horizontalHeader, otherwise setColumnWidth can not work
    tableReference->horizontalHeader()->setMinimumSectionSize(30);
    for (int i = 0; i < tableReference->columnCount(); ++i) {
        tableReference->setColumnWidth(i, (i % 2 == 0) ? 40 : 140); // Alternate between 40 and 140 width
    }
    tableReferenceSigAndSlot();


    /*********************************操作指引逻辑********************************************/
    connect(ui->editGuideKeyDef, &KeyEdit::textEdited, this, [=](QString text){
        // save the KeyDef setting in coresponding guidePoints
        qDebug() << "ui->editGuideKeyDef: " << text;
//        guidPoints.keyDef = text;

    });

}

ManualForm::~ManualForm()
{
    delete ui;
}

void ManualForm::on_btnNewButton_clicked()
{

    // to new button
    if (guidBtns.size() < 20)
    {
        DraggableButton* btn = new DraggableButton(ui->tabOperate);
//        // must initialize these property here, otherwise the new button is not draggable
//        btn->setDraggable(draggable[0]);
//        btn->setCheckable(draggable[0]);
//        btn->setAutoExclusive(true);

        guidBtns.append(btn);

        // the position set of buttons here, this part need to be optimized.
        btn->setFixedSize(120, 40);
        QRect btnPos = QRect(QPoint(20 + 120 * ((guidBtns.size() - 1) / 8 ), 20 + 50 * ((guidBtns.size() - 1) % 8)), btn->size());
        btn->setGeometry(btnPos);
        const QString btnName = tr((QString("按钮") + QString::number(guidBtns.size())).toUtf8().constData());
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

            //
//            ui->editGuideKeyDef->setText(guidePoints.keyDef);

        });
    }
}

void ManualForm::on_btnNewButtonReference_clicked()
{

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

        if (currentIndex != -1)
        {
            point.index = ++currentIndex;
        }
        else
        {
            point.index = referencePoints.size();
        }
//        qDebug() << "point.index = " << point.index;
        point.name = tr("参考点%1").arg(point.index + 1);
        point.button = btn;

        if (selectedButton[1])
        {
            referencePoints.insert(currentIndex, point);
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

            // get responding index of seletedButton in referencePoints
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

        // update the index for all the reference points when a new point was added to referencePoints,
        // actually it works just for these points after currentIndex.
        for (int i = 0; i < referencePoints.size(); i++)
        {
            referencePoints[i].index = i;

        }
        addPointsToTable();
        tableReference->setVisible(referencePoints.size());
    }

}

void ManualForm::on_btnDeleteButton_clicked()
{

    // examine if any button is clicked, delete the choosed one
    if (selectedButton[0] && guidBtns.contains(selectedButton[0]))
    {
        guidBtns.removeOne(selectedButton[0]);
        delete selectedButton[0];
        selectedButton[0] = nullptr;
    }
    // if no button is selected, delete the last in guidBtns
    else if (!guidBtns.isEmpty())
    {
        DraggableButton* btn = guidBtns.takeLast();
        delete btn;
    }
    //    if (guidBtns.isEmpty() == false)
    //    {
    //        DraggableButton* btn = guidBtns.last();
    //        selectedButton[0] = btn;

    //        selectedButton[0]->setChecked(true);
    //    }
}

void ManualForm::on_btnDeleteButtonReference_clicked()
{

    // clear the table at first
    tableReference->clear();

//    for (int i = 0; i < referencePoints.size(); ++i) {
//        if (referencePoints[i].button == selectedButton[1]) {
//            currentIndex = i;
//            break;
//        }
//    }
    currentIndex = getIndex(selectedButton[1]);
    if (currentIndex != -1) {
        qDebug() << "Button index in referencePoints:" << currentIndex;
    } else {
        qDebug() << "Button not found in referencePoints";
    }

    // examine if any button is clicked, delete the choosed one
    if (selectedButton[1] && currentIndex > 0 && currentIndex < referencePoints.size()+1) // currentIndex > 0 condition is essiensial
    {
//        referencePoints.removeOne(selectedButton[1]);
       referencePoints.removeAt(currentIndex);
        delete selectedButton[1];
    }
    // if no button is selected, delete the last one
    else if (!referencePoints.isEmpty())
    {
        ReferencePoint point = referencePoints.takeLast();
        delete (point.button);
    }

    if (!referencePoints.isEmpty())
    {       
        for (int i = 0; i < referencePoints.size(); i++)
        {
            referencePoints[i].index = i;

        }
        addPointsToTable();
        tableReference->setVisible(referencePoints.size());

        selectedButton[1] = referencePoints.last().button;
        selectedButton[1]->setChecked(true);
        // emit signal to update currentIndex and selete corresponding item in the table
        // need to be optimized here, cause meaningless self assignment operations will be performed
        emit selectedButton[1]->clicked();
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

    for (auto btn : guidBtns)
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
    for (auto point : referencePoints)
    {
        int row = point.index / 4;
        int col = (point.index % 4) * 2;
        QTableWidgetItem *indexItem = new QTableWidgetItem(QString::number(point.index + 1));
        indexItem->setFlags(indexItem->flags() & ~Qt::ItemIsEnabled);
//        indexItem->setCheckState(Qt::Unchecked);
        tableReference->setItem(row, col, indexItem);
        QTableWidgetItem * nameItem = new QTableWidgetItem(point.name);
        nameItem->setFlags(nameItem->flags() | Qt::ItemIsSelectable | Qt::ItemIsEditable);
        tableReference->setItem(row, col + 1, nameItem);
    }
}

void ManualForm::tableReferenceSigAndSlot()
{
    connect(tableReference, &QTableWidget::itemPressed, this, [=](QTableWidgetItem *item){
            int row = item->row();
            int col = item->column();
//            qDebug() << "Item pressed at row" << row << "column" << col;
            int index = 4 * row + col / 2;
            if (index > 0 && index < referencePoints.size())
            {
//                qDebug() << "index = " << index;
                selectedButton[1] = referencePoints.at(index).button;
                selectedButton[1]->setChecked(true);
//                qDebug() << "seleted button is checked:" << selectedButton[1]->isChecked();
            }
    });
}

int ManualForm::getIndex(const DraggableButton *button) const
{
    for (int i = 0; i < referencePoints.size(); ++i)
    {
        if (referencePoints[i].button == button)
        {
            return i;
        }
    }
    return -1;
}

#ifdef SAVEPOINT_VERSION_1
void ManualForm::saveState(QWidget *parent, const QString &settingsFile)
{
    QSettings settings(settingsFile, QSettings::IniFormat);
//    settings.clear();
    settings.beginGroup("Widgets");

    const QList<QWidget *> widgets = parent->findChildren<QWidget *>(QString(), Qt::FindDirectChildrenOnly);
    for (QWidget *widget : widgets) {
        QString key = widget->objectName();
        settings.beginGroup(key);

        settings.setValue("geometry", widget->geometry());
        settings.setValue("stylesheet", widget->styleSheet());

        if (QPushButton *button = qobject_cast<QPushButton *>(widget)) {
            settings.setValue("type", "QPushButton");
            settings.setValue("text", button->text());
        } else if (QTableWidget *tableWidget = qobject_cast<QTableWidget *>(widget)) {
            settings.setValue("type", "QTableWidget");
//            settings.setValue("item", tableWidget->i);
            // Save additional QTableWidget properties as needed
        }
        else if (QLineEdit *lineEdit = qobject_cast<QLineEdit *>(widget)) {
            settings.setValue("type", "QLineEdit");
            settings.setValue("text", lineEdit->text());
        } else if (QCheckBox *checkBox = qobject_cast<QCheckBox *>(widget)) {
            settings.setValue("type", "QCheckBox");
            settings.setValue("checked", checkBox->isChecked());
        }

        settings.endGroup();
    }
    settings.endGroup();
}

void ManualForm::restoreState(QWidget *parent, const QString &settingsFile)
{

    QSettings settings(settingsFile, QSettings::IniFormat);
    settings.beginGroup("Widgets");

    const QStringList keys = settings.childGroups();
    for (const QString &key : keys) {
        settings.beginGroup(key);

        QString type = settings.value("type").toString();
        QWidget *widget = parent->findChild<QWidget *>(key);

        if (!widget) {
            if (type == "QLineEdit") {
                widget = new QLineEdit(parent);
            } else if (type == "QCheckBox") {
                widget = new QCheckBox(parent);
            } else if (type == "QPushButton") {
                widget = new QPushButton(parent);
            } else if (type == "QTableWidget") {
                widget = new QTableWidget(parent);
                // Restore additional QTableWidget properties as needed
            }
            widget->setObjectName(key);
        }

        widget->setGeometry(settings.value("geometry").toRect());
        widget->setStyleSheet(settings.value("stylesheet").toString());

        if (QLineEdit *lineEdit = qobject_cast<QLineEdit *>(widget)) {
            lineEdit->setText(settings.value("text").toString());
        } else if (QCheckBox *checkBox = qobject_cast<QCheckBox *>(widget)) {
            checkBox->setChecked(settings.value("checked").toBool());
        } else if (QPushButton *button = qobject_cast<QPushButton *>(widget)) {
            button->setText(settings.value("text").toString());
        }

        widget->show();
        settings.endGroup();
    }
    settings.endGroup();
}
#else
void ManualForm::saveStateHelper(QSettings &settings, QWidget *widget)
{
    settings.beginGroup(widget->objectName());

    settings.setValue("geometry", widget->geometry());
    settings.setValue("stylesheet", widget->styleSheet());

    if (QLineEdit *lineEdit = qobject_cast<QLineEdit *>(widget)) {
        settings.setValue("type", "QLineEdit");
        settings.setValue("text", lineEdit->text());
    } else if (QCheckBox *checkBox = qobject_cast<QCheckBox *>(widget)) {
        settings.setValue("type", "QCheckBox");
        settings.setValue("checked", checkBox->isChecked());
    } else if (QPushButton *button = qobject_cast<QPushButton *>(widget)) {
        settings.setValue("type", "QPushButton");
        settings.setValue("text", button->text());
    } else if (QTableWidget *tableWidget = qobject_cast<QTableWidget *>(widget)) {
        settings.setValue("type", "QTableWidget");
        // Save additional QTableWidget properties as needed
    }

    const QList<QWidget *> children = widget->findChildren<QWidget *>(QString(), Qt::FindDirectChildrenOnly);
    for (QWidget *child : children) {
        saveStateHelper(settings, child);
    }

    settings.endGroup();
}


void saveState(QWidget *parent, const QString &settingsFile) {
    QSettings settings(settingsFile, QSettings::IniFormat);
    settings.clear();
    settings.beginGroup("Widgets");
    saveStateHelper(settings, parent);
    settings.endGroup();
}


void ManualForm::restoreStateHelper(QSettings &settings, QWidget *parent, const QString &group)
{
    settings.beginGroup(group);

    QWidget *widget = nullptr;
    QString type = settings.value("type").toString();

    if (type == "QLineEdit") {
        widget = new QLineEdit(parent);
        static_cast<QLineEdit *>(widget)->setText(settings.value("text").toString());
    } else if (type == "QCheckBox") {
        widget = new QCheckBox(parent);
        static_cast<QCheckBox *>(widget)->setChecked(settings.value("checked").toBool());
    } else if (type == "QPushButton") {
        widget = new QPushButton(parent);
        static_cast<QPushButton *>(widget)->setText(settings.value("text").toString());
    } else if (type == "QTableWidget") {
        widget = new QTableWidget(parent);
        // Restore additional QTableWidget properties as needed
    } else {
        widget = new QWidget(parent);
    }

    widget->setObjectName(group);
    widget->setGeometry(settings.value("geometry").toRect());
    widget->setStyleSheet(settings.value("stylesheet").toString());

    const QStringList childGroups = settings.childGroups();
    for (const QString &childGroup : childGroups) {
        restoreStateHelper(settings, widget, childGroup);
    }


    settings.endGroup();
}

void restoreState(QWidget *parent, const QString &settingsFile)
{
    QSettings settings(settingsFile, QSettings::IniFormat);
    settings.beginGroup("Widgets");
    restoreStateHelper(settings, parent, parent->objectName());
    settings.endGroup();
}

#endif

void ManualForm::initVar()
{
    draggable[0] = false;
    draggable[1] = false;
    selectedButton[0] = nullptr;
    selectedButton[1] = nullptr;

    currentIndex = -1;
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

