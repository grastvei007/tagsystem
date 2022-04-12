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

#include "tag.h"

#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QXmlStreamAttributes>

Tag::Tag(QObject *parent) : QObject(parent),
  mType(Tag::eDouble),
  mDoubleValue(0.0),
  mIntValue(0),
  mBoolValue(false),
  mStringValue(),
  mTimeStampFormat("dd.MM.yyyy hh:mm:ss.zzz"),
  mTimeStamp(QDateTime::currentMSecsSinceEpoch())
{

}


Tag::Tag(QString aSubSystem, QString aName, Type aType) :
    mSubSystem(aSubSystem),
    mName(aName),
    mType(aType),
    mDoubleValue(0.0),
    mIntValue(0),
    mBoolValue(false),
    mStringValue(),
    mTimeStampFormat("dd.MM.yyyy hh:mm:ss.zzz"),
    mTimeStamp(QDateTime::currentMSecsSinceEpoch())
{

}

Tag::Tag(QString subSystem, QString name, Tag::Type type, double initValue) :
    mSubSystem(subSystem),
    mName(name),
    mType(type),
    mDoubleValue(initValue),
    mIntValue(0),
    mBoolValue(false),
    mStringValue(),
    mTimeStampFormat("dd.MM.yyyy hh:mm:ss.zzz"),
    mTimeStamp(QDateTime::currentMSecsSinceEpoch())
{

}

Tag::Tag(QString subSystem, QString name, Tag::Type type, int initValue) :
    mSubSystem(subSystem),
    mName(name),
    mType(type),
    mDoubleValue(0.0),
    mIntValue(initValue),
    mBoolValue(false),
    mStringValue(),
    mTimeStampFormat("dd.MM.yyyy hh:mm:ss.zzz"),
    mTimeStamp(QDateTime::currentMSecsSinceEpoch())
{

}

Tag::Tag(QString subSystem, QString name, Tag::Type type, bool initValue) :
    mSubSystem(subSystem),
    mName(name),
    mType(type),
    mDoubleValue(0.0),
    mIntValue(0),
    mBoolValue(initValue),
    mStringValue(),
    mTimeStampFormat("dd.MM.yyyy hh:mm:ss.zzz"),
    mTimeStamp(QDateTime::currentMSecsSinceEpoch())
{

}

Tag::Tag(QString subSystem, QString name, Tag::Type type, QString initValue) :
    mSubSystem(subSystem),
    mName(name),
    mType(type),
    mDoubleValue(0.0),
    mIntValue(0),
    mBoolValue(false),
    mStringValue(initValue),
    mTimeStampFormat("dd.MM.yyyy hh:mm:ss.zzz"),
    mTimeStamp(QDateTime::currentMSecsSinceEpoch())
{

}

void Tag::setValue(double aValue, qint64 msSinceEpoc)
{
    if(qFuzzyCompare(aValue, mDoubleValue))
        return;

    mDoubleValue = aValue;
    if(msSinceEpoc < 0)
        mTimeStamp = QDateTime::currentMSecsSinceEpoch();
    else
        mTimeStamp = msSinceEpoc;
    emit valueChanged(this);
}


void Tag::setValue(int aValue, qint64 msSinceEpoc)
{
    if(aValue == mIntValue)
        return;

    mIntValue = aValue;
    if(msSinceEpoc < 0)
        mTimeStamp = QDateTime::currentMSecsSinceEpoch();
    else
        mTimeStamp = msSinceEpoc;
    emit valueChanged(this);
}


void Tag::setValue(bool aValue, qint64 msSinceEpoc)
{
    if(aValue == mBoolValue)
        return;

    mBoolValue = aValue;
    if(msSinceEpoc < 0)
        mTimeStamp = QDateTime::currentMSecsSinceEpoch();
    else
        mTimeStamp = msSinceEpoc;
    emit valueChanged(this);
}


void Tag::setValue(QString aValue, qint64 msSinceEpoc)
{
    if(mStringValue == aValue)
        return;

    if(msSinceEpoc < 0)
        mTimeStamp = QDateTime::currentMSecsSinceEpoch();
    else
        mTimeStamp = msSinceEpoc;
    mStringValue = aValue;
    emit valueChanged(this);
}

void Tag::setValue(QDateTime aValue, qint64 msSinceEpoc)
{
    qint64 value = aValue.toMSecsSinceEpoch();
    if(value == mTimeValue)
        return;
    if(msSinceEpoc < 0)
        mTimeStamp = QDateTime::currentMSecsSinceEpoch();
    else
        mTimeStamp = msSinceEpoc;

    mTimeValue = value;
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

QString Tag::getTimeStamp() const
{
    return QDateTime::fromMSecsSinceEpoch(mTimeStamp).toString(mTimeStampFormat);
}

const QString &Tag::getTimeStampFormat() const
{
    return mTimeStampFormat;
}

qint64 Tag::getMsSinceEpoc() const
{
    return mTimeStamp;
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
    case eString:
        return "String";
    case eTime:
        return "Time";
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


QString Tag::getStringValue() const
{
    return mStringValue;
}

QDateTime Tag::getTimeValue() const
{
    return QDateTime::fromMSecsSinceEpoch(mTimeValue);
}

void Tag::writeToXml(QXmlStreamWriter &aStream)
{
    aStream.writeStartElement("tag");
    aStream.writeAttribute("subsystem", mSubSystem);
    aStream.writeAttribute("name", mName);
    aStream.writeAttribute("timestamp", QString::number(mTimeStamp));
    aStream.writeAttribute("type", getTypeStr());
    if(mType == eDouble)
        aStream.writeAttribute("value", QString::number(mDoubleValue));
    else if(mType == eInt)
        aStream.writeAttribute("value", QString::number(mIntValue));
    else if(mType == eBool)
        aStream.writeAttribute("value", (mBoolValue) ? "1" : "0");
    else if(mType == eString)
        aStream.writeAttribute("value", mStringValue);
    else if(mType == eTime)
        aStream.writeAttribute("value", QString::number(mTimeValue));
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
    else if(type == "String")
    {
        tag = new Tag(sub, name, eString);
        tag->setValue(val);
    }
    else if(typeFromString(type) == eTime)
    {
        tag = new Tag(sub, name, eTime);
        tag->setValue(QDateTime::fromMSecsSinceEpoch(val.toLongLong()));
    }
    aReader.attributes().value("value");


    return tag;
}

Tag::Type Tag::typeFromString(const QString &aTypeString)
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

QString Tag::toString(Tag::Type aType)
{
    switch (aType) {
        case eInt:
            return "Int";
        case eBool:
            return "Bool";
        case eDouble:
            return "Double";
        case eString:
            return "String";
        case eTime:
            return "Time";
        default:
            break;
    }
    Q_UNREACHABLE();
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
    ba.append(mName.toLatin1());
    if(mType == eDouble)
    {
        ba.append(":f");
        union U{
           float f;
           char byte[4];
        }u;
        u.f = (float)mDoubleValue;
        ba.append(u.byte, 4);
    }
    else if(mType == eInt)
    {
        ba.append(":i");
        union U{
            int i;
            char byte[4];
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
