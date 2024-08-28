#ifndef AUTOCORRECTPOSDIALOG_H
#define AUTOCORRECTPOSDIALOG_H

#include <QDialog>

namespace Ui {
class AutoCorrectPosDialog;
}

class AutoCorrectPosDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AutoCorrectPosDialog(QWidget *parent = nullptr);
    ~AutoCorrectPosDialog();

private:
    Ui::AutoCorrectPosDialog *ui;
};

#endif // AUTOCORRECTPOSDIALOG_H
