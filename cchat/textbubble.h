#ifndef TEXTBUBBLE_H
#define TEXTBUBBLE_H

/**
*    @file:    textbubble.h
*    @brief:   继承 BubbleFrame，实现 文本气泡的绘制
*    @author:  star-cs
*    @date:    2025-06-18  15:07
*/

#include <QTextEdit>
#include "BubbleFrame.h"
#include <QHBoxLayout>

class TextBubble : public BubbleFrame
{
    Q_OBJECT
public:
    TextBubble(ChatRole role, const QString &text, QWidget *parent = nullptr);
protected:
    bool eventFilter(QObject *o, QEvent *e);
private:
    void adjustTextHeight();
    void setPlainText(const QString &text);
    void initStyleSheet();
private:
    QTextEdit *m_pTextEdit;
};

#endif // TEXTBUBBLE_H
