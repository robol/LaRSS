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

    // Set the source to an empty string, that means no source
    rssContent = new QHash<quint32, QString>();
    workQueue = new QList<QModelIndex> ();
    this->model = model;
    nowLoading = 0;
    activeItem = 0;

    // Create the QNetworkAccessManager and connect the loaded signal
    // with our handler.
    manager = new QNetworkAccessManager (this);
    manager->connect(manager, SIGNAL(finished(QNetworkReply*)),
                     this, SLOT(networkManagerReplyFinished(QNetworkReply*)));

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
    db.close();
    delete manager;
    delete rssContent;
    delete workQueue;
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

void
Larss::RssParser::loadItem(const QModelIndex &index)
{
    if (index.internalId() < FEEDMODEL_MAX_CATEGORIES)
        return;

    if (nowLoading != 0)
    {
        // We should queue this work and wait till there is
        // the networkmanager free to do it.
        workQueue->append(index);
    }
    else
    {
        nowLoading = index.internalId();
        manager->get(QNetworkRequest(QUrl(model->getUrl(index))));
    }
}

void
Larss::RssParser::networkManagerReplyFinished(QNetworkReply *reply)
{
    rssContent->insert (nowLoading, reply->readAll());

    // Now update the database with the new data obtained.
    QDomDocument doc;
    if (doc.setContent(rssContent->value(nowLoading)))
    {
        QDomElement doc_el = doc.documentElement();
        QDomNodeList items = doc_el.elementsByTagName("item");

        for (quint32 i = 0; i < items.length(); i++)
        {
            // Get the i-th news
            QDomNode item = items.item(i);
            QDomElement element = item.toElement();

            // Get the data in it
            QString link = element.elementsByTagName("link").item(0).firstChild().nodeValue();
            QString title = element.elementsByTagName("title").item(0).firstChild().nodeValue();
            QString description = element.elementsByTagName("description").item(0).firstChild().nodeValue();

            // We should enable this for RSS 2.0
            // QString guid = element.elementsByTagName("guid").item(0).firstChild().nodeValue();
            // QString pubDate = element.elementsByTagName("pubDate").item(0).firstChild().nodeValue();

            // Try to catch other news_feed with the same link
            QSqlQuery query(db);
            query.prepare ("SELECT id from news WHERE feed=:feed AND link=:link;");
            query.bindValue("feed", nowLoading);
            query.bindValue("link", link);
            if (query.exec())
            {
                if (!query.first())
                {
                    // That means that no results were found, so let's insert this one.
                    QSqlRecord record = this->record();
                    record.setValue("time", 0);
                    record.setValue("read", 0);
                    record.setValue("title", title);
                    record.setValue("link", link);
                    record.setValue("description", description);
                    record.setValue("feed", nowLoading - FEEDMODEL_MAX_CATEGORIES);

                    if (!insertRecord(-1, record))
                        qDebug () << "Error inserting record";
                }
            }
        }
    }
    else
        qDebug () << "Error parsing the document";


    if (activeItem == nowLoading)
    {
        // We shall call something like dataChanged in here
    }
    nowLoading = 0;

    // Check if there is work in the queue
    if (!workQueue->isEmpty())
    {
        QModelIndex next_item = workQueue->takeFirst();
        nowLoading = next_item.internalId();
        manager->get(QNetworkRequest(QUrl(model->getUrl(next_item))));
    }
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
