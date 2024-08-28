#ifndef IOPortDialog_H
#define IOPortDialog_H

#include <QDialog>
#include <QLineEdit>

enum class IOPortMode : unsigned char {
    IN = 0,
    OUT = 1
};

namespace Ui {
class IOPortDialog;
}

class IOPortDialog : public QDialog
{
    Q_OBJECT

public:
    explicit IOPortDialog(QWidget *parent, IOPortMode mode);
    ~IOPortDialog();

private:
    QString getMainBoardIn() const;
    QString getMainBoardOut() const;

public:
    QString getIOOnlineIn() const;
    QString getIOOnlineOut() const;

private:
    Ui::IOPortDialog *ui;
};

class IOOnlineInEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit IOOnlineInEdit(QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;
};

class IOOnlineOutEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit IOOnlineOutEdit(QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;
};


#endif // IOPortDialog_H
