#ifndef RESETDIALOG_H
#define RESETDIALOG_H

#include <functional>
#include <QDialog>
#include "global.h"

namespace Ui {
class ResetDialog;
}

class ResetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ResetDialog(QWidget *parent = 0);
    ~ResetDialog();

private:

    void showTip(QString str,bool b_ok);
    void AddTipErr(TipErr te,QString tips);
    void DelTipErr(TipErr te);

    void initHandlers();

    bool checkAccountVaild();
    bool checkEmailVaild();
    bool checkPasswdValid();
    bool checkVerifyCodeValid();

private:
    Ui::ResetDialog *ui;
    QMap<TipErr, QString> _tip_errs;
    QMap<ReqId, std::function<void(const QJsonObject&)>> _handlers;

private slots:
    void slot_reset_mod_finish(ReqId id, QString res, ErrorCodes err);

    void on_ensure_btn_clicked();

    void on_cancel_btn_clicked();

    void on_get_verfiy_btn_clicked();

signals:
    void sigSwitchLogin();



};

#endif // RESETDIALOG_H
