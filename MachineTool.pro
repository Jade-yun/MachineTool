QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


CONFIG += c++11
DESTDIR = $$PWD/bin/

contains(CONFIG, desktop) {
    DESTDIR = $$PWD/bin/desktop
} else: contains(CONFIG, embedded) {
    DESTDIR = $$PWD/bin/embedded
}

# print value of DESTDIR
#message(DESTDIR is $$DESTDIR)


# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
#DEFINES += QT_NO_DEBUG_OUTPUT

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# ensure this contains all dirPath you need, otherwise the latter will override former
INCLUDEPATH += Keyboard dialogs method Driver MyTitleBarDialog

include($$PWD/method/method.pri)
include($$PWD/Keyboard/keyboard.pri)

SOURCES += \
    Driver/backlighter.cpp \
    Driver/beeper.cpp \
    Driver/eventscanner.cpp \
    Driver/globaleventfilter.cpp \
    Driver/timesetter.cpp \
    Driver/usbdisk.cpp \
    MyTitleBarDialog/basewindow.cpp \
    MyTitleBarDialog/mytitlebar.cpp \
    Prompt_Dialog_page/prompt_com.cpp \
    Prompt_Dialog_page/prompt_dialog_page.cpp \
    Prompt_Dialog_page/prompt_yes_no_dialog_page.cpp \
    Prompt_Dialog_page/yes_no_page.cpp \
    alarmform.cpp \
    autoform.cpp \
    dialogs/file_preview.cpp \
    dialogs/softkeywidget.cpp \
    dialogs/tippasswddialog.cpp \
    dialogs/upgradedialog.cpp \
    main.cpp \
    mainwindow.cpp \
    manualform.cpp \
    monitorform.cpp \
    savebutton.cpp \
    setting.cpp \
    teach.cpp \
    draggablebutton.cpp \
    teachmanage.cpp \
    customedit.cpp \
    customkeyboard.cpp \
    dialogs/cleardialog.cpp \
    dialogs/stacksetdialog.cpp \
    dialogs/logindialog.cpp \
    dialogs/handwheeldialog.cpp \
    dialogs/keydefinedialog.cpp \
    dialogs/clearvardialog.cpp \
    dialogs/stackedit.cpp \
    dialogs/sigdefinedialog.cpp \
    dialogs/autocorrectposdialog.cpp \
    dialogs/errortipdialog.cpp \
    dialogs/referpointdialog.cpp \
    dialogs/ifprogramdialog.cpp \
    dialogs/ioportdialog.cpp \
    dialogs/vartypedialog.cpp \
    statebutton.cpp \
    referencewidget.cpp

HEADERS += \
    Driver/backlighter.h \
    Driver/beeper.h \
    Driver/eventscanner.h \
    Driver/globaleventfilter.h \
    Driver/timesetter.h \
    Driver/usbdisk.h \
    MyTitleBarDialog/basewindow.h \
    MyTitleBarDialog/mytitlebar.h \
    Prompt_Dialog_page/prompt_com.h \
    Prompt_Dialog_page/prompt_dialog_page.h \
    Prompt_Dialog_page/prompt_yes_no_dialog_page.h \
    Prompt_Dialog_page/yes_no_page.h \
    alarmform.h \
    autoform.h \
    dialogs/file_preview.h \
    dialogs/softkeywidget.h \
    dialogs/tippasswddialog.h \
    dialogs/upgradedialog.h \
    mainwindow.h \
    manualform.h \
    monitorform.h \
    savebutton.h \
    setting.h \
    teach.h \
    draggablebutton.h \
    teachmanage.h \
    customedit.h \
    customkeyboard.h \
    dialogs/cleardialog.h \
    dialogs/stacksetdialog.h \
    dialogs/logindialog.h \
    dialogs/handwheeldialog.h \
    dialogs/keydefinedialog.h \
    dialogs/clearvardialog.h \
    dialogs/stackedit.h \
    dialogs/sigdefinedialog.h \
    dialogs/autocorrectposdialog.h \
    dialogs/errortipdialog.h \
    dialogs/referpointdialog.h \
    dialogs/ifprogramdialog.h \
    dialogs/ioportdialog.h \
    dialogs/vartypedialog.h \
    statebutton.h \
    referencewidget.h \

FORMS += \
    Prompt_Dialog_page/prompt_dialog_page.ui \
    Prompt_Dialog_page/prompt_yes_no_dialog_page.ui \
    Prompt_Dialog_page/yes_no_page.ui \
    alarmform.ui \
    autoform.ui \
    dialogs/file_preview.ui \
    dialogs/softkeywidget.ui \
    dialogs/tippasswddialog.ui \
    dialogs/upgradedialog.ui \
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
    dialogs/clearvardialog.ui \
    dialogs/stackedit.ui \
    dialogs/sigdefinedialog.ui \
    dialogs/autocorrectposdialog.ui \
    dialogs/errortipdialog.ui \
    dialogs/referpointdialog.ui \
    dialogs/ifprogramdialog.ui \
    dialogs/ioportdialog.ui \
    dialogs/vartypedialog.ui


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    MyTitleDialog.qrc \
    images.qrc \
    stylesheets.qrc

DISTFILES += \
    Keyboard/keyboard.pri \
    MyTitleBarDialog/Resources/MyTitle.css \
    MyTitleBarDialog/Resources/MyTitle/close.png \
    MyTitleBarDialog/Resources/MyTitle/max.png \
    MyTitleBarDialog/Resources/MyTitle/min.png \
    MyTitleBarDialog/Resources/MyTitle/restore.png \
    MyTitleBarDialog/Resources/titleicon.png \
    images/gif/XYZ.gif \
    images/gif/YXZ.gif \
    images/gif/ZXY.gif \
    images/gif/ZYX.gif \
    images/referPoint.png

