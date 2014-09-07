#include "sitem.h"

SItem::SItem(QGraphicsItem *parent)
    : QGraphicsTextItem(parent)
{
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
}
//! [0]

//! [1]
QVariant SItem::itemChange(GraphicsItemChange change,
                     const QVariant &value)
{
    if (change == QGraphicsItem::ItemSelectedHasChanged)
        emit selectedChange(this);
    return value;
}
//! [1]

//! [2]
void SItem::focusOutEvent(QFocusEvent *event)
{
    setTextInteractionFlags(Qt::NoTextInteraction);
    emit lostFocus(this);
    QGraphicsTextItem::focusOutEvent(event);
}
//! [2]

//! [5]
void SItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if (textInteractionFlags() == Qt::NoTextInteraction)
        setTextInteractionFlags(Qt::TextEditorInteraction);
    QGraphicsTextItem::mouseDoubleClickEvent(event);
}

void SItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *o, QWidget *w)
{
    switch((int)m_eType)
    {
    case E_TYPE_PROCESS:
        painter->drawRect(boundingRect());
        break;
    case E_TYPE_JUMP:
        painter->drawEllipse(boundingRect());
        break;
    }
    QGraphicsTextItem::paint(painter, o, w);
}

void SItem::setType(E_TYPE _eType)
{
    m_eType = _eType;
}
