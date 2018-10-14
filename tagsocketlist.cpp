#include "tagsocketlist.h"

TagSocketList& TagSocketList::sGetInstance()
{
    static TagSocketList sTagSocketList;
    return sTagSocketList;
}

int TagSocketList::getNumberOfTagSockets() const
{
    return mTagSocketList.size();
}

bool TagSocketList::addTagSocket(TagSocket *aTagSocket)
{
    mTagSocketList.push_back(aTagSocket);
    mTagSocketByName[aTagSocket->getFullName()] = aTagSocket;
    emit tagSocketAdded();
}

void TagSocketList::removeTagSocket(TagSocket *aTagSocket)
{
    mTagSocketByName.remove(aTagSocket->getFullName());
    mTagSocketList.removeAll(aTagSocket);
    emit tagSocketRemoved();
}
