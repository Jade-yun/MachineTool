#ifndef MANUALFORM_H
#define MANUALFORM_H

#include <QWidget>
#include <QTableWidget>
#include <QSettings>
#include <QHash>

#include "draggablebutton.h"

#define SAVEPOINT_VERSION_1

struct GuidePara {
    QString guideName;
    QString keyDefStr;
//    D_KeyFuncStruct keyFuncDef;
};

struct ReferPointPara
{
    int index; // 唯一标识按钮的索引
    QString name;
    DraggableButton* button; // 直接绑定按钮对象

   // add other parameter...
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

signals:
    void sigShowStackPage();

private slots:
    void on_btnNewButton_clicked();
    void on_btnDeleteButton_clicked();
    void on_checkBoxEditPosGuide_stateChanged(int arg1);
//    void on_btnImportPicture_clicked();
    void on_btnEditReference_clicked();

    void on_btnNewButtonReference_clicked();
    void on_btnDeleteButtonReference_clicked();
    void on_checkBoxEditPosReference_stateChanged(int arg1);
//    void on_btnImportPictureReference_clicked();
    void on_btnEditGuideName_clicked();
    void onChangeGuideName(const QString& newText);

private:
    void initVar();
    void updateReferPointsTable();
    void tableReferenceInit();
    void pageInit();
    int getNextAvailableIndex();

//    int getIndex(const DraggableButton* button) const;

private:
    DraggableButton* selectedButton[2];
    bool draggable[2];

//    QHash<DraggableButton*, ReferPointInfo> referencePoints;
    QList<ReferPointPara> referencePoints;

    QTableWidget* tableReference;

    QHash<DraggableButton*, GuidePara> guidePoints;

private:
    Ui::ManualForm *ui;
};

#endif // MANUALFORM_H
