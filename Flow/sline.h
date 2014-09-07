#ifndef SLINE_H
#define SLINE_H

#include <QGraphicsLineItem>
#include "sitem.h"

class SLine : public QGraphicsLineItem
{
public:    
    SLine(SItem *pStart,SItem *pEnd,QGraphicsItem *paent = 0);
    SItem *getStart() { return m_pStart;}
    SItem *getEnd() { return m_pEnd;}

    void updatePositon();

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

private:
    SItem *m_pStart;
    SItem *m_pEnd;
};

#endif // SLINE_H
