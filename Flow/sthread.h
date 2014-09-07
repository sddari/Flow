#ifndef STHREAD_H
#define STHREAD_H

#include <QThread>
#include "sview.h"

class SThread : public QThread
{
    Q_OBJECT
public:
    explicit SThread(QObject *parent = 0);
    void stop();
    void SetControlView(SView *_pView) {m_pView = _pView;}

signals:

public slots:
protected:
    void run();

private:
    volatile bool stopped;
    SView *m_pView;
};

#endif // STHREAD_H
