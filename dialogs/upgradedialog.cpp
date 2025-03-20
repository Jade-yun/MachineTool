#include "upgradedialog.h"
#include "ui_upgradedialog.h"
#include "mainwindow.h"
#include <QMessageBox>
#include <QStorageInfo>
#include <QInputDialog>
#include <QCryptographicHash>
#include <QProcess>
#include "program_save.h"
#include "iniconfig.h"

#include <unistd.h>

#define COPYS "-副本"
#define DECOLLATOR '/'
#define ARROWS ">"
#define DOT '.'
#define HINTMESSAGE(Test) \
    QMessageBox::information(&QWidget(), "", Test)


upgradedialog::upgradedialog(const uint8_t &upgradetype, QWidget *parent) :
    BaseWindow(parent),UpgradeType(upgradetype),
    ui(new Ui::upgradedialog)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose,true);
    ui->upgradeWidget->setCurrentWidget(ui->upgradepage1);
    connect(ui->upgradeno, &QPushButton::clicked,this,[=](){this->close();});
    connect(ui->cancel_button,&QPushButton::clicked,this,[=](){this->close();});
    connect(ui->ResSettNO,&QPushButton::clicked,this,[=](){this->close();});
    connect(ui->upgreadok,&QPushButton::clicked,this,&upgradedialog::upgreadok_handle);
    dlgErrorTip = new ErrorTipDialog(this);
    dlgErrorTip->move(this->geometry().center()-dlgErrorTip->rect().center());
    dlgErrorTip->close();
    upgradelabel_handle(UpgradeType);

    //新建和重命名按钮暂时隐藏,当前汉字键盘不知道怎么调用
    ui->new_button->hide();
    ui->Rename_button->hide();
}

upgradedialog::~upgradedialog()
{
    delete ui;
}

