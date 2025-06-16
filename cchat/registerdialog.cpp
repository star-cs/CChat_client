#include "registerdialog.h"
#include "ui_registerdialog.h"

#include "global.h"
#include "httpmgr.h"

RegisterDialog::RegisterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegisterDialog),
    _countdown(5)
{
    ui->setupUi(this);

    /*  setEchoMode 设置lineEdit显示
        QLineEdit::Normal 正常
        QLineEdit::Password  小黑点 密码
        QLineEdit::PasswordEchoOnEdit 编辑时输入字符显示输入内容，否则用小黑点代替
        QLineEdit::NoEcho 任何输入都看不见
    */
    ui->passwd_edit->setEchoMode(QLineEdit::Password);
    ui->confirm_edit->setEchoMode(QLineEdit::Password);

    // 设置 err_tip qss 样式 种类
    ui->err_tip->setProperty("state", "normal");
    repolish(ui->err_tip);
    ui->err_tip->clear();

    // 错误样式 绑定
    connect(ui->name_edit, &QLineEdit::editingFinished,this,[this](){
        checkUserValid();
    });
    connect(ui->email_edit, &QLineEdit::editingFinished, this, [this](){
        checkEmailValid();
    });
    connect(ui->passwd_edit, &QLineEdit::editingFinished, this, [this](){
        checkPassValid();
    });
    connect(ui->confirm_edit, &QLineEdit::editingFinished, this, [this](){
        checkConfirmValid();
    });
    connect(ui->verify_edit, &QLineEdit::editingFinished, this, [this](){
         checkVerifyValid();
    });

    connect(HttpMgr::GetInstance().get(), &HttpMgr::sig_reg_mod_finish,
            this, &RegisterDialog::slot_reg_mod_finish);

    connect(ui->cancel_btn, &QPushButton::clicked, this, &RegisterDialog::sigSwitchLogin);

    initHttpHandlers();

    // 设置初始 获取 验证码按钮为 不可见
    ui->get_verfiy_btn->setEnabled(false);

    // 设置浮动显示手形状
    ui->confirm_visible->setCursor(Qt::PointingHandCursor);
    ui->passwd_visible->setCursor(Qt::PointingHandCursor);

    // 设置 passwd_visible，confrim_visible qss状态
    // 默认是不可见，normal
    ui->passwd_visible->SetState("unvisible","unvisible_hover","","visible",
                            "visible_hover","");
    ui->confirm_visible->SetState("unvisible","unvisible_hover","","visible",
                                "visible_hover","");

    //连接点击事件
    connect(ui->passwd_visible, &ClickedLabel::clicked, this, [this](){
        auto state = ui->passwd_visible->GetCurState();
        if(state == ClickLbState::Normal){
            ui->passwd_edit->setEchoMode(QLineEdit::Password);
        }else{
            ui->passwd_edit->setEchoMode(QLineEdit::Normal);
        }
        qDebug() << "Label was clicked!";
    });


    connect(ui->confirm_visible, &ClickedLabel::clicked, this, [this]() {
        auto state = ui->confirm_visible->GetCurState();
        if(state == ClickLbState::Normal){
            ui->confirm_edit->setEchoMode(QLineEdit::Password);
        }else{
            ui->confirm_edit->setEchoMode(QLineEdit::Normal);
        }
        qDebug() << "Label was clicked!";
    });

    // 设置 StackedWidget 最初是第一个page1
    ui->StackedWidget->setCurrentWidget(ui->page1);

    // page2 的定时器
    _counterdown_timer = new QTimer(this);

    // 注册成功，切换页面
    connect(_counterdown_timer, &QTimer::timeout, [this](){
        if(_countdown == 0){
            _counterdown_timer->stop();
            emit sigSwitchLogin();
            return;
        }
        _countdown--;
        auto str = QString("注册成功，%1 s后自动返回登录").arg(_countdown);
        ui->tip_lb->setText(str);
    });

}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}


void RegisterDialog::showTip(QString str, bool is_ok)
{
    if(is_ok){
        ui->err_tip->setProperty("state", "normal");
    }else{
        ui->err_tip->setProperty("state", "err");
    }

    ui->err_tip->setText(str);
}

