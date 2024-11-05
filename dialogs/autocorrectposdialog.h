#ifndef AUTOCORRECTPOSDIALOG_H
#define AUTOCORRECTPOSDIALOG_H

#include <QDialog>
#include "basewindow.h"
namespace Ui {
class AutoCorrectPosDialog;
}

class AutoCorrectPosDialog : public BaseWindow
{
    Q_OBJECT

public:
    explicit AutoCorrectPosDialog(QWidget *parent = nullptr);
    ~AutoCorrectPosDialog();

private:
    Ui::AutoCorrectPosDialog *ui;
};

#endif // AUTOCORRECTPOSDIALOG_H
