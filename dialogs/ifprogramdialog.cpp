#include "ifprogramdialog.h"
#include "ui_ifprogramdialog.h"
#include "cmd.h"

IfProgramDialog::IfProgramDialog(QWidget *parent) :
    BaseWindow(parent),
    ui(new Ui::IfProgramDialog)
{
    ui->setupUi(this);
    this->setModal(true);
    chboxsSigMb = new QButtonGroup(this);
    for (auto chbox : ui->tabMainBoard->findChildren<QCheckBox*>())
    {
        chboxsSigMb->addButton(chbox);
    }
    chboxsSigMb->setExclusive(true);

    QButtonGroup* chboxsVarPreOp = new QButtonGroup(this);
    chboxsVarPreOp->addButton(ui->chbox1Var);
    chboxsVarPreOp->addButton(ui->chbox2Var);
    chboxsVarPreOp->addButton(ui->chbox3Var);
    chboxsVarPreOp->addButton(ui->chbox4Var);
    chboxsVarPreOp->setExclusive(true);

    QButtonGroup* chboxsVarPostOp1 = new QButtonGroup(this);
    chboxsVarPostOp1->addButton(ui->chboxConstantVar);
    chboxsVarPostOp1->addButton(ui->chboxVariableVar);
    chboxsVarPostOp1->addButton(ui->chboxRealProd);
    chboxsVarPostOp1->setExclusive(true);

    QButtonGroup* chboxsVarPostOp2 = new QButtonGroup(this);
    chboxsVarPostOp2->addButton(ui->chboxConstantRealProd);
    chboxsVarPostOp2->addButton(ui->chboxVariableRealProd);
    chboxsVarPostOp2->setExclusive(true);

    QButtonGroup* chboxsVarPostOp3 = new QButtonGroup(this);
    chboxsVarPostOp3->addButton(ui->chboxConstantStack);
    chboxsVarPostOp3->addButton(ui->chboxStack);
    chboxsVarPostOp3->setExclusive(true);

    connect(ui->btnOK, &QPushButton::clicked, this, &IfProgramDialog::accept);
    connect(ui->btnCancel, &QPushButton::clicked, this, &IfProgramDialog::reject);

    ui->chboxConstantVar->clicked(true);
    ui->chboxConstantRealProd->clicked(true);
    ui->chboxConstantStack->clicked(true);

    ui->chboxConstantPos->clicked(true);
    ui->chboxConstantTimer->clicked(true);

    ui->chboxConstantVar->setChecked(true);
    ui->chboxConstantRealProd->setChecked(true);
    ui->chboxConstantStack->setChecked(true);

    ui->chboxConstantPos->setChecked(true);
    ui->chboxConstantTimer->setChecked(true);

    for (NumberEdit* edit : findChildren<NumberEdit*>())
    {
        if (edit->text() == "0")
        {
            edit->setDecimalPlaces(0);
        }
        else if (edit->text() == "0.00")
            edit->setDecimalPlaces(2);
    }
    ui->editVarPostOp->setDecimalPlaces(m_VariableType[ui->coboxVarSelectPreOp->currentIndex()]);//根据变量的类型设置输入控件的小数位数
    connect(ui->coboxVarSelectPreOp,QOverload<int>::of(&QComboBox::currentIndexChanged),this,[=](){
        ui->editVarPostOp->setDecimalPlaces(m_VariableType[ui->coboxVarSelectPreOp->currentIndex()]);});

    for (auto cobox : findChildren<QComboBox*>())
    {
        cobox->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    }
}

IfProgramDialog::~IfProgramDialog()
{
    delete ui;
}


void IfProgramDialog::on_chboxConstantVar_clicked()
{
    ui->editVarPostOp->show();
    ui->coboxVarSelectPostOp1->hide();
    ui->coboxVarRealProductNumPostOp->hide();
}

void IfProgramDialog::on_chboxVariableVar_clicked()
{
    ui->editVarPostOp->hide();
    ui->coboxVarSelectPostOp1->show();
    ui->coboxVarRealProductNumPostOp->hide();
}

void IfProgramDialog::on_chboxRealProd_clicked()
{
    ui->editVarPostOp->hide();
    ui->coboxVarSelectPostOp1->hide();
    ui->coboxVarRealProductNumPostOp->show();
}

void IfProgramDialog::on_chboxConstantRealProd_clicked()
{
    ui->editRealProdNumPostOp->show();
    ui->coboxVarSelectPostOp2->hide();
}

void IfProgramDialog::on_chboxVariableRealProd_clicked()
{
    ui->editRealProdNumPostOp->hide();
    ui->coboxVarSelectPostOp2->show();
}

