#include "tagtypesortfilterproxymodel.h"

#include "taglisttablemodel.h"
#include "tag.h"
#include "util/util.h"


TagTypeSortFilterProxyModel::TagTypeSortFilterProxyModel(QObject *parent) :
    QSortFilterProxyModel(parent)
{

}

void TagTypeSortFilterProxyModel::setFilterTagTypeCompatibleWithTagSocketType(TagSocket::Type type)
{
    tagSocketType_ = type;
    beginResetModel();
    endResetModel();
}

bool TagTypeSortFilterProxyModel::filterAcceptsRow(int sourceRow,
                                                   const QModelIndex &sourceParent) const
{
    if(!tagSocketType_.has_value())
        return true;

    QModelIndex index = sourceModel()->index(sourceRow, TagListTableModel::eType, sourceParent);

    auto tagType = Tag::typeFromString(sourceModel()->data(index).toString());

    return util::isTagCompatibleWithTagSocket(tagType, tagSocketType_.value());
}
