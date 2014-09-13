#include <QVBoxLayout>
#include <QPushButton>
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QToolBox>
#include <QApplication>
#include <QMenuBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextEdit>
#include <QTableWidget>
#include <QTime>
#include <QtDebug>
#include <QTextCodec>

#include "mainwindow.h"
#include "slibmgr.h"

#include <stdio.h>
#include <time.h>
void log(const char *_pchFmt, ...)
{
    time_t now;
    time(&now);
    struct tm t = *localtime(&now);
    char achFilename[_MAX_PATH];
    sprintf(achFilename, ".\\log\\%d-%02d-%02d.txt", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday);

    FILE *pFile = fopen(achFilename, "a+");
    fprintf(pFile, "[%02d:%02d:%02d] ", t.tm_hour, t.tm_min, t.tm_sec);

    char achBuffer[1024];
    va_list ap;
    va_start(ap, _pchFmt);
    vsprintf(achBuffer, _pchFmt, ap);
    va_end(ap);

    QTextCodec * codec = QTextCodec::codecForName("eucKR");
    QByteArray byArg = codec->fromUnicode(achBuffer);

    fprintf(pFile,byArg.data());
    fprintf(pFile, "\n");
    fclose(pFile);
}

SLibMgr g_lib;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    createMenu();
    QVBoxLayout *layout = new QVBoxLayout;

    m_pResult = createResult();

    {
        QHBoxLayout *upLayout = new QHBoxLayout;
        m_pTab = new QTabWidget(this);
        m_pTab->setTabsClosable(true);
        connect(m_pTab, SIGNAL(tabCloseRequested(int)), this, SLOT(CloseTab(int)));
        connect(m_pTab, SIGNAL(currentChanged(int)),this, SLOT(ChangeTab(int)));

        m_pTool = createTool();
        upLayout->addWidget(m_pTool);
        upLayout->addWidget(m_pTab);

        layout->addLayout(upLayout);
        layout->addWidget(m_pResult);
    }

    QWidget *widget = new QWidget;
    widget->setLayout(layout);
    setCentralWidget(widget);
    setWindowTitle(tr("Flow"));

    QObject::connect(&m_Timer,SIGNAL(timeout()),SLOT(Loop()));

    m_pResult->hide();    
    fileNew();

    m_nProcessSize = 0;

    if (g_lib.Load() == false)
        QMessageBox::information(this, "오류",g_lib.GetMsg());

    m_pSetupDlg = new SSetupDlg();
    m_nTime = 100;
}

MainWindow::~MainWindow()
{
    ComStop();
    DeleteThread();
    delete m_pSetupDlg;
}

void MainWindow::SetThread(int _nCount)
{
    DeleteThread();
    m_nProcessSize = _nCount;
    m_pCom = new SComThread [m_nProcessSize];
    for (int i = 0; i < m_nProcessSize; i++)
        m_pCom[i].Init((SView*)m_pTab->currentWidget());
}

void MainWindow::DeleteThread()
{
    if (m_nProcessSize == 0) return;
    delete [] m_pCom;
}

QAction *MainWindow::createAction(QString _str,int _key,const char *_slot)
{
    QAction *action = new QAction (_str,this);
    action->setShortcut((QKeySequence::StandardKey)_key);
    connect(action,SIGNAL(triggered()),this,_slot);
    return action;
}

QAction *MainWindow::createAction(QString _str,const char *_slot)
{
    QAction *action = new QAction (_str,this);
    connect(action,SIGNAL(triggered()),this,_slot);
    return action;
}

void MainWindow::fileNew()
{
    if (!m_pTab->isEnabled()) {MessageBox("프로세스가 실행 중에는 수행 할 수 없습니다.");return;}
    static int nNew = 1;
    QString str;
    str.sprintf("New %d",nNew++);
    m_pTab->addTab(new SView(),str);
    m_pTab->setCurrentIndex(m_pTab->count()-1);
}

void MainWindow::fileOpen()
{
    if (!m_pTab->isEnabled()) {MessageBox("프로세스가 실행 중에는 수행 할 수 없습니다.");return;}
    SView *pView = new SView();
    pView->SetFilePath(QFileDialog::getOpenFileName(this,tr("Open File"),"Flow File", tr("File (*.flo)")));
    if (!pView->GetFileName().isEmpty())
    {
        m_pTab->addTab(pView,pView->GetFileName());
        m_pTab->setCurrentIndex(m_pTab->count()-1);
        pView->LoadFile();
    }
    else
        delete pView;
}

