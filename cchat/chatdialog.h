#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QDialog>
#include "global.h"
#include "statewidget.h"
#include <QMap>
#include <QListWidgetItem>
#include "userdata.h"

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
    void loadMoreConUser();

private:
    void AddLBGroup(StateWidget *lb);
    void ClearLabelState(StateWidget *lb);
    void handleGlobalMousePress(QMouseEvent *event);

    // 根据用户ID（uid）选中聊天用户列表中的对应项，并更新当前聊天对象的UID。
    // 设置选中条目
    void SetSelectChatItem(int uid);
    // 根据选中的用户列表项，更新右侧聊天页面（chat_page）显示的用户信息。
    // 更新聊天界面信息
    void SetSelectChatPage(int uid);
    void UpdateChatMsg(std::vector<std::shared_ptr<TextChatData> > msgdata);

    void offline_switch_login();

protected:
    bool eventFilter(QObject *watched, QEvent *event);

public slots:
    void slot_item_clicked(QListWidgetItem *item);

    void slot_loading_chat_user();
    void slot_loading_contact_user();
    void slot_side_chat();
    void slot_side_contact();
    void slot_text_changed(const QString &str);
    void slot_show_search(bool search);
    void slot_switch_apply_friend_page();
    void slot_friend_info_page(int uid);

    void slot_jump_chat_item(int uid);
    void slot_jump_chat_item_from_infopage(int uid);

    void slot_apply_friend(std::shared_ptr<ApplyInfo> apply);

    void slot_text_msg_rsp(std::shared_ptr<TextChatMsg> msgdata);
    void slot_text_chat_msg(std::shared_ptr<TextChatMsg> msg);   // 接收到 他人发来的 聊天消息

private:
    Ui::ChatDialog *ui;

    QWidget* _last_contact_widget;      // 最右侧的 stackWidget 切换 contact_side 前的 page
    QWidget* _last_chat_widget;         // 切换 chat 之前
    ChatUIMode _mode;   // 实际状态的区别
    ChatUIMode _state;  // 左侧 side_ber 的 状态
    bool _b_loading;

    QList<StateWidget*> _lb_list;   // 最左侧的 状态栏 StateWidget

    // 所有的聊天记录 item
    QMap<int, QListWidgetItem*> _chat_items_added;

    // 当前所在的 聊天对象
//    int _cur_chat_uid;  直接通过 ui->chat_page 找 uid
    // 心跳包发送定时器
    QTimer* _timer;
};

#endif // CHATDIALOG_H
