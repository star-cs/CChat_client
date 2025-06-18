#include "PictureBubble.h"
#include <QLabel>

#define PIC_MAX_WIDTH 320
#define PIC_MAX_HEIGHT 200

PictureBubble::PictureBubble(const QPixmap &picture, ChatRole role, QWidget *parent)
    :BubbleFrame(role, parent)
{
    QLabel *lb = new QLabel();
    lb->setScaledContents(false); // 关闭自动拉伸，改为手动控制缩放比例 [8](@ref)

    // 关键修改：按原始比例缩放，且不超过最大尺寸
    QPixmap scaledPix = picture.scaled(
        PIC_MAX_WIDTH,
        PIC_MAX_HEIGHT,
        Qt::KeepAspectRatio,   // 保持原始宽高比 [6,8](@ref)
        Qt::SmoothTransformation // 平滑缩放保证质量 [6](@ref)
    );
    lb->setPixmap(scaledPix);
    this->setWidget(lb);

    // 根据缩放后的图片尺寸调整气泡大小
    const QMargins margins = this->layout()->contentsMargins();
    setFixedSize(
        scaledPix.width() + margins.left() + margins.right(),
        scaledPix.height() + margins.top() + margins.bottom()
    );
}
