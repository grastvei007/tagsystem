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


TagSocket* TagSocketList::getTagSocketByIndex(int aIndex)
{
    if(aIndex < 0 || aIndex > mTagSocketList.size())
        return nullptr;

    return mTagSocketList.at(aIndex);
}
