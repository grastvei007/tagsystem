#ifndef TAGSOCKETLISTVIEW_H
#define TAGSOCKETLISTVIEW_H

#include <QWidget>
#include <QTableView>
#include <QModelIndex>
#include <memory>

namespace Ui {
class TagSocketListView;
}

class TagSocketListTableModel;

class TagSocketListView : public QWidget
{
    Q_OBJECT
public:
    TagSocketListView(QWidget *parent = 0);


private slots:
    void onDoubleClick(const QModelIndex &aIndex);

private:
    Ui::TagSocketListView *mUi;
    TagSocketListTableModel *mTagSocketListTableModel;
};

#endif // TAGSOCKETLISTVIEW_H
