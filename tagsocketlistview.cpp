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

#include "tagsocketlistview.h"

#include <QGridLayout>
#include <QHeaderView>

#include "tagselectview.h"
#include "tag.h"

#include "tagsocketlisttablemodel.h"
#include "ui_tagsocketlistview.h"

TagSocketListView::TagSocketListView(QWidget *parent) : QWidget(parent),
    ui_(new Ui::TagSocketListView)
{
    ui_->setupUi(this);

    tagSocketListTableModel_ = new TagSocketListTableModel();
    ui_->mTable->setModel(tagSocketListTableModel_);
    ui_->mTable->horizontalHeader()->setStretchLastSection(true);
    ui_->mTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    connect(ui_->mTable, &QTableView::doubleClicked, this, &TagSocketListView::onDoubleClick);

    connect(ui_->mClose, &QPushButton::clicked, this, &TagSocketListView::onCloseClicked);
    connect(ui_->mOk, &QPushButton::clicked, this, &TagSocketListView::onOkClicked);
    connect(ui_->mSave, &QPushButton::clicked, this, &TagSocketListView::onSaveClicked);
}




void TagSocketListView::onDoubleClick(const QModelIndex &index)
{
    if(index.column() != TagSocketListTableModel::eTagName)
        return;

    TagSelectView tagSelect;
    tagSelect.setAttribute(Qt::WA_QuitOnClose, false);
    if(tagSelect.exec() == QDialog::Accepted)
    {        Tag *tag = tagSelect.getSelectedTag();
        if(tag)
            ui_->mTable->model()->setData(index, tag->getFullName(), Qt::EditRole);

    }

 }


void TagSocketListView::onCloseClicked(bool /*checked*/)
{
    hide();
}


void TagSocketListView::onOkClicked(bool /*checked*/)
{

}


void TagSocketListView::onSaveClicked(bool /*ckecked*/)
{
    tagSocketListTableModel_->saveTagSocketBindings();
}
