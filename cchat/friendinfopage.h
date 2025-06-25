#ifndef FRIENDINFOPAGE_H
#define FRIENDINFOPAGE_H

#include <QWidget>
#include <QString>
#include "global.h"
#include "userdata.h"

namespace Ui {
class FriendInfoPage;
}

class FriendInfoPage : public QWidget
{
    Q_OBJECT

public:
    explicit FriendInfoPage(QWidget *parent = nullptr);
    ~FriendInfoPage();
    void SetInfo(std::shared_ptr<FriendInfo> friendinfo);
private slots:
    void on_msg_chat_clicked();

private:
    Ui::FriendInfoPage *ui;
    int _uid;
    QString _name;
    QString _icon;
    QString _nick;
signals:
    void sig_jump_chat_item(int uid);
};

#endif // FRIENDINFOPAGE_H
