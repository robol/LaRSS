#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    // Create the application and its main_window.
    QApplication larss_application(argc, argv);

    // Set some default values for our application
    larss_application.setApplicationName("Larss");
    larss_application.setOrganizationName("PHC");
    larss_application.setOrganizationDomain("phc.unipi.it");

    // Create the MainWindow object
    Larss::MainWindow main_window;

    // Show the main window
    main_window.show();

    // Wait for the return code and return it
    return larss_application.exec();
}
