#ifndef FEEDNODE_H
#define FEEDNODE_H

#include <QObject>
#include <QStandardItem>

namespace Larss {

    class FeedNode : public QStandardItem
    {

    public:
        explicit FeedNode(quint64 id, QString name, QString url = "");

        enum ItemType {
            Root = 1001,
            Category = 1002,
            Feed = 1003
        };

        /**
         * @brief Return the type of this element.
         */
        ItemType type ();

        quint64 id();
        QString name();
        QString url();

    private:

        /**
         * @brief The id of the item in the database.
         */
        quint64 nodeId;

        /**
         * @brief The name of the item.
         */
        QString nodeName;

        /**
         * @brief The Url associated with the feed, if any
         * (Category do not have this one).
         */
        QString nodeUrl;

    private:

    };

}

#endif // FEEDNODE_H
