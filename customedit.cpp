#include "customedit.h"
#include "customkeyboard.h"
#include "mainwindow.h"

#include <QMessageBox>
#include <QTimer>
#include <QRegularExpression>

#include <limits>

NumberEdit::NumberEdit(QWidget *parent)
    : QLineEdit(parent),
      minValue(std::numeric_limits<double>::lowest()),
      maxValue(std::numeric_limits<double>::max()),
      value(0.0),
      previousValue(0.0),
      decimalPlaces(2)
{
    this->setText("0");
    connect(this, &NumberEdit::showRangeError, [=](const QString& message){
        ErrorTipDialog tip(message, TipMode::ONLY_OK);
        tip.exec();
    });
}


void NumberEdit::setDecimalPlaces(int places)
{
    decimalPlaces = qMax(0, places);
    if (!this->text().isEmpty())
        this->setText(formatInput(this->text()));
}

void NumberEdit::setInputRange(double min, double max)
{
    minValue = min;
    maxValue = max;
}

void NumberEdit::setValue(double newValue)
{
    value = newValue;
    this->setText(QString::number(value, 'f', decimalPlaces));
}

int NumberEdit::getDecimalPlaces() const
{
    return decimalPlaces;
}

double NumberEdit::getMinValue() const
{
    return minValue;
}

double NumberEdit::getMaxValue() const
{
    return maxValue;
}

double NumberEdit::getValue() const
{
    return value;
}

QString NumberEdit::formatInput(const QString& inputText) const
{
    QString formattedText = inputText.trimmed();
//    int decimalPlaces = getDecimalPlaces();
    bool isNegative = formattedText.startsWith('-');
    bool hasDecimalPoint = formattedText.contains('.');

    // 去掉负号，稍后再加回来
    if (isNegative) {
        formattedText = formattedText.mid(1);
    }

    // 处理前导零和小数点
    if (hasDecimalPoint) {
        int dotIndex = formattedText.indexOf('.');

        // 确保以小数点开头的数（如 .85）正确处理为 0.85
        if (dotIndex == 0) {
            formattedText.prepend('0');
        }

        // 移除前导零，但保留单个'0'在小数点前
        int nonZeroIndex = 0;
        while (nonZeroIndex < dotIndex && formattedText[nonZeroIndex] == '0') {
            nonZeroIndex++;
        }
        formattedText.remove(0, nonZeroIndex);

        // 如果小数点前面没有数字，补充'0'
        if (formattedText[0] == '.') {
            formattedText.prepend('0');
        }
    } else {
        // 对于没有小数点的数，移除所有前导零
        formattedText = formattedText.trimmed().remove(QRegularExpression("^0+"));
        if (formattedText.isEmpty()) {
            formattedText = "0";
        }
    }

    // 处理小数位数
    if (decimalPlaces > 0) {
        if (!formattedText.contains('.')) {
            formattedText += '.' + QString(decimalPlaces, '0');
        } else {
            int currentDecimalPlaces = formattedText.length() - formattedText.indexOf('.') - 1;
            if (currentDecimalPlaces < decimalPlaces) {
                formattedText += QString(decimalPlaces - currentDecimalPlaces, '0');
            }
            else if (currentDecimalPlaces > decimalPlaces) {
                formattedText = formattedText.left(formattedText.indexOf('.') + decimalPlaces + 1);
            }
        }
    } else if (decimalPlaces == 0 && hasDecimalPoint) {
        formattedText = formattedText.left(formattedText.indexOf('.'));
    }

    // 检查格式化后的文本是否为零
    bool isZero = (formattedText == "0" || formattedText == QString("0.").append(QString(decimalPlaces, '0')));

    // 如果原始输入是负数，且结果不是零值，添加负号
    if (isNegative && !isZero) {
        formattedText.prepend('-');
    }

    return formattedText;
}

void NumberEdit::validateInput()
{
    if (value < minValue || value > maxValue) {
        QTimer::singleShot(200, [=](){
            emit showRangeError(QString("输入的值范围不能超过 %1 ~ %2，请重新输入。")
                                .arg(minValue).arg(maxValue));
        });
        value = previousValue;  // 还原值
    }

    this->setText(formatInput(QString::number(value, 'f', decimalPlaces)));
}

void NumberEdit::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    previousValue = this->text().toDouble();
    NumKeyboard temp;
    NumKeyboard *keyboard = &temp;
    if (keyboard) {
        keyboard->clearText();
        keyboard->setCurrentEditObj(this);
        if (keyboard->exec() == QDialog::Accepted){
            validateInput();
            emit returnPressed();
            emit finishedInput();
        }
    }
}
