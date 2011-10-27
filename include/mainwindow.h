#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "rssparser.h"
#include "feedmodel.h"
#include "feedpoller.h"
#include <QSqlDatabase>
#include <QtGui>

namespace Ui {
    class MainWindow;
}

namespace Larss {

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionExit_activated();

    void on_feedTreeView_clicked(const QModelIndex &index);

    void on_newsTableView_clicked(const QModelIndex &index);

    void on_newsTableView_activated(const QModelIndex &index);

    void on_actionAdd_Feed_triggered();

    void on_actionAdd_Category_triggered();

    void on_actionNext_unread_news_triggered();

    void on_actionUpdate_this_feed_triggered();

    void on_feedTreeView_customContextMenuRequested(const QPoint &pos);

    void removeSelectedFeed ();

    void removeSelectedCategory ();

public slots:
    /**
     * @brief Callback for the start of an update of a feed.
     */
    void loadingFeedStart (QString feedName);

    /**
     * @brief Load a feed in the webview.
     */
    void loadFeed (const QModelIndex& index);

    /**
     * @brief Show a notification about new loaded elements.
     */
    void showNewElement (QString title, QString body);

protected:
    bool eventFilter (QObject *object, QEvent *event);
    void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;
    void do_exit();

    /**
     * @brief The database used for
     * storing all the feed-related data.
     */
    QSqlDatabase db;

    /**
     * The model describing feeds and categories.
     */
    FeedModel *feedModel;

    /**
     * @brief The model describing the news.
     */
    RssParser *rssParser;

    /**
     * @brief The poller for the news.
     */
    FeedPoller *poller;

    /**
     * @brief A string containing the url to load
     * for the loaded in the webview.
     */
    QString loadedNews;

    /**
     * @brief System tray icon used to show the notifications.
     */
    QSystemTrayIcon *systrayIcon;
};

}

#endif // MAINWINDOW_H
