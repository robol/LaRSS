#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    // Set some default values for our application
    QCoreApplication::setApplicationName("Larss");
    QCoreApplication::setOrganizationName("PHC");
    QCoreApplication::setOrganizationDomain("phc.unipi.it");

    // Create the application and its main_window.
    QApplication larss_application(argc, argv);
    Larss::MainWindow main_window;

    // Show the main window
    main_window.show();

    // Wait for the return code and return it
    return larss_application.exec();
}
