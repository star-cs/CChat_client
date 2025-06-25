#include "chatpage.h"
#include "ui_chatpage.h"
#include <QStyleOption>

#include "chatitembase.h"
#include "picturebubble.h"
#include "textbubble.h"
#include "messagetextedit.h"
#include "usermgr.h"
#include "tcpmgr.h"
#include "chatdialog.h"
#include "global.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QString>
#include <QUuid>

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

void ChatPage::SetUserInfo(std::shared_ptr<FriendInfo> friendinfo)
{
    _friend_info = friendinfo;
    ui->title_lb->setText(_friend_info->_name);
    // 1. 删除原始的 聊天 气泡
    ui->chat_data_list->removeAllItem();
    for(auto & msg : _friend_info->_chat_msgs){
        // 2. 重新添加聊天气泡
        AppendChatMsg(msg);
    }
}

void ChatPage::AppendChatMsg(std::shared_ptr<TextChatData> msg)
{
    // 从内存UserMgr UserInfo里加载 msg
    const auto self_info = UserMgr::GetInstance()->GetUserInfo();
    ChatRole role;
    MessageStatus status = msg->_status;
    if(self_info->_uid == msg->_from_uid){
        // 我发出来的消息
        role = ChatRole::Self;
        ChatItemBase *pChatItem = new ChatItemBase(role);
        pChatItem->setUserName(self_info->_name);
        pChatItem->setUserIcon(self_info->_icon);
        // 目前只有历史的文字消息
        QWidget *pBubble = nullptr;
        pBubble = new TextBubble(role, msg->_msg_content, status);
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
        pBubble = new TextBubble(role, msg->_msg_content, status);
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
    if (_friend_info == nullptr) {
        qDebug() << "friend_info is empty";
        return;
    }
    const auto slef_UserInfo = UserMgr::GetInstance()->GetUserInfo();

    auto pTextEdit = ui->chatEdit;
    ChatRole role= ChatRole::Self;
    QString slef_UserName = slef_UserInfo->_name;
    QString slef_UserIcon = slef_UserInfo->_icon;

    const QVector<MsgInfo>& msgList = pTextEdit->getMsgList();
    QJsonObject textObj;
    QJsonArray textArray;
    int txt_size = 0;
    std::vector<std::shared_ptr<TextChatData>> vec_text_msg;
    for(int i=0; i<msgList.size(); ++i)
    {
        //消息内容长度不合规就跳过
        if(msgList[i].content.length() > 1024){
            continue;
        }

        QString type = msgList[i].msgFlag;
        ChatItemBase *pChatItem = new ChatItemBase(role);
        pChatItem->setUserName(slef_UserName);
        pChatItem->setUserIcon(QPixmap(slef_UserIcon));
        QWidget *pBubble = nullptr;
        if(type == "text")
        {
            //生成唯一id
            QUuid uuid = QUuid::createUuid();
            //转为字符串
            QString uuidString = uuid.toString();
            pBubble = new TextBubble(role, msgList[i].content);

            if(txt_size + msgList[i].content.size() > 1024){
                qDebug() << "textArray is " << textArray ;

                textObj["fromuid"] = slef_UserInfo->_uid;
                textObj["touid"] = _friend_info->_uid;
                textObj["text_array"] = textArray;
                QJsonDocument doc(textObj);
                QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
                //发送并清空之前累计的文本列表
                txt_size = 0;
                textArray = QJsonArray();
                textObj = QJsonObject();
                //发送tcp请求给chat server
                emit TcpMgr::GetInstance()->sig_send_data(ReqId::ID_TEXT_CHAT_MSG_REQ, jsonData);
            }

            //将bubble和uid绑定，以后可以等网络返回消息后设置是否送达
            _bubble_map[uuidString] = pBubble;
            txt_size += msgList[i].content.size();
            QJsonObject obj;
            QByteArray utf8Message = msgList[i].content.toUtf8();
            obj["content"] = QString::fromUtf8(utf8Message);
            obj["msgid"] = uuidString;
            textArray.append(obj);
            auto txt_msg = std::make_shared<TextChatData>(uuidString, obj["content"].toString(), slef_UserInfo->_uid, _friend_info->_uid);
            vec_text_msg.push_back(txt_msg);
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

    // 更新到 UserMgr
    UserMgr::GetInstance()->AppendFriendChatMsg(_friend_info->_uid, vec_text_msg);

    // 把剩余的 消息发出去
    qDebug() << "textArray is " << textArray ;
    //发送给服务器
    textObj["text_array"] = textArray;
    textObj["fromuid"] = slef_UserInfo->_uid;
    textObj["touid"] = _friend_info->_uid;
    QJsonDocument doc(textObj);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    //发送并清空之前累计的文本列表
    txt_size = 0;
    textArray = QJsonArray();
    textObj = QJsonObject();
    //发送tcp请求给chat server
    emit TcpMgr::GetInstance()->sig_send_data(ReqId::ID_TEXT_CHAT_MSG_REQ, jsonData);
}
