#include "referencewidget.h"

#include <QHeaderView>
#include <QThread>

extern QString REFERPOINT_PIC;

ReferenceWidget::ReferenceWidget(QWidget *parent)
    : QDialog(parent),
      paramChangedFlag(false), selectedReferPoint(nullptr)
{
    init();

    updateReferPoints();
    updateUI(referencePoints);

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
            selectedReferPoint = btn;
            QString name = it->name;
            textReferPointName->setText(name);

            refreshPosTable();
        }
    });

    connect(btnOK, &QPushButton::clicked, this, [=](){
        if (paramChangedFlag) {
            this->saveReferPointsInfo();
        }
        this->hide();
    });
    connect(btnCancel, &QPushButton::clicked, this, [=](){
        this->hide();
    });

    connect(btnFresh, &QPushButton::clicked, this, &ReferenceWidget::refreshReferPointAxisPos);

    connect(btnEditRefName, &QPushButton::clicked, this, [=]()
    {
        if (selectedReferPoint)
        {
            auto it = std::find_if(referencePoints.begin(), referencePoints.end(), [=](const ReferPointPara& point) {
                return point.button == selectedReferPoint;
            });

            if (it != referencePoints.end())
            {
                auto keyboard = FullKeyboard::instance();

                keyboard->setText(it->name);
                keyboard->setCurrentEditObj(nullptr);

                keyboard->exec(); // must use exec here, using open cannot ensure to save the name.

                QApplication::processEvents();

                QString text = keyboard->getInputText();
                textReferPointName->setText(text);


                if (it->name != text )
                {
                    it->name = text;
                    paramChangedFlag = true;
                }
            }
        }
    });

}

void ReferenceWidget::updateUI(const QList<ReferPointPara> &referPoints)
{
    QPixmap pic(REFERPOINT_PIC);
    pic.scaled(labReferPointPic->size());
    labReferPointPic ->setPixmap(pic);

    for (const auto &point : referPoints) {
        auto btn = point.button;
        btn->setCheckable(true);
        btn->setAutoExclusive(true);
        btn->setDraggable(false);
        btn->setFixedSize(40, 40);
        btn->setStyleSheet("QPushButton{ border-style:solid; outline:none; border-width:1px; border-radius:20px;}");

//        btn->setGeometry(QRect(point.pointPos, btn->size()));
        btn->move(point.pointPos);
        btn->setText(QString::number(point.index));
        btn->show();
    }
    updateReferPointsTable();
}

void ReferenceWidget::init()
{
    // 原始大小增加 60px 的高度
    this->setFixedSize(1004, 581); // 高度从 521 增加到 581
    QPalette palette;
    palette.setColor(QPalette::Background, Qt::white);
    this->setAutoFillBackground(true);
    this->setPalette(palette);

    this->setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);