void MainWindow::fileSave()
{
    if (!m_pTab->isEnabled()) {MessageBox("프로세스가 실행 중에는 수행 할 수 없습니다.");return;}
    SView *pView = (SView*)m_pTab->currentWidget();
    if (pView == 0 ) return;
    if (pView->GetFileName().isEmpty())
        pView->SetFilePath(QFileDialog::getSaveFileName(this,tr("Save File"),"Flow File", tr("File (*.flo)")));
    if (!pView->GetFileName().isEmpty())
    {
        m_pTab->setTabText(m_pTab->currentIndex(),pView->GetFileName());
        pView->SaveFile();
    }
}

void MainWindow::fileSaveAs()
{
    if (!m_pTab->isEnabled()) {MessageBox("프로세스가 실행 중에는 수행 할 수 없습니다.");return;}
    SView *pView = (SView*)m_pTab->currentWidget();
    if (pView == 0 ) return;
    pView->SetFilePath(QFileDialog::getSaveFileName(this,tr("Save as File"),"Flow File", tr("File (*.flo)")));
    if (!pView->GetFileName().isEmpty())
    {
        m_pTab->setTabText(m_pTab->currentIndex(),pView->GetFileName());
        pView->SaveFile();
    }
}

void MainWindow::Loop()
{    
    ((SView*)m_pTab->currentWidget())->NextSchedule();
    ((SView*)m_pTab->currentWidget())->repaint();    
    UpdateMsg();
}

void MainWindow::SetTime(int _nTime)
{
    m_nTime = _nTime;
    if (m_Timer.isActive())
    {
        m_Timer.stop();
        m_Timer.start(m_nTime);
    }
}

void MainWindow::ComStart()
{    
    m_strMsg.clear();
    m_vecValueIndex.clear();
    while (m_pValueList->rowCount() > 0)
        m_pValueList->removeRow(0);
    SView *pView = (SView*)m_pTab->currentWidget();
    if (pView == 0) return;
    ComStop();
    if (((SView*)m_pTab->currentWidget())->Start() == true)
    {        
        m_Timer.start(m_nTime);
    }
    m_pTool->hide();
    m_pResult->show();
    m_pTab->setDisabled(true);    
    m_pMenuComStart->setEnabled(false);
    m_pMenuComPause->setEnabled(true);
    m_pMenuComStop->setEnabled(true);
    m_pMenuComPause->setText("일시 정지");    
}

void MainWindow::ComPause()
{
    if (m_Timer.isActive())
    {
        m_Timer.stop();
        m_pMenuComPause->setText("계속");
    }
    else
    {
        m_Timer.start(m_nTime);
        m_pMenuComPause->setText("일시 정지");
    }
}

void MainWindow::ComStop()
{
    if (m_Timer.isActive())
        m_Timer.stop();    
    SView *pView = (SView*)m_pTab->currentWidget();
    if (pView == 0) return;
    pView->Stop();

    while(GetRunningProcess());

    m_pTool->show();
    m_pResult->hide();
    m_pTab->setDisabled(false);
    m_pMenuComStart->setEnabled(true);
    m_pMenuComPause->setEnabled(false);
    m_pMenuComStop->setEnabled(false);
    m_pMenuComPause->setText("일시 정지");
}

int MainWindow::GetRunningProcess()
{
    int nCount = 0;
    for (int i = 0; i < m_nProcessSize; i++)
        if (m_pCom[i].isRunning())
            nCount++;
    return nCount;
}

void MainWindow::Setup()
{
    m_pSetupDlg->SetSpeed(m_nTime);
    m_pSetupDlg->activateWindow();
    m_pSetupDlg->show();
}

void MainWindow::helpAbout()
{
    QMessageBox::about(this,"About","Flow v0.3beta\n컴파일러 Qt 5.3.0 2014-6\n만든이 송 태 식\nE-mail sddari@gmail.com");
}

void MainWindow::MessageBox(QString _strMsg)
{
    QMessageBox msgBox;
    msgBox.addButton(QMessageBox::Ok);
    msgBox.setText(_strMsg);
    msgBox.exec();
}


