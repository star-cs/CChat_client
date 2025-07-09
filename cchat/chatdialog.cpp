#include "chatdialog.h"
#include "ui_chatdialog.h"
#include <QAction>
#include "chatuserwid.h"
#include <QDebug>
#include <vector>
#include <QRandomGenerator>
#include "loadingdlg.h"
#include "global.h"
#include "ChatItemBase.h"
#include "TextBubble.h"
#include "PictureBubble.h"
#include "MessageTextEdit.h"
#include "chatuserlist.h"
#include "grouptipitem.h"
#include "invaliditem.h"
#include "conuseritem.h"
#include "lineitem.h"
#include "tcpmgr.h"
#include "usermgr.h"
#include <QTimer>


ChatDialog::ChatDialog(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::ChatDialog), _b_loading(false), _mode(ChatUIMode::ChatMode),
    _state(ChatUIMode::ChatMode), _last_widget(nullptr),
    _cur_chat_thread_id(0), _loading_dlg(nullptr)
{
    ui->setupUi(this);

    ui->add_btn->SetState("normal", "hover", "press");
    ui->add_btn->setProperty("state", "normal");
    QAction* searchAction = new QAction(ui->search_edit);
    searchAction->setIcon(QIcon(":/res/search.png"));
    ui->search_edit->addAction(searchAction, QLineEdit::LeadingPosition);
    ui->search_edit->setPlaceholderText(QStringLiteral("搜索"));

    // 创建一个清除动作并设置图标
    QAction* clearAction = new QAction(ui->search_edit);
    clearAction->setIcon(QIcon(":/res/close_transparent.png"));
    // 初始时不显示清除图标
    // 将清除动作添加到LineEdit的末尾位置
    ui->search_edit->addAction(clearAction, QLineEdit::TrailingPosition);

    // 当需要显示清除图标时，更改为实际的清除图标
    connect(ui->search_edit, &QLineEdit::textChanged, [clearAction](const QString& text) {
        if (!text.isEmpty()) {
            clearAction->setIcon(QIcon(":/res/close_search.png"));
        }
        else {
            clearAction->setIcon(QIcon(":/res/close_transparent.png")); // 文本为空时，切换回透明图标
        }

        });

    //
    connect(clearAction, &QAction::triggered, [this, clearAction]() {
        ui->search_edit->clear();
        clearAction->setIcon(QIcon(":/res/close_transparent.png")); // 清除文本后，切换回透明图标
        ui->search_edit->clearFocus();
        //清除按钮被按下则不显示搜索框
        ShowSearch(false);
        });

    ui->search_edit->SetMaxLength(15);

    //连接 加载联系人
    connect(ui->chat_user_list, &ChatUserList::sig_loading_chat_user, this, &ChatDialog::slot_loading_chat_user);
    //模拟加载自己头像
    QString head_icon = UserMgr::GetInstance()->GetIcon();
    QPixmap pixmap(head_icon); // 加载图片
    QPixmap scaledPixmap = pixmap.scaled(ui->side_head_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation); // 将图片缩放到label的大小
    ui->side_head_lb->setPixmap(scaledPixmap); // 将缩放后的图片设置到QLabel上
    ui->side_head_lb->setScaledContents(true); // 设置QLabel自动缩放图片内容以适应大小

    ui->side_chat_lb->setProperty("state", "normal");
    ui->side_chat_lb->SetState("normal", "hover", "pressed", "selected_normal", "selected_hover", "selected_pressed");
    ui->side_contact_lb->SetState("normal", "hover", "pressed", "selected_normal", "selected_hover", "selected_pressed");
    ui->side_settings_lb->SetState("normal", "hover", "pressed", "selected_normal", "selected_hover", "selected_pressed");

    AddLBGroup(ui->side_chat_lb);
    AddLBGroup(ui->side_contact_lb);
    AddLBGroup(ui->side_settings_lb);

    connect(ui->side_chat_lb, &StateWidget::clicked, this, &ChatDialog::slot_side_chat);
    connect(ui->side_contact_lb, &StateWidget::clicked, this, &ChatDialog::slot_side_contact);
    connect(ui->side_settings_lb, &StateWidget::clicked, this, &ChatDialog::slot_side_setting);

    //链接搜索框输入变化
    connect(ui->search_edit, &QLineEdit::textChanged, this, &ChatDialog::slot_text_changed);

    ShowSearch(false);

    //检测鼠标点击位置判断是否要清空搜索框
    this->installEventFilter(this); // 安装事件过滤器

    //设置聊天label选中状态
    ui->side_chat_lb->SetSelected(true);
    //设置选中条目
    SetSelectChatItem();
    //更新聊天界面信息
    SetSelectChatPage();

    //连接加载联系人的信号和槽函数
    connect(ui->con_user_list, &ContactUserList::sig_loading_contact_user,
        this, &ChatDialog::slot_loading_contact_user);

    //连接联系人页面点击好友申请条目的信号
    connect(ui->con_user_list, &ContactUserList::sig_switch_apply_friend_page,
        this, &ChatDialog::slot_switch_apply_friend_page);

    //连接清除搜索框操作
    connect(ui->friend_apply_page, &ApplyFriendPage::sig_show_search, this, &ChatDialog::slot_show_search);

    //为searchlist 设置search edit
    ui->search_list->SetSearchEdit(ui->search_edit);

    //连接申请添加好友信号
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_friend_apply, this, &ChatDialog::slot_apply_friend);

    //连接认证添加好友信号
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_add_auth_friend, this, &ChatDialog::slot_add_auth_friend);

    //链接自己认证回复信号
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_auth_rsp, this,
        &ChatDialog::slot_auth_rsp);

    //连接点击联系人item发出的信号和用户信息展示槽函数
    connect(ui->con_user_list, &ContactUserList::sig_switch_friend_info_page,
        this, &ChatDialog::slot_friend_info_page);

    //设置中心部件为chatpage
    ui->stackedWidget->setCurrentWidget(ui->chat_page);

    //连接searchlist跳转聊天信号
    connect(ui->search_list, &SearchList::sig_jump_chat_item, this, &ChatDialog::slot_jump_chat_item);

    //连接好友信息界面发送的点击事件
    connect(ui->friend_info_page, &FriendInfoPage::sig_jump_chat_item, this,
        &ChatDialog::slot_jump_chat_item_from_infopage);

    //连接聊天列表点击信号
    connect(ui->chat_user_list, &QListWidget::itemClicked, this, &ChatDialog::slot_item_clicked);

    //连接对端消息通知
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_text_chat_msg,
        this, &ChatDialog::slot_text_chat_msg);

    _timer = new QTimer(this);
    connect(_timer, &QTimer::timeout, this, [this]() {
        auto user_info = UserMgr::GetInstance()->GetUserInfo();
        QJsonObject textObj;
        textObj["fromuid"] = user_info->_uid;
        QJsonDocument doc(textObj);
        QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
        emit TcpMgr::GetInstance()->sig_send_data(ReqId::ID_HEART_BEAT_REQ, jsonData);
        });

    _timer->start(10000);

    //连接tcp返回的加载聊天回复
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_load_chat_thread,
        this, &ChatDialog::slot_load_chat_thread);

    //连接tcp返回的创建私聊的回复
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_create_private_chat,
        this, &ChatDialog::slot_create_private_chat);

    // 加载 会话id 消息 回复
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_load_chat_msg, this, &ChatDialog::slot_load_chat_msg);
}