//    // 添加 titleBar，调整为 60px 高度
//    QWidget* titleBar = new QWidget(this);
//    titleBar->setGeometry(0, 0, this->width(), 60); // 设置 titleBar 高度为 60px
//    titleBar->setStyleSheet("background-color: #e0e0e0;"); // 设置背景颜色
//    titleBar->setObjectName("titleBar");

    // 原有组件 y 坐标平移 60px
    labReferPointPic = new QLabel(this);
    labReferPointPic->setGeometry(0, 130, 760, 400); // 原始 y 坐标 +60
    QPixmap pic(REFERPOINT_PIC);
    pic.scaled(labReferPointPic->size());
    labReferPointPic->setPixmap(pic);

    frameRerencePoint = new QFrame(this);
    frameRerencePoint->setObjectName(QString::fromUtf8("frameRerencePoint"));
    frameRerencePoint->setGeometry(QRect(0, 60, 761, 521)); // 原始 y 坐标 +60
    frameRerencePoint->setFrameShape(QFrame::NoFrame);
    frameRerencePoint->setFrameShadow(QFrame::Raised);

    textReferPointName = new QTextBrowser(this);
    textReferPointName->setObjectName(QString::fromUtf8("textReferPointName"));
    textReferPointName->setGeometry(QRect(820, 80, 170, 70)); // 原始 y 坐标 +60

    tableReference = new QTableWidget(frameRerencePoint);
    tableReference->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableReference->setVisible(referencePoints.size());
    tableReference->setFrameShape(QFrame::NoFrame);
    tableReference->setFrameShadow(QFrame::Raised);
    tableReference->setMaximumHeight(40 * 3);
    tableReference->setMinimumWidth(750);
    tableReference->setColumnCount(8);
    tableReference->verticalHeader()->setVisible(false);
    tableReference->horizontalHeader()->setVisible(false);
    tableReference->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    tableReference->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tableReference->horizontalHeader()->setMinimumSectionSize(30);
    tableReference->verticalHeader()->setDefaultSectionSize(40);
    for (int i = 0; i < tableReference->columnCount(); ++i) {
        tableReference->setColumnWidth(i, (i % 2 == 0) ? 40 : 140);
    }

    tableWgtAxisPos = new QTableWidget(this);
    if (tableWgtAxisPos->columnCount() < 1)
        tableWgtAxisPos->setColumnCount(1);
    tableWgtAxisPos->setObjectName(QString::fromUtf8("tableWgtAxisPos"));
    tableWgtAxisPos->setGeometry(QRect(760, 101 + 60, 241, 361)); // 原始 y 坐标 +60
    tableWgtAxisPos->setMidLineWidth(1);
    tableWgtAxisPos->setRowCount(0);
    tableWgtAxisPos->setColumnCount(1);
    tableWgtAxisPos->horizontalHeader()->setDefaultSectionSize(240);
    tableWgtAxisPos->verticalHeader()->setMinimumSectionSize(30);
    tableWgtAxisPos->verticalHeader()->setDefaultSectionSize(40);

    // 调整按钮位置
    const QSize buttonSize(100, 45);
    btnOK = new QPushButton(tr("确定"), this);
    btnCancel = new QPushButton(tr("取消"), this);
    btnFresh = new QPushButton(tr("刷新"), this);
    btnEditRefName = new QPushButton(tr("编辑"), this);
    btnOK->setFixedSize(buttonSize);
    btnCancel->setFixedSize(buttonSize);
    btnFresh->setFixedSize(buttonSize);
    btnEditRefName->setFixedSize(buttonSize);

    int buttonSpacing = 5;
    int yPos = this->height() - btnCancel->height() - 5;
    btnCancel->move(this->width() - btnCancel->width() - 20, yPos);
    btnOK->move(btnCancel->x() - btnOK->width() - buttonSpacing, yPos);
    btnEditRefName->move(btnOK->x() - btnEditRefName->width() - buttonSpacing, yPos);
    btnFresh->move(btnEditRefName->x() - btnFresh->width() - buttonSpacing, yPos);

    FramelessWidget2* fremeBase = new FramelessWidget2(this);
    fremeBase->setWidget(this);

//    keyboard = new FullKeyboard(this);
}


void ReferenceWidget::updateReferPointsTable()
{
    tableReference->clearContents();
    tableReference->setRowCount(referencePoints.count() / 4 + 1);

    int rowH = tableReference->rowCount();
    if (rowH > 3)
    {
        rowH = 3;
    }
    tableReference->setGeometry(5, 0, 760, 40 * rowH);

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
    if (selectedReferPoint == nullptr) return;

    int index = selectedReferPoint->text().toInt();
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

void ReferenceWidget::updateReferPoints()
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
            DraggableButton* btn = new DraggableButton(frameRerencePoint);
            QPoint btnPos(m_RefPoint[i].xPos, m_RefPoint[i].yPos);
            ReferPointPara point = {refIndex, referName, btn, btnPos,
                                    {m_RefPoint[i].pos[0], m_RefPoint[i].pos[1], m_RefPoint[i].pos[2],
                                     m_RefPoint[i].pos[3], m_RefPoint[i].pos[4],m_RefPoint[i].pos[5]}};
            referencePoints.push_back(point);

            connect(btn, &DraggableButton::pressed, this, [=]() {
                selectedReferPoint = btn;


                int index = 0;
                auto it = std::find_if(referencePoints.begin(), referencePoints.end(), [=](const ReferPointPara& p) {
                    return p.button == selectedReferPoint;
                });
                if (it != referencePoints.end()) {
                    textReferPointName->setText(it->name);
                    index = it->index;
                    refreshPosTable();
                }
                for (int row = 0; row < tableReference->rowCount(); ++row) {
                    for (int col = 0; col < tableReference->columnCount(); ++col) {
                        QTableWidgetItem *indexItem = tableReference->item(row, col);
                        if (indexItem && indexItem->data(Qt::DisplayRole).toInt() == index) {
                            tableReference->setCurrentCell(row, col + 1);
                            return;
                        }
                    }
                }
            }, Qt::UniqueConnection);
        }
    }
    if (referencePoints.isEmpty())
    {
        selectedReferPoint = nullptr;
    }
    else
    {
        selectedReferPoint = referencePoints.at(0).button;
        selectedReferPoint->setChecked(true);
        emit selectedReferPoint->pressed();
    }
}

