#include "conuseritem.h"
#include "ui_conuseritem.h"

ConUserItem::ConUserItem(QWidget *parent) :
    ListItemBase(parent),
    ui(new Ui::ConUserItem)
{
    ui->setupUi(this);
    SetItemType(ListItemType::CONTACT_USER_ITEM);
    ui->red_point->raise();
    ShowRedPoint(false);
}

ConUserItem::~ConUserItem()
{
    delete ui;
}

QSize ConUserItem::sizeHint() const
{
    return QSize(250, 70); // 返回自定义的尺寸
}

void ConUserItem::SetInfo(const std::shared_ptr<AuthInfo>& auth_info)
{
    _uid = auth_info->_uid;
    _name = auth_info->_name;
    _icon = auth_info->_icon;
    // 加载图片
    QPixmap pixmap(_icon);

    // 设置图片自动缩放
    ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_lb->setScaledContents(true);

    ui->user_name_lb->setText(_name);
}

void ConUserItem::SetInfo(int uid, QString name, QString icon)
{
     _uid = uid;
     _name = name;
     _icon = icon;

     // 加载图片
     QPixmap pixmap(_icon);

     // 设置图片自动缩放
     ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
     ui->icon_lb->setScaledContents(true);

     ui->user_name_lb->setText(_name);
}

void ConUserItem::SetInfo(const std::shared_ptr<AuthRsp>& auth_rsp){
    _uid = auth_rsp->_uid;
    _name = auth_rsp->_name;
    _icon = auth_rsp->_icon;
    // 加载图片
    QPixmap pixmap(_icon);

    // 设置图片自动缩放
    ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_lb->setScaledContents(true);

    ui->user_name_lb->setText(_name);
}

void ConUserItem::ShowRedPoint(bool show)
{
    if(show){
        ui->red_point->show();
    }else{
        ui->red_point->hide();
    }
}
