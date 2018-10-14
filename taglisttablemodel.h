#ifndef TAGLISTTABLEMODEL_H
#define TAGLISTTABLEMODEL_H

#include <QAbstractTableModel>



class TagListTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum Columns
    {
        eTagName,
        eType,
        eValue
    };

    TagListTableModel(QObject *aParent = nullptr);
    ~TagListTableModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex &index) const;
private slots:
    void onTagCreated();
    void onTagValueChanged();

};

#endif // TAGLISTTABLEMODEL_H
