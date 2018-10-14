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

    mTableView.reset(new TableView(ui->mTableView));

    mTableView->setModel(mTagListTableModel.get());
    mTableView->setSelectionMode(QAbstractItemView::SingleSelection);

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
    /*QModelIndexList list = ui->mTableView->selectedIndexes();
    if(!list.isEmpty())
    {
        QModelIndex idx = list.first();
        mSelectedTag = TagList::sGetInstance().getTagByIndex(idx.row());


        emit accepted();
    }*/

    mSelectedTag = mTableView->getSelectedTag();
    if(mSelectedTag)
        emit accept();
    else    // nothing selected.
        emit reject();
}


Tag* TagSelectView::getSelectedTag() const
{
    return mSelectedTag;
}


TableView::TableView(QWidget *aParent) : QTableView(aParent)
{

}


TableView::~TableView()
{

}


Tag* TableView::getSelectedTag()
{
    Tag *tag = nullptr;
    QModelIndexList list = selectedIndexes();
    if(!list.isEmpty())
    {
        QModelIndex idx = list.first();
        tag = TagList::sGetInstance().getTagByIndex(idx.row());
    }
    return tag;
}
