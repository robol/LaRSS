#include "feedmodel.h"
#include <QAbstractItemModel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QStringList>
#include <QLabel>

using namespace Larss;

FeedModel::FeedModel(QSqlDatabase db, QObject *parent) : QAbstractItemModel (parent)
{
    this->db = db;

    // Check that the right tables are present in the db
    if (!db.tables().contains("categories"))
    {
        QSqlQuery query(db);
        query.prepare("CREATE TABLE categories (id INTEGER PRIMARY KEY, name TEXT);");
        if (!query.exec())
            qDebug() << "Error while creating the categories table in the db";
    }

    if (!db.tables().contains("feeds"))
    {
        QSqlQuery query(db);
        query.prepare("CREATE TABLE feeds (id INTEGER PRIMARY KEY, category INTEGER, name TEXT, url TEXT);");
        if (!query.exec())
            qDebug() << "Error while creating the feeds table in the db";
    }
}

FeedModel::~FeedModel()
{
}


QModelIndex
FeedModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.internalId() == 0)
    {
        QSqlQuery query (db);
        query.prepare("SELECT id from categories ORDER by id;");
        if (query.exec())
        {
            if (!query.first())
                return QModelIndex();
            for (int i = 0; i < row; i++)
            {
                if (!query.next ())
                    return QModelIndex();
            }
            return createIndex(row, column, query.value(0).toInt());
        }
        else
            return QModelIndex();
    }
    else
    {
        QSqlQuery query(db);
        query.prepare ("SELECT id from feeds WHERE category=:category ORDER BY id;");
        query.bindValue("category", parent.internalId());
        if (query.exec())
        {
            if (!query.first())
                return QModelIndex();
            else
            {
                for(int i = 0; i < row; i++)
                {
                    if (!query.next())
                        return QModelIndex();
                }
                return createIndex(row, column, query.value(0).toInt() + FEEDMODEL_MAX_CATEGORIES);
            }
        }
        else
            return QModelIndex();
    }
}

bool
FeedModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole)
        return false;

    QSqlQuery query(db);

    if (index.internalId() < FEEDMODEL_MAX_CATEGORIES && index.internalId() != 0)
    {
        // We are trying to modify a category
        query.prepare("UPDATE categories SET name=:value WHERE id=:id;");
        query.bindValue("value", value.toString());
        query.bindValue("id", index.internalId());
    }
    else
    {
        // We are trying to modify a feed
        query.prepare("UPDATE feeds SET name=:value WHERE id=:id;");
        query.bindValue("value", value.toString());
        query.bindValue("id", index.internalId() - FEEDMODEL_MAX_CATEGORIES);
    }

    if (!query.exec())
    {
        qDebug() << "Query failed" << query.lastError() << query.executedQuery();
        return false;
    }
    else
    {
        // Emit the datachanged signal
        dataChanged(index, index);
        return true;
    }
}

Qt::ItemFlags
FeedModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return (Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
}

int
FeedModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
    {
        // Categories count
        QSqlQuery query(db);
        query.prepare ("SELECT id from categories ORDER by id;");
        if (query.exec())
        {
            int row_count = 1;
            if (!query.first())
                return 0;
            else
                while (query.next())
                    row_count++;
            return row_count;
        }
        else
            return 0;
    }
    else
    {
        int category_id = parent.internalId();
        QSqlQuery query(db);
        query.prepare("SELECT id from feeds where category=:category;");
        query.bindValue("category", category_id);
        if (query.exec())
        {
            int row_count = 1;
            if (!query.first())
                return 0;
            else
                while (query.next())
                    row_count++;
            return row_count;
        }
        else
            return 0;
    }
}

bool
FeedModel::addCategory(QString name)
{
    QSqlQuery query(db);
    query.prepare("INSERT INTO categories VALUES (NULL, :name);");
    query.bindValue("name", name);

    bool successful = query.exec();
    if (successful)
        reset();
    return successful;
}

