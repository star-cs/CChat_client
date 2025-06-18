#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QDialog>
#include "global.h"

namespace Ui {
class ChatDialog;
}

class ChatDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChatDialog(QWidget *parent = 0);
    ~ChatDialog();
    void ShowSearch(bool bsearch);
    void addChatUserList();

public slots:
    void slot_loading_chat_user();

private:
    Ui::ChatDialog *ui;

    ChatUIMode _mode;   // 实际状态的区别
    ChatUIMode _state;  // 左侧 side_ber 的 状态
    bool _b_loading;
};

#endif // CHATDIALOG_H
