#include "sscene.h"
#include <QDebug>
#include <QTextCursor>

SScene::SScene(QObject *parent) :
    QGraphicsScene(parent)
{
    m_eType = E_TYPE_NONE;
}

void SScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (m_eType == E_TYPE_NONE) return;

    if (m_eType == E_TYPE_PROCESS || m_eType == E_TYPE_JUMP)
    {        
        SItem *textItem = new SItem();
        textItem->setTextInteractionFlags(Qt::TextEditorInteraction);

        textItem->setType(m_eType);
        //textItem->setZValue(1000.0);

        connect(textItem, SIGNAL(lostFocus(SItem*)),
                this, SLOT(editorLostFocus(SItem*)));
        connect(textItem, SIGNAL(selectedChange(QGraphicsItem*)),
                this, SIGNAL(itemSelected(QGraphicsItem*)));

        addItem(textItem);
        //textItem->setDefaultTextColor(Qt::black);
        textItem->setPos(mouseEvent->scenePos());
        //emit textInserted(textItem);
        textItem->setFocus();
        m_eType = E_TYPE_NONE;
    }
    /*
    if (m_eType == SItem::Line) 
    {
        m_drawLine = new QGraphicsLineItem(QLineF(mouseEvent->scenePos(),
                                    mouseEvent->scenePos()));
        addItem(m_drawLine);
        return;
    }
    */

    /*
    SItem *Item = new SItem(m_eType);
    Item->setPos(mouseEvent->scenePos());
    addItem(Item);
    */
}

void SScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    /*
    if (m_eType == SItem::None) return;

    if (m_eType == SItem::Line)
    {
        if (m_drawLine == 0) return;
        QLineF newLine(m_drawLine->line().p1(), mouseEvent->scenePos());
        m_drawLine->setLine(newLine);

        removeItem(m_drawItem);
        delete m_drawItem;
        m_drawItem = 0;
        return;
    }
    else
    {
        if (m_drawItem == 0)
        {
            m_drawItem = new SItem(SItem::None);
            addItem(m_drawItem);
        }
        m_drawItem->setPos(mouseEvent->scenePos());
    }
    */
}

void SScene::editorLostFocus(SItem *item)
{
    QTextCursor cursor = item->textCursor();
    cursor.clearSelection();
    item->setTextCursor(cursor);

    if (item->toPlainText().isEmpty()) {
        removeItem(item);
        item->deleteLater();
    }
}


void SScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (m_eType == E_TYPE_LINE)
    {
        /*
        if (m_drawLine == 0) return;
        QList<QGraphicsItem *> startItems = items(m_drawLine->line().p1());
        if (startItems.count() && startItems.first() == m_drawLine)
            startItems.removeFirst();
        QList<QGraphicsItem *> endItems = items(m_drawLine->line().p2());
        if (endItems.count() && endItems.first() == m_drawLine)
            endItems.removeFirst();

        removeItem(m_drawLine);
        delete m_drawLine;
        m_drawLine = 0;
    //! [11] //! [12]

        /*
        if (startItems.count() > 0 && endItems.count() > 0 && startItems.first() != endItems.first())
        {
            SItem *startItem = qgraphicsitem_cast<SItem *>(startItems.first());
            SItem *endItem = qgraphicsitem_cast<SItem *>(endItems.first());
        }
        */
    }
}

void SScene::SetSelectItem(E_TYPE _eType)
{
    m_eType = _eType;
    //m_drawItem->SetType(_eType);
}
