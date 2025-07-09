#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tcpmgr.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _login_dlg = new LoginDialog(this);
    /*
     移除两个对话框的默认标题栏和边框

      Qt::FramelessWindowHint：
      移除窗口的系统边框（包括标题栏、最小化/最大化/关闭按钮、窗口边缘的调整大小边框）。
      窗口变为一个“光秃秃”的矩形区域，没有操作系统提供的任何装饰。

      Qt::CustomizeWindowHint：
      禁用默认的窗口标题栏（关闭按钮、标题文字等）。
      允许开发者完全自定义窗口的标题栏和边框（需手动实现）。
    */
    _login_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

    setWindowTitle("飞书");
    setCentralWidget(_login_dlg);       // 设置中心部件，填充主窗口
    _ui_status = LOGIN_UI;
    connect(_login_dlg, &LoginDialog::sigSwitchRegister, this, &MainWindow::SlotSwitchReg);

    // 忘记密码
    connect(_login_dlg, &LoginDialog::sigSwitchReset, this, &MainWindow::SlotSwitchReset);

    // 切换用户界面
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_swich_chatdlg, this, &MainWindow::SlotSwitchChat);

//    emit TcpMgr::GetInstance()->sig_swich_chatdlg();

    // 连接，接收到下线通知
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_notify_offline, this, &MainWindow::slot_notify_offline);

    // 连接，接收到 断开连接 通知
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_connection_closed, this, &MainWindow::slot_excepcon_offline);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SlotSwitchReg()
{
    _ui_status = REGISTER_UI;
    _register_dlg = new RegisterDialog(this);
    _register_dlg->hide();

    _register_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);


    setCentralWidget(_register_dlg);    // 因为这个 setCentralWidget 会把 原本的 widget回收，所以每次都得new

    connect(_register_dlg, &RegisterDialog::sigSwitchLogin, this, &MainWindow::SlotSwitchLog);

    _login_dlg->hide();
    _register_dlg->show();
}

void MainWindow::SlotSwitchLog()
{
    _ui_status = LOGIN_UI;
    _login_dlg = new LoginDialog(this); // 这里得 new
    _login_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    setCentralWidget(_login_dlg);

    connect(_login_dlg, &LoginDialog::sigSwitchRegister, this, &MainWindow::SlotSwitchReg);
    connect(_login_dlg, &LoginDialog::sigSwitchReset, this, &MainWindow::SlotSwitchReset);

    _register_dlg->hide();
    _login_dlg->show();
}

void MainWindow::SlotSwitchLog2()
{
    _ui_status = LOGIN_UI;
    _login_dlg = new LoginDialog(this); // 这里得 new
    _login_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    setCentralWidget(_login_dlg);

    connect(_login_dlg, &LoginDialog::sigSwitchRegister, this, &MainWindow::SlotSwitchReg);
    connect(_login_dlg, &LoginDialog::sigSwitchReset, this, &MainWindow::SlotSwitchReset);

    _reset_dlg->hide();
    _login_dlg->show();
}

void MainWindow::SlotSwitchReset()
{
    _ui_status = RESET_UI;
    _reset_dlg = new ResetDialog(this);
    _reset_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    setCentralWidget(_reset_dlg);

    connect(_reset_dlg, &ResetDialog::sigSwitchLogin, this, &MainWindow::SlotSwitchLog2);

    _login_dlg->hide();
    _reset_dlg->show();
}

void MainWindow::SlotSwitchChat()
{
    _ui_status = CHAT_UI;
    //流程2：初始化用户窗口，登录成功后好友消息，好友申请信息分别在 自定义控件 contactuserlist，applyfriendlist初始化完成。
    _chat_dlg = new ChatDialog(this);
    _chat_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    setCentralWidget(_chat_dlg);
    _login_dlg->hide();
    _chat_dlg->show();
    this->setMinimumSize(QSize(1080,800));
    this->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    //流程3：请求chatserver，增量加载聊天会话
    _chat_dlg->loadChatList();
}

void MainWindow::slot_notify_offline()
{
    QMessageBox::critical(
        this,                   // 父窗口指针（保证模态性）
        "下线提示",             // 弹窗标题
        "同账号异地登录，请重新登录", // 错误详情
        QMessageBox::Ok         // 显示"确定"按钮
    );
    TcpMgr::GetInstance()->CloseConnection();
    offline_switch_login();
}

void MainWindow::slot_excepcon_offline()
{
    QMessageBox::critical(
        this,                   // 父窗口指针（保证模态性）
        "下线提示",             // 弹窗标题
        "网络异常，请重新登录", // 错误详情
        QMessageBox::Ok         // 显示"确定"按钮
    );
    TcpMgr::GetInstance()->CloseConnection();
    offline_switch_login();
}

void MainWindow::offline_switch_login()
{
    if(_ui_status == LOGIN_UI){
        return;
    }
    _login_dlg = new LoginDialog(this); // 这里得 new
    _login_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    setCentralWidget(_login_dlg);

    _chat_dlg->hide();


    this->setMaximumSize(300,500);
    this->setMinimumSize(300,500);
    this->resize(300,500);
    _login_dlg->show();


    connect(_login_dlg, &LoginDialog::sigSwitchRegister, this, &MainWindow::SlotSwitchReg);
    connect(_login_dlg, &LoginDialog::sigSwitchReset, this, &MainWindow::SlotSwitchReset);
    _ui_status = LOGIN_UI;
}


