SOURCES +=  \
    $$PWD/KeyButton.cpp \
    $$PWD/Keyboard.cpp \
    $$PWD/NumberKeyboard.cpp

HEADERS  += \
    $$PWD/AbstractKeyboard.h \
    $$PWD/KeyButton.h \
    $$PWD/Keyboard.h \
    $$PWD/NumberKeyboard.h

RESOURCES += $$PWD/Resources/Image.qrc

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
    RESOURCES += $$PWD/Resources/GoogleChineseLib.qrc
}

#contains(DEFINES, ENABLED_CHINESE_LIB) {
#    RESOURCES += $$PWD/Resources/ChineseLib.qrc
#}

#contains(DEFINES, ENABLED_CHINESE_PHRASE_LIB) {
#    RESOURCES += $$PWD/Resources/ChinesePhraseLib.qrc
#}

#contains(DEFINES, ENABLED_WQY_FONT) {
#    RESOURCES += $$PWD/Resources/Font.qrc
#}
