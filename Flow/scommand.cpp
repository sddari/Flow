#include "scommand.h"
#include "mainwindow.h"

extern MainWindow *g_MainWindow;

SCommand::SCommand()
{
    m_nNext = D_SELECT_NONE;
}

void SCommand::InsertLine(int _nNext,QString _strLine)
{
    if (_strLine.isEmpty())
        m_nNext = _nNext;
    else
    {
        SLine stLine;
        stLine.m_nNext = _nNext;
        stLine.m_strLine = _strLine;
        m_vecLines.push_back(stLine);
    }
}

void SCommand::InsertValue(QString _strValue)
{
    m_Value.push_back(_strValue);
}

void SCommand::Start(QString _strCom)
{
    g_MainWindow->InsertMsg("명령 : " + _strCom);
}

void SCommand::End(QString _strOut)
{
    foreach(SLine stLine,m_vecLines)
    {
        QString strCompare = _strOut.left(stLine.m_strLine.length());
        if (strCompare.compare(stLine.m_strLine) == 0)
            m_nNext = stLine.m_nNext;
    }
    foreach(QString str,m_Value)
    {
        g_MainWindow->SetResultValue(str.trimmed(),_strOut.trimmed());
        g_MainWindow->InsertMsg("변수 : " + str.trimmed());
        g_MainWindow->InsertMsg("결과 : " + _strOut.trimmed());
    }        
    if (m_nFinish != D_SELECT_NONE)
        m_pView->ItemFinish(m_nFinish);
    if (m_nNext != D_SELECT_NONE)
        m_pView->InsertSchedule(m_nNext);
    m_Value.clear();
    m_vecLines.clear();
}
