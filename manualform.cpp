#include "manualform.h"
#include "ui_manualform.h"

#include <QPushButton>
#include <QToolButton>
#include <QHeaderView>
#include <QTabBar>
#include <QString>
#include <QDebug>

#include "cmd.h"
#include "mainwindow.h"
#include "RefreshKernelBuffer.h"

QString REFERPOINT_PIC = "/opt/MachineTool/resources/pics/referPoint.png";
const QString GuidePicPath = "/opt/MachineTool/resources/pics/guide.png";

ManualForm::ManualForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ManualForm)
{
    ui->setupUi(this);

    initVar();

    ui->stkWgtAdjustMachine->setCurrentIndex(0);
    // set tab to be invisible
//    ui->tabAxisAction->setVisible(false);
//    ui->tabWidgetManualPage->removeTab(1);
    initControls();

    tableReferenceInit();
    pageInit();
    setupReserveWidgets();

    ui->tabGuide->installEventFilter(this);
    ui->tabReference->installEventFilter(this);
    ui->tabClawAndMachine->installEventFilter(this);
    ui->tabWidgetManualPage->installEventFilter(this);

    for (auto cobox : findChildren<QComboBox*>())
    {
        cobox->setFocusPolicy(Qt::NoFocus);
    }

    ui->AdjustMachineInternalPulse->setDecimalPlaces(0);
    ui->editAxisActionSpeed->setDecimalPlaces(0);
    ui->editAxisActionSpeed->setInputRange(1, 100);
    ui->editPositionAdd->setDecimalPlaces(2);
    ui->editPositionAdd->setInputRange(0.01, 1000.00);
    ui->editPositionSub->setDecimalPlaces(2);
    ui->editPositionSub->setInputRange(0.01, 1000.00);

    this->setupAxisActionConnections();

    connect(ui->btnIntoStack, &QPushButton::clicked, this , [=](){
        emit sigShowStackPage();
    });

    setupClawAndMachineConnections();

    StateButtonInit();//初始化相关状态按钮状态

    /*********************************操作指引逻辑********************************************/
    connect(ui->editGuideKeyDef, &KeyEdit::returnPressed, this, &ManualForm::onSaveKeyDef);

    connect(ui->btnHideGuide, &QPushButton::toggled, this, [=](bool checked){

        ui->checkBoxEditPosGuide->setVisible(checked);
        ui->btnImportPictureGuide->setVisible(checked);
        ui->btnNewButton->setVisible(checked);
        ui->btnDeleteButton->setVisible(checked);
    });
    /*********************************参考点逻辑********************************************/
    connect(ui->btnHideReference, &QPushButton::toggled, this, [=](bool checked){

        ui->checkBoxEditPosReference->setVisible(checked);
        ui->btnImportPictureReference->setVisible(checked);
        ui->btnNewButtonReference->setVisible(checked);
        ui->btnDeleteButtonReference->setVisible(checked);
    });
    connect(ui->btnMovetoReference,&QPushButton::pressed,this,[=](){
        auto it = std::find_if(referencePoints.begin(), referencePoints.end(), [=](const ReferPointPara& point) {
            return point.button == selectedButton[1];
        });
        if (it != referencePoints.end())
        {
            uint8_t AxisIndex = ui->tableWgtAxisPos->currentRow();
            if(AxisIndex>=0 && AxisIndex<AXIS_TOTAL_NUM)
            {
                if(m_AxisPar[AxisIndex].axisType == 1)
                {
                    MoveReferencePos = it->axisPos[AxisIndex];
                    g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_AXIS, AxisIndex+1,3);//运动到目标位置
                }
            }
        }
    });
    connect(ui->btnMovetoReference,&QPushButton::released,this,[=](){
        auto it = std::find_if(referencePoints.begin(), referencePoints.end(), [=](const ReferPointPara& point) {
            return point.button == selectedButton[1];
        });
        if (it != referencePoints.end())
        {
            uint8_t AxisIndex = ui->tableWgtAxisPos->currentRow();
            if(AxisIndex>=0 && AxisIndex<AXIS_TOTAL_NUM)
            {
                if(m_AxisPar[AxisIndex].axisType == 1)
                {
                    g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_AXIS, AxisIndex+1,0);//抬起的时候停止发送移动指令
                }
            }
        }
    });
    /*********************************轴动作********************************************/
    update_Button_Name_Handel();
}

ManualForm::~ManualForm()
{
    delete ui;
    qDeleteAll(guidePoints.keys());
}


//手动界面-调机功能-轴选择复选框
void ManualForm::on_coboxAdjustMachineAxis_currentIndexChanged(int index)
{
    InitAdjustMachine(index);
}

//初始化调机界面参数
void ManualForm::InitAdjustMachine(uint8_t AxisIndex)
{
    if(ui->coboxAdjustMachineAxis->count()>0 && ui->coboxAdjustMachineAxis->count()>=AxisIndex)
    {
        ui->coboxAdjustMachineAxis->setCurrentIndex(AxisIndex);//默认为伺服第一个可使用轴
        for(int i=0;i<AXIS_TOTAL_NUM;i++)
        {
            if(ui->coboxAdjustMachineAxis->currentText() == m_NameDefine[1].axisName[i])
            {
                ui->AdjustMachineSpace->setText(QString::number((double)(m_AxisPar[i].circleDis)/100,'f',2));//每转距离
                ui->AdjustMachineEncoderPulse->setText(QString::number(0));//编码器脉冲
                ui->AdjustMachineInternalPulse->setText(QString::number(m_AxisPar[i].circlePluseNum));//内部脉冲
            }
        }
    }
}

void ManualForm::initControls()
{
    if(m_manualAxis.axis>0)
    {
        for(auto i=0;i<ui->cb_axisActionAxis->count();i++)
        {
            if(ui->cb_axisActionAxis->itemText(i) == m_NameDefine[1].axisName[m_manualAxis.axis])
            {
                ui->cb_axisActionAxis->setCurrentIndex(i);
            }
        }
    }
    else
    {
        ui->cb_axisActionAxis->setCurrentIndex(m_manualAxis.axis);
    }
    ui->editAxisActionSpeed->setText(QString::number(m_manualAxis.speed));
    ui->editPositionAdd->setText(QString::number(m_manualAxis.pos_pos/100.0));
    ui->editPositionSub->setText(QString::number(m_manualAxis.sub_pos/100.0));
    ui->chbZAxisDesend->setChecked(m_manualAxis.ZDrop);
}

void ManualForm::on_btnNewButton_clicked()
{
    if (guidePoints.size() < GUIDE_TOTAL_NUM)
    {
        DraggableButton* btn = new DraggableButton(ui->tabGuide);
        btn->setDraggable(draggable[0]);
        btn->setCheckable(draggable[0]);
        btn->setAutoExclusive(true);

        btn->setFixedSize(120, 45);
        QPoint btnPos = QPoint(20 + 120 * (guidePoints.size() / 8), 20 + 50 * (guidePoints.size() % 8));
        btn->setGeometry(QRect(btnPos, btn->size()));
        QString btnName = tr((QString("按钮") + QString::number(guidePoints.size() + 1)).toUtf8().constData());
        btn->setText(btnName);
        btn->show();

        // Create GuidePara for the new button
        GuidePara newGuidePara = { btnName, "0", btnPos, 0, 0 ,0};

        // Insert into QHash
        guidePoints.insert(btn, newGuidePara);

        //setup connections to perform responsible logic
        setupGuidePointConnections(btn);

        ui->btnSaveGuide->setParaChangedFlag(true);
    }
}

