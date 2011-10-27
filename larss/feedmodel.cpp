#include "feedmodel.h"
#include "feednode.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QStringList>
#include <QLabel>

using namespace Larss;

FeedModel::FeedModel(QSqlDatabase db, QObject *parent) : QStandardItemModel (parent)
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

    select();
}

FeedModel::~FeedModel()
{
}

void
FeedModel::select()
{
    // Clear the content of the model
    clear();

    // Get the root of the tree
    QStandardItem *root = invisibleRootItem();

    // Set the fake rootNode
    rootNode = new FeedNode (0, "");

    QSqlQuery query(db);
    query.prepare("SELECT id, name from categories;");
    if (query.exec() && query.first())
    {
        do {
            // Insert the new category in the tree
            FeedNode* node = new FeedNode(query.value(0).toInt(), query.value(1).toString());
            root->appendRow(node);

            // Find the feeds associated with this category and add the as childs.
            QSqlQuery feedQuery(db);
            feedQuery.prepare("SELECT id, name, url FROM feeds WHERE category=:category");
            feedQuery.bindValue("category", node->id());

            if (feedQuery.exec() && feedQuery.first())
            {
                do
                {
                    FeedNode *feedNode = new FeedNode(feedQuery.value(0).toInt(),
                                                      feedQuery.value(1).toString(),
                                                      feedQuery.value(2).toString());
                    node->appendRow(feedNode);
                } while (feedQuery.next());
            }
        } while (query.next());
    }
}

bool
FeedModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    // Get the involved node
    FeedNode *node = itemFromIndex(index);

    // First we need to update the data in the database.
    QSqlQuery query(db);
    if (node->type() == FeedNode::Category)
    {
        query.prepare("UPDATE categories SET name=:value WHERE id=:id;");
        query.bindValue("value", value);
        query.bindValue("id", node->id());
    }
    else
    {
        query.prepare("UPDATE feeds SET name=:value WHERE id=:id;");
        query.bindValue("value", value);
        query.bindValue("id", node->id());
    }

    // Check if the query has gone well
    if (query.exec())
        return QStandardItemModel::setData(index, value, role);
    else
    {
        qDebug() << query.executedQuery() << query.lastError();
        return false;
    }
}

bool
FeedModel::removeElements(int row, int count, const QModelIndex &parent)
{
    for(int i = 0; i < count; i++)
    {
        if (!removeElement(row, parent))
            return false;
    }
    return true;
}

bool
FeedModel::removeElement(int row, const QModelIndex &parent)
{
    if (parent.isValid())
    {
        // This means that this is a feed.
        QModelIndex index = parent.child(row, 0);
        FeedNode *node = itemFromIndex(index);

        QSqlQuery query(db);
        query.prepare ("DELETE FROM feeds WHERE id=:feed;");
        query.bindValue("feed", node->id());

        if (!query.exec())
        {
            qDebug() << "Error removing a row";
            return false;
        }
        else
            QStandardItemModel::removeRow(row, parent);
    }
    else
    {
        // This means that this is a feed.
        QModelIndex index = this->index(row, 0, parent);
        FeedNode *node = itemFromIndex(index);
        QSqlQuery query(db);
        query.prepare ("DELETE FROM categories WHERE id=:category");
        query.bindValue("category", node->id());

        if (!query.exec())
        {
            qDebug() << "Error removing a row";
            return false;
        }
        else
        {
            QStandardItemModel::removeRow(row, parent);
        }
    }

    return true;
}



bool
FeedModel::addCategory(QString name)
{
    // First push the data in the database.
    QSqlQuery query(db);
    query.prepare("INSERT INTO categories VALUES (NULL, :name);");
    query.bindValue("name", name);

    // If the insertion works then update the QStandardItemModel
    bool successful = query.exec();
    if (successful)
    {
        FeedNode *node = new FeedNode (query.lastInsertId().toInt(), name);
        QStandardItem *root = invisibleRootItem();
        root->appendRow(node);
    }
    return successful;
}

bool
FeedModel::addFeed(QString name, QString url, FeedNode* categoryNode)
{
    QSqlQuery query(db);
    query.prepare("INSERT INTO feeds VALUES (NULL, :category, :name, :url);");
    query.bindValue("category", categoryNode->id());
    query.bindValue("name", name);
    query.bindValue("url", url);

    bool successful = query.exec();
    if (successful)
    {
        FeedNode *node = new FeedNode (query.lastInsertId().toInt(),
                                       name, url);
        categoryNode->appendRow(node);
    }
    else
    {
        qDebug() << "Query failed: " << query.executedQuery() << query.lastError();
    }

    return successful;
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
    FeedNode *node = itemFromIndex(index);
    return node->url();
}

FeedNode*
FeedModel::itemFromIndex(const QModelIndex &index)
{
    if (index.isValid())
        return (FeedNode*) QStandardItemModel::itemFromIndex(index);
    else
        return rootNode;
}

void
FeedModel::triggerDataChanged()
{
    dataChanged(index(1, 1, QModelIndex()),
                index(1, 1, QModelIndex()));
}

