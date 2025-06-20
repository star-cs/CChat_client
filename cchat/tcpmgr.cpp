#include "tcpmgr.h"
#include <QJsonDocument>
#include <QJsonObject>

TcpMgr::~TcpMgr()
{

}

TcpMgr::TcpMgr(): _host(""), _port(0), _b_recv_pending(false), _message_id(0), _message_len(0)
{
    QObject::connect(&_socket, &QTcpSocket::connected, [&](){
        qDebug() << "Connected to server!";
        emit sig_conn_success(true);
    });

    QObject::connect(&_socket, &QTcpSocket::readyRead, [&](){
        _buffer.append(_socket.readAll());

        QDataStream stream(&_buffer, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_5_0);

        forever{
            // 1. 解析头部 id(2B) + len(2B)
            if(!_b_recv_pending){
                if(_buffer.size() < static_cast<int>(sizeof(quint16)*2)){
                    return;
                }

                stream >> _message_id >> _message_len;

                _buffer = _buffer.mid(sizeof(quint16) * 2);

                qDebug() << "Message ID:" << _message_id << ", length: " << _message_len;
            }
            // 2. 解析出消息内容
            if(_buffer.size() < _message_len){
                _b_recv_pending = true;
                return;
            }

            _b_recv_pending = false;    // 继续，重新接受头部

            QByteArray messageBody = _buffer.mid(0, _message_len);
            qDebug() << "receive body msg is " << messageBody;

            _buffer = _buffer.mid(_message_len);

            handleMsg(ReqId(_message_id), _message_len, messageBody);
        }
    });


    //5.15 之后版本
//       QObject::connect(&_socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred), [&](QAbstractSocket::SocketError socketError) {
//           Q_UNUSED(socketError)
//           qDebug() << "Error:" << _socket.errorString();
//       });
       // 处理错误（适用于Qt 5.15之前的版本）
       QObject::connect(&_socket, static_cast<void (QTcpSocket::*)(QTcpSocket::SocketError)>(&QTcpSocket::error),
                            [&](QTcpSocket::SocketError socketError) {
               qDebug() << "Error:" << _socket.errorString() ;
               switch (socketError) {
                   case QTcpSocket::ConnectionRefusedError:
                       qDebug() << "Connection Refused!";
                       emit sig_conn_success(false);
                       break;
                   case QTcpSocket::RemoteHostClosedError:
                       qDebug() << "Remote Host Closed Connection!";
                       break;
                   case QTcpSocket::HostNotFoundError:
                       qDebug() << "Host Not Found!";
                       emit sig_conn_success(false);
                       break;
                   case QTcpSocket::SocketTimeoutError:
                       qDebug() << "Connection Timeout!";
                       emit sig_conn_success(false);
                       break;
                   case QTcpSocket::NetworkError:
                       qDebug() << "Network Error!";
                       break;
                   default:
                       qDebug() << "Other Error!";
                       break;
               }
         });

    QObject::connect(&_socket, &QTcpSocket::disconnected, [&](){
        qDebug() << "Disconnected from server.";

    });


    // 发送消息
    QObject::connect(this, &TcpMgr::sig_send_data, this, &TcpMgr::slot_send_data);

    initHandlers();
}

void TcpMgr::initHandlers()
{
    // 连接 聊天服务器回包
    _handlers.insert(ReqId::ID_CHAT_LOGIN_RSP, [this](ReqId id, int len, QByteArray data){
        Q_UNUSED(len);  //
        qDebug()<< "handle id is "<< id ;

        // QByteArray --> QJsonDocument --> QJsonObject

        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        if(jsonDoc.isNull()){
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "Failed to create QJsonDocument.";
            emit sig_login_failed(err);
            return;
        }
        QJsonObject jsonObj = jsonDoc.object();
        qDebug()<< "data jsonobj is " << jsonObj;

        if(!jsonObj.contains("error")){
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "Login Failed, err is Json Parse Err" << err ;
            emit sig_login_failed(err);
            return;
        }

        int err = jsonObj["error"].toInt();
        if(err != ErrorCodes::SUCCESS){
            qDebug() << "Login Failed, err is " << err ;
            emit sig_login_failed(err);
            return;
        }

        qDebug() << "success ID_CHAT_LOGIN_RSP";
//        auto uid = jsonObj["uid"].toInt();
//        auto name = jsonObj["name"].toString();
//        auto nick = jsonObj["nick"].toString();
//        auto icon = jsonObj["icon"].toString();
//        auto sex = jsonObj["sex"].toInt();
//        auto desc = jsonObj["desc"].toString();
//        auto user_info = std::make_shared<UserInfo>(uid, name, nick, icon, sex,"",desc);


        // 成功后，切换界面
        emit sig_swich_chatdlg();
    });
}

void TcpMgr::handleMsg(ReqId id, int len, QByteArray data)
{
    auto find_iter = _handlers.find(id);
    if(find_iter ==  _handlers.end()){
        qDebug()<< "not found id ["<< id << "] to handle";
        return ;
    }
    // qMap Iterator 可以value取值
//    find_iter.key();
    find_iter.value()(id, len, data);
}

void TcpMgr::slot_tcp_connect(ServerInfo server)
{
    // 连接 聊天服务器
    _host = server.host;
    // QString --> uint --> uint16_t
    _port = static_cast<uint16_t>(server.port.toUInt());
    _socket.connectToHost(_host, _port);
}

void TcpMgr::slot_send_data(ReqId reqId, QByteArray dataByte)
{
    uint16_t id = reqId;

    quint16 len = static_cast<quint16>(dataByte.size());

    // 创建一个QByteArray用于存储要发送的所有数据
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out.setByteOrder(QDataStream::BigEndian);

    out << id << len;

    block.append(dataByte);     // 消息体是保持了原本的 小端方式

    _socket.write(block);
    qDebug() << "tcp mgr send byte data is " << block;
}
