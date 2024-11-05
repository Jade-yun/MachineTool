#ifndef TIMESETTER_H
#define TIMESETTER_H

#include <ctime>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <linux/rtc.h>
#include <cstdio>

class TimeSetter {

public:
    static TimeSetter* instance();
    int setTime(const tm &t);
private:
    TimeSetter();
    TimeSetter(const TimeSetter&) = delete;
    TimeSetter& operator=(const TimeSetter&) = delete;

    ~TimeSetter();

private:
    int setRtcTime(tm *time);
};

#endif // TIMESETTER_H
