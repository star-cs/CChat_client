/**
*    @file:    bubbleframe.h
*    @brief:   聊天气泡框中的 m_pBubble 基类，基础该基类后，实现文本，音频，图像等 气泡聊天信息绘制
*    @author:  star-cs
*    @date:    2025-06-18  15:03
*/

#ifndef BUBBLE_H
#define BUBBLE_H

#include <QFrame>
#include "global.h"
#include <QHBoxLayout>
class BubbleFrame : public QFrame
{
    Q_OBJECT
public:
    BubbleFrame(ChatRole role, QWidget *parent = nullptr);
    void setMargin(int margin);
    //inline int margin(){return margin;}
    void setWidget(QWidget *w);
protected:
    void paintEvent(QPaintEvent *e);
private:
    QHBoxLayout *m_pHLayout;
    ChatRole m_role;
     int      m_margin;
};

#endif // BUBBLE_H
