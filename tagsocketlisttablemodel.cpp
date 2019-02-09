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
    return 3;
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
                Tag *tag = tagsocket->getTag();
                if(tag)
                    return tag->getFullName();
                break;
            }
            case eType:
            {
                return tagsocket->getTypeStr();
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
