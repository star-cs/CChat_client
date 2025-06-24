# 都是 QListWidget 的自定义控件

HEADERS += \
    chatuserlist.h \	    # 聊天消息条目 列表的 滑动 发送 更新数据添加信号
    searchlist.h  \	    # 搜索列表
    contactuserlist.h \	    # 联系人列表
    $$PWD/applyfriendlist.h # 联系人最右侧的 好友申请列表

SOURCES += \
    chatuserlist.cpp \
    searchlist.cpp \
    contactuserlist.cpp \
    $$PWD/applyfriendlist.cpp
