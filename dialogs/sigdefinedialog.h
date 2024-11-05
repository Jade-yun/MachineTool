#ifndef SIGDEFINEDIALOG_H
#define SIGDEFINEDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include "basewindow.h"
namespace Ui {
class SigDefineDialog;
}

class SigDefineDialog : public BaseWindow
{
    Q_OBJECT

public:
    explicit SigDefineDialog(QWidget *parent = nullptr);
    ~SigDefineDialog();

private:
    QString getMainBoardInSig() const;
    QString getMainBoardOutSig() const;
    int getInputPort() const;
    int getOutputPort() const;
public:
    QString getSigDefine() const;
    std::pair<uint8_t, uint8_t> getSigTypeAndPortDefine() const;


private:
    Ui::SigDefineDialog *ui;
};

class SigLEDEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit SigLEDEdit(QWidget *parent = nullptr);

protected:
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    uint8_t sigType; // 信号类型 0 输出 1 输入
    uint8_t port; // 功能编号，输出类型时1-40，输入类型时1-60

public:
    uint8_t getSigType() const;
    uint8_t getSigPort() const;
    void setSigTypeAndPort(uint8_t type, uint8_t portIndex);
    void setSigType(uint8_t type);
    void setSigPort(uint8_t portIndex);

};

#endif // SIGDEFINEDIALOG_H
