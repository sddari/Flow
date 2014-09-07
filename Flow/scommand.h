#ifndef SCOMMAND_H
#define SCOMMAND_H

#include <QVector>
#include <QStringList>

class SView;

class SCommand
{
public:
    SCommand();
    void InsertLine(int _nNext,QString _strLine);
    int GetNext() {return m_nNext;}
    void InsertValue(QString strValue);
    void SetView(SView *_pView) {m_pView = _pView;}
    void Start(QString _strCom);
    void End(QString _strOut);

    virtual bool SetCommand(QString ,int ) {return true;}
    virtual void Run(){;}
protected:
    int m_nNext;
    int m_nFinish;
    QString m_strCommand;
    QStringList m_Value;
    struct SLine
    {
        QString m_strLine;
        int m_nNext;
    };
    QVector <SLine> m_vecLines;
    SView *m_pView;
};

#endif // SCOMMAND_H
