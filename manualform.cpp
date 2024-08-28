#include "manualform.h"
#include "ui_manualform.h"

#include <QPushButton>
#include <QToolButton>
#include <QHeaderView>
#include <QTabBar>
#include <QString>
#include <QDebug>


#define	REFERENCE_TOTAL_NUM	40

ManualForm::ManualForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ManualForm)
{
    ui->setupUi(this);

    initVar();

    // set tab to be invisible
//    ui->tabAxisAction->setVisible(false);
//    ui->tabWidgetManualPage->removeTab(1);

    tableReferenceInit();
    pageInit();


    connect(ui->btnIntoStack, &QPushButton::clicked, this , [=](){
        emit sigShowStackPage();
    });
    /*********************************操作指引逻辑********************************************/
    connect(ui->editGuideKeyDef, &KeyEdit::saveKeyDef, this, [=](){
        // save the KeyDef setting in coresponding guidePoints
        if (selectedButton[0] && guidePoints.contains(selectedButton[0]))
        {
            guidePoints[selectedButton[0]].keyDefStr = ui->editGuideKeyDef->text();
            // save the corresponding parameters in struct GuidePara

        }
    });
    connect(ui->btnHideGuide, &QPushButton::toggled, this, [=](bool checked){

        ui->checkBoxEditPosGuide->setVisible(checked);
        ui->btnImportPicture->setVisible(checked);
        ui->btnNewButton->setVisible(checked);
        ui->btnDeleteButton->setVisible(checked);
    });
    connect(ui->btnHideReference, &QPushButton::toggled, this, [=](bool checked){

        ui->checkBoxEditPosReference->setVisible(checked);
        ui->btnImportPictureReference->setVisible(checked);
        ui->btnNewButtonReference->setVisible(checked);
        ui->btnDeleteButtonReference->setVisible(checked);
    });
}

ManualForm::~ManualForm()
{
    delete ui;
    qDeleteAll(guidePoints.keys());
}

void ManualForm::on_btnNewButton_clicked()
{
    if (guidePoints.size() < 20)
    {
        DraggableButton* btn = new DraggableButton(ui->tabOperate);
        btn->setDraggable(draggable[0]);
        btn->setCheckable(draggable[0]);
        btn->setAutoExclusive(true);

        btn->setFixedSize(120, 40);
        QRect btnPos = QRect(QPoint(20 + 120 * (guidePoints.size() / 8), 20 + 50 * (guidePoints.size() % 8)), btn->size());
        btn->setGeometry(btnPos);
        QString btnName = tr((QString("按钮") + QString::number(guidePoints.size() + 1)).toUtf8().constData());
        btn->setText(btnName);
        btn->show();

        // Create GuidePara for the new button
        GuidePara newGuidePara;
        newGuidePara.guideName = btnName;
        newGuidePara.keyDefStr = "0";

        // Insert into QHash
        guidePoints.insert(btn, newGuidePara);

        // Connect button click signal
        connect(btn, &DraggableButton::pressed, this, [=]() {
            selectedButton[0] = btn;
            selectedButton[0]->setChecked(draggable[0]);

            // Set the guide info in the UI as needed
            if (guidePoints.contains(selectedButton[0])) {
                GuidePara para = guidePoints.value(selectedButton[0]);
                // update correspongding keyDef name in UI，
                 ui->editGuideKeyDef->setText(para.keyDefStr);
            }
        });
    }
}

void ManualForm::on_btnDeleteButton_clicked()
{
    if (selectedButton[0] && guidePoints.contains(selectedButton[0]))
    {
        // Remove the button and corresponding GuidePara
        guidePoints.remove(selectedButton[0]);
        delete selectedButton[0];
        selectedButton[0] = nullptr;
    }
    else if (!guidePoints.isEmpty())
    {
        // Remove the first button in the QHash
        DraggableButton* btn = guidePoints.keys().first();
        guidePoints.remove(btn);
        delete btn;
    }
}

void ManualForm::on_checkBoxEditPosGuide_stateChanged(int arg1)
{
    // Update draggable state
    draggable[0] = arg1;

    // Iterate over all buttons in guidePoints
    for (auto btn : guidePoints.keys())
    {
        btn->setDraggable(draggable[0]);
        // Optionally, set the buttons to be checkable as well
        btn->setCheckable(draggable[0]);
    }
}

