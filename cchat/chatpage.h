#ifndef CHATPAGE_H
#define CHATPAGE_H

#include <QWidget>
#include <QPainter>
#include "userdata.h"
#include <QMap>

namespace Ui {
class ChatPage;
}

/**
*    @file:    chatpage.h
*    @brief:   把聊天界面QWidget抽离出来，单独设计。只需要在 ChatDialog 的 StackWidget 设置即可
*    @author:  star-cs
*    @date:    2025-06-17  17:36
*/
class ChatPage : public QWidget
{
    Q_OBJECT

public:
    explicit ChatPage(QWidget *parent = 0);
    ~ChatPage();
    // 设置当前 聊天界面对象的 UserInfo
    void SetUserInfo(std::shared_ptr<FriendInfo> userinfo);
    // 添加 聊天气泡
    void AppendChatMsg(std::shared_ptr<TextChatData> msg);
    bool hasUserInfo(){return _friend_info != nullptr;}
    int getCurrentUid(){return _friend_info != nullptr ? _friend_info->_uid : 0;}
protected:
    virtual void paintEvent(QPaintEvent* event) override;


private slots:
    void on_send_btn_clicked();

private:
    Ui::ChatPage *ui;

    std::shared_ptr<FriendInfo> _friend_info;

    // uuid 绑定 对应 的一条消息。
    QMap<QString, QWidget*> _bubble_map;
};

#endif // CHATPAGE_H
