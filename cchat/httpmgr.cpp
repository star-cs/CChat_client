#include "httpmgr.h"

HttpMgr::~HttpMgr()
{

}

void HttpMgr::PostHttpReq(QUrl url, QJsonObject json, ReqId req_id, Modules mod)
{
    //创建一个HTTP POST请求，并设置请求头和请求体
    QByteArray data = QJsonDocument(json).toJson();
    //通过url构造请求
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(data.length()));
    auto self = shared_from_this();
    QNetworkReply *reply = _manager.post(request, data);

    QObject::connect(reply, &QNetworkReply::finished, [reply, self, req_id, mod]{

        if(reply->error() != QNetworkReply::NoError){   // 出错
            qDebug() << reply->errorString();
            emit self->sig_http_finish(req_id, "", ErrorCodes::ERR_NETWORK, mod);
            reply->deleteLater();       // 延迟删除
        }else{  // 无错误则读取请求
            QString res = reply->readAll();
            emit self->sig_http_finish(req_id, res, ErrorCodes::SUCCESS, mod);
            reply->deleteLater();
        }
        return;
    });
}

HttpMgr::HttpMgr()
{
    connect(this, &HttpMgr::sig_http_finish, this, &HttpMgr::slot_http_finish);
}

void HttpMgr::slot_http_finish(ReqId req_id, QString res, ErrorCodes err, Modules mod)
{
    if(mod == Modules::REGISTERMOD){
        //发送信号通知指定模块http响应结束
        emit sig_reg_mod_finish(req_id, res, err);
    }else if(mod == Modules::RESETMOD){
        emit sig_reset_mod_finish(req_id, res, err);
    }else if(mod == Modules::LOGINMOD){
        emit sig_login_mod_finish(req_id, res, err);
    }
}

