#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>

//#include"calendar.h"
//#include<QDesktopWidget>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow window;
    window.show();

    return a.exec();
}


