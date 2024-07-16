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

void KeyboardWindow::setCurrentEdit(QLineEdit *edit)
{
    if (edit)
    {
        editObj = edit;
    }
}

void KeyboardWindow::clearText()
{
    textInput->clear();
}

KeyboardWindow::KeyboardWindow(QWidget* parent)
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
    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);
//    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);


    textInput->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
//    textInput->setStyleSheet(qss);

    QVBoxLayout *v = new QVBoxLayout();
    v->addWidget(textInput, 1);
    v->addWidget(keyboard, 5);
    this->setLayout(v);

    connect(keyboard, &AeaQt::Keyboard::keyEnterPressed, this, &KeyboardWindow::onKeyEnterPressed);

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

void KeyboardWindow::onKeyEnterPressed()
{
    if (editObj)
    {
        editObj->setText(textInput->text());
    }
//    qDebug() << "editObj->text():" << editObj->text();
//    emit sendData(textInput->text());

    close();
}

NumberKeyboardWindow::NumberKeyboardWindow(QWidget *parent)
    : QDialog(parent)
{
    editObj = nullptr;
    textInput = new QLineEdit(this);
    keyboard = new AeaQt::Keyboard(this);

    setWindowTitle(tr("数字键盘"));
    setFixedSize(850, 370);
    this->setModal(true);


    textInput->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    QVBoxLayout *v = new QVBoxLayout;
    v->addWidget(textInput, 1);
    v->addWidget(keyboard, 5);
    this->setLayout(v);


    connect(keyboard, &AeaQt::Keyboard::keyEnterPressed, this, &NumberKeyboardWindow::onKeyEnterPressed);
}

NumberKeyboardWindow::~NumberKeyboardWindow()
{

}


void NumberKeyboardWindow::setCurrentEdit(QLineEdit *edit)
{
    if (edit)
    {
        editObj = edit;
    }
}

void NumberKeyboardWindow::clearText()
{
    textInput->clear();
}
void NumberKeyboardWindow::onKeyEnterPressed()
{
    if (editObj)
    {
        editObj->setText(textInput->text());
    }
}

//QVariant NumberKeyboardWindow::getValue() const
//{
//    return textInput->text();
//}
