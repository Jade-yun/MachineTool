#include "SafeFileHandler.h"

SafeFileHandler::SafeFileHandler(QObject *parent)
    : QObject(parent)
{
    m_backupDir = "/opt/MachineTool/backups/";
    QDir backupDir(m_backupDir);
    if (!backupDir.exists()) {
        // 尝试创建目录（包含父目录）
        if (!backupDir.mkpath(".")) {  // mkpath(".")比mkpath(m_backupDir)更高效
            qDebug() << "无法创建备份目录:" << m_backupDir;
        } else {
            QDir FirstBackDir(sysfiledir);
            if(!FirstBackDir.exists())
            {
                qDebug() << QString("无初始备份文件夹%1:").arg(sysfiledir);
            }
            else
            {
                QStringList files = FirstBackDir.entryList(QDir::Files);
                foreach (const QString& fileName, files) {
                    rotateBackups(sysfiledir+fileName);
                }
                qDebug() << "第一次备份系统参数文件完成，备份目录：" << m_backupDir;
            }
            qDebug() << "成功创建备份目录:" << m_backupDir;
        }
    } else {
        qDebug() << "备份目录已存在:" << m_backupDir;
    }
}

//备份文件
void SafeFileHandler::rotateBackups(QString filePath) {
    const int maxBackups = 1;//方便后面备份多个文件时使用，暂时每次只备份一个文件
    m_filePath = filePath;
    QFileInfo fi(m_filePath);
    QString baseName = fi.baseName();
    QString ext = fi.completeSuffix();

    // 删除最旧的备份
    QString oldestBackup = QString("%1%2.bak%3.%4")
        .arg(m_backupDir, baseName).arg(maxBackups).arg(ext);
    QFile::remove(oldestBackup);

    // 重命名现有备份
    for (int i = maxBackups - 1; i >= 1; --i) {
        QString oldName = QString("%1%2.bak%3.%4")
            .arg(m_backupDir, baseName).arg(i).arg(ext);
        QString newName = QString("%1%2.bak%3.%4")
            .arg(m_backupDir, baseName).arg(i + 1).arg(ext);
        QFile::rename(oldName, newName);
    }

    // 创建新备份
    QString backupPath = QString("%1%2.bak1.%4")
        .arg(m_backupDir, baseName,ext);
    bool backState = QFile::copy(m_filePath, backupPath);
    if(backState)
    {
        qDebug() << QString("文件%1备份成功").arg(backupPath);
    }
    else
    {
        qDebug() << QString("文件%1备份失败").arg(backupPath);
    }

}
//尝试从最新备份恢复
bool SafeFileHandler::attemptRecovery(QString filePath) {
    // 尝试从最新备份恢复
    m_filePath = filePath;
    QFileInfo fi(m_filePath);
    QString backupPath = QString("%1%2.bak1.%3")
        .arg(m_backupDir, fi.baseName(), fi.completeSuffix());

    QFile file(backupPath);
    if (file.open(QIODevice::ReadOnly) && file.size()>0) {
        QFile::remove(m_filePath);
        QFile::copy(backupPath, m_filePath);
        qDebug() << "从备份恢复成功:" << backupPath;
        return true;
    }

    qDebug() << QString("文件%1的所有备份恢复尝试失败!").arg(m_filePath);
    return false;
}
//删除备份文件
bool SafeFileHandler::ClearBackups(QString filePath) {
    // 尝试从最新备份恢复
    QFileInfo fi(filePath);
    QString backupPath = QString("%1%2.bak1.%3")
        .arg(m_backupDir, fi.baseName(), fi.completeSuffix());

    bool isRemoved = QFile::remove(backupPath);
    if(!isRemoved)
    {
        qDebug() << QString("删除备份文件%1失败!").arg(backupPath);
        return false;
    }
    qDebug() << QString("删除备份文件%1成功!").arg(backupPath);
    return true;
}
