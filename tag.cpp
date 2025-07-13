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
#include "tagsocket.h"

#include "util/json.h"

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
    setValue(initValue);
}

Tag::Tag(QString subSystem, QString name, Tag::Type type, int initValue, const QString &description) :
    subSystem_(subSystem),
    name_(name),
    type_(type),
    description_(description),
    intValue_(initValue)
{
    setValue(initValue);
}

Tag::Tag(QString subSystem, QString name, Tag::Type type, bool initValue, const QString &description) :
    subSystem_(subSystem),
    name_(name),
    type_(type),
    description_(description),
    boolValue_(initValue)
{
    setValue(initValue);
}

Tag::Tag(QString subSystem, QString name, Tag::Type type, QString initValue, const QString &description) :
    subSystem_(subSystem),
    name_(name),
    type_(type),
    description_(description),
    stringValue_(initValue)
{
    setValue(initValue);
}

Tag::Tag(QString subSystem, QString name, Type type, QDateTime initValue, const QString &description) :
    subSystem_(subSystem),
    name_(name),
    type_(type),
    description_(description)
{
    setValue(initValue);
}

void Tag::setEnumValues(const EnumMap &map)
{
    if(type_ != eInt)
        return;
    for(auto &[key, value] : map)
    {
        enumValues_[key] = value;
    }
}

void Tag::setValue(double aValue, qint64 msSinceEpoc)
{
    if(qFuzzyCompare(aValue, doubleValue_))
        return;

    doubleValue_ = aValue;
    if(msSinceEpoc < 0)
    {
        timeStamp_ = QDateTime::currentMSecsSinceEpoch();
        isUpdated_ = true;
    }
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
    {
        timeStamp_ = QDateTime::currentMSecsSinceEpoch();
        isUpdated_ = true;
    }
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
    {
        timeStamp_ = QDateTime::currentMSecsSinceEpoch();
        isUpdated_ = true;
    }
    else
        timeStamp_ = msSinceEpoc;
    emit valueChanged(this);
}


void Tag::setValue(QString aValue, qint64 msSinceEpoc)
{
    if(stringValue_ == aValue)
        return;

    if(msSinceEpoc < 0)
    {
        timeStamp_ = QDateTime::currentMSecsSinceEpoch();
        isUpdated_ = true;
    }
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
    {
        timeStamp_ = QDateTime::currentMSecsSinceEpoch();
        isUpdated_ = true;
    }
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
    case eUnknown:
        return "Unknown";
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

QString Tag::enumValue(int value) const
{
    if(enumValues_.contains(value))
        return enumValues_.at(value);
    return {};
}

Tag::Type Tag::typeMatchTagSocket(const TagSocket *tagsocket)
{
    if(!tagsocket)
        return eUnknown;
    switch (tagsocket->getType()) {
    case TagSocket::eBool:
        return eBool;
    case TagSocket::eDouble:
        return eDouble;
    case TagSocket::eInt:
        return eInt;
    case TagSocket::eString:
        return eString;
    case TagSocket::eTime:
        return eTime;
    case TagSocket::eNone:
        return eUnknown;
    default:
        return eUnknown;
    }
    Q_UNREACHABLE();
}

Tag::Type Tag::typeFromString(const QString &aTypeString)
{
    if(aTypeString.compare(QString("Int"), Qt::CaseInsensitive) == 0)
        return eInt;
    else if(aTypeString.compare(QString("Bool"), Qt::CaseInsensitive) == 0)
        return eBool;
    else if(aTypeString.compare(QString("Double"), Qt::CaseInsensitive) == 0)
        return eDouble;
    else if(aTypeString.compare(QString("String"), Qt::CaseInsensitive) == 0)
        return eString;
    else if(aTypeString.compare(QString("Time"), Qt::CaseInsensitive) == 0)
        return eTime;
    else
        return eUnknown;
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
    jsonObject_.insert("timestamp", timeStamp_);

    switch (type_) {
        case eDouble:
            jsonObject_.insert("value", doubleValue_);
            break;
        case eInt:
            if(!enumValues_.empty())
            {
                auto transform = [](const auto& value)
                {
                    //auto &[key, value] = static_cast<stvalue;
                    QJsonObject obj;
                    obj.insert("key", value.first);
                    obj.insert("value", value.second);
                    return QJsonValue(obj);
                };

                jsonObject_.insert("enumvalues", util::json::toJsonArray(enumValues_, transform));
            }
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
