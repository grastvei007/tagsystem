#ifndef TAGSELECTVIEW_H
#define TAGSELECTVIEW_H

#include <QWidget>
#include <QDialog>
#include <memory>
#include <QTableView>

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
    std::unique_ptr<TableView> mTableView;
    Tag *mSelectedTag;
};


#endif // TAGSELECTVIEW_H
