#include <QPainter>
#include <QDebug>
#include <Qfileinfo>
#include <QDataStream>
#include <QWaitCondition>
#include <QApplication>
#include <math.h>
#include <QMessageBox>

#define D_PI 3.141592

#include "sview.h"
#include "mainwindow.h"

extern MainWindow *g_MainWindow;

SView::SView(QWidget *parent) :
    QWidget(parent)
{
    m_nItem = E_TYPE_NONE;
    m_nSelect = D_SELECT_NONE;
    m_nMouseState = E_MOUSE_STATE_NONE;
    m_nEditType = E_TYPE_NONE;
    m_Edit = new QLineEdit(this);
    m_Edit->hide();
    connect(m_Edit, SIGNAL(editingFinished()), this, SLOT(SlotEditingFinished()));
    connect(m_Edit, SIGNAL(textEdited(QString)), this, SLOT(SlotEditing(QString)));
    setMouseTracking(true);
    m_nMode = E_MODE_EDIT;
    InitRectSizeExt(0.5f);
}

SView::~SView()
{
}

void SView::InitRectSizeExt(float _fRatio)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing,true);
    QFontMetrics fMetrics = painter.fontMetrics();

    m_sizeFont = fMetrics.size( Qt::TextSingleLine,"W");
    m_fRectExt = m_sizeFont.height() * _fRatio;
    m_data.InitData(m_fRectExt,m_sizeFont,this);
    painter.end();
}

void SView::SelectItem(int _nItem)
{
    m_nItem = _nItem;
}

void SView::paintEvent(QPaintEvent *)
{    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing,true);

    if (m_nSelect == D_SELECT_NONE)
    {
        if (m_pointMove.x() > 0 && m_pointMove.y() > 0)
        {
            QRectF drawRect(m_pointMove.x()-m_fRectExt,
                            m_pointMove.y()-m_fRectExt,
                            m_sizeFont.width()+m_fRectExt*2,
                            m_sizeFont.height()+m_fRectExt*2);

            switch(m_nItem)
            {
            case E_TYPE_JUMP:
                painter.drawEllipse(drawRect);
                break;
            case E_TYPE_PROCESS:
                painter.drawRect(drawRect);
                break;            
            case E_TYPE_PROCESS_THREAD:
                painter.drawRect(drawRect);
                drawRect.translate(-drawRect.height()/4.0,-drawRect.height()/4.0);
                drawRect.setSize(drawRect.size()  / 2);
                painter.drawArc(drawRect,0,270*16);
                break;
            }
        }
    }

    if (m_nItem == E_TYPE_LINE)
    {
        if (m_nMouseState == E_MOUSE_STATE_DOWN)
            painter.drawLine(m_pointLineS,m_pointLineE);
    }

    foreach(SItem it, m_data.GetItems() )
    {        
        it.m_rectDraw.translate(m_pointBack);
        it.m_rectText.translate(m_pointBack);

        switch(it.m_nColor)
        {
        case E_COLOR_NONE:
            painter.setBrush( Qt::NoBrush );
            painter.setPen( Qt::black );
            break;
        case E_COLOR_ERROR:
            painter.setBrush( Qt::red );
            painter.setPen( Qt::white );
            break;
        case E_COLOR_FINISH:
            painter.setBrush( Qt::darkGreen );
            painter.setPen( Qt::white );
            break;
        case E_COLOR_RUN:
            painter.setBrush( Qt::darkBlue );
            painter.setPen( Qt::white );
            break;
        }
        switch(it.m_nType)
        {
        case E_TYPE_JUMP:                        
            painter.drawEllipse(it.m_rectDraw);            
            break;
        case E_TYPE_PROCESS:
            painter.drawRect(it.m_rectDraw);            
            break;
        case E_TYPE_PROCESS_THREAD:
            painter.drawRect(it.m_rectDraw);
            QRectF drawRect = it.m_rectDraw;
            drawRect.setSize(QSize(drawRect.height(),drawRect.height()));
            drawRect.translate(-drawRect.height()/4.0,-drawRect.height()/4.0);            
            drawRect.setSize(drawRect.size() / 2);

            if (it.m_nColor == E_COLOR_RUN)
                painter.setPen( Qt::black );
            painter.drawArc(drawRect,0,270*16);
            if (it.m_nColor == E_COLOR_RUN)
                painter.setPen( Qt::white );
        }
        painter.drawText(it.m_rectText, Qt::AlignCenter|Qt::TextSingleLine,it.m_str );
    }

    painter.setBrush( Qt::NoBrush );
    painter.setPen( Qt::black );

    QFontMetrics fMetrics = painter.fontMetrics();
    int i = 0;
    foreach(SLine it, m_data.GetLines() )
    {
        QPointF pointStart = m_data.GetItemRect(it.m_nStart).center() + m_pointBack;
        QPointF pointEnd = m_data.GetItemRect(it.m_nEnd).center() + m_pointBack;
        {
            QLineF lineCenter(pointStart,pointEnd);            

            pointStart = GetPointCross(it.m_nStart,lineCenter);
            pointEnd = GetPointCross(it.m_nEnd,lineCenter);
            
            QLineF line (pointEnd,pointStart);
            painter.drawLine(line);
            painter.drawPolygon(GetPolygonArrow(line,6),Qt::WindingFill);
        }

        QSize szSize = fMetrics.size( Qt::TextSingleLine,m_data.GetLineStr(i));
        QPointF point (szSize.width()/2,szSize.height()/2);        
        
        {              
            QRectF lineCenter(m_data.GetItemRect(it.m_nStart).center(),m_data.GetItemRect(it.m_nEnd).center());
            QPointF center = lineCenter.center();
            center -= point;
            m_data.SetLineRectText(i,center,szSize);
        }
        
        QRectF rectText = m_data.GetLineStrRect(i);
        rectText.translate(m_pointBack);
        painter.drawText(rectText, Qt::AlignCenter|Qt::TextSingleLine,m_data.GetLineStr(i));
        i++;
    }

    painter.end();
}

