#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QtGui>

using namespace Larss;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Open the database
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("/home/leonardo/larss.db");
    db.open();

    // Load feedModel that will wrap the SQLite database
    feedModel = new FeedModel(db, this);
    ui->feedTreeView->setModel(feedModel);

    // Load the RSSParser, hiding the unnecessary columns
    rssParser = new RssParser(db, feedModel, this);
    ui->newsTableView->setModel(rssParser);
    ui->newsTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->newsTableView->setColumnHidden(0, true); // ID
    ui->newsTableView->setColumnHidden(1, true); // Feed ID
    ui->newsTableView->setColumnHidden(3, true); // Link
    ui->newsTableView->setColumnHidden(6, true); // Read state

}

MainWindow::~MainWindow()
{
    db.close();
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

void Larss::MainWindow::on_feedTreeView_clicked(const QModelIndex &index)
{
    // Trigger refresh of selected item
    rssParser->loadItem(index);
}
