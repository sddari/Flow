#ifndef SCOMTHREAD_H
#define SCOMTHREAD_H

#include <QThread>

#include "sdllcommgr.h"
#include "sintercommgr.h"
#include "sprocesscommgr.h"
#include "sview.h"

#define D_COMM_MAX 3

class SComThread : public QThread
{
    Q_OBJECT
public:
    explicit SComThread(QObject *parent = 0);
    void Init(SView *_pView );
    void SetData(int _nStart,QString _strToken,QStringList _strValueList)
    {
        m_nStart = _nStart;
        m_strToken = _strToken;
        m_strValueList = _strValueList;
    }
    void run();
signals:

public slots:
private:
    SInterComMgr m_Inter;
    SDllComMgr m_dll;
    SProcessComMgr m_proc;
    SCommand *m_pCom[D_COMM_MAX];
    int m_nStart;
    QString m_strToken;
    QStringList m_strValueList;
    SDataMgr *m_pData;
    SView *m_pView;

    bool CommandRun(SCommand *_pCom);
};

#endif // SCOMTHREAD_H