QPolygonF SView::GetPolygonArrow(QLineF line,qreal arrowSize)
{
    qreal angle = ::acos(line.dx() / line.length());
    if (line.dy() >= 0) angle = (D_PI * 2) - angle;
    QPointF arrowP1 = line.p1() + QPointF(sin(angle + D_PI / 3) * arrowSize, cos(angle + D_PI / 3) * arrowSize);
    QPointF arrowP2 = line.p1() + QPointF(sin(angle + D_PI - D_PI / 3) * arrowSize,  cos(angle + D_PI - D_PI / 3) * arrowSize);
    QPolygonF arrowHead;
    arrowHead << line.p1() << arrowP1 << arrowP2;
    return arrowHead;
}

QPointF SView::GetPointCross(int nIndex,QLineF line)
{
    SItem stItem = m_data.GetItem(nIndex);
    QRectF rect = stItem.m_rectDraw;
    rect.translate(m_pointBack);
    QPainterPath path;

    if (stItem.m_nType == E_TYPE_JUMP)
        path.addEllipse(rect);
    else if (stItem.m_nType == E_TYPE_PROCESS || stItem.m_nType == E_TYPE_PROCESS_THREAD)
        path.addRect(rect);

    QPointF intersectPoint;
    QPolygonF Polygon = path.toFillPolygon();

    for (int i = 1; i < Polygon.count(); ++i)
    {
        QLineF polyLine = QLineF(Polygon.at(i-1), Polygon.at(i));
        if (polyLine.intersect(line, &intersectPoint) == QLineF::BoundedIntersection)
            break;
    }

    return intersectPoint;
}


