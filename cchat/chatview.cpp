#include "chatview.h"
#include <QVBoxLayout>
#include <QScrollArea>
#include <QScrollBar>
#include <QEvent>
#include <QStyleOption>
#include <QPainter>
#include <QTImer>

/*
+-----------------------------------+
|         主窗口（QWidget）           |
| 边距0                              |
| +--------------------------------+ |
| | 滚动区域（QScrollArea）          | |
| |                                | |
| |  +----------------------------+ | |
| |  | 内部widget（w）             | | |
| |  | 垂直布局（pHLayout_1）      | | |
| |  |                            | | |
| |  |  [一个占位QWidget（弹簧）]   | | |
| |  |  占据所有空间                | | |
| |  |                            | | |
| |  +----------------------------+ | |
| |                                | |
| |  [垂直滚动条] - 这里试图将滚动条   | |
| |  放在滚动区域内部右侧             |
| +--------------------------------+ |
+-----------------------------------+
*/

ChatView::ChatView(QWidget *parent) : QWidget(parent), isAppended(false)
{
    // 布局
    QVBoxLayout *pMainLayout = new QVBoxLayout();
    this->setLayout(pMainLayout);
    pMainLayout->setMargin(0);

    // 滚动区域（无默认滚动条）
    m_pScrollArea = new QScrollArea();
    m_pScrollArea->setObjectName("chat_area");
    pMainLayout->addWidget(m_pScrollArea);

    // 内部容器
    QWidget *w = new QWidget(this);
    w->setObjectName("chat_bg");
    w->setAutoFillBackground(true);

    // w 内的 垂直布局
    QVBoxLayout *pHLayout_1 = new QVBoxLayout();
    pHLayout_1->addWidget(new QWidget(), 100000);   // new QWidget（） 弹簧，占位QWidget，拉伸因子100000

    w->setLayout(pHLayout_1);
    m_pScrollArea->setWidget(w);  //应该时在QSCrollArea构造后执行 才对

    m_pScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);    // 滚动区域（无默认滚动条）
    QScrollBar *pVScrollBar = m_pScrollArea->verticalScrollBar();   // 滚动区域 的 滚动条 需要 单独设置布局
    connect(pVScrollBar, &QScrollBar::rangeChanged ,this, &ChatView::onVScrollBarMoved);

    //把垂直ScrollBar放到上边 而不是原来的并排
    QHBoxLayout *pHLayout_2 = new QHBoxLayout();
    pHLayout_2->addWidget(pVScrollBar, 0, Qt::AlignRight);  // 脱离原本的布局位置，添加到自定义的布局 pHLayout_2
    pHLayout_2->setMargin(0);
    m_pScrollArea->setLayout(pHLayout_2);
    pVScrollBar->setHidden(true);

    m_pScrollArea->setWidgetResizable(true);
    m_pScrollArea->installEventFilter(this);
    initStyleSheet();
}

void ChatView::appendChatItem(QWidget *item)
{
   QVBoxLayout *vl = qobject_cast<QVBoxLayout *>(m_pScrollArea->widget()->layout());
   vl->insertWidget(vl->count()-1, item);
   isAppended = true;
}

void ChatView::prependChatItem(QWidget *item)
{

}

void ChatView::insertChatItem(QWidget *before, QWidget *item)
{

}

bool ChatView::eventFilter(QObject *o, QEvent *e)
{
    if(e->type() == QEvent::Enter && o == m_pScrollArea)
    {
        m_pScrollArea->verticalScrollBar()->setHidden(m_pScrollArea->verticalScrollBar()->maximum() == 0);
    }
    else if(e->type() == QEvent::Leave && o == m_pScrollArea)
    {
         m_pScrollArea->verticalScrollBar()->setHidden(true);
    }
    return QWidget::eventFilter(o, e);

}

void ChatView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ChatView::onVScrollBarMoved(int min, int max)
{
    if(isAppended) //添加item可能调用多次
    {
        QScrollBar *pVScrollBar = m_pScrollArea->verticalScrollBar();
        pVScrollBar->setSliderPosition(pVScrollBar->maximum());
        //500毫秒内可能调用多次
        QTimer::singleShot(500, [this]()
        {
            isAppended = false;
        });
    }
}

void ChatView::initStyleSheet()
{

}
