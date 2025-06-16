#ifndef GLOBAL_H
#define GLOBAL_H

#include <QWidget>
#include <functional>
#include <QDir>
#include <QSettings>

#include "QStyle"
#include "singleton.h"

extern std::function<void(QWidget*)> repolish;


// 细分到每种请求任务，结束调用的回调函数
enum ReqId{
    ID_GET_VERIFY_CODE = 1001,  //获取验证码
    ID_REG_USER = 1002,         //注册用户
    ID_RESET_PWD = 1003,        //重置密码
    ID_LOGIN_EMAIL = 1004,      //登录 （mysql）
    ID_CHAT_LOGIN = 1005,       //登录聊天服务器
    ID_CHAT_LOGIN_RSP = 1006,   //登录聊天服务器回包

};

extern QString gate_url_prefix;     // GateServer前缀


namespace ReqServlet {
    inline const QString& GET_TEST() {
        static const QString value = "/get_test";
        return value;
    }
    inline const QString& GET_VERIFYCODE() {
        static const QString value = "/get_verifycode";
        return value;
    }
    inline const QString& REGISTER() {
        static const QString value = "/register";
        return value;
    }
    inline const QString& RESET_PWD() {
        static const QString value = "/reset_pwd";
        return value;
    }

    inline const QString& LOGIN_EMAIL() {
        static const QString value = "/login";
        return value;
    }
}

enum ErrorCodes{
    SUCCESS = 0,
    ERR_JSON = 1, //Json解析失败
    ERR_NETWORK = 2,
};

// 用于 httpMgr 处理响应，向对应模块发送回调完成信号
enum Modules{
    REGISTERMOD = 0,
    RESETMOD = 1,
    LOGINMOD = 2,
};

enum TipErr{
    TIP_SUCCESS = 0,
    TIP_EMAIL_ERR = 1,
    TIP_PWD_ERR = 2,
    TIP_CONFIRM_ERR = 3,
    TIP_PWD_CONFIRM = 4,
    TIP_VARIFY_ERR = 5,
    TIP_USER_ERR = 6
};

enum ClickLbState{
    Normal = 0,
    Selected = 1
};

// 聊天服务器 属性
struct ServerInfo{
    QString host;
    QString port;
    QString token;
    int uid;
};

#endif // GLOBAL_H
