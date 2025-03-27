#include "producthistorydialog.h"
#include "ui_producthistorydialog.h"

#include "cmd.h"
#include "framelesswidget2.h"

ProductHistoryDialog::ProductHistoryDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProductHistoryDialog)
{
    ui->setupUi(this);

    this->setModal(true);

    m_productHistoryModel = new ProductHistoryModel(this);
    ui->tableView->setModel(m_productHistoryModel);

    // verticalHeader() 调用必须在 setModel 之后（之前为空)
    ui->tableView->verticalHeader()->setDefaultSectionSize(50);
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch); // 时间列自动适应内容
    ui->tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents); // 产量列填充剩余空间
    ui->tableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents); // 成型周期列填充
    ui->tableView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents); // 取物时间列填充

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &ProductHistoryDialog::checkProductNum);
    m_timer->start(1000);  // 每秒检查一次产量

    connect(ui->btnOK, &QPushButton::clicked, this, &QDialog::accept);

//    FramelessWidget2* frameBase = new FramelessWidget2(this);
//    frameBase->setWidget(this);
}

ProductHistoryDialog::~ProductHistoryDialog()
{
    delete ui;
}

void ProductHistoryDialog::checkProductNum()
{
    int currentProductNum = m_RunInfo.actualProductNum;
    static int lastProductNum = currentProductNum;
    if (currentProductNum != lastProductNum) { // 产量增加
        m_productHistoryModel->appendProductRecord(currentProductNum,
                                                   m_RunInfo.takeShapeCyc,
                                                   m_RunInfo.fetchTime);
        lastProductNum = currentProductNum; // 更新上次产量
    }
}
