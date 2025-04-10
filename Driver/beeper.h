#ifndef BEEPER_H
#define BEEPER_H

#include <QObject>
#include <QTimer>

class Beeper : public QObject {
    Q_OBJECT

private:
    explicit Beeper(QObject* parent = nullptr);
    Beeper(const Beeper&) = delete; // 禁止拷贝构造
    Beeper& operator=(const Beeper&) = delete; // 禁止赋值运算
    ~Beeper();

public:
    static Beeper* instance();
    int beep(int durationMs = 100);
    void setEnable(bool isUse);

private:
    int fd;
    bool useBeeper;
};

#endif // BEEPER_H