void ManualForm::on_btnNewButtonReference_clicked()
{
    if (referencePoints.size() < REFERENCE_TOTAL_NUM)
    {
        DraggableButton* btn = new DraggableButton(ui->frameRerencePoint);
        btn->setCheckable(true);
        btn->setAutoExclusive(true);
        btn->setDraggable(draggable[1]);
        btn->setFixedSize(40, 40);
        btn->setStyleSheet("QPushButton{ border-style:solid; border-width:1px; border-radius:20px;}");

        int newIndex = getNextAvailableIndex();
        if (newIndex == 0)
        {
            return;
        }
        QString btnName = QString("参考点%1").arg(newIndex);
        int posIndex = newIndex - 1;
        QPoint btnPos = QPoint(10 + 42 * (posIndex / 7), 130 + 42 * (posIndex % 7));

        ReferPointPara point = {newIndex, btnName, btn, btnPos};
        referencePoints.append(point);

        if (selectedButton[1] == nullptr)
        {
           btn->setChecked(true);
           selectedButton[1] = btn;
        }
        btn->setGeometry(QRect(btnPos, btn->size()));
        btn->setText(QString::number(point.index));
        btn->show();

        // this is used to update point geometry pos in ui
        connect(btn, &DraggableButton::released, this, [=](){
            auto it = std::find_if(referencePoints.begin(), referencePoints.end(), [=](const ReferPointPara& point) {
                return point.button == btn;
            });
            if (it != referencePoints.end())
            {
                it->pointPos = btn->pos();
            }
        });

        connect(btn, &DraggableButton::pressed, this, [=]() {
            selectedButton[1] = btn;

            int index = 0;
            auto it = std::find_if(referencePoints.begin(), referencePoints.end(), [=](const ReferPointPara& point) {
                return point.button == selectedButton[1];
            });
            if (it != referencePoints.end())
            {
                ui->textReferPointName->setText(it->name);
                index = it->index;
            }
            for (int row = 0; row < tableReference->rowCount(); ++row)
            {
                for (int col = 0; col < tableReference->columnCount(); ++col)
                {
                    QTableWidgetItem *indexItem = tableReference->item(row, col);
                    if (indexItem && indexItem->data(Qt::DisplayRole).toInt() == index)
                    {
//                        qDebug() << "Found item with armedIndex:" << index
//                                 << " at row:" << row
//                                 << " col:" << col;
                        tableReference->setCurrentCell(row, col + 1);
//                        tableReference->itemPressed(tableReference->itemAt(row, col + 1));
                        return ;
                    }
                }
            }
        });

        updateReferPointsTable();
    }
}

void ManualForm::on_btnDeleteButtonReference_clicked()
{
    if (selectedButton[1])
    {
        // 找到与 selectedButton[1] 关联的 ReferPointPara
        auto it = std::find_if(referencePoints.begin(), referencePoints.end(), [=](const ReferPointPara& point) {
            return point.button == selectedButton[1];
        });

        if (it != referencePoints.end())
        {
            delete it->button;
            referencePoints.erase(it);
            selectedButton[1] = nullptr;
        }
    }
    if (!referencePoints.isEmpty())
    {
        selectedButton[1] = referencePoints.last().button;
        selectedButton[1]->setChecked(true);
    }
    updateReferPointsTable();
}

void ManualForm::updateReferPointsTable()
{
    tableReference->clearContents();
    tableReference->setRowCount(referencePoints.count() / 4 + 1);

    QList<ReferPointPara> sortedPoints = referencePoints;
    std::sort(sortedPoints.begin(), sortedPoints.end(), [](const ReferPointPara &a, const ReferPointPara &b) {
        return a.index < b.index;
    });

    int num = 0;
    for (const auto &point : sortedPoints)
    {
        int row = num / 4;
        int col = (num % 4) * 2;
        num++;

        QTableWidgetItem *indexItem = new QTableWidgetItem(QString::number(point.index));
        indexItem->setFlags(indexItem->flags() & ~Qt::ItemIsEnabled);
        indexItem->setForeground(QBrush(Qt::black));
        tableReference->setItem(row, col, indexItem);

        QTableWidgetItem *nameItem = new QTableWidgetItem(point.name);
        tableReference->setItem(row, col + 1, nameItem);
    }

    for (int row = 0; row < tableReference->rowCount(); ++row)
    {
        for (int col = 0; col < tableReference->columnCount(); ++col)
        {
            if (!tableReference->item(row, col))
            {
                QTableWidgetItem *emptyItem = new QTableWidgetItem("");
                emptyItem->setFlags(emptyItem->flags() & ~(Qt::ItemIsSelectable | Qt::ItemIsEnabled));
                tableReference->setItem(row, col, emptyItem);
            }
        }
    }

    tableReference->setVisible(!referencePoints.isEmpty());
    // 选中当前checked button 对应的item
    if (selectedButton[1] == nullptr) return;

    int index = selectedButton[1]->text().toInt();
    for (int row = 0; row < tableReference->rowCount(); ++row)
    {
        for (int col = 0; col < tableReference->columnCount(); ++col)
        {
            QTableWidgetItem *indexItem = tableReference->item(row, col);
            if (indexItem && indexItem->data(Qt::DisplayRole).toInt() == index)
            {
                tableReference->setCurrentCell(row, col + 1);
                break;
            }
        }
    }
}

