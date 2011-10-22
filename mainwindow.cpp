#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "feedmodel.h"

using namespace Larss;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    FeedModel *model = new FeedModel (this);
    ui->feedTreeView->setModel(model);
    ui->feedTreeView->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::do_exit()
{
    // Some cleanup will be need here in the future
    QApplication::exit();
}

void MainWindow::on_actionExit_activated()
{
    // Exit the application
    do_exit();
}
