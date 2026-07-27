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
#include "tagvalue.h"

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
}

Tag::Tag(QString subSystem, QString name, TagType type, QVariant initValue, const QString &description, bool isArray) :
    subSystem_(subSystem),
    name_(name),
    type_(type),
	description_(description),
	isArray_(isArray)
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
	if(type_ == TagType::eDouble
		&& (value.metaType().id() == QMetaType::Double || value.metaType().id() == QMetaType::QVariantList))
    {
		if(tagValue_.empty())
		{
			if(isArray_)
			{
				auto list = value.toList();
				for(auto &v : list)
				{
					Value val;
					val.set<TagType::eDouble>(v.toDouble());
					tagValue_.push_back(val);
				}
			}
			else
			{
				Value val;
				val.set<TagType::eDouble>(value.toDouble());
				tagValue_.push_back(val);
			}
		}
		else
		{
			if(isArray_)
			{
				if(isEqual(value.toList()))
					return;

				tagValue_.clear();
				auto list = value.toList();
				for(auto &v : list)
				{
					Value val;
					val.set<TagType::eDouble>(v.toDouble());
					tagValue_.push_back(val);
				}
			}
			else
			{
				if(tagValue_[0] == value.toDouble())
					return;

				tagValue_[0].set<TagType::eDouble>(value.toDouble());
			}
		}
    }
	else if(type_ == TagType::eInt
			 && (value.metaType().id() == QMetaType::Int || value.metaType().id() == QMetaType::QVariantList))
    {
		if(tagValue_.empty())
		{
			if(isArray_)
			{
				const auto list = value.toList();
				for(const auto &v : list)
				{
					tagValue_.push_back(Value());
					tagValue_.back().set<TagType::eInt>(v.toInt());
				}
			}
			else
			{
				tagValue_.push_back(Value());
				tagValue_.front().set<TagType::eInt>(value.toInt());
			}
		}
		else
		{
			if(isArray_)
			{
				const auto list = value.toList();

				if(isEqual(list))
					return;

				tagValue_.clear();
				for(const auto &v : list)
				{
					tagValue_.push_back(Value());
					tagValue_.back().set<TagType::eInt>(v.toInt());
				}
			}
			else
			{
				int newValue = value.toInt();
				if(tagValue_[0] == newValue)
					return;
				else
				{
					tagValue_[0].set<TagType::eInt>(newValue);
				}
			}
		}
    }
	else if(type_ == TagType::eBool
		&& (value.metaType().id()  == QMetaType::Bool || value.metaType().id() == QMetaType::QVariantList))
    {
		if(tagValue_.empty())
		{
			if(isArray_)
			{
				const auto list = value.toList();
				for(const auto &v : list)
				{
					tagValue_.push_back(Value());
					tagValue_.back().set<TagType::eBool>(v.toBool());
				}
			}
			else
			{
				tagValue_.push_back(Value());
				tagValue_.front().set<TagType::eBool>(value.toBool());
			}
		}
		else
		{
			if(isArray_)
			{
				if(isEqual(value.toList()))
					return;

				tagValue_.clear();
				for(const auto &v : value.toList())
				{
					tagValue_.push_back(Value());
					tagValue_.back().set<TagType::eBool>(v.toBool());
				}
			}
			else
			{
				if(tagValue_[0] == value.toBool())
					return;

				tagValue_[0].set<TagType::eBool>(value.toBool());
			}
		}
    }
	else if(type_ == TagType::eString
		&& (value.metaType().id() == QMetaType::QString || value.metaType().id() == QMetaType::QVariantList))
    {
		if(tagValue_.empty())
		{
			if(isArray_)
			{
				const auto list = value.toList();
				for(const auto &v : list)
				{
					tagValue_.push_back(Value());
					tagValue_.back().set<TagType::eString>(v.toString());
				}
			}
			else
			{
				tagValue_.push_back(Value());
				tagValue_.front().set<TagType::eString>(value.toString());
			}
		}
		else
		{
			if(isArray_)
			{
				if(isEqual(value.toList()))
					return;

				tagValue_.clear();
				for(const auto &v : value.toList())
				{
					tagValue_.push_back(Value());
					tagValue_.back().set<TagType::eString>(v.toString());
				}
			}
			else
			{
				if(tagValue_[0] == value.toString())
					return;

				tagValue_[0].set<TagType::eString>(value.toString());
			}
		}
    }
	else if(type_ == TagType::eTime
		&& (value.metaType().id() == QMetaType::LongLong || value.metaType().id() == QMetaType::QDateTime  || value.metaType().id() == QMetaType::QVariantList))
    {
		if(tagValue_.empty())
		{
			if(isArray_)
			{
				const auto list = value.toList();
				for(const auto &v : list)
				{
					tagValue_.push_back(Value());
					tagValue_.back().set<TagType::eTime>(v.toLongLong());
				}
			}
			else
			{
				tagValue_.push_back(Value());
				tagValue_.front().set<TagType::eTime>(value.toLongLong());
			}
		}
		else
		{
			if(isArray_)
			{
				if(isEqual(value.toList()))
					return;

				tagValue_.clear();
				for(const auto &v : value.toList())
				{
					tagValue_.push_back(Value());
					tagValue_.back().set<TagType::eTime>(v.toLongLong());
				}
			}
			else
			{
				if(tagValue_[0] == value.toLongLong())
					return;

				tagValue_[0].set<TagType::eTime>(value.toLongLong());
			}
		}
    }
    else
    {
        qWarning() << "Set invalid value type to tag: " << getFullName();
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

void Tag::insert(unsigned int pos, QVariant value)
{
	if(!isArray_)
	{
		return;
	}
	switch (type_) {
		case TagType::eInt:
			{
				Value newValue;
				newValue.set<TagType::eInt>(value.toInt());
				tagValue_.insert(tagValue_.begin() + pos, newValue);
				break;
			}
		case TagType::eDouble:
			{
				Value newValue;
				newValue.set<TagType::eDouble>(value.toDouble());
				tagValue_.insert(tagValue_.begin() + pos, newValue);
				break;
			}
		case TagType::eBool:
			{
				Value newValue;
				newValue.set<TagType::eBool>(value.toBool());
				tagValue_.insert(tagValue_.begin() + pos, newValue);
				break;
			}
		case TagType::eString:
			{
				Value newValue;
				newValue.set<TagType::eString>(value.toString());
				tagValue_.insert(tagValue_.begin() + pos, newValue);
				break;
			}
		case TagType::eTime:
			{
				if (value.metaType().id() == QMetaType::LongLong) {
					Value newValue;
					newValue.set<TagType::eTime>(value.toLongLong());
					tagValue_.insert(tagValue_.begin() + pos, newValue);
				}
				else if (value.metaType().id() == QMetaType::QDateTime) {
					QDateTime newTime = value.toDateTime();
					Value newValue;
					newValue.set<TagType::eTime>(newTime.toMSecsSinceEpoch());
					tagValue_.insert(tagValue_.begin() + pos, newValue);
				}
				break;
			}
		default:
			qWarning() << "Insert invalid value type to tag: " << getFullName();
			return;
	}
}



void Tag::push_back(QVariant value)
{
	if(!isArray_)
	{
		return;
	}
	switch (type_) {
	case TagType::eInt:
		tagValue_.push_back(Value());
		tagValue_.back().set<TagType::eInt>(value.toInt());
		break;
	case TagType::eDouble:
		tagValue_.push_back(Value());
		tagValue_.back().set<TagType::eDouble>(value.toDouble());
		break;
	case TagType::eBool:
		tagValue_.push_back(Value());
		tagValue_.back().set<TagType::eBool>(value.toBool());
		break;
	case TagType::eString:
		tagValue_.push_back(Value());
		tagValue_.back().set<TagType::eString>(value.toString());
		break;
	case TagType::eTime:
		if (value.metaType().id() == QMetaType::LongLong) {
			qint64 newValue = value.toLongLong();
			tagValue_.push_back(Value());
			tagValue_.back().set<TagType::eTime>(newValue);
		} else if (value.metaType().id() == QMetaType::QDateTime) {
			QDateTime newTime = value.toDateTime();
			if (!newTime.isValid()) {
				qWarning() << "Invalid time format for tag: " << getFullName();
				return;
			}
			tagValue_.push_back(Value());
			tagValue_.back().set<TagType::eTime>(newTime.toMSecsSinceEpoch());
		} else {
			qWarning() << "Push back invalid value type to tag: " << getFullName();
			return;
		}
		break;
	default:
		qWarning() << "Push back invalid value type to tag: " << getFullName();
		return;
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


bool Tag::isArray() const
{
	return isArray_;
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
	if(tagValue_.empty())
		return 0.0;

	if(!tagValue_[0].is<TagType::eDouble>())
		return 0.0;

	return tagValue_[0].get<TagType::eDouble>();
}


int Tag::getIntValue() const
{
	if(tagValue_.empty())
		return 0;

	if(!tagValue_[0].is<TagType::eInt>())
		return 0;

	return tagValue_[0].get<TagType::eInt>();
}


bool Tag::getBoolValue() const
{
	if(tagValue_.empty())
		return false;

	if(!tagValue_[0].is<TagType::eBool>())
		return false;

	return tagValue_[0].get<TagType::eBool>();
}


QString Tag::getStringValue() const
{
	if(tagValue_.empty())
		return {};

	if(!tagValue_[0].is<TagType::eString>())
		return {};

	return tagValue_[0].get<TagType::eString>();
}

QDateTime Tag::getTimeValue() const
{
	if(tagValue_.empty())
		return QDateTime::fromMSecsSinceEpoch(0);

	if(!tagValue_[0].is<TagType::eTime>())
		return QDateTime::fromMSecsSinceEpoch(0);

	return QDateTime::fromMSecsSinceEpoch(tagValue_[0].get<TagType::eTime>());
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
		return tagValue_.size();
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
		u.f = (float)tagValue_[0].get<TagType::eDouble>();
        ba.append(u.byte, 4);
    }
    else if(type_ == TagType::eInt)
    {
        ba.append(":i");
        union U{
            int i;
            char byte[4];
        }u;
		u.i = tagValue_[0].get<TagType::eInt>();
        ba.append(u.byte, 4);
    }
    else if(type_ == TagType::eBool)
    {
        ba.append(":b");
		ba.append(tagValue_[0].get<TagType::eBool>() ? char(1) : char(0));
    }
    else
        Q_UNREACHABLE();

    return ba;
}

const QJsonObject &Tag::toJson()
{
	QJsonArray tagValueArray;


	auto crateJsonArray = [&tagValueArray](const auto &tagValueList){
		for(const Value &val : tagValueList)
		{
			if(val.is<TagType::eDouble>())
			{
				tagValueArray.append(val.get<TagType::eDouble>());
			}
			else if(val.is<TagType::eInt>())
			{
				tagValueArray.append(val.get<TagType::eInt>());
			}
			else if(val.is<TagType::eBool>())
			{
				tagValueArray.append(val.get<TagType::eBool>() ? 1 : 0);
			}
			else if(val.is<TagType::eString>())
			{
				tagValueArray.append(QString(val.get<TagType::eString>()));
			}
			else if(val.is<TagType::eTime>())
			{
				tagValueArray.append(val.get<TagType::eTime>());
			}
		}
	};

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
				crateJsonArray(tagValue_);
				jsonObject_.insert("arrayvalues", tagValueArray);
			}
			else
			{
				jsonObject_.insert("value", getDoubleValue());
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
				crateJsonArray(tagValue_);
				jsonObject_.insert("arrayvalues", tagValueArray);
			}
			else
			{
				jsonObject_.insert("value", getIntValue());
			}
            break;
        case TagType::eBool:
			if(isArray_)
			{
				crateJsonArray(tagValue_);
				jsonObject_.insert("arrayvalues", tagValueArray);
			}
			else
			{
				jsonObject_.insert("value", getBoolValue());
			}
            break;
        case TagType::eString:
			if(isArray_)
			{
				crateJsonArray(tagValue_);
				jsonObject_.insert("arrayvalues", tagValueArray);
			}
			else
			{
				jsonObject_.insert("value", getStringValue());
			}
            break;
        case TagType::eTime:
			if(isArray_)
			{
				crateJsonArray(tagValue_);
				jsonObject_.insert("arrayvalues", tagValueArray);
			}
			else
			{
				jsonObject_.insert("value", getTimeValue().toMSecsSinceEpoch());
			}
            break;
        default:
            jsonObject_.insert("value", {});
            break;
    }

    return jsonObject_;
}

bool Tag::isEqual(const QVariantList &list) const
{
	if(tagValue_.size() != list.size())
		return false;

	unsigned int i = 0;
	for(const auto &value : list)
	{
		if(type_ == TagType::eInt)
		{
			if(tagValue_[i++] != value.toInt())
				return false;
		}
		else if(type_ == TagType::eDouble)
		{
			if((tagValue_[i++] != value.toDouble()))
				return false;
		}
		else if(type_ == TagType::eBool)
		{
			if(tagValue_[i++] != value.toBool())
				return false;
		}
		else if(type_ == TagType::eString)
		{
			if(tagValue_[i++] != value.toString())
				return false;
		}
		else if(type_ == TagType::eTime)
		{
			if(tagValue_[i++] != value.toLongLong())
				return false;
		}
	}


	return true;
}
