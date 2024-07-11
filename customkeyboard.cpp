#include "customkeyboard.h"

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

KeyboardWindow::KeyboardWindow(QWidget* parent)
    : QDialog(parent)
{
    textInput = new QLineEdit(this);
    keyboard = new AeaQt::Keyboard(this);

    setWindowTitle(tr("键盘"));
    setFixedSize(850, 370);


    textInput->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
//    textInput->setStyleSheet(qss);
//    textInput->setGeometry(QRect(0, 5, 850, 60));
//    keyboard->setGeometry(QRect(0, 65, 850, 300));


    QVBoxLayout *v = new QVBoxLayout();
    v->addWidget(textInput, 1);
    v->addWidget(keyboard, 5);
    this->setLayout(v);

    connect(keyboard, &AeaQt::Keyboard::keyEnterPressed, this, &KeyboardWindow::processInput);

}

KeyboardWindow::~KeyboardWindow()
{
    delete keyboard;
    keyboard = nullptr;
    delete textInput;
    textInput = nullptr;
}

//QString KeyboardWindow::getText() const
//{
//    return textInput->text();
//}

void KeyboardWindow::processInput()
{
//    getText();
    emit sendData(textInput->text());
    qDebug() << "process Input" ;
}


NumberKeyboardWindow::NumberKeyboardWindow(QWidget *parent)
{
    textInput = new QLineEdit(this);
    keyboard = new AeaQt::Keyboard(this);

    setWindowTitle(tr("数字键盘"));
    setFixedSize(850, 370);


    textInput->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    QVBoxLayout *v = new QVBoxLayout;
    v->addWidget(textInput, 1);
    v->addWidget(keyboard, 5);
    this->setLayout(v);
}

NumberKeyboardWindow::~NumberKeyboardWindow()
{

}

//QVariant NumberKeyboardWindow::getValue() const
//{
//    return textInput->text();
//}