void RegisterDialog::initHttpHandlers()
{
    // 注册获取验证码回包逻辑
    _handlers.insert(ReqId::ID_GET_VERIFY_CODE, [this](QJsonObject jsonObj){
        int error = jsonObj["error"].toInt();
        if(error != ErrorCodes::SUCCESS){
            showTip(tr("参数错误"), false);
            return;
        }
        auto email = jsonObj["email"].toString();
        showTip(tr("验证码已发送到邮箱，注意查收"), true);
        qDebug()<< "email is " << email ;
    });

    //注册注册用户回包逻辑
    _handlers.insert(ReqId::ID_REG_USER, [this](QJsonObject jsonObj){
        int error = jsonObj["error"].toInt();
        if(error != ErrorCodes::SUCCESS){
            showTip(tr("参数错误"), false);
            return;
        }
        auto email = jsonObj["email"].toString();
        showTip(tr("用户注册成功"), true);
        qDebug()<< "email is " << email ;
        qDebug()<< "user uuid is " << jsonObj["uid"].toString();
        ChangeTipPage();
    });
}

void RegisterDialog::AddTipErr(TipErr te, QString tips)
{
    _tip_errs[te] = tips;
    showTip(tips, false);
}

void RegisterDialog::DelTipErr(TipErr te)
{
    _tip_errs.remove(te);
    if(_tip_errs.empty()){
        ui->err_tip->clear();
        return;
    }
    showTip(_tip_errs.first(), false);
}

bool RegisterDialog::checkUserValid()
{
    if(ui->name_edit->text() == ""){
        AddTipErr(TipErr::TIP_USER_ERR, tr("用户名不能为空"));
        return false;
    }
    DelTipErr(TipErr::TIP_USER_ERR);
    return true;
}

bool RegisterDialog::checkPassValid()
{
    auto pass = ui->passwd_edit->text();
    if(pass.length() < 6 || pass.length()>15){
        //提示长度不准确
        AddTipErr(TipErr::TIP_PWD_ERR, tr("密码长度应为6~15"));
        return false;
    }
    // 创建一个正则表达式对象，按照上述密码要求
    // 这个正则表达式解释：
    // ^[a-zA-Z0-9!@#$%^&*]{6,15}$ 密码长度至少6，可以是字母、数字和特定的特殊字符
    QRegularExpression regExp("^[a-zA-Z0-9!@#$%^&*]{6,15}$");
    bool match = regExp.match(pass).hasMatch();
    if(!match){
        //提示字符非法
        AddTipErr(TipErr::TIP_PWD_ERR, tr("不能包含非法字符"));
        return false;;
    }
    DelTipErr(TipErr::TIP_PWD_ERR);

    // 如果 重复密码有数据，还得重查一遍
    if(!ui->confirm_edit->text().isEmpty()){
        checkConfirmValid();
    }

    return true;
}

bool RegisterDialog::checkConfirmValid()
{
    auto pass = ui->passwd_edit->text();
    auto confirm = ui->confirm_edit->text();

    if(confirm.length() < 6 || confirm.length() > 15 ){
        //提示长度不准确
        AddTipErr(TipErr::TIP_CONFIRM_ERR, tr("密码长度应为6~15"));
        return false;
    }

    // 创建一个正则表达式对象，按照上述密码要求
    // 这个正则表达式解释：
    // ^[a-zA-Z0-9!@#$%^&*]{6,15}$ 密码长度至少6，可以是字母、数字和特定的特殊字符
    QRegularExpression regExp("^[a-zA-Z0-9!@#$%^&*.]{6,15}$");
    bool match = regExp.match(confirm).hasMatch();
    if(!match){
        //提示字符非法
        AddTipErr(TipErr::TIP_CONFIRM_ERR, tr("不能包含非法字符"));
        return false;
    }

    DelTipErr(TipErr::TIP_CONFIRM_ERR);

    if(pass != confirm){
        //提示密码不匹配
        AddTipErr(TipErr::TIP_PWD_CONFIRM, tr("确认密码和密码不匹配"));
        return false;
    }else{
       DelTipErr(TipErr::TIP_PWD_CONFIRM);
    }
    return true;
}

