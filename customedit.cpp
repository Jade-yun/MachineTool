#include "customedit.h"
#include "customkeyboard.h"
#include "mainwindow.h"

#include <QMessageBox>
#include <QTimer>

NumberEdit::NumberEdit(QWidget *parent)
    : QLineEdit(parent)
{
    this->setText("0");
    this->setDecimalPlaces(2);
    this->setInputRange(INT_MIN, INT_MAX);

    previousValue = this->text();

    connect(this, &NumberEdit::finishedInput, this, &NumberEdit::validateInput);
}

void NumberEdit::setDecimalPlaces(int places)
{
    this->setProperty("decimalPlaces", places);
    this->setText(formatInput(this->text()));
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

    int nonZeroIndex = 0;
    bool hasDecimalPoint = formattedText.contains('.');

    if (hasDecimalPoint) {
        int dotIndex = formattedText.indexOf('.');
        if (dotIndex == 1 && formattedText[0] == '0') {
            nonZeroIndex = 1;
        } else {
            while (nonZeroIndex < formattedText.length() &&
                   (formattedText[nonZeroIndex] == '0' || formattedText[nonZeroIndex] == '.')) {
                if (formattedText[nonZeroIndex] == '.')
                    break;
                nonZeroIndex++;
            }
        }
    } else {
        while (nonZeroIndex < formattedText.length() &&
               (formattedText[nonZeroIndex] == '0')) {
            nonZeroIndex++;
        }
    }

    if (nonZeroIndex > 0) {
        formattedText.remove(0, nonZeroIndex);
        if (formattedText.isEmpty() || formattedText[0] == '.') {
            formattedText.prepend('0');
        }
    }

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
    else if (decimalPlaces == 0)
    {
        if (formattedText.contains('.'))
        {
            int dotIndex = formattedText.indexOf('.');
            formattedText = formattedText.left(dotIndex);
        }
    }
    return formattedText;
}

void NumberEdit::validateInput()
{
    double value = this->text().toDouble();
//    double value = text.toDouble();

    QVariant minValue = getMinValue();
    QVariant maxValue = getMaxValue();

    if ((minValue.isValid() && value < minValue.toDouble()) ||
        (maxValue.isValid() && value > maxValue.toDouble()))
    {
        this->setText(previousValue);
        QTimer::singleShot(50, this, [this, minValue, maxValue](){
            emit showRangeError(QString("输入的值范围不能超过 %1 ~ %2, 请重新输入。")
                                .arg(minValue.toString())
                                .arg(maxValue.toString()));
        });
    }
}

void NumberEdit::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    previousValue = this->text();

    QWidget *topLevelWidget = this;
    while (topLevelWidget->parentWidget() != nullptr) {
        topLevelWidget = topLevelWidget->parentWidget();
    }
    MainWindow *mainWindow = qobject_cast<MainWindow *>(topLevelWidget);
    if (mainWindow) {
//        NumKeyboard *keyboard = mainWindow->getNumberKeyboard();
        NumKeyboard temp(this);
        NumKeyboard *keyboard = &temp;
        if (keyboard) {
            keyboard->clearText();
            keyboard->setCurrentEditObj(this);
            if (keyboard->exec() == QDialog::Accepted){

            }
//            emit finishedInput();
        }
    }
}
