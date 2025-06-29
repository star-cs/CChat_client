#ifndef TCPMGR_H
#define TCPMGR_H

#include <QObject>
#include <QTcpSocket>
#include "global.h"
#include "singleton.h"
#include "userdata.h"

class TcpMgr : public QObject, public Singleton<TcpMgr>, public std::enable_shared_from_this<TcpMgr>
{
    Q_OBJECT

public:
    ~TcpMgr();
    void CloseConnection();

private:
    friend class Singleton<TcpMgr>;
    TcpMgr();

    void initHandlers();
    void handleMsg(ReqId id, int len, QByteArray data);

private:
    QTcpSocket _socket;
    QString _host;
    uint16_t _port;
    QByteArray _buffer;
    bool _b_recv_pending;
    quint16 _message_id;
    quint16 _message_len;
    QMap<ReqId, std::function<void(ReqId id, int len, QByteArray data)>> _handlers;


public slots:
    void slot_tcp_connect(ServerInfo server);
    void slot_send_data(ReqId reqId, QByteArray dataByte);

signals:
    void sig_conn_success(bool success);
    void sig_send_data(ReqId reqId, QByteArray dataByte);

    void sig_swich_chatdlg();
    void sig_login_failed(int);

    void sig_load_apply_list(QJsonArray json_array);        // 加载完成 好友申请记录 列表数据

    void sig_user_search(std::shared_ptr<SearchInfo>);      // 用户搜索的回包，返回得到的 searchInfo

    void sig_friend_apply(std::shared_ptr<ApplyInfo>);      // 用户接收到 别人的 好友申请

    void sig_add_auth_friend(std::shared_ptr<AuthInfo>);    //

    void sig_auth_rsp(std::shared_ptr<AuthRsp>);            //

    void sig_text_chat_msg_rsp(std::shared_ptr<TextChatMsg>);   // 成功发送消息的 回包

    void sig_text_chat_msg(std::shared_ptr<TextChatMsg> msg);   // 接收到 他人发来的 聊天消息

    void sig_notify_offline();
    void sig_connection_closed();


};

#endif // TCPMGR_H
