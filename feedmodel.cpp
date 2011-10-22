#include "feedmodel.h"
#include <QAbstractItemModel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QStringList>

using namespace Larss;

#define FEEDMODEL_MAX_CATEGORIES 1024

FeedModel::FeedModel(QObject *parent) : QAbstractItemModel (parent)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("/home/leonardo/larss.db");
    db.open();
}

FeedModel::~FeedModel()
{
    db.close();
}


QModelIndex
FeedModel::index(int row, int column, const QModelIndex &parent) const
{
    qDebug () << "Called index for row "<< row << " and column " << column;
    if (parent.internalId() == 0)
    {
        qDebug () << "That is a category";
        QSqlQuery query (db);
        query.prepare("SELECT id from categories;");
        if (query.exec())
        {
            if (!query.first())
                return QModelIndex();
            for (int i = 1; i < row; i++)
            {
                if (!query.next ())
                    return QModelIndex();
            }

            qDebug () << "Creating index with id " << query.value(0).toInt();
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
                for(int i = 1; i < row; i++)
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

int
FeedModel::rowCount(const QModelIndex &parent) const
{
    qDebug() << "Called rowCount for parent with index " << parent.internalId();
    if (!parent.isValid())
    {
        // Categories count
        QSqlQuery query(db);
        query.prepare ("SELECT id from categories;");
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
            qDebug () << "Returning " << row_count << " childs for " << " category " << parent.internalId();
            return row_count;
        }
        else
            return 0;
    }
}

int
FeedModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QVariant
FeedModel::data(const QModelIndex &index, int role) const
{
    qDebug () << "Called data for index" << index.internalId();
    if (role == Qt::DisplayRole)
    {
        if (index.internalId() == 0)
            return QString ("Root");
        if (index.internalId() < FEEDMODEL_MAX_CATEGORIES)
        {
            QSqlQuery query(db);
            query.prepare ("SELECT name from categories WHERE id=:category;");
            query.bindValue("category", index.internalId());
            if (query.exec())
            {
                if (!query.first())
                    return QVariant(QVariant::Invalid);
                else
                    return query.value(0).toString();
            }
            else
                return QVariant(QVariant::Invalid);
        }
        else
        {
            QSqlQuery query(db);
            query.prepare ("SELECT name from feeds WHERE id=:feed;");
            query.bindValue("feed", index.internalId() - FEEDMODEL_MAX_CATEGORIES);
            if (query.exec())
            {
                if (query.first())
                    return query.value(0).toString();
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
        // We have a feed here, that actually has a real parent.
        // We need to get the ID of the category
        id -= FEEDMODEL_MAX_CATEGORIES;
        QSqlQuery query (db);
        query.prepare ("SELECT category from feeds WHERE id=:id;");
        query.bindValue("id", id);
        if (query.exec())
        {
            query.first ();
            qDebug () << "Parent of " << id << " is " << query.boundValue(0);
            return createIndex (row, 1, query.boundValue(0).toInt());
        }
        else
            return QModelIndex();
    }
}