void MainWindow::createMenu()
{
    QMenu *menuFile = menuBar()->addMenu(tr("파일"));
    menuFile->addAction(createAction(tr("새로 만들기"),QKeySequence::New,SLOT(fileNew())));
    menuFile->addAction(createAction(tr("열기"),QKeySequence::Open,SLOT(fileOpen())));
    menuFile->addAction(createAction(tr("저장"),QKeySequence::Save,SLOT(fileSave())));
    menuFile->addAction(createAction(tr("다른 이름으로 저장"),QKeySequence::SaveAs,SLOT(fileSaveAs())));
    menuFile->addSeparator();
    menuFile->addAction(createAction(tr("종료"),QKeySequence::SaveAs,SLOT(close())));
    QMenu *menuCom = menuBar()->addMenu(tr("명령"));
    m_pMenuComStart = createAction(tr("시작"),SLOT(ComStart()));
    menuCom->addAction(m_pMenuComStart);
    m_pMenuComStart->setEnabled(true);
    m_pMenuComPause = createAction(tr("일시 정지"),SLOT(ComPause()));
    menuCom->addAction(m_pMenuComPause);
    m_pMenuComPause->setEnabled(false);
    m_pMenuComStop = createAction(tr("종료"),SLOT(ComStop()));
    menuCom->addAction(m_pMenuComStop);
    m_pMenuComStop->setEnabled(false);
    QMenu *menuSetup = menuBar()->addMenu(tr("설정"));
    menuSetup->addAction(createAction(tr("속도"),SLOT(Setup())));
    QMenu *menuHelp = menuBar()->addMenu(tr("도움말"));
    menuHelp->addAction(createAction(tr("이 프로그램은..."),SLOT(helpAbout())));
}

QWidget *MainWindow::createTool()
{
    m_buttonGroup = new QButtonGroup(this);
    m_buttonGroup->setExclusive(false);
    connect(m_buttonGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(buttonGroupClicked(int)));

    QVBoxLayout *layout = new QVBoxLayout;
    for (int i = 0 ; i < E_TYPE_MAX;i++)
    {
        QIcon icon(image((E_TYPE)i));

        QPushButton *button = new QPushButton;
        button->setIcon(icon);
        button->setIconSize(QSize(50, 50));
        button->setCheckable(true);
        layout->addWidget(button);
        m_buttonGroup->addButton(button,i);
    }

    layout->setAlignment(Qt::AlignTop);
    QWidget *widget = new QWidget(this);
    widget->setLayout(layout);
    return widget;
}

QWidget *MainWindow::createResult()
{
    m_pResultList = new QListWidget(this);

    m_pValueList = new QTableWidget(this);
    m_pValueList->setColumnCount(2);
    m_pValueList->setHorizontalHeaderLabels(QString("변수 이름;값").split(";"));

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(m_pResultList);
    layout->addWidget(m_pValueList);
    QWidget *widget = new QWidget(this);
    widget->setLayout(layout);

    return widget;
}

QString MainWindow::MakeValue(QString _strCom)
{
    for (int i = 0; i < m_vecValueIndex.size(); i++)
    {
        QString strName = m_pValueList->item(m_vecValueIndex[i].m_nIndex, 0)->text();
        QString strValue = m_pValueList->item(m_vecValueIndex[i].m_nIndex, 1)->text();
        _strCom.replace(strName,strValue);
    }
    return _strCom;
}

void MainWindow::SetResultValue(QString _strName,QString _strValue)
{    
    for (int i = 0; i < m_vecValueIndex.size(); i++)
    {
        if (m_pValueList->item(m_vecValueIndex[i].m_nIndex, 0)->text() == _strName)
        {
            m_pValueList->item(m_vecValueIndex[i].m_nIndex, 1)->setText(_strValue);
            return;
        }
    }

    int nSelect = 0;
    for (int i = 0; i < m_vecValueIndex.size(); i++)
    {
        if (m_vecValueIndex[i].m_nSize < _strName.length())
        {
            nSelect = i;
            break;
        }

    }

    int nSize = m_pValueList->rowCount();
    SValueIndex stIndex;
    stIndex.m_nIndex = nSize;
    stIndex.m_nSize = _strName.length();
    m_vecValueIndex.insert(nSelect,stIndex);

    m_pValueList->setRowCount(nSize+1);
    m_pValueList->setItem(nSize,0,new QTableWidgetItem(_strName));
    m_pValueList->setItem(nSize,1,new QTableWidgetItem(_strValue));
}

QString MainWindow::GetResultValue(QString _strName)
{    
    for (int i = 0; i < m_vecValueIndex.size(); i++)
    {
        if (m_pValueList->item(m_vecValueIndex[i].m_nIndex, 0)->text() == _strName)
            return m_pValueList->item(m_vecValueIndex[i].m_nIndex, 1)->text();
    }
    return "";
}

void MainWindow::UpdateMsg()
{    
    m_pValueList->viewport()->update();
    m_pResultList->viewport()->update();
    m_pResultList->scrollToBottom();
}

