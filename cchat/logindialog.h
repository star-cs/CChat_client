#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include "global.h"
namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = 0);
    ~LoginDialog();

private:
    // 初始化登录界面的头像
    void initHead();
    void initHttpHandlers();
    void showTip(QString str, bool b_ok);
    bool checkEmailValid();
    bool checkPwdValid();

    void AddTipErr(TipErr te,QString tips);
    void DelTipErr(TipErr te);

    // 登录等到回包，判断后，解开登录按钮~
    void enableBtn(bool enabled);

    int _uid;
    QString _token;

private:
    QMap<ReqId, std::function<void(const QJsonObject&)>> _handlers;
    QMap<TipErr, QString> _tip_errs;


private:
    Ui::LoginDialog *ui;

signals:
    void sigSwitchRegister();
    void sigSwitchReset();
    void sig_connect_tcp(ServerInfo);

private slots:
    void slot_login_mod_finish(ReqId id, QString res, ErrorCodes err);
    void slot_tcp_con_finish(bool bsuccess);
    void slot_login_failed(int err);

    void on_login_btn_clicked();
};

#endif // LOGINDIALOG_H
