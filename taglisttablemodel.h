/*This file is part of June.

June is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Foobar is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar.  If not, see <https://www.gnu.org/licenses/>.*/

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
        eValue,
        eDescription,
        eTimeStamp
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
