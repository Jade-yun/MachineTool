#ifndef MANUALFORM_H
#define MANUALFORM_H

#include <QWidget>
#include <QTableWidget>
#include <QSettings>
#include <QHash>

#include "draggablebutton.h"
#include "customkeyboard.h"
#include "cmd.h"

#define SAVEPOINT_VERSION_1

struct GuidePara {
    QString guideName;
    QString keyDefStr;
//    D_KeyFuncStruct keyFuncDef;
};

struct ReferPointPara
{
    int index; // 唯一标识按钮的索引
    QString name; // 按钮text
    DraggableButton* button; // 直接绑定按钮对象

    QPoint pointPos; // 参考点在ui上的位置

   // add other parameter...
//    uint8_t  refFlag;							//参考点有效性，0无效 1有效
//    uint8_t  ret[1];
//    int32_t pos[AXIS_TOTAL_NUM]; // 每个轴的位置
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
    void on_btnEditGuideName_clicked();
    void onChangeGuideName(const QString& newText);

    void on_btnEditReference_clicked();
    void on_btnNewButtonReference_clicked();
    void on_btnDeleteButtonReference_clicked();
    void on_checkBoxEditPosReference_stateChanged(int arg1);
//    void on_btnImportPictureReference_clicked();

private:
    void initVar();
    void updateReferPointsTable();
    void tableReferenceInit();
    void pageInit();
    int getNextAvailableIndex();

public:
    const QList<ReferPointPara>& getRerferPoints() const;

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
