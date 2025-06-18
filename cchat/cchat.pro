#-------------------------------------------------
#
# Project created by QtCreator 2025-05-28T15:02:02
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = cchat
TEMPLATE = app

RC_ICONS = icon.ico
DESTDIR = ./bin

CONFIG += c++17

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    logindialog.cpp \
    registerdialog.cpp \
    global.cpp \
    httpmgr.cpp \
    timerbtn.cpp \
    clickedlabel.cpp \
    resetdialog.cpp \
    tcpmgr.cpp \
    chatdialog.cpp \
    clickedbtn.cpp \
    customizeedit.cpp \
    chatuserlist.cpp \
    chatuserwid.cpp \
    listitembase.cpp \
    chatpage.cpp \
    loadingdlg.cpp \
    chatview.cpp \
    chatitembase.cpp \
    bubbleframe.cpp \
    textbubble.cpp \
    picturebubble.cpp \
    messagetextedit.cpp

HEADERS += \
        mainwindow.h \
    logindialog.h \
    registerdialog.h \
    global.h \
    singleton.h \
    httpmgr.h \
    timerbtn.h \
    clickedlabel.h \
    resetdialog.h \
    tcpmgr.h \
    chatdialog.h \
    clickedbtn.h \
    customizeedit.h \
    chatuserlist.h \
    chatuserwid.h \
    listitembase.h \
    chatpage.h \
    loadingdlg.h \
    chatview.h \
    chatitembase.h \
    bubbleframe.h \
    textbubble.h \
    picturebubble.h \
    messagetextedit.h

FORMS += \
        mainwindow.ui \
    logindialog.ui \
    registerdialog.ui \
    resetdialog.ui \
    chatdialog.ui \
    chatuserwid.ui \
    chatpage.ui \
    loadingdlg.ui

RESOURCES += \
    rc.qrc

DISTFILES += \
    config.ini

win32:CONFIG(release, debug | release)
{
    #指定要拷贝的文件目录为工程目录下release目录下的所有dll、lib文件，例如工程目录在D:\QT\Test
    #PWD就为D:/QT/Test，DllFile = D:/QT/Test/release/*.dll
    TargetConfig = $${PWD}/config.ini
    #将输入目录中的"/"替换为"\"
    TargetConfig = $$replace(TargetConfig, /, \\)
    #将输出目录中的"/"替换为"\"
    OutputDir =  $${OUT_PWD}/$${DESTDIR}
    OutputDir = $$replace(OutputDir, /, \\)
    //执行copy命令
    QMAKE_POST_LINK += copy /Y \"$$TargetConfig\" \"$$OutputDir\"
}
