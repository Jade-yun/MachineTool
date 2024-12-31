#include "teachmanage.h"
#include "ui_teachmanage.h"
#include "errortipdialog.h"
#include "method/program_save.h"
#include "method/program_popup.h"
#include "usbdisk.h"
#include "file_preview.h"

#include <QDebug>
#include <QMainWindow>
#include <QComboBox>
#include <QFileDialog>
#include <QDir>

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

    ui->tableTeachManage->setColumnHidden(0, true);

    init();

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
        ErrorTipDialog dlgTip;
        dlgTip.setMessage(tr("程序文本不能为空"));
        dlgTip.exec();
        return;
    }
    // 程序文件名已存在
    else if (res == 2)
    {
        ErrorTipDialog dlgTip;
        dlgTip.setMessage(tr("文件已经存在！"));
        dlgTip.exec();
        return;
    }
    //无法打开文件进行写入
    else if (res == 3)
    {
        ErrorTipDialog dlgTip;
        dlgTip.setMessage(tr("无法打开文件进行写入"));
        dlgTip.exec();
        return;
    }
}

void TeachManage::on_btn_Copy_clicked()
{
    int curRow = ui->tableTeachManage->currentRow();
    QString curTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    QString fileName = ui->tableTeachManage->item(curRow, 2)->text() + ".txt";
    QString filePath = m_ProgramPath + "/" + fileName;
    QFile sourceFile(filePath);

    QString programName = ui->lineEdit_NowFileName->text();
    if (programName.isEmpty())
    {
        ErrorTipDialog tip(tr("程序名不能为空！"), TipMode::ONLY_OK);
        tip.exec();
        return;
    }

    QString targeFileName = ui->lineEdit_NowFileName->text() + ".txt";
    QString targeFilePath = QDir(m_ProgramPath).filePath(targeFileName);

    if (QFile::exists(targeFilePath))
    {
        ErrorTipDialog dlgTip(tr("文件已存在！"), TipMode::ONLY_OK);
        dlgTip.exec();
        return;
    }

    bool copied = false;
    if (sourceFile.open(QIODevice::ReadOnly))
    {
        QFile targetFile(targeFilePath);
        if (targetFile.open(QIODevice::WriteOnly))
        {
            targetFile.write(sourceFile.readAll());
            targetFile.close();

            copied = true;
        }
        sourceFile.close();
    }

    if (copied)
    {
        {
            D_ProgramNameAndPathStruct P_NamePathTemp;
            P_NamePathTemp.fileName = ui->lineEdit_NowFileName->text();
            m_FileNameNum++;
            P_NamePathTemp.filePath = targeFilePath;
            P_NamePathTemp.index = m_FileNameNum;
            P_NamePathTemp.changeTime= curTime;
            m_ProgramNameAndPath.append(P_NamePathTemp);
            SetProgramIndex();//设置程序文件编号
        }

        // fresh table dispaly

        auto tableTeachManage = ui->tableTeachManage;
        int newRow = tableTeachManage->rowCount();
        tableTeachManage->insertRow(newRow);
        QCheckBox* chbox = new QCheckBox(tableTeachManage);
        tableTeachManage->setCellWidget(newRow, 0, chbox);

        QLabel* labIcon = new QLabel(tableTeachManage);
        setFilePermision(labIcon, 0);
        QPixmap pix(authIconPath[0]);
        pix.scaled(tableTeachManage->columnWidth(1), tableTeachManage->rowHeight(newRow));
        labIcon->setPixmap(pix);
        tableTeachManage->setCellWidget(newRow, 1, labIcon);

        QString fileText = ui->lineEdit_NowFileName->text();
        tableTeachManage->setItem(newRow, 2, new QTableWidgetItem(fileText));
        tableTeachManage->setItem(newRow, 3, new QTableWidgetItem(curTime));

        tableTeachManage->setColumnHidden(0, true);
        tableTeachManage->resizeRowsToContents();
    }
}

void TeachManage::on_btn_Load_clicked()
{
    int curRow = ui->tableTeachManage->currentIndex().row();
    if(curRow<0)
    {
        curRow = 0;
    }
    QString fileName = ui->tableTeachManage->item(curRow, 2)->text();
    Load_Program_Handle(fileName);
}

