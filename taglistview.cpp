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

#include "taglistview.h"

#include "taglisttablemodel.h"

#include <QGridLayout>

TagListView::TagListView(QWidget *parent) : QWidget(parent)
{

    mTableView.reset(new QTableView(this));

    QGridLayout *grid = new QGridLayout(this);
    grid->addWidget(mTableView.get());

    setLayout(grid);


    mTagListTableModel = new TagListTableModel();

    mTableView->setModel(mTagListTableModel);

}
