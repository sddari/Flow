#ifndef SINTERCOMMGR_H
#define SINTERCOMMGR_H

#include <QString>
#include <QVector>
#include "scommand.h"

struct SCom
{
    QString strCom;
    QString (*Func)(QString);
};

void SetLoop(bool _bLoop);

class SInterComMgr : public SCommand
{
public:    
    SInterComMgr();
    bool SetCommand(QString _strCommand,int _nFinish);
    void Run();
private:
    int m_nSelect;
    QString m_strArg;
};

#endif // SINTERCOMMGR_H
