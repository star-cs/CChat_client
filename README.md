# CChat_client
协同办公软件（聊天，个人网盘，视频会议），QT前端仓库


[协同办公软件 Linux 后端服务器仓库](https://github.com/star-cs/CChat_server)

# 项目介绍



# 业务分析
## 聊天会话数据同步
### 1. 增量加载聊天记录
MianWidnow.cpp 切换聊天界面 初始化ChatDialog  

    ```cpp
    // 请求chatserver，增量加载聊天记录
    _chat_dlg->loadChatList();
    ```
    
    ```cpp
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
    ```
    > 当前用户uid和最近保存的会话id，请求服务器，增量同步会话id

    > 生成 chatUserItem
    ```cpp
    void ChatDialog::slot_load_chat_thread(bool load_more, int last_thread_id,
        std::vector<std::shared_ptr<ChatThreadInfo>> chat_threads)
    {
        for (auto& cti : chat_threads) {
            //先处理单聊，群聊跳过，以后添加
            if (cti->_type == "group") {
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

            auto chat_thread_data = std::make_shared<ChatThreadData>(other_uid, cti->_thread_id, 0);
            UserMgr::GetInstance()->AddChatThreadData(chat_thread_data, other_uid);

            auto* chat_user_wid = new ChatUserWid();
            chat_user_wid->SetChatData(chat_thread_data);
            QListWidgetItem* item = new QListWidgetItem;
            //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
            item->setSizeHint(chat_user_wid->sizeHint());
            ui->chat_user_list->addItem(item);
            ui->chat_user_list->setItemWidget(item, chat_user_wid);
            _chat_thread_items.insert(cti->_thread_id, item);
        }

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
    }
    ```
### 2. 首次单聊


    