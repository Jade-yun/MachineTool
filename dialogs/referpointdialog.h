#ifndef REFERPOINTDIALOG_H
#define REFERPOINTDIALOG_H

#include <QDialog>
#include <QString>
#include "manualform.h"

namespace Ui {
class ReferPointDialog;
}

class ReferPointDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReferPointDialog(QWidget *parent = nullptr);
    ~ReferPointDialog();

public:
    void updateUI(const QList<ReferPointPara>& referPoints);
    QString getSelectedRefPoint() const;
private:
    Ui::ReferPointDialog *ui;
};

class RefPointEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit RefPointEdit(QWidget *parent = nullptr);

protected:
    void mouseReleaseEvent(QMouseEvent *event) override;
};

#endif // REFERPOINTDIALOG_H
