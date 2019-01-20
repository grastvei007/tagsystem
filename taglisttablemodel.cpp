#include "taglisttablemodel.h"

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
    return 3;
}

bool TagListTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
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
        }

        default:
            break;
        }
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
                break;
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


void TagListTableModel::onTagCreated()
{
    emit layoutAboutToBeChanged();
    emit layoutChanged();
}


void TagListTableModel::onTagValueChanged()
{
  //  beginResetModel();
    QModelIndex top = index(0, eValue);
    QModelIndex bottom = index(rowCount(), eValue);
    emit dataChanged(top, bottom);
   // endResetModel();
}