void SView::SlotEditingFinished()
{
    if (m_nSelect == D_SELECT_NONE) return;

    if (m_Edit->text().isEmpty() == true)
    {
        if (m_nEditType != E_TYPE_LINE)
        {
            m_data.DeleteItem(m_nSelect);
        }
    }
    m_nSelect = D_SELECT_NONE;
    m_Edit->hide();
    repaint();
    m_Edit->setText("");
    return;
}

void SView::SlotEditing(QString text)
{
    if (m_nSelect == D_SELECT_NONE) return;
    text = text.trimmed();    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing,true);
    QFontMetrics fMetrics = painter.fontMetrics();
    QSize sz = fMetrics.size( Qt::TextSingleLine,text);

    if (m_nEditType == E_TYPE_LINE)
    {
        QPointF center = m_data.GetLineStrRect(m_nSelect).center();
        QPointF point (sz.width()/2,sz.height()/2);
        center -= point;
        m_data.SetLineRectText(m_nSelect,center,sz,text);
        SetEditRect(m_data.GetLineStrRect(m_nSelect));
    }
    else
    {
        QSize szExt(sz.width()+m_fRectExt*2.0f,sz.height()+m_fRectExt*2.0f);
        m_data.SetItem(m_nSelect,text,sz,szExt);
        SetEditRect(m_data.GetItemRectStr(m_nSelect));
    }
    painter.end();
    repaint();
}

void SView::SetEditRect(QRectF rect)
{    
    float fExt = m_sizeFont.height() * 0.2;
    rect.translate(m_pointBack);
    m_Edit->setGeometry(rect.left() - fExt,
                        rect.top() - fExt,
                        rect.width()+m_sizeFont.width()+ (fExt*2.0),
                        rect.height() + (fExt*2.0) );
}

void SView::mousePressEvent ( QMouseEvent * e )
{
    // store click position
    if (m_nMode == E_MODE_RUN) return;
    switch (m_nItem)
    {
    case E_TYPE_DEL:
        {
            m_Edit->hide();
            if (m_data.DeleteItem(e->pos() - m_pointBack)){repaint();return;}
            if (m_data.DeleteLine(e->pos() , m_pointBack)){repaint();return;}
        }
        break;
    case E_TYPE_NONE:
        {            
            m_Edit->hide();
            m_nSelect = m_data.FindItem(e->pos() - m_pointBack);

            if (m_nSelect != D_SELECT_NONE)
            {
                SItem item = m_data.GetItem(m_nSelect);
                m_nEditType = item.m_nType;
                m_Edit->setText(item.m_str);
                SetEditRect(item.m_rectText);
                m_Edit->show();
                m_Edit->setFocus();
                m_pointMove = e->pos();
                m_nMouseState = E_MOUSE_STATE_DOWN;
                return;
            }

            m_nSelect = m_data.FindLine(e->pos() , m_pointBack);

            if (m_nSelect != D_SELECT_NONE)
            {
                m_nEditType = E_TYPE_LINE;
                SLine line = m_data.GetLine(m_nSelect);
                m_Edit->setText(line.m_str);
                SetEditRect(line.m_rectText);
                m_Edit->show();
                m_Edit->setFocus();
                m_pointMove = e->pos();
                m_nMouseState = E_MOUSE_STATE_DOWN;
            }

            m_nItem = E_TYPE_MOVE;
            m_nMouseState = E_MOUSE_STATE_DOWN;
            m_pointMove = e->pos();
        }
        return;
    case E_TYPE_JUMP:
    case E_TYPE_PROCESS:
    case E_TYPE_PROCESS_THREAD:
    {
        if(m_nSelect != D_SELECT_NONE)
            m_data.ChkItemText(m_nSelect);
        m_nSelect = m_data.GetItems().size();
        m_Edit->setText("");
        m_nEditType = m_nItem;

        m_data.InsertItem(m_nItem,m_pointMove-m_pointBack,m_sizeFont);

        SetEditRect(m_data.GetItemRectStr(m_nSelect));
        m_Edit->show();
        m_Edit->setFocus();
    }break;
    case E_TYPE_LINE:
        if ((m_nMouseState == E_MOUSE_STATE_NONE) && ( e->button() == Qt::LeftButton))
        {
            m_pointLineS = e->pos();
            m_pointLineE = e->pos();
            m_nMouseState = E_MOUSE_STATE_DOWN;
        }
        break;
    }
    repaint();
}