int upgradedialog::showTip(const QString &message,TipMode mode)
{
    dlgErrorTip->setMode(mode);
    dlgErrorTip->setMessage(message);
    return dlgErrorTip->exec();
}
void upgradedialog::upgreadok_handle()
{
    QRegularExpression devicePattern("^/dev/sd[a-z]+[0-9]*$");
    QString TargetPath = "";
    foreach (storage, QStorageInfo::mountedVolumes()) {
        if (storage.isValid() && storage.isReady() && !storage.isRoot()) {
            if (devicePattern.match(storage.device()).hasMatch()) {
                TargetPath = storage.rootPath()+ui->upgrade_SearchName->text();
                break;
            }
        }
    }
    bool SameNameFileFlag = false;//是否存在同名文件
    QString tempPath = "/opt/MachineTool/configs/";
    switch (sure_handle_type) {
    case HANDHELD:
    {
        if(!UpFilePath.isEmpty())
        {//如果存在默认升级文件
            Upgrade_Progress_send(5);
            QThread *UpdataThread = new QThread();
            connect(UpdataThread,&QThread::started,this,&upgradedialog::Updata_handle);
            this->hide();
            emit updataWidgetShowSignal();
            UpdataThread->start();
        }
        break;
    }
    case MAINBOARD:
    {
        break;
    }
    case IOBOARD:
    {
        break;
    }
    case SERVO:
    {
        break;
    }
    case SYSTEM_DATA_COPY:
    {
        int reply = showTip(tr("请确认: [手持器]---->[U盘]"));
        if(reply == QDialog::Accepted)
        {
            this->hide();
            emit updataWidgetShowSignal();
            Upgrade_Progress_send(0);
            QDir FirstDir(tempPath);
            if(!FirstDir.exists())
            {
                qDebug() << QString("文件夹%1不存在").arg(tempPath);
            }
            else
            {
                if(ui->teach_checkbox->isChecked())
                {//如果选择的是示教文件
                    copyPath(m_ProgramPath,TargetPath);
                }
                else if(ui->SysDate_checkbox->isChecked())
                {//系统文件
                    Upgrade_Progress_send(30);
                    QStringList files = FirstDir.entryList(QDir::Files);
                    QFileInfoList fileList = FirstDir.entryInfoList(QStringList() << "Ini_Para.ini",
                                                               QDir::Files | QDir::NoDotAndDotDot);
                    for (const QFileInfo& fileInfo : fileList)
                    {
                        UsbDisk::instance()->copyToUsb(fileInfo.absoluteFilePath(), TargetPath);
                    }
                    Upgrade_Progress_send(100);
                    ::sync();
                }
                else if(ui->Cust_checkbox->isChecked())
                {
                    Upgrade_Progress_send(50);
                    QStringList files = FirstDir.entryList(QDir::Files);
                    QFileInfoList fileList = FirstDir.entryInfoList(QStringList() << "NameDef_Customize_*.ini",
                                                               QDir::Files | QDir::NoDotAndDotDot);
                    for (const QFileInfo& fileInfo : fileList)
                    {
                        UsbDisk::instance()->copyToUsb(fileInfo.absoluteFilePath(), TargetPath);
                    }
                    Upgrade_Progress_send(100);
                    ::sync();
                }
                else if(ui->All_checkbox->isChecked())
                {//全部参数
                    copyPath(m_ProgramPath,TargetPath);
                    copyPath("/opt/MachineTool/configs",TargetPath);
                    ::sync();
                }
            }
            emit ImplementFinishSignal();
        }
        break;
    }
    case COPY_DATA_REST:
    {
        emit updataWidgetShowSignal();
        Upgrade_Progress_send(0);
        int reply = showTip(tr("请确认: [U盘]---->[手持器]"));

        if(reply == QDialog::Accepted)
        {
            if(hasDuplicateFileNames(TargetPath,m_ProgramPath))
            {//if存在同名文件
                if(ui->teach_checkbox->isChecked() || ui->All_checkbox->isChecked())
                {
                    SameNameFileFlag = true;
                    int reply = showTip(tr("教导中存在相同文件，请确认是否覆盖？确认(覆盖同名文件)；取消(不恢复同名文件)"));
                    if(reply == QDialog::Accepted)
                    {
                        Cover_SameNameFile = true;
                    }
                    else if(reply == QDialog::Rejected)
                    {
                        Cover_SameNameFile = false;
                    }
                }
            }
            this->hide();
            Upgrade_Progress_send(20);
            if(ui->teach_checkbox->isChecked())
            {//如果选择的是示教文件
                QDir ProgramDir(TargetPath);
                if(!ProgramDir.exists())
                {
                    qDebug() << QString("文件夹%1不存在").arg(TargetPath);
                }
                else
                {
                    QStringList filters = {"*.ZHpro","*.ref"};
                    const QFileInfoList files = ProgramDir.entryInfoList(filters,
                            QDir::Files | QDir::NoDotAndDotDot | QDir::Readable);
                    QString programName;
                    QString progModifiedTime;
                    uint8_t permission;
                    for (const auto& matchingFile : files)
                    {
                        programName = matchingFile.baseName();
                        progModifiedTime = matchingFile.lastModified().toString("yyyy-MM-dd hh:mm:ss");
                        if(SameNameFileFlag == false)
                        {//如果不存在同名文件
                            UsbDisk::instance()->copy(matchingFile.absoluteFilePath(), m_ProgramPath + "/");
                            if(matchingFile.suffix() == "ZHpro")
                            {
                                // 添加新程序信息
                                permission = ::getProgramPermission(programName);
                                D_ProgramNameAndPathStruct P_NamePathTemp = {
                                    programName,
                                    m_ProgramPath + "/" + programName + SUFFIX_PROGRAM,
                                    ++m_FileNameNum,
                                    permission,
                                    progModifiedTime
                                };
                                m_ProgramNameAndPath.append(P_NamePathTemp);
                                SetProgramIndex();
                                setProgramNameAndPath(m_ProgramNameAndPath);
                            }
                        }
                        else
                        {
                            if(Cover_SameNameFile == true)
                            {//如果存在同名文件，且覆盖
                                auto it = std::find_if(m_ProgramNameAndPath.begin(), m_ProgramNameAndPath.end(),
                                                       [&](const D_ProgramNameAndPathStruct& item) { return item.fileName == programName; });
                                UsbDisk::instance()->copy(matchingFile.absoluteFilePath(), m_ProgramPath + "/");
                                if (it == m_ProgramNameAndPath.end())
                                {//不是同名文件
                                    if(matchingFile.suffix() == "ZHpro")
                                    {
                                        // 添加新程序信息
                                        permission = ::getProgramPermission(programName);
                                        D_ProgramNameAndPathStruct P_NamePathTemp = {
                                            programName,
                                            m_ProgramPath + "/" + programName + SUFFIX_PROGRAM,
                                            ++m_FileNameNum,
                                            permission,
                                            progModifiedTime
                                        };
                                        m_ProgramNameAndPath.append(P_NamePathTemp);
                                        SetProgramIndex();
                                        setProgramNameAndPath(m_ProgramNameAndPath);
                                    }
                                }
                                else
                                {//覆盖同名文件
                                    // 更新现有程序信息
                                    if(matchingFile.suffix() == "ZHpro")
                                    {
                                        it->filePath = m_ProgramPath + "/" + programName + SUFFIX_PROGRAM;
                                        it->filePermission = ::getProgramPermission(programName);
                                        it->changeTime = progModifiedTime;
                                    }
                                }
                            }
                            else
                            {//如果不覆盖
                                auto it = std::find_if(m_ProgramNameAndPath.begin(), m_ProgramNameAndPath.end(),
                                                       [&](const D_ProgramNameAndPathStruct& item) { return item.fileName == programName; });
                                if (it == m_ProgramNameAndPath.end())
                                {//不是同名文件
                                    UsbDisk::instance()->copy(matchingFile.absoluteFilePath(), m_ProgramPath + "/");
                                    if(matchingFile.suffix() == "ZHpro")
                                    {
                                        // 添加新程序信息
                                        permission = ::getProgramPermission(programName);
                                        D_ProgramNameAndPathStruct P_NamePathTemp = {
                                            programName,
                                            m_ProgramPath + "/" + programName + SUFFIX_PROGRAM,
                                            ++m_FileNameNum,
                                            permission,
                                            progModifiedTime
                                        };
                                        m_ProgramNameAndPath.append(P_NamePathTemp);
                                        SetProgramIndex();
                                        setProgramNameAndPath(m_ProgramNameAndPath);
                                    }
                                }
                            }
                        }
                    }
                    Upgrade_Progress_send(100);
                    ::sync();
                }
            }
            else if(ui->SysDate_checkbox->isChecked())
            {//系统参数
                QDir ProgramDir(TargetPath);
                if(!ProgramDir.exists())
                {
                    qDebug() << QString("文件夹%1不存在").arg(TargetPath);
                    return;
                }
                Upgrade_Progress_send(30);
                QStringList files = ProgramDir.entryList(QDir::Files);
                QFileInfoList fileList = ProgramDir.entryInfoList(QStringList() << "Ini_Para.ini",
                                                           QDir::Files | QDir::NoDotAndDotDot);
                for (const QFileInfo& fileInfo : fileList)
                {
                    UsbDisk::instance()->copy(fileInfo.absoluteFilePath(), tempPath);
                }
                Upgrade_Progress_send(100);
                ::sync();
            }
            else if(ui->Cust_checkbox->isChecked())
            {//名称自定义
                QDir ProgramDir(TargetPath);
                if(!ProgramDir.exists())
                {
                    qDebug() << QString("文件夹%1不存在").arg(TargetPath);
                    return;
                }
                Upgrade_Progress_send(50);
                QStringList files = ProgramDir.entryList(QDir::Files);
                QFileInfoList fileList = ProgramDir.entryInfoList(QStringList() << "NameDef_Customize_*.ini",
                                                           QDir::Files | QDir::NoDotAndDotDot);
                for (const QFileInfo& fileInfo : fileList)
                {
                    UsbDisk::instance()->copy(fileInfo.absoluteFilePath(), tempPath);
                }
                Upgrade_Progress_send(100);
                ::sync();
            }
            else if(ui->All_checkbox->isChecked())
            {//所有参数
                Upgrade_Progress_send(20);
                QDir ProgramDir(TargetPath);
                if(!ProgramDir.exists())
                {
                    qDebug() << QString("文件夹%1不存在").arg(TargetPath);
                    return;
                }
                Upgrade_Progress_send(40);
                QStringList files = ProgramDir.entryList(QDir::Files);
                QFileInfoList fileList = ProgramDir.entryInfoList(QStringList() << "*.ini",
                                                           QDir::Files | QDir::NoDotAndDotDot);
                for (const QFileInfo& fileInfo : fileList)
                {
                    if(fileInfo.baseName() != "AlarmInfoData" || fileInfo.baseName() == "PowerOnReadOneProInfo" || fileInfo.baseName() == "ProgramNameAndPathInfo")
                    {
                        UsbDisk::instance()->copy(fileInfo.absoluteFilePath(), tempPath);
                    }
                }
                Upgrade_Progress_send(60);
                QStringList filters1 = {"*.ZHpro","*.ref"};
                const QFileInfoList files1 = ProgramDir.entryInfoList(filters1,
                        QDir::Files | QDir::NoDotAndDotDot | QDir::Readable);
                QString programName;
                QString progModifiedTime;
                uint8_t permission;
                for (const auto& matchingFile : files1)
                {
                    programName = matchingFile.baseName();
                    progModifiedTime = matchingFile.lastModified().toString("yyyy-MM-dd hh:mm:ss");
                    if(SameNameFileFlag == false)
                    {//如果不存在同名文件
                        UsbDisk::instance()->copy(matchingFile.absoluteFilePath(), m_ProgramPath + "/");
                        if(matchingFile.suffix() == "ZHpro")
                        {
                            // 添加新程序信息
                            permission = ::getProgramPermission(programName);
                            D_ProgramNameAndPathStruct P_NamePathTemp = {
                                programName,
                                m_ProgramPath + "/" + programName + SUFFIX_PROGRAM,
                                ++m_FileNameNum,
                                permission,
                                progModifiedTime
                            };
                            m_ProgramNameAndPath.append(P_NamePathTemp);
                            SetProgramIndex();
                            setProgramNameAndPath(m_ProgramNameAndPath);
                        }
                    }
                    else
                    {
                        if(Cover_SameNameFile == true)
                        {//如果存在同名文件，且覆盖
                            auto it = std::find_if(m_ProgramNameAndPath.begin(), m_ProgramNameAndPath.end(),
                                                   [&](const D_ProgramNameAndPathStruct& item) { return item.fileName == programName; });
                            UsbDisk::instance()->copy(matchingFile.absoluteFilePath(), m_ProgramPath + "/");
                            if (it == m_ProgramNameAndPath.end())
                            {//不是同名文件
                                if(matchingFile.suffix() == "ZHpro")
                                {
                                    // 添加新程序信息
                                    permission = ::getProgramPermission(programName);
                                    D_ProgramNameAndPathStruct P_NamePathTemp = {
                                        programName,
                                        m_ProgramPath + "/" + programName + SUFFIX_PROGRAM,
                                        ++m_FileNameNum,
                                        permission,
                                        progModifiedTime
                                    };
                                    m_ProgramNameAndPath.append(P_NamePathTemp);
                                    SetProgramIndex();
                                    setProgramNameAndPath(m_ProgramNameAndPath);
                                }
                            }
                            else
                            {//覆盖同名文件
                                // 更新现有程序信息
                                if(matchingFile.suffix() == "ZHpro")
                                {
                                    it->filePath = m_ProgramPath + "/" + programName + SUFFIX_PROGRAM;
                                    it->filePermission = ::getProgramPermission(programName);
                                    it->changeTime = progModifiedTime;
                                }
                            }
                        }
                        else
                        {//如果不覆盖
                            auto it = std::find_if(m_ProgramNameAndPath.begin(), m_ProgramNameAndPath.end(),
                                                   [&](const D_ProgramNameAndPathStruct& item) { return item.fileName == programName; });
                            if (it == m_ProgramNameAndPath.end())
                            {//不是同名文件
                                UsbDisk::instance()->copy(matchingFile.absoluteFilePath(), m_ProgramPath + "/");
                                if(matchingFile.suffix() == "ZHpro")
                                {
                                    // 添加新程序信息
                                    permission = ::getProgramPermission(programName);
                                    D_ProgramNameAndPathStruct P_NamePathTemp = {
                                        programName,
                                        m_ProgramPath + "/" + programName + SUFFIX_PROGRAM,
                                        ++m_FileNameNum,
                                        permission,
                                        progModifiedTime
                                    };
                                    m_ProgramNameAndPath.append(P_NamePathTemp);
                                    SetProgramIndex();
                                    setProgramNameAndPath(m_ProgramNameAndPath);
                                }
                            }
                        }
                    }
                }
                ::sync();
                Upgrade_Progress_send(100);
            }
            emit ImplementFinishSignal();
        }
        break;
    }
    default:
        break;
    }
}
void upgradedialog::upgradelabel_handle(uint8_t Up_type)
{
    QRegularExpression devicePattern("^/dev/sd[a-z]+[0-9]*$");
    QString TargetPath = "";
    foreach (storage, QStorageInfo::mountedVolumes()) {
        if (storage.isValid() && storage.isReady() && !storage.isRoot()) {
            if (devicePattern.match(storage.device()).hasMatch()) {
                TargetPath = storage.rootPath()+ui->upgrade_SearchName->text();
                break;
            }
        }
    }

    switch (Up_type) {
    case HANDHELD:
    {
        ui->upgradeWidget->setCurrentWidget(ui->upgradepage1);
        ui->upgrade_label->setText(tr("是否现在进行手持器升级？"));
        ui->Data_select->hide();
        sure_handle_type = HANDHELD;
        UpFilePath = findFileAndGetDirectory(storage.rootPath(),"MachineTool");
        ui->upgrade_SearchName->setText(UpFilePath);
        break;
    }
    case MAINBOARD:
    {
        ui->upgradeWidget->setCurrentWidget(ui->upgradepage1);
        ui->upgrade_label->setText(tr("是否现在进行主板升级？"));
        ui->Data_select->hide();
        sure_handle_type = MAINBOARD;
        break;
    }
    case IOBOARD:
    {
        ui->upgradeWidget->setCurrentWidget(ui->upgradepage1);
        ui->upgrade_label->setText(tr("当前系统无IO板"));
        ui->Data_select->hide();
        sure_handle_type = IOBOARD;
        break;
    }
    case SERVO:
    {
        ui->upgradeWidget->setCurrentWidget(ui->upgradepage1);
        ui->upgrade_label->setText(tr("当前系统不支持伺服升级"));
        ui->Data_select->hide();
        sure_handle_type = SERVO;
        break;
    }
    case SYSTEM_DATA_COPY:
    {
        ui->upgradeWidget->setCurrentWidget(ui->upgradepage1);
        ui->upgrade_label->setText(tr("是否备份设置文件到U盘？"));
        ui->Data_select->show();
        ui->All_checkbox->setChecked(true);//默认选中全部参数
        ui->upgrade_SearchName->setText("/HMI/");
        sure_handle_type = SYSTEM_DATA_COPY;
        break;
    }
    case COPY_DATA_REST:
    {
        ui->upgradeWidget->setCurrentWidget(ui->upgradepage1);
        ui->upgrade_label->setText(tr("是否从U盘恢复设置文件？"));
        ui->Data_select->show();
        ui->All_checkbox->setChecked(true);//默认选中全部参数
        ui->upgrade_SearchName->setText("/HMI/");
        sure_handle_type = COPY_DATA_REST;
        break;
    }
    case RESTSETTING:
    {
        ui->upgradeWidget->setCurrentWidget(ui->upgradepage3);
        ui->ressetting_label->setText(tr("输入恢复密码，按确认按键，将恢复出场设置，并重启系统!"));
        ui->Reserve_teach_checkbox->setChecked(true);
        ui->Reserve_Other_checkbox->setChecked(true);
        ui->ResSettInput_password->setDecimalPlaces(0);
        ui->ResSettInput_password->setText("");
        sure_handle_type = RESTSETTING;
        break;
    }
    default:
        break;
    }
}

