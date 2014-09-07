#ifndef SSETUPDLG_H
#define SSETUPDLG_H

#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QVBoxLayout>

class SSetupDlg : public QWidget
{
    Q_OBJECT
public:
    explicit SSetupDlg(QWidget *parent = 0);
    void SetSpeed(int _nTime);

signals:

public slots:
    void SetValue(int _nValue);
private:
    QSlider *m_pslider;
    QLabel *m_plabel;
    QVBoxLayout *m_playout;
};

#endif // SSETUPDLG_H