void TeachManage::on_btn_Preview_clicked()
{
    int curProgramIndex = -1;
    int curRow = ui->tableTeachManage->currentIndex().row();
    if(curRow<0)
    {
        curRow = 0;
    }
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
    File_Preview *filePreview = new File_Preview(curProgramIndex,this);
    filePreview->setAttribute(Qt::WA_DeleteOnClose,true);//当对话框关闭时自动删除对话框
    if(filePreview->exec() == QDialog::Accepted)
    {//不做任何处理
    }

}

void TeachManage::on_btn_Del_clicked()
{
    ErrorTipDialog tip(tr("确认删除文件？"));
    if (tip.exec() == QDialog::Accepted)
    {
//    deleteOneRowFromTable(ui->tableTeachManage);

        int curRow = ui->tableTeachManage->currentRow();
        QString programName = ui->tableTeachManage->item(curRow, 2)->text();
        // juge whether the program can been deleted
        // to do...
//        if ()

        int selectedProgramIndex = -1;
        for(int i = 0; i < m_ProgramNameAndPath.count(); i++)
        {
            if(m_ProgramNameAndPath[i].fileName == programName)
            {
                selectedProgramIndex = i;
                break;
            }
        }

        // insure that selected program is not the current running
        if(m_CurrentProgramNameAndPath.fileName != m_ProgramNameAndPath[selectedProgramIndex].fileName)
        {
            // update program struct
            m_ProgramNameAndPath.removeAt(selectedProgramIndex);
            m_FileNameNum--;

            // delete file
            const QString filePath = m_ProgramPath + "/" + programName + ".txt";
            QFile file(filePath);

            if (file.exists())
            {
                file.remove();
            }

            // fresh table dispaly
            deleteOneRowFromTable(ui->tableTeachManage);
            SetProgramIndex();//设置程序文件编号
            setProgramNameAndPath(m_ProgramNameAndPath);//更新文件保存文件中的内容
        }
        else
        {
            ErrorTipDialog tip(tr("不能删除当前已经加载文件！"), TipMode::ONLY_OK);
            tip.exec();
        }
    }
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

    // check whether insert the USB FLASH Drive fistly.
    if (UsbDisk::instance()->isInserted() == false)
    {

         // remind
        ErrorTipDialog tip(tr("请插入U盘！"));
        tip.exec();
        return;
    }
    QStringList selectedFiles; // 当前选中的文件

    for (int i = 0; i < ui->tableTeachManage->rowCount(); i++)
    {
        QCheckBox* chbox = qobject_cast<QCheckBox*>(ui->tableTeachManage->cellWidget(i, 0));
        if (chbox->isChecked())
        {
//            selectedRow.push_back(i);
            QString programName = ui->tableTeachManage->item(i, 2)->text();
            selectedFiles << programName;
        }
    }

//    qDebug() << selectedFiles;


    for (const auto &programName : selectedFiles)
    {
        QString file = m_ProgramPath+ "/" + programName+ ".txt";
        UsbDisk::instance()->copyToUsb(file, "program/");
    }


}

