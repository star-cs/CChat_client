#ifndef CHATPAGE_H
#define CHATPAGE_H

#include <QWidget>
#include <QPainter>
#include "userdata.h"
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
    void SetUserInfo(std::shared_ptr<UserInfo> userinfo);
    // 添加 聊天气泡
    void AppendChatMsg(std::shared_ptr<TextChatData> msg);
    bool hasUserInfo(){return _user_info != nullptr;}
protected:
    virtual void paintEvent(QPaintEvent* event) override;


private slots:
    void on_send_btn_clicked();

private:
    Ui::ChatPage *ui;

    std::shared_ptr<UserInfo> _user_info;
};

#endif // CHATPAGE_H
