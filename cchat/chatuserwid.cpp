#include "chatuserwid.h"
#include "ui_chatuserwid.h"

ChatUserWid::ChatUserWid(QWidget *parent) :
    ListItemBase(parent),
    ui(new Ui::ChatUserWid)
{
    ui->setupUi(this);
    SetItemType(ListItemType::CHAT_USER_ITEM);
    ui->red_point->raise();
    ShowRedPoint(false);
}


ChatUserWid::~ChatUserWid()
{
    delete ui;
}

void ChatUserWid::SetInfo(const std::shared_ptr<UserInfo>& user_info)
{
    _uid = user_info->_uid;
    _icon = user_info->_icon;
    _name = user_info->_name;
    _last_msg = user_info->_last_msg;
    // 加载图片
    QPixmap pixmap(_icon);

    // 设置图片自动缩放
    ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_lb->setScaledContents(true);

    ui->user_name_lb->setText(_name);
    ui->user_chat_lb->setText(_last_msg);
}

void ChatUserWid::SetInfo(const std::shared_ptr<FriendInfo>& friend_info)
{
    _uid = friend_info->_uid;
    _icon = friend_info->_icon;
    _name = friend_info->_name;
    _last_msg = friend_info->_last_msg;
    // 加载图片
    QPixmap pixmap(_icon);

    // 设置图片自动缩放
    ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_lb->setScaledContents(true);

    ui->user_name_lb->setText(_name);
    ui->user_chat_lb->setText(_last_msg);
}

void ChatUserWid::ShowRedPoint(bool bshow)
{
    if(bshow){
        ui->red_point->show();
    }else{
        ui->red_point->hide();
    }
}

void ChatUserWid::updateLastMsg(const std::vector<std::shared_ptr<TextChatData> >& msgs)
{
    _last_msg = msgs.back()->_msg_content;
    ShowRedPoint(true);
    ui->user_chat_lb->setText(_last_msg);
}
