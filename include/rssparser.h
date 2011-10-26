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
         * @brief Function that tells the views that use this model
         * what to display in the title of the columns.
         */
        QVariant headerData (int section, Qt::Orientation orientation, int role) const;

        /**
         * @brief Get the link associated with a given ModelIndex.
         */
        QString getLink (const QModelIndex& index);

        /**
         * @brief Get the id of the feed pointed by the QModelIndex or
         * 0 if there is no feed pointed (i.e. is a category or
         * the root of the tree).
         */
        quint64 getFeed (const QModelIndex& index);

        /**
         * @brief Get the content associated with the feed.
         */
        QString getContent (const QModelIndex& index);

        /**
         * @brief Get the title of a news pointed by index
         */
        QString getTitle (const QModelIndex& index);

        /**
         * @brief Get the number of unread post for the feed pointed by
         * the given QModelIndex.
         */
        quint32 getUnreadCount (const QModelIndex& index);

        /**
         * @brief Set the read status on a news.
         */
        void setReadStatus (const QModelIndex& index, bool read);

        /**
         * @brief Reimplement data to make unread post bold.
         */
        QVariant data(const QModelIndex &idx, int role) const;

        /**
         * @brief Set the active category to display.
         */
        void selectActiveFeed (quint64 feed_id);

        /**
         * @brief Get the next unread element.
         */
        int getNextUnread (const QModelIndex& index);

        /**
         * @brief Database where all the news will be loaded and saved.
         */
        QSqlDatabase db;

    signals:

    public slots:

    private:

        /**
         * @brief The FeedModel
         */
        FeedModel *model;
    };

}

#endif // RSSPARSER_H
