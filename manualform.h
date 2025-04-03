#ifndef MANUALFORM_H
#define MANUALFORM_H

#include <QWidget>
#include <QTableWidget>
#include <QSettings>
#include <QHash>
#include <QGroupBox>

#include "draggablebutton.h"
#include "referencewidget.h"
#include "logindialog.h"

#define SAVEPOINT_VERSION_1

struct GuidePara {
    QString guideName;  // 操作指引名
    QString keyDefStr;  // 操作对应按键的状态
    QPoint btnPos;      // 操作指引点在ui上的位置

    uint8_t  keyType;	// 按键功能类型，0输出 1输入 2轴
    uint8_t  portNum;	// 功能编号，输出类型时1-44，输入类型时1-60，轴类型时1-6
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
    void AxisParRefreshSignal(uint8_t Index);
private slots:
    void setbuttonIcon(QPushButton *button, const QString &ButtonText, uint8_t state);
    void on_checkBoxEditPosGuide_stateChanged(int arg1);
    void on_btnImportPictureGuide_clicked();
    void on_btnNewButton_clicked();
    void on_btnDeleteButton_clicked();
    void on_btnEditGuideName_clicked();   
    void on_btnSaveGuide_clicked();


    void on_btnEditReference_clicked();
    void on_btnNewButtonReference_clicked();
    void on_btnRefresh_clicked();
    void on_btnDeleteButtonReference_clicked();
    void on_checkBoxEditPosReference_stateChanged(int arg1);
    void on_btnImportPictureReference_clicked();
    void on_btnSaveReference_clicked();

    void onSaveKeyDef();
    void onCheckSavedGuide();
    void onCheckSavedReferPointPara();   
    void on_coboxAdjustMachineAxis_currentIndexChanged(int index);

public slots:
    void InitAdjustMachine(uint8_t AxisIndex);//初始化调机界面参数

private:
    void initVar();
    void updateReferPointsTable();
    void tableReferenceInit();
    void pageInit();
    int getNextAvailableIndex();

    void addReferencePoint();
    void removeReferencePoint();

    void updateGuidePoints();
    void setupGuidePointConnections(DraggableButton* btn);
    void updateReferPointsList();
    void setupReferPointConnections(DraggableButton* btn);
    void onReferPointButtonPressed(DraggableButton* btn);
    /**
     * @brief 根据参考点全局变量的轴位置更新局部变量 referencePoints 中轴位置
     */
    void updateReferPointsAxisPos();
    /**
     * @brief 刷新参考点轴位置表格显示
     */
    void refreshPosTable();
    void saveManualAxisActionPara();

    void setupReserveWidgets();
    void setupClawAndMachineConnections();
    void setupAxisActionConnections();

    /**
     * @brief 该函数在每次 show 事件时调用
     */
    void refreshAxisActionPage();
    void refreshClawAndMachinePage();
    void refreshReservePage();

public:
    void handleLoginModeChanged(LoginMode mode);
    void reloadReferPoint();

private:
    DraggableButton* selectedButton[2];
    bool draggable[2];

//    QHash<DraggableButton*, ReferPointInfo> referencePoints;
    QList<ReferPointPara> referencePoints;
    QTableWidget* tableReference;

    QHash<DraggableButton*, GuidePara> guidePoints;

    std::vector<QPushButton*> reserveButtons;

    QTimer* checkTimer;
private:
    Ui::ManualForm *ui;

    // QObject interface
protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void changeEvent(QEvent* e) override;
    void retranslate();
};

#endif // MANUALFORM_H
