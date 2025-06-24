#ifndef CHATUSERWID_H
#define CHATUSERWID_H

#include <QWidget>
#include "listitembase.h"
#include "userdata.h"
/**
*    @file:    chatuserwid.h
*    @brief:   QListWidget 的每一个 item 的 聊天widget
*    @author:  star-cs
*    @date:    2025-06-17  16:01
*/

namespace Ui {
class ChatUserWid;
}

class ChatUserWid : public ListItemBase
{
    Q_OBJECT

public:
    explicit ChatUserWid(QWidget *parent = 0);
    ~ChatUserWid();

    QSize sizeHint() const override{
        return QSize(250, 70);  // 返回自定义的尺寸
    }
    void SetInfo(std::shared_ptr<UserInfo> user_info);
    void SetInfo(std::shared_ptr<FriendInfo> friend_info);
    void ShowRedPoint(bool bshow);
    std::shared_ptr<UserInfo> GetUserInfo(){return _user_info;}
    void updateLastMsg(std::vector<std::shared_ptr<TextChatData>> msgs);
private:
    Ui::ChatUserWid *ui;
    std::shared_ptr<UserInfo> _user_info;

};

#endif // CHATUSERWID_H