ChatDialog::~ChatDialog()
{
    _timer->stop();
    delete ui;
}

void ChatDialog::loadChatList()
{
    showLoadingDlg(true);

    //发送请求逻辑
    QJsonObject jsonObj;
    auto uid = UserMgr::GetInstance()->GetUid();
    jsonObj["uid"] = uid;
    int last_chat_thread_id = UserMgr::GetInstance()->GetLastChatThreadId();
    jsonObj["thread_id"] = last_chat_thread_id;

    QJsonDocument doc(jsonObj);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);

    //发送tcp请求给chat server
    emit TcpMgr::GetInstance()->sig_send_data(ReqId::ID_LOAD_CHAT_THREAD_REQ, jsonData);
}

void ChatDialog::slot_item_clicked(QListWidgetItem* item)
{
    QWidget* widget = ui->chat_user_list->itemWidget(item); // 获取自定义widget对象
    if (!widget) {
        qDebug() << "slot item clicked widget is nullptr";
        return;
    }

    // 对自定义widget进行操作， 将item 转化为基类ListItemBase
    ListItemBase* customItem = qobject_cast<ListItemBase*>(widget);
    if (!customItem) {
        qDebug() << "slot item clicked widget is nullptr";
        return;
    }

    auto itemType = customItem->GetItemType();
    if (itemType == ListItemType::INVALID_ITEM
        || itemType == ListItemType::GROUP_TIP_ITEM) {
        qDebug() << "slot invalid item clicked ";
        return;
    }


    if (itemType == ListItemType::CHAT_USER_ITEM) {
        // 创建对话框，提示用户
        qDebug() << "contact user item clicked ";

        auto chat_wid = qobject_cast<ChatUserWid*>(customItem);
        auto chat_data = chat_wid->GetChatData();
        //跳转到聊天界面
        ui->chat_page->SetChatData(chat_data);
        _cur_chat_thread_id = chat_data->GetThreadId();
        return;
    }
}

