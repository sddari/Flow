
#include <QApplication>
#include <QTextCodec>

#include "mainwindow.h"
MainWindow *g_MainWindow;

int main(int argv, char *args[])
{
    QApplication app(argv, args);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("eucKR"));
    MainWindow mainWindow;
    g_MainWindow = &mainWindow;
    mainWindow.show();    
    return app.exec();
}
