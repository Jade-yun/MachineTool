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

private slots:
    void on_btnOK_clicked();

    void on_btnCancel_clicked();

private:
    void changeEnable();


private:
    Ui::StackSetDialog *ui;
};

#endif // STACKSETDIALOG_H
