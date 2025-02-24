#include "usbdisk.h"

#include <QDir>
#include <QRegularExpression>
#include <QStorageInfo>
#include <QProcess>
#include <QDirIterator>

UsbDisk::UsbDisk(QObject *parent) : QObject(parent)
{
    m_mountPoint = "/run/media/sda1";
}

UsbDisk::~UsbDisk()
{

}

UsbDisk *UsbDisk::instance()
{
    static UsbDisk _instance;
    return &_instance;
}

bool UsbDisk::isInserted() const
{
    return opendir("/sys/bus/usb/drivers/usb-storage/2-1:1.0") != nullptr;
}

QByteArray UsbDisk::readFile(const QString &fileName)
{
    m_mountPoint = getUsbMountPoint();

    QString filePath = QDir(m_mountPoint).filePath(fileName);
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open file for reading:" << filePath;
        return QByteArray();  // 返回空的 QByteArray 表示读取失败
    }

//    QByteArray data;
//    while (!file.atEnd()) {
//        data.append(file.read(1024));  // 每次读取1KB，减少多次拷贝
//    }

    QByteArray data = file.readAll();

    file.close();
    return data;  // 返回读取的字节数组
}

bool UsbDisk::writeFile(const QString &fileName, const QString &content)
{
    m_mountPoint = getUsbMountPoint();

    QString filePath = QDir(m_mountPoint).filePath(fileName);
    QFile file(filePath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        qDebug() << "Failed to open file for writing:" << filePath;
        return false;
    }

    QTextStream out(&file);
    out << content;
    file.close();
    return true;
}

bool UsbDisk::copyFromUsb(const QString &file, const QString &destPath)
{
    m_mountPoint = getUsbMountPoint();

    QString filePath = QDir(m_mountPoint).filePath(file);

    QDir destDir(destPath);
    if (!destDir.exists())
    {
        qWarning() << "Directory not exits:" << destPath;
        return false;
    }

    QStringList arguments;
    arguments << filePath << destPath;

    QProcess process;
    process.start("cp", arguments);

    if (!process.waitForFinished()) {
        qDebug() << "Failed to execute cp command:" << process.errorString();
        return false;
    }

    if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) {
        qDebug() << "cp command failed with exit code:" << process.exitCode();
        return false;
    }

    return true;
}

bool UsbDisk::copyToUsb(const QString &file, const QString &destPath)
{
    m_mountPoint = getUsbMountPoint();

    // 构建目标文件路径
    QString destFilePath = QDir(m_mountPoint).filePath(destPath);

    QString destDirPath = QFileInfo(destFilePath).absolutePath();
    QDir destDir;
    if (!destDir.exists(destDirPath) && !destDir.mkpath(destDirPath))
    {
        qDebug() << "Failed to create directory:" << destDirPath;
        return false;
    }

    // 构造 cp 命令
    QStringList arguments;
    arguments << file << destFilePath;

    QProcess process;
    process.start("cp", arguments);

    if (!process.waitForFinished()) {
        qDebug() << "Failed to execute cp command:" << process.errorString();
        return false;
    }

    // 检查退出代码
    if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) {
        qDebug() << "cp command failed with exit code:" << process.exitCode();
        return false;
    }

    return true;
}

bool UsbDisk::copy(const QString &sourceFile, const QString &destPath)
{
    QStringList arguments;
    arguments << sourceFile << destPath;

    QProcess process;
    process.start("cp", arguments);

    if (!process.waitForFinished()) {
        qDebug() << "Failed to execute cp command:" << process.errorString();
        return false;
    }

    // 检查退出代码
    if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) {
        qDebug() << "cp command failed with exit code:" << process.exitCode();
        return false;
    }

    return true;
}

QString UsbDisk::getUsbMountPoint() const
{
    // 定义设备匹配的正则表达式，例如匹配 "sda", "sdb" ... "sdz", 以及后续的数字分区
       QRegularExpression devicePattern("^/dev/sd[a-z]+[0-9]*$");

    // 遍历所有挂载的存储设备
    for (const QStorageInfo &storage : QStorageInfo::mountedVolumes()) {
        // 检查设备是否有效、已准备好、并且不是根分区
        if (storage.isValid() && storage.isReady() && !storage.isRoot()) {
            // 使用正则表达式匹配设备名
            if (devicePattern.match(storage.device()).hasMatch()) {
//                qDebug() << "Found USB device at:" << storage.rootPath();
                return storage.rootPath(); // 找到匹配的设备后返回其挂载点路径
            }
        }
    }

    // 如果未找到匹配的设备，则返回空字符串或其他指示未找到的标记
    qDebug() << "No USB device found.";
    return QString();
}

QString UsbDisk::findFile(const QString &fileName) const
{
    const QString rootPath = getUsbMountPoint();

    if (rootPath.isEmpty() || !QDir(rootPath).exists()) {
        qWarning() << "USB directory does not exist:" << rootPath;
        return QString();
    }

    QDirIterator it(rootPath, QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);

    while (it.hasNext()) {
        QString filePath = it.next();
        if (QFileInfo(filePath).fileName() == fileName) {
            return filePath;
        }
    }

    return QString();
}

//遍历U盘中是否有某个文件
QString UsbDisk::findFileAndGetDirectory(const QString &usbPath, const QString &fileName) {
    QDir dir(usbPath);

    if (!dir.exists()) {
        qWarning() << "USB directory does not exist:" << usbPath;
        return QString();
    }

    QStringList entries = dir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot, QDir::DirsFirst);

    for (const QString &entry : entries) {
        QFileInfo fileInfo(dir.absoluteFilePath(entry));

        if (fileInfo.isFile() && fileInfo.fileName() == fileName) {
            return fileInfo.absolutePath()+"/"+fileName;
        } else if (fileInfo.isDir()) {
            QString result = findFileAndGetDirectory(fileInfo.absoluteFilePath(), fileName);
            if (!result.isEmpty()) {
                return result;
            }
        }
    }
    return QString();
}
