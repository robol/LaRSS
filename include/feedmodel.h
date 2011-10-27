#ifndef FEEDMODEL_H
#define FEEDMODEL_H

#include <QObject>
#include <QStandardItemModel>
#include <QSqlDatabase>
#include <QtXml>
#include "feednode.h"

namespace Larss {

#define FEEDMODEL_MAX_CATEGORIES 1024

class FeedModel : public QStandardItemModel {
    Q_OBJECT

public:
    explicit FeedModel(QSqlDatabase db, QObject *parent = 0);

    /**
     * @brief Destructor for the FeedModel
     */
    ~FeedModel();

    /**
     * @brief Return the data to be inserted in the column header of the treeview.
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    /**
     * @brief Return the URL associated with a given ModelIndex
     */
    QString getUrl (const QModelIndex& index);

    /**
     * @brief Method used to change the data in the database
     */
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    /**
     * @brief Add a category.
     */
    bool addCategory (QString name);

    /**
     * @brief Add a new feed in the specified category.
     */
    bool addFeed (QString name, QString url, FeedNode* categoryNode);

    /**
     * @brief Remove rows from the treeview
     */
    bool removeElements (int row, int count, const QModelIndex &parent = QModelIndex());

    /**
     * @brief Remove a single row from the treeview.
     */
    bool removeElement (int row, const QModelIndex &parent = QModelIndex());

    /**
     * @brief Select data from the database.
     */
    void select();

    /**
     * @brief Call dataChanged
     */
    void triggerDataChanged();

    FeedNode * itemFromIndex (const QModelIndex& index);

signals:

public slots:

private:
    /**
     * @brief Database containing the data of the feeds.
     */
    QSqlDatabase db;

    FeedNode *rootNode;


};

}

#endif // FEEDMODEL_H
