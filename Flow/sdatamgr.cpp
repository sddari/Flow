#include "sdatamgr.h"
#include <QFile>
#include <QDataStream>
#include <math.h>
#include <QStringList>

#include "mainwindow.h"
#include "sview.h"

extern MainWindow *g_MainWindow;

QDataStream &operator<<( QDataStream &stream, const SItem &data )
{
  stream << data.m_str << data.m_rectDraw << data.m_rectText << data.m_nType;
  return stream;
}

QDataStream &operator>>( QDataStream &stream, SItem &data )
{
  stream >> data.m_str >> data.m_rectDraw >> data.m_rectText >> data.m_nType;
  return stream;
}

QDataStream &operator<<( QDataStream &stream, const SLine &data )
{
  stream << data.m_str << data.m_rectText << data.m_nStart << data.m_nEnd ;
  return stream;
}

QDataStream &operator>>( QDataStream &stream, SLine &data )
{
  stream >> data.m_str >> data.m_rectText >> data.m_nStart >> data.m_nEnd ;
  return stream;
}


SDataMgr::SDataMgr()
{
}

bool SDataMgr::DeleteItem(QPoint _pt)
{
    for(int i = m_vecItems.size() - 1 ; i >= 0; i-- )
    {
        if (m_vecItems[i].m_rectDraw.contains(_pt))
        {
            DeleteItem(i);
            return true;
        }
    }
    return false;
}

void SDataMgr::DeleteItem(int _nSelect)
{
    m_vecItems.remove(_nSelect);
    bool bQuit = true;
    while(bQuit)
    {
        bQuit = false;
        for (int i = 0; i < m_vecLines.size(); i++)
        {
            if (m_vecLines[i].m_nStart == _nSelect || m_vecLines[i].m_nEnd == _nSelect)
            {
                m_vecLines.remove(i);
                bQuit = true;
                break;
            }
        }
    }

    for (int i = 0; i < m_vecLines.size(); i++)
    {
        if (m_vecLines[i].m_nStart >  _nSelect)
           m_vecLines[i].m_nStart--;
        if (m_vecLines[i].m_nEnd > _nSelect)
           m_vecLines[i].m_nEnd--;
    }
}

int SDataMgr::FindItem(QPoint _pt)
{
    for(int i = m_vecItems.size() - 1 ; i >= 0; i-- )
    {
        if (m_vecItems[i].m_rectDraw.contains(_pt))
        {
            return i;
        }
    }
    return D_SELECT_NONE;
}

int SDataMgr::FindLine(QPoint _pt,QPoint _back)
{
    for(int i = m_vecLines.size() - 1 ; i >= 0; i-- )
    {
        QLineF line(m_vecItems[m_vecLines[i].m_nStart].m_rectDraw.center()+_back,
                m_vecItems[m_vecLines[i].m_nEnd].m_rectDraw.center()+_back);
        if (m_vecLines[i].m_rectText.contains(_pt) || IsSelectLine(_pt,line))
        {
            return i;
        }
    }
    return D_SELECT_NONE;
}

bool SDataMgr::DeleteLine(QPoint _pt,QPoint _back)
{
    int nSelect = FindLine(_pt,_back);
    if (nSelect == D_SELECT_NONE) return false;
    m_vecLines.remove(nSelect);
    return true;
}

bool SDataMgr::SaveFile(QString _strFilePath)
{
    QFile file(_strFilePath);
    if (!file.open(QIODevice::WriteOnly)) return false;
    QDataStream stream(&file);
    stream.setVersion( QDataStream::Qt_5_3 );
    stream << m_vecItems;
    stream << m_vecLines;
    file.close();
    return true;
}

bool SDataMgr::LoadFile(QString _strFilePath)
{
    QFile file(_strFilePath);
    if (!file.open(QIODevice::ReadOnly)) return false;
    QDataStream stream(&file);
    stream.setVersion( QDataStream::Qt_5_3 );
    stream >> m_vecItems;
    stream >> m_vecLines;
    file.close();    
    return true;
}

void SDataMgr::ItemMoveLineText(int _nItem)
{
    for (int i = 0; i < m_vecLines.size(); i++)
    {
        if (m_vecLines[i].m_nStart == _nItem || m_vecLines[i].m_nEnd == _nItem)
        {
            SetLineTextRect(&m_vecLines[i]);
        }
    }
}

