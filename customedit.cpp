#include "customedit.h"

#include "customkeyboard.h"

NumberEdit::NumberEdit(QWidget *parent)
    : QLineEdit(parent)
{
    this->setDecimalPlaces(0);
    this->setInputRange(INT_MIN, INT_MAX);
}

void NumberEdit::setDecimalPlaces(int places)
{
    this->setProperty("decimalPlaces", places);
}

void NumberEdit::setInputRange(const QVariant& min, const QVariant& max)
{
    this->setProperty("minValue", min);
    this->setProperty("maxValue", max);
}

int NumberEdit::getDecimalPlaces() const
{
    return this->property("decimalPlaces").toInt();
}

QVariant NumberEdit::getMinValue() const
{
    return this->property("minValue");
}

QVariant NumberEdit::getMaxValue() const
{
    return this->property("maxValue");
}

QString NumberEdit::formatInput(const QString& inputText) const
{
    QString formattedText = inputText;
    int decimalPlaces = getDecimalPlaces();

    if (decimalPlaces > 0)
    {
        if (!formattedText.contains('.'))
        {
            formattedText += '.';
            formattedText += QString(decimalPlaces, '0');
        }
        else
        {
            int currentDecimalPlaces = formattedText.split('.')[1].length();
            if (currentDecimalPlaces < decimalPlaces)
            {
                formattedText += QString(decimalPlaces - currentDecimalPlaces, '0');
            }
        }
    }
    return formattedText;
}

void NumberEdit::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    NumKeyboard keyboard(this->parentWidget());
    keyboard.clearText();
    keyboard.setCurrentEditObj(this);
    keyboard.exec();
}
