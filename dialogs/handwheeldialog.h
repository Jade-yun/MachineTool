#ifndef HANDWHEELDIALOG_H
#define HANDWHEELDIALOG_H

#include <QDialog>

namespace Ui {
class HandWheelDialog;
}

class HandWheelDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HandWheelDialog(QWidget *parent = nullptr);
    ~HandWheelDialog();

private:
    Ui::HandWheelDialog *ui;
};

#endif // HANDWHEELDIALOG_H
