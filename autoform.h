#ifndef AUTOFORM_H
#define AUTOFORM_H

#include <QWidget>
#include <QTableWidget>

#include "stacksetdialog.h"
#include "customkeyboard.h"
#include "autocorrectposdialog.h"
#include "referencewidget.h"

namespace Ui {
class AutoForm;
}

class AutoForm : public QWidget
{
    Q_OBJECT

public:
    explicit AutoForm(QWidget *parent = nullptr);
    ~AutoForm();

signals:
    void Auto_File_List_Refresh_signal();//自动运行界面列表刷新信号
    void Switch_ProNum_Signal(uint8_t ProNum);//切换主子程序信号
private slots:

    void on_btnEdit_pressed();

    void on_btnAimedProduct_clicked();

    void on_btnEdit_toggled(bool checked);

    void showReferPointDialog();

private:
    void updateLabelState(int index);

public:
    void Auto_File_List_Refresh(uint8_t ProNum);//刷新自动界面内容显示列表
private:
    Ui::AutoForm *ui;
    ReferenceWidget* referEditDialog;

private:
    StackSetDialog* stackSet; // 堆叠指定
    void callNumKeyBoard(QObject* obj);
};

#endif // AUTOFORM_H
