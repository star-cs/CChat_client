#include "chatuserlist.h"

ChatUserList::ChatUserList(QWidget *parent) : QListWidget(parent)
{
    // setHorizontalScrollBarPolicy 设置滚动条显示策略
    // setHorizontalScrollBar 替换滚动条实例（自定义的滚动条替换默认滚动条）
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // 安装事件过滤器
    // 类似于HOOK
    // viewport 视口 滚动区域的可视区域。
    // installEventFilter，将当前对象注册为 视口 的事件过滤器，拦截所有的事件
    this->viewport()->installEventFilter(this);
}


// 安装过滤器后，重写 eventFilter()
bool ChatUserList::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == this->viewport()){
        if(event->type() == QEvent::Enter){
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        }else if(event->type() == QEvent::Leave){
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        }
    }

    // 鼠标滚轮事件
    if(watched == this->viewport() && event->type() == QEvent::Wheel){
        QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);
        int numDegrees = wheelEvent->angleDelta().y() / 8;
        int numSteps = numDegrees / 15; // 计算滚动步数

        // 设置滚动幅度
        this->verticalScrollBar()->setValue(this->verticalScrollBar()->value() - numSteps);

         // 检查是否滚动到底部
        QScrollBar *scrollBar = this->verticalScrollBar();
        int maxScrollValue = scrollBar->maximum();
        int currentValue = scrollBar->value();
        //int pageSize = 10; // 每页加载的联系人数量
        if (maxScrollValue - currentValue <= 0) {
            // 滚动到底部，加载新的联系人
            qDebug()<<"load more chat user";
            //加载流程: 发送信号通知聊天界面加载更多聊天内容
            emit sig_loading_chat_user();
         }
        return true; // 停止事件传递
    }

    return QListWidget::eventFilter(watched, event);
}