void ManualForm::on_btnDeleteButton_clicked()
{
    if (selectedButton[0] && guidePoints.contains(selectedButton[0]))
    {
        QString guideName = guidePoints.value(selectedButton[0]).guideName;
        ErrorTipDialog tip(tr("确认删除：") + guideName, nullptr);
        int reply = tip.exec();
        if (reply == QDialog::Accepted)
        {
           // Remove the button and corresponding GuidePara
           guidePoints.remove(selectedButton[0]);
           delete selectedButton[0];

           ui->btnSaveGuide->setParaChangedFlag(true);
       }
    }

    if (guidePoints.isEmpty())
    {
        selectedButton[0] = nullptr;
    }
    else
    {
        DraggableButton* btn = guidePoints.keys().first();
        btn->setChecked(true);

        selectedButton[0] = btn;
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

    if (selectedButton[0])
    {
        selectedButton[0]->setChecked(draggable[0]);
        update();
    }
}

void ManualForm::on_btnNewButtonReference_clicked()
{
    if (referencePoints.size() < REFERENCE_TOTAL_NUM)
    {
        addReferencePoint();
        updateReferPointsTable();
        ui->btnSaveReference->setParaChangedFlag(true);
    }
}

void ManualForm::on_btnRefresh_clicked()
{
    if (selectedButton[1])
    {
        auto it = std::find_if(referencePoints.begin(), referencePoints.end(), [=](const ReferPointPara& point) {
            return point.button == selectedButton[1];
        });

        if (it != referencePoints.end())
        {
            const std::array<int, AXIS_TOTAL_NUM> curAxisPos = {
                m_AxisCurPos.Pos_x, m_AxisCurPos.Pos_y, m_AxisCurPos.Pos_z,
                m_AxisCurPos.Pos_c, m_AxisCurPos.Pos_y2, m_AxisCurPos.Pos_z2
            };

            bool paramChangedFlag = false;
            for (int i = 0; i < AXIS_TOTAL_NUM; i++)
            {
                if (it->axisPos[i] != curAxisPos[i])
                {
                    it->axisPos[i] = curAxisPos[i];
                    paramChangedFlag = true;
                }
            }
            if (paramChangedFlag)
            {
                refreshPosTable();
            }
            ui->btnSaveReference->setParaChangedFlag(paramChangedFlag);
        }
    }
}

void ManualForm::on_btnDeleteButtonReference_clicked()
{
    if (!referencePoints.isEmpty())
    {
        if (selectedButton[1])
        {
            auto it = std::find_if(referencePoints.begin(), referencePoints.end(), [=](const ReferPointPara& point) {
                return point.button == selectedButton[1];
            });
            if (it != referencePoints.end())
            {
                QString referName = it->name;
                ErrorTipDialog tip(tr("确定删除：") + referName);
                int reply = tip.exec();
                if (reply == QDialog::Accepted)
                {
                    removeReferencePoint();
                    updateReferPointsTable();
                    ui->btnSaveReference->setParaChangedFlag(true);
                }
            }
        }
    }
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
    tableReference->setFrameShape(QFrame::NoFrame);
    tableReference->setFrameShadow(QFrame::Raised);

    tableReference->setMaximumHeight(40 * 3);
    tableReference->setFixedWidth(760);

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

            refreshPosTable();
        }
    });

}

void ManualForm::pageInit()
{
    ui->checkBoxEditPosGuide->setVisible(false);
    ui->btnImportPictureGuide->setVisible(false);
    ui->btnNewButton->setVisible(false);
    ui->btnDeleteButton->setVisible(false);

    ui->checkBoxEditPosReference->setVisible(false);
    ui->btnImportPictureReference->setVisible(false);
    ui->btnNewButtonReference->setVisible(false);
    ui->btnDeleteButtonReference->setVisible(false);

    ui->tableWgtAxisPos->setHorizontalHeaderLabels({tr("轴位置")});

    QPixmap picReferPoint(REFERPOINT_PIC);
    picReferPoint.scaled(ui->labReferPointBackGround->size());
    ui->labReferPointBackGround->setPixmap(picReferPoint);

    ui->labelGuideBackgroud->setPixmap(QPixmap(GuidePicPath));

    ui->btnAutoGate1Close->setEnabled(true);
    ui->btnStartProcess1Break->setEnabled(false);
    ui->btnMainAxisLocate1Break->setEnabled(false);
    ui->btnControlRotate1Break->setEnabled(false);

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

void ManualForm::updateGuidePoints()
{

    for (auto it = guidePoints.begin(); it != guidePoints.end(); ++it)
    {
        DraggableButton* btn = it.key();
        if (btn)
        {
            delete btn;
            btn = nullptr;
        }
    }
    guidePoints.clear();

    for (int i = 0; i < GUIDE_TOTAL_NUM; i++)
    {
        if (m_Guide[i].guideFlag == false) continue; // 操作点无效

        QString guideName = m_Guide[i].guideName;
        QString keyDefStr = m_Guide[i].keyDefStr;
        QPoint btnPos =  m_Guide[i].btnPos;
        uint8_t keyType = m_Guide[i].keyType;
        uint8_t portNum = m_Guide[i].portNum;
        uint8_t status = m_Guide[i].status;
        GuidePara guide { guideName, keyDefStr, btnPos, keyType, portNum, status };

        DraggableButton* btn = new DraggableButton(ui->tabGuide);
        btn->setAutoExclusive(true);
        guidePoints.insert(btn, guide);

        // setup connections to handle
        setupGuidePointConnections(btn);
    }

    for (auto it = guidePoints.begin(); it != guidePoints.end(); ++it)
    {
        DraggableButton* btn = it.key();
        const GuidePara& guide = it.value();

        if (btn) {
//            bool state = ui->checkBoxEditPosGuide->isChecked();
            btn->setDraggable(draggable[0]);
            btn->setText(guide.guideName);
            btn->setGeometry(QRect(guide.btnPos, QSize(120, 45)));
        }
    }

    if (guidePoints.isEmpty()) {
        selectedButton[0] = nullptr;
    }
    else {
        selectedButton[0] = guidePoints.begin().key();
    }

}

void ManualForm::setupGuidePointConnections(DraggableButton *btn)
{
    connect(btn, &DraggableButton::pressed, this, [this, btn]() {
        selectedButton[0] = btn;
        selectedButton[0]->setChecked(draggable[0]);

        // Check if the button exists in the guidePoints map
        auto it = guidePoints.find(selectedButton[0]);
        if (it != guidePoints.end()) {
            // Update corresponding keyDef name in UI
            ui->editGuideKeyDef->setText(it.value().keyDefStr);
        }
    });

    connect(btn, &DraggableButton::positionChanged, [=]() {
        auto it = guidePoints.find(selectedButton[0]);
        if (it != guidePoints.end()) {
            it.value().btnPos = btn->pos();
            ui->btnSaveGuide->setParaChangedFlag(true);
        }
    });
    connect(btn, &DraggableButton::clicked, this, [this, btn]() {
        // Retrieve guide information from the guidePoints map
        const auto it = guidePoints.find(btn);
        if (it != guidePoints.end()) {
            const GuidePara& para = it.value();

            qDebug() << "keyType:" << para.keyType << ",port:" << para.portNum << ",status:" << para.status;

            // 0阀输出 1输出 2轴
            if(para.keyType == 0)
            {
                g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT, para.portNum, para.status);
            }
            else if(para.keyType == 1)
            {

            }
//            else if(para.keyType == 2)
//            {
//                int axisIndex = para.portNum;
//                int direct = para.status == 1 ? 1 : 2;
//                g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_INCREMENT, axisIndex, direct);
//            }
        }
    });
    connect(btn, &DraggableButton::pressed, this, [this, btn]() {
        // Retrieve guide information from the guidePoints map
        const auto it = guidePoints.find(btn);
        if (it != guidePoints.end()) {
            const GuidePara& para = it.value();

            qDebug() << "keyType:" << para.keyType << ",port:" << para.portNum << ",status:" << para.status;

            // 0阀输出 1输出 2轴
            if(para.keyType == 2)
            {//如果是轴输出，需要长按移动，松开停止
                int axisIndex = para.portNum;
                int direct = para.status == 1 ? 1 : 2;
               if(axisIndex>0)
               {
                   if(m_AxisPar[m_KeyFunc[axisIndex-1].funcNum-1].axisType == 1)
                   {
                       g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_AXIS, axisIndex, direct);
                   }
               }
               else
               {
                   qDebug()<<tr("操作索引按键轴编号有误！");
               }
            }
        }
    });
    connect(btn, &DraggableButton::released, this, [this, btn]() {
        // Retrieve guide information from the guidePoints map
        const auto it = guidePoints.find(btn);
        if (it != guidePoints.end()) {
            const GuidePara& para = it.value();

            qDebug() << "keyType:" << para.keyType << ",port:" << para.portNum << ",status:" << para.status;

            // 0阀输出 1输出 2轴
            if(para.keyType == 2)
            {//如果是轴输出，需要长按移动，松开停止
                int axisIndex = para.portNum;
                int direct = 0;//按键松开时停止轴移动
               if(axisIndex>0)
               {
                   if(m_AxisPar[m_KeyFunc[axisIndex-1].funcNum-1].axisType == 1)
                   {
                       g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_AXIS, axisIndex, direct);
                   }
               }
               else
               {
                   qDebug()<<tr("操作索引按键轴编号有误！");
               }
            }
        }
    });
}

