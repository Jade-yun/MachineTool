#include "referpointdialog.h"
#include "ui_referpointdialog.h"
#include "mainwindow.h"

#include <QCheckBox>
#include <QGroupBox>
#include <QButtonGroup>

ReferPointDialog::ReferPointDialog(QWidget *parent) :
    BaseWindow(parent),
    ui(new Ui::ReferPointDialog)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("参考点指定"));
    setModal(true);
    ui->scrollArea->setWidgetResizable(true);

    connect(ui->btnCancle, &QPushButton::clicked, this, &ReferPointDialog::reject);
    connect(ui->btnOK, &QPushButton::clicked, this, &ReferPointDialog::accept);

}

ReferPointDialog::~ReferPointDialog()
{
    delete ui;
}

//void ReferPointDialog::updateUI(const QList<ReferPointPara> &referPoints)
//{
////    QLayout *oldLayout = ui->scrollWgt->layout();
////    if (oldLayout) {
////        QLayoutItem *item;
////        while ((item = oldLayout->takeAt(0)) != nullptr) {
////            delete item->widget(); // 删除子控件
////            delete item; // 删除布局项
////        }
////        delete oldLayout; // 删除旧的布局
////    }

////    QGroupBox *groupBox = new QGroupBox(tr("参考点指定"), this);

//       QVBoxLayout *mainLayout = new QVBoxLayout(ui->scrollWgt);

//    QGridLayout *gridLayout = new QGridLayout();
//    gridLayout->setVerticalSpacing(20);

//    chboxGroup = new QButtonGroup(this);
//    chboxGroup->setExclusive(true);

//    const int itemsPerRow = 3;
//    int row = 0;
//    int column = 0;

//    int id = 0;
//    QCheckBox* chbox = new QCheckBox(tr("不指定"), this);
//    chbox->setChecked(true);
//    chbox->setCheckable(true);
//    gridLayout->addWidget(chbox, row, column++);
//    chboxGroup->addButton(chbox, id++);

//    for (const auto& refPoint : referPoints) {
//        QCheckBox *checkBox = new QCheckBox(refPoint.name, this);
//        gridLayout->addWidget(checkBox, row, column);
//        chboxGroup->addButton(checkBox, id++);

//        column++;
//        if (column >= itemsPerRow) {
//            column = 0;
//            row++;
//        }
//    }

//    mainLayout->addLayout(gridLayout);
//    mainLayout->addStretch();

//    ui->scrollWgt->setLayout(mainLayout);
//}

void ReferPointDialog::updateUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(ui->scrollWgt);

    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->setVerticalSpacing(20);

    chboxGroup = new QButtonGroup(this);
    chboxGroup->setExclusive(true);

    const int itemsPerRow = 3;
    int row = 0;
    int column = 0;

    int id = 0;
    QCheckBox* chbox = new QCheckBox(tr("不指定"), this);
    chbox->setChecked(true);
    chbox->setCheckable(true);
    gridLayout->addWidget(chbox, row, column++);
    chboxGroup->addButton(chbox, id++);

    for (int i = 0; i < REFERENCE_TOTAL_NUM; i++) {
        if (m_RefPoint[i].refFlag) {

            QCheckBox *checkBox = new QCheckBox(m_RefPoint[i].refName, this);
            gridLayout->addWidget(checkBox, row, column);
            chboxGroup->addButton(checkBox, id++);

            column++;
            if (column >= itemsPerRow) {
                column = 0;
                row++;
            }
        }
    }

    mainLayout->addLayout(gridLayout);
    mainLayout->addStretch();

    ui->scrollWgt->setLayout(mainLayout);
}

QString ReferPointDialog::getSelectedRefPoint() const
{
    for (QCheckBox* chbox : findChildren<QCheckBox*>())
    {
        if (chbox->isChecked())
            return chbox->text();
    }
    return QString();
}

int ReferPointDialog::getRefPointIndex() const
{
//    auto manualWidget = qobject_cast<ManualForm*>(MainWindow::pMainWindow->findChild<ManualForm*>());
//    const QList<ReferPointPara>& referPoints = manualWidget->getRerferPoints();

//    int chboxId = chboxGroup->checkedId();
//    // 未指定参考点
//    if (!chboxId){
//        return 0;
//    }
//    int index = referPoints.isEmpty() ? 0 : referPoints.at(chboxId-1).index;

    int chboxId = chboxGroup->checkedId();
    // 未指定参考点
    if (!chboxId){
        return 0;
    }

    int index = 0;
    QString selectName = chboxGroup->checkedButton()->text();
    for (int i = 0; i < REFERENCE_TOTAL_NUM; i++)
    {
       if (m_RefPoint[i].refFlag)
       {
            if (selectName == m_RefPoint[i].refName)
            {
                index = i + 1;
                break;
            }
       }
    }

    return index;
}

RefPointEdit::RefPointEdit(QWidget *parent)
    : QLineEdit(parent),
      refPointIndex(0)
{

}

int RefPointEdit::getRefPointIndex() const
{
    return refPointIndex;
}

void RefPointEdit::setRefPointIndex(int index)
{
    refPointIndex = index;
}
