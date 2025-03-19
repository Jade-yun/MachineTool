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
#include <QDir>

#include "RefreshKernelBuffer.h"

// 文件权限图标路径
const QStringList authIconPath = {
    ":/images/teachManageImages/unlock.png",
    ":/images/teachManageImages/lock-blue.png",
    ":/images/teachManageImages/lock-yellow.png",
    ":/images/teachManageImages/lock-red.png"
};

extern const QString SUFFIX_PROGRAM;

TeachManage::TeachManage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TeachManage),
    filePermission(0)
{
    ui->setupUi(this);

    initWidgets();

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

    QString programName = ui->lineEdit_NowFileName->text().trimmed();
    QRegularExpression illegalChars("[<>:\"/\\\\|?*.]"); // "\/:*?"<>|"

    if (programName.contains(illegalChars))
    {
        ErrorTipDialog dlgTip;
        dlgTip.setMessage(tr("程序名格式错误！"));
        dlgTip.exec();
        return;
    }

    auto res = newBuildProgram(programName);
//    res = 0;
    if (res == 0)
    {
        QString changeTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
//        addOneRowToTable(ui->tableTeachManage);
        addNewRowToTable(programName, changeTime, 0);
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
    if (curRow == -1)
    {
        ErrorTipDialog tip(tr("请选择一个要复制的程序！"), TipMode::ONLY_OK);
        tip.exec();
        return;
    }
    QString curTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    QString sourceProgName = ui->tableTeachManage->item(curRow, 2)->text();
    QString filePath = m_ProgramPath + "/" + sourceProgName + SUFFIX_PROGRAM;
//    QFile sourceFile(filePath);

    QString targeProgName = ui->lineEdit_NowFileName->text().trimmed();
    if (targeProgName.isEmpty())
    {
        ErrorTipDialog tip(tr("程序名不能为空！"), TipMode::ONLY_OK);
        tip.exec();
        return;
    }
    QRegularExpression illegalChars("[<>:\"/\\\\|?*.]"); // "\/:*?"<>|"

    if (targeProgName.contains(illegalChars))
    {
        ErrorTipDialog dlgTip;
        dlgTip.setMessage(tr("程序名格式错误！"));
        dlgTip.exec();
        return;
    }

    QString targetFilePath = QDir(m_ProgramPath).filePath(targeProgName + SUFFIX_PROGRAM);

    if (QFile::exists(targetFilePath))
    {
        ErrorTipDialog dlgTip(tr("文件已存在！"), TipMode::ONLY_OK);
        dlgTip.exec();
        return;
    }

    bool copied = false;

    QDir progDir(m_ProgramPath);
    QString fileNamePattern = QString("%1.*").arg(sourceProgName);
    QFileInfoList fileInfoList = progDir.entryInfoList(QStringList() << fileNamePattern,
                                               QDir::Files | QDir::NoDotAndDotDot);

    for (const auto& fileInfo : fileInfoList)
    {
        QString sourceFilePath = fileInfo.absoluteFilePath();

        QString targetFilePathWithSuffix = targetFilePath.left(targetFilePath.lastIndexOf('.')) + "." + fileInfo.suffix();
        copied = QFile::copy(sourceFilePath, targetFilePathWithSuffix);
    }

    if (copied)
    {

        D_ProgramNameAndPathStruct P_NamePathTemp;
        P_NamePathTemp.fileName = ui->lineEdit_NowFileName->text();
        m_FileNameNum++;
        P_NamePathTemp.filePath = targetFilePath;
        P_NamePathTemp.index = m_FileNameNum;
        P_NamePathTemp.filePermission = ::getProgramPermission(sourceProgName);
        P_NamePathTemp.changeTime= curTime;
        m_ProgramNameAndPath.append(P_NamePathTemp);
        SetProgramIndex();//设置程序文件编号

        ::setProgramNameAndPath(m_ProgramNameAndPath);


        // fresh table dispaly
        addNewRowToTable(targeProgName, curTime, P_NamePathTemp.filePermission);
    }
    else
    {
        ErrorTipDialog tip(tr("文件复制失败！"), TipMode::ONLY_OK);
        tip.exec();
    }
}

void TeachManage::on_btn_Load_clicked()
{
    int curRow = ui->tableTeachManage->currentIndex().row();
    if(curRow < 0)
    {
        ErrorTipDialog tip(tr("请选择一个要加载的程序！"), TipMode::ONLY_OK);
        tip.exec();
        return;
    }
    QString fileName = ui->tableTeachManage->item(curRow, 2)->text();
    if(!Load_Program_Handle(fileName))
    {//如果加载失败，加载回之前的程序
        Load_Program_Handle(m_CurrentProgramNameAndPath.fileName);
    }
    emit labProgramNameChangeSignal();
    emit programLoaded();
}

void TeachManage::on_btn_Preview_clicked()
{
    int curProgramIndex = -1;
    int curRow = ui->tableTeachManage->currentIndex().row();
    if(curRow<0)
    {
        ErrorTipDialog tip(tr("请选择一个要预览的程序！"), TipMode::ONLY_OK);
        tip.exec();
        return;
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
    int curRow = ui->tableTeachManage->currentRow();
    if (curRow < 0) return;

    QString programName = ui->tableTeachManage->item(curRow, 2)->text();

    int selectedProgramIndex = -1;
    for(int i = 0; i < m_ProgramNameAndPath.count(); i++)
    {
        if(m_ProgramNameAndPath[i].fileName == programName)
        {
            selectedProgramIndex = i;
            break;
        }
    }

    if (selectedProgramIndex < 0) return;

    // juge whether the program can been deleted
    // to do...
    auto perm = m_ProgramNameAndPath[selectedProgramIndex].filePermission;
    if (perm)
    {
        ErrorTipDialog tip(tr("请先对文件进行解锁！"), TipMode::ONLY_OK, nullptr);
        tip.exec();

        return;
    }


    ErrorTipDialog tip(tr("确认删除文件？"));
    if (tip.exec() == QDialog::Accepted)
    {
//    deleteOneRowFromTable(ui->tableTeachManage);

        // insure that selected program is not the current running
        if(m_CurrentProgramNameAndPath.fileName != m_ProgramNameAndPath[selectedProgramIndex].fileName)
        {
            // update program struct
            m_ProgramNameAndPath.removeAt(selectedProgramIndex);
            m_FileNameNum--;

            // delete file
#if 0
            const QString filePath = m_ProgramPath + "/" + programName + SUFFIX_PROGRAM;
            QFile file(filePath);

            if (file.exists())
            {
                file.remove();
            }
#else
            // delete all the file corresponding the programName.
            QString fileNamePattern = QString("%1.*").arg(programName);
            QDir dir(m_ProgramPath);
            QFileInfoList fileList = dir.entryInfoList(QStringList() << fileNamePattern,
                                                       QDir::Files | QDir::NoDotAndDotDot);

            for (const QFileInfo& fileInfo : fileList)
            {
                QFile file(fileInfo.absoluteFilePath());
                if (file.exists())
                {
                    file.remove();
                }
            }
#endif
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
//        QString file = m_ProgramPath+ "/" + programName+ SUFFIX_PROGRAM;
//        UsbDisk::instance()->copyToUsb(file, "HMI/" + programName);

        QDir dir(m_ProgramPath);
        QString fileNamePattern = QString("%1.*").arg(programName);
        QFileInfoList fileList = dir.entryInfoList(QStringList() << fileNamePattern,
                                                   QDir::Files | QDir::NoDotAndDotDot);
        for (const QFileInfo& fileInfo : fileList)
        {
            UsbDisk::instance()->copyToUsb(fileInfo.absoluteFilePath(), "HMI/");
        }
        ::sync();
    }

    ErrorTipDialog tip(tr("导出成功！"), TipMode::ONLY_OK, nullptr);
    tip.exec();
}

void TeachManage::on_btn_Import_clicked()
{

    if (!UsbDisk::instance()->isInserted())
        {
            ErrorTipDialog tip(tr("请插入U盘！"));
            tip.exec();
            return;
        }
    fileDialog->setDirectory(UsbDisk::instance()->getUsbMountPoint());

    // 打开文件对话框并获取选择的文件路径
    if (fileDialog->exec() == QDialog::Rejected)
    {
        return; // 用户取消了文件选择
    }

    const QStringList selectedFiles = fileDialog->selectedFiles();
    if (selectedFiles.isEmpty())
    {
        ErrorTipDialog tip(tr("没有选择要导入的文件！"), TipMode::ONLY_OK);
        tip.exec();
        return;
    }

    QString filePath = selectedFiles.first(); // 获取第一个文件路径
    QFileInfo progFileInfo(filePath);
    QString programName = progFileInfo.baseName();
    QString progModifiedTime = progFileInfo.lastModified().toString("yyyy-MM-dd hh:mm:ss");
    g_SafeFileHandler->rotateBackups(filePath);
    // 检查是否存在相同名称的程序
    auto it = std::find_if(m_ProgramNameAndPath.begin(), m_ProgramNameAndPath.end(),
                           [&](const D_ProgramNameAndPathStruct& item) { return item.fileName == programName; });

    if (it != m_ProgramNameAndPath.end())
    {
        ErrorTipDialog tip(tr("文件已存在，请确认是否覆盖？"));
        if (tip.exec() != QDialog::Accepted)
            return;

        QDir dir(progFileInfo.dir());
        QString fileNamePattern = QString("%1.*").arg(programName);
        QFileInfoList fileList = dir.entryInfoList(QStringList() << fileNamePattern,
                                                   QDir::Files | QDir::NoDotAndDotDot);
        for (const auto& matchingFile : fileList)
        {
            UsbDisk::instance()->copy(matchingFile.absoluteFilePath(), m_ProgramPath + "/");
        }
        ::sync();

        // 更新现有程序信息
        it->filePath = m_ProgramPath + "/" + programName + SUFFIX_PROGRAM;
        it->filePermission = ::getProgramPermission(programName);
        it->changeTime = progModifiedTime;

        // 更新表格中的显示
        for (int i = 0; i < ui->tableTeachManage->rowCount(); ++i)
        {
            if (ui->tableTeachManage->item(i, 2)->text() == programName)
            {
                QLabel* labIcon = qobject_cast<QLabel*>(ui->tableTeachManage->cellWidget(i, 1));
                if (labIcon)
                {
                    setFilePermision(labIcon, 0);
                }
                ui->tableTeachManage->item(i, 3)->setText(progModifiedTime);
                break;
            }
        }
    }
    else
    {
        QDir dir(progFileInfo.dir());
        QString fileNamePattern = QString("%1.*").arg(programName);
        QFileInfoList fileList = dir.entryInfoList(QStringList() << fileNamePattern,
                                                   QDir::Files | QDir::NoDotAndDotDot);
        for (const auto& matchingFile : fileList)
        {
            UsbDisk::instance()->copy(matchingFile.absoluteFilePath(), m_ProgramPath + "/");
        }
        ::sync();

        // 添加新程序信息
        uint8_t permission = ::getProgramPermission(programName);
        D_ProgramNameAndPathStruct P_NamePathTemp = {
            programName,
            m_ProgramPath + "/" + programName + SUFFIX_PROGRAM,
            ++m_FileNameNum,
            permission,
            progModifiedTime
        };
        m_ProgramNameAndPath.append(P_NamePathTemp);
        SetProgramIndex();

        // 更新表格显示
        addNewRowToTable(programName, progModifiedTime, permission);
    }

    setProgramNameAndPath(m_ProgramNameAndPath);

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

void TeachManage::addNewRowToTable(const QString &programName, const QString &fileTime, int permission)
{
    ui->tableTeachManage->setUpdatesEnabled(false);

    int newRow = ui->tableTeachManage->rowCount();
    ui->tableTeachManage->insertRow(newRow);

    QCheckBox* chbox = new QCheckBox(ui->tableTeachManage);
    ui->tableTeachManage->setCellWidget(newRow, 0, chbox);

    QLabel* labIcon = new QLabel(ui->tableTeachManage);
    setFilePermision(labIcon, permission);
    QPixmap pix(authIconPath[permission]);
    labIcon->setPixmap(pix.scaled(ui->tableTeachManage->columnWidth(1), ui->tableTeachManage->rowHeight(newRow), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->tableTeachManage->setCellWidget(newRow, 1, labIcon);

    ui->tableTeachManage->setItem(newRow, 2, new QTableWidgetItem(programName));
    ui->tableTeachManage->setItem(newRow, 3, new QTableWidgetItem(fileTime));

    ui->tableTeachManage->setColumnHidden(0, true);
    ui->tableTeachManage->resizeRowsToContents();

    ui->tableTeachManage->setUpdatesEnabled(true);
}

void TeachManage::setFilePermision(QWidget *lab, const QVariant &permission)
{
    lab->setProperty("filePermission", permission);
}

int TeachManage::getFilePermission(QWidget *lab) const
{
    return lab->property("filePermission").toInt();
}

void TeachManage::updateFilePermission(int index)
{

    int curRow = ui->tableTeachManage->currentRow();
    if (curRow < 0)
        return;

    QLabel* labIcon = qobject_cast<QLabel*>(ui->tableTeachManage->cellWidget(curRow, 1));
    if (!labIcon)
        return;
    setFilePermision(labIcon, index);

    QPixmap pix(authIconPath[index]);
    labIcon->setPixmap(pix.scaled(ui->tableTeachManage->columnWidth(1), ui->tableTeachManage->rowHeight(curRow),
                                  Qt::KeepAspectRatio, Qt::SmoothTransformation));

    auto item = ui->tableTeachManage->item(curRow, 2);
    if (item)
    {
        QString progName = item->text();

        ::setProgramPermission(progName, index);

        for (auto &progInfo : m_ProgramNameAndPath)
        {
            if (progInfo.fileName == progName)
            {
                progInfo.filePermission = index;
                break;
            }
        }
        if (m_CurrentProgramNameAndPath.fileName == progName)
        {
            m_CurrentProgramNameAndPath.filePermission = index;
        }
    }
}

void TeachManage::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
    }
    QWidget::changeEvent(e);
}

void TeachManage::on_tableTeachManage_itemSelectionChanged()
{
    int curRow = ui->tableTeachManage->currentRow();
    if (curRow < 0)
        return;

    auto widget = ui->tableTeachManage->cellWidget(curRow, 1);
    if (widget)
    {
        filePermission = getFilePermission(ui->tableTeachManage->cellWidget(curRow, 1));
    }
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
    auto item  = ui->tableTeachManage->item(curRow, 2);
    if (item)
    {
        QString fileName = item->text();
        ui->lineEdit_OriginalFileName->setText(fileName);
    }
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

    std::vector<int> indicesToRemove;
        for (int i = 0; i < m_ProgramNameAndPath.count(); ++i) {
            QString filePath = QDir(m_ProgramPath).filePath(m_ProgramNameAndPath[i].fileName + SUFFIX_PROGRAM);
            if (!QFile::exists(filePath)) {
                indicesToRemove.push_back(i);
            }
        }

        // 反向迭代以安全地移除元素
        for (auto it = indicesToRemove.rbegin(); it != indicesToRemove.rend(); ++it) {
            m_ProgramNameAndPath.removeAt(*it);
        }

    // update all program file to table
    QDir dir(m_ProgramPath);
    QFileInfoList fileList = dir.entryInfoList(QStringList() << "*" + SUFFIX_PROGRAM, QDir::Files | QDir::NoDotAndDotDot);
    for (const auto& fileInfo : fileList)
    {
        const QString programName = fileInfo.baseName();
        for(int i = 0;i<m_ProgramNameAndPath.count();i++)
        {
            if (m_ProgramNameAndPath[i].fileName == programName)
            {
                addNewRowToTable(programName, m_ProgramNameAndPath[i].changeTime, m_ProgramNameAndPath[i].filePermission);
                m_FileNameNum++;
            }
        }
    }
}

void TeachManage::initWidgets()
{
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

    fileDialog = new QFileDialog(this);
    fileDialog->setFixedSize(800, 600);
    fileDialog->setViewMode(QFileDialog::Detail);
    fileDialog->setFileMode(QFileDialog::ExistingFile);
    fileDialog->setNameFilter(QString("Program files(*%1)").arg(SUFFIX_PROGRAM));
    fileDialog->setAcceptDrops(false);
    fileDialog->setOptions(QFileDialog::ReadOnly | QFileDialog::DontUseNativeDialog);

    for (auto& comboBox : fileDialog->findChildren<QComboBox*>())
    {
        comboBox->setFocusPolicy(Qt::NoFocus);
    }
}