void ManualForm::updateReferPointsList()
{
    for (auto& point : referencePoints)
    {
        delete point.button;
        point.button = nullptr;
    }
    referencePoints.clear();

    for (int i = 0; i < REFERENCE_TOTAL_NUM; i++)
    {
        if (true == m_RefPoint[i].refFlag)
        {
            QString referName = m_RefPoint[i].refName;
            int refIndex = i + 1;
            DraggableButton* btn = new DraggableButton(ui->frameRerencePoint);
            QPoint btnPos(m_RefPoint[i].xPos, m_RefPoint[i].yPos);
            ReferPointPara point = {refIndex, referName, btn, btnPos,
                                    {m_RefPoint[i].pos[0], m_RefPoint[i].pos[1], m_RefPoint[i].pos[2],
                                     m_RefPoint[i].pos[3], m_RefPoint[i].pos[4],m_RefPoint[i].pos[5]}};
            referencePoints.push_back(point);

            btn->setCheckable(true);
            btn->setAutoExclusive(true);
            btn->setDraggable(draggable[1]);
            btn->setFixedSize(40, 40);
            btn->setStyleSheet("QPushButton{ border-style:solid; outline:none; border-width:1px; border-radius:20px;}");

            btn->move(point.pointPos);
            btn->setText(QString::number(point.index));
            btn->show();

            setupReferPointConnections(btn);
        }
    }
    if (referencePoints.isEmpty())
    {
        selectedButton[1] = nullptr;
    }
    else
    {
        selectedButton[1] = referencePoints.at(0).button;
        selectedButton[1]->setChecked(true);
        onReferPointButtonPressed(selectedButton[1]);
    }

    updateReferPointsTable();

}

void ManualForm::setupReferPointConnections(DraggableButton *btn)
{
    // this is used to update point geometry pos in ui
    connect(btn, &DraggableButton::positionChanged, this, [=](){
        auto it = std::find_if(referencePoints.begin(), referencePoints.end(), [=](const ReferPointPara& point) {
            return point.button == btn;
        });
        if (it != referencePoints.end())
        {
            it->pointPos = btn->pos();
            ui->btnSaveReference->setParaChangedFlag(true);
        }
    });

    connect(btn, &DraggableButton::pressed, this, [=]() {
        if (selectedButton[1] == btn) return;

        selectedButton[1] = btn;

        onReferPointButtonPressed(btn);
    });

}

void ManualForm::onReferPointButtonPressed(DraggableButton *btn)
{
    int index = 0;
    auto it = std::find_if(referencePoints.begin(), referencePoints.end(), [=](const ReferPointPara& point) {
        return point.button == selectedButton[1];
    });
    if (it != referencePoints.end())
    {
        ui->textReferPointName->setText(it->name);
        index = it->index;
        refreshPosTable();
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
}

void ManualForm::updateReferPointsAxisPos()
{
    for (auto& point : referencePoints)
    {
        int arrayIndex = point.index - 1;
        for (int i = 0; i < AXIS_TOTAL_NUM; i++)
        {
            point.axisPos[i] =  m_RefPoint[arrayIndex].pos[i];
        }
    }
}

void ManualForm::refreshPosTable()
{
    static const QStringList axisNames = {
        "X1", "Y1", "Z1", "C", "Y2", "Z2"
    };
    ui->tableWgtAxisPos->setRowCount(AXIS_TOTAL_NUM);
    ui->tableWgtAxisPos->setVerticalHeaderLabels(axisNames);
    ui->tableWgtAxisPos->setHorizontalHeaderLabels({tr("轴位置")});

    ui->tableWgtAxisPos->clearContents();  // 清除旧数据

    auto it = std::find_if(referencePoints.begin(), referencePoints.end(), [=](const ReferPointPara& point) {
        return point.button == selectedButton[1];
    });
    if (it != referencePoints.end())
    {
        for (size_t i = 0; i < AXIS_TOTAL_NUM; i++)
        {
            if (m_AxisPar[i].axisType == 1)
            {
                auto item = new QTableWidgetItem(QString::number(it->axisPos[i] / 100.0, 'f', 2));
                ui->tableWgtAxisPos->setItem(i, 0, item);
            }
            else
            {
                ui->tableWgtAxisPos->hideRow(i);
            }
        }
    }
}

void ManualForm::saveManualAxisActionPara()
{
    int AxisIndex = 0;
    if(ui->cb_axisActionAxis->count() > 1)
    {
        QString selectAxis = ui->cb_axisActionAxis->currentText();
        for(int i = 0; i < AXIS_TOTAL_NUM; i++)
        {
            if(selectAxis == m_NameDefine[1].axisName[i])
            {
                AxisIndex = i + 1;
                break;
            }
        }
    }
    m_manualAxis.axis = AxisIndex;//ui->cb_axisActionAxis->currentIndex();
    m_manualAxis.speed = ui->editAxisActionSpeed->text().toInt();
    m_manualAxis.pos_pos = ui->editPositionAdd->text().toFloat() * 100;
    m_manualAxis.sub_pos = ui->editPositionSub->text().toDouble() * 100;
    m_manualAxis.ZDrop = ui->chbZAxisDesend->isChecked();
}

void ManualForm::setupReserveWidgets()
{
    for (auto vlayout : ui->tabReserve->findChildren<QVBoxLayout*>())
    {
        vlayout->setAlignment(Qt::AlignTop);
    }
    for (auto hlayout : ui->tabReserve->findChildren<QHBoxLayout*>())
    {
        hlayout->setAlignment(Qt::AlignLeft);
    }

    ui->tabWgtReserve->removeTab(1);
//    ui->tabWgtReserve->setTabEnabled(1, false);

    const std::vector<QPushButton*> group1 = {
        ui->btnPortY1, ui->btnPortY2, ui->btnPortY3, ui->btnPortY4,
        ui->btnPortY5, ui->btnPortY6, ui->btnPortY7, ui->btnPortY8};
    const std::vector<QPushButton*> group2 = {
        ui->btnPortY9, ui->btnPortY10, ui->btnPortY11, ui->btnPortY12,
        ui->btnPortY13, ui->btnPortY14, ui->btnPortY15, ui->btnPortY16};
    const std::vector<QPushButton*> group3 = {
        ui->btnPortY17, ui->btnPortY18, ui->btnPortY19, ui->btnPortY20,
        ui->btnPortY21, ui->btnPortY22, ui->btnPortY23, ui->btnPortY24};

    const std::vector<std::pair<std::vector<QPushButton*>, QGroupBox*>> buttonGroups = {
        {group1, ui->grboxReserveMb1},
        {group2, ui->grboxReserveMb2},
        {group3, ui->grboxReserveMb3},
    };

    for (size_t i = 0; i < reserveButtons.size(); i++)
    {
        auto btn = reserveButtons[i];
        setbuttonIcon(btn, btn->text(), 0);
        connect(btn, &QPushButton::clicked, this, [=](){
            const QString& portName = m_Port_Y[i].functionSet == 0 ? m_Port_Y[i].ResModifyName : m_Port_Y[i].modifyName;

            if (m_OutPortSta[i])
            {
                setbuttonIcon(btn, portName + tr("断"), 0);
                g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL, CMD_SUN_MANUAL_OUT, m_Port_Y[i].actualPortNum, 0);
            }
            else
            {
                setbuttonIcon(btn, portName + tr("通"), 1);
                g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL, CMD_SUN_MANUAL_OUT, m_Port_Y[i].actualPortNum, 1);
            }
        });
    }
}

