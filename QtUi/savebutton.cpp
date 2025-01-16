#include "savebutton.h"
#include <QIcon>

SaveButton::SaveButton(QWidget* parent)
    : QPushButton(parent), paraChanged(false) {
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &SaveButton::onTimeout);

    saveIcon = QIcon(":/images/save.png");

    QPixmap pixmap(25, 25);
    pixmap.fill(Qt::transparent);
    emptyIcon = QIcon(pixmap);

    setIcon(saveIcon);
//    setIconSize(QSize(25, 25));
}

void SaveButton::setParaChangedFlag(bool flag) {
    if (paraChanged == flag) {
        return;
    }

    paraChanged = flag;

    if (paraChanged) {
        startFlashing();
    } else {
        stopFlashing();
    }
}

bool SaveButton::isParaChanged() const
{
    return paraChanged;
}

void SaveButton::onTimeout() {
        setIcon(saveIcon);
        QTimer::singleShot(300, this, [this]() {
            if (timer->isActive()) {
                setIcon(emptyIcon);
            }
        });
}

void SaveButton::startFlashing() {
    QTimer::singleShot(300, this, [this]() {
        if (timer->isActive()) {
            setIcon(emptyIcon);
        }
    });
    timer->start(1000);
}

void SaveButton::stopFlashing() {
    timer->stop();
    setIcon(saveIcon);
}
