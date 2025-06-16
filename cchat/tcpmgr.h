#ifndef TCPMGR_H
#define TCPMGR_H

#include <QObject>
#include <QTcpSocket>
#include "global.h"
#include "singleton.h"

class TcpMgr : public QObject, public Singleton<TcpMgr>, public std::enable_shared_from_this<TcpMgr>
{
    Q_OBJECT

public:
    ~TcpMgr();
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

};

#endif // TCPMGR_H
