# 这里都是手写的 组件

HEADERS += \
    chatview.h \	# 整个聊天消息显示窗口， ChatPage.ui 中的 消息显示 部分 Widget，这部分是手写的。
    bubbleframe.h \	# 聊天气泡基类
    textbubble.h \	# 文本 聊天气泡
    picturebubble.h	# 图像 聊天气泡

SOURCES += \
    chatview.cpp \
    bubbleframe.cpp \
    textbubble.cpp \
    picturebubble.cpp
