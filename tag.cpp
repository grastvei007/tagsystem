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


Tag::Tag(QString aSubSystem, QString aName, TagType aType, bool isArray) :
    subSystem_(aSubSystem),
    name_(aName),
	type_(aType),
	isArray_(isArray)
{
	if(isArray_)
	{
		QList<QVariant> list;
		value_ = list;
	}
}

Tag::Tag(QString subSystem, QString name, TagType type, QVariant initValue, const QString &description, bool isArray) :
    subSystem_(subSystem),
    name_(name),
    type_(type),
    value_(initValue),
	description_(description),
	isArray_(isArray)
{
	if(isArray_)
	{
		QList<QVariant> list;
		value_ = list;
	}
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
        if(qFuzzyCompare(value.toDouble(), value_.toDouble()))
            return;
    }
    else if(type_ == TagType::eInt && value.metaType().id() == QMetaType::Int)
    {
        if(value == value_)
            return;
    }
    else if(type_ == TagType::eBool && value.metaType().id()  == QMetaType::Bool)
    {
        if(value == value_)
            return;
    }
    else if(type_ == TagType::eString && value.metaType().id() == QMetaType::QString)
    {
        if(value == value_)
            return;
    }
    else if(type_ == TagType::eTime && value.metaType().id() == QMetaType::LongLong)
    {
        if(value == value_)
            return;
    }
    else
    {
        qWarning() << "Set invalid value type to tag: " << getFullName();
        return;
    }

	if(!isArray_)
	{
		value_ = value;
	}
	else
	{
		value_.toList().push_back(value);
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

void Tag::insert(unsigned int pos, QVariant value)
{
	if(isArray_)
	{
		value_.toList().insert(pos, value);
	}
}

void Tag::push_back(QVariant value)
{
	if(isArray_)
	{
		value_.toList().push_back(value);
	}
}

void Tag::syncArray()
{
	timeStamp_ = QDateTime::currentMSecsSinceEpoch();
	isUpdated_ = true;

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
    if(value_.metaType().id() == QMetaType::Double)
        return value_.toDouble();
    return 0.0;
}


int Tag::getIntValue() const
{
    if(value_.metaType().id() == QMetaType::Int)
        return value_.toInt();
    return 0;
}


bool Tag::getBoolValue() const
{
    if(value_.metaType().id() == QMetaType::Bool)
        return value_.toBool();
    return false;
}


QString Tag::getStringValue() const
{
    if(value_.metaType().id() == QMetaType::QString)
        return value_.toString();
    return {};
}

QDateTime Tag::getTimeValue() const
{
    if(value_.metaType().id() == QMetaType::LongLong)
        return QDateTime::fromMSecsSinceEpoch(value_.toLongLong());
    return QDateTime::fromMSecsSinceEpoch(0);
}

QString Tag::enumValue(int value) const
{
    if(enumValues_.contains(value))
        return enumValues_.at(value);
    return {};
}

QString Tag::getValueAsString() const
{
    switch (type_)
    {
    case TagType::eInt:
        return QString::number(getIntValue());
    case TagType::eBool:
        return getBoolValue() ? "True" : "False";
    case TagType::eDouble:
        return QString::number(getDoubleValue());
    case TagType::eString:
        return getStringValue();
    case TagType::eTime:
        return getTimeValue().toString();
    default:
        break;
    }
    Q_UNREACHABLE();
}

unsigned int Tag::size() const
{
	if(isArray_)
	{
		return value_.toList().size();
	}
	return 0;
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
        u.f = (float)value_.toDouble();
        ba.append(u.byte, 4);
    }
    else if(type_ == TagType::eInt)
    {
        ba.append(":i");
        union U{
            int i;
            char byte[4];
        }u;
        u.i = value_.toInt();
        ba.append(u.byte, 4);
    }
    else if(type_ == TagType::eBool)
    {
        ba.append(":b");
        ba.append(value_.toBool() ? char(1) : char(0));
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
	jsonObject_.insert("isarray", isArray_);

    switch (type_) {
        case TagType::eDouble:
			if(isArray_)
			{
				QJsonArray array;
				for(const auto &val : value_.toList())
				{
					array.append(val.toDouble());
				}
				jsonObject_.insert("arrayvalues", array);
			}
			else
			{
				jsonObject_.insert("value", value_.toDouble());
			}
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
			if(isArray_)
			{
				QJsonArray array;
				for(const auto &val : value_.toList())
				{
					array.append(val.toInt());
				}
				jsonObject_.insert("arrayvalues", array);
			}
			else
			{
				jsonObject_.insert("value", value_.toInt());
			}
            break;
        case TagType::eBool:
			if(isArray_)
			{
				QJsonArray array;
				for(const auto &val : value_.toList())
				{
					array.append(val.toBool());
				}
				jsonObject_.insert("arrayvalues", array);
			}
			else
			{
				jsonObject_.insert("value", value_.toBool());
			}
            break;
        case TagType::eString:
			if(isArray_)
			{
				QJsonArray array;
				for(const auto &val : value_.toList())
				{
					array.append(val.toString());
				}
				jsonObject_.insert("arrayvalues", array);
			}
			else
			{
				jsonObject_.insert("value", value_.toString());
			}
            break;
        case TagType::eTime:
			if(isArray_)
			{
				QJsonArray array;
				for(const auto &val : value_.toList())
				{
					array.append(val.toLongLong());
				}
				jsonObject_.insert("arrayvalues", array);
			}
			else
			{
				jsonObject_.insert("value", value_.toLongLong());
			}
            break;
        default:
            jsonObject_.insert("value", {});
            break;
    }

    return jsonObject_;
}