void upgradedialog::on_Key_File_hand_select_clicked()
{//浏览按键处理
    ui->upgradeWidget->setCurrentWidget(ui->upgradepage2);
    FileSystemModel = new QFileSystemModel(this);
    ui->file_listView->setModel(FileSystemModel);
    ui->file_listView->setResizeMode(QListView::Fixed);
    ui->file_listView->setViewMode(QListView::ListMode);
    ui->file_listView->setSpacing(1);
    QRegularExpression devicePattern("^/dev/sd[a-z]+[0-9]*$");
    foreach (storage, QStorageInfo::mountedVolumes()) {
        if (storage.isValid() && storage.isReady() && !storage.isRoot()) {
            if (devicePattern.match(storage.device()).hasMatch()) {
                // 设置文件系统模型的根路径为 U 盘的挂载点
                FileSystemModel->setRootPath(storage.rootPath());
                ui->file_listView->setRootIndex(FileSystemModel->index(storage.rootPath()));
                break; // 假设只有一个 U 盘，找到后即退出循环
            }
        }
    }
    ui->current_select_file->setText(storage.rootPath());
}

void upgradedialog::on_file_listView_doubleClicked(const QModelIndex &index)
{//双击目录进入下一级目录中
    if (FileSystemModel->isDir(index))
    {
        ui->file_listView->setRootIndex(index);
        updatePath(index);
    }
}
//更新当前目录信息
void upgradedialog::updatePath(const QModelIndex &index)
{
    QString currentPath = FileSystemModel->filePath(index);
    ui->file_listView->setRootIndex(index);
    ui->current_select_file->setText(currentPath);
}
//返回上一级目录按钮
void upgradedialog::on_toolButton_clicked()
{
    QModelIndex currentIndex = ui->file_listView->rootIndex();
    QModelIndex parentIndex = FileSystemModel->parent(currentIndex);
    QString currentIndexStr = "/run/media/"+FileSystemModel->data(currentIndex).toString();
    if (parentIndex.isValid() && currentIndexStr != storage.rootPath())
    {//只允许查看U盘根目录往下的文件
        ui->file_listView->setRootIndex(parentIndex);
        updatePath(parentIndex);
    }
}

