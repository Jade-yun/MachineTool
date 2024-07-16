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
    QLineEdit* editObj;
public:
    void setCurrentEdit(QLineEdit* edit);
    void clearText();

public:
    KeyboardWindow(QWidget* parent = nullptr);
    ~KeyboardWindow();

private slots:
    void onKeyEnterPressed();

signals:
    void sendData(const QString& data);

    // QWidget interface
protected:
//    void closeEvent(QCloseEvent *event) override;
};

class NumberKeyboardWindow : public QDialog
{
    Q_OBJECT
private:
    QLineEdit* textInput;
    AeaQt::Keyboard* keyboard;
    QLine* editObj;
public:
    void setCurrentEdit(QLineEdit* edit);
    void clearText();

public:
    NumberKeyboardWindow(QWidget* parent = nullptr);
    ~NumberKeyboardWindow();
private slots:
    void onKeyEnterPressed();
};


#endif // CUSTOMKEYBOARD_H

