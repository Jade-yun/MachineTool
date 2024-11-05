#ifndef GLOBALEVENTFILTER_H
#define GLOBALEVENTFILTER_H

#include <QObject>

#include "beeper.h"
#include "backlighter.h"

class GlobalEventFilter : public QObject
{
    Q_OBJECT
public:
    explicit GlobalEventFilter(QObject *parent = nullptr);

signals:

private:
    bool mIsMouseReleased;


    // QObject interface
public:
    bool eventFilter(QObject *watched, QEvent *event);
};

#endif // GLOBALEVENTFILTER_H
