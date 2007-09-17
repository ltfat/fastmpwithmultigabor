#include <QApplication>

#include "main_window.h"
#include "qdebugstream.h"


int main(int argc, char *argv[])
{
    QApplication application(argc, argv); 
    MainWindow* mainWin = new MainWindow();
    QDebugStream cout(std::cout, mainWin);
    QDebugStream cerr(std::cerr, mainWin);
    mainWin->show();
    return application.exec();
}


