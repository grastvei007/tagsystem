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
}


TagSocketListTableModel::~TagSocketListTableModel()
{

}


int TagSocketListTableModel::rowCount(const QModelIndex &parent) const
{
    return TagSocketList::sGetInstance().getNumberOfTagSockets();
}


int TagSocketListTableModel::columnCount(const QModelIndex &parent) const
{
    return 4;
}


QVariant TagSocketListTableModel::data(const QModelIndex &index, int role) const
{
    if(role == Qt::DisplayRole)
    {
        TagSocket *tagsocket = TagSocketList::sGetInstance().getTagSocketByIndex(index.row());
        if(!tagsocket)
            return QVariant(QVariant::Invalid);
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
                else {
                   Q_UNREACHABLE();
                }
            }
            default:
                Q_UNREACHABLE();
        }
    }
    else if(role == Qt::BackgroundColorRole)
    {
        TagSocket *tagsocket = TagSocketList::sGetInstance().getTagSocketByIndex(index.row());
        if(!tagsocket)
            return QVariant(QVariant::Invalid);
        if(tagsocket->isHookedUp())
            return QColor(Qt::green);
        else if(!tagsocket->isHookedUp() && !tagsocket->getTagName().isEmpty())
            return QColor(Qt::red);
        else
            return QColor(Qt::gray);
    }

    return QVariant(QVariant::Invalid);
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
    return QVariant(QVariant::Invalid);
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
