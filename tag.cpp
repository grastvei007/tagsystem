#include "tag.h"

#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QXmlStreamAttributes>

Tag::Tag(QObject *parent) : QObject(parent),
  mDoubleValue(0.0),
  mIntValue(0),
  mBoolValue(false),
    mType(Tag::eDouble)
{

}


Tag::Tag(QString aSubSystem, QString aName, Type aType) :
    mSubSystem(aSubSystem),
    mName(aName),
    mType(aType),
    mDoubleValue(0.0),
    mIntValue(0),
    mBoolValue(false)
{

}

void Tag::setValue(double aValue)
{
    if(qFuzzyCompare(aValue, mDoubleValue))
        return;

    mDoubleValue = aValue;
    emit valueChanged(this);
}


void Tag::setValue(int aValue)
{
    if(aValue == mIntValue)
        return;

    mIntValue = aValue;
    emit valueChanged(this);
}


void Tag::setValue(bool aValue)
{
    if(aValue == mBoolValue)
        return;
    mBoolValue = aValue;
    emit valueChanged(this);
}

QString Tag::getFullName() const
{
    return QString("%1.%2").arg(mSubSystem).arg(mName);
}

QString Tag::getSubsystem() const
{
    return mSubSystem;
}


QString Tag::getName() const
{
    return mName;
}


Tag::Type Tag::getType() const
{
    return mType;
}

QString Tag::getTypeStr() const
{
    switch (mType) {
    case eDouble:
        return "Double";
    case eInt:
        return "Int";
    case eBool:
        return "Bool";
    }

    Q_UNREACHABLE();
}


double Tag::getDoubleValue() const
{
    return mDoubleValue;
}


int Tag::getIntValue() const
{
    return mIntValue;
}


bool Tag::getBoolValue() const
{
    return mBoolValue;
}


void Tag::writeToXml(QXmlStreamWriter &aStream)
{
    aStream.writeStartElement("tag");
    aStream.writeAttribute("subsystem", mSubSystem);
    aStream.writeAttribute("name", mName);
    aStream.writeAttribute("type", getTypeStr());
    if(mType == eDouble)
        aStream.writeAttribute("value", QString::number(mDoubleValue));
    else if(mType == eInt)
        aStream.writeAttribute("value", QString::number(mIntValue));
    else if(mType == eBool)
        aStream.writeAttribute("value", (mBoolValue) ? "1" : "0");
    else
        Q_UNREACHABLE(); ///< unhandled tag type.

    aStream.writeEndElement();
}


Tag* Tag::createFromXml(const QXmlStreamReader &aReader)
{
    QString sub = aReader.attributes().value("subsystem").toString();
    QString name = aReader.attributes().value("name").toString();
    QString type = aReader.attributes().value("type").toString();
    QString val = aReader.attributes().value("value").toString();
    Tag *tag = nullptr;
    if(type == "Double")
    {
        tag = new Tag(sub, name, eDouble);
        tag->setValue(val.toDouble());
    }
    else if(type == "Int")
    {
        tag = new Tag(sub, name, eInt);
        tag->setValue(val.toInt());
    }
    else if(type == "Bool")
    {
        tag = new Tag(sub,name, eBool);
        tag->setValue( (val.toInt() == 1) ? true : false );
    }

    aReader.attributes().value("value");


    return tag;
}


/**
 * @brief Tag::toMessage
 *
 * Convert a tag name and its value to binary representation.
 *
 * @return Binary representation of a name/value pair.
 */
QByteArray Tag::toMessage()
{
    QByteArray ba;
    ba.append(mName);
    if(mType == eDouble)
    {
        ba.append(":f");
        union U{
           float f;
           unsigned char byte[4];
        }u;
        u.f = (float)mDoubleValue;
        ba.append(u.byte, 4);
    }
    else if(mType == eInt)
    {
        ba.append(":i");
        union U{
            int i;
            unsigned char byte[4];
        }u;
        u.i = mIntValue;
        ba.append(u.byte, 4);
    }
    else if(mType == eBool)
    {
        ba.append(":b");
        ba.append( mBoolValue ? char(1) : char(0) );
    }
    else
        Q_UNREACHABLE();

    return ba;
}
