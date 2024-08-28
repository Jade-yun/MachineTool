#include "draggablebutton.h"

DraggableButton::DraggableButton(QWidget *parent)
    : QPushButton(parent), isDraggable(false)
{

}

void DraggableButton::setDraggable(bool draggable)
{
    isDraggable = draggable;
}


void DraggableButton::mousePressEvent(QMouseEvent *event)
{
    if (isDraggable) {
        startPos = event->pos();
    }
    QPushButton::mousePressEvent(event);
}

void DraggableButton::mouseMoveEvent(QMouseEvent *event)
{
//    if (isDraggable && (event->buttons() & Qt::LeftButton)) {
//        int distance = (event->pos() - startPos).manhattanLength();
//        if (distance >= QApplication::startDragDistance()) {
//            move(mapToParent(event->pos() - startPos));
//        }
//    }


    if (isDraggable && (event->buttons() & Qt::LeftButton))
    {
        QPoint newPos = mapToParent(event->pos() - startPos);
//        QWidget *parentWidget = parentWidget();

        // get geomety of button and it's parent widget
        QRect parentRect = this->parentWidget()->rect();
        QRect buttonRect = rect();

        // examine if new position exceed the border and adjust it
        if (newPos.x() < 0)
        {
            newPos.setX(0);
        }
        else if (newPos.x() + buttonRect.width() > parentRect.width())
        {
            newPos.setX(parentRect.width() - buttonRect.width());
        }

        if (newPos.y() < 0)
        {
            newPos.setY(0);
        }
        else if (newPos.y() + buttonRect.height() > parentRect.height())
        {
            newPos.setY(parentRect.height() - buttonRect.height());
        }

        this->move(newPos);
    }
    //
    QPushButton::mouseMoveEvent(event);
}
