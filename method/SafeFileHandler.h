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

    bool writeData(QString m_filePath,const QByteArray &data);
    void rotateBackups(QString filePath);//备份文件
    bool attemptRecovery(QString filePath);//从备份文件恢复
    QByteArray readData(QString m_filePath,bool verify = true);
private:
    QString m_filePath;
    QString m_backupDir;
    QByteArray m_lastChecksum;



    static int fsync(int fd) {
        return fsync(fd);
    }
};

#endif // SAFEFILEHANDLER_H