void TeachManage::on_btn_Import_clicked()
{
    if (UsbDisk::instance()->isInserted() == false)
    {
//        qDebug() << tr("请插入U盘！");
        ErrorTipDialog tip(tr("请插入U盘！"));
        tip.show();
        return;
    }

//    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
//                                                    "/run/media/sda1",
//                                                    tr("Text (*.txt)"));//文件类型
    QFileDialog *fileDialog = new QFileDialog(this);
    fileDialog->setFixedSize(800, 600);
    fileDialog->setViewMode(QFileDialog::Detail);
    fileDialog->setFileMode(QFileDialog::ExistingFile);
    QString mountPoint = UsbDisk::instance()->getUsbMountPoint();
    fileDialog->setDirectory(mountPoint);
    fileDialog->setNameFilter(tr("Text(*.txt)"));

    fileDialog->setAcceptDrops(false);
    fileDialog->setOptions(QFileDialog::ReadOnly | QFileDialog::DontUseNativeDialog);

    for (auto& cobox : fileDialog->findChildren<QComboBox*>())
    {
        cobox->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    }

    bool canceled = false;
    if (fileDialog->exec() == QDialog::Rejected)
    {
        canceled = true;
    }

    QStringList selectedFiles;
    if (!canceled)
    {
       selectedFiles = fileDialog->selectedFiles();
    }

    if (selectedFiles.isEmpty() || canceled) {
        ErrorTipDialog tip(tr("没有选择要导入的文件！"), TipMode::ONLY_OK);
        tip.exec();
        return;
    }
    else
    {
        QString file = selectedFiles.first(); // 获取第一个文件路径
        qDebug() << "Selected file:" << file;
        {
            QFileInfo fileInfo(file);

            QString programName = fileInfo.fileName().split(".").first();
            QString fileTime = fileInfo.lastModified().toString("yyyy-MM-dd hh:mm:ss");

            auto it = std::find_if(m_ProgramNameAndPath.begin(), m_ProgramNameAndPath.end(),
                                   [&](const D_ProgramNameAndPathStruct& item) { return item.fileName == programName; });

            if (it != m_ProgramNameAndPath.end())
            {
                ErrorTipDialog tip(tr("系统中存在相同文件，请确认是否覆盖？"));
                if (tip.exec() == QDialog::Accepted)
                {
                    it->fileName = programName;
                    //                    m_FileNameNum++;
                    it->filePath = m_ProgramPath + "/" + programName + ".txt";
                    //                    prog.index = m_FileNameNum;
                    it->changeTime= fileTime;
                    UsbDisk::instance()->copy(file, m_ProgramPath + "/");

                    {
                        for (int i = 0; i < ui->tableTeachManage->rowCount(); i++)
                        {

                            QString text = ui->tableTeachManage->item(i, 2)->text();
                            if (text == programName)
                            {
                                // update program permission

                                // update time
                                ui->tableTeachManage->item(i, 3)->setText(fileTime);

                            }
                        }
                    }

                }
                else
                {
                    return;
                }
            }
            else
            {
                D_ProgramNameAndPathStruct P_NamePathTemp;
                P_NamePathTemp.fileName = programName;
                m_FileNameNum++;
                P_NamePathTemp.filePath = m_ProgramPath + "/" + programName + ".txt";
                P_NamePathTemp.index = m_FileNameNum;
                P_NamePathTemp.changeTime = fileTime;
                m_ProgramNameAndPath.append(P_NamePathTemp);
                SetProgramIndex();//设置程序文件编号
                UsbDisk::instance()->copy(file, m_ProgramPath + "/");

                {
                    auto tableTeachManage = ui->tableTeachManage;
                    int newRow = tableTeachManage->rowCount();
                    tableTeachManage->insertRow(newRow);
                    QCheckBox* chbox = new QCheckBox(tableTeachManage);
                    tableTeachManage->setCellWidget(newRow, 0, chbox);

                    QLabel* labIcon = new QLabel(tableTeachManage);
                    //    labIcon->setProperty("filePermission", 0);
                    setFilePermision(labIcon, 0);
                    QPixmap pix(authIconPath[0]);
                    pix.scaled(tableTeachManage->columnWidth(1), tableTeachManage->rowHeight(newRow));
                    labIcon->setPixmap(pix);
                    tableTeachManage->setCellWidget(newRow, 1, labIcon);

                    tableTeachManage->setItem(newRow, 2, new QTableWidgetItem(programName));
                    tableTeachManage->setItem(newRow, 3, new QTableWidgetItem(fileTime));

                    tableTeachManage->setColumnHidden(0, true);
                    tableTeachManage->resizeRowsToContents();
                }
            }

        }
    }

    ErrorTipDialog tip(tr("文件导入成功！"), TipMode::ONLY_OK);
    tip.exec();

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
//    if (tableTeachManage->rowCount() != 0)
//    {
//        QList<QTableWidgetItem *> selectedItems = tableTeachManage->selectedItems();
//        if (selectedItems.isEmpty()) {
//            return;
//        }
//    }

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
//    qDebug() << "on_chboxAllow_clicked";

    updateFilePermission(0);
}

