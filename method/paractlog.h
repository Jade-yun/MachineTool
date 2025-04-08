#ifndef PARACTLOG_H
#define PARACTLOG_H

#include <QObject>

class ParActLog : public QObject
{
    Q_OBJECT
public:
    explicit ParActLog(QObject *parent = nullptr);

signals:

};

#endif // PARACTLOG_H
