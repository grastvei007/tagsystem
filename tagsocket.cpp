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

#include <QJsonObject>


TagSocket* TagSocket::createTagSocket(QString subSystem, QString name, TagSocket::Type type)
{
    QString fullname = QString("%1.%2").arg(subSystem, name);
    TagSocket *tagsocket = TagSocketList::sGetInstance().getTagSocketByName(fullname);
    if(tagsocket)
        return tagsocket;
    return new TagSocket(subSystem, name, type);
}

TagSocket *TagSocket::createFromJson(const QJsonObject &json)
{
    const auto subsystem = json.value("subsystem").toString();
    const auto name = json.value("name").toString();
    if(subsystem.isEmpty() || name.isEmpty())
        return nullptr;
    if(auto *tagsocket = TagSocketList::sGetInstance().findTagSocketByName(subsystem, name); tagsocket != nullptr)
        return tagsocket;

    auto type = TagSocket::typeFromString(json.value("type").toString());
    TagSocket *tagsocket = new TagSocket(subsystem, name, type);

    QString tag = json.value("tagname").toString();
    auto tagsubsystem = tag.split(".").first();
    auto tagname = tag.split(".").last();
    if(!tagsubsystem.isEmpty() && !tagname.isEmpty())
        tagsocket->hookupTag(tagsubsystem, tagname);
    return tagsocket;
}

TagSocket::TagSocket(QString subSystem, QString name, Type type) :
    subSystem_(subSystem),
    name_(name),
    type_(type)
{
    TagSocketList::sGetInstance().addTagSocket(this);
}

TagSocket::~TagSocket()
{
    TagSocketList::sGetInstance().removeTagSocket(this);
}

QString TagSocket::getFullName() const
{
    return QString("%1.%2").arg(subSystem_, name_);
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

void TagSocket::setScaleValue(double scale)
{
    scaleValue_ = scale;
}

bool TagSocket::hookupTag(Tag *tag)
{
    if(!tag)
        return false;

    if(type_ == eDouble)
    {
        if(tag->getType() != TagType::eDouble)
            return false;
        tag_ = tag;
    }
    else if(type_ == eBool)
    {
        if(tag->getType() != TagType::eBool)
            return false;
        tag_ = tag;
    }
    else if(type_ == eInt)
    {
        if(auto tagType = tag->getType(); tagType == TagType::eInt)
        {
            tag_ = tag;
        }
        else if(tagType == TagType::eBool)
        {
            tag_ = tag;
        }
        else
            return false;
    }
    else if(type_ == eString)
    {
        if(tag->getType() != TagType::eString)
            return false;
        tag_ = tag;
    }
    else if(type_ == eTime)
    {
        if(tag->getType() != TagType::eTime)
            return false;
        tag_ = tag;
    }
    else
        Q_UNREACHABLE();

    if(tag_)
    {
        connect(tag_, &Tag::valueChanged, this, &TagSocket::onTagValueChanged);
        onTagValueChanged(tag_); // get an update
        tagName_ = tag_->getFullName();
        qDebug() << "Hookup: " << QString("%1.%2").arg(subSystem_, name_) << " -> " << tagName_;
        return true;
    }

    return false;
}

bool TagSocket::hookupTag(QString tagSubsytem, QString tagName)
{
    tagName_ = QString("%1.%2").arg(tagSubsytem, tagName);
    Tag *tag = TagList::sGetInstance().findByTagName(tagName_);
    if(tag)
        return hookupTag(tag);

    // listen to changes to hookup tag when/if it is created.
    isWaitingForTag_ = true;
    connect(&TagList::sGetInstance(), &TagList::tagCreated, this, &TagSocket::onTagCreated);
    return false;
}

bool TagSocket::disconnectTag()
{
    if(isWaitingForTag_)
    {
        disconnect(&TagList::sGetInstance(), &TagList::tagCreated, this, &TagSocket::onTagCreated);
    }
    isWaitingForTag_ = false;
    tagName_ = {};
    if(tag_)
    {
        disconnect(tag_, &Tag::valueChanged, this, &TagSocket::onTagValueChanged);
        tag_ = nullptr;
    }
    return true;
}

bool TagSocket::isHookedUp() const
{
    return tag_;
}

bool TagSocket::isWaitingForTag() const
{
    return isWaitingForTag_;
}

void TagSocket::writeValue(double value)
{
    if(tag_)
        tag_->setValue(value * scaleValue_);
}


void TagSocket::writeValue(bool value)
{
    if(tag_)
        tag_->setValue(value);
}


void TagSocket::writeValue(int value)
{
    if(!tag_)
        return;
    if(type_ == eInt)
        tag_->setValue(value);
    else if(type_ == eBool)
    {
        bool val = std::clamp(0, 1, value);
        tag_->setValue(val);
    }
}


void TagSocket::writeValue(QString value)
{
    if(tag_)
        tag_->setValue(value);
}

void TagSocket::writeValue(QDateTime value)
{
    if(tag_)
        tag_->setValue(value);
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

    if(type_ == eInt)
    {
        rValue = tag_->getIntValue();
        return true;
    }
    else if(type_ == eBool)
    {
        rValue = static_cast<int>(tag_->getBoolValue());
        return true;

    }
    return false;
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

QJsonObject TagSocket::toJson() const
{
    QJsonObject obj;

    obj.insert("subsystem", getSubSystem());
    obj.insert("name", getName());
    obj.insert("type", getTypeStr());
    obj.insert("hookedup", isHookedUp());
    obj.insert("tagname", getTagName());

    return obj;
}

TagSocket::Type TagSocket::typeFromString(const QString &typeString)
{
    if(typeString == "Int")
        return eInt;
    else if(typeString == "Bool")
        return eBool;
    else if(typeString == "Double")
        return eDouble;
    else if(typeString == "String")
        return eString;
    else if(typeString == "Time")
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

TagSocket::Type TagSocket::typeMatchingTag(const Tag *tag)
{
    if(!tag)
        return eNone;
    switch (tag->getType()) {
    case TagType::eBool:
        return eBool;
    case TagType::eDouble:
        return eDouble;
    case TagType::eInt:
        return eInt;
    case TagType::eString:
        return eString;
    case TagType::eTime:
        return eTime;
    default:
        return eNone;
    }
    Q_UNREACHABLE();
}

void TagSocket::onTagValueChanged(Tag* tag)
{
    if(type_ == eDouble)
    {
        emit valueChanged(tag->getDoubleValue());
    }
    else if(type_ == eBool)
    {
        emit valueChanged(tag->getBoolValue());
    }
    else if(type_ == eInt)
    {
        if(tag->getType() == TagType::eBool)
        {
            int value = tag->getBoolValue();
            emit valueChanged(value);
        }
        else
        {
            emit valueChanged(tag->getIntValue());
        }
    }
    else if(type_ == eString)
        emit valueChanged(tag->getStringValue());
    else if(type_ == eTime)
        emit valueChanged(tag->getTimeValue());
    else
        Q_UNREACHABLE();

    emit valueChanged(this);
}

void TagSocket::onTagCreated(int index)
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