//仅升级文件复选框
void upgradedialog::on_checkBox_stateChanged(int arg1)
{
//    if(arg1 == 0)
//    {//未被选中
//        proxyModel->setFilterRegExp(""); // 清除过滤器
//    }
//    else if(arg1 == 2)
//    {//选中
//        proxyModel->setFilterRegExp("(?!.*\\..*$).*$|\\.\\.?$|\\.$");
//    }
//    ui->file_listView->setRootIndex(proxyModel->mapFromSource(FileSystemModel->index(FileSystemModel->rootPath())));
}
//复制文件或文件夹
void upgradedialog::copyFileOrFolder()
{
    QModelIndexList indexes = ui->file_listView->selectionModel()->selectedIndexes();
    listOfFilesToCopy.clear();
    for (QModelIndex index : indexes)
    {
        QString filePath = FileSystemModel->filePath(index);
        listOfFilesToCopy.append(filePath);
    }
    ui->file_listView->clearSelection();//清列表选中状态
}
//粘贴
void upgradedialog::pasteFileOrFolder()
{
    if (listOfFilesToCopy.size() > 0)
    {
        QString destDir = FileSystemModel->filePath(ui->file_listView->rootIndex());
        for (QString filePath : listOfFilesToCopy)
        {
            QFileInfo fileInfo(filePath);
            QString destPath = QString();                                                               // 目标全称
            QString fileName = fileInfo.fileName();                                                     // 文件名
            QString filedir = fileInfo.path();                                                          // 源路径
            QString destinationPath = QDir(destDir).filePath(fileName);                                 // 目标全称
            bool isCovered = false;                                                                     // 是否覆盖
            destPath = generateUniqueFileName(destDir, fileInfo.completeBaseName(), fileInfo.suffix()); // 目标拼接
            if (filedir != destDir)                                                                     // 当前路径相同时复制不做提示，直接加上副本
            {
                if (QFile::exists(destinationPath))
                {
                    QMessageBox::StandardButton btn = QMessageBox::question(this, tr("Confirm Overwrite"),
                                                                            tr("文件已存在，请确认是否覆盖？"),
                                                                            QMessageBox::Yes | QMessageBox::No);
                    if (btn == QMessageBox::Yes)
                    {
                        isCovered = true;
                        destPath = destinationPath;
                    }
                    else
                    {
                        isCovered = false;
                    }
                }
                else
                {
                    destPath = destinationPath;
                }
            }

            if (filedir == destDir && cutOperation) // 路径相同时剪切不做处理
            {
                continue;
            }

            if (fileInfo.isDir())
            {
                // 要复制目录内容，需要递归地复制所有文件和子目录
                copyPath(filePath, destPath);
                //暂时不支持复制文件夹
            }
            else
            {
                // 若路径不存在则创建不存在的文件,不然无法复制粘贴成功
                QDir path(destDir);
                if (path.exists())
                {
                    QFile file(filePath);
                    if (isCovered)
                    {
                        file.remove(destPath);
                    }
                    file.copy(filePath, destPath);
                }
                else
                {
                    path.mkdir(destDir);
                    QFile file(filePath);
                    file.copy(filePath, destPath);
                }
            }

            if (cutOperation) // 剪切后删除以前的内容
            {
                QFileInfo fileInfo(filePath);
                if (fileInfo.isDir())
                {
                    QDir dir(filePath);
                    if (dir.exists())
                    {
                        dir.removeRecursively();
                    }
                }
                else
                {
                    QFile file(filePath);
                    file.remove();
                }
            }
        }

        if (cutOperation)
        {
            listOfFilesToCopy.clear();
            cutOperation = false;
        }
        ui->file_listView->clearSelection();
    }

}
/*************************************************************************
**  函数名：  generateUniqueFileName(const QString &directory, const QString &baseName, const QString &extension)
**	输入参数：directory 目的文件路径 baseName 文件名 extension 文件后缀
**	输出参数：
**	函数功能：检测是否有同名
**  作者：    wukui
**  开发日期：2024/10/28
**************************************************************************/
QString upgradedialog::generateUniqueFileName(const QString &directory, const QString &baseName, const QString &extension)
{
    QFileInfo fileInfo;
    // 设置初始文件名
    QString uniqueName = baseName + DOT + extension;
    int counter = 1; // 设置文件名称后缀计数器
    do
    {
        fileInfo.setFile(directory + DECOLLATOR + uniqueName);
        if (fileInfo.exists())
        {
            // 如果文件已存在，生成带有计数器的新文件名
            uniqueName = QString("%1_%2.%3").arg(baseName).arg(counter++).arg(extension);
        }
        else
        {
            // 文件名是唯一的，可以使用
            break;
        }
    } while (true);

    return directory + DECOLLATOR + uniqueName;
}
//复制粘贴目录
void upgradedialog::copyPath(QString src, QString dst)
{
    QDir dir(src);
    if (!dir.exists())
        return;

    for (QString d : dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
    {
        QString dst_path = dst + DECOLLATOR + d;
        dir.mkpath(dst_path);
        copyPath(src + DECOLLATOR + d, dst_path);
    }
    Upgrade_Progress_send(60);
    for (QString f : dir.entryList(QDir::Files))
    {
        // 若路径不存在则创建不存在的文件,不然无法复制粘贴成功
        QDir path(dst);
        if (!path.exists())
        {
            path.mkdir(dst);
        }
        QFile file(dst + DECOLLATOR + f);
        if(file.exists())
        {
            if(UpgradeType == COPY_DATA_REST)
            {
                if(true == Cover_SameNameFile)
                {
                    file.remove();//如果存在同名文件,需要先删除后在拷贝
                }
                else
                {
                    continue;//如果数据还原不覆盖已有文件,则直接进行下一个文件拷贝
                }
            }
            else
            {
                file.remove();
            }
        }
        if(!QFile::copy(src + DECOLLATOR + f, dst + DECOLLATOR + f))
        {
            qDebug() << "复制文件" <<src + DECOLLATOR + f <<" 到 " << dst + DECOLLATOR + f <<"失败";
        }
    }
    Upgrade_Progress_send(100);
}
// 实现函数，检查两个文件夹中是否有同名文件
bool upgradedialog::hasDuplicateFileNames(const QString& dir1, const QString& dir2) {
    QDir dirObj1(dir1);
    QDir dirObj2(dir2);

    if (!dirObj1.exists() || !dirObj2.exists()) {
        qWarning() << "One or both directories do not exist.";
        return false;
    }

    // 使用QSet来存储第一个文件夹中的文件名，以便快速查找
    QSet<QString> fileNamesSet;

    // 获取第一个文件夹中的所有文件名
    QFileInfoList fileInfoList1 = dirObj1.entryInfoList(QDir::Files);
    for (const QFileInfo& fileInfo : fileInfoList1) {
        fileNamesSet.insert(fileInfo.fileName());
    }

    // 遍历第二个文件夹，检查是否有同名文件
    QFileInfoList fileInfoList2 = dirObj2.entryInfoList(QDir::Files);
    for (const QFileInfo& fileInfo : fileInfoList2) {
        if (fileNamesSet.contains(fileInfo.fileName())) {
            return true; // 找到同名文件
        }
    }

    return false; // 没有找到同名文件
}
//遍历U盘中是否有某个文件
QString upgradedialog::findFileAndGetDirectory(const QString &usbPath, const QString &fileName) {
    QDir dir(usbPath);

    if (!dir.exists()) {
        qWarning() << "USB directory does not exist:" << usbPath;
        return QString();
    }

    QStringList entries = dir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot, QDir::DirsFirst);

    for (const QString &entry : entries) {
        QFileInfo fileInfo(dir.absoluteFilePath(entry));

        if (fileInfo.isFile() && fileInfo.fileName() == fileName) {
            return fileInfo.absolutePath()+DECOLLATOR+fileName;
        } else if (fileInfo.isDir()) {
            QString result = findFileAndGetDirectory(fileInfo.absoluteFilePath(), fileName);
            if (!result.isEmpty()) {
                return result;
            }
        }
    }
    return QString();
}

