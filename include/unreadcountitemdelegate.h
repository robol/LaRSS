#ifndef UNREADCOUNTITEMDELEGATE_H
#define UNREADCOUNTITEMDELEGATE_H

#include <QObject>
#include <QStyledItemDelegate>
#include "feedmodel.h"
#include "rssparser.h"

namespace Larss {

    class UnReadCountItemDelegate : public QStyledItemDelegate
    {
        Q_OBJECT
    public:
        explicit UnReadCountItemDelegate(FeedModel *feedModel, RssParser *rssParser, QObject *parent = 0);

    protected:
        void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
        // QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

    signals:

    public slots:

    private:
        FeedModel *feedModel;
        RssParser *rssParser;

    };
}

#endif // UNREADCOUNTITEMDELEGATE_H
