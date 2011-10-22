#ifndef FEEDMODEL_H
#define FEEDMODEL_H

#include <QObject>
#include <QAbstractItemModel>
#include <QSqlDatabase>

namespace Larss {

class FeedModel : public QAbstractItemModel {
    Q_OBJECT

public:
    explicit FeedModel(QObject *parent = 0);
    ~FeedModel();
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QModelIndex parent(const QModelIndex &child) const;

signals:

public slots:

private:
    QSqlDatabase db;


};

}

#endif // FEEDMODEL_H
