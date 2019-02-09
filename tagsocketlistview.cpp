#include "tagsocketlistview.h"

#include <QGridLayout>
#include <QHeaderView>

#include "tagselectview.h"
#include "tag.h"

#include "tagsocketlisttablemodel.h"
#include "ui_tagsocketlistview.h".h"

TagSocketListView::TagSocketListView(QWidget *parent) : QWidget(parent),
    mUi(new Ui::TagSocketListView)
{
    mUi->setupUi(this);

    mTagSocketListTableModel = new TagSocketListTableModel();
    mUi->mTable->setModel(mTagSocketListTableModel);
    mUi->mTable->horizontalHeader()->setStretchLastSection(true);

    connect(mUi->mTable, &QTableView::doubleClicked, this, &TagSocketListView::onDoubleClick);

}




void TagSocketListView::onDoubleClick(const QModelIndex &aIndex)
{
    if(aIndex.column() != TagSocketListTableModel::eTagName)
        return;

    TagSelectView tagSelect;
    if(tagSelect.exec() == QDialog::Accepted)
    {
        Tag *tag = tagSelect.getSelectedTag();
        if(tag)
            mUi->mTable->model()->setData(aIndex, tag->getFullName(), Qt::EditRole);

    }

 }
