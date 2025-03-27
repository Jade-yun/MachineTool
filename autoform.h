#ifndef AUTOFORM_H
#define AUTOFORM_H

#include <QWidget>
#include <QTableWidget>
#include <QStandardItemModel>
#include "stacksetdialog.h"
#include "customkeyboard.h"
#include "autocorrectposdialog.h"
#include "referencewidget.h"
#include "cleardialog.h"
#include "producthistorydialog.h"

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
    void Send_planProductNum_Signal();//计划产品个数下发信号
private slots:
    void UpdateFloatPoPShow();

    void on_btnAimedProduct_clicked();

    void on_btnEdit_toggled(bool checked);

    void showReferPointDialog();

    void on_Auto_file_List_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);

    void on_AutoEditAxisPos_textChanged(const QString &arg1);

    void on_AutoEditchboxIfCondition1_clicked(bool checked);

    void on_AutoEditchboxIfCondition2_clicked(bool checked);

public slots:
    void Refresh_globalSpeedShowHandel();
    void SetAutoRunIcon();
private:
    void updateLabelState(int index);
    void Program_Follow_Refresh();
    void Var_State_Refresh();
    void Auto_State_Refresh();
    void Stack_State_Refresh();
    void OrderEditHandel();
    void OrderEditSaveHandel();
    void AutoFromIfOrderEditHandle(uint8_t IfIndex);
    void AutoFromIfOrderEditSaveHandle(uint8_t IfIndex);
    void SetAutoRunParIcon(uint8_t type);
    void Auto_CurStep_Refresh();
    QStringList processInstructions(uint8_t type);
public:
    void Auto_File_List_Refresh(uint8_t ProNum);//刷新自动界面内容显示列表
    void AutoForm_Refresh();
private:
    Ui::AutoForm *ui;
    ReferenceWidget* referEditDialog;
    StackSetDialog* stackSet; // 堆叠指定
    ClearDialog* clearDialog;
    ProductHistoryDialog* productHisDlg;

    QTableWidget* StackSpeedFloatPoP;//悬浮小弹窗
    std::vector<QAction*> operateActions; // 操作菜单
    std::vector<std::string> actionStrs;    // 操作菜单源文本（用于多语言）
    QComboBox* FloatPoP_coboxStkGroupNum;
    NumberEdit* FloatPoP_editX1Num;
    NumberEdit* FloatPoP_editY1Num;
    NumberEdit* FloatPoP_editZ1Num;
    QComboBox* FloatPoP_coboxStkFinishStop;
    std::array<NumberEdit*, VAR_TOTAL_NUM> VarStateEdit;//变量状态界面，变量列输入框
    std::array<QTableWidgetItem*, STACK_TOTAL_NUM> AtackStateEdit;//变量状态界面，堆叠状态列输入框
    QStandardItemModel* AutoStateMode;
    QStringList StackOrderNumList;//用来记录当前程序指令中存在的堆叠指令的堆叠组编号
    QStringList FloatPoPStackOrderNumList;
private:
    void callNumKeyBoard(QObject* obj);

protected:
    void changeEvent(QEvent *e) override;
    void retranslate();
};

#endif // AUTOFORM_H
