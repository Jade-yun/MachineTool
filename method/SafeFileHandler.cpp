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
            qDebug() << "成功创建备份目录:" << m_backupDir;
        }
    } else {
        qDebug() << "备份目录已存在:" << m_backupDir;
    }
}

bool SafeFileHandler::writeData(QString filePath,const QByteArray &data) {
    // 步骤1：原子写入临时文件
    m_filePath = filePath;
    QString tmpPath = m_filePath + ".tmp";
    QSaveFile tmpFile(tmpPath);

    if (!tmpFile.open(QIODevice::WriteOnly)) {
        qDebug() << "无法打开临时文件:" << tmpFile.errorString();
        return false;
    }

    // 写入数据并计算校验和
    tmpFile.write(data);
    m_lastChecksum = QCryptographicHash::hash(data, QCryptographicHash::Sha256);

    // 强制数据落盘
    if (!tmpFile.flush() || !fsync(tmpFile.handle())) {
        qDebug() << "数据写入失败:" << tmpFile.errorString();
        tmpFile.cancelWriting();
        return false;
    }

    // 原子提交
    if (!tmpFile.commit()) {
        qDebug() << "文件提交失败:" << tmpFile.errorString();
        return false;
    }

    // 步骤2：创建版本化备份
    rotateBackups(m_filePath);

    // 步骤3：清理临时文件
    QFile::remove(tmpPath);
    return true;
}

QByteArray SafeFileHandler::readData(QString filePath,bool verify) {
    // 优先读取主文件
//    m_filePath = filePath;
//    QFile file(m_filePath);
//    if (!file.open(QIODevice::ReadOnly)) {
//        qDebug() << "无法打开文件:" << file.errorString();
//        return attemptRecovery(m_filePath);
//    }

//    QByteArray data = file.readAll();
//    file.close();

//    // 校验数据完整性
//    if (verify && QCryptographicHash::hash(data, QCryptographicHash::Sha256) != m_lastChecksum) {
//        qDebug() << "校验失败，尝试恢复...";
//        return attemptRecovery(m_filePath);
//    }

    return 0;
}

void SafeFileHandler::rotateBackups(QString filePath) {
    const int maxBackups = 2;
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
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    QString backupPath = QString("%1%2.bak1.%4")
        .arg(m_backupDir, baseName,ext);
    QFile::copy(m_filePath, backupPath);
}

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

    qDebug() << "所有备份恢复尝试失败!";
    return false;
}
