#ifndef KEYDEFINEDIALOG_H
#define KEYDEFINEDIALOG_H

#include <QDialog>

namespace Ui {
class KeyDefineDialog;
}

class KeyDefineDialog : public QDialog
{
    Q_OBJECT

public:
    explicit KeyDefineDialog(QWidget *parent = nullptr);
    ~KeyDefineDialog();

private:
    Ui::KeyDefineDialog *ui;
};

#endif // KEYDEFINEDIALOG_H
