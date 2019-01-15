#ifndef TAGSOCKETLISTVIEW_H
#define TAGSOCKETLISTVIEW_H

#include <QWidget>
#include <QTableView>
#include <memory>

class TagSocketListTableModel;

class TagSocketListView : public QWidget
{
    Q_OBJECT
public:
    TagSocketListView(QWidget *parent = 0);

private:
    TagSocketListTableModel *mTagSocketListTableModel;
    std::unique_ptr<QTableView> mTableView;
};

#endif // TAGSOCKETLISTVIEW_H
