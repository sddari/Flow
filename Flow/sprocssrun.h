#ifndef SPROCSSRUN_H
#define SPROCSSRUN_H

#include <QProcess>
#include <QObject>
#include <QVector>

class SProcssRun : public QObject
{
    Q_OBJECT
public:
    SProcssRun(QObject *parent = 0);    
    QString GetOutput() {return QString(m_Process.readAllStandardOutput());}
    bool IsRunning() {return m_bIsRunning;}
    void Run(QString _strCommand);
private:
    QProcess m_Process;    
    bool m_bIsRunning;
};

#endif // SPROCSSRUN_H
