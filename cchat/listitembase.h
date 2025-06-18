#ifndef LISTITEMBASE_H
#define LISTITEMBASE_H
#include <QWidget>
#include "global.h"


/**
*    @file:    listitembase.h
*    @brief:   自定义的 QWidget 的内容基类，可以继承，自定义QListWidgetItem的几种类型
*    @author:  star-cs
*    @date:    2025-06-17  17:33
*/
class ListItemBase : public QWidget
{
    Q_OBJECT

public:
    explicit ListItemBase(QWidget *parent = nullptr);
    void SetItemType(ListItemType itemType);
    ListItemType GetItemType();
protected:
    // 自定义单独的 QWidget，而不是 QDialog里的，必须重写paintEvent
    void paintEvent(QPaintEvent* event);

private:
    ListItemType _itemType;
public slots:

signals:

};

#endif // LISTITEMBASE_H
