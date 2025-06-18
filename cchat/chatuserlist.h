#ifndef CHATUSERLIST_H
#define CHATUSERLIST_H
#include <QListWidget>
#include <QWheelEvent>
#include <QEvent>
#include <QScrollBar>
#include <QDebug>

/**
*    @file:    chatuserlist.h
*    @brief:   对于 QListWidget 的 自定义控件，滑动加载新用户页面
*    @author:  star-cs
*    @date:    2025-06-17  16:00
*/

class ChatUserList : public QListWidget
{
    Q_OBJECT
public:
    ChatUserList(QWidget *parent = nullptr);
protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
signals:
    void sig_loading_chat_user();
};

#endif // CHATUSERLIST_H
