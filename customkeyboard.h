#ifndef CUSTOMKEYBOARD_H
#define CUSTOMKEYBOARD_H

#include "Keyboard.h"
#include "NumberKeyboard.h"

#include <QDialog>
#include <QLineEdit>
#include <QFontDatabase>
#include <QMutex>

#define USE_INSTANCE 1
class FullKeyboard : public QDialog
{
    Q_OBJECT
private:
    QLineEdit* textInput;
    AeaQt::Keyboard* keyboard;
    QObject* editObj;
#if USE_INSTANCE
    static FullKeyboard* _instance;
    static QMutex mutex;

public:
    static FullKeyboard* instance(QWidget* parent = nullptr);
private:
//    FullKeyboard(QWidget* parent = nullptr);
#else

#endif
public:
    FullKeyboard(QWidget* parent = nullptr);
public:
    void setCurrentEditObj(QObject* edit);
    void clearText();
    void setText(const QString& text);
    QString getInputText() const;

public:
    ~FullKeyboard();

signals:
    void enterPressed(const QString& data);

    // QWidget interface
protected:
//    void closeEvent(QCloseEvent *event) override;

    // QWidget interface
protected:
    void keyReleaseEvent(QKeyEvent *event) override;
};

class NumKeyboard : public QDialog
{
    Q_OBJECT
private:
    QLineEdit* textInput;
    AeaQt::NumberKeyboard* keyboard;
    QObject* editObj;
    QValidator* validator;
public:
    NumKeyboard(QWidget* parent = nullptr);
public:
    void setCurrentEditObj(QObject* edit);
    void clearText();

public:
    ~NumKeyboard();
protected:
    void keyPressEvent(QKeyEvent *event) override;
};


#endif // CUSTOMKEYBOARD_H

