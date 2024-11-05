#ifndef PROGRAM_POPUP_H
#define PROGRAM_POPUP_H

#include <QString>
#include <QObject>
/********************程序弹框**************************/

#define PROGRAM_TEXT_EMPTY      1           //程序文本不能为空
#define PROGRAM_FILENAME_EXIST  2           //程序文件名已存在
#define PROGRAM_OPEN_WRITE   3           //无法打开文件进行写入

//串口弹框
#define SERIAL_NOT_OPEN     101             //串口未打开

extern QString m_ProgramChange;             //切换程序弹框

#endif // PROGRAM_POPUP_H
