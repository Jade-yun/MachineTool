#include "referencewidget.h"

#include <QHeaderView>
#include "mainwindow.h"

extern QString REFERPOINT_PIC;

ReferenceWidget::ReferenceWidget(QWidget *parent)
    : QDialog(parent), selectedReferPoint(nullptr)
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
        }
    });

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
        if (selectedReferPoint)
        {
            auto it = std::find_if(referencePoints.begin(), referencePoints.end(), [=](const ReferPointPara& point) {
                return point.button == selectedReferPoint;
            });

            if (it != referencePoints.end())
            {
                static FullKeyboard keyboard(this);

                keyboard.setText(it->name);
                keyboard.setCurrentEditObj(textReferPointName);
                keyboard.exec(); // must use exec here, using open cannot ensure to save the name.

                QString text = keyboard.getInputText();

//                keyboard->setText(it->name);
//                keyboard->setCurrentEditObj(textReferPointName);
//                keyboard->exec(); // must use exec here, using open cannot ensure to save the name.

//                QString text = keyboard->getInputText();


                if (it->name != text )
                {
                    it->name = text;;
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
    tableReference->setGeometry(5, 0, 760, 40);
    tableReference->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableReference->setVisible(referencePoints.size());
    tableReference->setFrameShape(QFrame::NoFrame);
    tableReference->setFrameShadow(QFrame::Raised);
    tableReference->setMaximumHeight(40 * 3);
    tableReference->setMinimumWidth(760);
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
//    keyboard->hide();
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

void ReferenceWidget::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    updateReferPoints();
    updateUI(referencePoints);
    this->update();
}
