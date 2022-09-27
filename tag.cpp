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

Tag::Tag(QObject *parent) : QObject(parent)
{

}


Tag::Tag(QString aSubSystem, QString aName, Type aType) :
    subSystem_(aSubSystem),
    name_(aName),
    type_(aType)
{

}

Tag::Tag(QString subSystem, QString name, Tag::Type type, double initValue, const QString &description) :
    subSystem_(subSystem),
    name_(name),
    type_(type),
    description_(description),
    doubleValue_(initValue)
{

}

Tag::Tag(QString subSystem, QString name, Tag::Type type, int initValue, const QString &description) :
    subSystem_(subSystem),
    name_(name),
    type_(type),
    description_(description),
    intValue_(initValue)
{

}

Tag::Tag(QString subSystem, QString name, Tag::Type type, bool initValue, const QString &description) :
    subSystem_(subSystem),
    name_(name),
    type_(type),
    description_(description),
    boolValue_(initValue)
{

}

Tag::Tag(QString subSystem, QString name, Tag::Type type, QString initValue, const QString &description) :
    subSystem_(subSystem),
    name_(name),
    type_(type),
    description_(description),
    stringValue_(initValue)
{

}

Tag::Tag(QString subSystem, QString name, Type type, QDateTime initValue, const QString &description) :
    subSystem_(subSystem),
    name_(name),
    type_(type),
    description_(description)
{
    setValue(initValue);
}

void Tag::setValue(double aValue, qint64 msSinceEpoc)
{
    if(qFuzzyCompare(aValue, doubleValue_))
        return;

    doubleValue_ = aValue;
    if(msSinceEpoc < 0)
        timeStamp_ = QDateTime::currentMSecsSinceEpoch();
    else
        timeStamp_ = msSinceEpoc;
    emit valueChanged(this);
}


void Tag::setValue(int aValue, qint64 msSinceEpoc)
{
    if(aValue == intValue_)
        return;

    intValue_ = aValue;
    if(msSinceEpoc < 0)
        timeStamp_ = QDateTime::currentMSecsSinceEpoch();
    else
        timeStamp_ = msSinceEpoc;
    emit valueChanged(this);
}


void Tag::setValue(bool aValue, qint64 msSinceEpoc)
{
    if(aValue == boolValue_)
        return;

    boolValue_ = aValue;
    if(msSinceEpoc < 0)
        timeStamp_ = QDateTime::currentMSecsSinceEpoch();
    else
        timeStamp_ = msSinceEpoc;
    emit valueChanged(this);
}


void Tag::setValue(QString aValue, qint64 msSinceEpoc)
{
    if(stringValue_ == aValue)
        return;

    if(msSinceEpoc < 0)
        timeStamp_ = QDateTime::currentMSecsSinceEpoch();
    else
        timeStamp_ = msSinceEpoc;
    stringValue_ = aValue;
    emit valueChanged(this);
}

void Tag::setValue(QDateTime aValue, qint64 msSinceEpoc)
{
    qint64 value = aValue.toMSecsSinceEpoch();
    if(value == timeValue_)
        return;
    if(msSinceEpoc < 0)
        timeStamp_ = QDateTime::currentMSecsSinceEpoch();
    else
        timeStamp_ = msSinceEpoc;

    timeValue_ = value;
    emit valueChanged(this);
}

QString Tag::getFullName() const
{
    return QString("%1.%2").arg(subSystem_).arg(name_);
}

QString Tag::getSubsystem() const
{
    return subSystem_;
}


QString Tag::getName() const
{
    return name_;
}

QString Tag::getTimeStamp() const
{
    return QDateTime::fromMSecsSinceEpoch(timeStamp_).toString(timeStampFormat_);
}

const QString &Tag::getDescription() const
{
    return description_;
}

const QString &Tag::getTimeStampFormat() const
{
    return timeStampFormat_;
}

qint64 Tag::getMsSinceEpoc() const
{
    return timeStamp_;
}


