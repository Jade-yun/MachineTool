#ifndef IOPortDialog_H
#define IOPortDialog_H

#include <QDialog>
#include <QLineEdit>
#include "basewindow.h"
enum class IOPortMode : unsigned char {
    IN = 0,
    OUT = 1,

//    RESERVED_IN
//    RESERVED_OUT
};

namespace Ui {
class IOPortDialog;
}

class IOPortDialog : public BaseWindow
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
    unsigned int getInputPort() const;
    unsigned int getOutputPort() const;
public slots:
    void setchboxMainBoardInItem();
    void setchboxMainBoardOutItem();
private:
    Ui::IOPortDialog *ui;
};

class IOOnlineInEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit IOOnlineInEdit(QWidget *parent = nullptr);

    uint8_t getCurrentPort() const;
    void setCurrentPort(uint8_t portIndex);

protected:
    void mouseReleaseEvent(QMouseEvent *event) override;
private:
    uint8_t port;
};

class IOOnlineOutEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit IOOnlineOutEdit(QWidget *parent = nullptr);

    uint8_t getCurrentPort() const;
    void setCurrentPort(uint8_t portIndex);

protected:
    void mouseReleaseEvent(QMouseEvent *event) override;
private:
    uint8_t port;
};


#endif // IOPortDialog_H
