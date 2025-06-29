#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "logindialog.h"
#include "registerdialog.h"
#include "resetdialog.h"
#include "chatdialog.h"

namespace Ui {
class MainWindow;
}

enum UIStatus{
    LOGIN_UI,
    REGISTER_UI,
    RESET_UI,
    CHAT_UI
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void SlotSwitchReg();
    void SlotSwitchLog();       // 从 _register_dlg ---> _login_dlg
    void SlotSwitchLog2();      // 从 __reset_dlg ---> _login_dlg
    void SlotSwitchReset();
    void SlotSwitchChat();
    void slot_notify_offline();
    void slot_excepcon_offline();

private:
    void offline_switch_login();

private:
    Ui::MainWindow *ui;
    LoginDialog *_login_dlg;
    RegisterDialog *_register_dlg;
    ResetDialog *_reset_dlg;
    ChatDialog *_chat_dlg;
    UIStatus _ui_status;
};

#endif // MAINWINDOW_H
