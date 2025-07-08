/**
*    @file:    picturebubble.h
*    @brief:   TextBubble类似，继承BubbleFrame，实现图片的消息框
*    @author:  star-cs
*    @date:    2025-06-18  15:13
*/

#ifndef PICTUREBUBBLE_H
#define PICTUREBUBBLE_H

#include "BubbleFrame.h"
#include <QHBoxLayout>
#include <QPixmap>
class PictureBubble : public BubbleFrame
{
    Q_OBJECT
public:
    PictureBubble(const QPixmap &picture, ChatRole role, QWidget *parent = nullptr);
};

#endif // PICTUREBUBBLE_H
