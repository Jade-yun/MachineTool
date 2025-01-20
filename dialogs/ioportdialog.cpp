#include "ioportdialog.h"
#include "ui_ioportdialog.h"

#include <QListView>

IOPortDialog::IOPortDialog(QWidget *parent, IOPortMode mode) :
    BaseWindow(parent),
    ui(new Ui::IOPortDialog)
{
    ui->setupUi(this);
    this->update();

	setModal(true);
    if (mode == IOPortMode::IN)
    {
        ui->stkWgt->setCurrentIndex(0);
    }
    else if (mode == IOPortMode::OUT)
    {
        ui->stkWgt->setCurrentIndex(1);
    }

    for (auto cobox : findChildren<QComboBox*>())
    {
        auto listView = new QListView(cobox);
        cobox->setView(listView);

        cobox->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    }

    connect(ui->btnOK, &QPushButton::clicked, this, &IOPortDialog::accept);
    connect(ui->btnCancel, &QPushButton::clicked, this, &IOPortDialog::reject);
}

IOPortDialog::IOPortDialog(QWidget *parent, IOPortMode mode, bool TeachTrigger) :
    BaseWindow(parent),
    ui(new Ui::IOPortDialog)
{
    ui->setupUi(this);
    this->update();

    setModal(true);
    if (mode == IOPortMode::IN)
    {
        ui->stkWgt->setCurrentIndex(0);
        if(TeachTrigger) setchboxMainBoardInItem();

    }
    else if (mode == IOPortMode::OUT)
    {
        ui->stkWgt->setCurrentIndex(1);
        if(TeachTrigger) setchboxMainBoardOutItem();
    }
    else if(mode == IOPortMode::SearchIN)
    {
        ui->stkWgt->setCurrentIndex(2);
    }

    for (auto cobox : findChildren<QComboBox*>())
    {
        auto listView = new QListView(cobox);
        cobox->setView(listView);

        cobox->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    }

    connect(ui->btnOK, &QPushButton::clicked, this, &IOPortDialog::accept);
    connect(ui->btnCancel, &QPushButton::clicked, this, &IOPortDialog::reject);
}

IOPortDialog::~IOPortDialog()
{
    delete ui;
}

QString IOPortDialog::getIOOnlineIn() const
{
    if (ui->stkWgt->currentIndex() == 0 && ui->tabWgtMainBoardIn->currentIndex() == 0)
    {
        return getMainBoardIn();
    }
    return QString();
}

QString IOPortDialog::getIOOnlineOut() const
{
    if (ui->stkWgt->currentIndex() == 1 && ui->tabWgtMainBoardOut->currentIndex() == 0)
    {
        return getMainBoardOut();
    }
    return QString();
}

QString IOPortDialog::getIOOnlineSearchStopIn() const
{
    if (ui->stkWgt->currentIndex() == 2)
    {
        return getSearchStopIN();
    }
    return QString();
}
unsigned int IOPortDialog::get_IOInEdit_InPort() const
{
    if (ui->chboxMainBoardIn0->isChecked())
        {
            int index = ui->coboxMainBoardIn0->currentIndex();
            return index + 1;
        }
        if (ui->chboxMainBoardIn1->isChecked())
        {
           int index = ui->coboxMainBoardIn1->currentIndex();
           return index + 12 * 1 + 1;
        }
        if (ui->chboxMainBoardIn2->isChecked())
        {
            int index =  ui->coboxMainBoardIn2->currentIndex();
            return index + 12 * 2 + 1;
        }
        if (ui->chboxMainBoardInExtend0->isChecked())
        {
            int index  = ui->coboxMainBoardInExtend0->currentIndex();
            return index + 12 * 3 + 1;
        }
        if (ui->chboxMainBoardInExtend1->isChecked())
        {
            int index  = ui->coboxMainBoardInExtend1->currentIndex();
            return index + 12 * 4 + 1;
        }
        return 0;
}

unsigned int IOPortDialog::get_IOInEdit_OutPort() const
{
    if (ui->chboxMainBoardOut0->isChecked()) {
           int index  = ui->coboxMainBoardOut0->currentIndex();
           return index + 1;
       }
       if (ui->chboxMainBoardOut1->isChecked()) {
           int index = ui->coboxMainBoardOut1->currentIndex();
           return index + 8*1 + 1;
       }
       if (ui->chboxMainBoardOut2->isChecked()) {

           int index = ui->coboxMainBoardOut2->currentIndex();
           return index + 8*2 + 1;
       }
       if (ui->chboxMainBoardOutExtend0->isChecked()) {
           int index = ui->coboxMainBoardOutExtend0->currentIndex();
           return index + 8*3 + 1;
       }
       if (ui->chboxMainBoardOutExtend1->isChecked()) {
           int index = ui->coboxMainBoardOutExtend1->currentIndex();
           return index + 8*4 + 1;
       }
       if (ui->chboxMainBoardOutExtend2->isChecked()) {
           int index = ui->coboxMainBoardOutExtend2->currentIndex();
           return index + 8*5 + 1;
       }
       return 0;
}

