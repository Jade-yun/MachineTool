#include "teachmanage.h"
#include "ui_teachmanage.h"
#include "errortipdialog.h"
#include "method/program_save.h"
#include "method/program_popup.h"

#include <QDebug>
#include <QMainWindow>

// 文件权限图标路径
const QStringList authIconPath = {
    ":/images/teachManageImages/unlock.png",
    ":/images/teachManageImages/lock-blue.png",
    ":/images/teachManageImages/lock-yellow.png",
    ":/images/teachManageImages/lock-red.png"
};

TeachManage::TeachManage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TeachManage),
    filePermission(0)
{
    ui->setupUi(this);

    ui->lineEdit_OriginalFileName->setAlignment(Qt::AlignCenter);
    ui->lineEdit_NowFileName->setAlignment(Qt::AlignCenter);
    QPalette palette = ui->lineEdit_OriginalFileName->palette();
    palette.setColor(QPalette::Disabled, QPalette::Text, QColor(Qt::black));
    ui->lineEdit_OriginalFileName->setPalette(palette);
//    ui->lineEdit_OriginalFileName->setStyleSheet("QLineEdit:disabled { color: black; }");



    ui->btn_USB->setCheckable(true);
    ui->stackedWidget_TeachManage->setCurrentIndex(0);

    ui->tableTeachManage->setRowCount(0);
    ui->tableTeachManage->setColumnCount(4);
    ui->tableTeachManage->setFixedWidth(860);
    ui->tableTeachManage->setMaximumWidth(860);
    ui->tableTeachManage->setMaximumHeight(450);
    ui->tableTeachManage->horizontalHeader()->setVisible(true);
    ui->tableTeachManage->verticalHeader()->setVisible(true);

    ui->tableTeachManage->setHorizontalHeaderLabels({"", "", tr("文件名"), tr("最后修改时间")});
    ui->tableTeachManage->setColumnWidth(0, 40);
    ui->tableTeachManage->setColumnWidth(1, 40);
    ui->tableTeachManage->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    ui->tableTeachManage->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);

    ui->tableTeachManage->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableTeachManage->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableTeachManage->setEditTriggers(QAbstractItemView::NoEditTriggers);





    connect(ui->btn_USB, &QPushButton::toggled, [=](bool checked) {
        bool firstColumnHidden = !checked;
        ui->tableTeachManage->setColumnHidden(0, firstColumnHidden);

        if (checked)
        {
            ui->stackedWidget_TeachManage->setCurrentIndex(1);
        }
        else
        {
            ui->stackedWidget_TeachManage->setCurrentIndex(0);
        }
    });


}

TeachManage::~TeachManage()
{
    delete ui;
}

void TeachManage::on_btn_New_clicked()
{

    QString fileName = ui->lineEdit_NowFileName->text();
//    if (fileName.isEmpty())
//    {
//        return;
//    }
//    QStringList fileNames;
//    for (int i = 0; i < ui->tableTeachManage->rowCount(); i++)
//    {
//        QTableWidgetItem* fileNameItem;
//        if(fileNameItem)
//        {
//            fileNameItem = ui->tableTeachManage->item(i, 2);
//            fileNames << fileNameItem->text();
//        }
//    }
//    if (fileNames.contains(fileName))
//    {
//        ErrorTipDialog dlgTip(this);
//        dlgTip.setMessage(tr("文件已经存在！"));
//        dlgTip.exec();
//        return;
//    }
    auto res = newBuildProgram(fileName);
//    res = 0;
    if (res == 0)
    {
        addOneRowToTable(ui->tableTeachManage);
        return;
    }
    // 程序文本不能为空
    else if (res == 1)
    {
        ErrorTipDialog dlgTip(this);
        dlgTip.setMessage(tr("程序文本不能为空"));
        dlgTip.exec();
        return;
    }
    // 程序文件名已存在
    else if (res == 2)
    {
        ErrorTipDialog dlgTip(this);
        dlgTip.setMessage(tr("文件已经存在！"));
        dlgTip.exec();
        return;
    }
    //无法打开文件进行写入
    else if (res == 3)
    {
        ErrorTipDialog dlgTip(this);
        dlgTip.setMessage(tr("无法打开文件进行写入"));
        dlgTip.exec();
        return;
    }
}

void TeachManage::on_btn_Copy_clicked()
{

}

