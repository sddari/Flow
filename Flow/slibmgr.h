#ifndef SLIBMGR_H
#define SLIBMGR_H

#include <QVector>
#include <QStringList>
#include <QLibrary>
#include "Define.h"

typedef char * (*Func_run)(char*,char*,char*);

class SLibMgr
{
    struct SList
    {
        QStringList m_list;
    };
public:    
    bool Load();
    SLibMgr();
    ~SLibMgr();
    QString GetMsg() { return m_str;}
    int FindComIndex(QString _strCom);
    QString Run(int _nSelect,QString _strArg);
    QString toUniString(char *_res);
private:
    QVector <SList> m_vecList;
    QString m_str;
    enum E_DLL
    {
        E_DLL_FILE = 0,
        E_DLL_FUNC,
        E_DLL_COM,
    };
    int m_nSelect;
    QLibrary m_lib;
    Func_run run;
};

#endif // SLIBMGR_H
