#include "taglistview.h"

#include "taglisttablemodel.h"

#include <QGridLayout>

TagListView::TagListView(QWidget *parent) : QWidget(parent)
{

    mTableView.reset(new QTableView(this));

    QGridLayout *grid = new QGridLayout(this);
    grid->addWidget(mTableView.get());

    setLayout(grid);


    mTagListTableModel = new TagListTableModel();

    mTableView->setModel(mTagListTableModel);

}
