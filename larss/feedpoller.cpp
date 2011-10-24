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
    timer->setInterval(1000);
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

    return false;
}

void
FeedPoller::stopPolling ()
{
    poll_active = false;
    QThread::exit();
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
    // Assume that the string is UTF-8 encoded. This is likely to be
    // true, but I should check it in some way.
    rssContent->insert (nowLoading, QString::fromUtf8(reply->readAll()));

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
        if (query.first())
        {
            links.append(query.value(0).toString());
            while (query.next())
                links.append(query.value(0).toString());
        }

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
            QString content = element.elementsByTagName("content:encoded").item(0).firstChild().nodeValue();

            // Check if this is RSS2 or not
            uint pubDate;
            QString guid;
            QString pubDateTimeContent;
            if (element.elementsByTagName("guid").length() != 0)
            {
                guid = element.elementsByTagName("guid").item(0).firstChild().nodeValue();
                pubDateTimeContent = element.elementsByTagName("pubDate").item(0).firstChild().nodeValue();
                pubDate = pubDateToDateTime(pubDateTimeContent).toTime_t();
            }
            else
            {
                qDebug() << "Rss 1.0";
                guid = link;
                pubDate = QDateTime::currentDateTime().toTime_t();
            }

            if (!links.contains(link))
            {
                // That means that no results were found, so let's insert this one.
                QSqlRecord record = parser->record();
                record.setValue("time", pubDate);
                record.setValue("read", 0);
                record.setValue("title", title);
                record.setValue("link", link);
                record.setValue("description", description);
                record.setValue("content", content);
                record.setValue("feed", nowLoading - FEEDMODEL_MAX_CATEGORIES);

                if (!parser->insertRecord(-1, record))
                    qDebug () << "Error inserting record";
            }
        }

        if (!parser->submitAll())
            qDebug() << "Error submitting new data";
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

QDateTime
FeedPoller::pubDateToDateTime (QString pubDate)
{
    // Parsing the data, take Sun, 12 Oct 2011 15:21:12 GMT
    // pieces[0] is Sun  --- pieces[1] is 12  --- pieces[2] is Oct
    // pieces[3] is 2011 --- pieces[4] is 15:21:12 --- pieces[6] is GMT
    QStringList pieces = pubDate.split(" ");
    QDateTime date;

    int month, year, day;

    // Set the day
    day = pieces.at(1).toInt();

    // Set the month
    if (pieces.at(2) == "Jan")
        month = 1;
    else if (pieces.at(2) == "Feb")
        month = 2;
    else if (pieces.at(2) == "Mar")
        month = 3;
    else if (pieces.at(2) == "Apr")
        month = 4;
    else if (pieces.at(2) == "May")
        month = 5;
    else if (pieces.at(2) == "Jun")
        month = 6;
    else if (pieces.at(2) == "Jul")
        month = 7;
    else if (pieces.at(2) == "Aug")
        month = 8;
    else if (pieces.at(2) == "Sep")
        month = 9;
    else if (pieces.at(2) == "Oct")
        month = 10;
    else if (pieces.at(2) == "Nov")
        month = 11;
    else if (pieces.at(2) == "Dec")
        month = 12;

    // Set the year
    year = pieces.at(3).toInt();

    date.setDate(QDate (year, month, day));

    // Set the hour
    date.setTime(QTime::fromString(pieces[4], "hh:mm:ss"));

    return date;
}
