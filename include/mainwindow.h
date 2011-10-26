#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "rssparser.h"
#include "feedmodel.h"
#include "feedpoller.h"
#include <QSqlDatabase>

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

public slots:
    /**
     * @brief Callback for the start of an update of a feed.
     */
    void loadingFeedStart (QString feedName);

    /**
     * @brief Load a feed in the webview.
     */
    void loadFeed (const QModelIndex& index);

protected:
    bool eventFilter (QObject *object, QEvent *event);
    void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;
    void do_exit();

    QSqlDatabase db;
    FeedModel *feedModel;
    RssParser *rssParser;
    FeedPoller *poller;

    /**
     * @brief A string containing the url to load
     * for the loaded in the webview.
     */
    QString loadedNews;
};

}

#endif // MAINWINDOW_H
