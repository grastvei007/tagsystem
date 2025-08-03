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

#include "tagselectview.h"
#include "ui_tagselectview.h"

#include <QTableView>
#include <QResizeEvent>
#include <QSize>

#include "taglisttablemodel.h"
#include "tag.h"
#include "taglist.h"
#include "ui/tagtypesortfilterproxymodel.h"

TagSelectView::TagSelectView(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TagSelectView),
    mSelectedTag(nullptr)
{
    ui->setupUi(this);

    mTagListTableModel.reset(new TagListTableModel());
    mItemSelectionModel.reset(new QItemSelectionModel());
    tagListSortFilterProxyModel_ = std::make_unique<TagTypeSortFilterProxyModel>(this);

    tagListSortFilterProxyModel_->setSourceModel(mTagListTableModel.get());

    ui->mTableview->setModel(tagListSortFilterProxyModel_.get());
    ui->mTableview->setSortingEnabled(true);
    connect(ui->mCancel, &QPushButton::clicked, this, &TagSelectView::onCancelClicked);
    connect(ui->mSelect, &QPushButton::clicked, this, &TagSelectView::onSelectClicked);


  //  connect( ui->mTableView, &QTableView::selectionChanged, this, &TagSelectView::onCurrentRowChanged);
}

TagSelectView::~TagSelectView()
{
    delete ui;
}

void TagSelectView::setFilterTagTypeCompatibleWithTagSocketType(TagSocket::Type type)
{
    tagListSortFilterProxyModel_->setFilterTagTypeCompatibleWithTagSocketType(type);
}

void TagSelectView::onCancelClicked(bool)
{
    emit reject();
}


void TagSelectView::onSelectClicked(bool)
{
    mSelectedTag = ui->mTableview->getSelectedTag();

    if(mSelectedTag)
        emit accept();
    else    // nothing selected.
        emit reject();
}


Tag* TagSelectView::getSelectedTag() const
{
    return mSelectedTag;
}


TableView::TableView(QWidget *parent) : QTableView(parent)
{
    setSelectionMode(QAbstractItemView::SingleSelection);
}

TableView::~TableView()
{

}

Tag* TableView::getSelectedTag()
{
    Tag* tag = nullptr;
    auto idx = currentIndex();
    auto index = model()->index(idx.row(), TagListTableModel::eTagName);
    auto tagName = model()->data(index).toString();
    if(!tagName.isEmpty())
    {
        tag = TagList::sGetInstance().findByTagName(tagName);
    }

    return tag;
}

