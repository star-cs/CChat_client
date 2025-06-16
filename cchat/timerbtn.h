#ifndef TIMERBTN_H
#define TIMERBTN_H

#include <QPushButton>
#include <QTimer>

class TimerBtn : public QPushButton
{
public:
    TimerBtn(QWidget *parent);
    ~TimerBtn();

    // 重写 mouseReleaseEvent
    void mouseReleaseEvent(QMouseEvent *e) override;
private:
    QTimer *_timer;
    int _counter;
};

#endif // TIMERBTN_H
