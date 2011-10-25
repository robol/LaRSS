#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "editfeeddialog.h"
#include "editcategorydialog.h"
#include "unreadcountitemdelegate.h"
#include <QDebug>
#include <QtGui>

using namespace Larss;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Open the database, and create the directories for the data
    // storage if they are not yet present in the filesystem.
    QString location = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    if (!QFile::exists(location))
        QDir().mkpath(location);
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(location + "/larss.db");
    db.open();

    // Load feedModel that will wrap the SQLite database
    feedModel = new FeedModel(db, this);
    rssParser = new RssParser(db, feedModel, this);

    // Create the delegate that counts unread posts.
    ui->feedTreeView->setModel(feedModel);
    ui->feedTreeView->setItemDelegate (new UnReadCountItemDelegate(feedModel, rssParser));

    // Load the RSSParser, hiding the unnecessary columns
    ui->newsTableView->setModel(rssParser);
    ui->newsTableView->setColumnHidden(0, true); // ID
    ui->newsTableView->setColumnHidden(1, true); // Feed ID
    ui->newsTableView->setColumnHidden(3, true); // Link
    ui->newsTableView->setColumnHidden(4, true); // Description
    ui->newsTableView->setColumnHidden(5, true); // Content
    ui->newsTableView->setColumnHidden(7, true); // Read state
    ui->newsTableView->horizontalHeader()->setResizeMode(2, QHeaderView::Stretch);
    ui->newsTableView->horizontalHeader()->setResizeMode(6, QHeaderView::ResizeToContents);

    // Show nothing for now.
    rssParser->setFilter("1 = 0");

    poller = new FeedPoller (this, rssParser, feedModel);
    poller->connect(poller, SIGNAL(startLoadingFeed(QString)), this,
                    SLOT(loadingFeedStart(QString)));
    poller->start();

    // Install event filter to handle particular events.
    ui->webViewTitleLabel->installEventFilter(this);
    loadedNews = "";

    // Give the window the right size, if present in the
    // settings.
    QSettings settings;
    if (settings.contains("width") && settings.contains("height"))
    {
        resize (settings.value("width").toInt(),
                settings.value("height").toInt());
    }

    // Expand all the categories. This is will be like this until we do
    // not implement a decent method to store open/close categories on close.
    QStandardItem *rootItem = feedModel->invisibleRootItem();
    for(qint32 i = 0; i < rootItem->rowCount(); i++)
    {
        QModelIndex index = rootItem->child(i, 0)->index();
        ui->feedTreeView->expand(index);
    }
}

MainWindow::~MainWindow()
{
    db.close();
    delete ui;
}

void
MainWindow::loadingFeedStart(QString feedName)
{
    ui->statusBar->showMessage(tr("Updating feed '%1'...").arg(feedName), 2000);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    do_exit();
}

void MainWindow::do_exit()
{
    // Kill the poller
    poller->stopPolling();
    poller->wait();

    // Save window size
    QSettings settings;
    settings.setValue("width",  width());
    settings.setValue("height", height());

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

        // Reset the title
        ui->webViewTitleLabel->setText("");
    }


}

void Larss::MainWindow::on_newsTableView_clicked(const QModelIndex &index)
{
    // A row got activated, so open it in the webview.
    loadFeed(index);
}

void Larss::MainWindow::on_newsTableView_activated(const QModelIndex &index)
{
    on_newsTableView_clicked(index);
}

void Larss::MainWindow::on_actionAdd_Feed_triggered()
{
    Larss::EditFeedDialog dialog(this, feedModel);
    if (dialog.exec() == QDialog::Accepted)
    {
        feedModel->addFeed(dialog.getFeedName(),
                           dialog.getFeedUrl(), dialog.getCategory());
    }
}

void Larss::MainWindow::on_actionAdd_Category_triggered()
{
    Larss::EditCategoryDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted)
    {
        feedModel->addCategory(dialog.getCategoryName());
    }
}

bool Larss::MainWindow::eventFilter(QObject *object, QEvent *event)
{
    if (object == ui->webViewTitleLabel)
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() ==  Qt::LeftButton && loadedNews != "")
            {
                ui->webView->load(QUrl(loadedNews));
            }
            return true;
        }
        else
            return false;
    }
    else
        return QMainWindow::eventFilter(object, event);

}

void Larss::MainWindow::loadFeed(const QModelIndex& index)
{
    quint64 rowNumber = index.row();
    loadedNews = rssParser->getLink(index);
    ui->webView->setHtml(rssParser->getContent(index));

    // Select the right title
    ui->webViewTitleLabel->setText(QString("<b>%1</b>").arg(rssParser->getTitle (index)));

    // And then mark it as read
    rssParser->setReadStatus(index, true);
    ui->newsTableView->selectRow(rowNumber);
}
