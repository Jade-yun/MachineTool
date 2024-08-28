#ifndef SIGDEFINEDIALOG_H
#define SIGDEFINEDIALOG_H

#include <QDialog>
#include <QLineEdit>

namespace Ui {
class SigDefineDialog;
}

class SigDefineDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SigDefineDialog(QWidget *parent = nullptr);
    ~SigDefineDialog();

private:
    QString getMainBoardInSig() const;
    QString getMainBoardOutSig() const;
public:
    QString getSigDefine() const;

private:
    Ui::SigDefineDialog *ui;
};

class SigLEDEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit SigLEDEdit(QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;
};

#endif // SIGDEFINEDIALOG_H
