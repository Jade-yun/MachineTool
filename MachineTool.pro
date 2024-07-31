QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


CONFIG += c++11
DESTDIR = $$PWD/bin

#win32{
#DESTDIR = $$PWD/bin
#1
#}
#unix{
#DESTDIR = $$PWD/bin
#}

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# ensure this contains all dirPath you need, otherwise the latter will override former
INCLUDEPATH = Keyboard dialogs

SOURCES += \
    alarmform.cpp \
    autoform.cpp \
    main.cpp \
    mainwindow.cpp \
    manualform.cpp \
    monitorform.cpp \
    setting.cpp \
    teach.cpp \
    draggablebutton.cpp \
    teachmanage.cpp \
    dialogs/cleardialog.cpp \
    dialogs/stacksetdialog.cpp \
    dialogs/logindialog.cpp \
    dialogs/handwheeldialog.cpp \
    dialogs/keydefinedialog.cpp \
    dialogs/clearvardialog.cpp \
    dialogs/stackedit.cpp \
    dialogs/sigdefinedialog.cpp

HEADERS += \
    alarmform.h \
    autoform.h \
    mainwindow.h \
    manualform.h \
    monitorform.h \
    setting.h \
    teach.h \
    draggablebutton.h \
    teachmanage.h \
    dialogs/cleardialog.h \
    dialogs/stacksetdialog.h \
    dialogs/logindialog.h \
    dialogs/handwheeldialog.h \
    dialogs/keydefinedialog.h \
    dialogs/clearvardialog.h \
    dialogs/stackedit.h \
    dialogs/sigdefinedialog.h

FORMS += \
    alarmform.ui \
    autoform.ui \
    mainwindow.ui \
    manualform.ui \
    monitorform.ui \
    setting.ui \
    teach.ui \
    teachmanage.ui \
    dialogs/cleardialog.ui \
    dialogs/stacksetdialog.ui \
    dialogs/logindialog.ui \
    dialogs/handwheeldialog.ui \
    dialogs/keydefinedialog.ui \
    dialogs/referencepointdialog.ui \
    dialogs/autocorrectposdialog.ui \
    dialogs/clearvardialog.ui \
    dialogs/autocorrectposdialog.ui \
    dialogs/stacksetdialog.ui \
    dialogs/stackedit.ui \
    dialogs/sigdefinedialog.ui

SOURCES +=  \
    Keyboard/KeyButton.cpp \
    Keyboard/Keyboard.cpp \
    Keyboard/NumberKeyboard.cpp \
    customkeyboard.cpp \

HEADERS  += \
    Keyboard/AbstractKeyboard.h \
    Keyboard/KeyButton.h \
    Keyboard/Keyboard.h \
    Keyboard/NumberKeyboard.h \
    customkeyboard.h \

RESOURCES += Resources/Image.qrc

# 可启用下列宏
# 启用汉字库
# ENABLED_CHINESE_LIB

# 启用词组汉字库
# ENABLED_CHINESE_PHRASE_LIB

# 启用谷歌汉字库(推荐使用)
# ENABLED_GOOGLE_CHINESE_LIB

# 启用文泉驿字体库
# ENABLED_WQY_FONT

DEFINES += ENABLED_GOOGLE_CHINESE_LIB

contains(DEFINES, ENABLED_GOOGLE_CHINESE_LIB) {
    RESOURCES += Resources/GoogleChineseLib.qrc
}

#contains(DEFINES, ENABLED_CHINESE_LIB) {
#    RESOURCES += Resources/ChineseLib.qrc
#}

#contains(DEFINES, ENABLED_CHINESE_PHRASE_LIB) {
#    RESOURCES += Resources/ChinesePhraseLib.qrc
#}

#contains(DEFINES, ENABLED_WQY_FONT) {
#    RESOURCES += Resources/Font.qrc
#}


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    images.qrc \
    stylesheets.qrc

DISTFILES +=