void ManualForm::setupClawAndMachineConnections()
{

//    connect(checkTimer, &QTimer::timeout, this, &ManualForm::checkAndUpdatePortButtonState);

#if 0
    /******按钮控制输出下发槽函数******/
    connect(ui->btnRawMaterial1Loosen,&QPushButton::clicked,this,[=](){//原料1松开/夹紧
        if(ui->btnRawMaterial1Loosen->text() == m_Port_Y[CLAW_METERIAL_1_LOOSENED].modifyName)
        {//原料1松开
            setbuttonIcon(ui->btnRawMaterial1Loosen,m_Port_Y[CLAW_METERIAL_1_CLAMP].modifyName,1);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[CLAW_METERIAL_1_CLAMP].portNum,1);
        }
        else
        {//原料1夹紧
            setbuttonIcon(ui->btnRawMaterial1Loosen,m_Port_Y[CLAW_METERIAL_1_LOOSENED].modifyName,0);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[CLAW_METERIAL_1_CLAMP].portNum,0);
        }
    });
    connect(ui->btnFinishedProduct1Loosen,&QPushButton::clicked,this,[=](){//成品1松开/夹紧
        if(ui->btnFinishedProduct1Loosen->text() == m_Port_Y[CLAW_PRODUCT_1_LOOSENED].modifyName)
        {
            setbuttonIcon(ui->btnFinishedProduct1Loosen,m_Port_Y[CLAW_PRODUCT_1_CLAMP].modifyName,1);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[CLAW_PRODUCT_1_CLAMP].portNum,1);
        }
        else
        {
            setbuttonIcon(ui->btnFinishedProduct1Loosen,m_Port_Y[CLAW_PRODUCT_1_LOOSENED].modifyName,0);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[CLAW_PRODUCT_1_CLAMP].portNum,0);
        }
    });
    connect(ui->btnClaw1Reverse,&QPushButton::clicked,this,[=](){//卡爪1反转/正转
        if(ui->btnClaw1Reverse->text() == m_Port_Y[CLAW_CLAW_1_LOOSENED].modifyName)
        {
            setbuttonIcon(ui->btnClaw1Reverse,m_Port_Y[CLAW_CLAW_1_CLAMP].modifyName,1);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[CLAW_CLAW_1_CLAMP].portNum,1);
        }
        else
        {
            setbuttonIcon(ui->btnClaw1Reverse,m_Port_Y[CLAW_CLAW_1_LOOSENED].modifyName,0);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[CLAW_CLAW_1_CLAMP].portNum,0);
        }

    });
    connect(ui->btnBlow1Loosen,&QPushButton::clicked,this,[=](){//吹气1断/通
        if(ui->btnBlow1Loosen->text() == m_Port_Y[MACHINE_GASS_1_LOOSENED].modifyName+tr("断"))
        {
            setbuttonIcon(ui->btnBlow1Loosen,m_Port_Y[MACHINE_GASS_1_LOOSENED].modifyName+tr("通"),1);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[MACHINE_GASS_1_LOOSENED].portNum,1);
        }
        else
        {
            setbuttonIcon(ui->btnBlow1Loosen,m_Port_Y[MACHINE_GASS_1_LOOSENED].modifyName+tr("断"),0);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[MACHINE_GASS_1_LOOSENED].portNum,0);
        }

    });
    connect(ui->btnChuck1Loosen,&QPushButton::clicked,this,[=](){//卡盘1松开/夹紧
        if(ui->btnChuck1Loosen->text() == m_Port_Y[MACHINE_CHUCK_1_LOOSENED].modifyName)
        {
            setbuttonIcon(ui->btnChuck1Loosen,m_Port_Y[MACHINE_CHUCK_1_CLAMP].modifyName,1);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[MACHINE_CHUCK_1_CLAMP].portNum,1);
        }
        else
        {
            setbuttonIcon(ui->btnChuck1Loosen,m_Port_Y[MACHINE_CHUCK_1_LOOSENED].modifyName,0);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[MACHINE_CHUCK_1_CLAMP].portNum,0);
        }
        int msec = m_ProductSet.pluseOutTime * 100 + 10;
        QTimer::singleShot(msec, this, &ManualForm::checkAndUpdatePortButtonState);
    });
    connect(ui->btnAutoGate1Close,&QPushButton::clicked,this,[=](){//自动门关/开
        if(ui->btnAutoGate1Close->text() == m_Port_Y[MACHINE_AUTO_DOOR_1_CLOSE].modifyName)
        {
            setbuttonIcon(ui->btnAutoGate1Close,m_Port_Y[MACHINE_AUTO_DOOR_1_OPEN].modifyName,1);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[MACHINE_AUTO_DOOR_1_OPEN].portNum,1);
        }
        else
        {
            setbuttonIcon(ui->btnAutoGate1Close,m_Port_Y[MACHINE_AUTO_DOOR_1_CLOSE].modifyName,0);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[MACHINE_AUTO_DOOR_1_OPEN].portNum,0);
        }
        int msec = m_ProductSet.pluseOutTime * 100 + 10;
        QTimer::singleShot(msec, this, &ManualForm::checkAndUpdatePortButtonState);
    });
    connect(ui->btnStartProcess1Break,&QPushButton::clicked,this,[=](){//启动加工1断/通
        if(ui->btnStartProcess1Break->text() == m_Port_Y[MACHINE_START_PROCESS_1].modifyName+tr("断"))
        {
            setbuttonIcon(ui->btnStartProcess1Break,m_Port_Y[MACHINE_START_PROCESS_1].modifyName+tr("通"),1);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[MACHINE_START_PROCESS_1].portNum,1);
        }
        else
        {
            setbuttonIcon(ui->btnStartProcess1Break,m_Port_Y[MACHINE_START_PROCESS_1].modifyName+tr("断"),0);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[MACHINE_START_PROCESS_1].portNum,0);
        }
        int msec = m_ProductSet.pluseOutTime * 100 + 10;
        QTimer::singleShot(msec, this, &ManualForm::checkAndUpdatePortButtonState);
    });
    connect(ui->btnMainAxisLocate1Break,&QPushButton::clicked,this,[=](){//主轴定位1断/通
        if(ui->btnMainAxisLocate1Break->text() == m_Port_Y[MACHINE_SPINDLE_FIXED_POS_1].modifyName+tr("断"))
        {
            setbuttonIcon(ui->btnMainAxisLocate1Break,m_Port_Y[MACHINE_SPINDLE_FIXED_POS_1].modifyName+tr("通"),1);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[MACHINE_SPINDLE_FIXED_POS_1].portNum,1);
        }
        else
        {
            setbuttonIcon(ui->btnMainAxisLocate1Break,m_Port_Y[MACHINE_SPINDLE_FIXED_POS_1].modifyName+tr("断"),0);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[MACHINE_SPINDLE_FIXED_POS_1].portNum,0);
        }
        int msec = m_ProductSet.pluseOutTime * 100 + 10;
        QTimer::singleShot(msec, this, &ManualForm::checkAndUpdatePortButtonState);
    });
    connect(ui->btnControlRotate1Break,&QPushButton::clicked,this,[=](){//主轴旋转1断/通
        if(ui->btnControlRotate1Break->text() == m_Port_Y[MACHINE_SPINDLE_ROTATE_1].modifyName+tr("断"))
        {
            setbuttonIcon(ui->btnControlRotate1Break,m_Port_Y[MACHINE_SPINDLE_ROTATE_1].modifyName+tr("通"),1);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[MACHINE_SPINDLE_ROTATE_1].portNum,1);
        }
        else
        {
            setbuttonIcon(ui->btnControlRotate1Break,m_Port_Y[MACHINE_SPINDLE_ROTATE_1].modifyName+tr("断"),0);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[MACHINE_SPINDLE_ROTATE_1].portNum,0);
        }
        int msec = m_ProductSet.pluseOutTime * 100 + 10;
        QTimer::singleShot(msec, this, &ManualForm::checkAndUpdatePortButtonState);
    });
