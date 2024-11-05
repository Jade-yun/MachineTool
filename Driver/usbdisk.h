#ifndef USBDISK_H
#define USBDISK_H

#include <QObject>
#include <libudev.h>
#include <sys/types.h>
#include <dirent.h>

#include <QDebug>

class UsbDisk : public QObject
{
    Q_OBJECT
private:
    explicit UsbDisk(QObject *parent = nullptr);
    ~UsbDisk();

    UsbDisk(const UsbDisk&) = delete;
    UsbDisk& operator=(const UsbDisk&) = delete;

public:
    static UsbDisk* instance();

    bool isInserted() const;
    QByteArray readFile(const QString &fileName) ;
    bool writeFile(const QString &fileName, const QString &content) ;
    /**
     * @brief copyFromUsb is similar to the cp command in linux
     * @param file is the path including filename relative to the root of usb directory.
     * Usb root directory is also the mount point.
     * @param destPath is the target directory doesn't including filename
     * @return Returns ture if successful, otherwise returns false.
     */
    bool copyFromUsb(const QString &file, const QString &destPath) ;
    bool copyToUsb(const QString& file, const QString &destPath) ;
    /**
     * @brief copy
     * @param sourceFile is file absolute path without mount point.
     * @param destPath is current diretory absolute path without mount point.
     * @return
     */
    bool copy(const QString& sourceFile, const QString &destPath);

    QString getUsbMountPoint() const;

private:
    QString m_mountPoint;
};

#endif // USBDISK_H
