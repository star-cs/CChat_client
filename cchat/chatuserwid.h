#ifndef CHATUSERWID_H
#define CHATUSERWID_H

#include <QWidget>
#include "listitembase.h"
#include "userdata.h"
#include <QTime>
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
    void SetInfo(const std::shared_ptr<UserInfo>& user_info);
    void SetInfo(const std::shared_ptr<FriendInfo>& friend_info);
    int getUid(){return _uid;}
    void ShowRedPoint(bool bshow);
    void updateLastMsg(const std::vector<std::shared_ptr<TextChatData>>& msgs);
public:
    bool isAppended = false;

private:
    Ui::ChatUserWid *ui;
    int _uid;
    QString _name;
    QString _icon;
    QString  _last_msg;
};

#endif // CHATUSERWID_H