#endif
    connect(ui->btnRawMaterial1Loosen,&QPushButton::clicked,this,[=](){//原料1松开/夹紧
        if(m_OutPortSta[CLAW_METERIAL_1_CLAMP] == 0)
        {//原料1松开
            setbuttonIcon(ui->btnRawMaterial1Loosen,m_Port_Y[CLAW_METERIAL_1_CLAMP].modifyName,1);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[CLAW_METERIAL_1_CLAMP].portNum,1);
        }
        else
        {//原料1夹紧
            setbuttonIcon(ui->btnRawMaterial1Loosen,m_Port_Y[CLAW_METERIAL_1_LOOSENED].modifyName,0);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[CLAW_METERIAL_1_CLAMP].portNum,0);
        }
    });
    connect(ui->btnFinishedProduct1Loosen,&QPushButton::clicked,this,[=](){//成品1松开/夹紧
        if(m_OutPortSta[CLAW_PRODUCT_1_CLAMP] == 0)
        {
            setbuttonIcon(ui->btnFinishedProduct1Loosen,m_Port_Y[CLAW_PRODUCT_1_CLAMP].modifyName,1);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[CLAW_PRODUCT_1_CLAMP].portNum,1);
        }
        else
        {
            setbuttonIcon(ui->btnFinishedProduct1Loosen,m_Port_Y[CLAW_PRODUCT_1_LOOSENED].modifyName,0);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[CLAW_PRODUCT_1_CLAMP].portNum,0);
        }
    });
    connect(ui->btnClaw1Reverse,&QPushButton::clicked,this,[=](){//卡爪1反转/正转
        if(m_OutPortSta[CLAW_CLAW_1_CLAMP] == 0)
        {
            setbuttonIcon(ui->btnClaw1Reverse,m_Port_Y[CLAW_CLAW_1_CLAMP].modifyName,1);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[CLAW_CLAW_1_CLAMP].portNum,1);
        }
        else
        {
            setbuttonIcon(ui->btnClaw1Reverse,m_Port_Y[CLAW_CLAW_1_LOOSENED].modifyName,0);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[CLAW_CLAW_1_CLAMP].portNum,0);
        }

    });
    connect(ui->btnBlow1Loosen,&QPushButton::clicked,this,[=](){//吹气1断/通
        if(m_OutPortSta[MACHINE_GASS_1_LOOSENED] == 0)
        {
            setbuttonIcon(ui->btnBlow1Loosen,m_Port_Y[MACHINE_GASS_1_LOOSENED].modifyName+tr("通"),1);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[MACHINE_GASS_1_LOOSENED].portNum,1);
        }
        else
        {
            setbuttonIcon(ui->btnBlow1Loosen,m_Port_Y[MACHINE_GASS_1_LOOSENED].modifyName+tr("断"),0);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[MACHINE_GASS_1_LOOSENED].portNum,0);
        }

    });
    connect(ui->btnChuck1Loosen,&QPushButton::clicked,this,[=](){//卡盘1松开/夹紧
        if(m_OutPortSta[MACHINE_CHUCK_1_CLAMP] == 0)
        {
            setbuttonIcon(ui->btnChuck1Loosen,m_Port_Y[MACHINE_CHUCK_1_CLAMP].modifyName,1);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[MACHINE_CHUCK_1_CLAMP].portNum,1);
            if (m_OutPortType[0][1]) {
                int pulse_times = 2*(m_OutPortType[0][1] - 1) + 1;
                int msec = m_ProductSet.pluseOutTime * 100 * pulse_times + 10;
                QTimer::singleShot(msec, this, [=](){
                    setbuttonIcon(ui->btnChuck1Loosen,m_Port_Y[MACHINE_CHUCK_1_LOOSENED].modifyName,0);
                });
            }
        }
        else
        {
            setbuttonIcon(ui->btnChuck1Loosen,m_Port_Y[MACHINE_CHUCK_1_LOOSENED].modifyName,0);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[MACHINE_CHUCK_1_CLAMP].portNum,0);
        }
    });
    connect(ui->btnAutoGate1Close,&QPushButton::clicked,this,[=](){//自动门关/开
        if(m_OutPortSta[MACHINE_AUTO_DOOR_1_OPEN] == 0)
        {
            setbuttonIcon(ui->btnAutoGate1Close,m_Port_Y[MACHINE_AUTO_DOOR_1_OPEN].modifyName,1);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[MACHINE_AUTO_DOOR_1_OPEN].portNum,1);
            if (m_OutPortType[1][1]) {
                int pulse_times = 2*(m_OutPortType[1][1] - 1) + 1;
                int msec = m_ProductSet.pluseOutTime * 100 * pulse_times + 10;
                QTimer::singleShot(msec, this, [=](){
                    setbuttonIcon(ui->btnAutoGate1Close,m_Port_Y[MACHINE_AUTO_DOOR_1_CLOSE].modifyName,0);
                });
            }
        }
        else
        {
            setbuttonIcon(ui->btnAutoGate1Close,m_Port_Y[MACHINE_AUTO_DOOR_1_CLOSE].modifyName,0);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[MACHINE_AUTO_DOOR_1_OPEN].portNum,0);
        }
    });
    connect(ui->btnStartProcess1Break,&QPushButton::clicked,this,[=](){//启动加工1断/通
        if(m_OutPortSta[MACHINE_START_PROCESS_1] == 0)
        {
            setbuttonIcon(ui->btnStartProcess1Break,m_Port_Y[MACHINE_START_PROCESS_1].modifyName+tr("通"),1);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[MACHINE_START_PROCESS_1].portNum,1);
            if (m_OutPortType[2][1]) {
                int msec = m_ProductSet.pluseOutTime * 100 * m_OutPortType[2][1] + 10;
                QTimer::singleShot(msec, this, [=](){
                    setbuttonIcon(ui->btnStartProcess1Break,m_Port_Y[MACHINE_START_PROCESS_1].modifyName+tr("断"),0);
                });
            }
        }
        else
        {
            setbuttonIcon(ui->btnStartProcess1Break,m_Port_Y[MACHINE_START_PROCESS_1].modifyName+tr("断"),0);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[MACHINE_START_PROCESS_1].portNum,0);
        }
    });
    connect(ui->btnControlRotate1Break,&QPushButton::clicked,this,[=](){//主轴旋转1断/通
        if(m_OutPortSta[MACHINE_SPINDLE_ROTATE_1] == 0)
        {
            setbuttonIcon(ui->btnControlRotate1Break,m_Port_Y[MACHINE_SPINDLE_ROTATE_1].modifyName+tr("通"),1);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[MACHINE_SPINDLE_ROTATE_1].portNum,1);
            if (m_OutPortType[4][1]) {
                int msec = m_ProductSet.pluseOutTime * 100 * m_OutPortType[4][1]+ 10;
                QTimer::singleShot(msec, this, [=](){
                    setbuttonIcon(ui->btnControlRotate1Break,m_Port_Y[MACHINE_SPINDLE_ROTATE_1].modifyName+tr("断"),0);
                });
            }
        }
        else
        {
            setbuttonIcon(ui->btnControlRotate1Break,m_Port_Y[MACHINE_SPINDLE_ROTATE_1].modifyName+tr("断"),0);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[MACHINE_SPINDLE_ROTATE_1].portNum,0);
        }
    });
    connect(ui->btnMainAxisLocate1Break,&QPushButton::clicked,this,[=](){//主轴定位1断/通
        if(m_OutPortSta[MACHINE_SPINDLE_FIXED_POS_1] == 0)
        {
            setbuttonIcon(ui->btnMainAxisLocate1Break,m_Port_Y[MACHINE_SPINDLE_FIXED_POS_1].modifyName+tr("通"),1);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[MACHINE_SPINDLE_FIXED_POS_1].portNum,1);
            if (m_OutPortType[5][1]) {
                int msec = m_ProductSet.pluseOutTime * 100 * m_OutPortType[5][1] + 10;
                QTimer::singleShot(msec, this, [=](){
                    setbuttonIcon(ui->btnMainAxisLocate1Break,m_Port_Y[MACHINE_SPINDLE_FIXED_POS_1].modifyName+tr("通"),0);
                });
            }
        }
        else
        {
            setbuttonIcon(ui->btnMainAxisLocate1Break,m_Port_Y[MACHINE_SPINDLE_FIXED_POS_1].modifyName+tr("断"),0);
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_OUT,m_Port_Y[MACHINE_SPINDLE_FIXED_POS_1].portNum,0);
        }
    });
    connect(ui->chbMachineControl,&QCheckBox::clicked,this,[=](){//机床控制使能复选框
        bool canUse = ui->chbMachineControl->isChecked();
//        ui->btnAutoGate1Close->setEnabled(canUse);
//        ui->btnStartProcess1Break->setEnabled(canUse);
//        ui->btnMainAxisLocate1Break->setEnabled(canUse);
//        ui->btnControlRotate1Break->setEnabled(canUse);

//        ui->btnAutoGate1Close->setEnabled(canUse && m_OutportInterlock[3][0]);//自动门1开关不受机床控制使能的控制
        ui->btnStartProcess1Break->setEnabled(canUse && m_Port_Y[MACHINE_START_PROCESS_1].functionSet);
        ui->btnMainAxisLocate1Break->setEnabled(canUse && m_Port_Y[MACHINE_SPINDLE_FIXED_POS_1].functionSet);
        ui->btnControlRotate1Break->setEnabled(canUse && m_Port_Y[MACHINE_SPINDLE_ROTATE_1].functionSet);
    });
}

