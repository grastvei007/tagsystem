#ifndef TAGSELECTVIEW_H
#define TAGSELECTVIEW_H

#include <QWidget>
#include <QDialog>
#include <memory>
#include <QTableView>
#include <QItemSelectionModel>



class TagListTableModel;
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

class Tag;
class TableView;

namespace Ui {
class TagSelectView;
}

class TagSelectView : public QDialog
{
    Q_OBJECT

public:
    explicit TagSelectView(QWidget *parent = 0);
    ~TagSelectView();

    Tag* getSelectedTag() const;

private slots:
    void onCancelClicked(bool);
    void onSelectClicked(bool);

private:
    Ui::TagSelectView *ui;

    std::unique_ptr<TagListTableModel> mTagListTableModel;
    Tag *mSelectedTag;
    std::unique_ptr<QItemSelectionModel> mItemSelectionModel;
   // std::unique_ptr<TableView> mTableView;
};


class TableView : public QTableView
{
    Q_OBJECT
public:
    TableView(QWidget *parent=nullptr);
    virtual ~TableView();

    Tag* getSelectedTag();

};


#endif // TAGSELECTVIEW_H
