#ifndef TAGSOCKETLISTTABLEMODEL_H
#define TAGSOCKETLISTTABLEMODEL_H

#include <QAbstractTableModel>

class TagSocketListTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum Columns
    {
        eTagSocketName,
        eTagName,
        eType
    };

    TagSocketListTableModel(QObject *parent = 0);
    ~TagSocketListTableModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex &index) const;
signals:

private slots:
    void onTagSocketCreated();
    void onTagSocketRemoved();
};

#endif // TAGSOCKETLISTTABLEMODEL_H
