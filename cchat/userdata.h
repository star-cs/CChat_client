#ifndef USERDATA_H
#define USERDATA_H
#include <QString>
#include <memory>
#include <QJsonArray>
#include <vector>
#include <QJsonObject>
#include <QMap>
#include "global.h"

class SearchInfo {
public:
    SearchInfo(int uid, QString name, QString nick, QString desc, int sex, QString icon);
    int _uid;
    QString _name;
    QString _nick;
    QString _desc;
    int _sex;
    QString _icon;
};

class AddFriendApply {
public:
    AddFriendApply(int from_uid, QString name, QString desc,
                   QString icon, QString nick, int sex);
    int _from_uid;
    QString _name;
    QString _desc;
    QString _icon;
    QString _nick;
    int     _sex;
};

struct ApplyInfo {
    ApplyInfo(int uid, QString name, QString desc,
        QString icon, QString nick, int sex, int status)
        :_uid(uid),_name(name),_desc(desc),
        _icon(icon),_nick(nick),_sex(sex),_status(status){}

    ApplyInfo(std::shared_ptr<AddFriendApply> addinfo)
        :_uid(addinfo->_from_uid),_name(addinfo->_name),
          _desc(addinfo->_desc),_icon(addinfo->_icon),
          _nick(addinfo->_nick),_sex(addinfo->_sex),
          _status(0)
    {}
    void SetIcon(QString head){
        _icon = head;
    }
    int _uid;
    QString _name;
    QString _desc;
    QString _icon;
    QString _nick;
    int _sex;
    int _status;
};

class TextChatData;
struct AuthInfo {
    AuthInfo(int uid, QString name,
             QString nick, QString icon, int sex, QString desc = ""):
        _uid(uid), _name(name), _nick(nick), _icon(icon),
        _sex(sex), _thread_id(0), _desc(desc){}

    void SetChatDatas(std::vector<std::shared_ptr<TextChatData>> _chat_datas);
    int _uid;
    QString _name;
    QString _nick;
    QString _icon;
    int _sex;
    int _thread_id;
    QString _desc;
    std::vector<std::shared_ptr<TextChatData>> _chat_datas;
};

struct AuthRsp {
    AuthRsp(int peer_uid, QString peer_name,
            QString peer_nick, QString peer_icon, int peer_sex, QString desc = "")
        :_uid(peer_uid),_name(peer_name),_nick(peer_nick),
          _icon(peer_icon),_sex(peer_sex),_thread_id(0), _desc(desc)
    {

    }


    void SetChatDatas(std::vector<std::shared_ptr<TextChatData>> _chat_datas);
    int _uid;
    QString _name;
    QString _nick;
    QString _icon;
    int _sex;
    int _thread_id;
    QString _desc;
    std::vector<std::shared_ptr<TextChatData>> _chat_datas;
};

struct UserInfo {
    UserInfo(int uid, QString name, QString nick, QString icon, int sex, QString last_msg = "", QString desc=""):
        _uid(uid),_name(name),_nick(nick),_icon(icon),_sex(sex),_desc(desc){}

    UserInfo(std::shared_ptr<AuthInfo> auth):
        _uid(auth->_uid),_name(auth->_name),_nick(auth->_nick),
        _icon(auth->_icon),_sex(auth->_sex),_desc(auth->_desc){}

    UserInfo(int uid, QString name, QString icon, int sex, QString desc=""):
        _uid(uid), _name(name), _icon(icon),_nick(_name),
        _sex(0),_desc(desc){}

    UserInfo(std::shared_ptr<AuthRsp> auth):
        _uid(auth->_uid),_name(auth->_name),_nick(auth->_nick),
        _icon(auth->_icon),_sex(auth->_sex),_desc(auth->_desc){}

    UserInfo(std::shared_ptr<SearchInfo> search_info):
        _uid(search_info->_uid),_name(search_info->_name),_nick(search_info->_nick),
    _icon(search_info->_icon),_sex(search_info->_sex), _desc(search_info->_desc){

    }

    int _uid;
    QString _name;
    QString _nick;
    QString _icon;
    int _sex;
    QString _desc;
};

class ChatDataBase {
public:
    ChatDataBase(int msg_id, int thread_id, ChatFormType form_type, ChatMsgType msg_type,
        QString content,int _send_uid, QString create_time = "");
    ChatDataBase(QString unique_id, int thread_id, ChatFormType form_type, ChatMsgType msg_type,
        QString content, int send_uid, QString create_time = "");
    int GetMsgId() { return _msg_id; }
    int GetThreadId() { return _thread_id; }
    ChatFormType GetFormType() { return _form_type; }
    ChatMsgType GetMsgType() { return _msg_type; }
    QString GetContent() { return _content; }
    int GetSendUid() { return _send_uid; }
    QString GetMsgContent(){return _content;}
    void SetUniqueId(int unique_id);
    QString GetUniqueId();
private:
    //客户端本地唯一标识
    QString _unique_id;
    //消息id
    int _msg_id;
    //会话id
    int _thread_id;
    //群聊还是私聊
    ChatFormType _form_type;
    //文本信息为0，图片为1，文件为2
    ChatMsgType _msg_type;
    QString _content;
    //发送者id
    int _send_uid;
    QString _create_time;
};

class TextChatData : public ChatDataBase {
public:

    TextChatData(int msg_id, int thread_id, ChatFormType form_type, ChatMsgType msg_type,  QString content,
        int send_uid, QString creat_time = ""):
        ChatDataBase(msg_id, thread_id, form_type, msg_type, content, send_uid, creat_time)
    {

    }

    TextChatData(QString unique_id, int thread_id, ChatFormType form_type, ChatMsgType msg_type, QString content,
        int send_uid, QString creat_time = ""):
        ChatDataBase(unique_id, thread_id, form_type, msg_type, content, send_uid, creat_time)
    {

    }

};

//聊天线程信息
struct ChatThreadInfo {
    int _thread_id;
    QString _type;     // "private" or "group"
    int _user1_id;    // 私聊时对应 private_chat.user1_id；群聊时设为 0
    int _user2_id;    // 私聊时对应 private_chat.user2_id；群聊时设为 0
};

//客户端本地存储的聊天线程数据结构
class ChatThreadData {
public:
    ChatThreadData(int other_id, int thread_id, int last_msg_id):
         _thread_id(thread_id), _other_id(other_id), _last_msg_id(last_msg_id){}
    void AddMsg(std::shared_ptr<ChatDataBase> msg);
    void SetLastMsgId(int msg_id);
    int GetLastMsgId(){return _last_msg_id;}
    void SetOtherId(int other_id);
    int  GetOtherId();
    QString GetGroupName();
    QMap<int, std::shared_ptr<ChatDataBase>> GetMsgMap();
    int  GetThreadId();
    QMap<int, std::shared_ptr<ChatDataBase>>&  GetMsgMapRef();
    QString GetLastMsg();

    int _thread_id;    //会话id
private:
    //如果是私聊，则为对方的id；如果是群聊，则为0
    int _other_id;
    int _last_msg_id;  // 最近的一条消息编号
    QString _last_msg;
    //群聊信息,成员列表
    std::vector<int> _group_members;
    //群聊名称
    QString _group_name;
    //缓存消息map，抽象为基类，因为会有图片等其他类型消息
    QMap<int, std::shared_ptr<ChatDataBase>>  _msg_map;
};


#endif
