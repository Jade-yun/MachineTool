#include "referpointdialog.h"
#include "ui_referpointdialog.h"

#include <QCheckBox>
#include <QGroupBox>
#include <QButtonGroup>

ReferPointDialog::ReferPointDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReferPointDialog)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("参考点指定"));

    ui->scrollArea->setWidgetResizable(true);

    connect(ui->btnCancle, &QPushButton::clicked, this, &ReferPointDialog::reject);
    connect(ui->btnCancle, &QPushButton::clicked, this, &ReferPointDialog::accept);

}

ReferPointDialog::~ReferPointDialog()
{
    delete ui;
}

void ReferPointDialog::updateUI(const QList<ReferPointPara> &referPoints)
{
//    QLayout *oldLayout = ui->scrollWgt->layout();
//    if (oldLayout) {
//        QLayoutItem *item;
//        while ((item = oldLayout->takeAt(0)) != nullptr) {
//            delete item->widget(); // 删除子控件
//            delete item; // 删除布局项
//        }
//        delete oldLayout; // 删除旧的布局
//    }

//    QGroupBox *groupBox = new QGroupBox(tr("参考点指定"), this);

       QVBoxLayout *mainLayout = new QVBoxLayout(ui->scrollWgt);

    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->setVerticalSpacing(20);

    QButtonGroup *buttonGroup = new QButtonGroup(this);
    buttonGroup->setExclusive(true);

    const int itemsPerRow = 3;
    int row = 0;
    int column = 0;

    QCheckBox* chbox = new QCheckBox(tr("不指定"), this);
    chbox->setChecked(true);
    gridLayout->addWidget(chbox, row, column++);
    buttonGroup->addButton(chbox);

    for (const auto& refPoint : referPoints) {
        QCheckBox *checkBox = new QCheckBox(refPoint.name, this);
        gridLayout->addWidget(checkBox, row, column);
        buttonGroup->addButton(checkBox);

        column++;
        if (column >= itemsPerRow) {
            column = 0;
            row++;
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

RefPointEdit::RefPointEdit(QWidget *parent)
    : QLineEdit(parent)
{

}

void RefPointEdit::mouseReleaseEvent(QMouseEvent *event)
{
    QLineEdit::mouseReleaseEvent(event);

    ReferPointDialog dlg;
    QWidget *topLevelWidget = this;
    while (topLevelWidget->parentWidget() != nullptr) {
        topLevelWidget = topLevelWidget->parentWidget();
    }
    auto manualWidget = qobject_cast<ManualForm*>(topLevelWidget->findChild<ManualForm*>("manualWidget"));
    if (manualWidget)
    {
        dlg.updateUI(manualWidget->getRerferPoints());
        if (dlg.exec() == QDialog::Accepted)
        {
            QString refPointName =  dlg.getSelectedRefPoint();
            this->setText(refPointName);
        }
    }
}