void upgradedialog::on_cut_button_clicked()
{//剪切
    cutOperation = true;
    copyFileOrFolder();
}

void upgradedialog::on_copy_button_clicked()
{//复制
    cutOperation = false;
    copyFileOrFolder();
}

void upgradedialog::on_paste_button_clicked()
{//粘贴
    pasteFileOrFolder();
}

void upgradedialog::on_ok_button_clicked()
{//确定
    QModelIndex currentIndex = ui->file_listView->currentIndex();

    switch (sure_handle_type) {
    case HANDHELD:
    {
        if (currentIndex.isValid() && FileSystemModel->isDir(currentIndex))
        { // 检查索引是否有效并且当前选中是否为目录
            ui->file_listView->setRootIndex(currentIndex); // 设置文件列表视图的根索引为该目录
            updatePath(currentIndex);                      // 更新路径显示
        }
        else
        {
            UpFilePath = FileSystemModel->filePath(ui->file_listView->currentIndex());
            Upgrade_Progress_send(5);
            QThread *UpdataThread_hand = new QThread();
            connect(UpdataThread_hand,&QThread::started,this,&upgradedialog::Updata_handle);
            this->hide();
            emit updataWidgetShowSignal();
            UpdataThread_hand->start();
        }
        break;
    }
    case MAINBOARD:
    {
        if (currentIndex.isValid() && FileSystemModel->isDir(currentIndex))
        { // 检查索引是否有效并且当前选中是否为目录
            ui->file_listView->setRootIndex(currentIndex); // 设置文件列表视图的根索引为该目录
            updatePath(currentIndex);                      // 更新路径显示
        }
        else
        {
            QModelIndex currentIndex = ui->file_listView->currentIndex();
            if (currentIndex.isValid()) {
                QFileInfo fileInfo = FileSystemModel->fileInfo(currentIndex);
                QString fileName = fileInfo.fileName();

                // 定义正则表达式，匹配以"M_V"开头，后跟一个数字，再跟"_"，然后是另一个数字，最后以".bin"结尾的文件名
                QRegularExpression re("M_V\\d+_\\d+\\.bin");

                // 使用正则表达式进行匹配
                QRegularExpressionMatch match = re.match(fileName);

                if (match.hasMatch()) {
                    // 文件名符合"M_Vx_x.bin"的格式
                    UpFilePath = FileSystemModel->filePath(ui->file_listView->currentIndex());
                    Upgrade_Progress_send(0);
//                    QThread *UpdataThread_main = new QThread();
                    M_MainUpdate.Upgrade_Thread_Run_Flag = true;
                    UpgradeTimer = new QTimer(this);
                    UpgradeTimer->start(1);
                    timer1->restart();
                    connect(UpgradeTimer,&QTimer::timeout,this,&upgradedialog::Updata_mian);
//                    connect(UpdataThread_main,&QThread::started,this,&upgradedialog::Updata_mian);
                    this->hide();
                    emit updataWidgetShowSignal();
                    emit Upgrade_Main_Progress_RefreshSignal();
//                    UpdataThread_main->start();
                } else {
                    // 文件名不符合所需格式
                    MainWindow::pMainWindow->showErrorTip(tr("升级文件类型异常！请选择类型为M_VX_X.bin类型的文件进行升级"));
                }
            } else {
                // 没有选中任何文件
                MainWindow::pMainWindow->showErrorTip(tr("未选中升级文件！"));
            }
        }
        break;
    }
    case IOBOARD:
    {
        break;
    }
    case SERVO:
    {
        break;
    }
    default:
        break;
    }

}

