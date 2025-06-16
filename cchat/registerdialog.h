#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>
#include <functional>
#include <QMap>
#include <QTimer>
#include "global.h"

namespace Ui {
class RegisterDialog;
}

class RegisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDialog(QWidget *parent = 0);
    ~RegisterDialog();

private slots:
    void on_get_verfiy_btn_clicked();

    void on_ensure_btn_clicked();

    void on_re_login_btn_clicked();

public slots:
    void slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err);

signals:
    void sigSwitchLogin();

private:
    void showTip(QString str, bool is_ok);
    void initHttpHandlers();
    void AddTipErr(TipErr te, QString tips);
    void DelTipErr(TipErr te);

    bool checkUserValid();
    bool checkEmailValid();
    bool checkPassValid();
    bool checkConfirmValid();
    bool checkVerifyValid();

    // 注册成功，切换到 提示页面
    void ChangeTipPage();

private:
    Ui::RegisterDialog *ui;

    QMap<ReqId, std::function<void(const QJsonObject&)>> _handlers;
    // 错误提示，缓存每一个错误类型，每次只显示一条。
    QMap<TipErr, QString> _tip_errs;

    QTimer* _counterdown_timer;
    int _countdown;
};

#endif // REGISTERDIALOG_H
