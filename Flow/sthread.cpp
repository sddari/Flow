#include "sthread.h"
#include "mainwindow.h"
extern MainWindow *g_MainWindow;


SThread::SThread(QObject *parent) :
    QThread(parent)
{
    stopped = false;
    m_pView = 0;
}

void SThread::stop()
{
    stopped = true;
}

void SThread::run()
{
    if (m_pView == 0) return;
    g_MainWindow->InsertMsg("Start\n");
    if (m_pView->Start() == true)
    {
        while(!stopped)
        {
            m_pView->NextSchedule();
        }
    }
    g_MainWindow->InsertMsg("Stop\n");
    m_pView->Stop();
    stopped=false;
}
