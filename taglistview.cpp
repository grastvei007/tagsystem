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
#include "ui_taglistview.h"

#include "taglisttablemodel.h"
#include "taglist.h"
#include <QHeaderView>
#include <QComboBox>

TagListView::TagListView(QWidget *parent) : QWidget(parent),
    ui_(new Ui::TagListView)
{
    ui_->setupUi(this);

    tableView_.reset(ui_->tableView);
    tableView_->setSortingEnabled(true);
    tableView_->horizontalHeader()->setSectionsClickable(true);
    tableView_->horizontalHeader()->setStretchLastSection(true);
    tableView_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);


    auto updateComboBoxItem = [this](int){
        blockUpdate_ = true;
        ui_->comboBox->clear();
        auto subsystems = TagList::sGetInstance().subsystems();
        subsystems.push_front("--All--");
        ui_->comboBox->addItems(subsystems);
        blockUpdate_ = false;
    };

    updateComboBoxItem(1);

    connect(&TagList::sGetInstance(), &TagList::tagCreated, this, updateComboBoxItem);
    connect(ui_->comboBox, &QComboBox::currentTextChanged, this, &TagListView::onCurrentTextChanged);

    tagListSortFilterProxyModel_ = std::make_unique<QSortFilterProxyModel>(this);
    tagListSortFilterProxyModel_->setFilterKeyColumn(TagListTableModel::eTagName);

    tagListTableModel_ = new TagListTableModel();

    tagListSortFilterProxyModel_->setSourceModel(tagListTableModel_);
    tableView_->setModel(tagListSortFilterProxyModel_.get());
    tableView_->setSortingEnabled(true);

}

void TagListView::onCurrentTextChanged(const QString &text)
{
    if(blockUpdate_)
        return;

    if(text == "--All--")
        tagListSortFilterProxyModel_->setFilterFixedString("");
    else
        tagListSortFilterProxyModel_->setFilterFixedString(text);
}