//todo 添加接收到的通知聊天消息
void ChatDialog::slot_text_chat_msg(std::vector<std::shared_ptr<TextChatData>> msglists)
{
}


bool ChatDialog::eventFilter(QObject* watched, QEvent* event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        handleGlobalMousePress(mouseEvent);
    }
    return QDialog::eventFilter(watched, event);
}

void ChatDialog::handleGlobalMousePress(QMouseEvent* event)
{
    // 实现点击位置的判断和处理逻辑
    // 先判断是否处于搜索模式，如果不处于搜索模式则直接返回
    if (_mode != ChatUIMode::SearchMode) {
        return;
    }

    // 将鼠标点击位置转换为搜索列表坐标系中的位置
    QPoint posInSearchList = ui->search_list->mapFromGlobal(event->globalPos());
    // 判断点击位置是否在聊天列表的范围内
    if (!ui->search_list->rect().contains(posInSearchList)) {
        // 如果不在聊天列表内，清空输入框
        ui->search_edit->clear();
        ShowSearch(false);
    }
}

void ChatDialog::CloseFindDlg()
{
    ui->search_list->CloseFindDlg();
}

void ChatDialog::UpdateChatMsg(std::vector<std::shared_ptr<TextChatData> > msgdata)
{
    for (auto& msg : msgdata) {
        if (msg->GetThreadId() != _cur_chat_thread_id) {
            break;
        }

        ui->chat_page->AppendChatMsg(msg);
    }
}



void ChatDialog::slot_load_chat_thread(bool load_more, int last_thread_id,
    std::vector<std::shared_ptr<ChatThreadInfo>> chat_threads)
{
    //流程 4： 加载聊天会话
    for (auto& cti : chat_threads) {
        //先处理单聊，群聊跳过，以后添加
        if (cti->_type == "group") {
            // todo，群聊的话，服务器还得发送 群聊的基础信息
            continue;
        }

        auto uid = UserMgr::GetInstance()->GetUid();
        auto other_uid = 0;
        if (uid == cti->_user1_id) {
            other_uid = cti->_user2_id;
        }
        else {
            other_uid = cti->_user1_id;
        }

        // 新建会话实例
        auto chat_thread_data = std::make_shared<ChatThreadData>(other_uid, cti->_thread_id, 0);
        UserMgr::GetInstance()->AddChatThreadData(chat_thread_data, other_uid);

        // todo 会话item
        auto* chat_user_wid = new ChatUserWid();
        // 备注：每个chatUserItem里存储着聊天记录，点击item后，会传递给 chatPage
        chat_user_wid->SetChatData(chat_thread_data);
        QListWidgetItem* item = new QListWidgetItem;
        //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
        item->setSizeHint(chat_user_wid->sizeHint());
        ui->chat_user_list->addItem(item);
        ui->chat_user_list->setItemWidget(item, chat_user_wid);

        // 记录 会话id -- 会话item
        _chat_thread_items.insert(cti->_thread_id, item);
    }

    // 更新最新加载的会话id
    UserMgr::GetInstance()->SetLastChatThreadId(last_thread_id);

    if (load_more) {
        //发送请求逻辑
        QJsonObject jsonObj;
        auto uid = UserMgr::GetInstance()->GetUid();
        jsonObj["uid"] = uid;
        jsonObj["thread_id"] = last_thread_id;

        QJsonDocument doc(jsonObj);
        QByteArray jsonData = doc.toJson(QJsonDocument::Compact);

        //发送tcp请求给chat server
        emit TcpMgr::GetInstance()->sig_send_data(ReqId::ID_LOAD_CHAT_THREAD_REQ, jsonData);
        return;
    }

    //更新聊天界面信息
    SetSelectChatItem();
    SetSelectChatPage();
    showLoadingDlg(false);

    //流程 5: 继续加载聊天数据
    loadChatMsg();
}

