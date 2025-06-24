#ifndef USERMGR_H
#define USERMGR_H
#include <QObject>
#include <memory>
#include "singleton.h"
#include "userdata.h"
#include <vector>
#include <QMap>

class UserMgr:public QObject,public Singleton<UserMgr>,
        public std::enable_shared_from_this<UserMgr>
{
    Q_OBJECT
public:
    friend class Singleton<UserMgr>;
    ~ UserMgr();
    void SetUserInfo(std::shared_ptr<UserInfo> user_info){_user_info = user_info;}
    void SetToken(QString token){_token = token;}
    int GetUid(){return _user_info->_uid;}
    QString GetName(){return _user_info->_name;}
    QString GetIcon(){return _user_info->_icon;}

    std::shared_ptr<UserInfo> GetUserInfo(){return _user_info;}

    // 初始化 好友申请
    void AppendApplyList(QJsonArray array);
    // 初始化 联系人
    void AppendFriendList(QJsonArray array);

    std::vector<std::shared_ptr<ApplyInfo>> GetApplyList(){return _apply_list;}

    // 新增 好友申请
    void AddApplyList(std::shared_ptr<ApplyInfo> app);

    // uid发来的好友申请 是否已申请
    bool AlreadyApply(int uid);
    // 获取 下一页展示 的 聊天记录 信息
    std::vector<std::shared_ptr<FriendInfo>> GetChatListPerPage();
    // 判断聊天记录是否到底
    bool IsLoadChatFin();
    // 更新 聊天list 显示的 item信息
    void UpdateChatLoadedCount();

    // 获取 下一页展示 的 联系人 信息，其实和 GetChatListPerPage 都是返回 要显示哪些用户信息
    std::vector<std::shared_ptr<FriendInfo>> GetConListPerPage();
    void UpdateContactLoadedCount();
    bool IsLoadConFin();

    // 检查 uid是否是 联系人
    bool CheckFriendById(int uid);

    // 新增 联系人
    void AddFriend(std::shared_ptr<AuthRsp> auth_rsp);
    void AddFriend(std::shared_ptr<AuthInfo> auth_info);

    std::shared_ptr<FriendInfo> GetFriendById(int uid);
    void AppendFriendChatMsg(int friend_id,std::vector<std::shared_ptr<TextChatData>> msgs);
private:
    UserMgr();
    std::shared_ptr<UserInfo> _user_info;
    std::vector<std::shared_ptr<ApplyInfo>> _apply_list;
    std::vector<std::shared_ptr<FriendInfo>> _friend_list;
    QMap<int, std::shared_ptr<FriendInfo>> _friend_map;
    QString _token;
    int _chat_loaded;
    int _contact_loaded;

public slots:
    void SlotAddFriendRsp(std::shared_ptr<AuthRsp> rsp);
    void SlotAddFriendAuth(std::shared_ptr<AuthInfo> auth);
};

#endif // USERMGR_H
