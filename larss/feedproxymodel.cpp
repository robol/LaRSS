#include "include/feedproxymodel.h"
#include <QFont>
#include <QSqlRecord>

using namespace Larss;

FeedProxyModel::FeedProxyModel(QObject *parent) :
    QProxyModel(parent)
{

}

QVariant
FeedProxyModel::data(const QModelIndex &index, int role) const
{
    // Check if this is  a feed, and in that case check the number of unread items.
    if (index.internalId() > FEEDMODEL_MAX_CATEGORIES)
    {
        if (role == Qt::DisplayRole)
        {
            QString feedName = this->model()->data(index, role).toString();
            quint32 unreadCount = parser->getUnreadCount(index);
            if (unreadCount > 0)
                return QString("%1 (%2)").arg(feedName).arg(unreadCount);
            else
                return feedName;
        }
        else if (role == Qt::FontRole)
        {
            quint32 unreadCount = parser->getUnreadCount(index);
            QFont font = this->model()->data(index, role).toString();
            if (unreadCount > 0)
                font.setBold(true);
            return font;
        }

        return this->model()->data(index, role);
    }
    else
        return this->model()->data(index, role);
}

void
FeedProxyModel::setParser(RssParser *parser)
{
    this->parser = parser;
    parser->connect(parser, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
                    this, SLOT(onRssParserDataChanged(QModelIndex,QModelIndex)));
}

void
FeedProxyModel::onRssParserDataChanged(QModelIndex index, QModelIndex index2)
{
    Q_UNUSED(index);
    Q_UNUSED(index2);
    // We need to get the feed where the news is, so we can emit dataChanged
    // for it. For now we simply propagate the change.
    QModelIndex rootIndex = this->model()->index(1, 1, QModelIndex());
    dataChanged(rootIndex, rootIndex);
}
