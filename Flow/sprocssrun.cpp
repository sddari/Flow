#include "sprocssrun.h"
#include <QStringList>

#include "mainwindow.h"
extern MainWindow *g_MainWindow;

SProcssRun::SProcssRun(QObject *parent):
    QObject(parent)
{
    m_bIsRunning = false;
}

void SProcssRun::Run(QString _strCommand)
{    
    QStringList    arguments = _strCommand.split(" ");
    QString        program = arguments[0];
    arguments.removeFirst();
    m_Process.start(program, arguments);
    m_bIsRunning = true;
    m_Process.waitForFinished();
    m_bIsRunning = false;
}
