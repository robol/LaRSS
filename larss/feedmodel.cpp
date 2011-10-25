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
    return QStandardItemModel::setData(index, value, role);
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

