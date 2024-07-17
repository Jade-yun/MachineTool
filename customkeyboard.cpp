#include "customkeyboard.h"

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

void KeyboardWindow::setCurrentEditObj(QObject *edit)
{
    if (editObj != edit)
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
    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);


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
    close();
}

void KeyboardWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
    {
        // 保存键盘的内容到触发键盘控件
//        qDebug() << "KeyboardWindow::keyPressEvent(QKeyEvent *event)";

    }
}

NumberKeyboardWindow::NumberKeyboardWindow(QWidget *parent)
    : QDialog(parent)
{
    editObj = nullptr;
    textInput = new QLineEdit(this);
    keyboard = new AeaQt::NumberKeyboard(this);


    setFixedSize(450, 370);
    this->setModal(true);
    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);

    textInput->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    QVBoxLayout *v = new QVBoxLayout;
    v->addWidget(textInput, 1);
    v->addWidget(keyboard, 5);
    this->setLayout(v);


    connect(keyboard, &AeaQt::NumberKeyboard::keyEnterPressed, this, &NumberKeyboardWindow::onKeyEnterPressed);
}

NumberKeyboardWindow::~NumberKeyboardWindow()
{
    delete keyboard;
    keyboard = nullptr;
    delete textInput;
    textInput = nullptr;
}


void NumberKeyboardWindow::setCurrentEditObj(QObject *edit)
{
    if (editObj != edit)
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
    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editObj);
    QLabel* label = qobject_cast<QLabel*>(editObj);
    if (lineEdit)
    {
        lineEdit->setText(textInput->text());
    }
    else if(label)
    {
        label->setText(textInput->text());
    }
    close();
}

