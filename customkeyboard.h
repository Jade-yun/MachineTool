#ifndef CUSTOMKEYBOARD_H
#define CUSTOMKEYBOARD_H

#include "Keyboard.h"
#include "NumberKeyboard.h"

#include <QDialog>
#include <QLineEdit>
#include <QFontDatabase>


class KeyboardWindow : public QDialog
{
    Q_OBJECT
private:
    QLineEdit* textInput;
    AeaQt::Keyboard* keyboard;


public:
    KeyboardWindow(QWidget* parent = nullptr);
    ~KeyboardWindow();

private slots:
    void processInput();

signals:
    void sendData(const QString& data);
};

class NumberKeyboardWindow : public QDialog
{
    Q_OBJECT
private:
    QLineEdit* textInput;
    AeaQt::Keyboard* keyboard;


public:
    NumberKeyboardWindow(QWidget* parent = nullptr);
    ~NumberKeyboardWindow();

//    QVariant getValue() const;
};


#endif // CUSTOMKEYBOARD_H