void TeachManage::on_btn_Load_clicked()
{
    int curProgramIndex = -1;
    int curRow = ui->tableTeachManage->currentIndex().row();
    QString fileName = ui->tableTeachManage->item(curRow, 2)->text();

    for(int i=0;i<m_ProgramNameAndPath.count();i++)
    {
        if(m_ProgramNameAndPath[i].fileName == fileName)
        {
            curProgramIndex = i;
            break;
        }
    }
    if (curProgramIndex == -1)
    {
        // need a tip dialog
        qDebug() << "cannot find Program " << fileName;
        return;
    }
    //如果选择的文件不是当前程序，进行载入
    if(m_CurrentProgramNameAndPath.index != m_ProgramNameAndPath[curProgramIndex].index)
    {//如果选择的文件不是当前程序，进行载入
        for(int i=0;i<PRO_NUM;i++)
        {
            for(int j=0;j<PRO_LINE_MAIN;j++)
            {
                g_FreeProOrder(&m_ProOrder[i][j]);//释放程序命令的数据指针
            }
        }
        memset(&m_CurrentProgramNameAndPath,0,sizeof(D_ProgramNameAndPathStruct));//清除当前程序信息
        memcpy(&m_CurrentProgramNameAndPath,&m_ProgramNameAndPath[curProgramIndex],sizeof(D_ProgramNameAndPathStruct));//改变当前程序信息
        readProgram(m_CurrentProgramNameAndPath);//读取当前程序指令
        memcpy(&m_OperateProOrder,&m_ProOrder,sizeof(P_ProOrderStruct));//将读取的程序赋给当前操作程序
    }
//    QMainWindow* mainWindow =  qobject_cast<QMainWindow*>(this->parent());
//    if(mainWindow)
//    {
//        mainWindow->setSysProgramName(QString("展晖机床机械手控制系统") + m_CurrentProgramNameAndPath.fileName);
//    }
}

void TeachManage::on_btn_Preview_clicked()
{


}

void TeachManage::on_btn_Del_clicked()
{
    deleteOneRowFromTable(ui->tableTeachManage);
}

void TeachManage::on_btn_SelectAll_clicked()
{
    for (QCheckBox* chbox : ui->tableTeachManage->findChildren<QCheckBox*>())
    {
        chbox->setChecked(true);
    }
}

void TeachManage::on_btn_SelectNotAll_clicked()
{
    for (QCheckBox* chbox : ui->tableTeachManage->findChildren<QCheckBox*>())
    {
        chbox->setChecked(false);
    }
}

void TeachManage::on_btn_Export_clicked()
{

    // check whether insert the USB FLASH fistly.
//    if (checkUSBInsert() == false)
//    {

//         // remind
//        return;
//    }
    QVector<int> selectedRow;
    for (int i = 0; i < ui->tableTeachManage->rowCount(); i++)
    {
        QCheckBox* chbox = qobject_cast<QCheckBox*>(ui->tableTeachManage->cellWidget(i, 0));
        if (chbox->isChecked())
        {
            selectedRow.push_back(i);
        }
    }


}

void TeachManage::on_btn_Import_clicked()
{

}

void TeachManage::deleteOneRowFromTable(QTableWidget *tableTeachManage)
{
    QList<QTableWidgetItem *> selectedItems = tableTeachManage->selectedItems();

    // 如果没有选中任何行，则直接返回
    if (selectedItems.isEmpty()) {
        return;
    }

    // 获取选中的行索引（假设单选模式，只选一行）
    int rowIndex = selectedItems.first()->row();

    // 删除该行之前，先处理每个单元格的组件
    for (int col = 0; col < tableTeachManage->columnCount(); ++col) {
        QWidget *cellWidget = tableTeachManage->cellWidget(rowIndex, col);
        if (cellWidget) {
            delete cellWidget; // 删除单元格中的QWidget对象
        } else {
            QTableWidgetItem *item = tableTeachManage->item(rowIndex, col);
            if (item) {
                delete item; // 删除普通的表格项
            }
        }
    }

    // 删除该行
    tableTeachManage->removeRow(rowIndex);

    // 删除空白行
    for (int row = tableTeachManage->rowCount() - 1; row >= 0; --row) {
        bool isRowEmpty = true;
        for (int col = 0; col < tableTeachManage->columnCount(); ++col) {
            QTableWidgetItem *item = tableTeachManage->item(row, col);
            if (item && !item->text().isEmpty()) {
                isRowEmpty = false;
                break;
            }
        }
        if (isRowEmpty) {
            tableTeachManage->removeRow(row);
        }
    }

    // 更新布局使表格填满
//    tableTeachManage->resizeColumnsToContents();
    tableTeachManage->resizeRowsToContents();

}

