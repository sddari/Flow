#include "sline.h"

SLine::SLine(SItem *pStart,SItem *pEnd,QGraphicsItem *parent = 0)
{
    m_pStart = pStart;
    m_pEnd = pEnd;
    setFlag(QGraphicsItem::ItemIsSelectable, true);
}

void SLine::updatePosition()
{
    QLineF line(mapFromItem(m_pStart, 0, 0), mapFromItem(m_pEnd, 0, 0));
    setLine(line);
}

void SLine::paint(QPainter *painter, const QStyleOptionGraphicsItem *,QWidget *)
{
    if (m_pStart->collidesWithItem(m_pEnd))
        return;

    QPen myPen = pen();
    myPen.setColor(myColor);
    qreal arrowSize = 4;
    painter->setPen(myPen);
    painter->setBrush(myColor);
//! [4] //! [5]

    QLineF centerLine(m_pStart->pos(), m_pEnd->pos());
    QPolygonF endPolygon = m_pEnd->polygon();
    QPointF p1 = endPolygon.first() + myEndItem->pos();
    QPointF p2;
    QPointF intersectPoint;
    QLineF polyLine;
    for (int i = 1; i < endPolygon.count(); ++i) 
    {
        p2 = endPolygon.at(i) + myEndItem->pos();
        polyLine = QLineF(p1, p2);
        QLineF::IntersectType intersectType = polyLine.intersect(centerLine, &intersectPoint);
        if (intersectType == QLineF::BoundedIntersection)
            break;
        p1 = p2;
    }
    setLine(QLineF(intersectPoint, myStartItem->pos()));
//! [5] //! [6]

    double angle = ::acos(line().dx() / line().length());
    if (line().dy() >= 0)
        angle = (Pi * 2) - angle;

        QPointF arrowP1 = line().p1() + QPointF(sin(angle + Pi / 3) * arrowSize,
                                        cos(angle + Pi / 3) * arrowSize);
        QPointF arrowP2 = line().p1() + QPointF(sin(angle + Pi - Pi / 3) * arrowSize,
                                        cos(angle + Pi - Pi / 3) * arrowSize);

        arrowHead.clear();
        arrowHead << line().p1() << arrowP1 << arrowP2;
//! [6] //! [7]
        painter->drawLine(line());
        painter->drawPolygon(arrowHead);
        if (isSelected()) {
            painter->setPen(QPen(myColor, 1, Qt::DashLine));
        QLineF myLine = line();
        myLine.translate(0, 4.0);
        painter->drawLine(myLine);
        myLine.translate(0,-8.0);
        painter->drawLine(myLine);
    }
}
//! [7]
