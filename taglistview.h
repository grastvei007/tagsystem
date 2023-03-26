/*This file is part of June.

June is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Foobar is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar.  If not, see <https://www.gnu.org/licenses/>.*/

#ifndef TAGLISTVIEW_H
#define TAGLISTVIEW_H

#include "tagsystem_global.h"
#include <QWidget>
#include <memory>
#include <QTableView>

#include "taglistsortfilterproxymodel.h"

class TagListTableModel;

class TAGSYSTEMSHARED_EXPORT TagListView : public QWidget
{
    Q_OBJECT
public:
    TagListView(QWidget *parent=nullptr);

private slots:

private:
    TagListTableModel *mTagListTableModel;
    std::unique_ptr<QTableView> mTableView;
    std::unique_ptr<QSortFilterProxyModel> tagListSortFilterProxyModel_;
};

#endif // TAGLISTVIEW_H