bool RegisterDialog::checkEmailValid()
{
    auto email = ui->email_edit->text();
    QRegularExpression regex(
        R"(^[a-zA-Z0-9.!#$%&'*+\/=?^_`{|}~-]+)"  // 用户名部分
        R"(@[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?)"  // 域名开头
        R"(\.[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?$)"  // 顶级域名
    );
    bool match = regex.match(email).hasMatch();
    if(!match){
        // 提示邮箱错误
        AddTipErr(TipErr::TIP_EMAIL_ERR, tr("邮箱地址不正确"));
        ui->get_verfiy_btn->setEnabled(false);
        return false;
    }
    DelTipErr(TipErr::TIP_EMAIL_ERR);
    ui->get_verfiy_btn->setEnabled(true);
    return true;
}

bool RegisterDialog::checkVerifyValid()
{
    auto pass = ui->verify_edit->text();
    if(pass.isEmpty()){
        AddTipErr(TipErr::TIP_VARIFY_ERR, tr("验证码不能为空"));
        return false;
    }
    DelTipErr(TipErr::TIP_VARIFY_ERR);
    return true;
}

void RegisterDialog::ChangeTipPage()
{
    _counterdown_timer->stop();
    ui->StackedWidget->setCurrentWidget(ui->page2);

    // 启动定时器，间隔1000毫秒
    _counterdown_timer->start(1000);
}

void RegisterDialog::on_get_verfiy_btn_clicked()
{
    bool res = checkEmailValid();
    if(res){
        auto email = ui->email_edit->text();
        QJsonObject json_obj;
        json_obj["email"] = email;
        HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix + ReqServlet::GET_VERIFYCODE())
            , json_obj, ReqId::ID_GET_VERIFY_CODE, Modules::REGISTERMOD);
    }
}

void RegisterDialog::slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err)
{
    if(err != ErrorCodes::SUCCESS){
        showTip(tr("网络请求错误"),false);
        return;
    }

    // 解析 JSON 字符串,res需转化为QByteArray
    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());
    // json 解析错误
    if(jsonDoc.isNull()){
        showTip(tr("json解析错误"), false);
        return;
    }

    //json解析错误
    if(!jsonDoc.isObject()){
        showTip(tr("json解析错误"),false);
        return;
    }

//    QJsonObject jsonObj = jsonDoc.object();
    // 处理 _handlers 对应 ReqId 的回调函数
    _handlers[id](jsonDoc.object());
    return;
}

void RegisterDialog::on_ensure_btn_clicked()
{
    if(ui->name_edit->text() == ""){
        showTip(tr("用户名不能为空"), false);
        return;
    }
    if(ui->email_edit->text() == ""){
        showTip(tr("邮箱不能为空"), false);
        return;
    }
    if(ui->passwd_edit->text() == ""){
        showTip(tr("密码不能为空"), false);
        return;
    }
    if(ui->confirm_edit->text() == ""){
        showTip(tr("确认密码不能为空"), false);
        return;
    }
    if(ui->confirm_edit->text() != ui->passwd_edit->text()){
        showTip(tr("密码和确认密码不匹配"), false);
        return;
    }
    if(ui->verify_edit->text() == ""){
        showTip(tr("验证码不能为空"), false);
        return;
    }

    QJsonObject json_obj;
    json_obj["name"] = ui->name_edit->text();
    json_obj["email"] = ui->email_edit->text();
    json_obj["passwd"] = ui->passwd_edit->text();
    json_obj["confirm"] = ui->confirm_edit->text();
    json_obj["verifycode"] = ui->verify_edit->text();

    HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix + ReqServlet::REGISTER()), json_obj, ReqId::ID_REG_USER, Modules::REGISTERMOD);
}

void RegisterDialog::on_re_login_btn_clicked()
{
    _counterdown_timer->stop();
    emit RegisterDialog::sigSwitchLogin();
}
