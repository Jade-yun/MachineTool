#ifndef VARTYPEDIALOG_H
#define VARTYPEDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <cmd.h>
#include "basewindow.h"
namespace Ui {
class VarTypeDialog;
}

class VarTypeDialog : public BaseWindow
{
    Q_OBJECT

public:
    explicit VarTypeDialog(QWidget *parent = nullptr);
    ~VarTypeDialog();

public slots:
    void saveVariableType();
    void Refresh_VarTypeDialog();
private slots:
    void setVariabText();
private:
    Ui::VarTypeDialog *ui;

    std::array<QComboBox*, VAR_TOTAL_NUM> coboxVarType;
};

#endif // VARTYPEDIALOG_H
