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
        void startLoadingFeed (QString name);
        void finishedLoadingFeed (QString name);
        void newElementsNotification (QString title, QString body);

    public slots:
        void networkManagerReplyFinished(QNetworkReply* reply);
        bool poll();
        void queueAll ();

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

        /**
         * @brief Convert the content of the pubDate tag
         * into a QDateTime object.
         */
        QDateTime pubDateToDateTime (QString pubDate);

        /**
         * @brief The time that the last bubble has been shown.
         */
        QDateTime lastShownBubble;

        /**
         * @brief True if the last bubble shown was of the type
         * "Many news to read, not listing them all".
         */
        bool floodedBubbleQueue;

    };
}

#endif // FEEDPOLLER_H
