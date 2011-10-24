#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication lars_application(argc, argv);
    Larss::MainWindow main_window;
    main_window.show();
    return lars_application.exec();
}
