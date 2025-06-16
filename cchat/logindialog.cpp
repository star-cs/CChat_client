#include "logindialog.h"
#include "ui_logindialog.h"
#include "httpmgr.h"
#include "tcpmgr.h"
#include <QPainter>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);

    // 重置密码 label
    ui->reset_passwd->SetState("normal","hover","","selected","selected_hover","");
    ui->reset_passwd->setCursor(Qt::PointingHandCursor);
    connect(ui->reset_passwd, &ClickedLabel::clicked, this, &LoginDialog::sigSwitchReset);

    // 注册按钮
    connect(ui->register_btn, &QPushButton::clicked, this, &LoginDialog::sigSwitchRegister);
    ui->passwd_edit->setEchoMode(QLineEdit::Password);

    // 登录密码 可见性 label
    ui->passwd_visible->setCursor(Qt::PointingHandCursor);
    ui->passwd_visible->SetState("unvisible","unvisible_hover","","visible","visible_hover","");
    connect(ui->passwd_visible, &ClickedLabel::clicked, [this](){
        if(ui->passwd_visible->GetCurState() == ClickLbState::Normal){
            ui->passwd_edit->setEchoMode(QLineEdit::Password);
        }else{
            ui->passwd_edit->setEchoMode(QLineEdit::Normal);
        }
    });

    // err_tip
    ui->err_tip->setProperty("state", "normal");
    ui->err_tip->clear();


    connect(ui->email_edit, &QLineEdit::editingFinished, [this](){
        this->checkEmailValid();
    });
    connect(ui->passwd_edit, &QLineEdit::editingFinished, [this](){
        this->checkPwdValid();
    });

    initHttpHandlers();

    /*
    登录流程：
    1. on_login_btn_clicked，发送HttpMgr Post请求
    2. HttpMgr接收到 --> sig_login_mod_finish
    3. slot_login_mod_finish 调用回调处理

    此时 数据库匹配成功，并且获取到聊天服务器的地址信息
    4. 发送 sig_connect_tcp --> TcpMgr::slot_tcp_connect  _socket.connectToHost(...);
    5. TcpMgr [&_socket, &QTcpSocket::connected]，绑定这回调 emit &TcpMgr::sig_conn_success(true);
    6. LoginDialog::slot_tcp_con_finish 中 emit TcpMgr::sig_send_data(ReqId::ID_CHAT_LOGIN, jsonData); 发送 uid,token 校验
    7. TcpMgr::slot_send_data 发送

    8. [&_socket, QTcpSocket::readyRead] 回调触发，handleMsg(...)，接受到 ReqId::ID_CHAT_LOGIN_RSP 回包

    9. 按照情况发送，emit TcpMgr::sig_login_failed() 或者 直接 切换界面
    */

    //连接登录回包信号
    connect(HttpMgr::GetInstance().get(), &HttpMgr::sig_login_mod_finish, this, &LoginDialog::slot_login_mod_finish);

    // 连接 tcp连接请求信号
    connect(this, &LoginDialog::sig_connect_tcp, TcpMgr::GetInstance().get(), &TcpMgr::slot_tcp_connect);

    //连接 TcpMgr 发送的连接成功信号
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_conn_success, this, &LoginDialog::slot_tcp_con_finish);

    //连接 TcpMgr 发送的连接失败信号
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_login_failed, this, &LoginDialog::slot_login_failed);


    initHead();
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::initHead()
{
    // 加载图片
    QPixmap originalPixmap(":/res/head_1.jpg");
      // 设置图片自动缩放
    qDebug()<< originalPixmap.size() << ui->head_label->size();
    originalPixmap = originalPixmap.scaled(ui->head_label->size(),
            Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // 创建一个和原始图片相同大小的QPixmap，用于绘制圆角图片
    QPixmap roundedPixmap(originalPixmap.size());
    roundedPixmap.fill(Qt::transparent); // 用透明色填充

    QPainter painter(&roundedPixmap);
    painter.setRenderHint(QPainter::Antialiasing); // 设置抗锯齿，使圆角更平滑
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    // 使用QPainterPath设置圆角
    QPainterPath path;
    path.addRoundedRect(0, 0, originalPixmap.width(), originalPixmap.height(), 10, 10); // 最后两个参数分别是x和y方向的圆角半径
    painter.setClipPath(path);

    // 将原始图片绘制到roundedPixmap上
    painter.drawPixmap(0, 0, originalPixmap);

    // 设置绘制好的圆角图片到QLabel上
    ui->head_label->setPixmap(roundedPixmap);
}

void LoginDialog::initHttpHandlers()
{
    _handlers.insert(ReqId::ID_LOGIN_EMAIL, [this](QJsonObject jsonObj){
        int error = jsonObj["error"].toInt();
        if(error != ErrorCodes::SUCCESS){
            showTip(tr("参数错误"), false);
            enableBtn(true);
            return;
        }
        auto email = jsonObj["email"].toString();
        ServerInfo server;
        server.host = jsonObj["host"].toString();
        server.port = jsonObj["port"].toString();
        server.token = jsonObj["token"].toString();
        server.uid = jsonObj["uid"].toInt();

        _uid = server.uid;
        _token = server.token;
        qDebug()<< "email is " << email << " uid is " << server.uid <<" host is "
                << server.host << " Port is " << server.port << " Token is " << server.token;

        // 发送消息通知 tcpMgr 建立长链接
        emit sig_connect_tcp(server);
    });
}

void LoginDialog::showTip(QString str, bool b_ok)
{
    if(b_ok){
         ui->err_tip->setProperty("state","normal");
    }else{
        ui->err_tip->setProperty("state","err");
    }
    ui->err_tip->setText(str);
    repolish(ui->err_tip);
}

bool LoginDialog::checkEmailValid()
{
    auto email = ui->email_edit->text();
    if(email.isEmpty()){
        AddTipErr(TipErr::TIP_EMAIL_ERR, tr("邮箱不能为空"));
        return false;
    }
    QRegularExpression regex(
        R"(^[a-zA-Z0-9.!#$%&'*+\/=?^_`{|}~-]+)"  // 用户名部分
        R"(@[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?)"  // 域名开头
        R"(\.[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?$)"  // 顶级域名
    );
    bool match = regex.match(email).hasMatch();
    if(!match){
        // 提示邮箱错误
        AddTipErr(TipErr::TIP_EMAIL_ERR, tr("邮箱地址格式不正确"));
        return false;
    }

    DelTipErr(TipErr::TIP_EMAIL_ERR);
    return true;
}

bool LoginDialog::checkPwdValid()
{
    auto pwd = ui->passwd_edit->text();
    if(pwd.length() < 6 || pwd.length() > 15){
        qDebug() << "Pass length invalid";
        //提示长度不准确
        AddTipErr(TipErr::TIP_PWD_ERR, tr("密码长度应为6~15"));
        return false;
    }

    // 创建一个正则表达式对象，按照上述密码要求
    // 这个正则表达式解释：
    // ^[a-zA-Z0-9!@#$%^&*]{6,15}$ 密码长度至少6，可以是字母、数字和特定的特殊字符
    QRegularExpression regExp("^[a-zA-Z0-9!@#$%^&*.]{6,15}$");
    bool match = regExp.match(pwd).hasMatch();
    if(!match){
        //提示字符非法
        AddTipErr(TipErr::TIP_PWD_ERR, tr("不能包含非法字符且长度为(6~15)"));
        return false;;
    }

    DelTipErr(TipErr::TIP_PWD_ERR);

    return true;
}

void LoginDialog::AddTipErr(TipErr te, QString tips)
{
    _tip_errs[te] = tips;
    showTip(tips, false);
}

void LoginDialog::DelTipErr(TipErr te)
{
    _tip_errs.remove(te);
    if(_tip_errs.empty()){
      ui->err_tip->clear();
      return;
    }

    showTip(_tip_errs.first(), false);
}

void LoginDialog::enableBtn(bool enabled)
{
    ui->login_btn->setEnabled(enabled);
    ui->register_btn->setEnabled(enabled);
    return;
}

void LoginDialog::slot_login_mod_finish(ReqId id, QString res, ErrorCodes err)
{
    if(err != ErrorCodes::SUCCESS){
        showTip(tr("网络请求错误"),false);
        enableBtn(true);
        return;
    }

    // 解析 JSON 字符串,res需转化为QByteArray
    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());
    //json解析错误
    if(jsonDoc.isNull()){
        showTip(tr("json解析错误"),false);
        enableBtn(true);
        return;
    }

    //json解析错误
    if(!jsonDoc.isObject()){
        showTip(tr("json解析错误"),false);
        enableBtn(true);
        return;
    }


    //调用对应的逻辑,根据id回调。
    _handlers[id](jsonDoc.object());

    return;
}

