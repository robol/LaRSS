#ifndef FEEDPOLLER_H
#define FEEDPOLLER_H

#include <QObject>
#include <QThread>
#include <QtSql>
#include <QtNetwork>
#include "rssparser.h"
#include "feedmodel.h"

namespace Larss {

    class FeedPoller : public QThread
    {
        Q_OBJECT
    public:
        explicit FeedPoller(QObject *parent, RssParser* parser, FeedModel *model);
        void queueWork (const QModelIndex& index);
        void stopPolling ();

    signals:

    public slots:
        void networkManagerReplyFinished(QNetworkReply* reply);
        bool poll();

    private:
        /**
         * @brief The parser of this instance of Larss.
         */
        RssParser *parser;

        /**
         * @brief The FeedModel of this instance of Larss.
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
         * @brief Queue of item that needs to be refreshed.
         */
        QList<QModelIndex> *workQueue;

        /**
         * @brief If the thread is requested to polling or not.
         */
        bool poll_active;

        /**
         * @brief Real work of the thread.
         */
        void run();

        QDateTime pubDateToDateTime (QString pubDate);

    };
}

#endif // FEEDPOLLER_H
