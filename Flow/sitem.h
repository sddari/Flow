#ifndef SITEM_H
#define SITEM_H

#include <QGraphicsTextItem>
#include <QPainter>
#include "Define.h"

class SItem : public QGraphicsTextItem
{
    Q_OBJECT

public:    
    enum { Type = UserType + 3 };
    SItem(QGraphicsItem *parent = 0);
    int type() const { return Type; }

    void paint( QPainter *painter, const QStyleOptionGraphicsItem *o, QWidget *w);
    void setType(E_TYPE _eType);

signals:
    void lostFocus(SItem *item);
    void selectedChange(QGraphicsItem *item);

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    void focusOutEvent(QFocusEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

private:
    E_TYPE m_eType;
};

#endif // SITEM_H
