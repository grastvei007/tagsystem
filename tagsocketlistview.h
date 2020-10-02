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

#ifndef TAGSOCKETLISTVIEW_H
#define TAGSOCKETLISTVIEW_H

#include "tagsystem_global.h"
#include <QWidget>
#include <QTableView>
#include <QModelIndex>
#include <memory>

namespace Ui {
class TagSocketListView;
}

class TagSocketListTableModel;

class TAGSYSTEMSHARED_EXPORT TagSocketListView : public QWidget
{
    Q_OBJECT
public:
    TagSocketListView(QWidget *parent = 0);


private slots:
    void onDoubleClick(const QModelIndex &aIndex);
    void onCloseClicked(bool aChecked);
    void onOkClicked(bool aChecked);
    void onSaveClicked(bool aCkecked);

private:
    Ui::TagSocketListView *mUi;
    TagSocketListTableModel *mTagSocketListTableModel;
};

#endif // TAGSOCKETLISTVIEW_H
