SOURCES += \
    mainwindow.cpp \	    # 主窗口
    logindialog.cpp \	    # 登录窗口
    registerdialog.cpp \    # 注册窗口
    chatdialog.cpp \	    # 用户操作 主窗口，分为左侧，中间，右侧 水平布局
    resetdialog.cpp \	    # 重置密码窗口
    chatuserwid.cpp \	    # 主窗口中间的 聊天记录 消息 的 小widget，继承 ListItemBase
    adduseritem.cpp \	    # 主窗口中间的 搜索 小widget，继承 ListItemBase
    chatpage.cpp \	    # 用户操作主窗口 最右侧 聊天操作主窗口
    loadingdlg.cpp \	    # 加载等待窗口
    findsuccessdlg.cpp \    # 查找用户成功 窗口界面
    applyfriend.cpp \	    # 好友申请界面
    friendlabel.cpp \	    # 好友申请界面里 标记 好友的 小标签
    $$PWD/conuseritem.cpp \   # 这个是 联系人列表里的QListWidget里的item，继承 ListItemBase
    $$PWD/grouptipitem.cpp \  # 这个是 联系人列表里 的  新朋友/联系人 的提示分段 标识
    $$PWD/applyfriendpage.cpp \	# 这个是 联系人，最右侧的 申请联系人的记录列表 （和 chatpage 在一个 stackWidget）
    $$PWD/applyfrienditem.cpp \	# 这个是 联系人，最右侧的 新朋友 每一条申请消息的item
    $$PWD/friendinfopage.cpp \	# 这个是 联系人，最右侧的 具体的好友 信息页面
    $$PWD/findfaildlg.cpp   \	# 联系人查找失败
    authenfriend.cpp	    # 同意好友申请界面

HEADERS += \
    mainwindow.h \
    logindialog.h \
    registerdialog.h \
    chatdialog.h \
    resetdialog.h \
    chatuserwid.h \
    adduseritem.h \
    chatpage.h \
    loadingdlg.h \
    findsuccessdlg.h \
    applyfriend.h \
    friendlabel.h  \
    $$PWD/conuseritem.h \
    $$PWD/grouptipitem.h \
    $$PWD/applyfriendpage.h \
    $$PWD/applyfrienditem.h \
    $$PWD/friendinfopage.h \
    $$PWD/findfaildlg.h \
    authenfriend.h

FORMS += \
    mainwindow.ui \
    logindialog.ui \
    registerdialog.ui \
    chatdialog.ui \
    resetdialog.ui \
    chatuserwid.ui \
    adduseritem.ui \
    chatpage.ui \
    loadingdlg.ui \
    findsuccessdlg.ui \
    applyfriend.ui \
    friendlabel.ui \
    $$PWD/conuseritem.ui \
    $$PWD/grouptipitem.ui \
    $$PWD/applyfriendpage.ui \
    $$PWD/applyfrienditem.ui \
    $$PWD/friendinfopage.ui \
    $$PWD/findfaildlg.ui \
    authenfriend.ui
