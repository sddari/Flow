#ifndef SVIEW_H
#define SVIEW_H

#include <QWidget>
#include <QMouseEvent>
#include <QLineEdit>
#include <QAction>
#include <QDataStream>
#include <QQueue>
#include <QMutex>

#include "Define.h"
#include "sdatamgr.h"
#include "scomthread.h"

enum E_MOUSE_STATE
{
    E_MOUSE_STATE_NONE = 0,
    E_MOUSE_STATE_DOWN ,
    //E_MOUSE_STATE_UP,
};

enum E_MODE
{
    E_MODE_EDIT = 0,
    E_MODE_RUN
};

class SView : public QWidget
{
    Q_OBJECT
public:
    explicit SView(QWidget *parent = 0);
    ~SView();
    void SelectItem(int _nItem);
    void SetFilePath(QString _str) { m_strFilePath = _str;}
    QString GetFileName();
    bool SaveFile();
    bool LoadFile();
    bool Start();
    void Stop();
    void NextSchedule();
    void InsertSchedule(int _nNum);
    void ItemFinish(int _nFinish);    
    SDataMgr* GetDataMgr() {return &m_data;}    
    int GetButtonItem() {return m_nItem;}
signals:

public slots:
    void SlotEditingFinished();
    void SlotEditing(QString text);
protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent ( QMouseEvent * e );
    void mouseMoveEvent(QMouseEvent* e);
    void mouseReleaseEvent ( QMouseEvent * e );
private:            
    QPoint m_pointMove;
    QPoint m_pointLineS,m_pointLineE;
    QPoint m_pointBack,m_pointBackup;
    int m_nItem;
    QLineEdit *m_Edit;
    int m_nEditType;
    int m_nSelect;
    int m_nMouseState;
    float m_fRectExt;
    QSize m_sizeFont;
    QString m_strFilePath;
    int m_nMode;        
    SDataMgr m_data;
private:
    void InitRectSizeExt(float _fRatio);
    void SetRectExt(QRectF *rect);
    void SetEditRect(QRectF rect);    

    QPointF GetPointCross(int nIndex,QLineF line);
    QPolygonF GetPolygonArrow(QLineF line,qreal arrowSize);    
};

#endif // SVIEW_H
