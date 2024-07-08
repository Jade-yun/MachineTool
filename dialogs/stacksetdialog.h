#ifndef STACKSETDIALOG_H
#define STACKSETDIALOG_H

#include <QDialog>

namespace Ui {
class StackSetDialog;
}

class StackSetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StackSetDialog(QWidget *parent = nullptr);
    ~StackSetDialog();

private:
    void changeEnable();


private:
    Ui::StackSetDialog *ui;
};

#endif // STACKSETDIALOG_H
