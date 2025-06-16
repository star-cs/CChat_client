#include "resetdialog.h"
#include "ui_resetdialog.h"

#include <QDebug>
#include <QRegularExpression>
#include "httpmgr.h"


ResetDialog::ResetDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ResetDialog)
{
    ui->setupUi(this);

    // 初始化 err_tip 属性
    ui->err_tip->setProperty("state", "normal");
    ui->err_tip->clear();

    ui->get_verfiy_btn->setEnabled(false);

    connect(ui->name_edit,&QLineEdit::editingFinished,this,[this](){
        checkAccountVaild();
    });
    connect(ui->email_edit, &QLineEdit::editingFinished, this, [this](){
        checkEmailVaild();
    });
    connect(ui->passwd_edit, &QLineEdit::editingFinished, this, [this](){
        checkPasswdValid();
    });
    connect(ui->verfiy_code_edit, &QLineEdit::editingFinished, this, [this](){
         checkVerifyCodeValid();
    });
    //连接reset相关信号和注册处理回调
    initHandlers();
    connect(HttpMgr::GetInstance().get(), &HttpMgr::sig_reset_mod_finish, this,
            &ResetDialog::slot_reset_mod_finish);

    //密码 可见性 label
    ui->passwd_visible->setCursor(Qt::PointingHandCursor);
    ui->passwd_visible->SetState("visible","visible_hover","","unvisible","unvisible_hover","");
    connect(ui->passwd_visible, &ClickedLabel::clicked, [this](){
        if(ui->passwd_visible->GetCurState() == ClickLbState::Normal){
            ui->passwd_edit->setEchoMode(QLineEdit::Password);
        }else{
            ui->passwd_edit->setEchoMode(QLineEdit::Normal);
        }
    });
}

ResetDialog::~ResetDialog()
{
    delete ui;
}

void ResetDialog::showTip(QString str, bool b_ok)
{
    if(b_ok){
        ui->err_tip->setProperty("state", "normal");
    }else{
        ui->err_tip->setProperty("state", "err");
    }
    ui->err_tip->setText(str);
    repolish(ui->err_tip);
}

void ResetDialog::AddTipErr(TipErr te, QString tips)
{
    _tip_errs[te] = tips;
    showTip(tips, false);
}

void ResetDialog::DelTipErr(TipErr te)
{
    _tip_errs.remove(te);
    if(_tip_errs.empty()){
      ui->err_tip->clear();
      return;
    }

    showTip(_tip_errs.first(), false);
}

void ResetDialog::initHandlers()
{
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

    _handlers.insert(ReqId::ID_RESET_PWD, [this](QJsonObject jsonObj){
        int error = jsonObj["error"].toInt();
        if(error != ErrorCodes::SUCCESS){
            showTip(tr("参数错误"), false);
            return;
        }
        auto email = jsonObj["email"].toString();
        showTip(tr("重置成功,点击返回登录"), true);
        qDebug()<< "email is " << email ;
        qDebug()<< "user uid is " <<  jsonObj["uid"].toString();
    });
}

bool ResetDialog::checkAccountVaild()
{
    if(ui->name_edit->text() == ""){
        AddTipErr(TipErr::TIP_USER_ERR, tr("用户名不能为空"));
        return false;
    }

    DelTipErr(TipErr::TIP_USER_ERR);
    return true;
}

bool ResetDialog::checkEmailVaild()
{
    //验证邮箱的地址正则表达式
    auto email = ui->email_edit->text();
    // 邮箱地址的正则表达式
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool match = regex.match(email).hasMatch(); // 执行正则表达式匹配
    if(!match){
        //提示邮箱不正确
        AddTipErr(TipErr::TIP_EMAIL_ERR, tr("邮箱地址不正确"));
        ui->get_verfiy_btn->setEnabled(false);

        return false;
    }
    DelTipErr(TipErr::TIP_EMAIL_ERR);
    ui->get_verfiy_btn->setEnabled(true);
    return true;
}

bool ResetDialog::checkPasswdValid()
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
    QRegularExpression regExp("^[a-zA-Z0-9!@#$%^&*.]{6,15}$");
    bool match = regExp.match(pass).hasMatch();
    if(!match){
        //提示字符非法
        AddTipErr(TipErr::TIP_PWD_ERR, tr("不能包含非法字符"));
        return false;;
    }

    DelTipErr(TipErr::TIP_PWD_ERR);

    return true;
}

bool ResetDialog::checkVerifyCodeValid()
{
    auto pass = ui->verfiy_code_edit->text();
    if(pass.isEmpty()){
        AddTipErr(TipErr::TIP_VARIFY_ERR, tr("验证码不能为空"));
        return false;
    }
    DelTipErr(TipErr::TIP_VARIFY_ERR);
    return true;
}

void ResetDialog::slot_reset_mod_finish(ReqId id, QString res, ErrorCodes err)
{
    if(err != ErrorCodes::SUCCESS){
        showTip(tr("网络请求错误"), false);
        return;
    }

    // 解析 json 字符串
    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());
    if(jsonDoc.isNull()){
        showTip(tr("json解析错误"),false);
        return;
    }

    //json解析错误
    if(!jsonDoc.isObject()){
        showTip(tr("json解析错误"),false);
        return;
    }

      //调用对应的逻辑,根据id回调。
    _handlers[id](jsonDoc.object());

    return;
}


void ResetDialog::on_ensure_btn_clicked()
{
    bool valid = checkAccountVaild();
    if(!valid){
        return;
    }

    valid = checkEmailVaild();
    if(!valid){
        return;
    }

    valid = checkPasswdValid();
    if(!valid){
        return;
    }

    valid = checkVerifyCodeValid();
    if(!valid){
        return;
    }

    //发送http重置用户请求
    QJsonObject json_obj;
    json_obj["name"] = ui->name_edit->text();
    json_obj["email"] = ui->email_edit->text();
    json_obj["passwd"] = ui->passwd_edit->text();
    json_obj["verifycode"] = ui->verfiy_code_edit->text();
    HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix + ReqServlet::RESET_PWD()),
                 json_obj, ReqId::ID_RESET_PWD,Modules::RESETMOD);
}

void ResetDialog::on_cancel_btn_clicked()
{
    emit ResetDialog::sigSwitchLogin();
}

void ResetDialog::on_get_verfiy_btn_clicked()
{
    bool b_verfiy = checkEmailVaild();
    if(b_verfiy){
        QJsonObject json_obj;
        json_obj["email"] = ui->email_edit->text();
        HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix + ReqServlet::GET_VERIFYCODE()),
                    json_obj, ReqId::ID_GET_VERIFY_CODE, Modules::RESETMOD);
    }
}