void ChatDialog::slot_create_private_chat(int uid, int other_id, int thread_id)
{
    auto* chat_user_wid = new ChatUserWid();
    auto chat_thread_data = std::make_shared<ChatThreadData>(other_id, thread_id, 0);
    UserMgr::GetInstance()->AddChatThreadData(chat_thread_data, other_id);

    chat_user_wid->SetChatData(chat_thread_data);
    QListWidgetItem* item = new QListWidgetItem;
    item->setSizeHint(chat_user_wid->sizeHint());
    qDebug() << "chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    ui->chat_user_list->insertItem(0, item);
    ui->chat_user_list->setItemWidget(item, chat_user_wid);
    _chat_thread_items.insert(thread_id, item);

    ui->side_chat_lb->SetSelected(true);
    SetSelectChatItem(thread_id);
    //更新聊天界面信息
    SetSelectChatPage(thread_id);
    slot_side_chat();
    return;
}

void ChatDialog::slot_load_chat_msg(int thread_id, int msg_id, bool load_more, std::vector<std::shared_ptr<TextChatData>> msglists)
{
    //流程 5: 继续加载聊天数据
    //备注：加载聊天数据，通过直接插入UserMgr， 最后通过 chatUserItem updateLastMsg lastMsg显示
    _cur_load_chat->SetLastMsgId(msg_id);
    //加载聊天信息，同步到 UserMgr
    for(auto& msg : msglists){
        _cur_load_chat->AddMsg(msg);
    }

    //当前会话 还有未加载完的消息，就继续加载
    if (load_more) {
        //发送请求给服务器
        //发送请求逻辑
        QJsonObject jsonObj;
        jsonObj["thread_id"] = _cur_load_chat->GetThreadId();
        jsonObj["message_id"] = _cur_load_chat->GetLastMsgId();

        QJsonDocument doc(jsonObj);
        QByteArray jsonData = doc.toJson(QJsonDocument::Compact);

        //发送tcp请求给chat server
        emit TcpMgr::GetInstance()->sig_send_data(ReqId::ID_LOAD_CHAT_MSG_REQ, jsonData);
        return;
    }

    // 加载完 _cur_load_chat 后， 需要更新 对应会话 item显示
    // 备注： QListWidgetItem 通过 QListWidget->itemWidget 得到 QWidget基类，再通过 qobject_cast 转为 自定义控件 ChatUserWid
    QWidget* wid = ui->chat_user_list->itemWidget(_chat_thread_items[_cur_load_chat->_thread_id]);
    ChatUserWid* chatUser = qobject_cast<ChatUserWid*>(wid);
    if(chatUser){
        chatUser->updateLastMsg();
    }

    //获取下一个chat_thread
    _cur_load_chat = UserMgr::GetInstance()->GetNextLoadData();
    //都加载完了
    if(!_cur_load_chat){
        //更新聊天界面信息
        SetSelectChatItem();
        SetSelectChatPage();
        showLoadingDlg(false);
        return;
    }

    //继续加载下一个聊天，发送请求给服务器
    QJsonObject jsonObj;
    jsonObj["thread_id"] = _cur_load_chat->GetThreadId();
    jsonObj["message_id"] = _cur_load_chat->GetLastMsgId();

    QJsonDocument doc(jsonObj);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);

    //发送tcp请求给chat server
    emit TcpMgr::GetInstance()->sig_send_data(ReqId::ID_LOAD_CHAT_MSG_REQ, jsonData);
}

void ChatDialog::showLoadingDlg(bool show)
{
    if (show) {
        if (_loading_dlg) {
            _loading_dlg->deleteLater();
            return;
        }
        _loading_dlg = new LoadingDlg(this, "正在加载聊天列表...");
        _loading_dlg->setModal(true);
        _loading_dlg->show();
        return;
    }

    if (_loading_dlg) {
        _loading_dlg->deleteLater();
        _loading_dlg = nullptr;
    }

}

void ChatDialog::AddLBGroup(StateWidget* lb)
{
    _lb_list.push_back(lb);
}

