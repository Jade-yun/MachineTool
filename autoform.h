#ifndef AUTOFORM_H
#define AUTOFORM_H

#include <QWidget>

#include "stacksetdialog.h"
#include "customkeyboard.h"

namespace Ui {
class AutoForm;
}

class AutoForm : public QWidget
{
    Q_OBJECT

public:
    explicit AutoForm(QWidget *parent = nullptr);
    ~AutoForm();

private slots:

    void on_btnEdit_pressed();

    void on_btnAimedProduct_clicked();

    void on_btnEdit_toggled(bool checked);

private:
    void updateLabelState(int index);
private:
    Ui::AutoForm *ui;


private:
    StackSetDialog* stackSet;

//    NumKeyboard* keyboard;

    void callNumKeyBoard(QObject* obj);
};

#endif // AUTOFORM_H
