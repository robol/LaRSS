#include "rssparser.h"
#include "feednode.h"
#include <QDebug>
#include <QtXml>
#include <QtSql>
#include <QtNetwork>
#include <QtGui>
#include <QtCore>

using namespace Larss;

Larss::RssParser::RssParser(QSqlDatabase db, FeedModel *model, QObject *parent) :
    QSqlTableModel (parent, db)
{
    // Create the database if it does not exists.
    if (!db.tables().contains ("news"))
    {
        qDebug () << "Creating table news that is not in database...";
        QSqlQuery query(db);
        query.prepare("CREATE TABLE news (id INTEGER PRIMARY KEY, feed INTEGER, title TEXT, link TEXT, description TEXT, content TEXT, time INTEGER, read INTEGER);");
        if (!query.exec())
            qDebug () << "Error occurred while creating the database:" << query.lastError();
    }

    // Set init parameters for the QSqlDataTable
    setTable("news");

    // Select manual submit so user cannot modify content directly
    setEditStrategy(QSqlTableModel::OnManualSubmit);
    this->model = model;

    // Make this sorted in different time
    this->sort(6, Qt::DescendingOrder);
    select();
}

QVariant
Larss::RssParser::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal)
        {
            switch (section)
            {
            case 0:
                return tr("ID");
                break;
            case 1:
                return tr("Feed");
                break;
            case 2:
                return tr("Title");
                break;
            case 3:
                return tr("Link");
                break;
            case 4:
                return tr("Description");
                break;
            case 5:
                return tr("Content");
                break;
            case 6:
                return tr("Date");
                break;
            case 7:
                return tr("Read");
            }
        }
    }

    return QVariant (QVariant::Invalid);
}

Larss::RssParser::~RssParser()
{
}

QVariant
Larss::RssParser::data(const QModelIndex &idx, int role) const
{
    if (role == Qt::FontRole)
    {
        // Get default font
        QFont default_font = QSqlTableModel::data(idx, role).toString();

        // Check if this news is read or not
        QSqlRecord record = this->record(idx.row());
        if (record.value("read") == 0)
            default_font.setBold(true);
        return default_font;
    }

    // Manage a nice rendering of Time
    if (role == Qt::DisplayRole && idx.column() == 6)
    {
        return (QDateTime::fromTime_t(QSqlTableModel::data(idx, role).toInt()).toString("dd/MM/yyyy hh:mm"));
    }

    // Call the default implementaton in almost every case
    return QSqlTableModel::data(idx, role);
}

QString
Larss::RssParser::getLink(const QModelIndex &index)
{
    QSqlRecord record = this->record(index.row());
    return record.value("link").toString();
}

void
Larss::RssParser::setReadStatus(const QModelIndex& index, bool read)
{
    QModelIndex read_index = createIndex(index.row(), 7, index.internalPointer());
    setData(read_index, read ? 1 : 0);
    if (!submitAll())
        qDebug() << "Error while setting the read flag";
    model->triggerDataChanged();
}

quint64
Larss::RssParser::getFeed(const QModelIndex &index)
{
    FeedNode *node = model->itemFromIndex (index);
    if (node->type() == FeedNode::Category)
        return 0;
    else
        return node->id();
}

QString
Larss::RssParser::getContent(const QModelIndex &index)
{
    QModelIndex description_index = createIndex(index.row(), 5, index.internalPointer());
    return data(description_index, Qt::DisplayRole).toString();
}

QString
Larss::RssParser::getTitle(const QModelIndex &index)
{
    QModelIndex title_index = createIndex(index.row(), 2, index.internalPointer());
    return data(title_index, Qt::DisplayRole).toString();
}

quint32
Larss::RssParser::getUnreadCount(const QModelIndex &index)
{
    quint64 feedId = getFeed(index);
    QSqlQuery query(db);
    query.prepare ("SELECT id from news WHERE read=0 AND feed=:feed;");
    query.bindValue("feed", feedId);
    if (query.exec())
    {
        quint32 count = 1;
        if (!query.first())
            return 0;
        while (query.next())
            count++;
        return count;
    }
    return 0;
}

void
Larss::RssParser::selectActiveFeed(quint64 feed_id)
{
    // Show only the news from the given feed
    setFilter(QString("feed='%1'").arg(feed_id));
}

int
Larss::RssParser::getNextUnread(const QModelIndex& starting)
{
    int row = starting.row();
    while (row < rowCount(starting.parent()))
    {
        QSqlRecord record = this->record(row);
        if (record.value("read").toInt() == 0)
            return row;
        row++;
    }

    return -1;
}

void
Larss::RssParser::removeNewsForFeed(const QModelIndex& index)
{
    if (!index.isValid())
        return;
    FeedNode *node = model->itemFromIndex(index);

    if (node->type() == FeedNode::Feed)
    {
        int id = node->id();
        QSqlQuery query(db);
        query.prepare("DELETE FROM news WHERE feed=:feed");
        query.bindValue("feed", id);

        if (!query.exec())
            qDebug() << "Error deleting news from feed " << node->name();
    }
}