bool
FeedModel::addFeed(QString name, QString url, quint32 category_id)
{
    QSqlQuery query(db);
    query.prepare("INSERT INTO feeds VALUES (NULL, :category, :name, :url);");
    query.bindValue("category", category_id);
    query.bindValue("name", name);
    query.bindValue("url", url);

    bool successful = query.exec();
    if (successful)
        reset();
    else
    {
        qDebug() << "Query failed: " << query.executedQuery() << query.lastError();
    }

    return successful;
}

int
FeedModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant
FeedModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (index.internalId() == 0)
            return QString ("Root");
        if (index.internalId() < FEEDMODEL_MAX_CATEGORIES)
        {
            QSqlQuery query(db);
            query.prepare ("SELECT id, name from categories WHERE id=:category;");
            query.bindValue("category", index.internalId());
            if (query.exec())
            {
                if (!query.first())
                    return QVariant(QVariant::Invalid);
                else
                {
                    return query.value(1).toString();
                }
            }
            else
                return QVariant(QVariant::Invalid);
        }
        else
        {
            QSqlQuery query(db);
            query.prepare ("SELECT id, category, name, url from feeds WHERE id=:feed;");
            query.bindValue("feed", index.internalId() - FEEDMODEL_MAX_CATEGORIES);
            if (query.exec())
            {
                if (query.first())
                    return query.value(2).toString();
                else
                    return QVariant(QVariant::Invalid);
            }
            else
                return QVariant(QVariant::Invalid);
        }
    }
    else
        return QVariant (QVariant::Invalid);
}

QModelIndex
FeedModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
        return QModelIndex ();

    quint32 row;
    quint32 id = child.internalId();

    if (id == 0)
        return QModelIndex();
    else if (id < FEEDMODEL_MAX_CATEGORIES)
    {
        // Get the position of the category
        QSqlQuery query (db);
        query.prepare ("SELECT id from category;");
        if (query.exec ())
        {
            if (query.first ())
                row = 1;
            else
                return QModelIndex ();
            while (query.next ())
            {
                row++;
                if ((quint64) query.value(0).toInt() == id)
                    break;
            }

            return createIndex (row, 1, 0);
        }
        else
            return QModelIndex();
    }
    else
    {
        quint32 category_id;
        // We have a feed here, that actually has a real parent.
        // We need to get the ID of the category
        id -= FEEDMODEL_MAX_CATEGORIES;
        QSqlQuery query (db);
        query.prepare ("SELECT category from feeds WHERE id=:id;");
        query.bindValue("id", id);
        if (query.exec())
        {
            if (!query.first ())
                return QModelIndex();
            else
            {
                category_id = query.value(0).toInt();

                // We need to get the position of the feed in the category
                query.prepare("SELECT id from feeds WHERE category=:category;");
                query.bindValue("category", category_id);
                if (query.exec())
                {
                    row = 1;
                    if (!query.first())
                        return QModelIndex();
                    else
                    {
                        while (query.next())
                            row++;
                        return createIndex(row, 1, category_id);
                    }
                }
                else
                    return QModelIndex();
            }
        }
        else
            return QModelIndex();
    }
}

QVariant
FeedModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // We have a header data only for the first column, horizontal mode.
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal && section == 0)
        return tr("Feed");
    else
        return QVariant ();
}

QString
FeedModel::getUrl(const QModelIndex &index)
{
    quint64 id = index.internalId();
    if (id < FEEDMODEL_MAX_CATEGORIES)
        return "";
    else
    {
        QSqlQuery query(db);
        query.prepare("SELECT url from feeds WHERE id=:id;");
        query.bindValue("id", id - FEEDMODEL_MAX_CATEGORIES);
        if (query.exec())
        {
            if (query.first())
                return query.value(0).toString();
            else
                return "";
        }
        else
            return "";
    }
}

