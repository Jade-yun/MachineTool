#include "statebutton.h"

StateButton::StateButton(QWidget *parent)
    : QPushButton(parent),
      connected(true), objectText("")
{
    updateState();

    setStyleSheet("text-align: left; padding-left: 10px;");

    connect(this, &QPushButton::clicked, this, &StateButton::toggleConnection);

}

void StateButton::toggleConnection()
{
    connected = !connected;
    updateState();
}

void StateButton::setObjectText(const QString& text)
{
    objectText = text;
}

void StateButton::updateState()
{
    if (connected) {
        setIcon(QIcon(":/images/state_green.png"));
        setText(objectText + tr("通"));
    } else {
        setIcon(QIcon(":/images/state_red.png"));
        setText(objectText + tr("断"));
    }
    setIconSize(QSize(20, 20));
}
