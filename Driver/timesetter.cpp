#include "timesetter.h"
#include <sys/ioctl.h>

TimeSetter *TimeSetter::instance()
{
    static TimeSetter _instance;
    return &_instance;
}

int TimeSetter::setTime(const tm &t) {
    struct tm ts = t;
    time_t tt = mktime(&ts);
//    printf("time:%ld\n", tt);
    struct timeval tv;
    tv.tv_sec = tt;
    tv.tv_usec = 0;

    if (0 != settimeofday(&tv, nullptr)) {
        printf("set time error:%s\n", strerror(errno));
        return errno;
    }

    return setRtcTime(&ts);
}

TimeSetter::TimeSetter()
{

}

TimeSetter::~TimeSetter()
{

}

int TimeSetter::setRtcTime(tm *time) {
    struct tm tm;
    if (!time) {
        return -EINVAL;
    }
    memcpy(&tm, time, sizeof(struct tm));

    int fd = 0;
    int counter = 10;
    do {
        fd = open("/dev/rtc0", O_WRONLY);
        if (fd < 0) {
            if (counter-- && errno == EBUSY) {
                usleep(2000);
                continue;
            } else {
                return fd;
            }
        }
    } while (0);

    int retval = ::ioctl(fd, RTC_SET_TIME, &tm);
    close(fd);
    if (retval != 0) {
        printf("set rtc time error:%s\n", strerror(errno));
    }
    return retval;
}