void MainWindow::InsertMsg(QString _strMsg)
{    
    if (m_pResultList->count() > 1000)
    {
        delete m_pResultList->item(0);
    }

    static QStringList list;

    if (list.size() > 4)
    {
        list.removeFirst();
        foreach(QString str,list)
        {
            if (str == _strMsg)
            {
                list.push_back(_strMsg);
                return;
            }
        }
    }
    list.push_back(_strMsg);
    m_pResultList->addItem(_strMsg);
}

QPixmap MainWindow::image(E_TYPE _eType)
{
    QPixmap pixmap(240,240);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setPen(QPen(Qt::black, 4));
    painter.translate(120, 120);

    QPolygonF Polygon;
    switch ((int)_eType){
        case E_TYPE_DEL:
                {
                        QPainterPath path;
                        path.lineTo(100,100);
                        path.moveTo(100,-100);
                        path.lineTo(-100,100);
                        path.moveTo(0,0);
                        path.lineTo(-100,-100);
                        Polygon = path.toFillPolygon();
                }break;
            break;
        case E_TYPE_JUMP:{
                    QPainterPath path;
                    path.addEllipse(-100,-50,200,100);
                    Polygon = path.toFillPolygon();
            }break;
        case E_TYPE_PROCESS: {
                float fDiv = 2.0f;                
                Polygon << QPointF(-100, -100/fDiv) << QPointF(100, -100/fDiv)
                          << QPointF(100, 100/fDiv) << QPointF(-100, 100/fDiv)
                          << QPointF(-100, -100/fDiv);
            }break;
        case E_TYPE_PROCESS_THREAD:{
                float fDiv = 2.0f;
                float fScale = 0.96f;
                Polygon << QPointF(-100*fScale, -100/fDiv*fScale) << QPointF(100*fScale, -100/fDiv*fScale)
                            << QPointF(100*fScale, 100/fDiv*fScale) << QPointF(-100*fScale, 100/fDiv*fScale)
                            << QPointF(-100*fScale, -100/fDiv*fScale);
                painter.drawArc(-125*fScale,-75*fScale,50*fScale,50*fScale,0*16,270*16);
            }break;
        case E_TYPE_LINE:
            Polygon << QPointF(-100, -100) << QPointF(100, 100);
            break;
    }
    painter.drawPolyline(Polygon);
    return pixmap;
}

void MainWindow::CloseTab(const int& index)
{
    if (index == -1) return;
    SView *pView = (SView*)m_pTab->widget(index);
    m_pTab->removeTab(index);
    delete pView;
}

void MainWindow::ChangeTab(const int& index)
{
    if (index == -1) return;
    foreach (QAbstractButton *button,m_buttonGroup->buttons())
        button->setChecked(false);
    SView *pView = (SView*)m_pTab->widget(index);
    int nItem = pView->GetButtonItem();
    if (nItem != E_TYPE_NONE)
        m_buttonGroup->button(nItem)->setChecked(true);
}

void MainWindow::buttonGroupClicked(int _nId)
{
    QList<QAbstractButton *> buttons = m_buttonGroup->buttons();
    bool bChk = false;
    foreach (QAbstractButton *button, buttons)
    {
        if (m_buttonGroup->button(_nId) != button)
            button->setChecked(false);

        if (button->isChecked()) bChk = true;
    }
    SView *pView = (SView*)m_pTab->currentWidget();
    if (pView == 0) return;
    if (bChk == true)
        pView->SelectItem(_nId);
    else
        pView->SelectItem(E_TYPE_NONE);

    if (_nId == E_TYPE_NONE)
        pView->repaint();
}

SComThread *MainWindow::GetIdleProcess()
{
    QTime timeCheck;
    timeCheck.start();
    while(1)
    {
        for (int i =0; i < m_nProcessSize ; i++)
        {
            if (m_pCom[i].isRunning() == false)
            {
                return &m_pCom[i];
            }
        }
        if (timeCheck.elapsed() >= 300000)
        {
            InsertMsg("오류 : 실행 할수 있는 프로세스 여유가 없습니다.");
            Stop();
            return 0;
        }
    }
    return 0;
}

bool MainWindow::Run(int _nStart,QString _strToken,QStringList _strValueList,bool _bThread)
{
    if (_strToken.isEmpty()) return false;
    SComThread *pThread = GetIdleProcess();
    if (pThread == 0) return false;
    pThread->SetData(_nStart,_strToken,_strValueList);
    if (_bThread)
       pThread->start();
    else
        pThread->run();
    return true;
}
