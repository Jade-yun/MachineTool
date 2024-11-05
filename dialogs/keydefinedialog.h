#ifndef KEYDEFINEDIALOG_H
#define KEYDEFINEDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QString>
#include "basewindow.h"
#define USE_LINEEDIT 1

namespace Ui {
class KeyDefineDialog;
}

class KeyDefineDialog : public BaseWindow
{
    Q_OBJECT

public:
    explicit KeyDefineDialog(QWidget *parent = nullptr);
    ~KeyDefineDialog();

    struct KeyFunc{
        uint8_t keyType;
        uint8_t portNum;
        uint8_t funcStatus;
    };
private:
    Ui::KeyDefineDialog *ui;   
private:
    KeyFunc m_KeyFunc;
private:
    QString getValveOutStatusStr() const;
    QString getValveOutputStr() const;
    QString getReserveOutStatusStr() const;
    QString getMainBoardOutStr() const;
    QString getAxisMoveDirectStr() const;

    int getValveOutputStatus() const;
    int getValveOutputPort() const;
    int getReserveOutStatus() const;
    int getMainBoardOutPort() const;
public:
    QString getKeyDefineStr() const;
    KeyFunc getKeyFuncDefine();

};

class KeyEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit KeyEdit(QWidget *parent = nullptr);

protected:
    void mouseReleaseEvent(QMouseEvent *event) override;

signals:
    void saveKeyDef();

public:
    uint8_t getKeyType() const;
    uint8_t getPortNum() const;
    uint8_t getKeyFuncStatus() const;
    void setKeyFunc(uint8_t keyType, uint8_t portNum, uint8_t funcStatus);

private:
    uint8_t keyType;    // 按键功能类型，0输出 1输入 2轴
    int8_t portNum;    // 端口对应编号，输出类型时1-40，输入类型时1-60，轴类型时1-6
    uint8_t funcStatus;     // 功能状态，输出类型时0断 1通 2翻转 轴类型时0正向 1反向
};

#endif // KEYDEFINEDIALOG_H
