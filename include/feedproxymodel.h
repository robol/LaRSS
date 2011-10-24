#ifndef FEEDPROXYMODEL_H
#define FEEDPROXYMODEL_H

#include <QObject>
#include <QProxyModel>
#include "rssparser.h"

namespace Larss {

    class FeedProxyModel : public QProxyModel
    {
        Q_OBJECT
    public:
        explicit FeedProxyModel(QObject *parent = 0);
        QVariant data (const QModelIndex &index, int role) const;
        void setParser (RssParser * parser);

    private:
        RssParser *parser;

    signals:

    public slots:
        void onRssParserDataChanged (QModelIndex index, QModelIndex index2);

    };

}

#endif // FEEDPROXYMODEL_H
