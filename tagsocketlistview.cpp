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
    mUi(new Ui::TagSocketListView)
{
    mUi->setupUi(this);

    mTagSocketListTableModel = new TagSocketListTableModel();
    mUi->mTable->setModel(mTagSocketListTableModel);
    mUi->mTable->horizontalHeader()->setStretchLastSection(true);
    mUi->mTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    connect(mUi->mTable, &QTableView::doubleClicked, this, &TagSocketListView::onDoubleClick);

    connect(mUi->mClose, &QPushButton::clicked, this, &TagSocketListView::onCloseClicked);
    connect(mUi->mOk, &QPushButton::clicked, this, &TagSocketListView::onOkClicked);
    connect(mUi->mSave, &QPushButton::clicked, this, &TagSocketListView::onSaveClicked);
}




void TagSocketListView::onDoubleClick(const QModelIndex &aIndex)
{
    if(aIndex.column() != TagSocketListTableModel::eTagName)
        return;

    TagSelectView tagSelect;
    tagSelect.setAttribute(Qt::WA_QuitOnClose, false);
    if(tagSelect.exec() == QDialog::Accepted)
    {        Tag *tag = tagSelect.getSelectedTag();
        if(tag)
            mUi->mTable->model()->setData(aIndex, tag->getFullName(), Qt::EditRole);

    }

 }


void TagSocketListView::onCloseClicked(bool aChecked)
{

}


void TagSocketListView::onOkClicked(bool aChecked)
{

}


void TagSocketListView::onSaveClicked(bool aCkecked)
{
    mTagSocketListTableModel->saveTagSocketBindings();
}
