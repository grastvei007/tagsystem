#ifndef TAGVALUE_H
#define TAGVALUE_H

#include <QString>
#include <variant>

enum class TagType{
	eDouble = 0,
	eInt,
	eBool,
	eString,
	eTime,
	eUnknown
};

template<TagType V>
class TagValue;

template<>
class TagValue<TagType::eDouble>
{
  public:
	using tagType = double;
};

template<>
class TagValue<TagType::eInt>
{
  public:
	using tagType = int;
};

template<>
class TagValue<TagType::eBool>
{
  public:
	using tagType = bool;
};

template<>
class TagValue<TagType::eString>
{
  public:
	using tagType = QString;
};

template<>
class TagValue<TagType::eTime>
{
  public:
	using tagType = qint64;
};

template<TagType... Vs>
using TagValueVariant = std::variant<typename TagValue<Vs>::tagType ...>;

using tagValue_t = TagValueVariant<TagType::eDouble, TagType::eInt, TagType::eBool, TagType::eString, TagType::eTime>;

class Value{
public:
	Value() : value_(0) {}

	template<TagType V>
	Value(typename TagValue<V>::tagType value){
		value_ = std::move(value);
	}

	template<TagType V>
	typename TagValue<V>::tagType get() const
	{
		return std::get<typename TagValue<V>::tagType>(value_);
	}

	template<TagType V>
	bool is() const
	{
		return value_.index() == static_cast<int>(V);
	}

	template<TagType V>
	bool set(typename TagValue<V>::tagType value)
	{
		value_ = value;

		return false;
	}

	bool operator==(const int other) const
	{
		if(const int *v = std::get_if<int>(&value_))
		{
			if(*v == other)
				return true;
		}

		return false;
	}

	bool operator==(const double other) const
	{
		if(const double *v = std::get_if<double>(&value_))
		{
			if(qFuzzyCompare(*v, other))
				return true;
		}

		return false;
	}

	bool operator==(const bool other) const
	{
		if(const bool *v = std::get_if<bool>(&value_))
		{
			if(*v == other)
				return true;
		}

		return false;
	}

	bool operator==(const QString &other) const
	{
		if(const QString *v = std::get_if<QString>(&value_))
		{
			if(*v == other)
				return true;
		}

		return false;
	}

	bool operator==(const qint64 &other) const
	{
		if(const qint64 *v = std::get_if<qint64>(&value_))
		{
			if(*v == other)
				return true;
		}

		return false;
	}

private:
	tagValue_t value_;
};


#endif // TAGVALUE_H
