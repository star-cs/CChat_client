#include "userdata.h"

SearchInfo::SearchInfo(int uid, QString name,
    QString nick, QString desc, int sex, QString icon):_uid(uid)
  ,_name(name), _nick(nick),_desc(desc),_sex(sex),_icon(icon){
}

void FriendInfo::AppendChatMsgs(const std::vector<std::shared_ptr<TextChatData> > text_vec)
{
    for(const auto & text: text_vec){
        _chat_msgs.push_back(text);
        if(text->_status != MessageStatus::Read){
            _map_msgs.insert(text->_msg_id, text);
        }
    }
}
