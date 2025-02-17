#include "ledcontroller.h"

#include <QDebug>

LEDController::LEDController()
    : fd(-1)
{
    fd = ::open("/dev/hc595", O_WRONLY | O_NONBLOCK);
    if (fd < 0) {
        qDebug() << "Cannot open /dev/hc595";
    }
}

LEDController::~LEDController()
{
    if (fd >= 0) {
        ::close(fd);
    }
}

LEDController *LEDController::instance()
{
    static LEDController _instance;
    return &_instance;
}

bool LEDController::updateLEDStatus(void* data)
{
    if(::write(fd, data, 1))
    {
        return true;
    }
    return false;
}