void TeachManage::addOneRowToTable(QTableWidget *tableTeachManage)
{
    if (tableTeachManage->rowCount() != 0)
    {
        QList<QTableWidgetItem *> selectedItems = tableTeachManage->selectedItems();
        if (selectedItems.isEmpty()) {
            return;
        }
    }

    int newRow = tableTeachManage->rowCount();
    tableTeachManage->insertRow(newRow);
    QCheckBox* chbox = new QCheckBox(tableTeachManage);
    tableTeachManage->setCellWidget(newRow, 0, chbox);
//    QTableWidgetItem* chbox= new QTableWidgetItem();
//    tableTeachManage->setItem(newRow, 0, chbox);
//    tableTeachManage->item(newRow, 0)->setCheckState(Qt::Checked);

    // add Icon of Limit Authority
//    tableTeachManage->setItem(newRow, 1, new QTableWidgetItem());
//    tableTeachManage->item(newRow, 1)->setIcon(QIcon(":/images/teachManageImages/unlock.png"));
//    tableTeachManage->setIconSize(QSize(tableTeachManage->columnWidth(0), tableTeachManage->rowHeight(newRow)));
    QLabel* labIcon = new QLabel(tableTeachManage);
//    labIcon->setProperty("filePermission", 0);
    setFilePermision(labIcon, 0);
    QPixmap pix(authIconPath[0]);
    pix.scaled(tableTeachManage->columnWidth(1), tableTeachManage->rowHeight(newRow));
    labIcon->setPixmap(pix);
    tableTeachManage->setCellWidget(newRow, 1, labIcon);


    QString fileName = ui->lineEdit_NowFileName->text();
    tableTeachManage->setItem(newRow, 2, new QTableWidgetItem(fileName));

    QString changeTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    tableTeachManage->setItem(newRow, 3, new QTableWidgetItem(changeTime));

    tableTeachManage->setColumnHidden(0, true);
    tableTeachManage->resizeRowsToContents();
}

void TeachManage::setFilePermision(QWidget *lab, const QVariant &permission)
{
    lab->setProperty("filePermission", permission);
}

int TeachManage::getFilePermission(QWidget *lab) const
{
    return lab->property("filePermission").toInt();
}

void TeachManage::updateFilePermission(int iconIndex)
{

    int curRow = ui->tableTeachManage->currentRow();
    if (curRow < 0)
        return;

    QLabel* labIcon = qobject_cast<QLabel*>(ui->tableTeachManage->cellWidget(curRow, 1));
    if (!labIcon)
        return;
    setFilePermision(labIcon, iconIndex);

    QPixmap pix(authIconPath[iconIndex]);
//    pix = pix.scaled(ui->tableTeachManage->columnWidth(1), ui->tableTeachManage->rowHeight(curRow), Qt::KeepAspectRatio);
    labIcon->setPixmap(pix);
}

void TeachManage::on_tableTeachManage_itemSelectionChanged()
{
    int curRow = ui->tableTeachManage->currentRow();
    filePermission = getFilePermission(ui->tableTeachManage->cellWidget(curRow, 1));
    if (filePermission == 0)
    {
        ui->chboxAllow->setChecked(true);
    }
    else if (filePermission == 1)
    {
        ui->chboxTimeAndPositionAllow->setChecked(true);
    }
    else if (filePermission == 2)
    {
        ui->chboxTimeAllow->setChecked(true);
    }
    else if (filePermission == 3)
    {
        ui->chboxForbide->setChecked(true);
    }

    // 在原文件名编辑框中显示文件名
    QString fileName = ui->tableTeachManage->item(curRow, 2)->text();
    ui->lineEdit_OriginalFileName->setText(fileName);
}

void TeachManage::on_chboxAllow_clicked()
{
    qDebug() << "on_chboxAllow_clicked";

    updateFilePermission(0);
}

void TeachManage::on_chboxTimeAndPositionAllow_clicked()
{
    qDebug() << "on_chboxTimeAndPositionAllow_clicked" ;
    updateFilePermission(1);
}

void TeachManage::on_chboxTimeAllow_clicked()
{
    qDebug() << "on_chboxTimeAllow_clicked" ;

    updateFilePermission(2);
}

void TeachManage::on_chboxForbide_clicked()
{
    qDebug() << "on_chboxForbide_clicked";
    updateFilePermission(3);
}
