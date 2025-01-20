#ifndef IOPortDialog_H
#define IOPortDialog_H

#include <QDialog>
#include <QLineEdit>
#include "basewindow.h"
enum class IOPortMode : unsigned char {
    IN = 0,
    OUT = 1,
    SearchIN =2,
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
    explicit IOPortDialog(QWidget *parent, IOPortMode mode, bool TeachTrigger);
    ~IOPortDialog();

private:
    QString getMainBoardIn() const;
    QString getMainBoardOut() const;
    QString getSearchStopIN() const;
public:
    QString getIOOnlineIn() const;
    QString getIOOnlineOut() const;
    QString getIOOnlineSearchStopIn() const;
    unsigned int get_IOInEdit_InPort() const;
    unsigned int get_IOInEdit_OutPort() const;
    unsigned int getInputPort() const;
    unsigned int getOutputPort() const;
    unsigned int getSearchStopInPort() const;
    bool getSearchStopPortState() const;
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


class IOInEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit IOInEdit(QWidget *parent = nullptr);

    uint8_t getCurrentPort() const;
    void setCurrentPort(uint8_t portIndex);

protected:
    void mouseReleaseEvent(QMouseEvent *event) override;
private:
    uint8_t port;
};

class IOOutEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit IOOutEdit(QWidget *parent = nullptr);

    uint8_t getCurrentPort() const;
    void setCurrentPort(uint8_t portIndex);

protected:
    void mouseReleaseEvent(QMouseEvent *event) override;
private:
    uint8_t port;
};


class IOOnlineSearchStopEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit IOOnlineSearchStopEdit(QWidget *parent = nullptr);

    uint8_t getCurrentPort() const;
    uint8_t getPortSetState() const;//获取端口设置状态
    void setCurrentPort(uint8_t portIndex);

protected:
    void mouseReleaseEvent(QMouseEvent *event) override;
private:
    uint8_t port;
    bool portState=true;
};

#endif // IOPortDialog_H
