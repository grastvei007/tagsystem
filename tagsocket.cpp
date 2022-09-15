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
    return new TagSocket(aSubSystem, aName, aType);
}


TagSocket::TagSocket(QString aSubSystem, QString aName, Type aType) :
    subSystem_(aSubSystem),
    name_(aName),
    type_(aType)
{
    TagSocketList::sGetInstance().addTagSocket(this);
}

TagSocket::~TagSocket()
{
    TagSocketList::sGetInstance().removeTagSocket(this);
}

QString TagSocket::getFullName() const
{
    return QString("%1.%2").arg(subSystem_).arg(name_);
}


QString TagSocket::getSubSystem() const
{
    return subSystem_;
}


QString TagSocket::getName() const
{
    return name_;
}


QString TagSocket::getTypeStr() const
{
    if(type_ == eDouble)
        return QString("Double");
    else if(type_ == eInt)
        return QString("Int");
    else if(type_ == eBool)
        return QString("Bool");
    else if(type_ == eString)
        return QString("String");
    else if(type_ == eTime)
        return QString("Time");
    else
        Q_UNREACHABLE();
}


QString TagSocket::getTagName() const
{
    return tagName_;
}

TagSocket::Type TagSocket::getType() const
{
    return type_;
}

Tag* TagSocket::getTag() const
{
    return tag_;
}

bool TagSocket::hookupTag(Tag *aTag)
{
    if(!aTag)
        return false;

    if(type_ == eDouble)
    {
        if(aTag->getType() != Tag::eDouble)
            return false;
        tag_ = aTag;
    }
    else if(type_ == eBool)
    {
        if(aTag->getType() != Tag::eBool)
            return false;
        tag_ = aTag;
    }
    else if(type_ == eInt)
    {
        if(aTag->getType()  != Tag::eInt)
            return false;
        tag_ = aTag;
    }
    else if(type_ == eString)
    {
        if(aTag->getType() != Tag::eString)
            return false;
        tag_ = aTag;
    }
    else if(type_ == eTime)
    {
        if(aTag->getType() != Tag::eTime)
            return false;
        tag_ = aTag;
    }
    else
        Q_UNREACHABLE();

    if(tag_)
    {
        connect(tag_, &Tag::valueChanged, this, &TagSocket::onTagValueChanged);
        onTagValueChanged(tag_); // get an update
        tagName_ = tag_->getFullName();
        return true;
    }

    return false;
}

bool TagSocket::hookupTag(QString aTagSubsytem, QString aTagName)
{
    tagName_ = QString("%1.%2").arg(aTagSubsytem).arg(aTagName);
    Tag *tag = TagList::sGetInstance().findByTagName(tagName_);
    if(tag)
        return hookupTag(tag);

    // listen to changes to hookup tag when/if it is created.
    isWaitingForTag_ = true;
    connect(&TagList::sGetInstance(), &TagList::tagCreated, this, &TagSocket::onTagCreated);
    return false;
}

bool TagSocket::isHookedUp() const
{
    return tag_;
}

bool TagSocket::isWaitingForTag() const
{
    return isWaitingForTag_;
}

void TagSocket::writeValue(double aValue)
{
    if(tag_)
        tag_->setValue(aValue);
}


void TagSocket::writeValue(bool aValue)
{
    if(tag_)
        tag_->setValue(aValue);
}


void TagSocket::writeValue(int aValue)
{
    if(tag_)
        tag_->setValue(aValue);
}


void TagSocket::writeValue(QString aValue)
{
    if(tag_)
        tag_->setValue(aValue);
}

void TagSocket::writeValue(QDateTime aValue)
{
    if(tag_)
        tag_->setValue(aValue);
}


bool TagSocket::readValue(double &rValue)
{
    if(!tag_)
        return false;
    rValue = tag_->getDoubleValue();
    return true;
}


bool TagSocket::readValue(bool &rValue)
{
    if(!tag_)
        return false;
    rValue = tag_->getBoolValue();
    return true;
}


bool TagSocket::readValue(int &rValue)
{
    if(!tag_)
        return false;
    rValue = tag_->getIntValue();
    return true;
}


bool TagSocket::readValue(QString &rValue)
{
    if(!tag_)
        return false;
    rValue = tag_->getStringValue();
    return true;
}

bool TagSocket::readValue(QDateTime &rValue)
{
    if(!tag_)
        return false;
    rValue = tag_->getTimeValue();
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
    else if(aTypeString == "Time")
        return eTime;
    else
        Q_UNREACHABLE();
}

QString TagSocket::toString(const TagSocket::Type type)
{
    switch (type) {
        case TagSocket::eDouble:
            return "Double";
        case TagSocket::eInt:
            return "Int";
        case TagSocket::eBool:
            return "Bool";
        case TagSocket::eString:
            return "String";
        case TagSocket::eTime:
            return "Time";
        default:
            Q_UNREACHABLE();
    }
    return QString();
}

void TagSocket::onTagValueChanged(Tag* aTag)
{
    if(type_ == eDouble)
    {
        emit valueChanged(aTag->getDoubleValue());
    }
    else if(type_ == eBool)
    {
        emit valueChanged(aTag->getBoolValue());
    }
    else if(type_ == eInt)
    {
        emit valueChanged(aTag->getIntValue());
    }
    else if(type_ == eString)
        emit valueChanged(aTag->getStringValue());
    else if(type_ == eTime)
        emit valueChanged(tag_->getTimeValue());
    else
        Q_UNREACHABLE();

    emit valueChanged(this);
}

void TagSocket::onTagCreated()
{
    Tag *tag = TagList::sGetInstance().findByTagName(tagName_);
    if(!tag)
        return;

    if(hookupTag(tag))
    {
        isWaitingForTag_ = false;
        disconnect(&TagList::sGetInstance(), &TagList::tagCreated, this, &TagSocket::onTagCreated);
    }
}
