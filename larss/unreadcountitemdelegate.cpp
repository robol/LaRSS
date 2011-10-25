#include "unreadcountitemdelegate.h"
#include "feednode.h"
#include <QStyledItemDelegate>
#include <QPainter>

using namespace Larss;

UnReadCountItemDelegate::UnReadCountItemDelegate(FeedModel *feedModel, RssParser *rssParser, QObject *parent) :
    QStyledItemDelegate(parent), feedModel(feedModel), rssParser(rssParser)
{
}

void
UnReadCountItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.isValid())
    {
        FeedNode *node = feedModel->itemFromIndex(index);
        if (node->type() == FeedNode::Feed)
        {
            quint32 unreadPosts = rssParser->getUnreadCount(index);
            if (unreadPosts > 0)
            {
                // Draw highlight if necessary
                if (option.state & QStyle::State_Selected)
                    painter->fillRect(option.rect, option.palette.highlight());

                // Set bold font
                QFont font = option.font;
                font.setBold(true);
                painter->setFont(font);
                painter->drawText(option.rect, option.displayAlignment,
                                  QString (" %1 (%2)").arg(index.data(Qt::DisplayRole).toString()).arg(unreadPosts));

                return;
            }
        }
    }
    QStyledItemDelegate::paint(painter, option, index);
}
