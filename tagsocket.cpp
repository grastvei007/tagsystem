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

#include "tagsocket.h"

#include "taglist.h"
#include "tagsocketlist.h"


TagSocket* TagSocket::createTagSocket(QString aSubSystem, QString aName, TagSocket::Type aType)
{
    QString fullname = QString("%1.%2").arg(aSubSystem).arg(aName);
    TagSocket *tagsocket = TagSocketList::sGetInstance().getTagSocketByName(fullname);
    if(tagsocket)
        return tagsocket;
    tagsocket = new TagSocket(aSubSystem, aName, aType);
    return tagsocket;
}


TagSocket::TagSocket(QString aSubSystem, QString aName, Type aType) :
    mSubSystem(aSubSystem),
    mName(aName),
    mType(aType),
    mTag(nullptr),
    mTagName()
{
    TagSocketList::sGetInstance().addTagSocket(this);
}

TagSocket::~TagSocket()
{
    TagSocketList::sGetInstance().removeTagSocket(this);
}

QString TagSocket::getFullName() const
{
    return QString("%1.%2").arg(mSubSystem).arg(mName);
}


QString TagSocket::getSubSystem() const
{
    return mSubSystem;
}


QString TagSocket::getName() const
{
    return mName;
}


QString TagSocket::getTypeStr() const
{
    if(mType == eDouble)
        return QString("Double");
    else if(mType == eInt)
        return QString("Int");
    else if(mType == eBool)
        return QString("Bool");
    else if(mType == eString)
        return QString("String");
    else
        Q_UNREACHABLE();
}


QString TagSocket::getTagName() const
{
    return mTagName;
}

TagSocket::Type TagSocket::getType() const
{
    return mType;
}

Tag* TagSocket::getTag() const
{
    return mTag;
}

bool TagSocket::hookupTag(Tag *aTag)
{
    if(!aTag)
        return false;

    if(mType == eDouble)
    {
        if(aTag->getType() != Tag::eDouble)
            return false;
        mTag = aTag;
    }
    else if(mType == eBool)
    {
        if(aTag->getType() != Tag::eBool)
            return false;
        mTag = aTag;
    }
    else if(mType == eInt)
    {
        if(aTag->getType()  != Tag::eInt)
            return false;
        mTag = aTag;
    }
    else if(mType == eString)
    {
        if(aTag->getType() != Tag::eString)
            return false;
        mTag = aTag;
    }

    if(mTag)
    {
        connect(mTag, &Tag::valueChanged, this, &TagSocket::onTagValueChanged);
        onTagValueChanged(mTag); // get an update
        mTagName = mTag->getFullName();
        return true;
    }

    return false;
}

bool TagSocket::hookupTag(QString aTagSubsytem, QString aTagName)
{
    mTagName = QString("%1.%2").arg(aTagSubsytem).arg(aTagName);
    Tag *tag = TagList::sGetInstance().findByTagName(mTagName);
    if(tag)
        return hookupTag(tag);

    // listen to changes to hookup tag when/if it is created.
    connect(&TagList::sGetInstance(), &TagList::tagCreated, this, &TagSocket::onTagCreated);
    return false;
}

bool TagSocket::isHookedUp() const
{
    return mTag;
}

void TagSocket::writeValue(double aValue)
{
    if(mTag)
        mTag->setValue(aValue);
}


void TagSocket::writeValue(bool aValue)
{
    if(mTag)
        mTag->setValue(aValue);
}


void TagSocket::writeValue(int aValue)
{
    if(mTag)
        mTag->setValue(aValue);
}


void TagSocket::writeValue(QString aValue)
{
    if(mTag)
        mTag->setValue(aValue);
}


bool TagSocket::readValue(double &rValue)
{
    if(!mTag)
        return false;
    rValue = mTag->getDoubleValue();
    return true;
}


bool TagSocket::readValue(bool &rValue)
{
    if(!mTag)
        return false;
    rValue = mTag->getBoolValue();
    return true;
}


bool TagSocket::readValue(int &rValue)
{
    if(!mTag)
        return false;
    rValue = mTag->getIntValue();
    return true;
}


bool TagSocket::readValue(QString &rValue)
{
    if(!mTag)
        return false;
    rValue = mTag->getStringValue();
    return true;
}

TagSocket::Type TagSocket::typeFromString(const QString &aTypeString)
{
    if(aTypeString == "Int")
        return eInt;
    else if(aTypeString == "Bool")
        return eBool;
    else if(aTypeString == "Double")
        return eDouble;
    else if(aTypeString == "String")
        return eString;
    else
        Q_UNREACHABLE();
}

void TagSocket::onTagValueChanged(Tag* aTag)
{
    if(mType == eDouble)
    {
        emit valueChanged(aTag->getDoubleValue());
    }
    else if(mType == eBool)
    {
        emit valueChanged(aTag->getBoolValue());
    }
    else if(mType == eInt)
    {
        emit valueChanged(aTag->getIntValue());
    }
    else if(mType == eString)
        emit valueChanged(aTag->getStringValue());
    else
        Q_UNREACHABLE();

    emit valueChanged(this);
}

void TagSocket::onTagCreated()
{
    Tag *tag = TagList::sGetInstance().findByTagName(mTagName);
    if(!tag)
        return;

    if(hookupTag(tag))
        disconnect(&TagList::sGetInstance(), &TagList::tagCreated, this, &TagSocket::onTagCreated);
}
