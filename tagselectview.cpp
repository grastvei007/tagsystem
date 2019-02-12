#include "tagselectview.h"
#include "ui_tagselectview.h"

#include <QTableView>
#include <QResizeEvent>
#include <QSize>

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
    mItemSelectionModel.reset(new QItemSelectionModel());

    ui->mTableview->setModel(mTagListTableModel.get());
    connect(ui->mCancel, &QPushButton::clicked, this, &TagSelectView::onCancelClicked);
    connect(ui->mSelect, &QPushButton::clicked, this, &TagSelectView::onSelectClicked);


  //  connect( ui->mTableView, &QTableView::selectionChanged, this, &TagSelectView::onCurrentRowChanged);
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
    mSelectedTag = ui->mTableview->getSelectedTag();

   // QModelIndex idx = mItemSelectionModel->currentIndex();
   /* if(!list.isEmpty())
    {
        QModelIndex idx = list.first();
        mSelectedTag = TagList::sGetInstance().getTagByIndex(idx.row());

    }*/

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
    if(idx.isValid())
    {
        tag = TagList::sGetInstance().getTagByIndex(idx.row());
    }

    return tag;
}

