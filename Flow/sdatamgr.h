#ifndef SDATAMGR_H
#define SDATAMGR_H

#include <QString>
#include <QStringList>
#include <QRectF>
#include <QVector>
#include <QQueue>
#include <QLineF>
#include "Define.h"

class SView;

enum E_COLOR
{
    E_COLOR_NONE = 0,
    E_COLOR_ERROR,
    E_COLOR_RUN,
    E_COLOR_FINISH,
};

typedef struct _SItem
{
    QString m_str;
    QRectF m_rectDraw;
    QRectF m_rectText;
    int m_nType;
    int m_nColor;    
    _SItem()
    {
        m_nColor = E_COLOR_NONE;
    }
} SItem;

typedef struct _SLine
{
    QString m_str;
    QRectF m_rectText;
    int m_nStart;
    int m_nEnd;
} SLine;

typedef struct _SJump
{
    int m_nRecv;
    int m_nSend;
} SJump;

class SDataMgr
{
public:
    SDataMgr();
    bool LoadFile(QString _strFilePath);
    bool SaveFile(QString _strFilePath);
    QVector <SItem> GetItems() { return m_vecItems;  }
    QVector <SLine> GetLines() { return m_vecLines;  }
    QRectF GetItemRect(int _nSelect){return m_vecItems[_nSelect].m_rectDraw;}
    QRectF GetItemRectStr(int _nSelect){return m_vecItems[_nSelect].m_rectText;}
    QString GetLineStr(int _nSelect) {return m_vecLines[_nSelect].m_str;}
    QRectF GetLineStrRect(int _nSelect) {return m_vecLines[_nSelect].m_rectText;}
    SItem GetItem(int _nSelect) {return m_vecItems[_nSelect];}    
    SLine GetLine(int _nSelect) {return m_vecLines[_nSelect];}    
    void ItemMoveLineText(int _nItem);
    int MakeJump();
    bool Start();
    void Stop();
    bool NextSchedule();
    void InsertSchedule(int _nNum);

    void SetItemColor(int _nSelect,int _nColor)
    {
        if (_nSelect == D_SELECT_NONE) return;
        m_vecItems[_nSelect].m_nColor = _nColor;
    }

    void SetLineRectText(int _nSelect,QPointF _pt,QSize _sz)
    {
        m_vecLines[_nSelect].m_rectText.setTopLeft(_pt);
        m_vecLines[_nSelect].m_rectText.setSize(_sz);
    }

    void SetLineRectText(int _nSelect,QPointF _pt,QSize _sz,QString _str)
    {
        m_vecLines[_nSelect].m_rectText.setTopLeft(_pt);
        m_vecLines[_nSelect].m_rectText.setSize(_sz);
        m_vecLines[_nSelect].m_str = _str;
    }

    void SetItem(int _nSelect,QString _str,QSize _szText,QSize _szDraw)
    {
        m_vecItems[_nSelect].m_str = _str;
        m_vecItems[_nSelect].m_rectText.setSize(_szText);
        m_vecItems[_nSelect].m_rectDraw.setSize(_szDraw);
    }

    bool DeleteItem(QPoint _pt);
    void DeleteItem(int nSelect);    
    bool DeleteLine(QPoint _pt,QPoint _back);

    int FindItem(QPoint _pt);
    int FindLine(QPoint _pt,QPoint _back);

    bool InsertLine(QPoint _ptStart,QPoint _ptEnd);
    void InsertItem(int _nType,QPoint _pt,QSize _sz);

    void ChkItemText(int _nSelect);
    void ItemTrans(int _nSelect,QPoint _pt);
    void InitData(float _fRectExt,QSize _sizeFont, SView *_pView)
    {
        m_fRectExt = _fRectExt;
        m_sizeFont = _sizeFont;
        m_pView = _pView;
    }

    QString  GetMsg() {return m_strMsg;}
    QRectF  GetSelectNow() {return m_lastSel;}

private:
    QVector <SItem> m_vecItems;
    QVector <SLine> m_vecLines;
    QVector <SJump> m_vecJump;
    QStringList m_listFinish;
    QQueue <int> m_Schedule;    
    float m_fRectExt;
    QSize m_sizeFont;
    QString m_strMsg;
    SView *m_pView;
    QRectF m_lastSel;

    void SetLineTextRect(SLine *_pLine);
    qreal GetDistance( QPointF _p1, QPointF _p2 );
    bool IsSelectLine(QPointF _point,QLineF _line);
    void SetRectExt(QRectF *rect);

    void FindLineInsertSchedule(int _nFind);
    void RunJump(int _nNext);
    void MakeLoop();
    void CommandProcess(int _nSelect,QStringList _strList);
};

#endif // SDATAMGR_H
