﻿#ifndef DRAGGABLEBUTTON_H
#define DRAGGABLEBUTTON_H

#include <QPushButton>
#include <QApplication>
#include <QMouseEvent>

class DraggableButton : public QPushButton
{
    Q_OBJECT
public:

    explicit DraggableButton(QWidget *parent = nullptr);
    void setDraggable(bool draggable);

protected:
    void mousePressEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;
signals:
    void dragged(DraggableButton *button);

private:
    QPoint startPos;
    bool isDraggable;
};

#endif // DRAGGABLEBUTTON_H