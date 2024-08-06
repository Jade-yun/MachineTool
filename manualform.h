#ifndef MANUALFORM_H
#define MANUALFORM_H

#include <QWidget>
#include <QTableWidget>
#include <QSettings>

#include "draggablebutton.h"

#define SAVEPOINT_VERSION_1
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
    void addPointsToTable();
    void tableReferenceSigAndSlot();

    int getIndex(const DraggableButton* button) const;

public:
    void saveState(QWidget *parent, const QString &settingsFile);
    void restoreState(QWidget *parent, const QString &settingsFile);
    void saveStateHelper(QSettings &settings, QWidget *widget);
    void restoreStateHelper(QSettings &settings, QWidget *parent, const QString &group);
private:
    DraggableButton* selectedButton[2];
    bool draggable[2];
    QList<DraggableButton*> guidBtns;

    QList<ReferencePoint> referencePoints;
    int currentIndex;

    QTableWidget* tableReference;


private:
    Ui::ManualForm *ui;
};

#endif // MANUALFORM_H
