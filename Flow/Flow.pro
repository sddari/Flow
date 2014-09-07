QT += widgets
QT += network
QT += core

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    sview.cpp \    
    sprocssrun.cpp \
    sprocesscommgr.cpp \
    sintercommgr.cpp \
    sdatamgr.cpp \
    scommand.cpp \
    sdllcommgr.cpp \
    scomthread.cpp \
    slibmgr.cpp \
    ssetupdlg.cpp


HEADERS += \
    mainwindow.h \
    Define.h \
    sview.h \    
    sprocssrun.h \
    sprocesscommgr.h \
    sintercommgr.h \
    sdatamgr.h \
    scommand.h \
    sdllcommgr.h \
    scomthread.h \
    slibmgr.h \
    ssetupdlg.h

LIBS += Dll.dll
