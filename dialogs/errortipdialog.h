#ifndef ERRORTIPDIALOG_H
#define ERRORTIPDIALOG_H

#include <QDialog>

namespace Ui {
class ErrorTipDialog;
}

class ErrorTipDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ErrorTipDialog(QWidget *parent = nullptr);
    ~ErrorTipDialog();

public:
    void setMessage(const QString& message);

private:
    Ui::ErrorTipDialog *ui;
};

#endif // ERRORTIPDIALOG_H