unsigned int IOPortDialog::getInputPort() const
{
    int index = 0;
    if (ui->chboxMainBoardIn0->isChecked())
    {
        for(int i=0;i<INPUT_TOTAL_NUM;i++)
        {
            if(m_Port_X[i].ResModifyName == ui->coboxMainBoardIn0->currentText())
            {
                index = m_Port_X[i].portNum;
                break;
            }
        }
        return index;
    }
    if (ui->chboxMainBoardIn1->isChecked())
    {
        for(int i=0;i<INPUT_TOTAL_NUM;i++)
        {
            if(m_Port_X[i].ResModifyName == ui->coboxMainBoardIn1->currentText())
            {
                index = m_Port_X[i].portNum;
                break;
            }
        }
        return index;
    }
    if (ui->chboxMainBoardIn2->isChecked())
    {
        for(int i=0;i<INPUT_TOTAL_NUM;i++)
        {
            if(m_Port_X[i].ResModifyName == ui->coboxMainBoardIn2->currentText())
            {
                index = m_Port_X[i].portNum;
                break;
            }
        }
        return index;
    }
    if (ui->chboxMainBoardInExtend0->isChecked())
    {
        for(int i=0;i<INPUT_TOTAL_NUM;i++)
        {
            if(m_Port_X[i].ResModifyName == ui->coboxMainBoardInExtend0->currentText())
            {
                index = m_Port_X[i].portNum;
                break;
            }
        }
        return index;
    }
    if (ui->chboxMainBoardInExtend1->isChecked())
    {
        for(int i=0;i<INPUT_TOTAL_NUM;i++)
        {
            if(m_Port_X[i].ResModifyName == ui->coboxMainBoardInExtend1->currentText())
            {
                index = m_Port_X[i].portNum;
                break;
            }
        }
        return index;
    }
    return index;
}

unsigned int IOPortDialog::getOutputPort() const
{
    int index =0;
    if (ui->chboxMainBoardOut0->isChecked()) {
        for(int i=0;i<OUTPUT_TOTAL_NUM;i++)
        {
            if(m_Port_Y[i].ResModifyName == ui->coboxMainBoardOut0->currentText())
            {
                index = m_Port_Y[i].portNum;
                break;
            }
        }
    }
    if (ui->chboxMainBoardOut1->isChecked()) {
        for(int i=0;i<OUTPUT_TOTAL_NUM;i++)
        {
            if(m_Port_Y[i].ResModifyName == ui->coboxMainBoardOut1->currentText())
            {
                index = m_Port_Y[i].portNum;
                break;
            }
        }
    }
    if (ui->chboxMainBoardOut2->isChecked()) {
        for(int i=0;i<OUTPUT_TOTAL_NUM;i++)
        {
            if(m_Port_Y[i].ResModifyName == ui->coboxMainBoardOut2->currentText())
            {
                index = m_Port_Y[i].portNum;
                break;
            }
        }
    }
    if (ui->chboxMainBoardOutExtend0->isChecked()) {
        for(int i=0;i<OUTPUT_TOTAL_NUM;i++)
        {
            if(m_Port_Y[i].ResModifyName == ui->coboxMainBoardOutExtend0->currentText())
            {
                index = m_Port_Y[i].portNum;
                break;
            }
        }
    }
    if (ui->chboxMainBoardOutExtend1->isChecked()) {
        for(int i=0;i<OUTPUT_TOTAL_NUM;i++)
        {
            if(m_Port_Y[i].ResModifyName == ui->coboxMainBoardOutExtend1->currentText())
            {
                index = m_Port_Y[i].portNum;
                break;
            }
        }
    }
    if (ui->chboxMainBoardOutExtend2->isChecked()) {
        for(int i=0;i<OUTPUT_TOTAL_NUM;i++)
        {
            if(m_Port_Y[i].ResModifyName == ui->coboxMainBoardOutExtend2->currentText())
            {
                index = m_Port_Y[i].portNum;
                break;
            }
        }
    }
    return index;
}

unsigned int IOPortDialog::getSearchStopInPort() const
{
    if (ui->chboxAxisSearchIn0->isChecked())
    {
        int index = ui->coboxAxisSearchIn0->currentIndex();
        return index + 1;
    }
    if (ui->chboxAxisSearchIn1->isChecked())
    {
       int index = ui->coboxAxisSearchIn1->currentIndex();
       return index + 12 * 1 + 1;
    }
    if (ui->chboxAxisSearchIn2->isChecked())
    {
        int index =  ui->coboxAxisSearchIn2->currentIndex();
        return index + 12 * 2 + 1;
    }
    if (ui->chboxAxisSearchExIn0->isChecked())
    {
        int index  = ui->coboxAxisSearchExIn0->currentIndex();
        return index + 12 * 3 + 1;
    }
    if (ui->chboxAxisSearchExIn1->isChecked())
    {
        int index  = ui->coboxAxisSearchExIn1->currentIndex();
        return index + 12 * 4 + 1;
    }
    return 0;
}

