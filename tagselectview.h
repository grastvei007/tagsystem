#ifndef TAGSELECTVIEW_H
#define TAGSELECTVIEW_H

#include <QWidget>
#include <QDialog>
#include <memory>
#include <QTableView>
#include <QItemSelectionModel>



class TagListTableModel;
class Tag;
class TableView;

namespace Ui {
class TagSelectView;
}

class TagSelectView : public QDialog
{
    Q_OBJECT

public:
    explicit TagSelectView(QWidget *parent = 0);
    ~TagSelectView();

    Tag* getSelectedTag() const;

private slots:
    void onCancelClicked(bool);
    void onSelectClicked(bool);

private:
    Ui::TagSelectView *ui;

    std::unique_ptr<TagListTableModel> mTagListTableModel;
    Tag *mSelectedTag;
    std::unique_ptr<QItemSelectionModel> mItemSelectionModel;
   // std::unique_ptr<TableView> mTableView;
};


class TableView : public QTableView
{
    Q_OBJECT
public:
    TableView(QWidget *parent=nullptr);
    virtual ~TableView();

    Tag* getSelectedTag();

};


#endif // TAGSELECTVIEW_H
