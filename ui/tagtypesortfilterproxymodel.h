#ifndef TAGTYPESORTFILTERPROXYMODEL_H
#define TAGTYPESORTFILTERPROXYMODEL_H

#include <optional>

#include <QSortFilterProxyModel>

#include "tagsocket.h"

class TagTypeSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    TagTypeSortFilterProxyModel(QObject *parent = nullptr);

    void setFilterTagTypeCompatibleWithTagSocketType(TagSocket::Type type);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:
    std::optional<TagSocket::Type> tagSocketType_;
};

#endif // TAGTYPESORTFILTERPROXYMODEL_H
