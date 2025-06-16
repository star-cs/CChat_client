#include "mainwindow.h"
#include "ui_mainwindow.h"

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
    connect(_login_dlg, &LoginDialog::sigSwitchRegister, this, &MainWindow::SlotSwitchReg);

    // 忘记密码
    connect(_login_dlg, &LoginDialog::sigSwitchReset, this, &MainWindow::SlotSwitchReset);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SlotSwitchReg()
{
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
    _reset_dlg = new ResetDialog(this);
    _reset_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    setCentralWidget(_reset_dlg);

    connect(_reset_dlg, &ResetDialog::sigSwitchLogin, this, &MainWindow::SlotSwitchLog2);

    _login_dlg->hide();
    _reset_dlg->show();
}

