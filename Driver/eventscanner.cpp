#include "eventscanner.h"
#include <QThread>
#include <unistd.h>
#include <fcntl.h>
#include <QDebug>
#include <QVector>

#define USE_EPOLL
//#define USE_POLL

EventScanner::EventScanner(const QVector<QString>& devicePaths, QObject* parent)
    : QObject(parent), m_devicePaths(devicePaths), m_isRunning(false) {}

EventScanner::~EventScanner()
{
    stop();
}

void EventScanner::start()
{
    if (!m_isRunning)
    {
        m_isRunning = true;
        m_thread = new QThread();
        connect(m_thread, &QThread::started, this, &EventScanner::run);
        moveToThread(m_thread);
        m_thread->start();
    }
}

void EventScanner::stop()
{
    if (m_isRunning)
    {
        m_isRunning = false;
        m_thread->quit();
        m_thread->wait();
        delete m_thread;
    }
}

#if defined(USE_POLL) && !defined(USE_EPOLL)
#include <sys/poll.h>

void EventScanner::run() {
    QVector<struct pollfd> pollfds;

    for (const auto& path : m_devicePaths) {
        int fd = open(path.toUtf8().constData(), O_RDONLY | O_NONBLOCK);
        if (fd > 0) {
            m_devices[fd] = path;
            struct pollfd pfd = {fd, POLLIN, 0}; // 监视可读事件
            pollfds.append(pfd);
        } else {
            qWarning() << "Failed to open device file:" << path;
            emit errorOccurred("Failed to open device file: " + path);
        }
    }

    struct input_event in_ev;
    while (m_isRunning) {
        // 使用 poll 等待事件发生
        int pollCount = poll(pollfds.data(), pollfds.size(), -1); // -1 表示无限期阻塞
        if (pollCount < 0) {
            emit errorOccurred("Poll error");
            break;
        }

        for (int i = 0; i < pollfds.size(); ++i) {
            if (pollfds[i].revents & POLLIN) { // 检查可读事件
                int fd = pollfds[i].fd;
                ssize_t bytesRead = read(fd, &in_ev, sizeof(input_event));
                if (bytesRead == sizeof(input_event)) {
                    parseEvent(in_ev, m_devices[fd]);
                } else if (bytesRead < 0) {
                    if (errno != EAGAIN && errno != EWOULDBLOCK) {
                        emit errorOccurred("Error reading from device: " + m_devices[fd]);
                    }
                }
            }
        }
    }

    for (int fd : m_devices.keys()) {
        close(fd); // 关闭每个设备的文件描述符
    }
}


#elif defined (USE_EPOLL)
#include <sys/epoll.h>
void EventScanner::run() {
    int epollFd = epoll_create1(0);
    if (epollFd < 0) {
        qWarning() << "Failed to create epoll instance";
        return;
    }

    for (const auto& path : m_devicePaths) {
        int fd = open(path.toUtf8().constData(), O_RDONLY | O_NONBLOCK);
        if (fd > 0) {
            m_devices[fd] = path;
            struct epoll_event event; // monitoring event
            event.events = EPOLLIN; // monitor readable event
            event.data.fd = fd;
            if (epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &event) < 0) {
                qWarning() << "Failed to add file descriptor to epoll";
            }
        } else {
            qWarning() << "Failed to open device file:" << path;
        }
    }

    std::vector<struct epoll_event> events(m_devices.size());

    struct input_event in_ev;
    while (m_isRunning) {
        int numEvents = epoll_wait(epollFd, events.data(), events.size(), -1); // -1 表示无限期阻塞
        if (numEvents < 0) {
            qWarning() << "Epoll wait error";
            emit errorOccurred("Epoll wait error");
        }
        for (int i = 0; i < numEvents; i++) {
            int fd = events[i].data.fd;
            ssize_t bytesRead = read(fd, &in_ev, sizeof(input_event));
            if (bytesRead == sizeof(input_event)) {
                parseEvent(in_ev, m_devices[fd]);
            } else if (bytesRead < 0) {
                if (errno != EAGAIN && errno != EWOULDBLOCK) {
                    qWarning() << "Error reading from device: " << m_devices[fd];
                    emit errorOccurred("Error reading from device: " + m_devices[fd]);
                }
            }
        }
    }

    for (int fd : m_devices.keys()) {
        close(fd);
    }
    close(epollFd);
}
#elif !defined(USE_POLL) && !defined(USE_EPOLL)
void EventScanner::run() {
    for (const auto& path : m_devicePaths)
    {
        int fd = open(path.toUtf8().constData(), O_RDONLY | O_NONBLOCK);
        if (fd > 0)
        {
            m_devices[fd] = path;
        } else
        {
            qWarning() << "Failed to open device file:" << path;
            emit errorOccurred("Failed to open device file: " + path);
        }
    }

    struct input_event in_ev;
    while (m_isRunning)
    {
        for (auto it = m_devices.begin(); it != m_devices.end(); it++)
        {
            int fd = it.key();
            const QString& devicePath = it.value();
            ssize_t bytesRead = read(fd, &in_ev, sizeof(input_event));
            if (bytesRead == sizeof(input_event))
            {
                parseEvent(in_ev, devicePath);
            }
            else if (bytesRead < 0)
            {
                // 处理读取错误，可能是没有数据
                if (errno != EAGAIN && errno != EWOULDBLOCK)
                {
                    emit errorOccurred("Error reading from device: " + devicePath);
                }
            }
        }
        QThread::msleep(10); // Slight delay to reduce CPU usage
    }

    for (const int fd : m_devices.keys())
    {
        close(fd);
    }
}
#endif
void EventScanner::parseEvent(struct input_event in_ev, const QString& deviceName)
{

    // 屏蔽用于分割的同步事件0x00 和 一些杂类0x04
    if (in_ev.type == 0 || in_ev.type == 0x04)
    {
        return;
    }
    if (deviceName == "/dev/input/event4" || deviceName == "/dev/input/event8")
    {
        //4表示首轮事件，9代表使能按键
        emit eventRotaryEncoder(in_ev.code, in_ev.value);
    }
//    else if (deviceName == "/dev/input/event8")
//    {
//        // 屏蔽不在该档位动作0， 只保留进入动作1
//        if (in_ev.value == 1)
//        {
//            emit eventTrimodeSwitch(in_ev.code, in_ev.value);
//        }
//    }

    else if (deviceName == "/dev/input/event1")
    {
        // 左侧按键只保留按下动作1
//        if (in_ev.value == 1)
//        {
//            emit eventLeftKey(in_ev.code, in_ev.value);
//        }
        if(in_ev.value == 2)
            return;
        emit eventLeftKey(in_ev.code, in_ev.value);
    }
    else if (deviceName == "/dev/input/event2")
    {
        // 右侧按键保留按下动作1和长按动作2， 屏蔽松开动作0
//        if (in_ev.value)
//        {
//            emit eventRightKey(in_ev.code, in_ev.value);
//        }
        if(in_ev.value == 2)
            return;
        emit eventRightKey(in_ev.code, in_ev.value);
//        qDebug() << "code: " << in_ev.code << ", value: " << in_ev.value;
    }
}
