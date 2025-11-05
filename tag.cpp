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
#include <QMetaType>
#include <QVariant>

Tag::Tag(QObject *parent) : QObject(parent)
{

}


Tag::Tag(QString aSubSystem, QString aName, TagType aType) :
    subSystem_(aSubSystem),
    name_(aName),
    type_(aType)
{

}

Tag::Tag(QString subSystem, QString name, TagType type, QVariant initValue, const QString &description) :
    subSystem_(subSystem),
    name_(name),
    type_(type),
    description_(description)
{
    setValue(initValue);
}

void Tag::setEnumValues(const EnumMap &map)
{
    if(type_ != TagType::eInt)
        return;
    for(auto &[key, value] : map)
    {
        enumValues_[key] = value;
    }
}

void Tag::setValue(QVariant value, qint64 msSinceEpoc)
{
    // validate value based on type
    if(type_ == TagType::eDouble && value.metaType().id() == QMetaType::Double)
    {
        try
        {
            double val = std::get<(int)TagType::eDouble>(value_);
            if(qFuzzyCompare(value.toDouble(), val))
                return;
        }
        catch(std::bad_variant_access const& ex)
        {

        }

        value_.emplace<(int)TagType::eDouble>(value.toDouble());
    }
    else if(type_ == TagType::eInt && value.metaType().id() == QMetaType::Int)
    {
        try
        {
            int val = std::get<(int)TagType::eInt>(value_);
            if(value == val)
                return;
        }
        catch(std::bad_variant_access const& ex)
        {

        }

        value_.emplace<(int)TagType::eInt>(value.toInt());
    }
    else if(type_ == TagType::eBool && value.metaType().id()  == QMetaType::Bool)
    {
        try
        {
            bool val = std::get<(int)TagType::eBool>(value_);
            if(value == val)
                return;
        }
        catch(std::bad_variant_access const& ex)
        {

        }

        value_.emplace<(int)TagType::eBool>(value.toBool());
    }
    else if(type_ == TagType::eString && value.metaType().id() == QMetaType::QString)
    {
        try
        {
            QString val = std::get<(int)TagType::eString>(value_);
            if(value == val)
                return;
        }
        catch(std::bad_variant_access const& ex)
        {

        }

        value_.emplace<(int)TagType::eString>(value.toString());
    }
    else if(type_ == TagType::eTime && value.metaType().id() == QMetaType::LongLong)
    {
        try
        {
            qint64 val = std::get<(int)TagType::eTime>(value_);
            if(value.toLongLong() == val)
                return;
        }
        catch(std::bad_variant_access const& ex)
        {

        }

        value_.emplace<(int)TagType::eTime>(value.toLongLong());
    }
    else if(type_ == TagType::eTime && value.metaType().id() == QMetaType::QDateTime)
    {
        value = value.toLongLong();
        try
        {
            qint64 val = std::get<(int)TagType::eTime>(value_);
            if(value.toLongLong() == val)
                return;
        }
        catch(std::bad_variant_access const& ex)
        {

        }

        value_.emplace<(int)TagType::eTime>(value.toLongLong());
    }
    else
    {
        qWarning() << "Set invalid value type to tag: " << getFullName() << " meta data: " << value.metaType().name();
        return;
    }

    if(msSinceEpoc < 0)
    {
        timeStamp_ = QDateTime::currentMSecsSinceEpoch();
        isUpdated_ = true;
    }
    else
        timeStamp_ = msSinceEpoc;
    emit valueChanged(this);
}

