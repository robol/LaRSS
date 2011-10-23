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
    ui->newsTableView->setColumnHidden(4, true); // Description
    ui->newsTableView->setColumnHidden(5, true); // Time
    ui->newsTableView->setColumnHidden(6, true); // Read state
    ui->newsTableView->verticalHeader()->setHidden(true);
    ui->newsTableView->horizontalHeader()->setStretchLastSection(false);
    ui->newsTableView->horizontalHeader()->setResizeMode(2, QHeaderView::Stretch);

    poller = new FeedPoller (this, rssParser, feedModel);
    poller->start();
}

MainWindow::~MainWindow()
{
    db.close();
    delete ui;
}

void MainWindow::do_exit()
{
    poller->stopPolling();
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
    poller->queueWork(index);

    // Set the active filter
    quint64 feed_id;
    if ((feed_id = rssParser->getFeed (index)))
    {
        rssParser->selectActiveFeed(feed_id);
    }

}

void Larss::MainWindow::on_newsTableView_clicked(const QModelIndex &index)
{
    // Get the number of the row, since index.row () is likely to change
    // while we set the read status on the post.
    quint32 row_number = index.row();

    // A row got activated, so open it in the webview.
    QString link = rssParser->getLink(index);
    ui->webView->load(link);

    // And then mark it as read
    rssParser->setReadStatus(index, true);
    ui->newsTableView->selectRow(row_number);
}