void upgradedialog::on_new_button_clicked()
{//新建文件夹
//    QString newName;
//    if (newName.isEmpty())
//        return;
//    QString parentPath = FileSystemModel->filePath(ui->file_listView->rootIndex());
//    QDir dir;
//    if (!dir.exists(parentPath + DECOLLATOR + newName))
//    {
//        if (dir.mkdir(parentPath + DECOLLATOR + newName))
//        {
//        }
//        else
//        {
//            QMessageBox::information(this, tr("mkdir message"), tr("新建文件夹失败"));
//        }
//    }
//    else
//    {
//        QMessageBox::information(this, tr("mkdir message"), tr("新建文件夹失败"));
//    }
}
//重命名
void upgradedialog::on_Rename_button_clicked()
{

}
void upgradedialog::on_delete_button_clicked()
{//删除
    QModelIndexList indexes = ui->file_listView->selectionModel()->selectedIndexes();
    for (QModelIndex index : indexes)
    {
        if (FileSystemModel->fileInfo(index).isDir())
        {
            QString filePath = FileSystemModel->filePath(index);
            QDir dir(filePath);
            if (dir.exists())
            {
                dir.removeRecursively();
            }
        }
        else
        {
            FileSystemModel->remove(index);
        }
    }
    ui->file_listView->clearSelection();
}

