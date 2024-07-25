#ifndef STACKEDIT_H
#define STACKEDIT_H

#include <QWidget>
#include <QPointer>
#include <QComboBox>
#include <QLineEdit>
#include <QTableWidgetItem>

struct StackPara /*: public QObject*/
{
//    Q_OBJECT

public:
    QComboBox* axisSelect[3];
    QLineEdit* speed[3];
    QLineEdit* pointNum[3];
    QLineEdit* startPtA[3];
    QLineEdit* endPtB_X[3];
    QLineEdit* endPtC_Y[3];
    QLineEdit* posD[3];
    QLineEdit* speedDischage[3];
    QLineEdit* startPtDischage[3];

    explicit StackPara(QTableWidget* tableWidget)
    {
        tableWidget->setRowCount(9);
        tableWidget->setColumnCount(3);

        for (int i = 0; i < 3; ++i)
        {
            axisSelect[i] = new QComboBox(tableWidget);
            switch (i) {
                case 0:
                    axisSelect[i]->addItems({ QObject::tr("X1轴"), QObject::tr("X2轴") });
                    break;
                case 1:
                    axisSelect[i]->addItems({ QObject::tr("Y1轴"), QObject::tr("Y2轴") });
                    break;
                case 2:
                    axisSelect[i]->addItems({ QObject::tr("Z1轴"), QObject::tr("Z2轴") });
                    break;
                default:
                    break;
            }

            speed[i] = new QLineEdit(tableWidget);
            pointNum[i] = new QLineEdit(tableWidget);
            startPtA[i] = new QLineEdit(tableWidget);
            endPtB_X[i] = new QLineEdit(tableWidget);
            endPtC_Y[i] = new QLineEdit(tableWidget);
            posD[i] = new QLineEdit(tableWidget);
            speedDischage[i] = new QLineEdit(tableWidget);
            startPtDischage[i] = new QLineEdit(tableWidget);

            tableWidget->setCellWidget(0, i, axisSelect[i]);
            tableWidget->setCellWidget(1, i, speed[i]);
            tableWidget->setCellWidget(2, i, pointNum[i]);
            tableWidget->setCellWidget(3, i, startPtA[i]);
            tableWidget->setCellWidget(4, i, endPtB_X[i]);
            tableWidget->setCellWidget(5, i, endPtC_Y[i]);
            tableWidget->setCellWidget(6, i, posD[i]);
            tableWidget->setCellWidget(7, i, speedDischage[i]);
            tableWidget->setCellWidget(8, i, startPtDischage[i]);
        }

        QStringList verticalHeaderLabels = {
            QObject::tr("轴选择"),
            QObject::tr("速度"),
            QObject::tr("点数"),
            QObject::tr("A-起点"),
            QObject::tr("B-X方向终点"),
            QObject::tr("C-Y方向终点"),
            QObject::tr("D点坐标"),
            QObject::tr("放料速度"),
            QObject::tr("放料起始点")
        };
        tableWidget->setVerticalHeaderLabels(verticalHeaderLabels);

        QStringList horizontalHeaderLabels = {
            QObject::tr("X1轴"),
            QObject::tr("Y1轴"),
            QObject::tr("Z1轴")
        };
        tableWidget->setHorizontalHeaderLabels(horizontalHeaderLabels);
    }
};

namespace Ui {
class StackEdit;
}

class StackEdit : public QWidget
{
    Q_OBJECT

public:
    explicit StackEdit(QWidget *parent = nullptr);
    ~StackEdit();

signals:
    void moveButtonPos(bool para);


private:
    Ui::StackEdit *ui;

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event) override;
};

#endif // STACKEDIT_H

extern QPointer<StackEdit> stack[8];
