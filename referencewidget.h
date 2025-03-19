#ifndef REFERENCEWIDGET_H
#define REFERENCEWIDGET_H

#include <QDialog>
#include <QTableWidget>
#include <QPushButton>
#include <QTextBrowser>
#include <QLabel>
#include <QList>
#include "draggablebutton.h"
#include "cmd.h"
#include "customkeyboard.h"

#include "framelesswidget2.h"

struct ReferPointPara
{
    int index; // 唯一标识按钮的索引 从 1 开始
    QString name; // 参考点名字
    DraggableButton* button; // 直接绑定按钮对象

    QPoint pointPos; // 参考点在ui上的位置

    int axisPos[6];
};

class ReferenceWidget : public QDialog
{
    Q_OBJECT
public:
    explicit ReferenceWidget(QWidget *parent = nullptr);

//    void addReferencePoint();
//    void removeReferencePoint();
    // other public methods

private:
    void updateUI(const QList<ReferPointPara>& referPoints);
    void init();
    void updateReferPointsTable();
    void updateReferPoints();

    void updateReferPointsAxisPos();
    void refreshPosTable();

private:
    QList<ReferPointPara> referencePoints;
    QTableWidget* tableReference;
    DraggableButton* selectedReferPoint;

    QLabel* labReferPointPic;
    QFrame* frameRerencePoint;
    QTextBrowser* textReferPointName;
    QTableWidget* tableWgtAxisPos;

    QPushButton *btnOK;
    QPushButton *btnCancel;
    QPushButton *btnFresh;
    QPushButton *btnEditRefName;

//    FullKeyboard* keyboard;

    // QWidget interface
protected:
    void showEvent(QShowEvent *event) override;
    void changeEvent(QEvent* e) override;
};

#endif // REFERENCEWIDGET_H