// 不需要加载了，前期服务器请求会话，全部加载啦~
void ChatDialog::loadMoreChatUser() {

//    auto thread_list = UserMgr::GetInstance()->GetChatListPerPage();
//    if(thread_list.empty() == false){
//        for(auto& thread : thread_list){
//            auto
//        }
//    }

//    auto friend_list = UserMgr::GetInstance()->GetChatListPerPage();
//    if (friend_list.empty() == false) {
//        for (auto& friend_ele : friend_list) {
//            auto find_iter = _chat_items_added.find(friend_ele->_uid);
//            if (find_iter != _chat_items_added.end()) {
//                continue;
//            }
//            auto* chat_user_wid = new ChatUserWid();
//            auto user_info = std::make_shared<UserInfo>(friend_ele);
//            chat_user_wid->SetInfo(user_info);
//            QListWidgetItem* item = new QListWidgetItem;
//            //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
//            item->setSizeHint(chat_user_wid->sizeHint());
//            ui->chat_user_list->addItem(item);
//            ui->chat_user_list->setItemWidget(item, chat_user_wid);
//            _chat_items_added.insert(friend_ele->_uid, item);
//        }

//        //更新已加载条目
//        UserMgr::GetInstance()->UpdateChatLoadedCount();
//    }
}


void ChatDialog::ClearLabelState(StateWidget* lb)
{
    for (auto& ele : _lb_list) {
        if (ele == lb) {
            continue;
        }

        ele->ClearState();
    }
}

// 加载联系人
void ChatDialog::loadMoreConUser()
{
    auto friend_list = UserMgr::GetInstance()->GetConListPerPage();
    if (friend_list.empty() == false) {
        for (auto& friend_ele : friend_list) {
            auto* chat_user_wid = new ConUserItem();
            chat_user_wid->SetInfo(friend_ele);
            QListWidgetItem* item = new QListWidgetItem;
            //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
            item->setSizeHint(chat_user_wid->sizeHint());
            ui->con_user_list->addItem(item);
            ui->con_user_list->setItemWidget(item, chat_user_wid);
        }

        //更新已加载条目
        UserMgr::GetInstance()->UpdateContactLoadedCount();
    }
}

void ChatDialog::SetSelectChatItem(int thread_id)
{
    if (ui->chat_user_list->count() <= 0) {
        return;
    }

    if (thread_id == 0) {
        ui->chat_user_list->setCurrentRow(0);
        QListWidgetItem* firstItem = ui->chat_user_list->item(0);
        if (!firstItem) {
            return;
        }

        //转为widget
        QWidget* widget = ui->chat_user_list->itemWidget(firstItem);
        if (!widget) {
            return;
        }

        auto con_item = qobject_cast<ChatUserWid*>(widget);
        if (!con_item) {
            return;
        }

        _cur_chat_thread_id = con_item->GetChatData()->GetThreadId();

        return;
    }

    auto find_iter = _chat_thread_items.find(thread_id);
    if (find_iter == _chat_thread_items.end()) {
        qDebug() << "thread_id [" << thread_id << "] not found, set curent row 0";
        ui->chat_user_list->setCurrentRow(0);
        return;
    }

    ui->chat_user_list->setCurrentItem(find_iter.value());

    _cur_chat_thread_id = thread_id;
}

void ChatDialog::SetSelectChatPage(int thread_id)
{
    if (ui->chat_user_list->count() <= 0) {
        return;
    }

    if (thread_id == 0) {
        auto item = ui->chat_user_list->item(0);
        //转为widget
        QWidget* widget = ui->chat_user_list->itemWidget(item);
        if (!widget) {
            return;
        }

        auto con_item = qobject_cast<ChatUserWid*>(widget);
        if (!con_item) {
            return;
        }

        //设置信息
        auto chat_data = con_item->GetChatData();
        ui->chat_page->SetChatData(chat_data);
        return;
    }

    auto find_iter = _chat_thread_items.find(thread_id);
    if (find_iter == _chat_thread_items.end()) {
        return;
    }

    //转为widget
    QWidget* widget = ui->chat_user_list->itemWidget(find_iter.value());
    if (!widget) {
        return;
    }

    //判断转化为自定义的widget
    // 对自定义widget进行操作， 将item 转化为基类ListItemBase
    ListItemBase* customItem = qobject_cast<ListItemBase*>(widget);
    if (!customItem) {
        qDebug() << "qobject_cast<ListItemBase*>(widget) is nullptr";
        return;
    }

    auto itemType = customItem->GetItemType();
    if (itemType == CHAT_USER_ITEM) {
        auto con_item = qobject_cast<ChatUserWid*>(customItem);
        if (!con_item) {
            return;
        }

        //设置信息
        auto chat_data = con_item->GetChatData();
        ui->chat_page->SetChatData(chat_data);

        return;
    }

}