bool SDataMgr::InsertLine(QPoint _ptStart,QPoint _ptEnd)
{
    int nStart,nEnd;
    nStart = nEnd = D_SELECT_NONE;
    for(int i = 0 ; i < m_vecItems.size(); i++ )
    {
        if (m_vecItems[i].m_rectDraw.contains(_ptStart)) nStart = i;
        if (m_vecItems[i].m_rectDraw.contains(_ptEnd)) nEnd = i;
    }

    if ((nStart == nEnd) || (nStart == D_SELECT_NONE) || (nEnd == D_SELECT_NONE))
        return false;

    SLine line;
    line.m_nStart = nStart;
    line.m_nEnd = nEnd;

    SetLineTextRect(&line);

    m_vecLines.push_back(line);
    return true;
}

void SDataMgr::InsertItem(int _nType,QPoint _pt,QSize _sz)
{
    SItem item;
    item.m_nType = _nType;
    item.m_rectText.setTopLeft(_pt);
    item.m_rectText.setSize(_sz);
    item.m_rectDraw = item.m_rectText;
    SetRectExt(&item.m_rectDraw);
    m_vecItems.push_back(item);
}

void SDataMgr::ChkItemText(int _nSelect)
{
    if (m_vecItems[_nSelect].m_str.isEmpty())
        m_vecItems.remove(_nSelect);
}

void SDataMgr::SetLineTextRect(SLine *_pLine)
{
    QRectF lineCenter(m_vecItems[_pLine->m_nStart].m_rectDraw.center(),m_vecItems[_pLine->m_nEnd].m_rectDraw.center());
    QPointF center = lineCenter.center();

    if (_pLine->m_str.isEmpty())
    {
        QPointF point (m_sizeFont.width()/2,m_sizeFont.height()/2);
        center -= point;
        _pLine->m_rectText.setTopLeft(center);
        _pLine->m_rectText.setSize(m_sizeFont);
    }
}

qreal SDataMgr::GetDistance( QPointF _p1, QPointF _p2 )
{
    qreal vx = _p2.x() - _p1.x();
    qreal vy = _p2.y() - _p1.y();
    return sqrt( pow(vx,2.0) + pow(vy,2.0));
}

bool SDataMgr::IsSelectLine(QPointF _point,QLineF _line)
{
    float u = ( ( ( _point.x() - _line.p1().x() ) * ( _line.p2().x() - _line.p1().x() ) ) +
          ( ( _point.y() - _line.p1().y() ) * ( _line.p2().y() - _line.p1().y() ) ) )  / ( pow(GetDistance( _line.p1() , _line.p2() ),2.0) );

    if ( u < 0.0f || u > 1.0f ) return false;

    QPointF intersect( _line.p1().x() + u * ( _line.p2().x() - _line.p1().x() ),_line.p1().y() + u * ( _line.p2().y() - _line.p1().y() ));
    if (GetDistance( _point , intersect ) < 5.0 ) return true;
    return false;
}

void SDataMgr::SetRectExt(QRectF *rect)
{
    qreal x,y,w,h;
    rect->getRect(&x,&y,&w,&h);
    rect->setRect(x-m_fRectExt,y-m_fRectExt,w+m_fRectExt*2.0,h+m_fRectExt*2.0);
}

void SDataMgr::ItemTrans(int _nSelect,QPoint _pt)
{
    m_vecItems[_nSelect].m_rectText.translate(_pt);
    m_vecItems[_nSelect].m_rectDraw.translate(_pt);
    ItemMoveLineText(_nSelect);
}

typedef struct _SDist
{
    QString m_str;
    QVector<int> m_vecIndex;
}SDist;

int SDataMgr::MakeJump()
{
    m_vecJump.clear();
    QVector <SDist> vecData;
    for(int i = 0 ; i < m_vecItems.size(); i++ )
    {
        if (m_vecItems[i].m_nType == E_TYPE_JUMP)
        {
            int nSelect = D_SELECT_NONE;
            for(int j = 0 ; j < vecData.size(); j++ )
            {
                if (vecData[j].m_str.toUpper() == m_vecItems[i].m_str.toUpper())
                {
                    vecData[j].m_vecIndex.push_back(i);
                    nSelect = i;
                    break;
                }
            }
            if (nSelect == D_SELECT_NONE)
            {
                SDist stDist;
                stDist.m_str = m_vecItems[i].m_str;
                stDist.m_vecIndex.push_back(i);
                vecData.push_back(stDist);
            }
        }
    }

    int nStart = D_SELECT_NONE;
    foreach(SDist it, vecData )
    {
        if (nStart == D_SELECT_NONE)
        {
            if (it.m_str.toUpper() == "시작")
            {
                if(it.m_vecIndex.size() == 1)
                {
                    nStart = it.m_vecIndex[0];
                    continue;
                }
                else
                {
                    m_strMsg = "오류 : 시작 지점은 하나입니다.\n";
                    return -2;
                }
            }
        }

        if (it.m_str.toUpper() == "종료") continue;

        if (it.m_vecIndex.size() == 2)
        {
            SJump stJump;
            stJump.m_nRecv = it.m_vecIndex[0];
            stJump.m_nSend = it.m_vecIndex[1];
            m_vecJump.push_back(stJump);
            stJump.m_nRecv = it.m_vecIndex[1];
            stJump.m_nSend = it.m_vecIndex[0];
            m_vecJump.push_back(stJump);
        }
        else
        {
            m_strMsg.clear();
            m_strMsg.append("오류 : 점프는 두개의 지점만 연결합니다 ");
            m_strMsg.append(m_vecItems[it.m_vecIndex[0]].m_str);
            m_strMsg.append(" 은");
            m_strMsg.append(QString::number(it.m_vecIndex.size()));
            m_strMsg.append(" 개 있습니다.");
            return -2;
        }
    }
    return nStart;
}

