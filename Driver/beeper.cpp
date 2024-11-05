#include "beeper.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <QDebug>

Beeper::Beeper(QObject* parent)
    : QObject(parent), fd(-1), useBeeper(true)
{
    fd = open("/dev/buzzer", O_WRONLY);
    if (fd == -1) {
        qDebug() << "Failed to open the file.";
    }
}

Beeper::~Beeper() {
    if (fd != -1) {
        close(fd);
    }
}

int Beeper::beep(int durationMs) {

    int ret = ::ioctl(fd, useBeeper, &durationMs);
    if (ret == -1)
    {
//        qWarning() << "beep failed!";
        return -errno;
    }
    return 0;
}

Beeper* Beeper::instance()
{
    static Beeper beeper;
    return &beeper;
}

void Beeper::setEnable(bool isUse)
{
    useBeeper = isUse;
}