void ChatDialog::loadChatMsg()
{
    //流程 5: 继续加载聊天数据
    _cur_load_chat = UserMgr::GetInstance()->GetCurLoadData();
    if(_cur_load_chat == nullptr){
        return;
    }

    showLoadingDlg(true);

    QJsonObject jsonObj;
    jsonObj["thread_id"] = _cur_load_chat->GetThreadId();
    jsonObj["message_id"] = _cur_load_chat->GetLastMsgId();

    QJsonDocument doc(jsonObj);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);

    emit TcpMgr::GetInstance()->sig_send_data(ReqId::ID_LOAD_CHAT_MSG_REQ, jsonData);
}


void ChatDialog::ShowSearch(bool bsearch)
{
    if (bsearch) {
        ui->chat_user_list->hide();
        ui->con_user_list->hide();
        ui->search_list->show();
        _mode = ChatUIMode::SearchMode;
    }
    else if (_state == ChatUIMode::ChatMode) {
        ui->chat_user_list->show();
        ui->con_user_list->hide();
        ui->search_list->hide();
        _mode = ChatUIMode::ChatMode;
        ui->search_list->CloseFindDlg();
        ui->search_edit->clear();
        ui->search_edit->clearFocus();
    }
    else if (_state == ChatUIMode::ContactMode) {
        ui->chat_user_list->hide();
        ui->search_list->hide();
        ui->con_user_list->show();
        _mode = ChatUIMode::ContactMode;
        ui->search_list->CloseFindDlg();
        ui->search_edit->clear();
        ui->search_edit->clearFocus();
    }
    else if (_state == ChatUIMode::SettingsMode) {
        ui->chat_user_list->hide();
        ui->search_list->hide();
        ui->con_user_list->show();
        _mode = ChatUIMode::ContactMode;
        ui->search_list->CloseFindDlg();
        ui->search_edit->clear();
        ui->search_edit->clearFocus();
    }
}

void ChatDialog::slot_loading_chat_user()
{
    if (_b_loading) {
        return;
    }

    _b_loading = true;
    LoadingDlg* loadingDialog = new LoadingDlg(this);
    loadingDialog->setModal(true);
    loadingDialog->show();
    loadMoreChatUser();
    // 加载完成后关闭对话框
    loadingDialog->deleteLater();

    _b_loading = false;
}

void ChatDialog::slot_side_chat()
{
    qDebug() << "receive side chat clicked";
    ClearLabelState(ui->side_chat_lb);
    ui->stackedWidget->setCurrentWidget(ui->chat_page);
    _state = ChatUIMode::ChatMode;
    ShowSearch(false);
    //设置选中条目
    SetSelectChatItem(_cur_chat_thread_id);
    //更新聊天界面信息
    SetSelectChatPage(_cur_chat_thread_id);
}

void ChatDialog::slot_side_contact() {
    qDebug() << "receive side contact clicked";
    ClearLabelState(ui->side_contact_lb);
    //设置
    if (_last_widget == nullptr) {
        ui->stackedWidget->setCurrentWidget(ui->friend_apply_page);
        _last_widget = ui->friend_apply_page;
    }
    else {
        ui->stackedWidget->setCurrentWidget(_last_widget);
    }

    _state = ChatUIMode::ContactMode;
    ShowSearch(false);
}

void ChatDialog::slot_side_setting() {
    qDebug() << "receive side setting clicked";
    ClearLabelState(ui->side_settings_lb);
    //设置
    ui->stackedWidget->setCurrentWidget(ui->user_info_page);

    _state = ChatUIMode::SettingsMode;
    ShowSearch(false);
}

void ChatDialog::slot_text_changed(const QString& str)
{
    //qDebug()<< "receive slot text changed str is " << str;
    if (!str.isEmpty()) {
        ShowSearch(true);
    }
}

void ChatDialog::slot_focus_out()
{
    qDebug() << "receive focus out signal";
    ShowSearch(false);
}

void ChatDialog::slot_loading_contact_user()
{
    qDebug() << "slot loading contact user";
    if (_b_loading) {
        return;
    }

    _b_loading = true;
    LoadingDlg* loadingDialog = new LoadingDlg(this);
    loadingDialog->setModal(true);
    loadingDialog->show();
    qDebug() << "add new data to list.....";
    loadMoreConUser();
    // 加载完成后关闭对话框
    loadingDialog->deleteLater();

    _b_loading = false;
}