void SView::mouseMoveEvent(QMouseEvent* e)
{
    if (m_nMode == E_MODE_RUN) return;
    switch (m_nItem)
    {
    case E_TYPE_NONE:
        if (m_nMouseState == E_MOUSE_STATE_DOWN)
        {
            if (m_nSelect == D_SELECT_NONE) break;            
            m_data.ItemTrans(m_nSelect,e->pos() - m_pointMove);
            SetEditRect(m_data.GetItemRectStr(m_nSelect));
            m_pointMove = e->pos();            
        }
        break;
    case E_TYPE_JUMP:        
    case E_TYPE_PROCESS:
    case E_TYPE_PROCESS_THREAD:
        m_pointMove = e->pos();
        break;
    case E_TYPE_LINE:
        if (m_nMouseState == E_MOUSE_STATE_DOWN)
            m_pointLineE = e->pos();
        break;
    case E_TYPE_MOVE:
        if (m_nMouseState == E_MOUSE_STATE_DOWN)
        {
            QPoint pos = e->pos() - m_pointMove;
            m_pointBack += pos;
            m_pointBackup = m_pointBack;
            m_pointMove = e->pos();            
        }
        break;
    }    
    repaint();
}

void SView::mouseReleaseEvent ( QMouseEvent * e)
{
    if (m_nMode == E_MODE_RUN) return;

    if (( m_nMouseState == E_MOUSE_STATE_DOWN) && ( e->button() == Qt::LeftButton))
    {
        m_nMouseState = E_MOUSE_STATE_NONE;
    }

    if (m_nItem == E_TYPE_LINE)
    {
        if (m_data.InsertLine(m_pointLineS-m_pointBack,m_pointLineE-m_pointBack))
        {
            m_nSelect = m_data.GetLines().size() - 1;
            m_nEditType = m_nItem;			
        }
        else
            return ;
		m_Edit->setText("");
        SetEditRect(m_data.GetLineStrRect(m_nSelect));
       	m_Edit->show();
       	m_Edit->setFocus();
    }

    if (m_nItem == E_TYPE_MOVE)
    {
        m_nItem = E_TYPE_NONE;
    }
}


bool SView::Start()
{    
    m_nMode = E_MODE_RUN;
    if (m_data.Start() == false)
    {
        g_MainWindow->InsertMsg(m_data.GetMsg());        
        return false;
    }    
    SetLoop(true);
    return true;
}

void SView::Stop()
{    
    SetLoop(false);
    m_nMode = E_MODE_EDIT;
    m_data.Stop();
    m_pointBack = m_pointBackup;
}

void SView::NextSchedule()
{
    m_data.NextSchedule();
    QPoint center = m_data.GetSelectNow().center().toPoint();
    QPoint viewCenter(size().width()/2,size().height()/2);
    m_pointBack = viewCenter - center;
}

QString SView::GetFileName()
{
    if (m_strFilePath.isEmpty()) return "";
    QFileInfo info = QFileInfo(m_strFilePath);    
    return info.baseName();
}

bool SView::SaveFile()
{
    return m_data.SaveFile(m_strFilePath);
}

bool SView::LoadFile()
{
    return m_data.LoadFile(m_strFilePath);
    repaint();
}

void SView::InsertSchedule(int _nSchedule)
{
    m_data.InsertSchedule(_nSchedule);
}

void SView::ItemFinish(int _nFinish)
{
    if (m_nMode != E_MODE_EDIT)
        m_data.SetItemColor(_nFinish,E_COLOR_FINISH);
}
