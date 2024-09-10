#ifndef VARTYPEDIALOG_H
#define VARTYPEDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <cmd.h>

namespace Ui {
class VarTypeDialog;
}

class VarTypeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit VarTypeDialog(QWidget *parent = nullptr);
    ~VarTypeDialog();

public slots:
    void saveVariableType();

private:
    Ui::VarTypeDialog *ui;

    std::array<QComboBox*, VAR_TOTAL_NUM> coboxVarType;
};

#endif // VARTYPEDIALOG_H
