#include "feedpoller.h"
#include <QtXml>
#include "feedmodel.h"

using namespace Larss;

FeedPoller::FeedPoller(QObject *parent, RssParser *parser, FeedModel *model) :
    QThread(parent)
{
    this->parser = parser;
    this->model = model;

    workQueue = new QList<QModelIndex> ();
    nowLoading = 0;

    rssContent = new QHash<quint32, QString>();
    poll_active = true;

    // Create the QNetworkAccessManager and connect the loaded signal
    // with our handler.
    manager = new QNetworkAccessManager ();
    manager->connect(manager, SIGNAL(finished(QNetworkReply*)),
                     this, SLOT(networkManagerReplyFinished(QNetworkReply*)));
}

void
FeedPoller::run()
{

    // Create the timer that will call the function every second.
    QTimer *timer = new QTimer ();
    timer->setInterval(800);
    timer->connect(timer, SIGNAL(timeout()),
                   this, SLOT(poll()));
    timer->start();

    QThread::exec();
}

bool
FeedPoller::poll()
{
    // Poll indefinitely until we are requested to exit.
    if (nowLoading == 0)
    {
        if (workQueue->isEmpty())
            return false;
        else
        {
            QModelIndex next_item = workQueue->takeFirst();
            nowLoading = next_item.internalId();
            manager->get(QNetworkRequest(QUrl(model->getUrl(next_item))));
            return true;
        }
    }
}

void
FeedPoller::stopPolling ()
{
    poll_active = false;
}

void
FeedPoller::queueWork(const QModelIndex &index)
{
    if (index.internalId() < FEEDMODEL_MAX_CATEGORIES)
        return;
    workQueue->append(index);
}

void
FeedPoller::networkManagerReplyFinished(QNetworkReply *reply)
{
    rssContent->insert (nowLoading, reply->readAll());

    // Now update the database with the new data obtained.
    QDomDocument doc;
    if (doc.setContent(rssContent->value(nowLoading)))
    {
        // Try to catch other news_feed with the same link, so preload all of them.
        QSqlQuery query(parser->db);
        query.prepare ("SELECT link from news WHERE feed=:feed");
        query.bindValue("feed", nowLoading - FEEDMODEL_MAX_CATEGORIES);
        if (!query.exec ())
            return;
        QStringList links;
        query.first();
        do
        {
            links.append(query.value(0).toString());
        } while (query.next());

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

            if (!links.contains(link))
            {
                // That means that no results were found, so let's insert this one.
                QSqlRecord record = parser->record();
                record.setValue("time", 0);
                record.setValue("read", 0);
                record.setValue("title", title);
                record.setValue("link", link);
                record.setValue("description", description);
                record.setValue("feed", nowLoading - FEEDMODEL_MAX_CATEGORIES);

                if (!parser->insertRecord(-1, record))
                    qDebug () << "Error inserting record";
            }
        }
    }
    else
        qDebug () << "Error parsing the document";

    nowLoading = 0;
    return;

    // Check if there is work in the queue
    if (!workQueue->isEmpty())
    {
        QModelIndex next_item = workQueue->takeFirst();
        nowLoading = next_item.internalId();
        manager->get(QNetworkRequest(QUrl(model->getUrl(next_item))));
    }
}
