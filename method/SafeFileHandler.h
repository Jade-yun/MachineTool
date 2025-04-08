#ifndef SAFEFILEHANDLER_H
#define SAFEFILEHANDLER_H

#include <QObject>
#include <QFile>
#include <QSaveFile>
#include <QCryptographicHash>
#include <QFileInfo>
#include <QDateTime>
#include <QDir>
#include <QDebug>

class SafeFileHandler : public QObject {
    Q_OBJECT
public:
    explicit SafeFileHandler(QObject *parent = nullptr);

    void rotateBackups(QString filePath);//备份文件
    bool attemptRecovery(QString filePath);//从备份文件恢复
    bool ClearBackups(QString filePath);//删除备份文件
private:
    QString m_filePath;
    QString m_backupDir;
    QByteArray m_lastChecksum;
    QString sysfiledir = "/opt/MachineTool/configs/";//用来存储系统配置文件存储目录，系统第一次启动时自动备份一次所有文件


    static int fsync(int fd) {
        return fsync(fd);
    }
};

#endif // SAFEFILEHANDLER_H
