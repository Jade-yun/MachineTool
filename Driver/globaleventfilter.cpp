#include "globaleventfilter.h"

#include <QEvent>
#include <QDateTime>

#include <QDebug>
GlobalEventFilter::GlobalEventFilter(QObject *parent) : QObject(parent),
    mIsMouseReleased(true)
{

}

bool GlobalEventFilter::eventFilter(QObject *watched, QEvent *event)
{
    static qint64 lastBeepTime = 0;
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();

    if (event->type() == QEvent::MouseMove ||
        event->type() == QEvent::MouseButtonPress ||
        event->type() == QEvent::MouseButtonRelease)
    {

        if (event->type() == QEvent::MouseButtonPress)
        {
//            // 限制蜂鸣器的调用频率为至少300ms一次
//            if (currentTime - lastBeepTime > 200) {
//                Beeper::instance()->beep();
//                lastBeepTime = currentTime;
//                return true;
//            }
//            qDebug() << "mouse press";
            if (mIsMouseReleased)
            {
                mIsMouseReleased = false;
                if (BackLighter::instance()->lightOn())
                {
                    return true;
                }
                Beeper::instance()->beep();
            }
        }
        else if (event->type() == QEvent::MouseButtonRelease)
        {
//            qDebug() << "mouse release";
            mIsMouseReleased = true;
        }
        else {
//            qDebug() << "============ mouse move.....\n";
            if (BackLighter::instance()->lightOn())
            {
                return true;
            }
        }
        return QObject::eventFilter(watched, event);
    }
//    else if(event->type() == QEvent::KeyPress
//            || event->type() == QEvent::KeyRelease)
//    {
//        if(event->type() == QEvent::KeyPress)
//        {
//            if(BackLighter::instance()->lightOn())
//            {
//                return true;
//            }
//        }
////        qDebug() << "filter" << (event->type() == QEvent::KeyPress ? "key press": "key release");
//    }

    return QObject::eventFilter(watched, event);
}
