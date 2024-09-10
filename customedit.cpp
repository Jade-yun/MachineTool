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
    QString formattedText = inputText.trimmed();
    int decimalPlaces = getDecimalPlaces();
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