bool IOPortDialog::getSearchStopPortState() const
{
    if (ui->chboxAxisSearchIn0->isChecked()) return ui->AxisSearchIn0button->getState();
    if (ui->chboxAxisSearchIn1->isChecked()) return ui->AxisSearchIn1button->getState();
    if (ui->chboxAxisSearchIn2->isChecked()) return ui->AxisSearchIn2button->getState();
    if (ui->chboxAxisSearchExIn0->isChecked()) return ui->AxisSearchExIn0button->getState();
    if (ui->chboxAxisSearchExIn1->isChecked()) return ui->AxisSearchExIn1button->getState();
    return true;
}
QString IOPortDialog::getMainBoardIn() const
{
    if (ui->chboxMainBoardIn0->isChecked()) return ui->coboxMainBoardIn0->currentText();
    if (ui->chboxMainBoardIn1->isChecked()) return ui->coboxMainBoardIn1->currentText();
    if (ui->chboxMainBoardIn2->isChecked()) return ui->coboxMainBoardIn2->currentText();
    if (ui->chboxMainBoardInExtend0->isChecked()) return ui->coboxMainBoardInExtend0->currentText();
    if (ui->chboxMainBoardInExtend1->isChecked()) return ui->coboxMainBoardInExtend1->currentText();
    return QString("0");
}

