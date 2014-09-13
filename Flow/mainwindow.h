#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QButtonGroup>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QListWidget>
#include <QTimer>

#include "scomthread.h"
#include "Define.h"
#include "sview.h"
#include "SSetupDlg.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:    

private slots:
    void buttonGroupClicked(int _nId);    
    void fileNew();
    void fileOpen();
    void fileSave();
    void fileSaveAs();
    void ComStart();
    void ComPause();
    void ComStop();
    void Setup();
    void helpAbout();
    void Loop();
    void CloseTab(const int& index);
    void ChangeTab(const int& index);
public:
    void SetResultValue(QString _strName,QString _strValue);
    QString GetResultValue(QString _strName);
    QString MakeValue(QString _strCom);
    void InsertMsg(QString _strMsg);
    void InsertSchedule(int _nSchedule);
    void Stop() {ComStop();}
    void UpdateMsg();
    bool Run(int _nStart,QString _strToken,QStringList _strValueList,bool _bThread = true);
    void SetThread(int _nCount);
    int GetRunningProcess();
    int GetTime() {return m_nTime;}
    void SetTime(int _nTime);
private:
    void createMenu();
    QAction *createAction(QString _str,int _key,const char *_slot);
    QAction *createAction(QString _str,const char *_slot);
    QWidget *createTool();
    QWidget *createResult();
    SComThread *GetIdleProcess();
    void DeleteThread();
    void MessageBox(QString _strMsg);

    QButtonGroup *m_buttonGroup;
    QPixmap image(E_TYPE _eType);
    QTabWidget *m_pTab;
    QTimer m_Timer;
    QWidget *m_pTool;
    QWidget *m_pResult;
    QListWidget *m_pResultList;
    QTableWidget *m_pValueList;
    QString m_strMsg;

    SSetupDlg *m_pSetupDlg;

    QAction *m_pMenuComStart;
    QAction *m_pMenuComPause;
    QAction *m_pMenuComStop;
    SComThread *m_pCom;
    int m_nProcessSize;
    int m_nTime;

    struct SValueIndex
    {
        int m_nIndex;
        int m_nSize;
    };

    QVector <SValueIndex> m_vecValueIndex;
    ////////////////////////////
    /// \brief fileNew
    ///
};

#endif // MAINWINDOW_H