void ManualForm::setupAxisActionConnections()
{
    connect(ui->cb_axisActionAxis, QOverload<int>::of(&QComboBox::activated),[=](int){
        saveManualAxisActionPara();
        setManualAxis(m_manualAxis);
    });
    connect(ui->editAxisActionSpeed,&NumberEdit::returnPressed,[=](){
        saveManualAxisActionPara();
        setManualAxis(m_manualAxis);
    });
    connect(ui->editPositionAdd,&NumberEdit::returnPressed,[=](){
        saveManualAxisActionPara();
        setManualAxis(m_manualAxis);
    });
    connect(ui->editPositionSub,&NumberEdit::returnPressed,[=](){
        saveManualAxisActionPara();
        setManualAxis(m_manualAxis);
    });
    connect(ui->chbZAxisDesend, QOverload<int>::of(&QCheckBox::stateChanged), [=](int){
        saveManualAxisActionPara();
        setManualAxis(m_manualAxis);
    });

    connect(ui->handPosAdd,&QPushButton::clicked,this,[=](){
        g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_INCREMENT,m_manualAxis.axis,2);
    });
    connect(ui->HandPosDown,&QPushButton::clicked,this,[=](){
        g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_INCREMENT,m_manualAxis.axis,1);
    });

    /**********************************************************/
    // 调机
    connect(ui->btnAdjustFunc, &QPushButton::clicked, this, [=](){
        if(m_RobotOriginState == 0)
        {//如果未回零点
            ErrorTipDialog tip(tr("当前未找原点"),nullptr);
            tip.exec();
        }
        else
        {
            TipPasswdDialog tip(tr("请输入高级管理员密码"), nullptr);
            int reply = tip.exec();
            if (reply == QDialog::Rejected)
            {
                return;
            }
            else if (reply == QDialog::Accepted)
            {
                uint advancePasswd = tip.getPasswd();

                if (advancePasswd == passwd[1])
                {
                    ui->stkWgtAdjustMachine->setCurrentIndex(1);
                    ui->coboxAdjustMachineAxis->clear();
                    for(int i=0;i<AXIS_TOTAL_NUM;i++)
                    {
                        if(m_AxisPar[i].axisType == 1)
                        {
                           ui->coboxAdjustMachineAxis->addItem(m_NameDefine[1].axisName[i]);
                        }
                    }
                    InitAdjustMachine(0);
                }
            }
        }
    });
    connect(ui->chboxUseAdjustMachine, &QCheckBox::clicked, this, [=](int state){
        if (!state)
        {
            ui->stkWgtAdjustMachine->setCurrentIndex(0);
            ui->chboxUseAdjustMachine->setChecked(true);
        }
    });
    connect(ui->AdjustMachineSpace,&NumberEdit::returnPressed, this,[=](){//每转距离
        for(int i=0;i<AXIS_TOTAL_NUM;i++)
        {
            if(ui->coboxAdjustMachineAxis->currentText() == m_NameDefine[1].axisName[i])
            {
                m_AxisPar[i].circleDis = (int32_t)(ui->AdjustMachineSpace->text().toDouble()*100);
                setAxisPar(m_AxisPar[i],i);
                emit AxisParRefreshSignal(i);
            }
        }
    });
    connect(ui->AdjustMachineInternalPulse,&NumberEdit::returnPressed, this,[=](){//内部脉冲
        for(int i=0;i<AXIS_TOTAL_NUM;i++)
        {
            if(ui->coboxAdjustMachineAxis->currentText() == m_NameDefine[1].axisName[i])
            {
                m_AxisPar[i].circlePluseNum = (int32_t)(ui->AdjustMachineInternalPulse->text().toInt());
                setAxisPar(m_AxisPar[i],i);
                emit AxisParRefreshSignal(i);
            }
        }
    });
    connect(ui->AdjustMachineForward,&QPushButton::clicked,this,[=](){//电机正转1圈
        uint16_t AdjustMachineAxisIndex = ui->coboxAdjustMachineAxis->currentIndex();
        if(AdjustMachineAxisIndex<AXIS_TOTAL_NUM)
        {
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_AXIS,AdjustMachineAxisIndex+1,4);
        }
    });
    connect(ui->AdjustMachineReversal,&QPushButton::clicked,this,[=](){//电机反转1圈
        uint16_t AdjustMachineAxisIndex = ui->coboxAdjustMachineAxis->currentIndex();
        if(AdjustMachineAxisIndex<AXIS_TOTAL_NUM)
        {
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_AXIS,AdjustMachineAxisIndex+1,5);
        }
    });
}

void ManualForm::handleLoginModeChanged(LoginMode mode)
{
    if (mode == LoginMode::Operator)
    {
        ui->tabWidgetManualPage->removeTab(4);
        bool canUse = false;

        ui->btnIntoStack->setEnabled(canUse);

        ui->btnHideGuide->setChecked(false);
        ui->checkBoxEditPosReference->setVisible(canUse);
        ui->btnImportPictureReference->setVisible(canUse);
        ui->btnNewButtonReference->setVisible(canUse);
        ui->btnDeleteButtonReference->setVisible(canUse);
        ui->btnEditGuideName->setVisible(canUse);
        ui->editGuideKeyDef->setVisible(canUse);
        ui->btnSaveGuide->setVisible(canUse);
        ui->btnHideGuide->setVisible(canUse);
    }
    else
    {
        bool canUse = true;

        ui->tabWidgetManualPage->addTab(ui->tabReference, tr("参考点"));

        ui->btnIntoStack->setEnabled(canUse);

        ui->btnHideGuide->setChecked(false);
        ui->checkBoxEditPosReference->setVisible(canUse);
        ui->btnImportPictureReference->setVisible(canUse);
        ui->btnNewButtonReference->setVisible(canUse);
        ui->btnDeleteButtonReference->setVisible(canUse);
        ui->btnEditGuideName->setVisible(canUse);
        ui->editGuideKeyDef->setVisible(canUse);
        ui->btnSaveGuide->setVisible(canUse);
        ui->btnHideGuide->setVisible(canUse);
    }
}

void ManualForm::reloadReferPoint()
{
    ::readReferenceInfo();
    updateReferPointsList();
}

void ManualForm::updateReserveButtonState()
{
    const std::vector<QPushButton*> interLockButtons = {
        ui->btnRawMaterial1Loosen, ui->btnFinishedProduct1Loosen, ui->btnClaw1Reverse, ui->btnAutoGate1Close,
        ui->btnChuck1Loosen, ui->btnBlow1Loosen, ui->btnStartProcess1Break, ui->btnControlRotate1Break,
        ui->btnMainAxisLocate1Break
    };
    std::vector<bool> buttonState(9, true);
    for (int i = 0; i < 5; i++)
    {
        buttonState[i] = m_OutportInterlock[i][0];
    }
    buttonState[5] = m_Port_Y[MACHINE_GASS_1_LOOSENED].functionSet;//吹气1通/断
    buttonState[6] = m_Port_Y[MACHINE_START_PROCESS_1].functionSet;//启动加工1通/断
    buttonState[7] = m_Port_Y[MACHINE_SPINDLE_ROTATE_1].functionSet;//主轴旋转通/断
    buttonState[8] = m_Port_Y[MACHINE_SPINDLE_FIXED_POS_1].functionSet;//主轴定位通/断
    for (size_t i = 0; i < interLockButtons.size(); i++)
    {
       auto btn = interLockButtons.at(i);

       bool isUse = buttonState[i];
       btn->setEnabled(isUse);//失能显示灰色会更好一点，方便知道是什么功能未
//       btn->setVisible(isUse);
    }

    for (size_t i = 0; i < reserveButtons.size(); ++i)
    {
        const auto& port = m_Port_Y[i];
        auto button = reserveButtons[i];

        bool isVisible = (port.functionSet == 0);
        button->setVisible(isVisible);

        const QString& portName = (port.functionSet == 0) ? port.ResModifyName : port.modifyName;

        QString statusText = m_OutPortSta[i] ? tr("通") : tr("断");

        button->setText(portName + statusText);
    }

}

void ManualForm::addReferencePoint()
{
    DraggableButton* btn = new DraggableButton(ui->frameRerencePoint);
    btn->setCheckable(true);
    btn->setAutoExclusive(true);
    btn->setDraggable(draggable[1]);
    btn->setFixedSize(40, 40);
    btn->setStyleSheet("QPushButton{ border-style:solid; outline:none; border-width:1px; border-radius:20px;}");

    int newIndex = getNextAvailableIndex();
    if (newIndex == 0)
    {
        return;
    }
    QString referName = QString("参考点%1").arg(newIndex);
    int posIndex = newIndex - 1;
    QPoint btnPos = QPoint(10 + 42 * (posIndex / 7), 120 + 42 * (posIndex % 7));

    ReferPointPara point = {newIndex, referName, btn, btnPos};
    referencePoints.append(point);

    btn->setGeometry(QRect(btnPos, btn->size()));
    btn->setText(QString::number(point.index));
    btn->show();

    setupReferPointConnections(btn);

    if (selectedButton[1] == nullptr)
    {
       selectedButton[1] = btn;
       btn->setChecked(true);
       onReferPointButtonPressed(btn);
    }
}

void ManualForm::removeReferencePoint()
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
        auto btn = referencePoints.last().button;
        btn->setChecked(true);
        selectedButton[1] = btn;
        onReferPointButtonPressed(btn);
    }
    else {
        selectedButton[1] = nullptr;
    }
}