void IfProgramDialog::on_chboxConstantStack_clicked()
{
    ui->editStackPostOp->show();
    ui->coboxVarStackPostOp->hide();
}

void IfProgramDialog::on_chboxStack_clicked()
{
    ui->editStackPostOp->hide();
    ui->coboxVarStackPostOp->show();
}

void IfProgramDialog::on_chboxConstantTimer_clicked()
{
    ui->editTimerPostOp->show();
    ui->labTimerUnit->show();
    ui->coboxTimerPostOp->hide();

}

void IfProgramDialog::on_chboxTimerTimer_clicked()
{
    ui->editTimerPostOp->hide();
    ui->labTimerUnit->hide();
    ui->coboxTimerPostOp->show();

}

void IfProgramDialog::on_chboxConstantPos_clicked()
{
    ui->editPosPostOp->show();
    ui->labPosUnit->show();
    ui->coboxPosVarSelectPostOp->hide();
    ui->coboxPosPostOpAxisSelect->hide();
}

void IfProgramDialog::on_chboxVariablePos_clicked()
{
    ui->editPosPostOp->hide();
    ui->labPosUnit->hide();
    ui->coboxPosVarSelectPostOp->show();
    ui->coboxPosPostOpAxisSelect->hide();
}

void IfProgramDialog::on_chboxAxis_clicked()
{
    ui->editPosPostOp->hide();
    ui->labPosUnit->hide();
    ui->coboxPosVarSelectPostOp->hide();
    ui->coboxPosPostOpAxisSelect->show();
}

QString IfProgramDialog::getMainBoardSig() const
{
    QCheckBox* checkBoxes[] = {
        ui->chboxMainBoardIn0,
        ui->chboxMainBoardIn1,
        ui->chboxMainBoardIn2,
        ui->chboxMainBoardInExtend0,
        ui->chboxMainBoardInExtend1,
        ui->chboxMainBoardOut0,
        ui->chboxMainBoardOut1,
        ui->chboxMainBoardOut2,
        ui->chboxMainBoardOutExtend0,
        ui->chboxMainBoardOutExtend1,
        ui->chboxMainBoardOutExtend2
    };
    QComboBox* comboBoxes[] = {
        ui->coboxMainBoardIn0,
        ui->coboxMainBoardIn1,
        ui->coboxMainBoardIn2,
        ui->coboxMainBoardInExtend0,
        ui->coboxMainBoardInExtend1,
        ui->coboxMainBoardOut0,
        ui->coboxMainBoardOut1,
        ui->coboxMainBoardOut2,
        ui->coboxMainBoardOutExtend0,
        ui->coboxMainBoardOutExtend1,
        ui->coboxMainBoardOutExtend2
    };

    QPushButton* buttons[] = {
        ui->btnMainBoardIn0,
        ui->btnMainBoardIn1,
        ui->btnMainBoardIn2,
        ui->btnMainBoardInExtend0,
        ui->btnMainBoardInExtend1,
        ui->btnMainBoardOut0,
        ui->btnMainBoardOut1,
        ui->btnMainBoardOut2,
        ui->btnMainBoardOutExtend0,
        ui->btnMainBoardOutExtend1,
        ui->btnMainBoardOutExtend2
    };

    const int count = sizeof(checkBoxes) / sizeof(QCheckBox*); // Number of elements in arrays

    for (int i = 0; i < count; ++i)
    {
        if (checkBoxes[i]->isChecked())
        {
            return comboBoxes[i]->currentText() + buttons[i]->text();
        }
    }

    return QString(); // Default return if no checkbox is checked
}

QString IfProgramDialog::getVarOperateExpression() const
{
    if (ui->chbox1Var->isChecked())
    {
        QString expression = ui->coboxVarSelectPreOp->currentText() + ui->coboxVarOperand1->currentText();
        if (ui->chboxConstantVar->isChecked())
        {
            expression += ui->editVarPostOp->text();
        }
        else if (ui->chboxVariableVar->isChecked())
        {
            expression += ui->coboxVarSelectPostOp1->currentText();
        }
        else {
            expression += ui->coboxVarRealProductNumPostOp->currentText();
        }
        return expression;
    }
    if (ui->chbox2Var->isChecked())
    {
        QString expression = ui->coboxVarRealProductNumPreOp->currentText() + ui->coboxVarOperand2->currentText();
        if (ui->chboxConstantRealProd->isChecked())
        {
            expression += ui->editRealProdNumPostOp->text();
        }
        else
        {
            expression += ui->coboxVarSelectPostOp2->currentText();
        }
        return expression;
    }
    if (ui->chbox3Var->isChecked())
    {
        QString expression = ui->coboxVarStackPreOp1->currentText() + ui->coboxVarOperand3->currentText();
        if (ui->chboxConstantStack->isChecked())
        {
            expression += ui->editStackPostOp->text();
        }
        else
        {
            expression += ui->coboxVarStackPostOp->currentText();
        }
        return expression;
    }
    if (ui->chbox4Var->isChecked())
    {
        QString expression = ui->coboxVarStackPreOp2->currentText() + ui->coboxVarOperand4->currentText();
        expression += ui->coboxVarFinisStack->currentText();
        return expression;
    }

    return QString();
}

