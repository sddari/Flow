#include "ssetupdlg.h"
#include "MainWindow.h"

extern MainWindow *g_MainWindow;

SSetupDlg::SSetupDlg(QWidget *parent) :
    QWidget(parent)
{
    m_pslider = new QSlider(Qt::Horizontal,0);
    m_plabel = new QLabel;
    m_playout = new QVBoxLayout;

    m_playout->addWidget(m_plabel);
    m_playout->addWidget(m_pslider);
    setLayout(m_playout);

    m_pslider->setTickInterval(200);
    m_pslider->setTickPosition(QSlider::TicksAbove);
    m_pslider->setMinimum(5);
    m_pslider->setMaximum(5000);

    connect(m_pslider,SIGNAL(valueChanged(int)),SLOT(SetValue(int)));
    setWindowTitle("속도 조절");
}

void SSetupDlg::SetValue(int _nValue)
{
    SetSpeed(_nValue);

    g_MainWindow->SetTime(_nValue);
}

void SSetupDlg::SetSpeed(int _nTime)
{
    m_plabel->setText("현재 속도 : " + QString::number(_nTime) + "/1000초") ;
    m_pslider->setValue(_nTime);
}
