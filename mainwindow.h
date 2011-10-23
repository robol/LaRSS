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

private:
    Ui::MainWindow *ui;
    void do_exit();

    QSqlDatabase db;
    FeedModel *feedModel;
    RssParser *rssParser;
    FeedPoller *poller;
};

}

#endif // MAINWINDOW_H
