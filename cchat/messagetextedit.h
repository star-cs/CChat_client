#ifndef MESSAGETEXTEDIT_H
#define MESSAGETEXTEDIT_H

#include <QObject>
#include <QTextEdit>
#include <QMouseEvent>
#include <QApplication>
#include <QDrag>
#include <QMimeData>
#include <QMimeType>
#include <QFileInfo>
#include <QFileIconProvider>
#include <QPainter>
#include <QVector>
#include "global.h"

/**
 * @file    messagetextedit.h
 * @brief   聊天消息输入框控件，支持文本、图片和文件的混合输入
 * @author  star-cs
 * @date    2025-06-18  15:19
 *
 * @details 继承自 QTextEdit，扩展以下功能：
 *          1. 支持拖拽添加图片/文件并生成预览
 *          2. 解析图文混排内容为结构化消息列表
 *          3. 回车键发送消息（Shift+Enter换行）
 *          4. 自动缩放图片和文件图标生成
 */
class MessageTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit MessageTextEdit(QWidget *parent = nullptr);
    ~MessageTextEdit();

    /**
     * @brief 获取当前编辑框中的消息列表
     * @return QVector<MsgInfo> - 包含文本/图片/文件的结构化消息
     * @note 调用后清空编辑框内容
     */
    QVector<MsgInfo> getMsgList();

    /**
     * @brief 插入文件或图片的URL列表
     * @param urls - 文件路径列表
     */
    void insertFileFromUrl(const QStringList &urls);

signals:
    /// 触发发送消息信号（回车键触发）
    void send();

protected:
    // 拖拽事件处理
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    // 键盘事件处理（支持回车发送）
    void keyPressEvent(QKeyEvent *e) override;

private:
    /// 插入图片并生成缩略图
    void insertImages(const QString &url);
    /// 插入文件并生成图标预览
    void insertTextFile(const QString &url);
    /// 检查MIME数据是否可插入
    bool canInsertFromMimeData(const QMimeData *source) const override;
    /// 从MIME数据插入内容（拖拽/粘贴）
    void insertFromMimeData(const QMimeData *source) override;

    // 辅助方法
    bool isImage(QString url);                         // 判断文件是否为图片
    void insertMsgList(QVector<MsgInfo> &list,         // 添加消息到列表
                       QString flag, QString text, QPixmap pix);
    QStringList getUrl(QString text);                  // 从文本提取URL
    QPixmap getFileIconPixmap(const QString &url);     // 生成文件预览图
    QString getFileSize(qint64 size);                  // 格式化文件大小

private slots:
    /// 文本变化槽函数（预留）
    void textEditChanged();

private:
    QVector<MsgInfo> mMsgList;     // 存储当前嵌入的非文本消息（图片/文件）
    QVector<MsgInfo> mGetMsgList;   // 最终解析的消息列表
};

#endif // MESSAGETEXTEDIT_H
