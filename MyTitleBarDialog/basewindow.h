#ifndef BASEWINDOW_H
#define BASEWINDOW_H
#include "stackedit.h"
#include <QDialog>
#include "mytitlebar.h"
#include "QVBoxLayout"
class BaseWindow : public QDialog
{
    Q_OBJECT

public:
    BaseWindow(QWidget *parent = 0);
    ~BaseWindow();

    void setStackEdit(StackEdit *stackEdit){
        mylayout = new QVBoxLayout(this);
        mylayout->setContentsMargins(0, 50, 0, 0);
        mylayout->addWidget(stackEdit);
        setLayout(mylayout);
    }
public:
    void StackEditBaseInit();
protected:
    QVBoxLayout* mylayout;
private:
    void initTitleBar();
    void paintEvent(QPaintEvent *event);
private slots:
    void onButtonMinClicked();
    void onButtonRestoreClicked();
    void onButtonMaxClicked();
    void onButtonCloseClicked();

protected:
    MyTitleBar* m_titleBar;

};

#endif
