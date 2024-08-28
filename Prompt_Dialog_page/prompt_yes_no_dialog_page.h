#ifndef PROMPT_YES_NO_DIALOG_PAGE_H
#define PROMPT_YES_NO_DIALOG_PAGE_H

#include <QDialog>


namespace Ui {
class prompt_yes_no_Dialog_page;
}

class prompt_yes_no_Dialog_page : public QDialog
{
    Q_OBJECT

public:
    explicit prompt_yes_no_Dialog_page(QWidget *parent = 0);
    ~prompt_yes_no_Dialog_page();
    Ui::prompt_yes_no_Dialog_page *ui;
private slots:
    void on_Key_yes_no_page_yes_clicked();

    void on_Key_yes_no_page_no_clicked();
signals:
    void Prompt_Button_Handle_Result();//提示界面处理函数处理完发送信号

    void DeleteOrder_Return();//教导-删除指令处理返回信号

    void signal_RestoreFactory();      //恢复出厂设置参数下发信号

    void signal_ResetMacPar();         //重置机器参数下发信号

    void signal_ResetSysPar();         //重置系统参数下发信号

    void signal_Clear_File_success();  //删除文件成功返回信号

    void signal_Copy_File_success();  //删除文件成功返回信号
private:

};

#endif // PROMPT_YES_NO_DIALOG_PAGE_H
