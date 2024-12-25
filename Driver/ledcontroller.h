#ifndef LEDCONTROLLER_H
#define LEDCONTROLLER_H

#include <fcntl.h>
#include <unistd.h>

class LEDController {
private:
    int fd;

private:
    LEDController();
    ~LEDController();
    LEDController(const LEDController&) = delete;
    LEDController& operator=(const LEDController&) = delete;

public:
    static LEDController* instance();
    bool updateLEDStatus(void* data);
};


#endif // LEDCONTROLLER_H
