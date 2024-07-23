#ifndef CLEARVARDIALOG_H
#define CLEARVARDIALOG_H

#include <QDialog>

namespace Ui {
class ClearVarDialog;
}

class ClearVarDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ClearVarDialog(QWidget *parent = nullptr);
    ~ClearVarDialog();

private:
    Ui::ClearVarDialog *ui;
};

#endif // CLEARVARDIALOG_H
