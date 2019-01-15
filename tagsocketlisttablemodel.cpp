#include "tagsocketlisttablemodel.h"

#include "tagsocket.h"
#include "tagsocketlist.h"
#include "tag.h"


TagSocketListTableModel::TagSocketListTableModel(QObject *parent) : QObject(parent)
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
                Q_UNREACHABLE;
        }
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
    //TODO: allow to change tag from gui.
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
