#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "logindialog.h"
#include "registerdialog.h"
#include "resetdialog.h"

namespace Ui {
class MainWindow;
}

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

private:
    Ui::MainWindow *ui;
    LoginDialog *_login_dlg;
    RegisterDialog *_register_dlg;
    ResetDialog *_reset_dlg;
};

#endif // MAINWINDOW_H
