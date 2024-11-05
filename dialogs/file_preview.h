#ifndef FILE_PREVIEW_H
#define FILE_PREVIEW_H

#include <QDialog>
#include "program_save.h"
#include "Orderjoint.h"

namespace Ui {
class File_Preview;
}

class File_Preview : public QDialog
{
    Q_OBJECT

public:
    explicit File_Preview(int curProgramIndex,QWidget *parent = nullptr);
    ~File_Preview();
private:
    int PreviewIndex;
    Ui::File_Preview *ui;
    P_ProOrderStruct m_PreviewProOrder[PRO_LINE_MAIN];//定义一个预览程序临时变量
    int PreviewProOrder_num;//程序行数
private:
    void Preview_show();
};
extern bool File_Preview_Label;
#endif // FILE_PREVIEW_H
