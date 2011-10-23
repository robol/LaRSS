#include "rssparser.h"
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
        query.prepare("CREATE TABLE news (id INTEGER PRIMARY KEY, feed INTEGER, title TEXT, link TEXT, description TEXT, time INTEGER, read INTEGER);");
        if (!query.exec())
            qDebug () << "Error occurred while creating the database:" << query.lastError();
    }

    // Set init parameters for the QSqlDataTable
    setTable("news");

    // Select manual submit so user cannot modify content directly
    setEditStrategy(QSqlTableModel::OnRowChange);
    this->model = model;
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
                return tr("Time");
                break;
            case 6:
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
    QModelIndex read_index = createIndex(index.row(), 6, index.internalPointer());
    setData(read_index, read ? 1 : 0);
}

quint64
Larss::RssParser::getFeed(const QModelIndex &index)
{
    quint64 id = index.internalId();
    if (id < FEEDMODEL_MAX_CATEGORIES)
        return 0;
    else
        return (id - FEEDMODEL_MAX_CATEGORIES);
}

void
Larss::RssParser::selectActiveFeed(quint64 feed_id)
{
    // Show only the news from the given feed
    setFilter(QString("feed='%1'").arg(feed_id));
}
