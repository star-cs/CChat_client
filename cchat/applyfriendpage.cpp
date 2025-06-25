#include "applyfriendpage.h"
#include "ui_applyfriendpage.h"
#include <QPainter>
#include <QPaintEvent>
#include <QStyleOption>
#include <QRandomGenerator>
#include "applyfrienditem.h"
#include "authenfriend.h"
#include "applyfriend.h"
#include "tcpmgr.h"
#include "usermgr.h"
#include "authenfriend.h"

ApplyFriendPage::ApplyFriendPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ApplyFriendPage)
{
    ui->setupUi(this);
    connect(ui->apply_friend_list, &ApplyFriendList::sig_show_search, this, &ApplyFriendPage::sig_show_search);
    loadApplyList();
    //接受tcp传递的authrsp信号处理，用户收到 我同意对端的好友申请 的回包
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_auth_rsp, this, &ApplyFriendPage::slot_auth_rsp);
}
ApplyFriendPage::~ApplyFriendPage()
{
    delete ui;
}
void ApplyFriendPage::AddNewApply(std::shared_ptr<ApplyInfo> apply)
{
    auto* apply_item = new ApplyFriendItem();
    auto apply_info = apply;

    apply_item->SetInfo(apply_info);

    QListWidgetItem* item = new QListWidgetItem;
    item->setSizeHint(apply_item->sizeHint());
    item->setFlags(item->flags() & ~Qt::ItemIsEnabled & ~Qt::ItemIsSelectable);
    ui->apply_friend_list->insertItem(0, item);
    ui->apply_friend_list->setItemWidget(item, apply_item);

    apply_item->ShowAddBtn(true);
    _unauth_items[apply->_uid] = apply_item;

    //收到审核好友信号，点击 好友申请item，显示确认点击窗口。（可以添加备注，标签 ~ ）
    connect(apply_item, &ApplyFriendItem::sig_auth_friend, [this](std::shared_ptr<ApplyInfo> apply_info){
        auto* authFriend = new AuthenFriend(this);
        authFriend->setModal(true);
        authFriend->SetApplyInfo(apply_info);
        authFriend->show();
    });
}

void ApplyFriendPage::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ApplyFriendPage::loadApplyList()
{
    //添加好友申请
    auto apply_list = UserMgr::GetInstance()->GetApplyList();
    for(auto &apply: apply_list){
        auto* apply_item = new ApplyFriendItem();
        auto head = apply->_icon;
        if(head.isEmpty()){
            head = ":/res/default_icon.png";
        }
        apply->SetIcon(head);
        apply_item->SetInfo(apply); // 每个apply_item指向 UserMgr ApplyInfo
        QListWidgetItem* item = new QListWidgetItem;
        //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
        item->setSizeHint(apply_item->sizeHint());
        item->setFlags(item->flags() & ~Qt::ItemIsEnabled & ~Qt::ItemIsSelectable);
        ui->apply_friend_list->insertItem(0,item);
        ui->apply_friend_list->setItemWidget(item, apply_item);
        if(apply->_status){
            apply_item->ShowAddBtn(false);
        }else{
             apply_item->ShowAddBtn(true);
             auto uid = apply_item->GetUid();
             _unauth_items[uid] = apply_item;
        }
        //收到审核好友信号
        connect(apply_item, &ApplyFriendItem::sig_auth_friend, [this](std::shared_ptr<ApplyInfo> apply_info) {
            auto* authFriend = new AuthenFriend(this);
            authFriend->setModal(true);
            authFriend->SetApplyInfo(apply_info);
            authFriend->show();
            });
    }
}
void ApplyFriendPage::slot_auth_rsp(std::shared_ptr<AuthRsp> auth_rsp)
{
    auto uid = auth_rsp->_uid;
    auto find_iter = _unauth_items.find(uid);
    if (find_iter == _unauth_items.end()) {
        qDebug() << "Error not find apply_uid" << uid;
        return;
    }
    find_iter->second->ShowAddBtn(false);
}
