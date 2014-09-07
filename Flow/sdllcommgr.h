#ifndef SDLLCOMMGR_H
#define SDLLCOMMGR_H

#include "scommand.h"


class SDllComMgr : public SCommand
{    
public:    
    SDllComMgr();    
    bool SetCommand(QString _strCommand,int _nFinish);
    void Run();    
private:        
    QString m_strArg;
    int m_nSelect;
};

#endif // SDLLCOMMGR_H