void LoginDialog::slot_tcp_con_finish(bool bsuccess)
{
    if(bsuccess){
        showTip(tr("连接聊天服务器中..."), true);

        QJsonObject jsonObj;
        jsonObj["uid"] = _uid;
        jsonObj["token"] = _token;

        QJsonDocument doc(jsonObj);
        QByteArray jsonData = doc.toJson(QJsonDocument::Indented);

        // 发送 tcp 请求给 chat server
        emit TcpMgr::GetInstance()->sig_send_data(ReqId::ID_CHAT_LOGIN, jsonData);
    }else{
        showTip(tr("网络异常"),false);
        enableBtn(true);
    }
}

void LoginDialog::slot_login_failed(int err)
{
    QString result = QString("登录失败, err is %1").arg(err);
    showTip(result,false);
    enableBtn(true);
}

void LoginDialog::on_login_btn_clicked()
{
    if(checkEmailValid() == false){
        return;
    }
    if(checkPwdValid() == false){
        return;
    }

    auto email = ui->email_edit->text();
    auto passwd = ui->passwd_edit->text();

    enableBtn(false);

    showTip(tr("登录中......"), true);

    QJsonObject json_obj;
    json_obj["email"] = email;
    json_obj["passwd"] = passwd;
    HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix + ReqServlet::LOGIN_EMAIL())
        , json_obj, ReqId::ID_LOGIN_EMAIL, Modules::LOGINMOD);
}
