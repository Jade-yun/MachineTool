#include "referencewidget.h"

#include <QHeaderView>
#include "mainwindow.h"

ReferenceWidget::ReferenceWidget(QWidget *parent)
    : QWidget(parent), selectedReferPoint(nullptr)
{
    init();

    updateReferPoints();
    initUI(referencePoints);

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
        }
    });
    for (const auto &point : referencePoints)
    {
        auto btn = point.button;
        connect(btn, &DraggableButton::released, this, [=]() {
            auto it = std::find_if(referencePoints.begin(), referencePoints.end(), [=](const ReferPointPara& p) {
                return p.button == btn;
            });
            if (it != referencePoints.end()) {
                it->pointPos = btn->pos();
            }
        });

        connect(btn, &DraggableButton::pressed, this, [=]() {
            selectedReferPoint = btn;

            int index = 0;
            auto it = std::find_if(referencePoints.begin(), referencePoints.end(), [=](const ReferPointPara& p) {
                return p.button == selectedReferPoint;
            });
            if (it != referencePoints.end()) {
                textReferPointName->setText(it->name);
                index = it->index;
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
        });
    }

    connect(btnOK, &QPushButton::clicked, this, [=](){
        this->hide();
    });
    connect(btnCancel, &QPushButton::clicked, this, [=](){
        this->hide();
    });

    connect(btnFresh, &QPushButton::clicked, this, [=]()
    {

    });

    connect(btnEditRefName, &QPushButton::clicked, this, [=]()
    {
        callFullKeyboard(textReferPointName);
//        qDebug() << textReferPointName->property("text").toString();
    });

}

void ReferenceWidget::initUI(const QList<ReferPointPara> &referPoints)
{
    for (const auto &point : referPoints) {
        auto btn = point.button; // Using 'this' as parent to manage lifetime
        btn->setCheckable(true);
        btn->setAutoExclusive(true);
        btn->setDraggable(false); // Assuming you want draggable buttons
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
    this->setFixedSize(1004, 521);
    QPalette palette;
    palette.setColor(QPalette::Background, Qt::white);
    this->setAutoFillBackground(true);
    this->setPalette(palette);

    this->setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);

    QLabel* labBankgroud = new QLabel(this);
    labBankgroud->setGeometry(0, 120, 760, 521-120);
    QPixmap pic("./referPoint.png");
    pic.scaled(labBankgroud->size());
    labBankgroud ->setPixmap(pic);
//    labBankgroud ->setScaledContents(true);

    frameRerencePoint = new QFrame(this);
    frameRerencePoint->setObjectName(QString::fromUtf8("frameRerencePoint"));
    frameRerencePoint->setGeometry(QRect(0, 0, 761, 521));
    frameRerencePoint->setFrameShape(QFrame::NoFrame);
    frameRerencePoint->setFrameShadow(QFrame::Raised);

    textReferPointName = new QTextBrowser(this);
    textReferPointName->setObjectName(QString::fromUtf8("textReferPointName"));
    textReferPointName->setGeometry(QRect(820, 20, 170, 70));

    tableReference = new QTableWidget(frameRerencePoint);
    // Set table to be uneditable
    tableReference->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // Set table to be unselectable
//    tableReference->setSelectionMode(QAbstractItemView::NoSelection);
    // Initialize table to be invisible
    tableReference->setVisible(referencePoints.size());
    tableReference->setFrameShape(QFrame::NoFrame);
    tableReference->setFrameShadow(QFrame::Raised);

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

    btnOK = new QPushButton(tr("确定"), this);
    btnCancel = new QPushButton(tr("取消"), this);
    btnFresh = new QPushButton(tr("刷新"), this);
    btnEditRefName = new QPushButton(tr("编辑"), this);
    const QSize buttonSize(100, 45);
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


    keyboard = new FullKeyboard(this);
}
void ReferenceWidget::updateReferPointsTable()
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

void ReferenceWidget::callFullKeyboard(QObject *obj)
{
    QString text = obj->property("text").toString();
    keyboard->setText(text);
    keyboard->setCurrentEditObj(obj);
    keyboard->open();
}

void ReferenceWidget::updateReferPoints()
{
//    for (auto point : referencePoints)
//    {
//        delete point.button;
//    }

//    for (int i = 0; i < REFERENCE_TOTAL_NUM; i++)
//    {
//        if (true == m_RefPoint[i].refFlag)
//        {
//            QString referName = m_RefPoint[i].refName;
//            int refIndex = m_RefPoint[i].index;
//            DraggableButton* btn = new DraggableButton(frameRerencePoint);
//            QPoint btnPos(m_RefPoint[i].pointPos);
//            ReferPointPara point = {refIndex, referName, btn, btnPos};
//            referencePoints.push_back(point);
//        }
//    }

    for (int i = referencePoints.size() - 1; i >= 0; --i)
    {
        int refIndex = referencePoints[i].index - 1; // 获取索引，用于查找对应的 m_RefPoint
        if (m_RefPoint[refIndex].refFlag == false)   // 如果参考点无效
        {
            delete referencePoints[i].button;  // 释放按钮资源
            referencePoints.removeAt(i);       // 从列表中移除
        }
    }

    // 处理有效的参考点
    for (int i = 0; i < REFERENCE_TOTAL_NUM; i++)
    {
        if (m_RefPoint[i].refFlag == true) // 如果参考点有效
        {
            QString referName = m_RefPoint[i].refName;
            int refIndex = i + 1;

            // 检查是否已经存在对应的参考点
            auto it = std::find_if(referencePoints.begin(), referencePoints.end(), [=](const ReferPointPara& point) {
                return point.index == refIndex;
            });

            DraggableButton* btn = nullptr;

            if (it == referencePoints.end()) // 如果没有找到，创建新的按钮
            {
                btn = new DraggableButton(frameRerencePoint);
                QPoint btnPos(m_RefPoint[i].pointPos);

                btn->setCheckable(true);
                btn->setAutoExclusive(true);
                btn->setDraggable(false);
                btn->setFixedSize(40, 40);
                btn->setStyleSheet("QPushButton{ border-style:solid; outline:none; border-width:1px; border-radius:20px;}");
                btn->setText(QString::number(refIndex));
                btn->setGeometry(QRect(btnPos, btn->size()));
                btn->show();

                // 添加到 referencePoints
                ReferPointPara point = {refIndex, referName, btn, btnPos};
                referencePoints.push_back(point);
            }
            else // 如果找到了，只更新按钮的位置
            {
                it->button->setGeometry(QRect(m_RefPoint[i].pointPos, it->button->size()));
                it->pointPos = m_RefPoint[i].pointPos; // 更新位置
            }
        }
    }
    updateReferPointsTable();
}

void ReferenceWidget::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    updateReferPoints();
//    initUI(referencePoints);
    this->update();
}
