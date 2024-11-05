#ifndef MANUALFORM_H
#define MANUALFORM_H

#include <QWidget>
#include <QTableWidget>
#include <QSettings>
#include <QHash>

#include "draggablebutton.h"
#include "customkeyboard.h"
#include "cmd.h"
#include "referencewidget.h"

#define SAVEPOINT_VERSION_1

struct GuidePara {
    QString guideName;  // 操作指引名
    QString keyDefStr;  // 操作对应按键的状态
    QPoint btnPos;      // 操作指引点在ui上的位置

    uint8_t  keyType;	// 按键功能类型，0输出 1输入 2轴
    uint8_t  portNum;	// 功能编号，输出类型时1-40，输入类型时1-60，轴类型时1-6
    uint8_t  status;    // 功能状态，输出类型时0断 1通 2翻转，输入类型时0无信号 1有信号，轴类型时0正向 1反向
};

//struct ReferPointPara
//{
//    int index; // 唯一标识按钮的索引 从 1 开始
//    QString name; // 参考点名字
//    DraggableButton* button; // 直接绑定按钮对象

//    QPoint pointPos; // 参考点在ui上的位置

//};

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
    void on_checkBoxEditPosGuide_stateChanged(int arg1);
    void on_btnImportPictureGuide_clicked();
    void on_btnNewButton_clicked();
    void on_btnDeleteButton_clicked();
    void on_btnEditGuideName_clicked();   
    void on_btnSaveGuide_clicked();


    void on_btnEditReference_clicked();
    void on_btnNewButtonReference_clicked();
    void on_btnDeleteButtonReference_clicked();
    void on_checkBoxEditPosReference_stateChanged(int arg1);
    void on_btnImportPictureReference_clicked();
    void on_btnSaveReference_clicked();

private slots:
    void onTabChanged(int index);
    void onSaveKeyDef();
    void onCheckSavedGuide();
    void onCheckSavedReferPointPara();
    void on_cb_axisActionAxis_currentIndexChanged(int index);

private:
    void initVar();
    void updateReferPointsTable();
    void tableReferenceInit();
    void pageInit();
    int getNextAvailableIndex();

    void addReferencePoint();
    void removeReferencePoint();

    void updateGuidePoints();
    void updateReferPointsList();

    void initControls();            //pxc 初始化控件并对控件进行赋值

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

    // QObject interface
public:
    bool eventFilter(QObject *watched, QEvent *event) override;
};

#endif // MANUALFORM_H