// 手持器升级文件拷贝函数，适用于后台线程
void upgradedialog::Updata_handle() {
    QDir destdir(destFilePath);
    // 若路径不存在则创建不存在的文件,不然无法复制粘贴成功
    if (!destdir.exists())
    {
        destdir.mkdir(destFilePath);
    }
    QCoreApplication::processEvents();
    Upgrade_Progress_send(10);
    QFileInfo fileInfo(UpFilePath);
    QString fileName = fileInfo.fileName();
    if(fileName != "MachineTool")
    {
        MainWindow::pMainWindow->showErrorTip(tr("请选择文件名为MachineTool的升级文件进行升级"));
        this->close();
        return;
    }
    QFile destfile(destFilePath+fileName);
    if (destfile.exists())
    {
        destfile.remove();
    }
    Upgrade_Progress_send(20);
    if(QFile::copy(UpFilePath, destFilePath+fileName))
    {
        Upgrade_Progress_send(50);
        //进行程序校验
//        if(verifyHash(destFilePath+fileName,expectedHash))
        {//校验通过
            QString backupPash = "/opt/MachineTool/bin/MachineTool";
            QFile backfile(backupPash);
            if(backfile.exists())
            {
                QFile::copy(backupPash,backupPash+".bak");//备份当前可执行文件
                backfile.remove();
            }
            Upgrade_Progress_send(80);
            if(QFile::copy(destFilePath+fileName,backupPash))
            {//升级成功，重新启动程序
                g_Usart->m_serialPort->clear(QSerialPort::AllDirections);// 清空缓冲区
                // 2. 关闭串口连接
                if (g_Usart->m_serialPort->isOpen()) {
                    g_Usart->m_serialPort->close();
                }
                // 3. 断开所有相关信号槽
                disconnect(g_Usart->m_serialPort, nullptr, nullptr, nullptr);
                ::sync();
                Upgrade_Progress_send(100);
                system("reboot");
            }
            else
            {
                qDebug()<<"升级失败";
            }
        }
    }
}


// 控制器升级文件拷贝函数，适用于后台线程
void upgradedialog::Updata_mian()
{
    QTimer timer(this);
    static bool isTimerStart = false;
    connect(&timer,&QTimer::timeout,this,[](){
        M_MainUpdate.Again_SendDate_MaxNum++;
        g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_UPGRADE_DATE);
    });
//    while(M_MainUpdate.Upgrade_Thread_Run_Flag)
    {
        if(M_MainUpdate.Upgrade_Status == 0 && M_MainUpdate.SendDateReturn_State == false)
        {//升级状态未升级
            M_MainUpdate.Upgrade_command = 1;  //控制板开始升级
            M_MainUpdate.Upgrade_Communication_Mode = 1;//升级通信方式：串口升级
            M_MainUpdate.Upgrade_Ver_Code = (uint32_t)GetUpgradeFileLastFourBytes(UpFilePath).toUInt();//升级校验编码
            M_MainUpdate.Upgrade_all_Rate = GetUpgradeFileAllRate(UpFilePath);//升级总帧数
            M_MainUpdate.Upgrade_Start_Rate = 1;//开始升级的帧编号
            M_MainUpdate.Current_Upgrade_Rate = 1;//当前要升级i的帧编号
            g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_UPGRADE_CONTROL);
            isTimerStart = false;
//            QThread::msleep(50);
            M_MainUpdate.SendDateReturn_State = true;
        }
        if(M_MainUpdate.Upgrade_Status == 1 || M_MainUpdate.Upgrade_Status == 2 || M_MainUpdate.Upgrade_Status == 5)
        {//如果控制器升级状态是升级中，或者当前帧数据写入完成，下发数据帧
            if(M_MainUpdate.Upgrade_Frame_Rate<=M_MainUpdate.Upgrade_all_Rate && ((M_MainUpdate.Upgrade_Frame_Rate+1 != M_MainUpdate.Current_Upgrade_Rate) || M_MainUpdate.Upgrade_Frame_Rate == 0))
            {//如果已经升级的帧数小于总帧数，,并且已经升级的帧数不等于当前要升级的帧编号，则继续升级
                M_MainUpdate.Current_Upgrade_Rate = M_MainUpdate.Upgrade_Frame_Rate+1;
                GetUpgradeFileOneRateDate(UpFilePath,M_MainUpdate.Current_Upgrade_Rate);
                g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_UPGRADE_DATE);
                qDebug() << "Upgrade_Frame_Rate_time:" << timer1->elapsed() << "毫秒";
            }
        }
        else if(M_MainUpdate.Upgrade_Status == 6)
        {//如果升级状态为当前帧数据写入失败，重新下发这帧数据
//            GetUpgradeFileOneRateDate(UpFilePath,M_MainUpdate.Current_Upgrade_Rate);
            if(isTimerStart == false)
            {
                timer.start(50);
                isTimerStart = true;
            }
            if(M_MainUpdate.Again_SendDate_MaxNum > 3 && M_MainUpdate.Upgrade_command != 4)
            {//连续发送3次都写入失败，
                M_MainUpdate.Upgrade_command = 4;//主控板升级异常
                g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_UPGRADE_CONTROL);
                timer.stop();
            }
        }
        else if(M_MainUpdate.Upgrade_Status == 3 || M_MainUpdate.Upgrade_Status == 4)
        {

            if(M_MainUpdate.Upgrade_Status == 3)
            {
                M_MainUpdate.Upgrade_command = 2;  //控制板升级完成
                g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_UPGRADE_CONTROL);
                emit Upgrade_Main_State_signal(M_MainUpdate.Upgrade_Status);
            }
            else if(M_MainUpdate.Upgrade_Status == 4)
            {
                M_MainUpdate.Upgrade_command = 4;  //控制板升级异常
                g_Usart->ExtendSendManualOperationDeal(CMD_MAIN_MANUAL,CMD_SUN_MANUAL_UPGRADE_CONTROL);
                emit Upgrade_Main_State_signal(M_MainUpdate.Upgrade_Status);
            }
//            UpgradeTimer->stop();
        }
//        Upgrade_Progress_send((static_cast<double>(M_MainUpdate.Current_Upgrade_Rate)/static_cast<double>(M_MainUpdate.Upgrade_all_Rate))*100.0);//显示升级进度
    }
}

