#ifndef TAGLISTVIEW_H
#define TAGLISTVIEW_H

#include <QWidget>
#include <memory>
#include <QTableView>

class TagListTableModel;

class TagListView : public QWidget
{
    Q_OBJECT
public:
    TagListView(QWidget *parent=nullptr);

private:
    TagListTableModel *mTagListTableModel;
    std::unique_ptr<QTableView> mTableView;
};

#endif // TAGLISTVIEW_H
