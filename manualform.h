#ifndef MANUALFORM_H
#define MANUALFORM_H

#include <QWidget>

#include "draggablebutton.h"

namespace Ui {
class ManualForm;
}

class ManualForm : public QWidget
{
    Q_OBJECT

public:
    explicit ManualForm(QWidget *parent = nullptr);
    ~ManualForm();

private slots:
    void on_btnNewButton_clicked();

    void on_btnDeleteButton_clicked();

    void on_checkBoxEditPosition_stateChanged(int arg1);

    void on_btnImportPicture_clicked();

private:
    void initButtons();

private:
//    std::array<DraggableButton*, 20> buttons;
    DraggableButton* selectedButton;
    bool draggableMode;
    QList<DraggableButton*> btns;

private:
    Ui::ManualForm *ui;
};

#endif // MANUALFORM_H