QString IOPortDialog::getMainBoardOut() const
{
    if (ui->chboxMainBoardOut0->isChecked()) return ui->coboxMainBoardOut0->currentText();
    if (ui->chboxMainBoardOut1->isChecked()) return ui->coboxMainBoardOut1->currentText();
    if (ui->chboxMainBoardOut2->isChecked()) return ui->coboxMainBoardOut2->currentText();
    if (ui->chboxMainBoardOutExtend0->isChecked()) return ui->coboxMainBoardOutExtend0->currentText();
    if (ui->chboxMainBoardOutExtend1->isChecked()) return ui->coboxMainBoardOutExtend1->currentText();
    if (ui->chboxMainBoardOutExtend2->isChecked()) return ui->coboxMainBoardOutExtend2->currentText();
    return QString("0");
}
QString IOPortDialog::getSearchStopIN() const
{
    if (ui->chboxAxisSearchIn0->isChecked()) return ui->coboxAxisSearchIn0->currentText()+ui->AxisSearchIn0button->text();
    if (ui->chboxAxisSearchIn1->isChecked()) return ui->coboxAxisSearchIn1->currentText()+ui->AxisSearchIn1button->text();
    if (ui->chboxAxisSearchIn2->isChecked()) return ui->coboxAxisSearchIn2->currentText()+ui->AxisSearchIn2button->text();
    if (ui->chboxAxisSearchExIn0->isChecked()) return ui->coboxAxisSearchExIn0->currentText()+ui->AxisSearchExIn0button->text();
    if (ui->chboxAxisSearchExIn1->isChecked()) return ui->coboxAxisSearchExIn1->currentText()+ui->AxisSearchExIn1button->text();
    return QString("0");
}
//设置输入IO每个复选框可供选择项
void IOPortDialog::setchboxMainBoardInItem()
{
    ui->coboxMainBoardIn0->clear();
    ui->coboxMainBoardIn1->clear();
    ui->coboxMainBoardIn2->clear();
    ui->coboxMainBoardInExtend0->clear();
    ui->coboxMainBoardInExtend1->clear();
    //输入IO第一组
    if(m_OutportInterlock[0][1] == 0)
    {
        ui->coboxMainBoardIn0->addItem(m_Port_X[0].ResModifyName);
    }
    if(m_OutportInterlock[0][3] == 0)
    {
        ui->coboxMainBoardIn0->addItem(m_Port_X[1].ResModifyName);
    }
    if(m_OutportInterlock[1][1] == 0)
    {
        ui->coboxMainBoardIn0->addItem(m_Port_X[2].ResModifyName);
    }
    if(m_OutportInterlock[1][3] == 0)
    {
        ui->coboxMainBoardIn0->addItem(m_Port_X[3].ResModifyName);
    }
    if(m_OutportInterlock[2][1] == 0)
    {
        ui->coboxMainBoardIn0->addItem(m_Port_X[4].ResModifyName);
    }
    if(m_OutportInterlock[2][3] == 0)
    {
        ui->coboxMainBoardIn0->addItem(m_Port_X[5].ResModifyName);
    }
    if(m_SeniorFunc.stackSaveIn1Check == 0)
    {
        ui->coboxMainBoardIn0->addItem(m_Port_X[6].ResModifyName);
    }
    if(m_SeniorFunc.emergencyStopCheck == 0)
    {
        ui->coboxMainBoardIn0->addItem(m_Port_X[7].ResModifyName);
    }
    if(m_OutportInterlock[3][1] == 0)
    {
        ui->coboxMainBoardIn0->addItem(m_Port_X[8].ResModifyName);
    }
    if(m_OutportInterlock[3][3] == 0)
    {
        ui->coboxMainBoardIn0->addItem(m_Port_X[9].ResModifyName);
    }
    if(m_OutportInterlock[4][1] == 0)
    {
        ui->coboxMainBoardIn0->addItem(m_Port_X[10].ResModifyName);
    }
    if(m_OutportInterlock[4][3] == 0)
    {
        ui->coboxMainBoardIn0->addItem(m_Port_X[11].ResModifyName);
    }
    if(ui->coboxMainBoardIn0->count() == 0)
    {
        ui->chboxMainBoardIn0->setEnabled(false);
        ui->coboxMainBoardIn0->setEnabled(false);
    }
    else
    {
        ui->chboxMainBoardIn0->setEnabled(true);
        ui->coboxMainBoardIn0->setEnabled(true);
    }
    //输入IO第二组
    if(m_SeniorFunc.remoteAuto == 0)
    {
        ui->coboxMainBoardIn1->addItem(m_Port_X[12].ResModifyName);
    }
    if(m_SeniorFunc.remoteStop == 0)
    {
        ui->coboxMainBoardIn1->addItem(m_Port_X[13].ResModifyName);
    }
    if(m_SeniorFunc.manualChuckIn1 == 0)
    {
        ui->coboxMainBoardIn1->addItem(m_Port_X[14].ResModifyName);
    }
    if(m_SeniorFunc.pressureCheck == 0)
    {
        ui->coboxMainBoardIn1->addItem(m_Port_X[15].ResModifyName);
    }
    if(m_Port_X[16].functionSet == 0)
    {
        ui->coboxMainBoardIn1->addItem(m_Port_X[16].ResModifyName);
    }
    if(m_Port_X[17].functionSet == 0)
    {
        ui->coboxMainBoardIn1->addItem(m_Port_X[17].ResModifyName);
    }
    if(m_Port_X[18].functionSet == 0)
    {
        ui->coboxMainBoardIn1->addItem(m_Port_X[18].ResModifyName);
    }
    if(m_Port_X[19].functionSet == 0)
    {
        ui->coboxMainBoardIn1->addItem(m_Port_X[19].ResModifyName);
    }
    if(m_Port_X[20].functionSet == 0)
    {
        ui->coboxMainBoardIn1->addItem(m_Port_X[20].ResModifyName);
    }
    if(m_Port_X[21].functionSet == 0)
    {
        ui->coboxMainBoardIn1->addItem(m_Port_X[21].ResModifyName);
    }
    if(m_Port_X[22].functionSet == 0)
    {
        ui->coboxMainBoardIn1->addItem(m_Port_X[22].ResModifyName);
    }
    if(m_Port_X[23].functionSet == 0)
    {
        ui->coboxMainBoardIn1->addItem(m_Port_X[23].ResModifyName);
    }
    if(ui->coboxMainBoardIn1->count() == 0)
    {
        ui->chboxMainBoardIn1->setEnabled(false);
        ui->coboxMainBoardIn1->setEnabled(false);
    }
    else
    {
        ui->chboxMainBoardIn1->setEnabled(true);
        ui->coboxMainBoardIn1->setEnabled(true);
    }
    //输入IO第三组
    if(m_Port_X[24].functionSet == 0)
    {
        ui->coboxMainBoardIn2->addItem(m_Port_X[24].ResModifyName);
    }
    if(m_Port_X[25].functionSet == 0)
    {
        ui->coboxMainBoardIn2->addItem(m_Port_X[25].ResModifyName);
    }
    if(m_Port_X[26].functionSet == 0)
    {
        ui->coboxMainBoardIn2->addItem(m_Port_X[26].ResModifyName);
    }
    if(m_Port_X[27].functionSet == 0)
    {
        ui->coboxMainBoardIn2->addItem(m_Port_X[27].ResModifyName);
    }
    if(m_Port_X[28].functionSet == 0)
    {
        ui->coboxMainBoardIn2->addItem(m_Port_X[28].ResModifyName);
    }
    if(m_Port_X[29].functionSet == 0)
    {
        ui->coboxMainBoardIn2->addItem(m_Port_X[29].ResModifyName);
    }
    if(m_Port_X[30].functionSet == 0)
    {
        ui->coboxMainBoardIn2->addItem(m_Port_X[30].ResModifyName);
    }
    if(m_Port_X[31].functionSet == 0)
    {
        ui->coboxMainBoardIn2->addItem(m_Port_X[31].ResModifyName);
    }
    if(m_Port_X[32].functionSet == 0)
    {
        ui->coboxMainBoardIn2->addItem(m_Port_X[32].ResModifyName);
    }
    if(m_Port_X[33].functionSet == 0)
    {
        ui->coboxMainBoardIn2->addItem(m_Port_X[33].ResModifyName);
    }
    if(m_Port_X[34].functionSet == 0)
    {
        ui->coboxMainBoardIn2->addItem(m_Port_X[34].ResModifyName);
    }
    if(m_Port_X[35].functionSet == 0)
    {
        ui->coboxMainBoardIn2->addItem(m_Port_X[35].ResModifyName);
    }
    if(ui->coboxMainBoardIn2->count() == 0)
    {
        ui->chboxMainBoardIn2->setEnabled(false);
        ui->coboxMainBoardIn2->setEnabled(false);
    }
    else
    {
        ui->chboxMainBoardIn2->setEnabled(true);
        ui->coboxMainBoardIn2->setEnabled(true);
    }
    //输入IO第4组
    if(m_Port_X[36].functionSet == 0)
    {
        ui->coboxMainBoardInExtend0->addItem(m_Port_X[36].ResModifyName);
    }
    if(m_Port_X[37].functionSet == 0)
    {
        ui->coboxMainBoardInExtend0->addItem(m_Port_X[37].ResModifyName);
    }
    if(m_Port_X[38].functionSet == 0)
    {
        ui->coboxMainBoardInExtend0->addItem(m_Port_X[38].ResModifyName);
    }
    if(m_Port_X[39].functionSet == 0)
    {
        ui->coboxMainBoardInExtend0->addItem(m_Port_X[39].ResModifyName);
    }
    if(m_Port_X[40].functionSet == 0)
    {
        ui->coboxMainBoardInExtend0->addItem(m_Port_X[40].ResModifyName);
    }
    if(m_Port_X[41].functionSet == 0)
    {
        ui->coboxMainBoardInExtend0->addItem(m_Port_X[41].ResModifyName);
    }
    if(m_Port_X[42].functionSet == 0)
    {
        ui->coboxMainBoardInExtend0->addItem(m_Port_X[42].ResModifyName);
    }
    if(m_Port_X[43].functionSet == 0)
    {
        ui->coboxMainBoardInExtend0->addItem(m_Port_X[43].ResModifyName);
    }
    if(m_Port_X[44].functionSet == 0)
    {
        ui->coboxMainBoardInExtend0->addItem(m_Port_X[44].ResModifyName);
    }
    if(m_Port_X[45].functionSet == 0)
    {
        ui->coboxMainBoardIn2->addItem(m_Port_X[45].ResModifyName);
    }
    if(m_Port_X[34].functionSet == 0)
    {
        ui->coboxMainBoardInExtend0->addItem(m_Port_X[34].ResModifyName);
    }
    if(m_Port_X[46].functionSet == 0)
    {
        ui->coboxMainBoardInExtend0->addItem(m_Port_X[46].ResModifyName);
    }
    if(m_Port_X[47].functionSet == 0)
    {
        ui->coboxMainBoardInExtend0->addItem(m_Port_X[47].ResModifyName);
    }
    if(ui->coboxMainBoardInExtend0->count() == 0)
    {
        ui->coboxMainBoardInExtend0->setEnabled(false);
        ui->coboxMainBoardInExtend0->setEnabled(false);
    }
    else
    {
        ui->coboxMainBoardInExtend0->setEnabled(true);
        ui->coboxMainBoardInExtend0->setEnabled(true);
    }
    //输入IO第5组
    if(m_Port_X[48].functionSet == 0)
    {
        ui->coboxMainBoardInExtend1->addItem(m_Port_X[48].ResModifyName);
    }
    if(m_Port_X[49].functionSet == 0)
    {
        ui->coboxMainBoardInExtend1->addItem(m_Port_X[49].ResModifyName);
    }
    if(m_Port_X[50].functionSet == 0)
    {
        ui->coboxMainBoardInExtend1->addItem(m_Port_X[50].ResModifyName);
    }
    if(m_Port_X[51].functionSet == 0)
    {
        ui->coboxMainBoardInExtend1->addItem(m_Port_X[51].ResModifyName);
    }
    if(m_Port_X[52].functionSet == 0)
    {
        ui->coboxMainBoardInExtend1->addItem(m_Port_X[52].ResModifyName);
    }
    if(m_Port_X[53].functionSet == 0)
    {
        ui->coboxMainBoardInExtend1->addItem(m_Port_X[53].ResModifyName);
    }
    if(m_Port_X[54].functionSet == 0)
    {
        ui->coboxMainBoardInExtend1->addItem(m_Port_X[54].ResModifyName);
    }
    if(m_Port_X[55].functionSet == 0)
    {
        ui->coboxMainBoardInExtend1->addItem(m_Port_X[55].ResModifyName);
    }
    if(m_Port_X[56].functionSet == 0)
    {
        ui->coboxMainBoardInExtend1->addItem(m_Port_X[56].ResModifyName);
    }
    if(m_Port_X[57].functionSet == 0)
    {
        ui->coboxMainBoardInExtend1->addItem(m_Port_X[57].ResModifyName);
    }
    if(m_Port_X[58].functionSet == 0)
    {
        ui->coboxMainBoardInExtend1->addItem(m_Port_X[58].ResModifyName);
    }
    if(m_Port_X[59].functionSet == 0)
    {
        ui->coboxMainBoardInExtend1->addItem(m_Port_X[59].ResModifyName);
    }
    if(ui->coboxMainBoardInExtend1->count() == 0)
    {
        ui->coboxMainBoardInExtend1->setEnabled(false);
        ui->coboxMainBoardInExtend1->setEnabled(false);
    }
    else
    {
        ui->coboxMainBoardInExtend1->setEnabled(true);
        ui->coboxMainBoardInExtend1->setEnabled(true);
    }
}
//设置输出IO每个复选框可供选择项
void IOPortDialog::setchboxMainBoardOutItem()
{
    ui->coboxMainBoardOut0->clear();
    ui->coboxMainBoardOut1->clear();
    ui->coboxMainBoardOut2->clear();
    ui->coboxMainBoardOutExtend0->clear();
    ui->coboxMainBoardOutExtend1->clear();
    ui->coboxMainBoardOutExtend2->clear();
    //输入IO第一组
    if(m_OutportInterlock[0][1] == 0)
    {
        ui->coboxMainBoardOut0->addItem(m_Port_Y[0].ResModifyName);
    }
    if(m_OutportInterlock[0][2] == 0)
    {
        ui->coboxMainBoardOut0->addItem(m_Port_Y[1].ResModifyName);
    }
    if(m_OutportInterlock[1][0] == 0)
    {
        ui->coboxMainBoardOut0->addItem(m_Port_Y[2].ResModifyName);
    }
    if(m_OutportInterlock[1][2] == 0)
    {
        ui->coboxMainBoardOut0->addItem(m_Port_Y[3].ResModifyName);
    }
    if(m_OutportInterlock[2][0] == 0)
    {
        ui->coboxMainBoardOut0->addItem(m_Port_Y[4].ResModifyName);
    }
    if(m_OutportInterlock[2][2] == 0)
    {
        ui->coboxMainBoardOut0->addItem(m_Port_Y[5].ResModifyName);
    }
    if(m_SeniorFunc.autoLight == 0)
    {
        ui->coboxMainBoardOut0->addItem(m_Port_Y[6].ResModifyName);
    }
    if(m_SeniorFunc.alarmLight == 0)
    {
        ui->coboxMainBoardOut0->addItem(m_Port_Y[7].ResModifyName);
    }
    if(ui->coboxMainBoardOut0->count() == 0)
    {
        ui->chboxMainBoardOut0->setEnabled(false);
        ui->coboxMainBoardOut0->setEnabled(false);
    }
    else
    {
        ui->chboxMainBoardOut0->setEnabled(true);
        ui->coboxMainBoardOut0->setEnabled(true);
    }
    //输入IO第二组
    if(m_OutportInterlock[3][0] == 0)
    {
        ui->coboxMainBoardOut1->addItem(m_Port_Y[8].ResModifyName);
    }
    if(m_OutportInterlock[3][2] == 0)
    {
        ui->coboxMainBoardOut1->addItem(m_Port_Y[9].ResModifyName);
    }
    if(m_SeniorFunc.biowAir1 == 0)
    {
        ui->coboxMainBoardOut1->addItem(m_Port_Y[10].ResModifyName);
    }
    if(m_OutportInterlock[4][2] == 0)
    {
        ui->coboxMainBoardOut1->addItem(m_Port_Y[11].ResModifyName);
    }
    if(m_SeniorFunc.emergencyStopOut == 0)
    {
        ui->coboxMainBoardOut1->addItem(m_Port_Y[12].ResModifyName);
    }
    if(m_SeniorFunc.lubPump == 0)
    {
        ui->coboxMainBoardOut1->addItem(m_Port_Y[13].ResModifyName);
    }
    if(m_SeniorFunc.alarmBuzzer == 0)
    {
        ui->coboxMainBoardOut1->addItem(m_Port_Y[14].ResModifyName);
    }
    if(m_SeniorFunc.pauseLight == 0)
    {
        ui->coboxMainBoardOut1->addItem(m_Port_Y[15].ResModifyName);
    }
    if(ui->coboxMainBoardOut1->count() == 0)
    {
        ui->chboxMainBoardOut1->setEnabled(false);
        ui->coboxMainBoardOut1->setEnabled(false);
    }
    else
    {
        ui->chboxMainBoardOut1->setEnabled(true);
        ui->coboxMainBoardOut1->setEnabled(true);
    }
    //输入IO第三组
    if(m_SeniorFunc.startProduct1 == 0)
    {
        ui->coboxMainBoardOut2->addItem(m_Port_Y[16].ResModifyName);
    }
    if(m_SeniorFunc.mainAxisLocate1 == 0)
    {
        ui->coboxMainBoardOut2->addItem(m_Port_Y[17].ResModifyName);
    }
    if(m_SeniorFunc.processSave1 == 0)
    {
        ui->coboxMainBoardOut2->addItem(m_Port_Y[18].ResModifyName);
    }
    if(m_OutportInterlock[4][0] == 0)
    {
        ui->coboxMainBoardOut2->addItem(m_Port_Y[19].ResModifyName);
    }
    if(m_SeniorFunc.mainAxisRotate1 == 0)
    {
        ui->coboxMainBoardOut2->addItem(m_Port_Y[20].ResModifyName);
    }
    if(m_Port_Y[22].functionSet == 0)
    {
        ui->coboxMainBoardOut2->addItem(m_Port_Y[21].ResModifyName);
    }
    if(m_Port_Y[23].functionSet == 0)
    {
        ui->coboxMainBoardOut2->addItem(m_Port_Y[22].ResModifyName);
    }
    if(m_Port_X[24].functionSet == 0)
    {
        ui->coboxMainBoardOut2->addItem(m_Port_Y[23].ResModifyName);
    }
    if(ui->coboxMainBoardOut2->count() == 0)
    {
        ui->chboxMainBoardOut2->setEnabled(false);
        ui->coboxMainBoardOut2->setEnabled(false);
    }
    else
    {
        ui->chboxMainBoardOut2->setEnabled(true);
        ui->coboxMainBoardOut2->setEnabled(true);
    }
    //输入IO第4组
    if(m_OutportInterlock[6][0] == 0)
    {
        ui->coboxMainBoardOutExtend0->addItem(m_Port_Y[24].ResModifyName);
    }
    if(m_OutportInterlock[6][2] == 0)
    {
        ui->coboxMainBoardOutExtend0->addItem(m_Port_Y[25].ResModifyName);
    }
    if(m_OutportInterlock[7][0] == 0)
    {
        ui->coboxMainBoardOutExtend0->addItem(m_Port_Y[26].ResModifyName);
    }
    if(m_OutportInterlock[7][2] == 0)
    {
        ui->coboxMainBoardOutExtend0->addItem(m_Port_Y[27].ResModifyName);
    }
    if(m_OutportInterlock[8][0] == 0)
    {
        ui->coboxMainBoardOutExtend0->addItem(m_Port_Y[28].ResModifyName);
    }
    if(m_OutportInterlock[8][2] == 0)
    {
        ui->coboxMainBoardOutExtend0->addItem(m_Port_Y[29].ResModifyName);
    }
    if(m_SeniorFunc.mainAxisRotate2 == 0)
    {
        ui->coboxMainBoardOutExtend0->addItem(m_Port_Y[30].ResModifyName);
    }
    if(m_SeniorFunc.biowAir2 == 0)
    {
        ui->coboxMainBoardOutExtend0->addItem(m_Port_Y[31].ResModifyName);
    }
    if(ui->coboxMainBoardOutExtend0->count() == 0)
    {
        ui->chboxMainBoardOutExtend0->setEnabled(false);
        ui->coboxMainBoardOutExtend0->setEnabled(false);
    }
    else
    {
        ui->chboxMainBoardOutExtend0->setEnabled(true);
        ui->coboxMainBoardOutExtend0->setEnabled(true);
    }
    //输入IO第5组
    if(m_OutportInterlock[9][0] == 0)
    {
        ui->coboxMainBoardOutExtend1->addItem(m_Port_Y[32].ResModifyName);
    }
    if(m_OutportInterlock[9][2] == 0)
    {
        ui->coboxMainBoardOutExtend1->addItem(m_Port_Y[33].ResModifyName);
    }
    if(m_OutportInterlock[10][0] == 0)
    {
        ui->coboxMainBoardOutExtend1->addItem(m_Port_Y[34].ResModifyName);
    }
    if(m_OutportInterlock[10][2] == 0)
    {
        ui->coboxMainBoardOutExtend1->addItem(m_Port_Y[35].ResModifyName);
    }
    if(m_OutportInterlock[5][0] == 0)
    {//翻转台1正转/翻转暂时用在互锁界面1的预留
        ui->coboxMainBoardOutExtend1->addItem(m_Port_Y[36].ResModifyName);
    }
    if(m_OutportInterlock[5][1] == 0)
    {
        ui->coboxMainBoardOutExtend1->addItem(m_Port_Y[37].ResModifyName);
    }
    if(m_OutportInterlock[11][0] == 0)
    {//翻转台1夹紧/翻转暂时用在互锁界面2的预留
        ui->coboxMainBoardOutExtend1->addItem(m_Port_Y[38].ResModifyName);
    }
    if(m_OutportInterlock[11][2] == 0)
    {
        ui->coboxMainBoardOutExtend1->addItem(m_Port_Y[39].ResModifyName);
    }
    if(ui->coboxMainBoardOutExtend1->count() == 0)
    {
        ui->chboxMainBoardOutExtend1->setEnabled(false);
        ui->coboxMainBoardOutExtend1->setEnabled(false);
    }
    else
    {
        ui->chboxMainBoardOutExtend1->setEnabled(true);
        ui->coboxMainBoardOutExtend1->setEnabled(true);
    }
    //输入IO第6组
    if(m_SeniorFunc.startProduct2 == 0)
    {
        ui->coboxMainBoardOutExtend2->addItem(m_Port_Y[40].ResModifyName);
    }
    if(m_SeniorFunc.mainAxisLocate2 == 0)
    {
        ui->coboxMainBoardOutExtend2->addItem(m_Port_Y[41].ResModifyName);
    }
    if(m_SeniorFunc.processSave2 == 0)
    {
        ui->coboxMainBoardOutExtend2->addItem(m_Port_Y[42].ResModifyName);
    }
    if(m_Port_Y[43].functionSet == 0)
    {
        ui->coboxMainBoardOutExtend2->addItem(m_Port_Y[43].ResModifyName);
    }
    if(ui->coboxMainBoardOutExtend2->count() == 0)
    {
        ui->chboxMainBoardOutExtend2->setEnabled(false);
        ui->coboxMainBoardOutExtend2->setEnabled(false);
    }
    else
    {
        ui->chboxMainBoardOutExtend2->setEnabled(true);
        ui->coboxMainBoardOutExtend2->setEnabled(true);
    }
}

