#include "sintercommgr.h"
#include <QStringList>
#include <sstream>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QtCore/QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <time.h>

using namespace std;

QString output(QString str);
QString calc(QString str);
QString string_ext(QString str);
QString trans(QString str);
QString is_english(QString str);
QString file_save(QString str);
QString file_load(QString str);
QString to_sleep(QString str);

SCom g_com[] =
{
    {"출력" , output } ,
    {"계산" , calc } ,
    {"문자_추출" , string_ext },
    {"번역" , trans },
    {"영어입니까" , is_english },
    {"파일_저장하기" , file_save },
    {"파일_불러오기" , file_load },
    {"잠자다" , to_sleep },
  {"" , 0 } ,
};

SInterComMgr::SInterComMgr()
{
}

bool SInterComMgr::SetCommand(QString _strCommand,int _nFinish)
{
    m_strCommand = _strCommand;
    m_nFinish=_nFinish;
    QStringList strList = _strCommand.split(" ");
    for(int i = 0; !g_com[i].strCom.isEmpty() ; i++)
    {
        if (strList.at(0).trimmed().compare(g_com[i].strCom) == 0)
        {
            m_nSelect = i;
            strList.removeFirst();
            m_strArg = strList.join(" ");
            return true;
        }
    }
    return false;
}

void SInterComMgr::Run()
{
    Start(m_strCommand);
    End(g_com[m_nSelect].Func(m_strArg));
}

bool g_bLoop;

void SetLoop(bool _bLoop)
{
    g_bLoop = _bLoop;
}

QString output(QString str)
{    
    return str;
}

QString calc(QString str)
{
    string postfixExpression;
    const string simbols = "(+-*/) ";

    {
        string infixExpression = str.toStdString();
        string::iterator i = infixExpression.begin();
        vector<char> stack;
        for (; i != infixExpression.end(); ++i)
        {
            if (simbols.find(*i) == string::npos)
            {
                postfixExpression += *i;
                continue;
            }

            postfixExpression += " ";
            switch (*i)
            {
            case '(': stack.push_back('('); break;
            case ')':
                while (stack.back() != '(')
                {
                    postfixExpression += stack.back();
                    postfixExpression += " ";
                    stack.pop_back();
                }
                stack.pop_back();
                break;
            case '+':
            case '-':
                while(stack.size() != 0 && stack.back() != '(')
                {
                    postfixExpression += stack.back();
                    postfixExpression += " ";
                    stack.pop_back();
                }
                stack.push_back(*i);
                break;
            case '*':
            case '/':
                while(stack.size() != 0 && (stack.back() == '*' || stack.back() == '/'))
                {
                    postfixExpression += stack.back();
                    postfixExpression += " ";
                    stack.pop_back();
                }
                stack.push_back(*i);
                break;
            case ' ': break;
            default:
                break;
            }
            postfixExpression += " ";
        }

        size_t stackSize = stack.size();
        for (size_t i = 0; i < stackSize; ++i)
        {
            postfixExpression += " ";
            postfixExpression += stack.back();
            stack.pop_back();
        }
    }

    {
        vector<double> stack;
        string str = "X";

        stringstream temp(postfixExpression);
        while(!temp.eof())
        {
            temp >> str;
            if (simbols.find(str) == string::npos)
            {
                double dtemp;
                stringstream(str) >> dtemp;
                stack.push_back(dtemp);
            }
            else
            {
                double d1, d2;
                d2 = stack.back();
                stack.pop_back();
                d1 = stack.back();
                stack.pop_back();
                switch (simbols[simbols.find(str)])
                {
                case '+':
                    stack.push_back(d1 + d2); break;
                case '-':
                    stack.push_back(d1 - d2); break;
                case '*':
                    stack.push_back(d1 * d2); break;
                case '/':
                    stack.push_back(d1 / d2); break;
                }
            }
        }
        return QString::number(stack.back());
    }
}

bool IsNumber(QString str)
{
    for (int i = 0 ; i < str.size() ; i++)
    {
        if(str.at(i).isNumber() == false)  return false;
    }
    return true;
}

QString string_ext(QString str)
{
    QVector <int>  vecNumber;
    {
        QStringList strList = str.split(" ");
        if (strList.size() < 1)
            return "오류 : 인자 개수 오류";

        foreach (str,strList)
        {
            if (IsNumber(str))
                vecNumber.push_back(str.toInt());
            else
                break;
        }
    }

    QChar ch[2] = {';',','};
    for (int i = 0; i < vecNumber.size(); i++)
    {
        QStringList strList = str.split(ch[i]);
        if (vecNumber[i] >=  strList.size())
        {
            str = "<완료>";
            break;
        }
        str = strList[vecNumber[i]];
    }
    return str;
}

QString trans(QString str)
{
    QStringList strList = str.split(" ");
    QString from = strList.at(0);
    QString to = strList.at(1);
    strList.removeFirst();
    strList.removeFirst();
    QString keyword = strList.join(" ");
    keyword.replace("\n"," ");
    keyword.replace(";",".");   

    strList = keyword.split(".");
    QString strOut;

    for (int i = 0 ;i < strList.size() ;i++)
    {        
        QString url = QString("http://translate.google.com/translate_a/t?client=t&text=%0&hl=%1&sl=%2&tl=%1&multires=1&prev=enter&oc=2&ssel=0&tsel=0&uptl=%1&sc=1").arg(strList.at(i)).arg(to).arg(from);
        QNetworkAccessManager manager;
        QNetworkRequest request(url);
        QNetworkReply *reply = manager.get(request);
        do {
            QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        } while(!reply->isFinished());        
        QString translation(reply->readAll());
        reply->close();

        delete reply;
        translation = translation.replace("[[[\"", "");
        strOut += translation.mid(0, translation.indexOf(",\"") - 1);
        strOut += ".";
    }
    return strOut;
}

QString is_english(QString str)
{
    for (int i = 0; i < str.size(); i++)
    {
        if (str.at(i).isPrint()) continue;
        return "아니오";
    }
    return "예";
}

QString file_save(QString str)
{
    QStringList strList = str.split(" ");
    QFile file(strList.at(0));
    strList.erase(strList.begin());
    file.open(QIODevice::WriteOnly|QIODevice::Append|QIODevice::Text);
    QTextStream in(&file);
    in << strList.join(" ");
    file.close();
    return "file_save 완료";
}

QString file_load(QString str)
{
    QFile file(str);
    if(file.open(QIODevice::ReadOnly|QIODevice::Text) == false)
    {
        return "오류 : 파일을 열지 못함";
    }
    QTextStream in(&file);
    in >> str;
    file.close();
    return str;
}

QString to_sleep(QString str)
{
    if (IsNumber(str) == false)
        return "오류 : 인자값 오류";
    clock_t wait = clock () + str.toInt() * CLOCKS_PER_SEC;
    while (clock() < wait && g_bLoop);
    return "to_sleep 완료";
}
