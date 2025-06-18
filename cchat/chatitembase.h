#ifndef CHATITEMBASE_H
#define CHATITEMBASE_H

/**
*    @file:    chatitembase.h
*    @brief:   聊天气泡框，主干 QWidget 。其中的气泡，通过 m_pBubble 单独设计
*    @author:  star-cs
*    @date:    2025-06-18  15:01
*/

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include "global.h"
class BubbleFrame;

class ChatItemBase : public QWidget
{
    Q_OBJECT
public:
    explicit ChatItemBase(ChatRole role, QWidget *parent = nullptr);
    void setUserName(const QString &name);
    void setUserIcon(const QPixmap &icon);
    void setWidget(QWidget *w);

private:
    ChatRole m_role;
    QLabel *m_pNameLabel;
    QLabel *m_pIconLabel;
    QWidget *m_pBubble;
};

#endif // CHATITEMBASE_H