//获取升级文件最后四位值作为校验编码
QByteArray upgradedialog::GetUpgradeFileLastFourBytes(QString filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "无法打开文件:" << file.errorString();
        return 0;
    }

    // 获取文件大小
    qint64 fileSize = file.size();
    if (fileSize < 4) {
        qDebug() << "文件太小，无法读取最后四个字节";
        file.close();
        return 0;
    }

    // 将文件指针移动到文件末尾向前4个字节的位置
    if (!file.seek(fileSize - 4)) {
        qDebug() << "无法移动到文件末尾向前4个字节的位置:" << file.errorString();
        file.close();
        return 0;
    }

    // 读取最后四个字节
    QByteArray lastFourBytes = file.read(4);
    if (lastFourBytes.size() != 4) {
        qDebug() << "读取的字节数不正确";
        return 0;
    } else {
        // 输出最后四个字节（以十六进制显示）
        qDebug() << "最后四个字节是:" << lastFourBytes.toHex();
        return lastFourBytes;
    }

    // 关闭文件
    file.close();
}

//获取升级文件总帧数
uint32_t upgradedialog::GetUpgradeFileAllRate(QString filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "无法打开文件:" << file.errorString();
        return 0;
    }

    // 获取文件大小
    int fileSize = file.size();
    file.close();

    return (fileSize+63)/64;//向上取整，不足64字节的算一帧数据
}

//发送升级文件当前要发送帧数据,Rate_Index表示发送第几帧数据
void upgradedialog::GetUpgradeFileOneRateDate(QString filePath,uint32_t Rate_Index)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "文件打开失败:" << file.errorString();
        return;
    }
    // 将文件指针移动到指定偏移位置
    if (!file.seek((Rate_Index-1)*UPGRADE_ONE_FRAME_LENGTH)) {
        qDebug() << "文件指针移动失败:" << file.errorString();
        file.close();
        return;
    }
    memset(M_MainUpdate.Upgrade_Date,0XFF,UPGRADE_ONE_FRAME_LENGTH);//先全赋值为0xff
    M_MainUpdate.Current_Rate_Len = file.read(reinterpret_cast<char*>(M_MainUpdate.Upgrade_Date), UPGRADE_ONE_FRAME_LENGTH);
    file.close();
}


bool upgradedialog::verifyHash(const QString &filePath, const QByteArray &expectedHash) {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly)) {
            qWarning() << "Failed to open file for hash verification.";
            return false;
        }

        QByteArray fileData = file.readAll();
        QByteArray actualHash = QCryptographicHash::hash(fileData, QCryptographicHash::Sha256);

        return actualHash == expectedHash;
}

//发送升级进度信号函数
void upgradedialog::Upgrade_Progress_send(uint8_t progress)
{
    Upgrade_Progress = progress;
    emit UpProgressRefreshSignal();
}

//恢复出场设置弹窗确认按钮
void upgradedialog::on_ResSettOK_clicked()
{
    if(ui->ResSettInput_password->text().toUInt() == passwd[3])
    {//如果输入密码正确
        if(RESTSETTING == sure_handle_type)
        {
#if 0
            //恢复出场设置
            QFile file(m_defaultConfigFileNamePath);
            if(file.exists())
            {
                if(cpStateReturn(m_defaultConfigFileNamePath,m_configFileNamePath))
                {
                    if(cpStateReturn(m_defaultconfigPortXYNameIniPath,m_configPortXYNameIniPath))
                    {
                        if(cpStateReturn(m_defaultconfigPortSettingPath,m_configPortSettingPath))
                        {
                             system("reboot");
                        }
                    }
                }
            }
            else
            {
                showTip(tr("未发现恢复出场设置文件！"));
            }
#else
            QString defaultConfigDir = "/opt/MachineTool/default_configs";  // 默认配置文件目录
            QString targetDir = "/opt/MachineTool/configs";  // app运行配置文件目录
            QString programDir = "/opt/MachineTool/program/";  //教导程序存储目录
            QString ProgramNameAndPathInfoDir = "/opt/MachineTool/configs/ProgramNameAndPathInfo.ini";  //教导程序信息文件
            QString PowerOnReadOneProInfoDir = "/opt/MachineTool/configs/PowerOnReadOneProInfo.ini";  //教导程序断电保存文件
            QDir dir(defaultConfigDir);

            if (!dir.exists()) {
                qWarning() << "Failed to restore factory settings";
                return;
            }


//            auto res = cpStateReturn(m_defaultConfigFileNamePath, m_configFileNamePath);
//            res = res && cpStateReturn(m_defaultconfigPortSettingPath, m_configPortSettingPath);

            QString command = QString("cp -rf %1/* %2/").arg(defaultConfigDir, targetDir);
            int result = system(command.toStdString().c_str());

            if (result != 0) {
                qDebug() << "Failed to copy configuration files!";
            }
            if(!ui->Reserve_teach_checkbox->isChecked())
            {//保留教导程序未选中，清除所有程序
                QString command = QString("rm -r %1/* %2 %3").arg(programDir,ProgramNameAndPathInfoDir,PowerOnReadOneProInfoDir);
                int result = system(command.toStdString().c_str());

                if (result != 0) {
                    qDebug() << "Failed to copy configuration files!";
                }
            }

            if(!ui->Reserve_Other_checkbox->isChecked())
            {//保留每转距离等数据不选择时处理

            }
            ::sync();
            system("reboot");
#endif

        }
        emit ImplementFinishSignal();
    }
    else
    {
        ui->ResSettInput_password->setStyleSheet("QLineEdit { border: 2px solid red; }");

        ErrorTipDialog *tip = new ErrorTipDialog(tr("密码错误！!"), TipMode::ONLY_OK, nullptr);
        QTimer::singleShot(1000, tip, [tip]() {
            if (tip->isVisible()) {
                tip->accept();
            }
            tip->deleteLater();
        });
        tip->exec();
    }
}
