#include "customkeyboard.h"
#include "customedit.h"

#include <QTextEdit>
#include <QLabel>

//using namespace AeaQt;

//static const QString qss = "                               \
//                            QLineEdit {                    \
//                                border-style: none;        \
//                                padding: 3px;              \
//                                border-radius: 5px;        \
//                                border: 1px solid #dce5ec; \
//                                font-size: 22px;           \
//                            }                              \
//                            ";

#if USE_INSTANCE
FullKeyboard* FullKeyboard::instance(QWidget* parent)
{
    if (_instance == nullptr)
    {
        QMutexLocker locker(&mutex);
        if (_instance == nullptr)
            _instance = new FullKeyboard(parent);
    }
    return _instance;

}
FullKeyboard* FullKeyboard::_instance = nullptr;
QMutex FullKeyboard::mutex;

#endif

void FullKeyboard::setCurrentEditObj(QObject *edit)
{
    if (editObj != edit)
    {
        editObj = edit;
    }
}

void FullKeyboard::clearText()
{
    textInput->clear();
}

void FullKeyboard::setText(const QString &text)
{
    textInput->setText(text);
}

FullKeyboard::FullKeyboard(QWidget* parent)
    : QDialog(parent)
{
    editObj = nullptr;
    keyboard = new AeaQt::Keyboard(this);
    textInput = new QLineEdit(keyboard);

//    setWindowTitle(tr("键盘"));
//    setFixedSize(850, 370);
    setFixedSize(850, 430);
    setAcceptDrops(true);
    this->setModal(true);
//    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);
    this->setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);

    textInput->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
//    textInput->setStyleSheet(qss);

    QVBoxLayout *v = new QVBoxLayout();
    v->addWidget(textInput, 1);
    v->addWidget(keyboard, 5);
    this->setLayout(v);
//    connect(keyboard, &AeaQt::Keyboard::keyEnterPressed, this, &FullKeyboard::onKeyEnterPressed);

}

FullKeyboard::~FullKeyboard()
{
//    delete keyboard;
//    keyboard = nullptr;
//    delete textInput;
//    textInput = nullptr;
}

//QString FullKeyboard::getText() const
//{
//    return textInput->text();
//}

void FullKeyboard::onKeyEnterPressed()
{
    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editObj);
    QTextEdit* textEdit = qobject_cast<QTextEdit*>(editObj);
    QLabel* label = qobject_cast<QLabel*>(editObj);
    if (lineEdit)
    {
        lineEdit->setText(textInput->text());
    }
    else if (textEdit)
    {
        textEdit->setText(textInput->text());
    }
    else if(label)
    {
        label->setText(textInput->text());
    }
    hide();
}

void FullKeyboard::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
    {
        // 保存键盘的内容到触发键盘控件
//        qDebug() << "FullKeyboard::keyPressEvent(QKeyEvent *event)";
        QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editObj);
        QTextEdit* textEdit = qobject_cast<QTextEdit*>(editObj);
        QLabel* label = qobject_cast<QLabel*>(editObj);
        if (lineEdit)
        {
            lineEdit->setText(textInput->text());
        }
        else if (textEdit)
        {
            textEdit->setText(textInput->text());
        }
        else if(label)
        {
            label->setText(textInput->text());
        }
        hide();
    }
    else
        QWidget::keyPressEvent(event);
}

NumKeyboard::NumKeyboard(QWidget *parent)
    : QDialog(parent)
{
    editObj = nullptr;
    textInput = new QLineEdit(this);
    keyboard = new AeaQt::NumberKeyboard(this);
    validator = nullptr;


    setFixedSize(450, 370);
//    this->setModal(false);
//    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);
//    this->setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    this->setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint | Qt::CustomizeWindowHint);

    textInput->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

//    QIntValidator valid = QIntValidator(INT_MIN, INT_MAX,this);
//    textInput->setValidator(&valid);

    QVBoxLayout *v = new QVBoxLayout;
    v->addWidget(textInput, 1);
    v->addWidget(keyboard, 5);
    this->setLayout(v);

}

NumKeyboard::~NumKeyboard()
{
    delete keyboard;
    keyboard = nullptr;
    delete textInput;
    textInput = nullptr;
    delete validator;
    validator = nullptr;
}

#if USE_INSTANCE
//NumKeyboard* NumKeyboard::instance(QWidget* parent)
//{
////    static NumKeyboard _instance(parent);
////    return &_instance;
//    if (_instance == nullptr)
//    {
//        QMutexLocker locker(&mutex);
//        if (_instance == nullptr)
//            _instance = new NumKeyboard(parent);
//    }
//    return _instance;
//}
//NumKeyboard* NumKeyboard::_instance = nullptr;
//QMutex NumKeyboard::mutex;
#endif


void NumKeyboard::setCurrentEditObj(QObject *edit)
{
    if (editObj != edit)
    {
        editObj = edit;
        delete validator;

        QVariant minValue = INT_MIN;
        QVariant maxValue = INT_MAX;
        NumberEdit* numberEdit = qobject_cast<NumberEdit*>(editObj);
        if (numberEdit)
        {

            int decimalPlaces = numberEdit->getDecimalPlaces();
            minValue = numberEdit->getMinValue();
            maxValue = numberEdit->getMaxValue();

            if (decimalPlaces == 0)
            {
                validator = new QIntValidator(minValue.toInt(), maxValue.toInt(), this);
            }
            else
            {
                validator = new QDoubleValidator(minValue.toDouble(), maxValue.toDouble(), decimalPlaces, this);
                static_cast<QDoubleValidator*>(validator)->setNotation(QDoubleValidator::StandardNotation);
            }

            textInput->setValidator(validator);
        }
    }
}

void NumKeyboard::clearText()
{
    textInput->clear();
}

void NumKeyboard::keyPressEvent(QKeyEvent *event)
{
    const int key = event->key();

    if (key == Qt::Key_Enter || key == Qt::Key_Return)
    {
        QString inputText = textInput->text();

        if (editObj->setProperty("text", inputText))
        {
            close();
        }
//        else
//        {
//            QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editObj);
//            QLabel* label = qobject_cast<QLabel*>(editObj);
//            if (lineEdit)
//            {
//                lineEdit->setText(inputText);
//                close();
//            }
//            else if (label)
//            {
//                label->setText(inputText);
//                close();
//            }
//        }
    }
    else if (key == Qt::Key_Escape)
    {
        this->close();
    }
    else if (key == Qt::Key_Clear)
    {
        this->clearText();
    }
    else
    {
        QWidget::keyPressEvent(event);
    }
}


