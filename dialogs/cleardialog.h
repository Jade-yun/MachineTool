#ifndef CLEARDIALOG_H
#define CLEARDIALOG_H

#include <QDialog>

namespace Ui {
class ClearDialog;
}

class ClearDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ClearDialog(QWidget *parent = nullptr);
    ~ClearDialog();

private:
    Ui::ClearDialog *ui;
};

#endif // CLEARDIALOG_H
