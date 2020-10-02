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

#include "taglisttablemodel.h"

#include <QColor>

#include "taglist.h"
#include "tag.h"

TagListTableModel::TagListTableModel(QObject *aParent) : QAbstractTableModel(aParent)
{
    connect(&TagList::sGetInstance(), &TagList::tagCreated, this, &TagListTableModel::onTagCreated);
    connect(&TagList::sGetInstance(), &TagList::valueChanged, this, &TagListTableModel::onTagValueChanged);
}

TagListTableModel::~TagListTableModel()
{

}


int TagListTableModel::rowCount(const QModelIndex &parent) const
{
    return TagList::sGetInstance().getNumberOfTags();
}


int TagListTableModel::columnCount(const QModelIndex &parent) const
{
    return 4;
}

bool TagListTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(role == Qt::EditRole)
    {
        if(index.column() == eValue)
        {
            Tag *tag = TagList::sGetInstance().getTagByIndex(index.row());
            switch (tag->getType()) {
            case Tag::eDouble:
                tag->setValue(value.toDouble());
                return true;
            case Tag::eInt:
                tag->setValue(value.toInt());
                return true;
            case Tag::eBool:
                tag->setValue(value.toBool());
                return true;
            case Tag::eString:
                tag->setValue(value.toString());
                return true;
            }
        }
    }

    return false;
}


Qt::ItemFlags TagListTableModel::flags(const QModelIndex &index) const
{
    if(index.column() == eValue)
        return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;

    return Qt::ItemIsEnabled;
}

QVariant TagListTableModel::data(const QModelIndex &index, int role) const
{
    if(role == Qt::DisplayRole)
    {
        Tag *tag = TagList::sGetInstance().getTagByIndex(index.row());
        switch (index.column()) {
        case eTagName:
        {
            return tag->getFullName();
        }
        case eType:
        {
            return tag->getTypeStr();
        }
        case eValue:
        {
            if(tag->getType() == Tag::eDouble)
                return tag->getDoubleValue();
            else if(tag->getType() == Tag::eInt)
                return tag->getIntValue();
            else if(tag->getType() == Tag::eBool)
                return tag->getBoolValue();
            else if(tag->getType() == Tag::eString)
                return tag->getStringValue();
            else
                Q_UNREACHABLE();
        }
        case eTimeStamp:
            return tag->getTimeStamp();

        default:
            break;
        }
    }
    else if(role == Qt::BackgroundRole)
    {
        if(index.row() == 0)
            return false;
        else if((index.row() % 2) == 1)
            return QColor(Qt::gray);
    }

    return QVariant(QVariant::Invalid);
}


QVariant TagListTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal)
    {
        if(role == Qt::DisplayRole || role == Qt::EditRole)
        {
            switch (section) {
            case eTagName:
                return "Tag Name";
            case eType:
                return "Type";
            case eValue:
                return "Value";
            case eTimeStamp:
                return "TimeStamp";
            default:
                break;
            }
        }
    }
    else if(orientation == Qt::Vertical)
    {
        if(role == Qt::DisplayRole || role == Qt::EditRole)
            return QString::number(section);
    }

    return QVariant(QVariant::Invalid);
}


void TagListTableModel::onTagCreated()
{
    emit layoutAboutToBeChanged();
    emit layoutChanged();
}


void TagListTableModel::onTagValueChanged()
{
    //beginResetModel();
    QModelIndex top = index(0, eValue);
    QModelIndex bottom = index(rowCount(), eTimeStamp);
    emit dataChanged(top, bottom);
   // endResetModel();
}

