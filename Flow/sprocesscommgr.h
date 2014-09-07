#ifndef SPROCESSCOMMGR_H
#define SPROCESSCOMMGR_H

#include "scommand.h"

class SProcessComMgr : public SCommand
{
public:
    SProcessComMgr();
    bool SetCommand(QString _strCommand,int _nFinish)  {m_strCommand = _strCommand;m_nFinish=_nFinish;return true;}
    void Run();
};

#endif // SPROCESSTHREAD_H