QString IfProgramDialog::getPosExpression() const
{
    QString expression = ui->coboxPosPreOpAxisSelect->currentText() + ui->coboxOperandPos->currentText();
    if (ui->chboxConstantPos->isChecked())
    {
        QString postOpText = ui->editPosPostOp->text() + "mm";
        expression += postOpText;
    }
    else if (ui->chboxVariablePos->isChecked())
    {
        expression += ui->coboxPosVarSelectPostOp->currentText();
    }
    else
    {
        expression += ui->coboxPosPostOpAxisSelect->currentText();
    }

    return expression;
}

QString IfProgramDialog::getTimerExpression() const
{
    QString expression = ui->coboxTimerPreOp->currentText() + ui->coboxTimerOperand->currentText();
    if (ui->chboxConstantTimer->isChecked())
    {
        QString postOpText = ui->editTimerPostOp->text() + "s";
        expression += postOpText;
    }
    else {
        expression += ui->coboxTimerPostOp->currentText();
    }
    return expression;
}

QString IfProgramDialog::getConditionExpression() const
{
    if (ui->tabWidget->currentIndex() == 0)
    {
        return getMainBoardSig();
    }
    else if (ui->tabWidget->currentIndex() == 1)
    {
        return getVarOperateExpression();
    }
    else if (ui->tabWidget->currentIndex() == 2)
    {
        return getPosExpression();
    }
    else
    {
        return getTimerExpression();
    }
}
/*************************************************************************
**  函数名：  SaveLogicIfStruct()
**	输入参数：condindex是条件1/条件2索引
**	输出参数：
**	函数功能：保存条件结构体参数处理函数，
**  作者：    wukui
**  开发日期：2024/9/3
**************************************************************************/
void IfProgramDialog::SaveLogicIfStruct(uint8_t CondIndex)
{
    uint8_t tabwidgetindex = 0;
    tabwidgetindex = ui->tabWidget->currentIndex();
    setLogicStructPar(tabwidgetindex,CondIndex);
}
/*************************************************************************
**  函数名：  setLogicStructPar()
**	输入参数：page_index:弹出界面索引 CondIndex是条件1/条件2索引
**	输出参数：
**	函数功能：条件结构体参数赋值
**  作者：    wukui
**  开发日期：2024/9/3
**************************************************************************/
void IfProgramDialog::setLogicStructPar(uint8_t page_index,uint8_t CondIndex)
{
    switch(page_index)
    {
    case 0://信号
    {
        QCheckBox* checkBoxes[] = {
            ui->chboxMainBoardIn0,
            ui->chboxMainBoardIn1,
            ui->chboxMainBoardIn2,
            ui->chboxMainBoardInExtend0,
            ui->chboxMainBoardInExtend1,
            ui->chboxMainBoardOut0,
            ui->chboxMainBoardOut1,
            ui->chboxMainBoardOut2,
            ui->chboxMainBoardOutExtend0,
            ui->chboxMainBoardOutExtend1,
            ui->chboxMainBoardOutExtend2
        };
        QComboBox* comboBoxes[] = {
            ui->coboxMainBoardIn0,
            ui->coboxMainBoardIn1,
            ui->coboxMainBoardIn2,
            ui->coboxMainBoardInExtend0,
            ui->coboxMainBoardInExtend1,
            ui->coboxMainBoardOut0,
            ui->coboxMainBoardOut1,
            ui->coboxMainBoardOut2,
            ui->coboxMainBoardOutExtend0,
            ui->coboxMainBoardOutExtend1,
            ui->coboxMainBoardOutExtend2
        };

        StateButton* buttons[] = {
            ui->btnMainBoardIn0,
            ui->btnMainBoardIn1,
            ui->btnMainBoardIn2,
            ui->btnMainBoardInExtend0,
            ui->btnMainBoardInExtend1,
            ui->btnMainBoardOut0,
            ui->btnMainBoardOut1,
            ui->btnMainBoardOut2,
            ui->btnMainBoardOutExtend0,
            ui->btnMainBoardOutExtend1,
            ui->btnMainBoardOutExtend2
        };

        const int count = sizeof(checkBoxes) / sizeof(QCheckBox*); // Number of elements in arrays

        for (int i = 0; i < count; ++i)
        {
            if (checkBoxes[i]->isChecked())
            {

                if(i<5)
                {
                    Temp_LogicIfStruct.cmpType[CondIndex] = 0;
                    Temp_LogicIfStruct.inportNum[CondIndex] = comboBoxes[i]->currentIndex()+1+i*12;//输入信号端口号
                    Temp_LogicIfStruct.inportType[CondIndex] = buttons[i]->getState();//输入信号有效类型
                }
                else
                {
                    Temp_LogicIfStruct.cmpType[CondIndex] = 103;
                    Temp_LogicIfStruct.inportNum[CondIndex] = comboBoxes[i]->currentIndex()+1+(i-5)*8;//输出信号端口号
                    Temp_LogicIfStruct.inportType[CondIndex] = buttons[i]->getState();//输出信号有效类型
                }
            }
        }
        break;
    }
    case 1://变量
    {
        if (ui->chbox1Var->isChecked())
        {
            if (ui->chboxConstantVar->isChecked())
            {
                Temp_LogicIfStruct.cmpType[CondIndex] = ui->coboxVarSelectPreOp->currentIndex()+1;//比较类型-变量1-20
                Temp_LogicIfStruct.cmpMode[CondIndex] = ui->coboxVarOperand1->currentIndex();//比较方式
                Temp_LogicIfStruct.sufferCmpType[CondIndex] = 0;//被比较对象类型-常量
                int32_t sufferOperValue = 0;
                if(m_VariableType[ui->coboxVarSelectPreOp->currentIndex()]==0)
                {
                    sufferOperValue = ui->editVarPostOp->text().toInt();
                }
                else if(m_VariableType[ui->coboxVarSelectPreOp->currentIndex()]==1)
                {
                    sufferOperValue = ui->editVarPostOp->text().toDouble()*10;
                }
                else if(m_VariableType[ui->coboxVarSelectPreOp->currentIndex()]==2)
                {
                    sufferOperValue = ui->editVarPostOp->text().toDouble()*100;
                }
                Temp_LogicIfStruct.sufferCmpValue[CondIndex] = sufferOperValue;
            }
            else if (ui->chboxVariableVar->isChecked())
            {
                Temp_LogicIfStruct.cmpType[CondIndex] = ui->coboxVarSelectPreOp->currentIndex()+1;//比较类型-变量1-20
                Temp_LogicIfStruct.cmpMode[CondIndex] = ui->coboxVarOperand1->currentIndex();//比较方式
                Temp_LogicIfStruct.sufferCmpType[CondIndex] = ui->coboxVarSelectPostOp1->currentIndex()+1;//被比较对象类型-变量1-20
            }
            else {
                Temp_LogicIfStruct.cmpType[CondIndex] = ui->coboxVarSelectPreOp->currentIndex()+1;//比较类型-变量1-20
                Temp_LogicIfStruct.cmpMode[CondIndex] = ui->coboxVarOperand1->currentIndex();//比较方式
                Temp_LogicIfStruct.sufferCmpType[CondIndex] = 102;//被比较对象类型-102实际产量
            }
        }
        if (ui->chbox2Var->isChecked())
        {
            QString expression = ui->coboxVarRealProductNumPreOp->currentText() + ui->coboxVarOperand2->currentText();
            if (ui->chboxConstantRealProd->isChecked())
            {
                Temp_LogicIfStruct.cmpType[CondIndex] = 102;  //比较对象类型-实际产量
                Temp_LogicIfStruct.cmpMode[CondIndex] = ui->coboxVarOperand2->currentIndex();//比较方式
                Temp_LogicIfStruct.sufferCmpType[CondIndex] = 0;//被比较对象类型-常量
                Temp_LogicIfStruct.sufferCmpValue[CondIndex] = ui->editRealProdNumPostOp->text().toInt();//被比较对象值
            }
            else
            {
                Temp_LogicIfStruct.cmpType[CondIndex] = 102;  //比较对象类型-实际产量
                Temp_LogicIfStruct.cmpMode[CondIndex] = ui->coboxVarOperand2->currentIndex();//比较方式
                Temp_LogicIfStruct.sufferCmpType[CondIndex] = ui->coboxVarSelectPostOp2->currentIndex()+1;//被比较对象类型-变量1-20
            }
        }
        if (ui->chbox3Var->isChecked())
        {
            if (ui->chboxConstantStack->isChecked())
            {
                Temp_LogicIfStruct.cmpType[CondIndex] = 21+ui->coboxVarStackPreOp1->currentIndex();//比较对象类型 21-40堆叠组方式1
                Temp_LogicIfStruct.cmpMode[CondIndex] = ui->coboxVarOperand3->currentIndex();//比较方式
                Temp_LogicIfStruct.sufferCmpType[CondIndex] = 0;//被比较对象类型-常量
                Temp_LogicIfStruct.sufferCmpValue[CondIndex] = ui->editStackPostOp->text().toUInt();//被比较对象值
            }
            else
            {
                Temp_LogicIfStruct.cmpType[CondIndex] = 21+ui->coboxVarStackPreOp1->currentIndex();//比较对象类型 21-40堆叠组方式1
                Temp_LogicIfStruct.cmpMode[CondIndex] = ui->coboxVarOperand3->currentIndex();//比较方式
                Temp_LogicIfStruct.sufferCmpType[CondIndex] = ui->coboxVarStackPostOp->currentIndex()+21;//被比较对象类型-21-40堆叠组方式1
            }
        }
        if (ui->chbox4Var->isChecked())
        {
            Temp_LogicIfStruct.cmpType[CondIndex] = 41+ui->coboxVarStackPreOp2->currentIndex();//比较对象类型 41-60堆叠组方式2
            Temp_LogicIfStruct.cmpMode[CondIndex] = 4+ui->coboxVarOperand4->currentIndex();//比较方式,只有==和!=
            Temp_LogicIfStruct.sufferCmpType[CondIndex] = 103;//被比较对象类型-堆叠完成
        }

        break;
    }
    case 2://位置
    {
        if (ui->chboxConstantPos->isChecked())
        {
            Temp_LogicIfStruct.cmpType[CondIndex] = 61+ui->coboxPosPreOpAxisSelect->currentIndex();//比较对象类型-61-80轴
            Temp_LogicIfStruct.cmpMode[CondIndex] = ui->coboxOperandPos->currentIndex();//比较方式
            Temp_LogicIfStruct.sufferCmpType[CondIndex] = 0;//被比较对象类型-常量
            Temp_LogicIfStruct.sufferCmpValue[CondIndex] = ui->editVarPostOp->text().toDouble()*100;//被比较对象值
        }
        else if (ui->chboxVariablePos->isChecked())
        {
            Temp_LogicIfStruct.cmpType[CondIndex] = 61+ui->coboxPosPreOpAxisSelect->currentIndex();//比较对象类型-61-80轴
            Temp_LogicIfStruct.cmpMode[CondIndex] = ui->coboxOperandPos->currentIndex();//比较方式
            Temp_LogicIfStruct.sufferCmpType[CondIndex] = ui->coboxPosVarSelectPostOp->currentIndex()+1;//被比较对象类型-变量1-20
        }
        else
        {
            Temp_LogicIfStruct.cmpType[CondIndex] = 61+ui->coboxPosPreOpAxisSelect->currentIndex();//比较对象类型-61-80轴
            Temp_LogicIfStruct.cmpMode[CondIndex] = ui->coboxOperandPos->currentIndex();//比较方式
            Temp_LogicIfStruct.sufferCmpType[CondIndex] = 61+ui->coboxPosPostOpAxisSelect->currentIndex();//比较对象类型-61-80轴
        }

        break;
    }
    case 3://定时器
    {
        QString expression = ui->coboxTimerPreOp->currentText() + ui->coboxTimerOperand->currentText();
        if (ui->chboxConstantTimer->isChecked())
        {
            Temp_LogicIfStruct.cmpType[CondIndex] = 81+ui->coboxTimerPreOp->currentIndex();//比较对象类型-81-100定时器
            Temp_LogicIfStruct.cmpMode[CondIndex] = ui->coboxTimerOperand->currentIndex();//比较方式
            Temp_LogicIfStruct.sufferCmpType[CondIndex] = 0;//被比较对象类型-常量
            Temp_LogicIfStruct.sufferCmpValue[CondIndex] = ui->editTimerPostOp->text().toDouble()*100;//被比较对象值
        }
        else {
            Temp_LogicIfStruct.cmpType[CondIndex] = 81+ui->coboxTimerPreOp->currentIndex();//比较对象类型-81-100定时器
            Temp_LogicIfStruct.cmpMode[CondIndex] = ui->coboxTimerOperand->currentIndex();//比较方式
            Temp_LogicIfStruct.sufferCmpType[CondIndex] = 81+ui->coboxTimerPostOp->currentIndex();//被比较对象类型-81-100定时器
        }
        break;
    }
    default:
        break;
    }
}
