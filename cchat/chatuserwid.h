#ifndef CHATUSERWID_H
#define CHATUSERWID_H

#include <QWidget>
#include "listitembase.h"

/**
*    @file:    chatuserwid.h
*    @brief:   QListWidget 的每一个 item 的 聊天widget
*    @author:  star-cs
*    @date:    2025-06-17  16:01
*/

namespace Ui {
class ChatUserWid;
}

class ChatUserWid : public ListItemBase
{
    Q_OBJECT

public:
    explicit ChatUserWid(QWidget *parent = 0);
    ~ChatUserWid();

    QSize sizeHint() const override{
        return QSize(250, 70);  // 返回自定义的尺寸
    }
    void SetInfo(QString name, QString head, QString msg);

private:
    Ui::ChatUserWid *ui;
    QString _name;
    QString _head;
    QString _msg;
};

#endif // CHATUSERWID_H