bool ManualForm::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::Hide) {
//        QString objectName = watched->objectName();
//        qDebug() << objectName << "was hidden";

        if (watched == ui->tabGuide) {
            onCheckSavedGuide();
            return true;
        }
        else if (watched == ui->tabReference) {
            onCheckSavedReferPointPara();
            return true;
        }
    }

    if (event->type() == QEvent::Show)
    {
        if (watched == ui->tabReference) {

            updateReferPointsAxisPos();

            if (!referencePoints.isEmpty())
            {
                const auto& point = referencePoints.at(0);
                ui->textReferPointName->setText(point.name);

                selectedButton[1] = point.button;
                refreshPosTable();
            }
        }
        return true;
    }

    return QWidget::eventFilter(watched, event);
}

void ManualForm::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange)
    {
        retranslate();
    }
    QWidget::changeEvent(e);
}

void ManualForm::retranslate()
{
    ui->retranslateUi(this);
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

            FullKeyboard *keyboard = FullKeyboard::instance();
            keyboard->setText(it->name);
            keyboard->setCurrentEditObj(ui->textReferPointName);
            keyboard->exec(); // must use exec here, using open cannot ensure to save the name.

            QString text = keyboard->getInputText();
            if (it->name != text )
            {
                it->name = text;
                ui->btnSaveReference->setParaChangedFlag(true);
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

    reserveButtons = {
        ui->btnPortY1, ui->btnPortY2, ui->btnPortY3, ui->btnPortY4,
        ui->btnPortY5, ui->btnPortY6, ui->btnPortY7, ui->btnPortY8,
        ui->btnPortY9, ui->btnPortY10, ui->btnPortY11, ui->btnPortY12,
        ui->btnPortY13, ui->btnPortY14, ui->btnPortY15, ui->btnPortY16,
        ui->btnPortY17, ui->btnPortY18, ui->btnPortY19, ui->btnPortY20,
        ui->btnPortY21, ui->btnPortY22, ui->btnPortY23, ui->btnPortY24
    };
    checkTimer = new QTimer(this);

    ::readGuideInfo();
    updateGuidePoints();

    updateReserveButtonState();
}

void ManualForm::on_btnEditGuideName_clicked()
{
    if (selectedButton[0] == nullptr) return;

    QString guideName = selectedButton[0]->text();

    FullKeyboard *keyboard = FullKeyboard::instance();
    keyboard->setText(guideName);
    keyboard->setCurrentEditObj(selectedButton[0]);
    keyboard->exec();

    QString currentText = selectedButton[0]->text();

    if (guideName != currentText)
    {
        DraggableButton *currentButton = selectedButton[0];

        GuidePara &guidePara = guidePoints[currentButton];
        guidePara.guideName = currentText;

        ui->btnSaveGuide->setParaChangedFlag(true);
    }
}

void ManualForm::onSaveKeyDef()
{
    // save the KeyDef setting in coresponding guidePoints
    if (selectedButton[0] && guidePoints.contains(selectedButton[0]))
    {
        guidePoints[selectedButton[0]].keyDefStr = ui->editGuideKeyDef->text();
        // save the corresponding parameters in struct GuidePara
        guidePoints[selectedButton[0]].keyType = ui->editGuideKeyDef->getKeyType();
        guidePoints[selectedButton[0]].portNum = ui->editGuideKeyDef->getPortNum();
        guidePoints[selectedButton[0]].status = ui->editGuideKeyDef->getKeyFuncStatus();

        ui->btnSaveGuide->setParaChangedFlag(true);
    }
}

void ManualForm::onCheckSavedGuide()
{
    if (ui->btnSaveGuide->isParaChanged() == false) {
        return;
    }

    ErrorTipDialog tip(tr("操作指引参数有修改，是否需要保存？"));
    int reply = tip.exec();
    if (reply == QDialog::Accepted)
    {
        on_btnSaveGuide_clicked();
    }
    else if (reply == QDialog::Rejected)
    {
        updateGuidePoints();
    }
    ui->btnSaveGuide->setParaChangedFlag(false);;
}

void ManualForm::onCheckSavedReferPointPara()
{
    if (ui->btnSaveReference->isParaChanged() == false) {
        return;
    }

    ErrorTipDialog tip(tr("参考点参数有修改，是否需要保存？"));
    int reply = tip.exec();
    if (reply == QDialog::Accepted)
    {
        on_btnSaveReference_clicked();
    }
    else if (reply == QDialog::Rejected)
    {
        updateReferPointsList();
    }
    ui->btnSaveReference->setParaChangedFlag(false);
}
void ManualForm::on_btnSaveGuide_clicked()
{
//    for (auto& [k, v] : guidePoints)
//    {

//    }
    if (ui->btnSaveGuide->isParaChanged() == false) return;

    P_GuideStruct defaultGuide = {0, "", "0", QPoint(0, 0), 0, 0, 0};

    // 清空 m_Guide 数组
    std::fill(std::begin(m_Guide), std::end(m_Guide), defaultGuide);

    int index = 0;
    for (const auto& guide : guidePoints) {
        if (index >= GUIDE_TOTAL_NUM) break;

        m_Guide[index].guideFlag = true;
        m_Guide[index].guideName = guide.guideName;
        m_Guide[index].keyDefStr = guide.keyDefStr;
        m_Guide[index].btnPos = guide.btnPos;
        m_Guide[index].keyType = guide.keyType;
        m_Guide[index].portNum = guide.portNum;
        m_Guide[index].status = guide.status;
        ++index;
    }

    ::writeGuideInfo();

    ui->btnSaveGuide->setParaChangedFlag(false);

}
void ManualForm::on_btnSaveReference_clicked()
{
    if (ui->btnSaveReference->isParaChanged() == false) return;

    if (selectedButton[1])
    {
        auto it = std::find_if(referencePoints.begin(), referencePoints.end(), [=](const ReferPointPara& point) {
            return point.button == selectedButton[1];
        });

        if (it != referencePoints.end())
        {
            it->name = ui->textReferPointName->toPlainText();
            QPoint pointPos = selectedButton[1]->geometry().topLeft();
            it->pointPos = pointPos;

            updateReferPointsTable();
        }
    }


    // 重置结构体信息
    P_RefStruct defaultRefPoint = {0, {0}, {0}, "", 0, 0};
    std::fill(std::begin(m_RefPoint), std::end(m_RefPoint), defaultRefPoint);

   // 更新结构体信息
    for (int i = 0; i < referencePoints.size(); i++)
    {
        int arrayIndex = referencePoints.at(i).index - 1;
        m_RefPoint[arrayIndex].refFlag = true;
        //            m_RefPoint[arrayIndex].index = referencePoints.at(i).index;
        m_RefPoint[arrayIndex].refName = referencePoints.at(i).name;
        m_RefPoint[arrayIndex].xPos = referencePoints.at(i).pointPos.x();
        m_RefPoint[arrayIndex].yPos = referencePoints.at(i).pointPos.y();
//     need to save axis position to corresponding program file.
//     TO DO...

        for (int j = 0; j < AXIS_TOTAL_NUM; j++)
        {
            m_RefPoint[arrayIndex].pos[j] = referencePoints.at(i).axisPos[j];
        }
        g_Usart->ExtendSendProDeal(CMD_MAIN_PRO,CMD_SUN_PRO_REF,i+1);//发送参考点信息
    }

    ::writeReferenceInfo();

    ui->btnSaveReference->setParaChangedFlag(false);
}

//void ManualForm::onTabChanged(int index)
//{

//    static int lastTabIndex = 4;
//    if (lastTabIndex == 0)
//    {
//        onCheckSavedGuide();
//    }
//    else if (lastTabIndex == 4)
//    {
////        qDebug() << "check if referPoint has been saved";
//        onCheckSavedReferPointPara();
//    }
//    lastTabIndex = index;
//}

void ManualForm::on_btnImportPictureGuide_clicked()
{
    bool useDefultPic = false;

    auto filePath = UsbDisk::instance()->findFile("guide.png");

    if (filePath.isNull())
    {
        ErrorTipDialog tip((tr("未找到图片：guide.png，图片像素: 1001×450px。是否使用默认图片？"), nullptr));
        int res = tip.exec();
        useDefultPic = res == QDialog::Accepted;

        if (useDefultPic)
        {
            filePath = "/opt/MachineTool/resources/pics/guide_default.png";
        }
    }

    QString targetPath = GuidePicPath;

    if (QFile::exists(targetPath)) {
        QFile::remove(targetPath);
    }
    if (!QFile::copy(filePath, targetPath)) {
        qWarning() << "Failed to copy file:" << filePath;
    }
    REFRESH_KERNEL_BUFFER(targetPath.toStdString().c_str())

    QPixmap pic(filePath);
    if (!pic.isNull()) {
        QPixmap scaledPic = pic.scaled(QSize(1001, 450), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui->labelGuideBackgroud->setPixmap(scaledPic);
    }
    else {
        ui->labelGuideBackgroud->setPixmap(pic);
    }
}

void ManualForm::on_btnImportPictureReference_clicked()
{
    bool useDefultPic = false;

    auto filePath = UsbDisk::instance()->findFile("referPoint.png");

    if (filePath.isNull())
    {
        ErrorTipDialog tip(
                    tr("未找到图片: referPoint.png，图片像素: 760×480px。是否使用默认图片？"), nullptr);
        int res = tip.exec();
        useDefultPic = res == QDialog::Accepted;

        if (useDefultPic)
        {
            filePath = "/opt/MachineTool/resources/pics/referPoint_default.png";
        }
    }

    QString targetPath = REFERPOINT_PIC;

    if (QFile::exists(targetPath)) {
        QFile::remove(targetPath);
    }
    if (!QFile::copy(filePath, targetPath)) {
        qWarning() << "Failed to copy file:" << filePath;
    }
    REFRESH_KERNEL_BUFFER(targetPath.toUtf8().constData());


    QPixmap pic(filePath);
    if (!pic.isNull()) {
        QPixmap scaledPic = pic.scaled(QSize(760, 480), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui->labReferPointBackGround->setPixmap(scaledPic);
    }
    else {
        ui->labReferPointBackGround->setPixmap(pic);
    }
}

//待状态灯的按钮设置状态灯状态
void ManualForm::setbuttonIcon(QPushButton *button,const QString &ButtonText, uint8_t state)
{
    switch (state) {
    case 1://红灯为夹紧、正转、通或开状态
        button->setIcon(QIcon(":/images/state_red.png"));
        break;
    case 0://熄灭为松开、反转、断或关状态
        button->setIcon(QIcon(":/images/state_grey.png"));
        break;
    default:
        break;
    }
    button->setStyleSheet("text-align: left; padding-left: 10px;");
    button->setText(ButtonText);
    button->setIconSize(QSize(24, 24));
}

//卡爪/机床,预留界面状态灯按钮初始化
void ManualForm::StateButtonInit()
{
    //卡爪/机床
    setbuttonIcon(ui->btnRawMaterial1Loosen,m_Port_Y[CLAW_METERIAL_1_LOOSENED].modifyName,false);
    setbuttonIcon(ui->btnFinishedProduct1Loosen,m_Port_Y[CLAW_PRODUCT_1_LOOSENED].modifyName,false);
    setbuttonIcon(ui->btnClaw1Reverse,m_Port_Y[CLAW_CLAW_1_LOOSENED].modifyName,false);
    setbuttonIcon(ui->btnBlow1Loosen,m_Port_Y[MACHINE_GASS_1_LOOSENED].modifyName+tr("断"),false);
    setbuttonIcon(ui->btnChuck1Loosen,m_Port_Y[MACHINE_CHUCK_1_LOOSENED].modifyName,false);
    setbuttonIcon(ui->btnAutoGate1Close,m_Port_Y[MACHINE_AUTO_DOOR_1_CLOSE].modifyName,false);
    setbuttonIcon(ui->btnStartProcess1Break,m_Port_Y[MACHINE_START_PROCESS_1].modifyName+tr("断"),false);
    setbuttonIcon(ui->btnMainAxisLocate1Break,m_Port_Y[MACHINE_SPINDLE_FIXED_POS_1].modifyName+tr("断"),false);
    setbuttonIcon(ui->btnControlRotate1Break,m_Port_Y[MACHINE_SPINDLE_ROTATE_1].modifyName+tr("断"),false);

    for (size_t i = 0; i < reserveButtons.size(); i++)
    {
        auto btn = reserveButtons[i];
        setbuttonIcon(btn, btn->text(), 0);
        const QString& portName = m_Port_Y[i].functionSet == 0 ? m_Port_Y[i].ResModifyName : m_Port_Y[i].modifyName;

        if (m_OutPortSta[i]==0)
        {
            setbuttonIcon(btn, portName + tr("断"), 0);
        }
        else
        {
            setbuttonIcon(btn, portName + tr("通"), 1);
        }
    }
}
//手动界面按钮需要更新名称
void ManualForm::update_Button_Name_Handel()
{
    if(m_OutPortSta[CLAW_METERIAL_1_CLAMP] == 0){
        ui->btnRawMaterial1Loosen->setText(m_Port_Y[CLAW_METERIAL_1_LOOSENED].modifyName);
    }
    else {
        ui->btnRawMaterial1Loosen->setText(m_Port_Y[CLAW_METERIAL_1_CLAMP].modifyName);
    }

    if(m_OutPortSta[CLAW_PRODUCT_1_CLAMP] == 0){
        ui->btnFinishedProduct1Loosen->setText(m_Port_Y[CLAW_PRODUCT_1_LOOSENED].modifyName);
    }
    else {
        ui->btnFinishedProduct1Loosen->setText(m_Port_Y[CLAW_PRODUCT_1_CLAMP].modifyName);
    }

    if(m_OutPortSta[CLAW_CLAW_1_CLAMP] == 0){
        ui->btnClaw1Reverse->setText(m_Port_Y[CLAW_CLAW_1_LOOSENED].modifyName);
    }
    else {
        ui->btnClaw1Reverse->setText(m_Port_Y[CLAW_CLAW_1_CLAMP].modifyName);
    }

    if(m_OutPortSta[CLAW_CLAW_1_CLAMP] == 0){
        ui->btnBlow1Loosen->setText(m_Port_Y[MACHINE_GASS_1_LOOSENED].modifyName+tr("断"));
    }
    else {
        ui->btnBlow1Loosen->setText(m_Port_Y[CLAW_CLAW_1_CLAMP].modifyName+tr("通"));
    }

    if(m_OutPortSta[MACHINE_CHUCK_1_CLAMP] == 0){
        ui->btnChuck1Loosen->setText(m_Port_Y[MACHINE_CHUCK_1_LOOSENED].modifyName);
    }
    else {
        ui->btnChuck1Loosen->setText(m_Port_Y[MACHINE_CHUCK_1_CLAMP].modifyName);
    }

    if(m_OutPortSta[MACHINE_AUTO_DOOR_1_OPEN] == 0){
        ui->btnAutoGate1Close->setText(m_Port_Y[MACHINE_AUTO_DOOR_1_CLOSE].modifyName);
    }
    else {
        ui->btnAutoGate1Close->setText(m_Port_Y[MACHINE_AUTO_DOOR_1_OPEN].modifyName);
    }

    if(m_OutPortSta[MACHINE_START_PROCESS_1] == 0){
        ui->btnStartProcess1Break->setText(m_Port_Y[MACHINE_START_PROCESS_1].modifyName+tr("断"));
    }
    else {
        ui->btnStartProcess1Break->setText(m_Port_Y[MACHINE_START_PROCESS_1].modifyName+tr("通"));
    }

    if(m_OutPortSta[MACHINE_SPINDLE_FIXED_POS_1] == 0){
        ui->btnMainAxisLocate1Break->setText(m_Port_Y[MACHINE_SPINDLE_FIXED_POS_1].modifyName+tr("断"));
    }
    else {
        ui->btnMainAxisLocate1Break->setText(m_Port_Y[MACHINE_SPINDLE_FIXED_POS_1].modifyName+tr("通"));
    }

    if(m_OutPortSta[MACHINE_SPINDLE_ROTATE_1] == 0){
        ui->btnControlRotate1Break->setText(m_Port_Y[MACHINE_SPINDLE_ROTATE_1].modifyName+tr("断"));
    }
    else {
        ui->btnControlRotate1Break->setText(m_Port_Y[MACHINE_SPINDLE_ROTATE_1].modifyName+tr("通"));
    }
    //预留按钮
    for (size_t i = 0; i < reserveButtons.size(); i++)
    {
        auto btn = reserveButtons[i];
        setbuttonIcon(btn, btn->text(), 0);
        const QString& portName = m_Port_Y[i].functionSet == 0 ? m_Port_Y[i].ResModifyName : m_Port_Y[i].modifyName;

        if (m_OutPortSta[i]==0)
        {
            setbuttonIcon(btn, portName + tr("断"), 0);
        }
        else
        {
            setbuttonIcon(btn, portName + tr("通"), 1);
        }
    }
    //轴选择，调机界面轴选择
    ui->cb_axisActionAxis->clear();
    ui->cb_axisActionAxis->addItem(tr("禁止"));
    for(int i=0;i<AXIS_TOTAL_NUM;i++)
    {
        if(m_AxisPar[i].axisType == 1)
        {
            ui->cb_axisActionAxis->addItem(m_NameDefine[1].axisName[i]);
        }
    }
}

