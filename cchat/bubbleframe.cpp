#include "bubbleframe.h".h"
#include <QPainter>
#include <QDebug>
#include <QToolTip>
#include <QHelpEvent>

BubbleFrame::BubbleFrame(ChatRole role, MessageStatus status, QWidget *parent)
    : QFrame(parent), m_role(role), m_margin(3), m_currentStatus(status)
{
    // 主布局（垂直）
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(2);

    // 内容布局（水平）
    m_contentLayout = new QHBoxLayout();
    m_contentLayout->setContentsMargins(
        (m_role == ChatRole::Self) ? m_margin : WIDTH_SANJIAO + m_margin,
        m_margin,
        (m_role == ChatRole::Other) ? WIDTH_SANJIAO + m_margin : m_margin,
        m_margin
    );

    // 状态布局（水平）
    m_statusLayout = new QHBoxLayout();
    m_statusLayout->setContentsMargins(5, 0, 5, 3);
    m_statusLayout->setAlignment((m_role == ChatRole::Self) ? Qt::AlignRight : Qt::AlignLeft);

    // 创建状态图标
    m_sendStatusIcon = new QLabel(this);
    m_sendStatusIcon->setFixedSize(6, 6);
    m_sendStatusIcon->setToolTip("发送中...");

    m_readStatusIcon = new QLabel(this);
    m_readStatusIcon->setFixedSize(6, 6);
    m_readStatusIcon->setToolTip("未读");

    // 添加到状态布局
    m_statusLayout->addWidget(m_sendStatusIcon);
    m_statusLayout->addWidget(m_readStatusIcon);

    // 组合布局
    m_mainLayout->addLayout(m_contentLayout);
    m_mainLayout->addLayout(m_statusLayout);

    // 初始状态
    updateStatusIcons();
    this->setLayout(m_mainLayout);
    installEventFilter(this);
}

void BubbleFrame::setWidget(QWidget *w) {
    if (m_contentLayout->count() == 0) {
        m_contentLayout->addWidget(w);
    }
}

void BubbleFrame::setStatus(MessageStatus status) {
    m_currentStatus = status;
    updateStatusIcons();
}

void BubbleFrame::updateStatusIcons() {
     // 更新发送状态
    if (m_role == ChatRole::Self) { // 只有自己的消息才显示状态
        switch (m_currentStatus) {
        case Sending:
            m_sendStatusIcon->setStyleSheet("background-color: gray; border-radius: 3px;");
            m_sendStatusIcon->setToolTip("发送中...");
            break;
        case Sent:
            m_sendStatusIcon->setStyleSheet("background-color: #3498db; border-radius: 3px;");
            m_sendStatusIcon->setToolTip("已发送到服务器");
            break;
        case NoRead:
            m_sendStatusIcon->setStyleSheet("background-color: #2ecc71; border-radius: 3px;");
            m_sendStatusIcon->setToolTip("已送达");
            break;
        }

        // 更新已读状态
        m_readStatusIcon->setStyleSheet(
            (m_currentStatus == Read) ?
            "background-color: #27ae60; border-radius: 3px;" :
            "background-color: lightgray; border-radius: 3px;"
        );
        m_readStatusIcon->setToolTip(
            (m_currentStatus == Read) ?
            "对方已读" : "未读"
        );

        // 确保状态图标可见
        m_sendStatusIcon->show();
        m_readStatusIcon->show();
    } else {
        // 如果是对方消息，隐藏状态图标
        if (m_sendStatusIcon) m_sendStatusIcon->hide();
        if (m_readStatusIcon) m_readStatusIcon->hide();
    }
}

void BubbleFrame::paintEvent(QPaintEvent *e) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);

    // 状态标签区域高度
    const int statusHeight = m_statusLayout->minimumSize().height();

    if (m_role == ChatRole::Other) {
        // 绘制气泡主体（避开状态区域）
        QColor bk_color(Qt::white);
        painter.setBrush(bk_color);
        QRect bk_rect = QRect(WIDTH_SANJIAO, 0,
                             width() - WIDTH_SANJIAO,
                             height() - statusHeight);
        painter.drawRoundedRect(bk_rect, 8, 8);

        // 绘制小三角
        QPointF points[3] = {
            QPointF(bk_rect.x(), 12),
            QPointF(bk_rect.x(), 10 + WIDTH_SANJIAO + 2),
            QPointF(bk_rect.x() - WIDTH_SANJIAO, 10 + WIDTH_SANJIAO - WIDTH_SANJIAO/2),
        };
        painter.drawPolygon(points, 3);
    } else {
        // 绘制气泡主体（避开状态区域）
        QColor bk_color(158, 234, 106);
        painter.setBrush(bk_color);
        QRect bk_rect = QRect(0, 0,
                             width() - WIDTH_SANJIAO,
                             height() - statusHeight);
        painter.drawRoundedRect(bk_rect, 8, 8);

        // 绘制小三角
        QPointF points[3] = {
            QPointF(bk_rect.x() + bk_rect.width(), 12),
            QPointF(bk_rect.x() + bk_rect.width(), 12 + WIDTH_SANJIAO + 2),
            QPointF(bk_rect.x() + bk_rect.width() + WIDTH_SANJIAO, 12 + WIDTH_SANJIAO - WIDTH_SANJIAO/2),
        };
        painter.drawPolygon(points, 3);
    }

    QFrame::paintEvent(e);
}

bool BubbleFrame::eventFilter(QObject *o, QEvent *e) {
    if (e->type() == QEvent::ToolTip) {
        // 转换为帮助事件获取鼠标位置
        QHelpEvent *helpEvent = static_cast<QHelpEvent *>(e);

        // 将全局坐标转换为当前组件坐标
        QPoint localPos = mapFromGlobal(helpEvent->globalPos());

        // 检查发送状态图标
        if (m_sendStatusIcon->isVisible() &&
            m_sendStatusIcon->geometry().contains(localPos)) {
            QToolTip::showText(helpEvent->globalPos(),
                              m_sendStatusIcon->toolTip(),
                              this,
                              m_sendStatusIcon->geometry());
            return true;
        }
        // 检查已读状态图标
        else if (m_readStatusIcon->isVisible() &&
                 m_readStatusIcon->geometry().contains(localPos)) {
            QToolTip::showText(helpEvent->globalPos(),
                              m_readStatusIcon->toolTip(),
                              this,
                              m_readStatusIcon->geometry());
            return true;
        }
    }
    return QFrame::eventFilter(o, e);
}

void BubbleFrame::setMargin(int margin) {
    Q_UNUSED(margin);
}
