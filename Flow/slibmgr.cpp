#include "slibmgr.h"

#include <QFile>
#include <QLibrary>
#include <QTextStream>
#include <QTextCodec>

SLibMgr::SLibMgr()
{    
}

SLibMgr::~SLibMgr()
{
    m_lib.unload();
}

bool SLibMgr::Load()
{
    typedef int (*Func)(char*,char*);
    m_lib.setFileName("Dll.dll");
    m_str.clear();
    if (m_lib.load()==false)
    {
        m_str.append("오류 : Dll.dll 읽기 실패");
        return false;
    }

    Func isDll = (Func)m_lib.resolve("isDll");
    run = (Func_run)m_lib.resolve("run");

    QFile file;
    file.setFileName("setup");
    file.open(QIODevice::ReadOnly|QIODevice::Text);

    QTextStream inStream( &file );
    QStringList strlist;
    while( !inStream.atEnd() )
    {
        QString strLine = inStream.readLine();
        QTextCodec *codec = QTextCodec::codecForLocale();
        strlist.append(codec->toUnicode( strLine.toLocal8Bit() ));
    }
    file.close();
    m_vecList.clear();
    bool bError = true;
    foreach(QString str,strlist)
    {
        str = str.trimmed();
        if (str.isEmpty()) break;
        SList list;
        list.m_list = str.split(',');
        switch(isDll((char*)list.m_list.at(E_DLL_FILE).toStdString().c_str(),
                     (char*)list.m_list.at(E_DLL_FUNC).toStdString().c_str()))
        {
        case 1:
            m_vecList.push_back(list);
            break;
        case -1:
            m_str.append(list.m_list.at(E_DLL_FILE));
            m_str.append("파일을 찾을수 없습니다.\n");
            bError = false;
            break;
        case -2:
            m_str.append(list.m_list.at(E_DLL_FILE));
            m_str.append("안에 ");
            m_str.append(list.m_list.at(E_DLL_FUNC));
            m_str.append("함수를 찾을수 없습니다.\n");
            bError = false;
            break;
        }
    }
    return bError;
}

int SLibMgr::FindComIndex(QString _strCom)
{
    for(int i = 0; i < m_vecList.size() ; i++)
    {
        if (_strCom.trimmed().compare(m_vecList[i].m_list.at(E_DLL_COM).trimmed()) == 0)
        {
            return i;
        }
    }
    return D_SELECT_NONE;
}

QString SLibMgr::toUniString(char *_res)
{
    QTextCodec * codec = QTextCodec::codecForName("eucKR");
    QString localeStr = codec->toUnicode(_res);
    return localeStr;
}

QString SLibMgr::Run(int _nSelect,QString _strArg)
{    
    QTextCodec * codec = QTextCodec::codecForName("eucKR");
    QByteArray byArg = codec->fromUnicode(_strArg);
    return toUniString(run((char*)m_vecList[_nSelect].m_list.at(E_DLL_FILE).toStdString().c_str(),
                    (char*)m_vecList[_nSelect].m_list.at(E_DLL_FUNC).toStdString().c_str(),
                    byArg.data()));
}
