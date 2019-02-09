#include "tagselectview.h"
#include "ui_tagselectview.h"

#include <QTableView>

#include "taglisttablemodel.h"
#include "tag.h"
#include "taglist.h"

TagSelectView::TagSelectView(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TagSelectView),
    mSelectedTag(nullptr)
{
    ui->setupUi(this);

    mTagListTableModel.reset(new TagListTableModel());

    ui->mTableView->setModel(mTagListTableModel.get());
    ui->mTableView->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(ui->mCancel, &QPushButton::clicked, this, &TagSelectView::onCancelClicked);
    connect(ui->mSelect, &QPushButton::clicked, this, &TagSelectView::onSelectClicked);
}

TagSelectView::~TagSelectView()
{
    delete ui;
}


void TagSelectView::onCancelClicked(bool)
{
    emit reject();
}


void TagSelectView::onSelectClicked(bool)
{
    QModelIndexList list = ui->mTableView->selectedIndexes();
    if(!list.isEmpty())
    {
        QModelIndex idx = list.first();
        mSelectedTag = TagList::sGetInstance().getTagByIndex(idx.row());


        emit accepted();
    }

  /*  mSelectedTag = mTableView->getSelectedTag();
    if(mSelectedTag)
        emit accept();
    else    // nothing selected.
        emit reject();*/
}


Tag* TagSelectView::getSelectedTag() const
{
    return mSelectedTag;
}

