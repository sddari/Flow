#include "sdllcommgr.h"
#include "slibmgr.h"
#include <qstring.h>

extern SLibMgr g_lib;

SDllComMgr::SDllComMgr()
{
}

bool SDllComMgr::SetCommand(QString _strCommand,int _nFinish)
{
    m_strCommand = _strCommand;
    m_nFinish=_nFinish;
    QStringList strList = _strCommand.split(" ");

    m_nSelect = g_lib.FindComIndex(strList.at(0).trimmed());
    if (m_nSelect == D_SELECT_NONE)
        return false;
    strList.removeFirst();
    m_strArg = strList.join(" ");

    return true;
}

void SDllComMgr::Run()
{
    Start(m_strCommand);
    End(g_lib.Run(m_nSelect,m_strArg));
}