void TeachManage::on_chboxTimeAndPositionAllow_clicked()
{
//    qDebug() << "on_chboxTimeAndPositionAllow_clicked" ;
    updateFilePermission(1);
}

void TeachManage::on_chboxTimeAllow_clicked()
{
//    qDebug() << "on_chboxTimeAllow_clicked" ;

    updateFilePermission(2);
}

void TeachManage::on_chboxForbide_clicked()
{
//    qDebug() << "on_chboxForbide_clicked";
    updateFilePermission(3);
}

void TeachManage::init()
{
#if 0
    // update all program file to table
    QDir dir(m_ProgramPath);
    QFileInfoList fileList = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);

    for (const auto& fileInfo : fileList)
    {
        QString programName = fileInfo.fileName().split(".").first();
        QString fileTime = fileInfo.lastModified().toString("yyyy-MM-dd hh:mm:ss");
        QString filePath = fileInfo.absoluteFilePath();

        auto tableTeachManage = ui->tableTeachManage;
        int newRow = tableTeachManage->rowCount();
        tableTeachManage->insertRow(newRow);
        QCheckBox* chbox = new QCheckBox(tableTeachManage);
        tableTeachManage->setCellWidget(newRow, 0, chbox);

        QLabel* labIcon = new QLabel(tableTeachManage);
//        labIcon->setProperty("filePermission", 0);
        setFilePermision(labIcon, 0);
        QPixmap pix(authIconPath[0]);
        pix.scaled(tableTeachManage->columnWidth(1), tableTeachManage->rowHeight(newRow));
        labIcon->setPixmap(pix);
        tableTeachManage->setCellWidget(newRow, 1, labIcon);

        tableTeachManage->setItem(newRow, 2, new QTableWidgetItem(programName));
        tableTeachManage->setItem(newRow, 3, new QTableWidgetItem(fileTime));

        tableTeachManage->setColumnHidden(0, true);
        tableTeachManage->resizeRowsToContents();

        D_ProgramNameAndPathStruct P_NamePathTemp;
        P_NamePathTemp.fileName = programName;
        m_FileNameNum++;
        P_NamePathTemp.filePath = filePath;
        P_NamePathTemp.index = m_FileNameNum;
        P_NamePathTemp.changeTime= fileTime;
        m_ProgramNameAndPath.append(P_NamePathTemp);

    }
#else
    // update all program file to table
    QDir dir(m_ProgramPath);
    QFileInfoList fileList = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    for (const auto& fileInfo : fileList)
    {
        for(int i = 0;i<m_ProgramNameAndPath.count();i++)
        {
            if(m_ProgramNameAndPath[i].fileName == fileInfo.fileName().split(".").first())
            {
                auto tableTeachManage = ui->tableTeachManage;
                int newRow = tableTeachManage->rowCount();
                tableTeachManage->insertRow(newRow);
                QCheckBox* chbox = new QCheckBox(tableTeachManage);
                tableTeachManage->setCellWidget(newRow, 0, chbox);

                QLabel* labIcon = new QLabel(tableTeachManage);
        //        labIcon->setProperty("filePermission", 0);
                setFilePermision(labIcon, m_ProgramNameAndPath[i].filePermission);
                QPixmap pix(authIconPath[m_ProgramNameAndPath[i].filePermission]);
                pix.scaled(tableTeachManage->columnWidth(1), tableTeachManage->rowHeight(newRow));
                labIcon->setPixmap(pix);
                tableTeachManage->setCellWidget(newRow, 1, labIcon);

                tableTeachManage->setItem(newRow, 2, new QTableWidgetItem(m_ProgramNameAndPath[i].fileName));
                tableTeachManage->setItem(newRow, 3, new QTableWidgetItem(m_ProgramNameAndPath[i].changeTime));

                tableTeachManage->setColumnHidden(0, true);
                tableTeachManage->resizeRowsToContents();
                m_FileNameNum++;
            }
        }
    }

#endif
}