Tag::Type Tag::getType() const
{
    return type_;
}

QString Tag::getTypeStr() const
{
    switch (type_) {
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
    return doubleValue_;
}


int Tag::getIntValue() const
{
    return intValue_;
}


bool Tag::getBoolValue() const
{
    return boolValue_;
}


QString Tag::getStringValue() const
{
    return stringValue_;
}

QDateTime Tag::getTimeValue() const
{
    return QDateTime::fromMSecsSinceEpoch(timeValue_);
}

void Tag::writeToXml(QXmlStreamWriter &aStream)
{
    aStream.writeStartElement("tag");
    aStream.writeAttribute("subsystem", subSystem_);
    aStream.writeAttribute("name", name_);
    aStream.writeAttribute("timestamp", QString::number(timeStamp_));
    aStream.writeAttribute("type", getTypeStr());
    if(type_ == eDouble)
        aStream.writeAttribute("value", QString::number(doubleValue_));
    else if(type_ == eInt)
        aStream.writeAttribute("value", QString::number(intValue_));
    else if(type_ == eBool)
        aStream.writeAttribute("value", (boolValue_) ? "1" : "0");
    else if(type_ == eString)
        aStream.writeAttribute("value", stringValue_);
    else if(type_ == eTime)
        aStream.writeAttribute("value", QString::number(timeValue_));
    else
        Q_UNREACHABLE(); ///< unhandled tag type.
    aStream.writeAttribute("description", description_);

    aStream.writeEndElement();
}


Tag* Tag::createFromXml(const QXmlStreamReader &aReader)
{
    QString sub = aReader.attributes().value("subsystem").toString();
    QString name = aReader.attributes().value("name").toString();
    QString type = aReader.attributes().value("type").toString();
    QString val = aReader.attributes().value("value").toString();
    QString description = aReader.attributes().value("descripton").toString();

    Tag *tag = nullptr;
    if(type == "Double")
    {
        tag = new Tag(sub, name, eDouble, val.toDouble(), description);
    }
    else if(type == "Int")
    {
        tag = new Tag(sub, name, eInt, val.toInt(), description);
    }
    else if(type == "Bool")
    {
        tag = new Tag(sub,name, eBool, (val.toInt() == 1) ? true : false, description);
    }
    else if(type == "String")
    {
        tag = new Tag(sub, name, eString, val, description);
    }
    else if(typeFromString(type) == eTime)
    {
        tag = new Tag(sub, name, eTime, QDateTime::fromMSecsSinceEpoch(val.toLongLong()), description);
    }

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
    ba.append(name_.toLatin1());
    if(type_ == eDouble)
    {
        ba.append(":f");
        union U{
           float f;
           char byte[4];
        }u;
        u.f = (float)doubleValue_;
        ba.append(u.byte, 4);
    }
    else if(type_ == eInt)
    {
        ba.append(":i");
        union U{
            int i;
            char byte[4];
        }u;
        u.i = intValue_;
        ba.append(u.byte, 4);
    }
    else if(type_ == eBool)
    {
        ba.append(":b");
        ba.append(boolValue_ ? char(1) : char(0));
    }
    else
        Q_UNREACHABLE();

    return ba;
}

const QJsonObject &Tag::toJson()
{
    jsonObject_.insert("name", name_);
    jsonObject_.insert("subsystem", subSystem_);
    jsonObject_.insert("type", Tag::toString(type_));
    jsonObject_.insert("description", description_);

    switch (type_) {
        case eDouble:
            jsonObject_.insert("value", doubleValue_);
            break;
        case eInt:
            jsonObject_.insert("value", intValue_);
            break;
        case eBool:
            jsonObject_.insert("value", boolValue_);
            break;
        case eString:
            jsonObject_.insert("value", stringValue_);
            break;
        case eTime:
            jsonObject_.insert("value", timeValue_);
            break;
        default:
            jsonObject_.insert("value", QString());
            break;
    }

    return jsonObject_;
}
