#ifndef RSSPARSER_H
#define RSSPARSER_H

#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QList>
#include <QSqlTableModel>
#include "feedmodel.h"

namespace Larss {

    class RssParser : public QSqlTableModel
    {
        Q_OBJECT
    public:
        /**
         * @brief RssParser constructor.
         */
        explicit RssParser(QSqlDatabase db, FeedModel * model = NULL, QObject *parent = 0);

        /**
         * @brief Default destructor.
         */
        ~RssParser();

        /**
         * @brief Reload the feed from a given NewsFeed.
         */
        void loadItem (const QModelIndex& index);

        /**
         * @brief Function that tells the views that use this model
         * what to display in the title of the columns.
         */
        QVariant headerData (int section, Qt::Orientation orientation, int role) const;

    signals:

    public slots:
        void networkManagerReplyFinished (QNetworkReply* reply);

    private:
        /**
         * @brief Database where all the news will be loaded and saved.
         */
        QSqlDatabase db;

        /**
         * @brief The url of the current rss feed.
         */
        FeedModel *model;

        /**
         * @brief The content of the rss loaded from the various
         * items in the feedmodel.
         */
        QHash<quint32, QString> *rssContent;

        /**
         * @brief The NetworkAccessManager that will be used to retrieve
         * the data from sourceUrl.
         */
        QNetworkAccessManager *manager;

        /**
         * @brief An index of the RSS that is currently being loaded, or 0
         * if there is nothing in the queue.
         */
        quint32 nowLoading;

        /**
         * @brie A pointer to the active news feed.
         */
        quint32 activeItem;

        /**
         * @brief Queue of item that needs to be refreshed.
         */
        QList<QModelIndex> *workQueue;
    };

}

#endif // RSSPARSER_H
