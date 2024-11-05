#ifndef REFERPOINTDIALOG_H
#define REFERPOINTDIALOG_H

#include <QDialog>
#include <QString>
#include <QCheckBox>
#include "manualform.h"
#include "basewindow.h"
#include "mainwindow.h"
namespace Ui {
class ReferPointDialog;
}

class ReferPointDialog : public BaseWindow
{
    Q_OBJECT

public:
    explicit ReferPointDialog(QWidget *parent = nullptr);
    ~ReferPointDialog();

public:
    void updateUI(const QList<ReferPointPara>& referPoints);
    QString getSelectedRefPoint() const;
    int getRefPointIndex() const;
private:
    Ui::ReferPointDialog *ui;
     QButtonGroup *chboxGroup;
};

class RefPointEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit RefPointEdit(QWidget *parent = nullptr);
public:
    int getRefPointIndex() const;
    void setRefPointIndex(int index);
    int refPointIndex;
};

#endif // REFERPOINTDIALOG_H
