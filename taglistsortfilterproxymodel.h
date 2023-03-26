#ifndef TAGLISTSORTFILTERPROXYMODEL_H
#define TAGLISTSORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>


class TagListSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    TagListSortFilterProxyModel(QObject *parent = nullptr);
};

#endif // TAGLISTSORTFILTERPROXYMODEL_H
