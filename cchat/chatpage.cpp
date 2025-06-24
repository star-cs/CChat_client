#include "chatpage.h"
#include "ui_chatpage.h"
#include <QStyleOption>

#include "chatitembase.h"
#include "picturebubble.h"
#include "textbubble.h"
#include "messagetextedit.h"
#include "usermgr.h"

ChatPage::ChatPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatPage)
{
    ui->setupUi(this);

    //设置按钮样式
    ui->receive_btn->SetState("normal","hover","press");
    ui->send_btn->SetState("normal","hover","press");
    //设置图标样式
    ui->emo_lb->SetState("normal","hover","press","normal","hover","press");
    ui->file_lb->SetState("normal","hover","press","normal","hover","press");
    ui->voice_lb->SetState("normal","hover","press","normal","hover","press");
    ui->video_lb->SetState("normal","hover","press","normal","hover","press");
}

ChatPage::~ChatPage()
{
    delete ui;
}

void ChatPage::SetUserInfo(std::shared_ptr<UserInfo> userinfo)
{
    _user_info = userinfo;
    ui->title_lb->setText(_user_info->_name);
    // 1. 删除原始的 聊天 气泡
    ui->chat_data_list->removeAllItem();
    for(auto & msg : _user_info->_chat_msgs){
        // 2. 重新添加聊天气泡
        AppendChatMsg(msg);
    }
}

void ChatPage::AppendChatMsg(std::shared_ptr<TextChatData> msg)
{
    auto self_info = UserMgr::GetInstance()->GetUserInfo();
    ChatRole role;

    if(self_info->_uid = msg->_from_uid){
        // 我发出来的消息
        role = ChatRole::Self;
        ChatItemBase *pChatItem = new ChatItemBase(role);
        pChatItem->setUserName(self_info->_name);
        pChatItem->setUserIcon(self_info->_icon);
        // 目前只有历史的文字消息
        QWidget *pBubble = nullptr;
        pBubble = new TextBubble(role, msg->_msg_content);
        pChatItem->setWidget(pBubble);
        ui->chat_data_list->appendChatItem(pChatItem);
    }else{
        // 别人发出来的消息
        auto friend_info = UserMgr::GetInstance()->GetFriendById(msg->_from_uid);
        if(friend_info == nullptr){
            return;
        }
        role = ChatRole::Other;
        ChatItemBase *pChatItem = new ChatItemBase(role);
        pChatItem->setUserName(friend_info->_name);
        pChatItem->setUserIcon(friend_info->_icon);
        // 目前只有历史的文字消息
        QWidget *pBubble = nullptr;
        pBubble = new TextBubble(role, msg->_msg_content);
        pChatItem->setWidget(pBubble);
        ui->chat_data_list->appendChatItem(pChatItem);
    }
}

void ChatPage::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ChatPage::on_send_btn_clicked()
{
    auto pTextEdit = ui->chatEdit;
    ChatRole role= ChatRole::Self;
    QString userName = UserMgr::GetInstance()->GetName();
    QString userIcon = UserMgr::GetInstance()->GetIcon();

    const QVector<MsgInfo>& msgList = pTextEdit->getMsgList();
    for(int i=0; i<msgList.size(); ++i)
    {
        QString type = msgList[i].msgFlag;
        ChatItemBase *pChatItem = new ChatItemBase(role);
        pChatItem->setUserName(userName);
        pChatItem->setUserIcon(QPixmap(userIcon));
        QWidget *pBubble = nullptr;
        if(type == "text")
        {
            pBubble = new TextBubble(role, msgList[i].content);
        }
        else if(type == "image")
        {
            pBubble = new PictureBubble(QPixmap(msgList[i].content) , role);
        }
        else if(type == "file")
        {
        }
        if(pBubble != nullptr)
        {
            pChatItem->setWidget(pBubble);
            ui->chat_data_list->appendChatItem(pChatItem);
        }
    }
}
