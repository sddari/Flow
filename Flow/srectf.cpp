#include "srectf.h"

SRectF::SRectF(QObject *parent) :
    QRectF(parent)
{
}

void SRectF::SetExt(qreal _Ext)
{
    qreal x , y , w, h;
    getRect(&x , &y , &w, &h);
    x -= _Ext;
    y -= _Ext;
    w += (_Ext * 2.0);
    h += (_Ext * 2.0);
    setRect(x , y , w, h);
}
