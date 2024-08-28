#ifndef KEYDEFINEDIALOG_H
#define KEYDEFINEDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QString>

#define USE_LINEEDIT 1

namespace Ui {
class KeyDefineDialog;
}

class KeyDefineDialog : public QDialog
{
    Q_OBJECT

public:
    explicit KeyDefineDialog(QWidget *parent = nullptr);
    ~KeyDefineDialog();
private:
    QString getValveOutStatus() const;
    QString getValveOutput() const;
    QString getReserveOutStatus() const;
    QString getMainBoardOut() const;
    QString getAxisMoveDirect() const;
public:
    QString getKeyDefine() const;

private:
    Ui::KeyDefineDialog *ui;   
};

//class KeyEdit : public QLineEdit
//{
//    Q_OBJECT

//public:
//    explicit KeyEdit(QWidget *parent = nullptr);

//protected:
//    void mousePressEvent(QMouseEvent *event) override;
//};

#if USE_LINEEDIT

class KeyEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit KeyEdit(QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;

signals:
    void saveKeyDef();
};
#else
class KeyEdit : public QLabel
{
    Q_OBJECT

public:
    explicit KeyEdit(QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;
};
#endif

#endif // KEYDEFINEDIALOG_H