IOOnlineInEdit::IOOnlineInEdit(QWidget *parent)
    : QLineEdit(parent),
      port(0)
{

}

uint8_t IOOnlineInEdit::getCurrentPort() const
{
    return port;

}

void IOOnlineInEdit::setCurrentPort(uint8_t portIndex)
{
    port = portIndex;
}

void IOOnlineInEdit::mouseReleaseEvent(QMouseEvent *event)
{
    QLineEdit::mouseReleaseEvent(event);
    IOPortDialog dialog(nullptr, IOPortMode::IN,true);
    if (dialog.exec() == QDialog::Accepted)
    {
        QString text = dialog.getIOOnlineIn();
        port = static_cast<uint8_t>(dialog.getInputPort());
        this->setText(text);
    }
}

IOOnlineOutEdit::IOOnlineOutEdit(QWidget *parent)
    : QLineEdit(parent),
      port(0)
{

}

uint8_t IOOnlineOutEdit::getCurrentPort() const
{
    return port;
}

void IOOnlineOutEdit::setCurrentPort(uint8_t portIndex)
{
    port = portIndex;
}

void IOOnlineOutEdit::mouseReleaseEvent(QMouseEvent *event)
{
    QLineEdit::mouseReleaseEvent(event);
    IOPortDialog dialog(nullptr, IOPortMode::OUT,true);
    if (dialog.exec() == QDialog::Accepted)
    {
        QString text = dialog.getIOOnlineOut();
        port = static_cast<uint8_t>(dialog.getOutputPort());
        this->setText(text);
    }
}


