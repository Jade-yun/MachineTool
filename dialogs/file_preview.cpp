#include "file_preview.h"
#include "ui_file_preview.h"
#include "framelesswidget2.h"

bool File_Preview_Label = false;
File_Preview::File_Preview(int curProgramIndex, QWidget *parent) :
    QDialog(parent),PreviewIndex(curProgramIndex),
    ui(new Ui::File_Preview)
{
    ui->setupUi(this);

    FramelessWidget2* frameBase = new FramelessWidget2(this);
    frameBase->setWidget(this);

    setModal(true);
    ui->preview_list->setSelectionBehavior(QAbstractItemView::SelectRows); //设置选择行为时每次选择一行
    ui->preview_list->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置不可编辑
    ui->preview_list->setSelectionMode(QAbstractItemView::SingleSelection);//设置为单选
    ui->preview_list->setShowGrid(true); //是否显示网格
    ui->preview_list->setAlternatingRowColors(true);//开启隔行自动变色
    ui->preview_list->setPalette(QPalette(QColor(237, 212, 0)));//隔行颜色
    ui->preview_list->setColumnWidth(0,80);
    ui->preview_list->setColumnWidth(1,10);
    connect(ui->btnOK,&QPushButton::clicked,this,[=](){
        for(int j=0;j<PreviewProOrder_num;j++)
        {
            g_FreeProOrder(&m_PreviewProOrder[j]);//释放程序命令的数据指针
        }
        this->accept();
    });
    Preview_show();
}

File_Preview::~File_Preview()
{
    delete ui;
}

void File_Preview::Preview_show()
{
    File_Preview_Label = true;
    readPreviewLableOrderName(m_ProgramNameAndPath[PreviewIndex]);//读取浏览程序中标签名称列表
    PreviewProOrder_num = readPreviewProgram(m_ProgramNameAndPath[PreviewIndex],m_PreviewProOrder);//读取浏览程序指令
    if(PreviewProOrder_num!=0)
    {
        uint16_t i=0;
        //先清空表格并重新设置行数和列数
        ui->preview_list->clearContents();
        ui->preview_list->setRowCount(0);
        ui->preview_list->setColumnCount(3);
        for(i=0;i<PreviewProOrder_num;i++)
        {
            QTableWidgetItem *Teach_File_List_NumItem = new QTableWidgetItem();
            QTableWidgetItem *Teach_File_List_OrderItem = new QTableWidgetItem();
            QTableWidgetItem *Teach_File_List_OrderColor = new QTableWidgetItem();
            Teach_File_List_NumItem->setText(JointRunOrderNum(m_PreviewProOrder[i]));
            Teach_File_List_NumItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);//设置执行行号内容居中显示
            Teach_File_List_OrderItem->setText(JointStrDeal(m_PreviewProOrder[i]));//拼接显示内容
            Teach_File_List_OrderItem->setData(Qt::TextAlignmentRole, Qt::AlignLeft);//内容左右对齐靠左显示
            Teach_File_List_OrderItem->setData(Qt::TextAlignmentRole, Qt::AlignVCenter);//内容上下对齐居中对齐显示
            Teach_File_List_OrderItem->setData(Qt::TextWordWrap,1);//设置内容自动换行显示
            ui->preview_list->insertRow(i);
            ui->preview_list->setItem(i,0,Teach_File_List_NumItem);   //显示命令执行序号
            ui->preview_list->setItem(i,2,Teach_File_List_OrderItem);                 //显示命令内容
            ui->preview_list->setItem(i,1,Teach_File_List_OrderColor);
            if(m_PreviewProOrder[i].noteFlag == 1)
            {//屏蔽指令时行背景显示灰色
                ui->preview_list->item(i,0)->setBackground(QColor(192, 191, 188));
                ui->preview_list->item(i,1)->setBackground(QColor(192, 191, 188));
                ui->preview_list->item(i,2)->setBackground(QColor(192, 191, 188));
            }
        }
        ui->preview_list->selectRow(0);
    }
}
