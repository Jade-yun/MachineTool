#ifndef AUTOFORM_H
#define AUTOFORM_H

#include <QWidget>
#include <QTableWidget>

#include "stacksetdialog.h"
#include "customkeyboard.h"
#include "autocorrectposdialog.h"

namespace Ui {
class AutoForm;
}

class AutoForm : public QWidget
{
    Q_OBJECT

public:
    explicit AutoForm(QWidget *parent = nullptr);
    ~AutoForm();

private slots:

    void on_btnEdit_pressed();

    void on_btnAimedProduct_clicked();

    void on_btnEdit_toggled(bool checked);

    void showReferPointDialog();

private:
    void updateLabelState(int index);
public:

private:
    Ui::AutoForm *ui;

private:
    StackSetDialog* stackSet; // 堆叠指定
    QDialog *dialogReferPoint;
    QTabWidget* tabReferPoint;

    void callNumKeyBoard(QObject* obj);
};

#endif // AUTOFORM_H