void ManualForm::on_checkBoxEditPosReference_stateChanged(int arg1)
{
    draggable[1] = arg1;

    for (auto point : referencePoints)
    {
        point.button->setDraggable(draggable[1]);
    }
}


void ManualForm::tableReferenceInit()
{
    tableReference = new QTableWidget(ui->frameRerencePoint);

    // Set table to be uneditable
    tableReference->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // Set table to be unselectable
//    tableReference->setSelectionMode(QAbstractItemView::NoSelection);
    // Initialize table to be invisible
    tableReference->setVisible(referencePoints.size());

    tableReference->setMaximumHeight(40 * 3);
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
    tableReference->verticalHeader()->setDefaultSectionSize(40);
    for (int i = 0; i < tableReference->columnCount(); ++i) {
        tableReference->setColumnWidth(i, (i % 2 == 0) ? 40 : 140); // Alternate between 40 and 140 width
    }

    connect(tableReference, &QTableWidget::itemPressed, this, [=](QTableWidgetItem *item) {
        int row = item->row();
        int col = item->column() - 1;
        QTableWidgetItem *indexItem = tableReference->item(row, col);
        int index = indexItem->data(Qt::DisplayRole).toInt();
//        if (indexItem && indexItem->data(Qt::DisplayRole).toInt() == index)

        auto it = std::find_if(referencePoints.begin(), referencePoints.end(), [=](const ReferPointPara& point){
            return point.index == index;
        });
        if (it != referencePoints.end())
        {
            DraggableButton* btn =  it->button;
            btn->setChecked(true);
            selectedButton[1] = btn;
            QString name = it->name;
            ui->textReferPointName->setText(name);
        }
    });

}

void ManualForm::pageInit()
{
    ui->checkBoxEditPosGuide->setVisible(false);
    ui->btnImportPicture->setVisible(false);
    ui->btnNewButton->setVisible(false);
    ui->btnDeleteButton->setVisible(false);

    ui->checkBoxEditPosReference->setVisible(false);
    ui->btnImportPictureReference->setVisible(false);
    ui->btnNewButtonReference->setVisible(false);
    ui->btnDeleteButtonReference->setVisible(false);
}

int ManualForm::getNextAvailableIndex()
{
    QSet<int> usedIndices;
    for (const auto &point : referencePoints)
    {
        usedIndices.insert(point.index);
    }
    for (int i = 1; i <= REFERENCE_TOTAL_NUM; ++i)
    {
        if (!usedIndices.contains(i))
        {
            return i;
        }
    }
    return 0;
}

const QList<ReferPointPara> &ManualForm::getRerferPoints() const
{
    return referencePoints;
}

void ManualForm::on_btnEditReference_clicked()
{
    if (selectedButton[1])
    {
        auto it = std::find_if(referencePoints.begin(), referencePoints.end(), [=](const ReferPointPara& point) {
            return point.button == selectedButton[1];
        });

        if (it != referencePoints.end())
        {
            // 实例化并配置 FullKeyboard 对话框
//            FullKeyboard temp(this);
            FullKeyboard *keyboard = FullKeyboard::instance();
            keyboard->setText(it->name);
            keyboard->setCurrentEditObj(ui->textReferPointName);

            if (keyboard->exec() == QDialog::Accepted)
            {
                it->name = ui->textReferPointName->toPlainText();
                updateReferPointsTable();
            }
        }
    }
}

void ManualForm::initVar()
{
    draggable[0] = false;
    draggable[1] = false;
    selectedButton[0] = nullptr;
    selectedButton[1] = nullptr;
}

void ManualForm::on_btnEditGuideName_clicked()
{
    if (selectedButton[0] == nullptr) return;

    QString guidName = selectedButton[0]->text();

    FullKeyboard *keyboard = FullKeyboard::instance();
    keyboard->setText(guidName);
    keyboard->setCurrentEditObj(nullptr);

    connect(keyboard, &FullKeyboard::enterPressed, this, &ManualForm::onChangeGuideName, Qt::UniqueConnection);

    keyboard->exec();
//    keyboard->show();
}

void ManualForm::onChangeGuideName(const QString& newText)
{
    if (selectedButton[0] != nullptr) {
        selectedButton[0]->setText(newText);
    }
}