void ChatDialog::slot_switch_apply_friend_page()
{
    qDebug() << "receive switch apply friend page sig";
    _last_widget = ui->friend_apply_page;
    ui->stackedWidget->setCurrentWidget(ui->friend_apply_page);
}

void ChatDialog::slot_friend_info_page(std::shared_ptr<UserInfo> user_info)
{
    qDebug() << "receive switch friend info page sig";
    _last_widget = ui->friend_info_page;
    ui->stackedWidget->setCurrentWidget(ui->friend_info_page);
    ui->friend_info_page->SetInfo(user_info);
}


void ChatDialog::slot_show_search(bool show)
{
    ShowSearch(show);
}

void ChatDialog::slot_apply_friend(std::shared_ptr<AddFriendApply> apply)
{
    qDebug() << "receive apply friend slot, applyuid is " << apply->_from_uid << " name is "
        << apply->_name << " desc is " << apply->_desc;

    bool b_already = UserMgr::GetInstance()->AlreadyApply(apply->_from_uid);
    if (b_already) {
        return;
    }

    UserMgr::GetInstance()->AddApplyList(std::make_shared<ApplyInfo>(apply));
    ui->side_contact_lb->ShowRedPoint(true);
    ui->con_user_list->ShowRedPoint(true);
    ui->friend_apply_page->AddNewApply(apply);
}

void ChatDialog::slot_add_auth_friend(std::shared_ptr<AuthInfo> auth_info) {
    // 加载聊天会话

    qDebug() << "receive slot_add_auth__friend uid is " << auth_info->_uid
        << " name is " << auth_info->_name << " nick is " << auth_info->_nick;

    //判断如果已经是好友则跳过
    auto bfriend = UserMgr::GetInstance()->CheckFriendById(auth_info->_uid);
    if (bfriend) {
        return;
    }

    // 添加到 内存里
    UserMgr::GetInstance()->AddFriend(auth_info);

    // 创建 会话
    auto* chat_user_wid = new ChatUserWid();
    auto chat_thread_data = std::make_shared<ChatThreadData>(auth_info->_uid, auth_info->_thread_id, 0);
    UserMgr::GetInstance()->AddChatThreadData(chat_thread_data, auth_info->_uid);
    for (auto& chat_msg : auth_info->_chat_datas) {
        chat_thread_data->AddMsg(chat_msg);
    }

    chat_user_wid->SetChatData(chat_thread_data);
    QListWidgetItem* item = new QListWidgetItem;
    //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    item->setSizeHint(chat_user_wid->sizeHint());
    ui->chat_user_list->insertItem(0, item);
    ui->chat_user_list->setItemWidget(item, chat_user_wid);
    _chat_thread_items.insert(auth_info->_thread_id, item);   
}

void ChatDialog::slot_auth_rsp(std::shared_ptr<AuthRsp> auth_rsp)
{
    qDebug() << "receive slot_auth_rsp uid is " << auth_rsp->_uid
        << " name is " << auth_rsp->_name << " nick is " << auth_rsp->_nick;

    //判断如果已经是好友则跳过
    auto bfriend = UserMgr::GetInstance()->CheckFriendById(auth_rsp->_uid);
    if (bfriend) {
        return;
    }

    UserMgr::GetInstance()->AddFriend(auth_rsp);
    auto* chat_user_wid = new ChatUserWid();
    std::shared_ptr<ChatThreadData> chat_thread_data = std::make_shared<ChatThreadData>(auth_rsp->_uid, auth_rsp->_thread_id, 0);
    UserMgr::GetInstance()->AddChatThreadData(chat_thread_data, auth_rsp->_uid);
    for (auto& chat_msg : auth_rsp->_chat_datas) {
        chat_thread_data->AddMsg(chat_msg);
    }
    chat_user_wid->SetChatData(chat_thread_data);
    QListWidgetItem* item = new QListWidgetItem;
    //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    item->setSizeHint(chat_user_wid->sizeHint());
    ui->chat_user_list->insertItem(0, item);
    ui->chat_user_list->setItemWidget(item, chat_user_wid);
    _chat_thread_items.insert(auth_rsp->_thread_id, item);
}

