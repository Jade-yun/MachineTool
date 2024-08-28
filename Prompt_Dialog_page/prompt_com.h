#ifndef PROMPT_COM_H
#define PROMPT_COM_H
#include "qstring.h"

class Prompt_Com
{
public:
    Prompt_Com();
};

//双按键弹窗页面
#define DOUBLE_BUTTON_00     		0	  //
#define DOUBLE_BUTTON_01         	1	  //你是否确定删除这条指令？


extern uint8_t LCD_NonButton_Num;       //无按钮提示界面
extern uint8_t LCD_SingleButton_Num;    //单按钮提示页面
extern uint8_t LCD_DoubleButton_Num;    //双按键提示页面

extern QString Return_DoubleButtonDisplay_string(uint8_t num);
#endif // PROMPT_COM_H
