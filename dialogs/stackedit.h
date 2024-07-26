#ifndef STACKEDIT_H
#define STACKEDIT_H

#include <QWidget>
#include <QPointer>
#include <QComboBox>
#include <QLineEdit>
#include <QTableWidgetItem>
#include <QMovie>

namespace Ui {
class StackEdit;
}

class StackEdit : public QWidget
{
    Q_OBJECT

#define AXIS_NUM 3
public:
    QComboBox* axisSelect[3];
    QLineEdit* speed[3];
    QLineEdit* pointNum[3]; // 和个数等价
    QLineEdit* startPosA[3]; // 和起始(mm)等价
    QLineEdit* endPosB_X[3];
    QLineEdit* endPosC_Y[3];
    QLineEdit* posD[3];
    QLineEdit* speedDischage[3];
    QLineEdit* startPosDischage[3];

    // 1. startPosA 起始
    // 2. intervalDis
    // 3. speed
    // 4. pointNum 个数
    // 5. offset 偏移
    // 6. stackDirect 方向
    QComboBox* stackDirect[3]; //0 从小到大  1 从大到小
    QLineEdit* intervalDis[3];
    QLineEdit* offset[3];

    void initStackPara(QTableWidget* tableWidget);
    void initNormalStackUI(QWidget* frameStack);

public:
    void switchStackWay(int mode);
    void showOKandCancelButton(bool isVisible);
public:
    explicit StackEdit(QWidget *parent = nullptr);
    ~StackEdit();

private:
    void loadAndPlayGif(const QString& path);

private:
    Ui::StackEdit *ui;
private:
    QMovie *movie;

private slots:
    void on_coboxStackOrder_currentIndexChanged(int index);
};

#endif // STACKEDIT_H

extern QPointer<StackEdit> stack[8];