void ReferenceWidget::updateReferPointsAxisPos()
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

void ReferenceWidget::refreshPosTable()
{
    static const QStringList axisNames = {
        "X1", "Y1", "Z1", "X2", "Y2", "Z2"
    };
    tableWgtAxisPos->setRowCount(AXIS_TOTAL_NUM);
    tableWgtAxisPos->setVerticalHeaderLabels(axisNames);
    tableWgtAxisPos->setHorizontalHeaderLabels({tr("轴位置")});

    tableWgtAxisPos->clearContents();  // 清除旧数据

    auto it = std::find_if(referencePoints.begin(), referencePoints.end(), [=](const ReferPointPara& point) {
        return point.button == selectedReferPoint;
    });
    if (it != referencePoints.end())
    {
        for (size_t i = 0; i < AXIS_TOTAL_NUM; i++)
        {
            if (m_AxisPar[i].axisType == 1)
            {
                auto item = new QTableWidgetItem(QString::number(it->axisPos[i] / 100.0, 'f', 2));
                tableWgtAxisPos->setItem(i, 0, item);
            }
            else
            {
                tableWgtAxisPos->hideRow(i);
            }
        }
    }
}

void ReferenceWidget::refreshReferPointAxisPos()
{
    if (selectedReferPoint)
    {
        auto it = std::find_if(referencePoints.begin(), referencePoints.end(), [=](const ReferPointPara& point) {
            return point.button == selectedReferPoint;
        });

        if (it != referencePoints.end())
        {
            const std::array<int, AXIS_TOTAL_NUM> curAxisPos = {
                m_AxisCurPos.Pos_x, m_AxisCurPos.Pos_y, m_AxisCurPos.Pos_z,
                m_AxisCurPos.Pos_c, m_AxisCurPos.Pos_y2, m_AxisCurPos.Pos_z2
            };

            bool axisPosChangedFlag = false;
            for (int i = 0; i < AXIS_TOTAL_NUM; i++)
            {
                if (it->axisPos[i] != curAxisPos[i])
                {
                    it->axisPos[i] = curAxisPos[i];
                    axisPosChangedFlag = true;
                }
            }
            if (axisPosChangedFlag)
            {
                refreshPosTable();
                paramChangedFlag = true;
            }
        }

    }
}

void ReferenceWidget::saveReferPointsInfo()
{
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
        g_Usart->ExtendSendProDeal(CMD_MAIN_PRO, CMD_SUN_PRO_REF, arrayIndex + 1);//发送参考点信息
    }

    ::writeReferenceInfo();
}

void ReferenceWidget::showEvent(QShowEvent *event)
{
    updateReferPoints();
    updateUI(referencePoints);
    updateReferPointsAxisPos();

    QWidget::showEvent(event);
}

void ReferenceWidget::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange)
    {
        btnOK->setText(tr("确定"));
        btnCancel->setText(tr("取消"));
        btnFresh->setText(tr("刷新"));
        btnEditRefName->setText(tr("编辑"));
    }
    QWidget::changeEvent(e);
}
