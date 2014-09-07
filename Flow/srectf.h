#ifndef SRECTF_H
#define SRECTF_H

#include <QRectF>
#include <QObject>

class SRectF : public QRectF
{
    Q_OBJECT
public:
    explicit SRectF(QObject *parent = 0);

signals:

public slots:
public:
    void SetExt(qreal );
};

#endif // SRECTF_H
