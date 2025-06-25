#ifndef BUBBLEFRAME_H
#define BUBBLEFRAME_H

/**
*    @file:    bubbleframe.h
*    @brief:   聊天气泡框中的 m_pBubble 基类，基础该基类后，实现文本，音频，图像等 气泡聊天信息绘制
*    @author:  star-cs
*    @date:    2025-06-18  15:03
*/

#include "global.h"

#include <QFrame>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QEvent>


class BubbleFrame : public QFrame {
    Q_OBJECT
public:
    BubbleFrame(ChatRole role, MessageStatus status = Sending, QWidget *parent = nullptr);
    void setWidget(QWidget *w);
    void setStatus(MessageStatus status);
    void setMargin(int margin);

protected:
    void paintEvent(QPaintEvent *e) override;
    bool eventFilter(QObject *o, QEvent *e) override;

private:
    void updateStatusIcons();

    ChatRole m_role;
    int m_margin;
    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_contentLayout;
    QHBoxLayout *m_statusLayout;
    QLabel *m_sendStatusIcon;
    QLabel *m_readStatusIcon;
    MessageStatus m_currentStatus = Sending;
    const int WIDTH_SANJIAO = 8;
};

#endif // BUBBLEFRAME_H
