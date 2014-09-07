#include "scomthread.h"
#include "mainwindow.h"

extern MainWindow *g_MainWindow;

SComThread::SComThread(QObject *parent) :
    QThread(parent)
{
    m_pCom[0] = &m_Inter;
    m_pCom[1] = &m_dll;
    m_pCom[2] = &m_proc;    
}

void SComThread::Init(SView *_pView)
{    
    for (int i = 0 ; i < D_COMM_MAX; i++)
        m_pCom[i]->SetView(_pView);    
    m_pView = _pView;
}

bool SComThread::CommandRun(SCommand *_pCom)
{
    if (_pCom->SetCommand(g_MainWindow->MakeValue(m_strToken).trimmed(),m_nStart))
    {
        m_pView->GetDataMgr()->SetItemColor(m_nStart,E_COLOR_RUN);
        if (m_nStart != D_SELECT_NONE)
        {
            if (m_pView->GetDataMgr()->GetItem(m_nStart).m_nType == E_TYPE_PROCESS_THREAD) //아무것도 없을때 자신을 호출 하기 우함
                _pCom->InsertLine(m_nStart,"");
        }
        foreach(SLine stLine , m_pView->GetDataMgr()->GetLines())
        {
            if (m_nStart == stLine.m_nStart)
                _pCom->InsertLine(stLine.m_nEnd,g_MainWindow->MakeValue(stLine.m_str).trimmed());
        }
        foreach (QString str , m_strValueList)
            _pCom->InsertValue(str);
        _pCom->Run();
        return true;
    }
    return false;
}

void SComThread::run()
{
    for (int i = 0; i < D_COMM_MAX ;i++)
    {
        if (CommandRun(m_pCom[i]))
            return;
    }    
}
