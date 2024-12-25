#include "statebutton.h"

StateButton::StateButton(QWidget *parent)
    : QPushButton(parent),
      connected(true), objectText(tr("")),
      connectedText(tr("通")),disconnectedText(tr("断"))
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

void StateButton::setStateTexts(const QStringList &texts)
{
    connectedText = texts[0];
    disconnectedText = texts[1];
}

void StateButton::setStateTexts(const QString &connectedText, const QString &disconnectedText)
{
    this->connectedText = connectedText;
    this->disconnectedText = disconnectedText;
}

bool StateButton::getState() const
{
    return connected;
}

void StateButton::setState(bool state)
{
    connected = state;
    updateState();
}

void StateButton::updateState()
{
    if (connected) {
        setIcon(QIcon(":/images/state_green.png"));
        setText(objectText + connectedText);
    } else {
        setIcon(QIcon(":/images/state_red.png"));
        setText(objectText + disconnectedText);
    }
    setIconSize(QSize(20, 20));
}
