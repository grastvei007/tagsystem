#include "tagsocketlistview.h"

#include <QGridLayout>

#include "tagsocketlisttablemodel.h"

TagSocketListView::TagSocketListView(QWidget *parent) : QWidget(parent)
{
    mTableView.reset(new QTableView(this));

    QGridLayout *grid = new QGridLayout(this);
    grid->addWidget(mTableView.get());

    setLayout(grid);

    mTagSocketListTableModel = new TagSocketListTableModel();
    mTableView->setModel(mTagSocketListTableModel);
}

