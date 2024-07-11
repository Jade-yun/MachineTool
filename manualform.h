#ifndef MANUALFORM_H
#define MANUALFORM_H

#include <QWidget>
#include <QTableWidget>

#include "draggablebutton.h"

struct ReferencePoint
{
    int index;
    QString name;
    DraggableButton* button;
};

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
    void on_btnNewButtonReference_clicked();

    void on_btnDeleteButton_clicked();
    void on_btnDeleteButtonReference_clicked();

    void on_checkBoxEditPos_stateChanged(int arg1);
    void on_checkBoxEditPosReference_stateChanged(int arg1);
    void on_btnImportPicture_clicked();
    void on_btnImportPictureReference_clicked();


private:
    void initVar();
    void addPointToTable(const QPushButton* button);
    void removePointFromTable();
    void tableReferenceSigAndSlot();

private:
    DraggableButton* selectedButton[2];
    bool draggable[2];
    QList<DraggableButton*> btns;
    QList<DraggableButton*> referenceBtns;

    QList<ReferencePoint> referencePoints;

    QTableWidget* tableReference;
    int buttonCount;

private:
    Ui::ManualForm *ui;
};

#endif // MANUALFORM_H
