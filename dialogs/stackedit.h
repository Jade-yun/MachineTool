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

enum StackMode : unsigned char {
    THREE_POINT = 0,
    FOUR_POINT,
    NORMAL
};

namespace Ui {
class StackEdit;
}

class StackFollow;

class StackEdit : public QWidget
{
    Q_OBJECT

#define AXIS_NUM 3
public:
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
    QComboBox* stackDirect[3]; //0 从小到大  1 从大到小
    NumberEdit* intervalDis[3];
    NumberEdit* offset[3];

    StackFollow* moveStack;
    StackFollow* moveFollow;

    void initStackPara(QTableWidget* tableWidget);

public:
    void switchStackWay(StackMode mode);
    void showOKandCancelButton(bool isVisible);

public slots:
    void saveStackInfo();
    void updateGroupIndex(int index);
public:
    explicit StackEdit(QWidget *parent = nullptr);
    ~StackEdit();

private:
    void loadAndPlayGif(const QString& path);
    void logicSigsSlots();

    void saveInfoConnections();

private:
    Ui::StackEdit *ui;
private:
    QMovie *movie;
    int groupIndex;

private slots:
    void on_coboxStackOrder_currentIndexChanged(int index);
};

class StackFollow : public QDialog {
    Q_OBJECT

public:
    StackFollow(QWidget *parent = nullptr);

public slots:
    void updateVerticalHeader(int index, const QString& text);

private:
    QTableWidget *table;
    std::array<NumberEdit*, 3> nums; // 个数
    std::array<QTableWidgetItem*, 3> objPos; // 目标位置
public:
    QPushButton *moveButton;
    QPushButton *closeButton;

};

#endif // STACKEDIT_H

extern QPointer<StackEdit> stack[8];
