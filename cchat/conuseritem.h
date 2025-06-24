#ifndef CONUSERITEM_H
#define CONUSERITEM_H

#include <QWidget>


#include "listitembase.h"
#include "userdata.h"
#include <memory>

/**
*    @file:    conuseritem.cpp
*    @brief:   联系人 item
*    @author:  star-cs
*    @date:    2025-06-19  16:28
*/

namespace Ui {
class ConUserItem;
}


class ConUserItem : public ListItemBase
{
    Q_OBJECT
public:
    explicit ConUserItem(QWidget *parent = nullptr);
    ~ConUserItem();
    QSize sizeHint() const override;
    void SetInfo(std::shared_ptr<AuthInfo> auth_info);
    void SetInfo(std::shared_ptr<AuthRsp> auth_rsp);
    void SetInfo(int uid, QString name, QString icon);
    void ShowRedPoint(bool show = false);
    std::shared_ptr<UserInfo> GetInfo();

private:
    Ui::ConUserItem *ui;
    std::shared_ptr<UserInfo> _info;
};

#endif // CONUSERITEM_H
