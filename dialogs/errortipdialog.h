#ifndef ERRORTIPDIALOG_H
#define ERRORTIPDIALOG_H

#include <QDialog>
#include "basewindow.h"
namespace Ui {
class ErrorTipDialog;
}

enum class TipMode : unsigned char {
    NORMAL = 0,
    ONLY_OK,
    NULL_BUTTON
//  other mode
};

class ErrorTipDialog : public BaseWindow
{
    Q_OBJECT

public:
    explicit ErrorTipDialog(QWidget *parent = nullptr);
    explicit ErrorTipDialog(const QString& message, TipMode mode, QWidget *parent = nullptr);
    explicit ErrorTipDialog(const QString& message, QWidget *parent = nullptr);
    ~ErrorTipDialog();

public:
    void setMessage(const QString& message);
    void setMode(TipMode mode);

private:
    Ui::ErrorTipDialog *ui;
};

#endif // ERRORTIPDIALOG_H