IOInEdit::IOInEdit(QWidget *parent)
    : QLineEdit(parent),
      port(0)
{

}

uint8_t IOInEdit::getCurrentPort() const
{
    return port;

}

void IOInEdit::setCurrentPort(uint8_t portIndex)
{
    port = portIndex;
}

void IOInEdit::mouseReleaseEvent(QMouseEvent *event)
{
    QLineEdit::mouseReleaseEvent(event);
    IOPortDialog dialog(nullptr, IOPortMode::IN);
    if (dialog.exec() == QDialog::Accepted)
    {
        QString text = dialog.getIOOnlineIn();
        port = static_cast<uint8_t>(dialog.get_IOInEdit_InPort());
        this->setText(text);
    }
}

IOOutEdit::IOOutEdit(QWidget *parent)
    : QLineEdit(parent),
      port(0)
{

}

uint8_t IOOutEdit::getCurrentPort() const
{
    return port;
}

void IOOutEdit::setCurrentPort(uint8_t portIndex)
{
    port = portIndex;
}

void IOOutEdit::mouseReleaseEvent(QMouseEvent *event)
{
    QLineEdit::mouseReleaseEvent(event);
    IOPortDialog dialog(nullptr, IOPortMode::OUT);
    if (dialog.exec() == QDialog::Accepted)
    {
        QString text = dialog.getIOOnlineOut();
        port = static_cast<uint8_t>(dialog.get_IOInEdit_OutPort());
        this->setText(text);
    }
}


IOOnlineSearchStopEdit::IOOnlineSearchStopEdit(QWidget *parent)
    : QLineEdit(parent),
      port(0)
{

}

uint8_t IOOnlineSearchStopEdit::getCurrentPort() const
{
    return port;
}

uint8_t IOOnlineSearchStopEdit::getPortSetState() const
{
    return portState;
}

void IOOnlineSearchStopEdit::setCurrentPort(uint8_t portIndex)
{
    port = portIndex;
}

void IOOnlineSearchStopEdit::mouseReleaseEvent(QMouseEvent *event)
{
    QLineEdit::mouseReleaseEvent(event);
    IOPortDialog dialog(nullptr, IOPortMode::SearchIN,true);
    if (dialog.exec() == QDialog::Accepted)
    {
        QString text = dialog.getIOOnlineSearchStopIn();
        port = static_cast<uint8_t>(dialog.getSearchStopInPort());
        portState = static_cast<uint8_t>(dialog.getSearchStopPortState());
        this->setText(text);
    }
}