//todo: 点击搜索的联系人聊天，跳转到聊天界面，之后添加申请创建私聊或者查找已有 私聊消息
void ChatDialog::slot_jump_chat_item(std::shared_ptr<SearchInfo> si)
{
    qDebug() << "slot jump chat item " << endl;
    auto chat_thread_data = UserMgr::GetInstance()->GetChatThreadByUid(si->_uid);
    if (chat_thread_data) {
        auto find_iter = _chat_thread_items.find(chat_thread_data->GetThreadId());
        if (find_iter != _chat_thread_items.end()) {
            qDebug() << "jump to chat item , uid is " << si->_uid;
            ui->chat_user_list->scrollToItem(find_iter.value());
            ui->side_chat_lb->SetSelected(true);
            SetSelectChatItem(chat_thread_data->GetThreadId());
            //更新聊天界面信息
            SetSelectChatPage(chat_thread_data->GetThreadId());
            slot_side_chat();
            return;
        } //说明之前有缓存过聊天列表，只是被删除了，那么重新加进来即可
        else {
            auto* chat_user_wid = new ChatUserWid();
            chat_user_wid->SetChatData(chat_thread_data);
            QListWidgetItem* item = new QListWidgetItem;
            qDebug() << "chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
            ui->chat_user_list->insertItem(0, item);
            ui->chat_user_list->setItemWidget(item, chat_user_wid);
            _chat_thread_items.insert(chat_thread_data->GetThreadId(), item);
            ui->side_chat_lb->SetSelected(true);
            SetSelectChatItem(chat_thread_data->GetThreadId());
            //更新聊天界面信息
            SetSelectChatPage(chat_thread_data->GetThreadId());
            slot_side_chat();
            return;
        }
    }

    //如果没找到，则发送创建请求
    auto uid = UserMgr::GetInstance()->GetUid();
    QJsonObject jsonObj;
    jsonObj["uid"] = uid;
    jsonObj["other_id"] = si->_uid;

    QJsonDocument doc(jsonObj);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);

    //发送tcp请求给chat server
    emit TcpMgr::GetInstance()->sig_send_data(ReqId::ID_CREATE_PRIVATE_CHAT_REQ, jsonData);

}

void ChatDialog::slot_jump_chat_item_from_infopage(std::shared_ptr<UserInfo> user_info)
{
    qDebug() << "slot jump chat item " << endl;
    auto chat_thread_data = UserMgr::GetInstance()->GetChatThreadByUid(user_info->_uid);
    if (chat_thread_data) {
        auto find_iter = _chat_thread_items.find(chat_thread_data->GetThreadId());
        if (find_iter != _chat_thread_items.end()) {
            qDebug() << "jump to chat item , uid is " << user_info->_uid;
            // scrollToItem 将列表项滚动到可视区域
            ui->chat_user_list->scrollToItem(find_iter.value());
            // 选中聊天side
            ui->side_chat_lb->SetSelected(true);
            SetSelectChatItem(chat_thread_data->GetThreadId());
            //更新聊天界面信息
            SetSelectChatPage(chat_thread_data->GetThreadId());
            slot_side_chat();
            return;
        } //说明之前有缓存过聊天列表，只是被删除了，那么重新加进来即可
        else {
            auto* chat_user_wid = new ChatUserWid();
            chat_user_wid->SetChatData(chat_thread_data);
            QListWidgetItem* item = new QListWidgetItem;
            qDebug() << "chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
            ui->chat_user_list->insertItem(0, item);
            ui->chat_user_list->setItemWidget(item, chat_user_wid);
            _chat_thread_items.insert(chat_thread_data->GetThreadId(), item);
            ui->side_chat_lb->SetSelected(true);
            SetSelectChatItem(chat_thread_data->GetThreadId());
            //更新聊天界面信息
            SetSelectChatPage(chat_thread_data->GetThreadId());
            slot_side_chat();
            return;
        }
    }

    //如果没找到，则发送创建请求
    auto uid = UserMgr::GetInstance()->GetUid();
    QJsonObject jsonObj;
    jsonObj["uid"] = uid;
    jsonObj["other_id"] = user_info->_uid;

    QJsonDocument doc(jsonObj);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);

    //发送tcp请求给chat server，创建一个新会话
    emit TcpMgr::GetInstance()->sig_send_data(ReqId::ID_CREATE_PRIVATE_CHAT_REQ, jsonData);
}

