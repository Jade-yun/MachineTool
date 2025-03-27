#ifndef STACKEDIT_H
#define STACKEDIT_H

#include <QWidget>
#include <QPointer>
#include <QComboBox>
#include <QTableWidgetItem>
#include <QMovie>
#include <QDialog>

#include "customedit.h"
#include "cmd.h"
#include "iniconfig.h"

enum StackMode : unsigned char {
    THREE_POINT = 0,
    FOUR_POINT,
    NORMAL,
    ROTATE_BIN,
    TEACH_THREE_POINT,
    TEACH_FOUR_POINT,
    TEACH_NOMAL,
    TEACH_ROTATE_BIN
};

namespace Ui {
class StackEdit;
}

class StackFollow;

class StackEdit : public QWidget
{
    Q_OBJECT
private:
    // 三点式/四点式 模式界面对应的控件
    QComboBox* axisSelect[3];
    NumberEdit* speed[3];
    NumberEdit* pointNum[3]; // 和个数等价
    NumberEdit* startPosA[3]; // 和起始(mm)等价
    NumberEdit* endPosB_X[3];
    NumberEdit* endPosC_Y[3];
    NumberEdit* posD[3];
    NumberEdit* speedDischage[3];
    NumberEdit* startPosDischage[3];

    // 0. 轴选择  共用
    // 1. startPosA 起始  共用
    // 2. intervalDis
    // 3. speed  共用
    // 4. pointNum 个数  共用
    // 5. offset 偏移
    // 6. stackDirect 方向

    // 普通模式界面对应的控件
    QComboBox* axisSelectNormal[3];
    NumberEdit* startPosANormal[3];
    NumberEdit* intervalDis[3];
    NumberEdit* speedNormal[3];
    NumberEdit* pointNumNormal[3];
    NumberEdit* offset[3];
    QComboBox* stackDirect[3]; //0 从小到大  1 从大到小

    StackFollow* moveStack;
    StackFollow* moveFollow;

public:
    void switchStackWay(StackMode mode);
    void showOKandCancelButton(bool isVisible);

public slots:
    void saveStackBasicInfo();          //基础参数
    void saveStackPointPosInfo();           //点位参数
    void saveRotateBinStackInfo(); // 保存旋转料仓模式下的堆叠参数

    void updateGroupIndex(int index);
    void StackAxisSelectQcomboboxRefresh();//堆叠轴选择框需要刷新
    void syncParaToUI();
public:
    explicit StackEdit(int groupIndex, QWidget *parent = nullptr);
    ~StackEdit();

private:
    void initWidgets();
    void loadAndPlayGif(const QString& path);
    void logicSigsSlots();
    void saveInfoConnections();
    void setupRefreshConnections();

protected:
    void changeEvent(QEvent *e) override;
    void retranslate();
    void showEvent(QShowEvent *event) override;

private:
    Ui::StackEdit *ui;
    QMovie *movie;
private:
    int groupIndex; // 堆叠组号 0-7
    int pressedRow; // 堆叠 table header 被按下的行
signals:
    void closeStackEditDialogSignal(); // 定义关闭对话框的信号
    void stackParRefreshSignal();
    void StackFollowRefreshSignal(); //移至堆叠点/跟随点界面参数刷新信号
private slots:
    void on_coboxStackOrder_currentIndexChanged(int index);
    void onRefreshClicked(); // 三点/四点式下的刷新位置槽函数
    /**
     * @brief 普通模式下刷新光标所在输入控件的位置
     * @param colIndex 指定的列编号（范围：0-2），用于标识普通模式下的三列之一
     */
    void onRefreshNormalStackClicked(int colIndex);
};

class StackFollow : public QDialog {
    Q_OBJECT

public:
    StackFollow(QWidget *parent = nullptr);

public slots:
    void updateVerticalHeader(int index, const QString& text);
    void StackFollowInit();
private:
    QTableWidget *table;
    std::array<NumberEdit*, 3> nums; // 个数
    std::array<QTableWidgetItem*, 3> objPos; // 目标位置
public:
    QPushButton *moveButton;
    QPushButton *closeButton;

protected:
    void changeEvent(QEvent *e) override;
};

#endif // STACKEDIT_H

extern QPointer<StackEdit> stack[8];
