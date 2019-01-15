#include "tagsocket.h"

#include "taglist.h"
#include "tagsocketlist.h"

TagSocket::TagSocket(QString aSubSystem, QString aName, Type aType) :
    mSubSystem(aSubSystem),
    mName(aName),
    mType(aType),
    mTag(nullptr)
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
    else
        Q_UNREACHABLE;
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

    if(mTag)
    {
        connect(mTag, &Tag::valueChanged, this, &TagSocket::onTagValueChanged);
        onTagValueChanged(mTag); // get an update
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
    else
        Q_UNREACHABLE();
}

void TagSocket::onTagCreated()
{
    Tag *tag = TagList::sGetInstance().findByTagName(mTagName);
    if(!tag)
        return;

    if(hookupTag(tag))
        disconnect(&TagList::sGetInstance(), &TagList::tagCreated, this, &TagSocket::onTagCreated);
}
