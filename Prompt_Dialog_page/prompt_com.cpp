#include "prompt_com.h"

Prompt_Com::Prompt_Com()
{

}

uint8_t LCD_NonButton_Num = 0;       //无按钮提示界面
uint8_t LCD_SingleButton_Num = 0;    //单按钮提示页面
uint8_t LCD_DoubleButton_Num = 0;    //双按键提示页面

//双按键界面显示文本
uint8_t  Double_Button_Text[12][64]= {
        {""},	//0
        {"你是否确定删除这条指令？"},		//1
};

/*************************************************************************
**  函数名：  Return_DoubleButtonDisplay_string()
**	输入参数：num-显示内容编号
**	输出参数：无
**	函数功能：双按键确定提示框显示函数，返回显示内容
**  作者：    wukui
**  开发日期：2024/8/5
**************************************************************************/
QString Return_DoubleButtonDisplay_string(uint8_t num)
{
    LCD_DoubleButton_Num = num;
    QString display_String;
    display_String += QString().asprintf("%s",&Double_Button_Text[num]);

    return display_String;
}

