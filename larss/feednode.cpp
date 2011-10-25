#include "include/feednode.h"
#include <QDebug>

using namespace Larss;

FeedNode::FeedNode(quint64 id, QString name, QString url) :
    nodeId(id), nodeName(name), nodeUrl(url)
{
    setText (name);
}

FeedNode::ItemType
FeedNode::type()
{
    if (nodeId == 0)
        return Root;
    if (nodeUrl != "")
        return Feed;
    else
        return Category;
}

quint64
FeedNode::id()
{
    return nodeId;
}

QString
FeedNode::name()
{
    return nodeName;
}

QString
FeedNode::url()
{
    return nodeUrl;
}
