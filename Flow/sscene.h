#ifndef SSCENE_H
#define SSCENE_H

#include <QGraphicsScene>
#include <QGraphicsLineItem>
#include <QGraphicsSceneMouseEvent>

#include "sitem.h"
#include "Define.h"

class SScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit SScene(QObject *parent = 0);
    void SetSelectItem(E_TYPE _type);
signals:
    void textInserted(QGraphicsTextItem *item);
    void itemSelected(QGraphicsItem *item);

public slots:
    void editorLostFocus(SItem *item);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
private:
    E_TYPE m_eType;
};

#endif // SSCENE_H
