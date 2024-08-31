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

#include "tagsocketlisttablemodel.h"

#include <QColor>

#include "tagsocket.h"
#include "tagsocketlist.h"
#include "tag.h"
#include "taglist.h"


TagSocketListTableModel::TagSocketListTableModel(QObject *parent) : QAbstractTableModel(parent)
{
    connect(&TagSocketList::sGetInstance(),&TagSocketList::tagSocketAdded, this, &TagSocketListTableModel::onTagSocketCreated);
    connect(&TagSocketList::sGetInstance(), &TagSocketList::tagSocketRemoved, this, &TagSocketListTableModel::onTagSocketRemoved);
    connect(&TagSocketList::sGetInstance(), &TagSocketList::tagSocketValueChanged, this, &TagSocketListTableModel::onTagSocketValueChanged);
}


TagSocketListTableModel::~TagSocketListTableModel()
{

}


int TagSocketListTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return TagSocketList::sGetInstance().getNumberOfTagSockets();
}


int TagSocketListTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 4;
}


QVariant TagSocketListTableModel::data(const QModelIndex &index, int role) const
{
    if(role == Qt::DisplayRole)
    {
        TagSocket *tagsocket = TagSocketList::sGetInstance().getTagSocketByIndex(index.row());
        if(!tagsocket)
            return QVariant();
        switch(index.column())
        {
            case eTagSocketName:
            {
                return tagsocket->getFullName();
            }
            case eTagName:
            {
               return tagsocket->getTagName();
            }
            case eType:
            {
                return tagsocket->getTypeStr();
            }
            case eValue:
            {
                if(tagsocket->getType() == TagSocket::eBool)
                {
                    bool b;
                    tagsocket->readValue(b);
                    return QVariant(b);
                }
                else if(tagsocket->getType() == TagSocket::eDouble)
                {
                    double d;
                    tagsocket->readValue(d);
                    return QVariant(d);
                }
                else if(tagsocket->getType() == TagSocket::eInt)
                {
                    int i;
                    tagsocket->readValue(i);
                    return QVariant(i);
                }
                else if(tagsocket->getType() == TagSocket::eString)
                {
                    QString str;
                    tagsocket->readValue(str);
                    return str;
                }
                else if(tagsocket->getType() == TagSocket::eTime)
                {
                    break;
                }
                else {
                   Q_UNREACHABLE();
                }
            }
            default:
                Q_UNREACHABLE();
        }
    }
    else if(role == Qt::BackgroundRole)
    {
        TagSocket *tagsocket = TagSocketList::sGetInstance().getTagSocketByIndex(index.row());
        if(!tagsocket)
            return QVariant();
        if(tagsocket->isHookedUp())
            return QColor(Qt::green);
        else if(!tagsocket->isHookedUp() && !tagsocket->getTagName().isEmpty())
            return QColor(Qt::red);
        else
            return QColor(Qt::gray);
    }

    return QVariant();
}


QVariant TagSocketListTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal)
    {
        if(role == Qt::DisplayRole || role == Qt::EditRole)
        {
            switch(section)
            {
                case eTagSocketName:
                    return "TagSocket Name";
                case eTagName:
                    return "Tag Name";
                case eType:
                    return "Type";
                case eValue:
                    return "Value";
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
    return QVariant();
}


bool TagSocketListTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(role == Qt::EditRole)
    {
        if(index.column() == eTagName)
        {
            Tag *tag = TagList::sGetInstance().findByTagName(value.toString());
            if(!tag)
                return false;

            TagSocket *tagsocet = TagSocketList::sGetInstance().getTagSocketByIndex(index.row());
            if(!tagsocet)
                return false;

            tagsocet->hookupTag(tag);
            return true;
        }
    }
    return false;
}

Qt::ItemFlags TagSocketListTableModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return Qt::ItemIsEnabled;
}


void TagSocketListTableModel::onTagSocketCreated()
{
    emit layoutAboutToBeChanged();
    emit layoutChanged();
}


void TagSocketListTableModel::onTagSocketRemoved()
{
    emit layoutAboutToBeChanged();
    emit layoutChanged();
}



void TagSocketListTableModel::saveTagSocketBindings()
{
    TagSocketList::sGetInstance().saveBindingList();
}


void TagSocketListTableModel::onTagSocketValueChanged()
{
    QModelIndex top = index(0, eValue);
    QModelIndex bottom = index(rowCount(), eValue);
    emit dataChanged(top, bottom);
}
