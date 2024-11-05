#ifndef EVENTSCANNER_H
#define EVENTSCANNER_H

#include <QThread>
#include <QFile>
#include <QVector>
#include <QMap>
#include <QObject>
#include <linux/input.h>

class EventScanner : public QObject {
    Q_OBJECT

public:
    explicit EventScanner(const QVector<QString>& devicePaths, QObject* parent = nullptr);
    ~EventScanner();
    void start();
    void stop();

signals:
    void eventRotaryEncoder(int code, int value);
    void eventTrimodeSwitch(int code, int value);
    void eventLeftKey(int code, int value);
    void eventRightKey(int code, int value);
    void errorOccurred(const QString &errorString); // 错误信号

private:
    const QVector<QString> m_devicePaths;
    QThread* m_thread;
    bool m_isRunning;
    QMap<int, QString> m_devices;

    void run();
    void parseEvent(struct input_event in_ev, const QString& deviceName);
};

#endif // EVENTSCANNER_H