void SDataMgr::MakeLoop()
{
    int nCount = 0;
    for(int i = 0 ; i < m_vecItems.size(); i++ )
    {
        if (m_vecItems[i].m_nType == E_TYPE_PROCESS_THREAD)
        {
            QStringList list = m_vecItems[i].m_str.trimmed().split(":");
            if (list.size() > 1) nCount++;
        }
    }
    g_MainWindow->SetThread(nCount+2);

    for(int i = 0 ; i < m_vecItems.size(); i++ )
    {
        if (m_vecItems[i].m_nType == E_TYPE_PROCESS_THREAD)
        {
            QStringList list = m_vecItems[i].m_str.trimmed().split(":");
            if (list.size() > 1)
            {
                QStringList strValueList;
                g_MainWindow->Run(i,list.at(0),strValueList);
                m_listFinish.push_back(list.at(1));                         
            }            
        }
    }        
}

bool SDataMgr::Start()
{    
    MakeLoop();
    int nStart = MakeJump();    
    if (nStart < 0)
    {
        if(nStart == D_SELECT_NONE)
            m_strMsg = "오류 : 시작 지점을 찾을 수 없습니다.";
        Stop();
        return false;
    }
    else
        FindLineInsertSchedule(nStart);
    return true;
}

void SDataMgr::Stop()
{
    m_Schedule.clear();
    QStringList strValueList;

    while(g_MainWindow->GetRunningProcess() != m_listFinish.size());

    foreach(QString str,m_listFinish)
        g_MainWindow->Run(D_SELECT_NONE,str,strValueList,false);

    m_listFinish.clear();
    for(int i = 0; i < m_vecItems.size();i++)
        m_vecItems[i].m_nColor = E_COLOR_NONE;
}

void SDataMgr::FindLineInsertSchedule(int _nFind)
{
    m_vecItems[_nFind].m_nColor = E_COLOR_FINISH;
    foreach(SLine it, m_vecLines )
    {
        if (it.m_nStart == _nFind)
            m_Schedule.enqueue(it.m_nEnd);
    }
}

void SDataMgr::RunJump(int _nNext)
{    
    foreach(SJump it, m_vecJump )
    {
        if (it.m_nSend == _nNext)
        {
            FindLineInsertSchedule(it.m_nRecv);
        }
    }
}

void SDataMgr::InsertSchedule(int _nNum)
{
    m_Schedule.enqueue(_nNum);
}

void SDataMgr::CommandProcess(int _nSelect,QStringList _strList)
{
    QStringList strValueList;
    foreach (QString strToken,_strList)
    {
        strToken = strToken.trimmed();
        if (strToken.at(0) == '*')
            strValueList.push_back(strToken);
        else
            g_MainWindow->Run(_nSelect,strToken,strValueList);
    }
}

bool SDataMgr::NextSchedule()
{
    while (!m_Schedule.isEmpty())
    {
        int nSelect = m_Schedule.dequeue();
        if (nSelect == D_SELECT_NONE) continue;
        switch(m_vecItems[nSelect].m_nType)
        {
        case E_TYPE_JUMP:
            if (m_vecItems[nSelect].m_str != "종료")
                RunJump(nSelect);
            m_vecItems[nSelect].m_nColor=E_COLOR_FINISH;            
            break;
        case E_TYPE_PROCESS:
            CommandProcess(nSelect,m_vecItems[nSelect].m_str.split("=",QString::SkipEmptyParts));
            break;
        case E_TYPE_PROCESS_THREAD:
            QStringList strList = m_vecItems[nSelect].m_str.trimmed().split(":");
            switch(strList.size())
            {
            case 1:
            case 3:
                CommandProcess(nSelect,strList.at(strList.size()-1).split("=",QString::SkipEmptyParts));
            }
            break;
        }
        m_lastSel = m_vecItems[nSelect].m_rectDraw;
        return true;
    }
    return false;
}