QString Tag::getFullName() const
{
    return QString("%1.%2").arg(subSystem_, name_);
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


TagType Tag::getType() const
{
    return type_;
}

QString Tag::getTypeStr() const
{
    switch (type_) {
    case TagType::eDouble:
        return "Double";
    case TagType::eInt:
        return "Int";
    case TagType::eBool:
        return "Bool";
    case TagType::eString:
        return "String";
    case TagType::eTime:
        return "Time";
    case TagType::eUnknown:
        return "Unknown";
    }

    Q_UNREACHABLE();
}


double Tag::getDoubleValue() const
{
    if(type_ == TagType::eDouble)
        return std::get<(int)TagType::eDouble>(value_);
    return 0.0;
}


int Tag::getIntValue() const
{
    if(type_ == TagType::eInt)
        return std::get<(int)TagType::eInt>(value_);
    return 0;
}


bool Tag::getBoolValue() const
{
    if(type_ == TagType::eBool)
        return std::get<(int)TagType::eBool>(value_);
    return false;
}


QString Tag::getStringValue() const
{
    if(type_ == TagType::eString)
        return std::get<(int)TagType::eString>(value_);
    return {};
}

QDateTime Tag::getTimeValue() const
{
    if(type_ == TagType::eTime)
        return QDateTime::fromMSecsSinceEpoch(std::get<(int)TagType::eTime>(value_));
    return QDateTime::fromMSecsSinceEpoch(0);
}

QString Tag::enumValue(int value) const
{
    if(enumValues_.contains(value))
        return enumValues_.at(value);
    return {};
}

TagType Tag::typeMatchTagSocket(const TagSocket *tagsocket)
{
    if(!tagsocket)
        return TagType::eUnknown;
    switch (tagsocket->getType()) {
    case TagSocket::eBool:
        return TagType::eBool;
    case TagSocket::eDouble:
        return TagType::eDouble;
    case TagSocket::eInt:
        return TagType::eInt;
    case TagSocket::eString:
        return TagType::eString;
    case TagSocket::eTime:
        return TagType::eTime;
    case TagSocket::eNone:
        return TagType::eUnknown;
    default:
        return TagType::eUnknown;
    }
    Q_UNREACHABLE();
}

TagType Tag::typeFromString(const QString &aTypeString)
{
    if(aTypeString.compare(QString("Int"), Qt::CaseInsensitive) == 0)
        return TagType::eInt;
    else if(aTypeString.compare(QString("Bool"), Qt::CaseInsensitive) == 0)
        return TagType::eBool;
    else if(aTypeString.compare(QString("Double"), Qt::CaseInsensitive) == 0)
        return TagType::eDouble;
    else if(aTypeString.compare(QString("String"), Qt::CaseInsensitive) == 0)
        return TagType::eString;
    else if(aTypeString.compare(QString("Time"), Qt::CaseInsensitive) == 0)
        return TagType::eTime;
    else
        return TagType::eUnknown;
}

QString Tag::toString(TagType aType)
{
    switch (aType) {
        case TagType::eInt:
            return "Int";
        case TagType::eBool:
            return "Bool";
        case TagType::eDouble:
            return "Double";
        case TagType::eString:
            return "String";
        case TagType::eTime:
            return "Time";
        case TagType::eUnknown:
            return "Unknown";
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
    if(type_ == TagType::eDouble)
    {
        ba.append(":f");
        union U{
           float f;
           char byte[4];
        }u;
        u.f = (float)std::get<(int)TagType::eDouble>(value_);
        ba.append(u.byte, 4);
    }
    else if(type_ == TagType::eInt)
    {
        ba.append(":i");
        union U{
            int i;
            char byte[4];
        }u;
        u.i = std::get<(int)TagType::eInt>(value_);
        ba.append(u.byte, 4);
    }
    else if(type_ == TagType::eBool)
    {
        ba.append(":b");
        ba.append(std::get<(int)TagType::eBool>(value_) ? char(1) : char(0));
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
        case TagType::eDouble:
            jsonObject_.insert("value", std::get<(int)TagType::eDouble>(value_));
            break;
        case TagType::eInt:
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
            jsonObject_.insert("value", std::get<(int)TagType::eInt>(value_));
            break;
        case TagType::eBool:
            jsonObject_.insert("value", std::get<(int)TagType::eBool>(value_));
            break;
        case TagType::eString:
            jsonObject_.insert("value", std::get<(int)TagType::eString>(value_));
            break;
        case TagType::eTime:
            jsonObject_.insert("value", std::get<(int)TagType::eTime>(value_));
            break;
        default:
            jsonObject_.insert("value", {});
            break;
    }

    return jsonObject_;
}
