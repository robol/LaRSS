#ifndef FEEDMODEL_H
#define FEEDMODEL_H

#include <QObject>
#include <QAbstractItemModel>
#include <QSqlDatabase>
#include <QtXml>

namespace Larss {

#define FEEDMODEL_MAX_CATEGORIES 1024

class FeedModel : public QAbstractItemModel {
    Q_OBJECT

public:
    explicit FeedModel(QSqlDatabase db, QObject *parent = 0);

    /**
     * @brief Destructor for the FeedModel
     */
    ~FeedModel();

    /**
     * @brief Get the ModelIndex associated with a given row and column.
     */
    QModelIndex index(int row, int column, const QModelIndex &parent) const;

    /**
     * @brief Get the number of rows in the list.
     */
    int rowCount(const QModelIndex &parent) const;

    /**
     * @brief Get the number of column
     */
    int columnCount(const QModelIndex &parent) const;

    /**
     * @brief Get the data associated to a given node.
     */
    QVariant data(const QModelIndex &index, int role) const;

    /**
     * @brief Get the parent of a given node.
     */
    QModelIndex parent(const QModelIndex &child) const;

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
    bool addFeed (QString name, QString url, quint32 category_id);

signals:

public slots:

private:
    /**
     * @brief Database containing the data of the feeds.
     */
    QSqlDatabase db;


};

}

#endif // FEEDMODEL_H
